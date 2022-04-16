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
#include <skyeye_net_intf.h>
#define DEBUG
#include <skyeye_log.h>
#include "emac_module_omapl138.h"

//#define FRAME_DBG

/* WARING: only can open this macros on linux */
//#define LINUX

static void dbg_buff_desc(buff_desc_t *pdesc)
{
	printf("\n---------------------------------------SEND-------------------------------------------\n");
	printf("PNEXT_DESC: 0x%x\n", pdesc->pnext);
	printf("BUFF_POINTER: 0x%x\n", pdesc->pbuffer);
	printf("BUFF_OFF: %d\tBUFF_LEN: %d\n", pdesc->buff_off, pdesc->buff_len);
	printf("FLAG: ");
	if (pdesc->pkt_flg & EMAC_DSC_FLAG_SOP)
		printf("SOP ");
	if (pdesc->pkt_flg & EMAC_DSC_FLAG_EOP)
		printf("EOP ");
	if (pdesc->pkt_flg & EMAC_DSC_FLAG_OWNER)
		printf("OWNER ");
	if (pdesc->pkt_flg & EMAC_DSC_FLAG_EOQ)
		printf("EOQ ");
	if (pdesc->pkt_flg & EMAC_DSC_FLAG_TDOWNCMPLT)
		printf("TDOWNCMPLT ");
	if (pdesc->pkt_flg & EMAC_DSC_FLAG_PASSCRC)
		printf("PASSCRC ");
	printf("\n");

	printf("PKT_LEN: %d\n", pdesc->pkt_len);
	printf("\n\n");
}
static void dbg_mac_addr(unsigned char *macaddr)
{
	printf("MACADDR: %02x:%02x:%02x:%02x:%02x:%02x\n", macaddr[0], macaddr[1],
			macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
}
#ifdef LINUX
extern void decodeEtherFrame(unsigned char *buffer, int size);
#endif
static void dbg_ethernet_frame(uint8_t *frame, uint16_t len)
{
#ifdef LINUX
	decodeEtherFrame(frame, (unsigned int)len);
#endif
}
/* --------------------------------------------------------------------------------- */
static void do_tx_signal(conf_object_t *opaque, int index)
{
	emac_module_omapl138_device *dev = opaque->obj;
	uint32_t c0txenid, c0txen, c0txstatid, c0txstat;

	dev->regs.txintstatraw |= 1 << index;

	if (BITS(dev->txintmask, index, index) == 0) /* masked */{
		dev->regs.txintstatmasked |= 1 << index;
	} else if (BITS(dev->txintmask, index, index) == 1) /* unmasked */{
		dev->regs.macinvector |= (1 << index) << 16;
		/* FIXME: how to distinguish is c0txen, c1txen, c2txen? */
		c0txenid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c0txen");
		c0txen = dev->reg_iface->get_regvalue_by_id(dev->emac_ctl, c0txenid);

		if (BITS(c0txen, index, index)) {
			c0txstatid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c0txstat");
			c0txstat = dev->reg_iface->get_regvalue_by_id(dev->emac_ctl, c0txstatid);
			c0txstat |= 1 << index;
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, c0txstat, c0txstatid);

#define EMAC_C0TX 35
#define EMAC_C1TX 39
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, EMAC_C0TX);
		}
	}
}
static void do_rx_signal(conf_object_t *opaque, int index)
{
	emac_module_omapl138_device *dev = opaque->obj;
	uint32_t c0rxenid, c0rxen, c0rxstatid, c0rxstat;

	dev->regs.rxintstatraw |= 1 << index;

	if (BITS(dev->rxintmask, index, index) == 0) /* masked */{
		dev->regs.rxintstatmasked |= 1 << index;
	} else if (BITS(dev->rxintmask, index, index) == 1) /* unmasked */{
		dev->regs.macinvector |= (1 << index) << 16;
		if (dev->reg_iface == NULL)
			return;
		/* FIXME: how to distinguish is c0txen, c1txen, c2txen? */
		c0rxenid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c0rxen");
		c0rxen = dev->reg_iface->get_regvalue_by_id(dev->emac_ctl, c0rxenid);

		if (BITS(c0rxen, index, index)) {
			if (dev->reg_iface == NULL)
				return;
			c0rxstatid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c0rxstat");
			c0rxstat = dev->reg_iface->get_regvalue_by_id(dev->emac_ctl, c0rxstatid);
			c0rxstat |= 1 << index;
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, c0rxstat, c0rxstatid);

#define EMAC_C0RX 34
#define EMAC_C1RX 38
			if (dev->signal_iface)
				dev->signal_iface->raise_signal(dev->signal, EMAC_C0RX);
		}
	}
}
static void do_emac_tx(conf_object_t *opaque, uint32_t *txnhdp, uint32_t *txncp, int index)
{
	emac_module_omapl138_device *dev = opaque->obj;
	get_page_t *page;
	buff_desc_t *ppos, *sop_desc;
	uint8_t *pbuffer;
	uint16_t pos = 0, ispktf = 0, offset;
	uint32_t guest_addr = *txnhdp;

	if (*txnhdp == 0 || BITS(dev->regs.txcontrol, 0, 0) == 0) return;

	while (guest_addr) {
		*txncp = guest_addr;

		if (dev->memory_iface)
		{
			page = dev->memory_iface->get_page(dev->memory, guest_addr);
			ppos = (buff_desc_t *)(page->page_base + (guest_addr & 0xfff));
		}
#ifdef FRAME_DBG
		dbg_buff_desc(ppos);
#endif

		guest_addr = ppos->pnext;

		if (guest_addr == 0 && HAS_EOP(ppos->pkt_flg))
			SET_EOQ(ppos->pkt_flg);

		if (HAS_SOP(ppos->pkt_flg)) {
			if (ppos->pkt_len < 60 || ppos->pkt_len > 1514) {
				printf("%s:%d:Illegal packet!\n", __FILE__, __LINE__);
				continue;
			}
			sop_desc	= ppos;
			pos		= 0;
			ispktf		= 1;
		}

		if (ispktf) {
			if (dev->memory_iface)
			{
				page = dev->memory_iface->get_page(dev->memory, ppos->pbuffer);
				pbuffer = (uint8_t *)(page->page_base + (ppos->pbuffer & 0xfff));
			}

			offset = HAS_SOP(ppos->pkt_flg) ? ppos->buff_off : 0;
			memcpy(&dev->tx_data[pos], &pbuffer[offset], ppos->buff_len);
			pos += ppos->buff_len;

			if (pos < sop_desc->pkt_len && !HAS_EOP(sop_desc->pkt_flg))
				continue;

			if (pos == sop_desc->pkt_len && HAS_EOP(sop_desc->pkt_flg)) {
				pos = ispktf = 0;
				CLR_OWNER(sop_desc->pkt_flg);
#ifdef FRAME_DBG
				dbg_ethernet_frame(dev->tx_data, sop_desc->pkt_len);
#endif
				if (dev->net_sim_isok) {
					dev->net_iface->net_write(NET_DEVICE(dev), dev->tx_data, sop_desc->pkt_len);
					do_tx_signal(opaque, index);
				}
				continue;
			}

			printf("%s:%d:Illegal packet!\n", __FILE__, __LINE__);
			pos = ispktf = 0;
		}
	}
	*txnhdp = guest_addr;
}
static void do_emac_rx(conf_object_t *opaque, uint8_t *rx_data, uint16_t len, int channel)
{
	emac_module_omapl138_device *dev = ((conf_object_t *)opaque)->obj;
	get_page_t *page;
	buff_desc_t *ppos, *sop_desc;
	uint16_t buff_len, pos = 0, copy_len;
	int is_sop = 1;
	uint8_t *pbuffer;
	uint32_t guest_addr;

	guest_addr = *dev->rx_ctx[channel].rxnhdp;

	while (guest_addr) {
		*dev->rx_ctx[channel].rxncp = guest_addr;

		if (dev->memory_iface)
		{
			page = dev->memory_iface->get_page(dev->memory, guest_addr);
			ppos = (buff_desc_t *)(page->page_base + (guest_addr & 0xfff));
		}

		guest_addr = ppos->pnext;
		*dev->rx_ctx[channel].rxnhdp = guest_addr;

		if (guest_addr == 0)
			SET_EOQ(ppos->pkt_flg);

		buff_len = ppos->buff_len;

		if (is_sop == 1) {
			sop_desc = ppos;

			if (dev->memory_iface)
			{
				page = dev->memory_iface->get_page(dev->memory, ppos->pbuffer);
				pbuffer = (uint8_t *)(page->page_base + (ppos->pbuffer & 0xfff));
			}

			ppos->buff_off = dev->regs.rxbufferoffset;

			if (buff_len - ppos->buff_off >= len) {
				memcpy(&pbuffer[ppos->buff_off], &rx_data[pos], len);
				SET_SOP(sop_desc->pkt_flg);
				SET_EOP(ppos->pkt_flg);
				CLR_OWNER(sop_desc->pkt_flg);
				SET_BUFF_LEN(ppos->buff_len, len);
				SET_PKG_LEN(sop_desc->pkt_len, len);

				do_rx_signal(opaque, channel);
				break;
			}

			memcpy(&pbuffer[ppos->buff_off], &rx_data[pos], buff_len - ppos->buff_off);
			pos += buff_len - ppos->buff_off;

			SET_SOP(sop_desc->pkt_flg);
			SET_BUFF_LEN(ppos->buff_len, buff_len - ppos->buff_off);

			is_sop = 0;
			continue;
		}
		if (is_sop == 0) {
			if (dev->memory_iface)
			{
				page = dev->memory_iface->get_page(dev->memory, ppos->pbuffer);
				pbuffer = (uint8_t *)(page->page_base + (ppos->pbuffer & 0xfff));
			}

			if (buff_len >= len - pos) {
				memcpy(pbuffer, &rx_data[pos], len - pos);
				SET_EOP(ppos->pkt_flg);
				CLR_OWNER(sop_desc->pkt_flg);
				SET_BUFF_LEN(ppos->buff_len, len - pos);
				SET_PKG_LEN(sop_desc->pkt_len, len);

				do_rx_signal(opaque, channel);
				break;
			}
			memcpy(pbuffer, &rx_data[pos], buff_len);
			pos += buff_len;

			SET_BUFF_LEN(ppos->buff_len, buff_len);
		}
	}
}
#define ISBROADCAST(pmac) ({							\
	unsigned char broadcast[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};	\
	int ret = 0;								\
	ret = memcmp(pmac, broadcast, sizeof(broadcast));			\
	!ret;									\
})
#define ISMULTICAST(pmac) ((pmac)[0] & 0x1)
#define ISUNICAST(pmac) (!((pmac)[0] & 0x1))
static void emac_read(void *opaque)
{
	emac_module_omapl138_device *dev = ((conf_object_t *)opaque)->obj;
	struct ethhdr *hdr_ether;
	int ret, channel;
	mac_table_t *mac_info;

	if (BITS(dev->regs.rxcontrol, 0, 0) == 0) return;

	if ((ret = dev->net_iface->net_read(NET_DEVICE(dev), dev->rx_data, 1514)) == -1)
		return;

	hdr_ether = (struct ethhdr *)dev->rx_data;
	if (ISBROADCAST(hdr_ether->h_dest) && BITS(dev->regs.rxmbpenable, 13, 13)) {
		channel = BITS(dev->regs.rxmbpenable, 8, 10);
		mac_info = dev->rx_ctx[channel].mac_info;

		if (mac_info && mac_info->valid == 1) {
#ifdef FRAME_DBG
			printf("\n---------------------------------------RECV-------------------------------------------\n");
			printf("LEN: %d\tTYPE:ISBROADCAST\tCHANNEL:%d\n", ret, channel);
			dbg_ethernet_frame(dev->rx_data, ret);
#endif
			do_emac_rx(opaque, dev->rx_data, (uint16_t)ret, channel);
		}

	} else if (ISMULTICAST(hdr_ether->h_dest) && BITS(dev->regs.rxmbpenable, 5, 5)) {
		channel = BITS(dev->regs.rxmbpenable, 0, 2); 
		/* FIXME: use MACHASH to detemin accept or discard for multicast */

#ifdef FRAME_DBG
		printf("\n---------------------------------------RECV-------------------------------------------\n");
		printf("LEN: %d\tTYPE:ISBROADCAST\tCHANNEL:%d\n", ret, channel);
		dbg_ethernet_frame(dev->rx_data, ret);
#endif
	} else if (ISUNICAST(hdr_ether->h_dest)) {
		for (channel = 0; channel < MAXCHANNEL; channel++) {
			mac_info = dev->rx_ctx[channel].mac_info;
			if (mac_info == NULL)
				continue;
			if (mac_info->valid == 0)
				continue;
			if (mac_info->matchfilt == 0)
				continue;
			if (memcmp(hdr_ether->h_dest, mac_info->macaddr, 6) == 0
					&& BITS(dev->rxunicast, channel, channel)) {
#ifdef FRAME_DBG
				printf("\n---------------------------------------RECV-------------------------------------------\n");
				printf("LEN: %d\tTYPE:ISUNICAST\tCHANNEL:%d\n", ret, channel);
				dbg_ethernet_frame(dev->rx_data, ret);
#endif
				do_emac_rx(opaque, dev->rx_data, (uint16_t)ret, channel);
				break;
			}
		}
	}
}
static void do_maceoivector(conf_object_t *opaque, uint32_t val)
{
	emac_module_omapl138_device *dev = opaque->obj;
	uint32_t regid;

	if (dev->reg_iface == NULL)
		return;
	switch (val & 0x1f) {
		case 0x0:
			/* Acknowledge C0RXTHRESH Interrupt */
			CLRS(dev->regs.macinvector, 8, 15);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c0rxthreshstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x1:
			/* Acknowledge C0RX Interrupt */
			dev->regs.rxintstatraw = 0;
			dev->regs.rxintstatmasked = 0;
			CLRS(dev->regs.macinvector, 0, 7);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c0rxstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x2:
			/* Acknowledge C0TX Interrupt */
			dev->regs.txintstatraw = 0;
			dev->regs.txintstatmasked = 0;
			CLRS(dev->regs.macinvector, 16, 23);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c0txstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x3:
			/* Acknowledge C0MISC Interrupt */
			CLRS(dev->regs.macinvector, 24, 27);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c0miscstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x4:
			/* Acknowledge C1RXTHRESH Interrupt */
			CLRS(dev->regs.macinvector, 8, 15);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c1rxthreshstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x5:
			/* Acknowledge C1RX Interrupt */
			dev->regs.rxintstatraw = 0;
			dev->regs.rxintstatmasked = 0;
			CLRS(dev->regs.macinvector, 0, 7);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c1rxstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x6:
			/* Acknowledge C1TX Interrupt */
			dev->regs.txintstatraw = 0;
			dev->regs.txintstatmasked = 0;
			CLRS(dev->regs.macinvector, 16, 23);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c1txstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x7:
			/* Acknowledge C1MISC Interrupt */
			CLRS(dev->regs.macinvector, 24, 27);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c1miscstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x8:
			/* Acknowledge C2RXTHRESH Interrupt */
			CLRS(dev->regs.macinvector, 8, 15);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c2rxthreshstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0x9:
			/* Acknowledge C2RX Interrupt */
			dev->regs.rxintstatraw = 0;
			dev->regs.rxintstatmasked = 0;
			CLRS(dev->regs.macinvector, 0, 7);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c2rxstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0xa:
			/* Acknowledge C2TX Interrupt */
			dev->regs.txintstatraw = 0;
			dev->regs.txintstatmasked = 0;
			CLRS(dev->regs.macinvector, 16, 23);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c2txstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		case 0xb:
			/* Acknowledge C2MISC Interrupt */
			CLRS(dev->regs.macinvector, 24, 27);
			regid = dev->reg_iface->get_regid_by_name(dev->emac_ctl, "c2miscstat");
			dev->reg_iface->set_regvalue_by_id(dev->emac_ctl, 0, regid);
			break;
		default:
			break;
	}
}
static exception_t emac_module_omapl138_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	emac_module_omapl138_device *dev = opaque->obj;
	switch (offset)
	{
		case TXREVID:
			*(uint32_t *)buf = dev->regs.txrevid;
			break;
		case TXCONTROL:
			*(uint32_t *)buf = dev->regs.txcontrol;
			break;
		case TXTEARDOWN:
			*(uint32_t *)buf = dev->regs.txteardown;
			break;
		case RXREVID:
			*(uint32_t *)buf = dev->regs.rxrevid;
			break;
		case RXCONTROL:
			*(uint32_t *)buf = dev->regs.rxcontrol;
			break;
		case RXTEARDOWN:
			*(uint32_t *)buf = dev->regs.rxteardown;
			break;
		case TXINTSTATRAW:
			*(uint32_t *)buf = dev->regs.txintstatraw;
			break;
		case TXINTSTATMASKED:
			*(uint32_t *)buf = dev->regs.txintstatmasked;
			break;
		case TXINTMASKSET:
			*(uint32_t *)buf = dev->regs.txintmaskset;
			break;
		case TXINTMASKCLEAR:
			*(uint32_t *)buf = dev->regs.txintmaskclear;
			break;
		case MACINVECTOR:
			*(uint32_t *)buf = dev->regs.macinvector;
			break;
		case MACEOIVECTOR:
			*(uint32_t *)buf = dev->regs.maceoivector;
			break;
		case RXINTSTATRAW:
			*(uint32_t *)buf = dev->regs.rxintstatraw;
			break;
		case RXINTSTATMASKED:
			*(uint32_t *)buf = dev->regs.rxintstatmasked;
			break;
		case RXINTMASKSET:
			*(uint32_t *)buf = dev->regs.rxintmaskset;
			break;
		case RXINTMASKCLEAR:
			*(uint32_t *)buf = dev->regs.rxintmaskclear;
			break;
		case MACINTSTATRAW:
			*(uint32_t *)buf = dev->regs.macintstatraw;
			break;
		case MACINTSTATMASKED:
			*(uint32_t *)buf = dev->regs.macintstatmasked;
			break;
		case MACINTMASKSET:
			*(uint32_t *)buf = dev->regs.macintmaskset;
			break;
		case MACINTMASKCLEAR:
			*(uint32_t *)buf = dev->regs.macintmaskclear;
			break;
		case RXMBPENABLE:
			*(uint32_t *)buf = dev->regs.rxmbpenable;
			break;
		case RXUNICASTSET:
			*(uint32_t *)buf = dev->regs.rxunicastset;
			break;
		case RXUNICASTCLEAR:
			*(uint32_t *)buf = dev->regs.rxunicastclear;
			break;
		case RXMAXLEN:
			*(uint32_t *)buf = dev->regs.rxmaxlen;
			break;
		case RXBUFFEROFFSET:
			*(uint32_t *)buf = dev->regs.rxbufferoffset;
			break;
		case RXFILTERLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rxfilterlowthresh;
			break;
		case RX0FLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rx0flowthresh;
			break;
		case RX1FLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rx1flowthresh;
			break;
		case RX2FLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rx2flowthresh;
			break;
		case RX3FLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rx3flowthresh;
			break;
		case RX4FLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rx4flowthresh;
			break;
		case RX5FLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rx5flowthresh;
			break;
		case RX6FLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rx6flowthresh;
			break;
		case RX7FLOWTHRESH:
			*(uint32_t *)buf = dev->regs.rx7flowthresh;
			break;
		case RX0FREEBUFFER:
			*(uint32_t *)buf = dev->regs.rx0freebuffer;
			break;
		case RX1FREEBUFFER:
			*(uint32_t *)buf = dev->regs.rx1freebuffer;
			break;
		case RX2FREEBUFFER:
			*(uint32_t *)buf = dev->regs.rx2freebuffer;
			break;
		case RX3FREEBUFFER:
			*(uint32_t *)buf = dev->regs.rx3freebuffer;
			break;
		case RX4FREEBUFFER:
			*(uint32_t *)buf = dev->regs.rx4freebuffer;
			break;
		case RX5FREEBUFFER:
			*(uint32_t *)buf = dev->regs.rx5freebuffer;
			break;
		case RX6FREEBUFFER:
			*(uint32_t *)buf = dev->regs.rx6freebuffer;
			break;
		case RX7FREEBUFFER:
			*(uint32_t *)buf = dev->regs.rx7freebuffer;
			break;
		case MACCONTROL:
			*(uint32_t *)buf = dev->regs.maccontrol;
			break;
		case MACSTATUS:
			*(uint32_t *)buf = dev->regs.macstatus;
			break;
		case EMCONTROL:
			*(uint32_t *)buf = dev->regs.emcontrol;
			break;
		case FIFOCONTROL:
			*(uint32_t *)buf = dev->regs.fifocontrol;
			break;
		case MACCONFIG:
			*(uint32_t *)buf = dev->regs.macconfig;
			break;
		case SOFTRESET:
			*(uint32_t *)buf = dev->regs.softreset;
			break;
		case MACSRCADDRLO:
			*(uint32_t *)buf = dev->regs.macsrcaddrlo;
			break;
		case MACSRCADDRHI:
			*(uint32_t *)buf = dev->regs.macsrcaddrhi;
			break;
		case MACHASH1:
			*(uint32_t *)buf = dev->regs.machash1;
			break;
		case MACHASH2:
			*(uint32_t *)buf = dev->regs.machash2;
			break;
		case BOFFTEST:
			*(uint32_t *)buf = dev->regs.bofftest;
			break;
		case TPACETEST:
			*(uint32_t *)buf = dev->regs.tpacetest;
			break;
		case RXPAUSE:
			*(uint32_t *)buf = dev->regs.rxpause;
			break;
		case TXPAUSE:
			*(uint32_t *)buf = dev->regs.txpause;
			break;
		case MACADDRLO:
			*(uint32_t *)buf = dev->regs.macaddrlo;
			break;
		case MACADDRHI:
			*(uint32_t *)buf = dev->regs.macaddrhi;
			break;
		case MACINDEX:
			*(uint32_t *)buf = dev->regs.macindex;
			break;
		case TX0HDP:
			*(uint32_t *)buf = dev->regs.tx0hdp;
			break;
		case TX1HDP:
			*(uint32_t *)buf = dev->regs.tx1hdp;
			break;
		case TX2HDP:
			*(uint32_t *)buf = dev->regs.tx2hdp;
			break;
		case TX3HDP:
			*(uint32_t *)buf = dev->regs.tx3hdp;
			break;
		case TX4HDP:
			*(uint32_t *)buf = dev->regs.tx4hdp;
			break;
		case TX5HDP:
			*(uint32_t *)buf = dev->regs.tx5hdp;
			break;
		case TX6HDP:
			*(uint32_t *)buf = dev->regs.tx6hdp;
			break;
		case TX7HDP:
			*(uint32_t *)buf = dev->regs.tx7hdp;
			break;
		case RX0HDP:
			*(uint32_t *)buf = dev->regs.rx0hdp;
			break;
		case RX1HDP:
			*(uint32_t *)buf = dev->regs.rx1hdp;
			break;
		case RX2HDP:
			*(uint32_t *)buf = dev->regs.rx2hdp;
			break;
		case RX3HDP:
			*(uint32_t *)buf = dev->regs.rx3hdp;
			break;
		case RX4HDP:
			*(uint32_t *)buf = dev->regs.rx4hdp;
			break;
		case RX5HDP:
			*(uint32_t *)buf = dev->regs.rx5hdp;
			break;
		case RX6HDP:
			*(uint32_t *)buf = dev->regs.rx6hdp;
			break;
		case RX7HDP:
			*(uint32_t *)buf = dev->regs.rx7hdp;
			break;
		case TX0CP:
			*(uint32_t *)buf = dev->regs.tx0cp;
			break;
		case TX1CP:
			*(uint32_t *)buf = dev->regs.tx1cp;
			break;
		case TX2CP:
			*(uint32_t *)buf = dev->regs.tx2cp;
			break;
		case TX3CP:
			*(uint32_t *)buf = dev->regs.tx3cp;
			break;
		case TX4CP:
			*(uint32_t *)buf = dev->regs.tx4cp;
			break;
		case TX5CP:
			*(uint32_t *)buf = dev->regs.tx5cp;
			break;
		case TX6CP:
			*(uint32_t *)buf = dev->regs.tx6cp;
			break;
		case TX7CP:
			*(uint32_t *)buf = dev->regs.tx7cp;
			break;
		case RX0CP:
			*(uint32_t *)buf = dev->regs.rx0cp;
			break;
		case RX1CP:
			*(uint32_t *)buf = dev->regs.rx1cp;
			break;
		case RX2CP:
			*(uint32_t *)buf = dev->regs.rx2cp;
			break;
		case RX3CP:
			*(uint32_t *)buf = dev->regs.rx3cp;
			break;
		case RX4CP:
			*(uint32_t *)buf = dev->regs.rx4cp;
			break;
		case RX5CP:
			*(uint32_t *)buf = dev->regs.rx5cp;
			break;
		case RX6CP:
			*(uint32_t *)buf = dev->regs.rx6cp;
			break;
		case RX7CP:
			*(uint32_t *)buf = dev->regs.rx7cp;
			break;
		case RXGOODFRAMES:
			*(uint32_t *)buf = dev->regs.rxgoodframes;
			break;
		case RXBCASTFRAMES:
			*(uint32_t *)buf = dev->regs.rxbcastframes;
			break;
		case RXMCASTFRAMES:
			*(uint32_t *)buf = dev->regs.rxmcastframes;
			break;
		case RXPAUSEFRAMES:
			*(uint32_t *)buf = dev->regs.rxpauseframes;
			break;
		case RXCRCERRORS:
			*(uint32_t *)buf = dev->regs.rxcrcerrors;
			break;
		case RXALIGNCODEERRORS:
			*(uint32_t *)buf = dev->regs.rxaligncodeerrors;
			break;
		case RXOVERSIZED:
			*(uint32_t *)buf = dev->regs.rxoversized;
			break;
		case RXJABBER:
			*(uint32_t *)buf = dev->regs.rxjabber;
			break;
		case RXUNDERSIZED:
			*(uint32_t *)buf = dev->regs.rxundersized;
			break;
		case RXFRAGMENTS:
			*(uint32_t *)buf = dev->regs.rxfragments;
			break;
		case RXFILTERED:
			*(uint32_t *)buf = dev->regs.rxfiltered;
			break;
		case RXQOSFILTERED:
			*(uint32_t *)buf = dev->regs.rxqosfiltered;
			break;
		case RXOCTETS:
			*(uint32_t *)buf = dev->regs.rxoctets;
			break;
		case TXGOODFRAMES:
			*(uint32_t *)buf = dev->regs.txgoodframes;
			break;
		case TXBCASTFRAMES:
			*(uint32_t *)buf = dev->regs.txbcastframes;
			break;
		case TXMCASTFRAMES:
			*(uint32_t *)buf = dev->regs.txmcastframes;
			break;
		case TXPAUSEFRAMES:
			*(uint32_t *)buf = dev->regs.txpauseframes;
			break;
		case TXDEFERRED:
			*(uint32_t *)buf = dev->regs.txdeferred;
			break;
		case TXCOLLISION:
			*(uint32_t *)buf = dev->regs.txcollision;
			break;
		case TXSINGLECOLL:
			*(uint32_t *)buf = dev->regs.txsinglecoll;
			break;
		case TXMULTICOLL:
			*(uint32_t *)buf = dev->regs.txmulticoll;
			break;
		case TXEXCESSIVECOLL:
			*(uint32_t *)buf = dev->regs.txexcessivecoll;
			break;
		case TXLATECOLL:
			*(uint32_t *)buf = dev->regs.txlatecoll;
			break;
		case TXUNDERRUN:
			*(uint32_t *)buf = dev->regs.txunderrun;
			break;
		case TXCARRIERSENSE:
			*(uint32_t *)buf = dev->regs.txcarriersense;
			break;
		case TXOCTETS:
			*(uint32_t *)buf = dev->regs.txoctets;
			break;
		case FRAME64:
			*(uint32_t *)buf = dev->regs.frame64;
			break;
		case FRAME65T127:
			*(uint32_t *)buf = dev->regs.frame65t127;
			break;
		case FRAME128T255:
			*(uint32_t *)buf = dev->regs.frame128t255;
			break;
		case FRAME256T511:
			*(uint32_t *)buf = dev->regs.frame256t511;
			break;
		case FRAME512T1023:
			*(uint32_t *)buf = dev->regs.frame512t1023;
			break;
		case FRAME1024TUP:
			*(uint32_t *)buf = dev->regs.frame1024tup;
			break;
		case NETOCTETS:
			*(uint32_t *)buf = dev->regs.netoctets;
			break;
		case RXSOFOVERRUNS:
			*(uint32_t *)buf = dev->regs.rxsofoverruns;
			break;
		case RXMOFOVERRUNS:
			*(uint32_t *)buf = dev->regs.rxmofoverruns;
			break;
		case RXDMAOVERRUNS:
			*(uint32_t *)buf = dev->regs.rxdmaoverruns;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in emac_module_omapl138\n", offset);
			break;
	}
	return 0;
}
static exception_t emac_module_omapl138_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	emac_module_omapl138_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	int channel;
	switch (offset)
	{
		case TXCONTROL:
			dev->regs.txcontrol = val;
			break;
		case TXTEARDOWN:
			dev->regs.txteardown = val;
			break;
		case RXCONTROL:
			dev->regs.rxcontrol = val;
			break;
		case RXTEARDOWN:
			dev->regs.rxteardown = val;
			break;
		case TXINTMASKSET:
			dev->regs.txintmaskset = val;
			dev->txintmask |= val;
			break;
		case TXINTMASKCLEAR:
			dev->regs.txintmaskclear = val;
			dev->txintmask &= ~val;
			break;
		case MACINVECTOR:
			dev->regs.macinvector = val;
			break;
		case MACEOIVECTOR:
			dev->regs.maceoivector = val;
			do_maceoivector(opaque, val);
			break;
		case RXINTMASKSET:
			dev->regs.rxintmaskset = val;
			dev->rxintmask |= val;
			break;
		case RXINTMASKCLEAR:
			dev->regs.rxintmaskclear = val;
			dev->rxintmask &= ~val;
			break;
		case MACINTMASKSET:
			dev->regs.macintmaskset = val;
			dev->macintmask |= val;
			break;
		case MACINTMASKCLEAR:
			dev->regs.macintmaskclear = val;
			dev->macintmask &= ~val;
			break;
		case RXMBPENABLE:
			dev->regs.rxmbpenable = val;
			break;
		case RXUNICASTSET:
			dev->regs.rxunicastset = val;
			dev->rxunicast |= val;
			break;
		case RXUNICASTCLEAR:
			dev->regs.rxunicastclear = val;
			dev->rxunicast &= ~val;
			break;
		case RXMAXLEN:
			dev->regs.rxmaxlen = val;
			break;
		case RXBUFFEROFFSET:
			dev->regs.rxbufferoffset = val;
			break;
		case RXFILTERLOWTHRESH:
			dev->regs.rxfilterlowthresh = val;
			break;
		case RX0FLOWTHRESH:
			dev->regs.rx0flowthresh = val;
			break;
		case RX1FLOWTHRESH:
			dev->regs.rx1flowthresh = val;
			break;
		case RX2FLOWTHRESH:
			dev->regs.rx2flowthresh = val;
			break;
		case RX3FLOWTHRESH:
			dev->regs.rx3flowthresh = val;
			break;
		case RX4FLOWTHRESH:
			dev->regs.rx4flowthresh = val;
			break;
		case RX5FLOWTHRESH:
			dev->regs.rx5flowthresh = val;
			break;
		case RX6FLOWTHRESH:
			dev->regs.rx6flowthresh = val;
			break;
		case RX7FLOWTHRESH:
			dev->regs.rx7flowthresh = val;
			break;
		case RX0FREEBUFFER:
			dev->regs.rx0freebuffer = val;
			break;
		case RX1FREEBUFFER:
			dev->regs.rx1freebuffer = val;
			break;
		case RX2FREEBUFFER:
			dev->regs.rx2freebuffer = val;
			break;
		case RX3FREEBUFFER:
			dev->regs.rx3freebuffer = val;
			break;
		case RX4FREEBUFFER:
			dev->regs.rx4freebuffer = val;
			break;
		case RX5FREEBUFFER:
			dev->regs.rx5freebuffer = val;
			break;
		case RX6FREEBUFFER:
			dev->regs.rx6freebuffer = val;
			break;
		case RX7FREEBUFFER:
			dev->regs.rx7freebuffer = val;
			break;
		case MACCONTROL:
			dev->regs.maccontrol = val;
			break;
		case EMCONTROL:
			dev->regs.emcontrol = val;
			break;
		case FIFOCONTROL:
			dev->regs.fifocontrol = val;
			break;
		case MACCONFIG:
			dev->regs.macconfig = val;
			break;
		case SOFTRESET:
			dev->regs.softreset = val;
			/* soft reset */
			if (val & 0x1) {
				/* do something soft reset */
				dev->regs.softreset &= ~0x1;
			}
			break;
		case MACSRCADDRLO:
			dev->regs.macsrcaddrlo = val;
			break;
		case MACSRCADDRHI:
			dev->regs.macsrcaddrhi = val;
			break;
		case MACHASH1:
			dev->regs.machash1 = val;
			break;
		case MACHASH2:
			dev->regs.machash2 = val;
			break;
		case TPACETEST:
			dev->regs.tpacetest = val;
			break;
		case MACADDRLO:
			dev->regs.macaddrlo = val;
			if (dev->regs.macindex < 0 || dev->regs.macindex > 7) return;
			channel = BITS(dev->regs.macaddrlo, 16, 18);
			dev->mac_table[dev->regs.macindex].macaddr[5] = BITS(dev->regs.macaddrlo, 8, 15);
			dev->mac_table[dev->regs.macindex].macaddr[4] = BITS(dev->regs.macaddrlo, 0, 7);
			dev->mac_table[dev->regs.macindex].macaddr[3] = BITS(dev->regs.macaddrhi, 24, 31);
			dev->mac_table[dev->regs.macindex].macaddr[2] = BITS(dev->regs.macaddrhi, 16, 23);
			dev->mac_table[dev->regs.macindex].macaddr[1] = BITS(dev->regs.macaddrhi, 8, 15);
			dev->mac_table[dev->regs.macindex].macaddr[0] = BITS(dev->regs.macaddrhi, 0, 7);
			dev->mac_table[dev->regs.macindex].matchfilt = BITS(dev->regs.macaddrlo, 19, 19);
			dev->mac_table[dev->regs.macindex].valid = BITS(dev->regs.macaddrlo, 20, 20);
			dev->mac_table[dev->regs.macindex].channel = channel;

			dev->rx_ctx[channel].mac_info = &dev->mac_table[dev->regs.macindex];

			switch (channel) {
				case 0:
					dev->rx_ctx[channel].rxnhdp = &dev->regs.rx0hdp;
					dev->rx_ctx[channel].rxncp = &dev->regs.rx0cp;
					break;
				case 1:
					dev->rx_ctx[channel].rxnhdp = &dev->regs.rx1hdp;
					dev->rx_ctx[channel].rxncp = &dev->regs.rx1cp;
					break;
				case 2:
					dev->rx_ctx[channel].rxnhdp = &dev->regs.rx2hdp;
					dev->rx_ctx[channel].rxncp = &dev->regs.rx2cp;
					break;
				case 3:
					dev->rx_ctx[channel].rxnhdp = &dev->regs.rx3hdp;
					dev->rx_ctx[channel].rxncp = &dev->regs.rx3cp;
					break;
				case 4:
					dev->rx_ctx[channel].rxnhdp = &dev->regs.rx4hdp;
					dev->rx_ctx[channel].rxncp = &dev->regs.rx4cp;
					break;
				case 5:
					dev->rx_ctx[channel].rxnhdp = &dev->regs.rx5hdp;
					dev->rx_ctx[channel].rxncp = &dev->regs.rx5cp;
					break;
				case 6:
					dev->rx_ctx[channel].rxnhdp = &dev->regs.rx6hdp;
					dev->rx_ctx[channel].rxncp = &dev->regs.rx6cp;
					break;
				case 7:
					dev->rx_ctx[channel].rxnhdp = &dev->regs.rx7hdp;
					dev->rx_ctx[channel].rxncp = &dev->regs.rx7cp;
					break;
			}

			break;
		case MACADDRHI:
			dev->regs.macaddrhi = val;
			break;
		case MACINDEX:
			dev->regs.macindex = val;
			break;
		case TX0HDP:
			dev->regs.tx0hdp = val;
			do_emac_tx(opaque, &dev->regs.tx0hdp, &dev->regs.tx0cp, 0);
			break;
		case TX1HDP:
			dev->regs.tx1hdp = val;
			do_emac_tx(opaque, &dev->regs.tx1hdp, &dev->regs.tx1cp, 1);
			break;
		case TX2HDP:
			dev->regs.tx2hdp = val;
			do_emac_tx(opaque, &dev->regs.tx2hdp, &dev->regs.tx2cp, 2);
			break;
		case TX3HDP:
			dev->regs.tx3hdp = val;
			do_emac_tx(opaque, &dev->regs.tx3hdp, &dev->regs.tx3cp, 3);
			break;
		case TX4HDP:
			dev->regs.tx4hdp = val;
			do_emac_tx(opaque, &dev->regs.tx4hdp, &dev->regs.tx4cp, 4);
			break;
		case TX5HDP:
			dev->regs.tx5hdp = val;
			do_emac_tx(opaque, &dev->regs.tx5hdp, &dev->regs.tx5cp, 5);
			break;
		case TX6HDP:
			dev->regs.tx6hdp = val;
			do_emac_tx(opaque, &dev->regs.tx6hdp, &dev->regs.tx6cp, 6);
			break;
		case TX7HDP:
			dev->regs.tx7hdp = val;
			do_emac_tx(opaque, &dev->regs.tx7hdp, &dev->regs.tx7cp, 7);
			break;
		case RX0HDP:
			dev->regs.rx0hdp = val;
			break;
		case RX1HDP:
			dev->regs.rx1hdp = val;
			break;
		case RX2HDP:
			dev->regs.rx2hdp = val;
			break;
		case RX3HDP:
			dev->regs.rx3hdp = val;
			break;
		case RX4HDP:
			dev->regs.rx4hdp = val;
			break;
		case RX5HDP:
			dev->regs.rx5hdp = val;
			break;
		case RX6HDP:
			dev->regs.rx6hdp = val;
			break;
		case RX7HDP:
			dev->regs.rx7hdp = val;
			break;
		case TX0CP:
			break;
		case TX1CP:
			break;
		case TX2CP:
			break;
		case TX3CP:
			break;
		case TX4CP:
			break;
		case TX5CP:
			break;
		case TX6CP:
			break;
		case TX7CP:
			break;
		case RX0CP:
			break;
		case RX1CP:
			break;
		case RX2CP:
			break;
		case RX3CP:
			break;
		case RX4CP:
			break;
		case RX5CP:
			break;
		case RX6CP:
			break;
		case RX7CP:
			break;
		case RXGOODFRAMES:
			dev->regs.rxgoodframes = val;
			break;
		case RXBCASTFRAMES:
			dev->regs.rxbcastframes = val;
			break;
		case RXMCASTFRAMES:
			dev->regs.rxmcastframes = val;
			break;
		case RXPAUSEFRAMES:
			dev->regs.rxpauseframes = val;
			break;
		case RXCRCERRORS:
			dev->regs.rxcrcerrors = val;
			break;
		case RXALIGNCODEERRORS:
			dev->regs.rxaligncodeerrors = val;
			break;
		case RXOVERSIZED:
			dev->regs.rxoversized = val;
			break;
		case RXJABBER:
			dev->regs.rxjabber = val;
			break;
		case RXUNDERSIZED:
			dev->regs.rxundersized = val;
			break;
		case RXFRAGMENTS:
			dev->regs.rxfragments = val;
			break;
		case RXFILTERED:
			dev->regs.rxfiltered = val;
			break;
		case RXQOSFILTERED:
			dev->regs.rxqosfiltered = val;
			break;
		case RXOCTETS:
			dev->regs.rxoctets = val;
			break;
		case TXGOODFRAMES:
			dev->regs.txgoodframes = val;
			break;
		case TXBCASTFRAMES:
			dev->regs.txbcastframes = val;
			break;
		case TXMCASTFRAMES:
			dev->regs.txmcastframes = val;
			break;
		case TXPAUSEFRAMES:
			dev->regs.txpauseframes = val;
			break;
		case TXDEFERRED:
			dev->regs.txdeferred = val;
			break;
		case TXCOLLISION:
			dev->regs.txcollision = val;
			break;
		case TXSINGLECOLL:
			dev->regs.txsinglecoll = val;
			break;
		case TXMULTICOLL:
			dev->regs.txmulticoll = val;
			break;
		case TXEXCESSIVECOLL:
			dev->regs.txexcessivecoll = val;
			break;
		case TXLATECOLL:
			dev->regs.txlatecoll = val;
			break;
		case TXUNDERRUN:
			dev->regs.txunderrun = val;
			break;
		case TXCARRIERSENSE:
			dev->regs.txcarriersense = val;
			break;
		case TXOCTETS:
			dev->regs.txoctets = val;
			break;
		case FRAME64:
			dev->regs.frame64 = val;
			break;
		case FRAME65T127:
			dev->regs.frame65t127 = val;
			break;
		case FRAME128T255:
			dev->regs.frame128t255 = val;
			break;
		case FRAME256T511:
			dev->regs.frame256t511 = val;
			break;
		case FRAME512T1023:
			dev->regs.frame512t1023 = val;
			break;
		case FRAME1024TUP:
			dev->regs.frame1024tup = val;
			break;
		case NETOCTETS:
			dev->regs.netoctets = val;
			break;
		case RXSOFOVERRUNS:
			dev->regs.rxsofoverruns = val;
			break;
		case RXMOFOVERRUNS:
			dev->regs.rxmofoverruns = val;
			break;
		case RXDMAOVERRUNS:
			dev->regs.rxdmaoverruns = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in emac_module_omapl138\n", offset);
			break;
	}
	return 0;
}
static char* emac_module_omapl138_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t emac_module_omapl138_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	emac_module_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t emac_module_omapl138_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t emac_module_omapl138_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	emac_module_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t emac_module_omapl138_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static exception_t general_memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	emac_module_omapl138_device *dev = obj->obj;

	dev->memory = obj2;
	dev->memory_iface = (memory_space_intf *)SKY_get_iface(dev->memory, MEMORY_SPACE_INTF_NAME);
	if (dev->memory_iface == NULL){
		if (dev->memory == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_memory_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	emac_module_omapl138_device *dev = obj->obj;

	*obj2 = dev->memory;
	*port = NULL;
	return No_exp;
}
static exception_t general_net_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	emac_module_omapl138_device *dev = obj->obj;

	dev->net = obj2;
	dev->net_iface = (skyeye_net_ops_intf *)SKY_get_iface(dev->net, SKYEYE_NET_INTF_NAME);
	if (dev->net_iface == NULL){
		if (dev->net == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_NET_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_NET_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	dev->net_sim_isok = !dev->net_iface->net_open(NET_DEVICE(dev));
	return No_exp;
}

static exception_t general_net_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	emac_module_omapl138_device *dev = obj->obj;

	*obj2 = dev->net;
	*port = NULL;
	return No_exp;
}
static exception_t general_emac_ctl_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	emac_module_omapl138_device *dev = obj->obj;

	dev->emac_ctl = obj2;
	dev->reg_iface = (skyeye_reg_intf *)SKY_get_iface(dev->emac_ctl, SKYEYE_REG_INTF);
	if (dev->reg_iface == NULL){
		if (dev->emac_ctl == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_REG_INTF);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_REG_INTF, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_emac_ctl_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	emac_module_omapl138_device *dev = obj->obj;

	*obj2 = dev->emac_ctl;
	*port = NULL;
	return No_exp;
}
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	emac_module_omapl138_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	emac_module_omapl138_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static conf_object_t* new_emac_module_omapl138(char *obj_name)
{
	emac_module_omapl138_device* dev = skyeye_mm_zero(sizeof(emac_module_omapl138_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);
	int i;

	regs->txrevid		= 0x4ec0020d;
	regs->rxrevid		= 0x4ec0020d;
	regs->rxmaxlen		= 0x5ee;
	regs->fifocontrol	= 0x2;
	regs->macconfig		= 0x2 | 0x2 << 8 | 0x3 << 16 | 0x3 << 24;

	dev->tx_data = skyeye_mm_zero(1514);
	dev->rx_data = skyeye_mm_zero(1514);

	create_thread_scheduler(1, Periodic_sched, emac_read, dev->obj, &dev->scheduler_id);
	return dev->obj;
}
static exception_t free_emac_module_omapl138(conf_object_t* opaque)
{
	emac_module_omapl138_device *dev = opaque->obj;

	if (dev->scheduler_id != -1) {
		del_thread_scheduler(dev->scheduler_id);
		dev->scheduler_id = -1;
	}
	skyeye_free(dev->tx_data);
	skyeye_free(dev->rx_data);

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_emac_module_omapl138(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "omapl138_emac_module",
		.class_desc = "omapl138_emac_module",
		.new_instance = new_emac_module_omapl138,
		.free_instance = free_emac_module_omapl138,
		.module_type = SKYML_ONCHIP(SC_omapl138),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = emac_module_omapl138_read,
		.write = emac_module_omapl138_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = emac_module_omapl138_get_regvalue_by_id,
		.get_regname_by_id = emac_module_omapl138_get_regname_by_id,
		.set_regvalue_by_id = emac_module_omapl138_set_regvalue_by_id,
		.get_regnum = emac_module_omapl138_get_regnum,
		.get_regid_by_name = emac_module_omapl138_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
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
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.set = general_memory_space_set,
			.get = general_memory_space_get,
		},
		(struct ConnectDescription) {
			.name = SKYEYE_NET_INTF_NAME,
			.set = general_net_space_set,
			.get = general_net_space_get,
		},
		(struct ConnectDescription) {
			.name = SKYEYE_REG_INTF,
			.set = general_emac_ctl_set,
			.get = general_emac_ctl_get,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
