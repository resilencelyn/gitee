#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include "rm48x_spi.h"

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

static void assert_cs_line(conf_object_t *opaque)
{
	rm48x_spi_device *dev = opaque->obj;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	slave_obj = dev->slaves[dev->index].slave_obj;
	spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

	if (spi_dev_iface && slave_obj)
		spi_dev_iface->spi_assert_cs_line(slave_obj);
}

static void deassert_cs_line(conf_object_t *opaque)
{
	rm48x_spi_device *dev = opaque->obj;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	slave_obj = dev->slaves[dev->index].slave_obj;
	spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

	if (spi_dev_iface && slave_obj)
		spi_dev_iface->spi_deassert_cs_line(slave_obj);
}

static uint32_t get_format_reg(conf_object_t *opaque)
{
	rm48x_spi_device *dev = opaque->obj;

	/* slave mode only support spifmt0 */
	if ((dev->regs.spigcr1 & 0x3) == 0x0)
		return dev->regs.spifmt0;

	switch ((dev->regs.spidat1 >> 24) & 0x3) {
		case 0: return dev->regs.spifmt0;
		case 1: return dev->regs.spifmt1;
		case 2: return dev->regs.spifmt2;
		case 3: return dev->regs.spifmt3;
	}
	return 0;
}

static int get_data_length(uint32_t spifmt)
{
	int len;

	len = spifmt & 0x1f;
	len = len < 2 ? 8 : len > 16 ? 16 : len;
	return len;
}

static int send_data_to_spi_device(conf_object_t *opaque, uint32_t spidat)
{
	rm48x_spi_device *dev = opaque->obj;
	uint32_t spifmt, data;
	int len;
	char *p = (char *)&spidat;
	conf_object_t *slave_obj = dev->slaves[dev->index].slave_obj;
	spi_device_intf *spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

	spifmt = get_format_reg(opaque);
	len = get_data_length(spifmt);

	if (slave_obj && spi_dev_iface) {
		spi_dev_iface->spi_device_receive(slave_obj, p, len / 8);
	}
	return 0;
}

static void check_cs_line(conf_object_t *opaque)
{
	rm48x_spi_device *dev = opaque->obj;
	conf_object_t *slave_obj;
	spi_device_intf *spi_dev_iface;

	/* if CSHOLD bit is zero, then deassert cs line */
	if ((dev->regs.spidat1 & (1 << 28)) == 0) {
		slave_obj = dev->slaves[dev->index].slave_obj;
		spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

		if (spi_dev_iface && slave_obj)
			spi_dev_iface->spi_deassert_cs_line(slave_obj);
	}
}

static int start_transmit(conf_object_t *opaque, uint32_t spidat)
{
	rm48x_spi_device *dev = opaque->obj;
	RW_WRLOCK(dev->lock);
	dev->regs.spiflg &= ~(1 << 9);
	dev->regs.intvect1 = 0;
	RW_UNLOCK(dev->lock);
	
	if ((dev->regs.spigcr1 & 0x3) == 0x3) {
		/* master mode, spidata & 0xff is real data, begin
		 * transmit here we should get data length and dir
		 */
		/* enable cs line */
		assert_cs_line(opaque);
		/* send data to spi device */
		send_data_to_spi_device(opaque, spidat);
		/* after transmit data, we diasble cs line by cshold */
		check_cs_line(opaque);
		if (dev->regs.spiint0 & (1 << 9)) {
			/* raise signal */
			RW_WRLOCK(dev->lock);
			dev->regs.spiflg |= 1 << 9;
			dev->regs.intvect1 = 0x14 << 1;
			RW_UNLOCK(dev->lock);

			if (dev->signal_iface != NULL) {
				dev->signal_iface->signal(dev->signal, dev->interrupt_number);
			}
		}
	} else {
		/* FIXME:slave mode */
		/* spidata & 0xff is real data, begin transmit */
		if (dev->regs.spiint0 & (1 << 9)) {
			//dbg_spi_data(opaque, spidat);
			/* raise signal */
			RW_WRLOCK(dev->lock);
			dev->regs.spiflg |= 1 << 9;
			dev->regs.intvect1 = 0x14 << 1;
			RW_UNLOCK(dev->lock);

			if (dev->signal_iface != NULL) {
				dev->signal_iface->signal(dev->signal, dev->interrupt_number);
			}
		}
	}
}

static int notify_transmit_done(conf_object_t *opaque)
{
	rm48x_spi_device *dev = opaque->obj;
	conf_object_t *slave_obj = dev->slaves[dev->index].slave_obj;
	spi_device_intf *spi_dev_iface = dev->slaves[dev->index].spi_dev_iface;

	if (slave_obj && spi_dev_iface)
		spi_dev_iface->spi_device_receive_done(slave_obj);
	return 0;
}

static int get_slave_index(uint32_t spidef)
{
	int i;

	for (i = 0; i < 8; i++) {
		if (spidef & (1 << i))
			return i;
	}
}

static exception_t MibSPI_RAM_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_spi_device *dev = opaque->obj;
	
	return 0;
}

static exception_t MibSPI_RAM_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_spi_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	
	return 0;
}

static exception_t rm48x_spi_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_spi_device *dev = opaque->obj;
	static int offset2 = 0;
	switch (offset)
	{
		case SPIGCR0:
			*(uint32_t *)buf = dev->regs.spigcr0;
			break;
		case SPIGCR1:
			*(uint32_t *)buf = dev->regs.spigcr1;
			break;
		case SPIINT0:
			*(uint32_t *)buf = dev->regs.spiint0;
			break;
		case SPILVL:
			*(uint32_t *)buf = dev->regs.spilvl;
			break;
		case SPIFLG:
			*(uint32_t *)buf = dev->regs.spiflg;
			break;
		case SPIPC0:
			*(uint32_t *)buf = dev->regs.spipc0;
			break;
		case SPIPC1:
			*(uint32_t *)buf = dev->regs.spipc1;
			break;
		case SPIPC2:
			*(uint32_t *)buf = dev->regs.spipc2;
			break;
		case SPIPC3:
			*(uint32_t *)buf = dev->regs.spipc3;
			break;
		case SPIPC4:
			*(uint32_t *)buf = dev->regs.spipc4;
			break;
		case SPIPC5:
			*(uint32_t *)buf = dev->regs.spipc5;
			break;
		case SPIPC6:
			*(uint32_t *)buf = dev->regs.spipc6;
			break;
		case SPIPC7:
			*(uint32_t *)buf = dev->regs.spipc7;
			break;
		case SPIPC8:
			*(uint32_t *)buf = dev->regs.spipc8;
			break;
		case SPIDAT0:
			*(uint32_t *)buf = dev->regs.spidat0;
			break;
		case SPIDAT1:
			*(uint32_t *)buf = dev->regs.spidat1;
			break;
		case SPIBUF:
			//int rx_data = dev->regs.spibuf & 0xffff;

			if ((dev->regs.mibspie) & 0x1 == 1)
			{
				dev->regs.spibuf = dev->MibSPI_RAM.TxRAM[offset2];
				offset2 = offset2 + 4;
				if (offset2 >= 0x80)
					offset2 = 0;
				*(uint32_t *)buf = dev->regs.spibuf;
				dev->regs.spiflg &= ~(1 << 8);
			
			}else {
				if (CheckCanReadNum(dev->spi_fifo) > 0) {
				char ch;

				ReadFIFO(dev->spi_fifo, &ch, 1);

				dev->regs.spibuf &= ~0xffff;
				dev->regs.spibuf |= 0xff & ch;

				*(uint32_t *)buf = dev->regs.spibuf;
				if (CheckCanReadNum(dev->spi_fifo) <= 0)
					dev->regs.spiflg &= ~(1 << 8);
				} else {
					*(uint32_t *)buf = dev->regs.spibuf;
					dev->regs.spiflg &= ~(1 << 8);
				}
				
			}
			
			break;
		case SPIEMU:
			*(uint32_t *)buf = dev->regs.spiemu;
			break;
		case SPIDELAY:
			*(uint32_t *)buf = dev->regs.spidelay;
			break;
		case SPIDEF:
			*(uint32_t *)buf = dev->regs.spidef;
			break;
		case SPIFMT0:
			*(uint32_t *)buf = dev->regs.spifmt0;
			break;
		case SPIFMT1:
			*(uint32_t *)buf = dev->regs.spifmt1;
			break;
		case SPIFMT2:
			*(uint32_t *)buf = dev->regs.spifmt2;
			break;
		case SPIFMT3:
			*(uint32_t *)buf = dev->regs.spifmt3;
			break;
		case INTVECT0:
			*(uint32_t *)buf = dev->regs.intvect0;
			break;
		case INTVECT1:
			*(uint32_t *)buf = dev->regs.intvect1;
			if (((dev->regs.intvect1 >> 1) & 0x1f) == 0x12)
			{
				dev->regs.spiflg &= ~(1 << 8);
				dev->regs.spiflg &= ~(1 << 6);
			}
			break;
		case SPIPC9:
			*(uint32_t *)buf = dev->regs.spipc9;
			break;
		case SPIPMCTRL:
			*(uint32_t *)buf = dev->regs.spipmctrl;
			break;
		case MIBSPIE:
			*(uint32_t *)buf = dev->regs.mibspie;
			break;
		case TGITENST:
			*(uint32_t *)buf = dev->regs.tgitenst;
			break;
		case TGITENCR:
			*(uint32_t *)buf = dev->regs.tgitencr;
			break;
		case TGITLVST:
			*(uint32_t *)buf = dev->regs.tgitlvst;
			break;
		case TGITLVCR:
			*(uint32_t *)buf = dev->regs.tgitlvcr;
			break;
		case TGINTFLG:
			*(uint32_t *)buf = dev->regs.tgintflg;
			break;
		case TICKCNT:
			*(uint32_t *)buf = dev->regs.tickcnt;
			break;
		case LTGPEND:
			*(uint32_t *)buf = dev->regs.ltgpend;
			break;
		case TGXCTRL:
			*(uint32_t *)buf = dev->regs.tgxctrl;
			break;
		case DMAXCTRL:
			*(uint32_t *)buf = dev->regs.dmaxctrl;
			break;
		case ICOUNT:
			*(uint32_t *)buf = dev->regs.icount;
			break;
		case DMACNTLEN:
			*(uint32_t *)buf = dev->regs.dmacntlen;
			break;
		case UERRCTRL:
			*(uint32_t *)buf = dev->regs.uerrctrl;
			break;
		case UERRSTAT:
			*(uint32_t *)buf = dev->regs.uerrstat;
			break;
		case UERRADDR1:
			*(uint32_t *)buf = dev->regs.uerraddr1;
			break;
		case UERRADDR0:
			*(uint32_t *)buf = dev->regs.uerraddr0;
			break;
		case RXOVRN_BUF_ADDR:
			*(uint32_t *)buf = dev->regs.rxovrn_buf_addr;
			break;
		case IOLPBKTSTCR:
			*(uint32_t *)buf = dev->regs.iolpbktstcr;
			break;
		case EXTENDED_PRESCALE1:
			*(uint32_t *)buf = dev->regs.extended_prescale1;
			break;
		case EXTENDED_PRESCALE2:
			*(uint32_t *)buf = dev->regs.extended_prescale2;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rm48x_spi\n", offset);
			break;
	}
	//printf(" %s\t offset = 0x%x\t value = 0x%x\n",__FUNCTION__,offset,*(uint32_t*)buf);
	return 0;
}

static exception_t rm48x_spi_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	rm48x_spi_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	uint32_t tmp_spiint0,tmp_spidat1;
	static int offset1 = 0;
	//printf(" %s\t offset = 0x%x\t value = 0x%x\n",__FUNCTION__,offset,val);
	/* support byte access for register */
	if (offset >SPIDAT1 && offset < SPIBUF && count == 1) {
		char *pspidata1 = (char *)&dev->regs.spidat1;
		uint32_t off = offset - SPIDAT1;

		tmp_spidat1 = dev->regs.spidat1;
		*(pspidata1 + off) = val;

		/* deassert cs line, stop translate and receive */
		if ((tmp_spidat1 & (1 << 28)) && ((dev->regs.spidat1 & (1 << 28)) == 0)) {
			deassert_cs_line(opaque);
		}
		return No_exp;
	}
	
	switch (offset)
	{
		case SPIGCR0:
			dev->regs.spigcr0 = val;
			if ((dev->regs.spigcr0 & 0x1) == 1)
			{
				memset(&dev->regs, 0, sizeof(dev->regs));
				dev->regs.spidef = 0xff;
				dev->regs.spiemu = 0x80000000;
				dev->regs.uerrctrl = 0x5;
				dev->regs.uerraddr1 = 0x200;
				dev->regs.rxovrn_buf_addr = 0x200;
				dev->index = 0;
			}
			break;
		case SPIGCR1:
			dev->regs.spigcr1 = val;
			if ((dev->regs.spigcr1 & 0x1000000))
			{
				dev->regs.spidat0 = 0;
				dev->regs.spidat1 &= ~0xff;
				dev->regs.spiflg &= ~(1 << 9);
				dev->regs.spiflg &= ~(1 << 8);
				dev->regs.spiflg &= ~(1 << 6);
				dev->regs.spiflg &= ~(1 << 4);
				dev->regs.spiflg &= ~(1 << 3);
				dev->regs.spiflg &= ~(1 << 2);
				dev->regs.spiflg &= ~(1 << 1);
				dev->regs.spiflg &= ~(1 << 0);
				dev->regs.intvect0 = 0;
				dev->regs.intvect1 = 0;
			}
			if (val == ((dev->regs.spigcr1 & 0xFEFFFFFF) | 0x01000000) && dev->t_flag == 1){
				uint16_t RX_Data[16]  = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2};
				int i;
				for(i = 0; i <16; i++){
					WriteFIFO(dev->spi_fifo, &RX_Data[i], 1);
					
				}
				SET_BIT(dev->regs.spiflg, 0x200);
			}
			break;
		case SPIINT0:
			tmp_spiint0 = dev->regs.spiint0;
			dev->regs.spiint0 = val;
			/* because begin, we should raise signal to active transmit */
			if ((dev->regs.spiint0 & (1 << 9)) && (tmp_spiint0 & (1 << 9)) == 0) {
				RW_WRLOCK(dev->lock);
				dev->regs.spiflg |= 1 << 9;
				dev->regs.intvect1 = 0x14 << 1;
				RW_UNLOCK(dev->lock);
				if (dev->signal_iface != NULL) {
					dev->signal_iface->signal(dev->signal, dev->interrupt_number);
				}
			}
			/* DMA active transmit */
			if ((dev->regs.spiint0 & (1 << 16)) && (tmp_spiint0 & (1 << 16)) == 0) {
				RW_WRLOCK(dev->lock);
				dev->regs.spiflg |= 1 << 9;
				dev->regs.intvect1 = 0x14 << 1;
				RW_UNLOCK(dev->lock);

				if (dev->signal_iface != NULL) {
					dev->signal_iface->signal(dev->signal, dev->interrupt_number);
				}
			}
			/* transmit done */
			if (((dev->regs.spiint0 & (1 << 9)) == 0) && (tmp_spiint0 & (1 << 9))) {
				RW_WRLOCK(dev->lock);
				dev->regs.intvect1 = 0;
				dev->regs.spiflg &= ~(1 << 9);
				RW_UNLOCK(dev->lock);

				notify_transmit_done(opaque);
			}
			/*DMA Mode transmit done */
			if (((dev->regs.spiint0 & (1 << 16)) == 0) && (tmp_spiint0 & (1 << 16))) {
				RW_WRLOCK(dev->lock);
				dev->regs.intvect1 = 0;
				dev->regs.spiflg &= ~(1 << 9);
				RW_UNLOCK(dev->lock);

				notify_transmit_done(opaque);
			}
			/* receive done */
			if (((dev->regs.spiint0 & (1 << 8)) == 0) && (tmp_spiint0 & (1 << 8))) {
				RW_WRLOCK(dev->lock);
				dev->regs.spiflg &= ~(1 << 8);
				dev->regs.intvect1 = 0;
				RW_UNLOCK(dev->lock);
			}
			break;
		case SPILVL:
			dev->regs.spilvl = val;
			break;
		case SPIFLG:
			dev->regs.spiflg &= ~val;
			dev->regs.intvect0 = 0;
			dev->regs.intvect1 = 0;
			break;
		case SPIPC0:
			dev->regs.spipc0 = val;
			break;
		case SPIPC1:
			dev->regs.spipc1 = val;
			break;
		case SPIPC2:
			dev->regs.spipc2 = val;
			break;
		case SPIPC3:
			dev->regs.spipc3 = val;
			break;
		case SPIPC4:
			dev->regs.spipc4 = val;
			break;
		case SPIPC5:
			dev->regs.spipc5 = val;
			break;
		case SPIPC6:
			dev->regs.spipc6 = val;
			break;
		case SPIPC7:
			dev->regs.spipc7 = val;
			break;
		case SPIPC8:
			dev->regs.spipc8 = val;
			break;
		case SPIDAT0:
			dev->regs.spidat0 = val;
			if (dev->regs.spigcr1 & (1 << 24))
			{
				/* enable, now we start transmit */
				start_transmit(opaque, dev->regs.spidat0);
			}
			break;
		case SPIDAT1:
			dev->regs.spidat1 = val;
			SET_BIT(dev->regs.spiflg, 0x100);
			if (dev->regs.spigcr1 & (1 << 24))
			{
				/* enable, now we start transmit */
				start_transmit(opaque, dev->regs.spidat1);
			}
			break;
		case SPIBUF:
			dev->regs.spibuf = val;
			
			break;
		case SPIEMU:
			dev->regs.spiemu = val;
			break;
		case SPIDELAY:
			dev->regs.spidelay = val;
			break;
		case SPIDEF:
			dev->regs.spidef = val;
			//dev->index = get_slave_index(0);
			break;
		case SPIFMT0:
			dev->regs.spifmt0 = val;
			break;
		case SPIFMT1:
			dev->regs.spifmt1 = val;
			break;
		case SPIFMT2:
			dev->regs.spifmt2 = val;
			break;
		case SPIFMT3:
			dev->regs.spifmt3 = val;
			break;
		case INTVECT0:
			dev->regs.intvect0 = val;
			break;
		case INTVECT1:
			dev->regs.intvect1 = val;
			break;
		case SPIPC9:
			dev->regs.spipc9 = val;
			break;
		case SPIPMCTRL:
			dev->regs.spipmctrl = val;
			break;
		case MIBSPIE:
			dev->regs.mibspie = val;
			break;
		case TGITENST:
			dev->regs.tgitenst = val;
			break;
		case TGITENCR:
			dev->regs.tgitencr = val;
			break;
		case TGITLVST:
			dev->regs.tgitlvst = val;
			break;
		case TGITLVCR:
			dev->regs.tgitlvcr = val;
			break;
		case TGINTFLG:
			dev->regs.tgintflg = val;
			break;
		case TICKCNT:
			dev->regs.tickcnt = val;
			break;
		case LTGPEND:
			dev->regs.ltgpend = val;
			break;
		case TGXCTRL:
			dev->regs.tgxctrl = val;
			break;
		case DMAXCTRL:
			dev->regs.dmaxctrl = val;
			break;
		case ICOUNT:
			dev->regs.icount = val;
			break;
		case DMACNTLEN:
			dev->regs.dmacntlen = val;
			break;
		case UERRCTRL:
			 dev->regs.uerrctrl = val;
			break;
		case UERRSTAT:
			dev->regs.uerrstat &= ~val;
			break;
		case UERRADDR1:
			dev->regs.uerraddr1 = val;
			break;
		case UERRADDR0:
			dev->regs.uerraddr0 = val;
			break;
		case RXOVRN_BUF_ADDR:
			dev->regs.rxovrn_buf_addr = val;
			break;
		case IOLPBKTSTCR:
			if (val & (1 << 24) == 1)
				dev->regs.iolpbktstcr &= ~(val & (1 << 24));
			else
				dev->regs.iolpbktstcr = val;
			break;
		case EXTENDED_PRESCALE1:
			dev->regs.extended_prescale1 = val;
			break;
		case EXTENDED_PRESCALE2:
			dev->regs.extended_prescale2 = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in rm48x_spi\n", offset);
			break;
	}
	return 0;
}

static char* rm48x_spi_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t rm48x_spi_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	rm48x_spi_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t rm48x_spi_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

static uint32_t rm48x_spi_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	rm48x_spi_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t rm48x_spi_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_rm48x_spi(const char *obj_name)
{
	rm48x_spi_device* dev = skyeye_mm_zero(sizeof(rm48x_spi_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	
	dev->regs.spidef = 0xff;
	dev->regs.spiemu = 0x80000000;
	dev->regs.uerrctrl = 0x5;
	dev->regs.uerraddr1 = 0x200;
	dev->regs.rxovrn_buf_addr = 0x200;
	dev->index = 0;
	dev->regs.spibuf = 0x80000000;
	int i;
	for (i = 0; i <= 0x80; i++){
		dev->MibSPI_RAM.RxRAM[i] = 0x80000000;
	}
	dev->spi_fifo = CreateFIFO(1024);
	RWLOCK_INIT(dev->lock);
	return dev->obj;
}

static exception_t free_rm48x_spi(conf_object_t* opaque)
{
	rm48x_spi_device* dev = opaque->obj;
	FreeFIFO(dev->spi_fifo);
	RWLOCK_DESTROY(dev->lock);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t config_rm48x_spi(conf_object_t* conf_obj)
{
	rm48x_spi_device *dev = conf_obj->obj;
	int i = 0;
	
	static memory_space_intf io_memory =
	{
		.read  = MibSPI_RAM_read,
		.write = MibSPI_RAM_write
	};
	if (dev->MibSPI_RAM_BASE == 0 )
		return No_exp;
	if (dev->image != NULL) {
	exception_t ret;
	ret = SKY_add_map(dev->image->obj, conf_obj, dev->MibSPI_RAM_BASE, 0x400, 0, &io_memory, 1, 1);
	}else {
		skyeye_log(Error_log, __FUNCTION__, "Cannot get dev->image in rm48x_spi\n");
	}
	
	return No_exp;
}

static exception_t core_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_spi_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (core_signal_intf *)SKY_get_iface(dev->signal, CORE_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", CORE_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", CORE_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t core_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_spi_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t spi_device_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_spi_device *dev = obj->obj;
	if (index >= MAX_SLAVES || index < 0)
		return No_exp;

	dev->slaves[index].spi_dev_iface = (spi_device_intf *)SKY_get_iface(obj2, SPI_DEVICE_INTF_NAME);
	dev->slaves[index].slave_obj = obj2;

	if (dev->slaves[index].spi_dev_iface == NULL){
		if (dev->slaves[index].slave_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SPI_DEVICE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SPI_DEVICE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t spi_device_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_spi_device *dev = obj->obj;
	*obj2 = dev->slaves[index].slave_obj;
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rm48x_spi_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rm48x_spi_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_ram_address(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rm48x_spi_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->MibSPI_RAM_BASE);
	return int_number;
}

static exception_t set_attr_ram_address(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rm48x_spi_device *dev = conf_obj->obj;
	dev->MibSPI_RAM_BASE = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_test_data(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	rm48x_spi_device *dev = conf_obj->obj;
	attr_value_t t_flag = SKY_make_attr_uinteger(dev->t_flag);
	return t_flag;
}

static exception_t set_attr_test_data(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	rm48x_spi_device *dev = conf_obj->obj;
	dev->t_flag = SKY_attr_uinteger(*value);
	return No_exp;
}

void rm48x_spi_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of systimer");
	SKY_register_attribute(clss, "ram_address", get_attr_ram_address, NULL, set_attr_ram_address, NULL, SKY_Attr_Optional, "uinteger", "get ram address for mibspi");
	SKY_register_attribute(clss, "test_data", get_attr_test_data, NULL, set_attr_test_data, NULL, SKY_Attr_Optional, "uinteger", "this is a flag to test");
}

static void spi_bus_receive(void *spi_bus, char *buf, int len)
{
	rm48x_spi_device *dev = ((conf_object_t *)spi_bus)->obj;
	int i = 0;
	if (len > 0) {
		
		for (i = 0; i < len; i++) {
			if ((dev->regs.mibspie) & 0x1 == 1){
				dev->MibSPI_RAM.RxRAM[i] = buf[i];
				dev->MibSPI_RAM.TxRAM[i] = buf[i];
				if (len >= 0x80 && i >= 0x80){
					len = len - 0x80;
					i = 0;
				}
			}
			else
				WriteFIFO(dev->spi_fifo, &buf[i], 1);
			if ((dev->regs.spiint0 & (1 << 8)) || (dev->regs.spiint0 & (1 << 16))) {
				RW_WRLOCK(dev->lock);
				dev->regs.spiflg |= 1 << 8;
				dev->regs.intvect1 = 0x12 << 1;
				RW_UNLOCK(dev->lock);

				if (dev->signal_iface != NULL) {
					dev->signal_iface->signal(dev->signal, dev->interrupt_number);
				}
			}
		}

		return;
	}
	if ((dev->regs.spiint0 & (1 << 8)) && (len == 0)) {
		RW_WRLOCK(dev->lock);
		dev->regs.spiflg |= 1 << 8;
		dev->regs.intvect1 = 0x12 << 1;
		RW_UNLOCK(dev->lock);

		if (dev->signal_iface != NULL) {
			dev->signal_iface->signal(dev->signal, dev->interrupt_number);
		}
	}
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	rm48x_spi_device *dev = obj->obj;
	dev->image = obj2;
	dev->image_space = (memory_space_intf *)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);

	if (dev->image_space == NULL){
		if (dev->image == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	rm48x_spi_device *dev = obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

void init_rm48x_spi(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "rm48x_spi",
		.class_desc = "rm48x_spi",
		.new_instance = new_rm48x_spi,
		.free_instance = free_rm48x_spi,
		.config_instance = config_rm48x_spi,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = rm48x_spi_read,
		.write = rm48x_spi_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = rm48x_spi_get_regvalue_by_id,
		.get_regname_by_id = rm48x_spi_get_regname_by_id,
		.set_regvalue_by_id = rm48x_spi_set_regvalue_by_id,
		.get_regnum = rm48x_spi_get_regnum,
		.get_regid_by_name = rm48x_spi_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static spi_bus_intf spi_bus_intf = {
		.spi_bus_receive = spi_bus_receive
	};
	SKY_register_iface(clss, SPI_BUS_INTF_NAME, &spi_bus_intf);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
		(struct InterfaceDescription)
		{
			.name = SPI_BUS_INTF_NAME,
			.iface = &spi_bus_intf
		},
	};
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription ){
			.name = CORE_SIGNAL_INTF_NAME,
			.set = core_signal_set,
			.get = core_signal_get,
		},
		(struct ConnectDescription) {
			.name = SPI_DEVICE_INTF_NAME,
			.set = spi_device_set,
			.get = spi_device_get,
		},
		(struct ConnectDescription ){
		.name = MEMORY_SPACE_INTF_NAME,
		.set = memory_set,
		.get = memory_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	rm48x_spi_register_attribute(clss);
}
