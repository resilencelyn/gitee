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
#include "qportal_fd.h"
#include <memory_monitor.h>
#include <skyeye_system.h>
#define DEBUG
#include <skyeye_log.h>
#include "FMan_T2080.h"


#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

static exception_t FMan_T2080_regs_init(FMan_T2080_device *dev){
	dev->rx_addr = 0x7f40000 ; 
	dev->FPM[0x0c4] = 0x0A070603;
	dev->FPM[0x064] = 0x00010000;
	dev->FPM[0x010] = 0x10101010;
	dev->FPM[0x014] = 0x10101010;
	dev->FPM[0x100] = 0x80000000;
	int FMFP_PSn;
	for (FMFP_PSn=0;FMFP_PSn<0xC4;FMFP_PSn+=4){
		dev->FPM[0x100+FMFP_PSn] = 0x80000000;
	}
	dev->FMHP.QMI_common[0x400] = 0xc0002020;
	int id ;
	for (id=0x08;id<0x12;id++){//Rx

		dev->FMHP.id_fmhp[id-1].BMI[0x004]=id*0x00110000;
		dev->FMHP.id_fmhp[id-1].BMI[0x00C]=0x03FF03FF;
		dev->FMHP.id_fmhp[id-1].BMI[0x014]=0x00000002;
		dev->FMHP.id_fmhp[id-1].BMI[0x01C]=0x00020000;
		dev->FMHP.id_fmhp[id-1].BMI[0x020]=0x00440000;
		dev->FMHP.id_fmhp[id-1].BMI[0x024]=0x803C0000;
		dev->FMHP.id_fmhp[id-1].BMI[0x028]=0x00480000;
		dev->FMHP.id_fmhp[id-1].BMI[0x040]=id*0x01000000;
		dev->FMHP.id_fmhp[id-1].BMI[0x070]=0x00D40000;
		dev->FMHP.id_fmhp[id-1].BMI[0x300]=0x00000090;
		dev->FMHP.id_fmhp[id-1].QMI[0x41C]=0x005000C0;
		
	}
	for (id>=0x28;id<0x31;id++){//Tx
		dev->FMHP.id_fmhp[id-1].BMI[0x004]=id*0x00110000;
		dev->FMHP.id_fmhp[id-1].BMI[0x00C]=id*0x00001000+0x13;
		dev->FMHP.id_fmhp[id-1].BMI[0x018]=0x00580000;
		dev->FMHP.id_fmhp[id-1].BMI[0x01C]=0x8C000000;
		dev->FMHP.id_fmhp[id-1].BMI[0x028]=0x00D40000;
		dev->FMHP.id_fmhp[id-1].BMI[0x070]=0x80000000;
		dev->FMHP.id_fmhp[id-1].BMI[0x074]=0x01234567;
		dev->FMHP.id_fmhp[id-1].BMI[0x300]=0x00000090;
		dev->FMHP.id_fmhp[id-1].QMI[0x41C]=0x005000C0+0x100*id;
		dev->FMHP.id_fmhp[id-1].QMI[0x42C]=0x00500274;
		dev->FMHP.id_fmhp[id-1].QMI[0x430]=0x1000FFFF;
	}
	dev->Policer[0x000] = 0x00500002;
	dev->Policer[0x08c] = 0xA000FFFF;
	dev->KeyGen[0x110] =0xFFFFFFFF;
	dev->KeyGen[0x120] =0x00FF0000;
	dev->KeyGen[0x124] =0x00FF0000;
	dev->KeyGen[0x128] =0x00FF0000;
	dev->KeyGen[0x12C] =0x00FF0000;
	dev->KeyGen[0x130] =0x00FF0000;
	dev->KeyGen[0x134] =0x00FF0000;
	dev->KeyGen[0x138] =0x00FF0000;
	dev->KeyGen[0x13c] =0x00FF0000;
	dev->DMA[0x04] = 0x2000E800;
	dev->DMA[0x08] = 0x3F003F00;
	dev->DMA[0x0C] = 0x2A000000;
	dev->IEEE_1588[0x000] = 0x03000100;
	dev->IEEE_1588[0x080] = 0x00010001;
	dev->IEEE_1588[0x0A8] = 0x00000002;
	dev->IEEE_1588[0x0B8] = 0xFFFFFFFF;
	dev->IEEE_1588[0x0BC] = 0xFFFFFFFF;
	dev->IEEE_1588[0x0C0] = 0xFFFFFFFF;
	dev->IEEE_1588[0x0C4] = 0xFFFFFFFF;
	dev->IEEE_1588[0x0D0] = 0xFFFFFFFF;
	dev->IEEE_1588[0x0D4] = 0xFFFFFFFF;
	dev->IEEE_1588[0x0D8] = 0xFFFFFFFF;
	int i;
	for (i=0;i<10;i++){
		dev->EMAC[i].mEMAC[0x008] = 0x00000840;
		dev->EMAC[i].mEMAC[0x014] = 0x00000600;
		dev->EMAC[i].mEMAC[0x01C] = 0x0020;
		dev->EMAC[i].mEMAC[0x020] = 0x00100020;
		dev->EMAC[i].mEMAC[0x040] = 0x00000060;
		dev->EMAC[i].mEMAC[0x044] = 0x0000000C;
		dev->EMAC[i].mEMAC[0x300] = 0x00000010;
		dev->EMAC[i].mEMAC[0x3F8] = 0x0A800101;
		dev->EMAC[i].MDIO[0x030] = 0x00001448;
		dev->EMAC[i].MACsec[0x004] = 0x000088E5;
		dev->EMAC[i].MACsec[0x3F8] = 0x0A800101;
		dev->EMAC[i].MACsec[0x444] = 0x00000001;
	}

}

static exception_t PHY_regs_regs_reset(FMan_T2080_device *dev){
	dev->PHY.PHY_CR = 0x3100;
	dev->PHY.PHY_SR = 0x7849;
	dev->PHY.PHY1_Register = 0x0;
	dev->PHY.PHY2_Register = 0x8201;
	dev->PHY.AAR = 0x1E1;
	dev->PHY.ALPAR = 0x80;
	dev->PHY.AER = 0x0;
}

static void fill_rx_bd(FMan_T2080_device *memac, const uint8_t **buf,uint32_t size){
	get_page_t *bufptr_page;
	uint8_t  *bufptr_pbuffer;

	if ((memac->rx_addr + size) > 0x7f80000) {
		memac->rx_addr = 0x7f40000;
	}
	bufptr_page = memac->memory_iface->get_page(memac->memory,memac->rx_addr + 0x82);
	bufptr_pbuffer = (uint8_t *)(bufptr_page->page_base + ((memac->rx_addr + 0x82) & 0xfff));
	memcpy(bufptr_pbuffer, *buf, size);

		memac->fd_iface->set_fd(memac->fd,size,memac->rx_addr);

	// INIT: memac->rx_addr  = 0x7f40000 + 0x82;
	memac->rx_addr += size;
	
}

static void rx_init_frame(FMan_T2080_device *memac, const uint8_t *buf, size_t size){
	 uint32_t remaining_data;
	/* Do not copy the frame for now */
		memac->rx_data = (uint8_t *)buf;
		memac->rx_buffer_len = size;

	/* CRC padding (We don't have to compute the CRC) */
	memac->rx_padding = 4;
	memac->rx_remaining_data = memac->rx_buffer_len;
	
	if (memac->rx_buffer_len == 0) {
		/* No frame to send */
		printf("No frame to send\n");
		return;
	}
	remaining_data = memac->rx_remaining_data + memac->rx_padding;
	buf = memac->rx_data;
	if(buf != NULL){
		fill_rx_bd(memac,&buf,remaining_data);
	}

}

static void memac_receive(void *opaque){
	FMan_T2080_device *dev = ((conf_object_t *)opaque)->obj;
    struct ethhdr *hdr_ether;
	int size;
	unsigned char dest_ip[4] = {0xc0, 0xa8, 0x5, 0x14};//192.168.5.20
	unsigned char self_ip[4] = {0xc0, 0xa8, 0x5, 0xa};//192.168.5.10
	if (dev->net_iface == NULL) return;

	if ((size = dev->net_iface->net_read(NET_DEVICE(dev), dev->rx_data, 1522)) == -1)
		return;
	if ((dev->PHY.PHY_SR & 0x4) ==0)
			return;
	hdr_ether = (struct ethhdr *)dev->rx_data;

	if (ISBROADCAST(hdr_ether->h_dest)){
		rx_init_frame(dev,dev->rx_data,size);
	}else if (ISMULTICAST(hdr_ether->h_dest)){
		
	}else if (ISUNICAST(hdr_ether->h_dest))
	{
		if(memcmp(hdr_ether->h_dest,dev->macInfo,6) == 0)
			rx_init_frame(dev,dev->rx_data,size);
		
	}
	
}


 static void memac_transmit(void *opaque){
	FMan_T2080_device *dev = ((conf_object_t *)opaque)->obj;
	get_page_t *bufptr_page;
	uint8_t *bufptr_pbuffer,*dsec_pbuffer;
	int i;
	for (i=0;i<8;i++){
		uint8_t verb = dev->QPORTAL_ENQUEUE[i].qportalVerb & 0x1;
		/*if frame enqueue complete, send frame data*/
		if(verb == 0x1){
			dev->tx_buffer_len = dev->QPORTAL_ENQUEUE[i].qportalFd.qportalOffLen;
			uint64_t addr = (((uint64_t)dev->QPORTAL_ENQUEUE[i].qportalFd.qportalAddrHi) << 32) | (dev->QPORTAL_ENQUEUE[i].qportalFd.qportalAddrLo & 0xFFFFFFFF);
			bufptr_page = dev->memory_iface->get_page(dev->memory, addr);
			bufptr_pbuffer = (uint8_t *)(bufptr_page->page_base + (addr & 0xfff));
			memcpy(dev->tx_data, bufptr_pbuffer,dev->tx_buffer_len);
			if (dev->tx_buffer_len >= 0x2a){
				dev->net_iface->net_write(NET_DEVICE(dev), dev->tx_data, dev->tx_buffer_len);
				
				if(dev->QPORTAL_ENQUEUE[i].qportalFd.qportalCmd & 0x04000000){
					skyeye_log(Error_log, __FUNCTION__, "Unsupported fd format!\n");
					return;
				}
				if(dev->QPORTAL_ENQUEUE[i].qportalFd.qportalCmd & 0x02000000){
					skyeye_log(Error_log, __FUNCTION__, "Large frame error !\n");
					return;
				}
				if(dev->QPORTAL_ENQUEUE[i].qportalFd.qportalCmd & 0x01000000){
					skyeye_log(Error_log, __FUNCTION__, " DMA error\n");
					return;
				}
				
				memset(&dev->QPORTAL_ENQUEUE[i],0,sizeof(struct qportal_enqueue));
			}
			else{
				printf("error len\n");
				break;
			}
		}
		
	}
	
 }

static exception_t FMan_T2080_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	FMan_T2080_device *dev = opaque->obj;
	if(offset>=0x00000 && offset<=0x7FFFF){//FMan_memory
		*(uint32_t *)buf = dev->FMan_memory[offset-0x00000];
	}
	else if(offset>=0x80000 && offset<=0xBFFFF){//FMHP
		/*4KB common*/
		if (offset-0x80000 >= 0x0000 && offset-0x80000 <=0x03FF)
			*(uint32_t *)buf = dev->FMHP.BMI_common[offset-0x80000];
		else if (offset-0x80000 >= 0x0400 && offset-0x80000 <=0x07FF)
			*(uint32_t *)buf = dev->FMHP.QMI_common[offset-0x80000];
		else if (offset-0x80000 >0x0FFF){
			/*1-63 4KB */
			uint32_t j = offset-0x80000;
			uint32_t port_id = j/0x1000;
			if ((j%0x1000)>= 0x000 && (j%0x1000) <= 0x3FF)
				*(uint32_t *)buf = dev->FMHP.id_fmhp[port_id-1].BMI[j%0x1000];
			else if ((j%0x1000) >= 0x400 && (j%0x1000) <= 0x7FF)
				*(uint32_t *)buf = dev->FMHP.id_fmhp[port_id-1].QMI[j%0x1000];
			else if ((j%0x1000) >= 0x800 && (j%0x1000) <= 0xBFF)
				*(uint32_t *)buf = dev->FMHP.id_fmhp[port_id-1].Parser[j%0x1000];
			
		}
	}
	else if(offset>=0xC0000 && offset<=0xC0FFF){
		*(uint32_t *)buf = dev->Policer[offset-0xC0000];
	}
	else if(offset>=0xC1000 && offset<=0xC1FFF){
		*(uint32_t *)buf = dev->KeyGen[offset-0xC1000] ;
	}
	else if(offset>=0xC2000 && offset<=0xC2FFF){
		*(uint32_t *)buf = dev->DMA[offset-0xC2000];
	}
	else if(offset>=0xC3000 && offset<=0xC3FFF){
		*(uint32_t *)buf = dev->FPM[offset-0xC3000] ; 
	}
	else if(offset>=0xC4000 && offset<=0xC4FFF){
		/*read FMCDDATA*/
		if((offset-0xC4000)==0x4){
			uint32_t addr = dev->FMIR.FMCDADDR & 0x7FFFFFFF;
			addr &= 0xFFFFFFFC;
			*(uint32_t *)buf = dev->FMIR.FMCDDATA[addr ];
		}
		else if((offset-0xC4000)==0x0)
			*(uint32_t *)buf =dev->FMIR.FMCDADDR;
		else if((offset-0xC4000)==0x8)
			*(uint32_t *)buf =dev->FMIR.FMCDREADY;
	}
	else if(offset>=0xC7000 && offset<=0xC7FFF){
		*(uint32_t *)buf = dev->Soft_Parser[offset-0xC7000];
	}
	else if(offset>=0xDB000 && offset<=0xDBFFF){
		*(uint32_t *)buf = dev->Congestion_groups[offset-0xDB000];
	}
	else if(offset>=0xDC000 && offset<=0xDCFFF){
		*(uint32_t *)buf = dev->Virtual_Storage_Profile[offset-0xDC000];
	}
	else if (offset>=0xE0000 && offset <=0xF3FFF){
		uint8_t memac_i=0;
		uint32_t j=offset-0xE0000;
		memac_i = j/0x2000;//第i块网卡
		if ((j%0x1000) >= 0x000 && (j%0x1000) <= 0x7FF){
			*(uint32_t *)buf = dev->EMAC[memac_i].mEMAC[j%0x1000];
		}
		else if((j%0x1000) >= 0x800 && (j%0x1000) <= 0xFFF)
			*(uint32_t *)buf =dev->EMAC[memac_i].MACsec[j%0x1000];
		else
			*(uint32_t *)buf =dev->EMAC[memac_i].MDIO[j%0x1000];

	}
	else if(offset>=0xFC000 && offset<=0xFCFFF){
		/*read PHY*/
		if (offset-0xFC000 == 0x38){//MDIO_data
			uint8_t MDIO_CTRL_PORT_ADDR = dev->MDIO1[0x34] & 0x0000003E0;
			uint8_t DEV_ADDR = dev->MDIO1[0x34] & 0x1F ;
			if (MDIO_CTRL_PORT_ADDR == 0x20){
				if (DEV_ADDR == 0x0)
					*(uint32_t *)buf = dev->PHY.PHY_CR ;
				else if (DEV_ADDR == 0x1)
					*(uint32_t *)buf = dev->PHY.PHY_SR ;
				else if (DEV_ADDR == 0x2)
					*(uint32_t *)buf = dev->PHY.PHY1_Register;
				else if (DEV_ADDR == 0x3)
					*(uint32_t *)buf = dev->PHY.PHY2_Register;
				else if (DEV_ADDR == 0x4)
					*(uint32_t *)buf = dev->PHY.AAR ;
				else if (DEV_ADDR == 0x5)
					*(uint32_t *)buf = dev->PHY.ALPAR;
				else if (DEV_ADDR == 0x6)
					*(uint32_t *)buf = dev->PHY.AER ;
				else{
					skyeye_log(Error_log, __FUNCTION__, " PHY read error\n");
				}
			}
		}
		else
			*(uint32_t *)buf = dev->MDIO1[offset-0xFC000];
	}
	else if(offset>=0xFD000 && offset<=0xFDFFF){
		*(uint32_t *)buf = dev->MDIO2[offset-0xFD000];
	}
	else if(offset>=0xFE000 && offset<=0xFEFFF){
		*(uint32_t *)buf = dev->IEEE_1588[offset-0xFE000] ;
	}
	else{
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in FMan_T2080\n", offset);
	}
	return 0;
}
static exception_t FMan_T2080_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	FMan_T2080_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	if(offset>=0x00000 && offset<=0x7FFFF){
		dev->FMan_memory[offset-0x00000] = val ;
	}
	else if(offset>=0x80000 && offset<=0xBFFFF){
		if (offset-0x80000 >= 0x0000 && offset-0x80000 <=0x03FF)
			dev->FMHP.BMI_common[offset-0x80000]= val;
		else if (offset-0x80000 >= 0x0400 && offset-0x80000 <=0x07FF)
			dev->FMHP.QMI_common[offset-0x80000] = val;
		else if (offset-0x80000 > 0x0FFF){
			uint32_t j = offset-0x80000;
			uint32_t port_id = j/0x1000;
			if ((j%0x1000)>= 0x000 && (j%0x1000) <= 0x3FF)
				dev->FMHP.id_fmhp[port_id-1].BMI[j%0x1000] = val;
			else if ((j%0x1000) >= 0x400 && (j%0x1000) <= 0x7FF){
				dev->FMHP.id_fmhp[port_id-1].QMI[j%0x1000] = val;
			}
			else if ((j%0x1000) >= 0x800 && (j%0x1000) <= 0xBFF)
				dev->FMHP.id_fmhp[port_id-1].Parser[j%0x1000] = val;
		}
	}
	else if(offset>=0xC0000 && offset<=0xC0FFF){
		int i,entry,byte;
		/*FMan Policer Profile Action Register clear GO_bit and PSI_bit and set GO_bit*/
		if((offset-0xC0000) == 0x08c){
			for (entry=0;entry<256;entry++){
				for (byte=0;byte<16;byte++){
					dev->PRAM[entry][byte] = val ;
				}
			}
			CLR_BIT(dev->Policer[0x08c],0xA0000000);
			SET_BIT(dev->Policer[0x008],0x80000000);
		}
		/*FMan Policer Event Register w1c*/
		else if(offset-0xC0000 == 0x008){
			for (i=0;i<32;i++) {
				if ((val >> i) & 0x1) {
					CLR_BIT(dev->Policer[0x008], 0x1 << i);//如果该位是1，就强制清除该位
				}
			}
		}
		else{
			dev->Policer[offset-0xC0000] = val;
		}
	}else if(offset>=0xC1000 && offset<=0xC1FFF){
		dev->KeyGen[offset-0xC1000] = val;
	}
	else if(offset>=0xC2000 && offset<=0xC2FFF){
		dev->DMA[offset-0xC2000] = val ;
	}
	else if(offset>=0xC3000 && offset<=0xC3FFF){
		/*FMan Reset Command*/
		if((offset-0xC3000) == 0xCC){
			if (dev->FPM[0xcc] & 0x80000000){
				// Do reset operation
				CLR_BIT(dev->FPM[0xcc],0x80000000);
			}
			
		}
		else 
			dev->FPM[offset-0xC3000] = val;
	}
	else if(offset>=0xC4000 && offset<=0xC4FFF){
		if((offset - 0xC4000) == 0x4){ // IDATA
			uint32_t addr = dev->FMIR.FMCDADDR & 0x7FFFFFFF;
			addr &= 0xFFFFFFFC;
			dev->FMIR.FMCDDATA[addr ] = val;
			if(dev->FMIR.FMCDADDR & 0x80000000)
				dev->FMIR.FMCDADDR += 4;
		}
		else if ((offset- 0xC4000) ==0x0 ){
			dev->FMIR.FMCDADDR = val;
		}
	}
	else if(offset>=0xC7000 && offset<=0xC7FFF){
		dev->Soft_Parser[offset-0xC7000] = val;
	}
	else if(offset>=0xDB000 && offset<=0xDBFFF){
		dev->Congestion_groups[offset-0xDB000] = val ;
	}
	else if(offset>=0xDC000 && offset<=0xDCFFF){
		dev->Virtual_Storage_Profile[offset-0xDC000] = val ;
	}
	else if (offset>=0xE0000 && offset <=0xF3FFF){
		uint8_t memac_i=0;
		uint32_t j=offset-0xE0000;
		memac_i = j/0x2000;//第i块网卡
		if ((j%0x1000) >= 0x000 && (j%0x1000) <= 0x7FF){
			dev->EMAC[memac_i].mEMAC[j%0x1000] = val;
			if ((j%0x1000) == 0x8){
				if (CHK_BIT(dev->EMAC[memac_i].mEMAC[0x8],0x00001000) ){
					CLR_BIT(dev->EMAC[memac_i].mEMAC[0x8],0x00001000);//clear reset 
				}
			}
			else if ((j%0x1000) == 0xc){//mac_addr0
				dev->macInfo[0] = val & 0xff;
				dev->macInfo[1] = (val >> 8) & 0xff;
				dev->macInfo[2] = (val >> 16) & 0xff;
				dev->macInfo[3] = (val >> 24) & 0xff;
			}
			else if ((j%0x1000) == 0x10){//mac_addr1
				dev->macInfo[4] = val & 0xff;
				dev->macInfo[5] = (val >> 8) & 0xff;
			}
		}
		else if((j%0x1000) >= 0x800 && (j%0x1000) <= 0xFFF)
			dev->EMAC[memac_i].MACsec[j%0x1000] =val;
		else 
			dev->EMAC[memac_i].MDIO[j%0x1000] =val;

	}
	else if(offset>=0xFC000 && offset<=0xFCFFF){
		/*write MDIO to link PHY*/
		if (offset-0xFC000 == 0x38){//QMM_MDIO_DATA
			uint8_t MDIO_CTRL_PORT_ADDR = dev->MDIO1[0x34] & 0x000003E0;
			uint8_t MDIO_CTRL_DEV_ADDR = dev->MDIO1[0x34] & 0x1F ;
			if (MDIO_CTRL_PORT_ADDR == 0x20){
				if(MDIO_CTRL_DEV_ADDR == 0x0){
					dev->PHY.PHY_CR = val;
					if(CHK_BIT(dev->PHY.PHY_CR, 0x8000)){
						// reset operation 
						PHY_regs_regs_reset(dev);
						// self clearing
						CLR_BIT( dev->PHY.PHY_CR, 0x8000);
					}
					else{
						if (CHK_BIT( dev->PHY.PHY_CR, 0x200)){
							// restart negoniation operation
							SET_BIT(dev->PHY.PHY_SR,0x4);//link_bit
							// self clearing
							CLR_BIT( dev->PHY.PHY_CR, 0x200);
						}
						if (CHK_BIT(dev->PHY.PHY_CR, 0x1000)){
							SET_BIT(dev->PHY.PHY_SR,0x20);//negotiation_complete
							SET_BIT(dev->PHY.PHY_SR,0x4);//link_bit
						}
						dev->PHY.PHY_CR = val;
					}
				}
				else if(MDIO_CTRL_DEV_ADDR == 0x1)
					dev->PHY.PHY_SR = val;
				else if(MDIO_CTRL_DEV_ADDR == 0x2)
					dev->PHY.PHY1_Register = val;
				else if(MDIO_CTRL_DEV_ADDR == 0x3)
					dev->PHY.PHY2_Register = val;
				else if(MDIO_CTRL_DEV_ADDR == 0x4)
					dev->PHY.AAR = val;
				else if(MDIO_CTRL_DEV_ADDR == 0x5)
					dev->PHY.ALPAR = val;
				else if(MDIO_CTRL_DEV_ADDR == 0x6)
					dev->PHY.AER = val;
				else
				skyeye_log(Error_log, __FUNCTION__, " PHY write error\n");
			}
			
		}
		else 
			dev->MDIO1[offset-0xFC000] = val ;
		
	}
	else if(offset>=0xFD000 && offset<=0xFDFFF){
		dev->MDIO2[offset-0xFD000] = val ;
	}
	else if(offset>=0xFE000 && offset<=0xFEFFF){
		dev->IEEE_1588[offset-0xFE000] = val ;
	}
	else{
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in FMan_T2080\n", offset);
	}
	
	return 0;
}
static char* FMan_T2080_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	
	return 0;
}
static exception_t FMan_T2080_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	return 0;
}
static uint32_t FMan_T2080_get_regnum(conf_object_t *conf_obj)
{
	return 0;
}
static uint32_t FMan_T2080_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return 0;
}
static uint32_t FMan_T2080_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	return 0;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	FMan_T2080_device *dev =  obj->obj;
	dev->signal= obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
	if(dev->signal_iface == NULL){
		skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
	}

	return No_exp;
}
 
static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	FMan_T2080_device *dev =  obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}

static exception_t general_net_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	FMan_T2080_device *dev = obj->obj;

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
	uint32_t MII_SR_LINK_STATUS = dev->PHY.PHY_SR & 0x4;
	if (dev->net_sim_isok)
		dev->PHY.PHY_SR |= MII_SR_LINK_STATUS;
	else
		dev->PHY.PHY_SR &= ~MII_SR_LINK_STATUS;

	return No_exp;
}

static exception_t general_net_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	FMan_T2080_device *dev = obj->obj;

	*obj2 = dev->net;
	*port = NULL;
	return No_exp;
}

static exception_t get_fd(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	FMan_T2080_device *dev = obj->obj;
	*obj2 = dev->fd;
	*port = NULL;
	return No_exp;
}
static exception_t set_fd(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	FMan_T2080_device *dev = obj->obj;
	dev->fd = obj2;
	dev->fd_iface = (qportal_fd_intf *)SKY_get_iface(dev->fd, QPORTAL_FDESC_INTF_NAME);
	if (dev->fd_iface == NULL){
		if (dev->fd == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", QPORTAL_FDESC_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", QPORTAL_FDESC_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	FMan_T2080_device *dev = obj->obj;

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
	FMan_T2080_device *dev = obj->obj;

	*obj2 = dev->memory;
	*port = NULL;
	return No_exp;
}


static conf_object_t* new_FMan_T2080(const char *obj_name)
{
	FMan_T2080_device* dev = skyeye_mm_zero(sizeof(FMan_T2080_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->tx_data = skyeye_mm_zero(1522);
	dev->rx_data = skyeye_mm_zero(1522);
	FMan_T2080_regs_init(dev);
	PHY_regs_regs_reset(dev);

	system_register_timer_handler(dev->obj, 100, (time_sched_t)memac_receive, SCHED_US|SCHED_NORMAL);
	system_register_timer_handler(dev->obj, 100, (time_sched_t)memac_transmit, SCHED_US|SCHED_NORMAL);
	return dev->obj;
}

static exception_t config_FMan_T2080(conf_object_t* opaque)
{
	FMan_T2080_device *dev =  opaque->obj;

	dev->fd_iface->init_fd(dev->fd, &(dev->QPORTAL_ENQUEUE), &(dev->QPORTAL_DEQUEUE));

}

static exception_t free_FMan_T2080(conf_object_t* opaque)
{
	FMan_T2080_device *dev =  opaque->obj;
	skyeye_free(dev->tx_data);
	skyeye_free(dev->rx_data);

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}


void init_FMan_T2080(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "FMan_T2080",
		.class_desc = "FMan_T2080",
		.new_instance = new_FMan_T2080,
		.config_instance = config_FMan_T2080,
		.free_instance = free_FMan_T2080,
		.module_type = SKYML_ONCHIP(SC_t2080),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = FMan_T2080_read,
		.write = FMan_T2080_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = FMan_T2080_get_regvalue_by_id,
		.get_regname_by_id = FMan_T2080_get_regname_by_id,
		.set_regvalue_by_id = FMan_T2080_set_regvalue_by_id,
		.get_regnum = FMan_T2080_get_regnum,
		.get_regid_by_name = FMan_T2080_get_regid_by_name
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
		(struct ConnectDescription) {
			.name = SKYEYE_NET_INTF_NAME,
			.set = general_net_space_set,
			.get = general_net_space_get,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = signal_set,
			.get = signal_get,
		},
		(struct ConnectDescription ){
			.name = QPORTAL_FDESC_INTF_NAME,
			.set = set_fd,
			.get = get_fd,
		},
		(struct ConnectDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.set = general_memory_space_set,
			.get = general_memory_space_get,
		}
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	
}
