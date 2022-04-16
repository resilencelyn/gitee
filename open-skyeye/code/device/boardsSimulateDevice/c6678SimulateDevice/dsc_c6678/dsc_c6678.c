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
#include "dsc_c6678.h"

void Check_BootCfg(dsc_c6678_device *dev)
{
	if ((dev->regs.kick0 == KICK0_KEY) && (dev->regs.kick1 == KICK1_KEY)) {
		dev->BootCfgKicker_Status = BOOTCFGKICKER_UNLOCK;
		//printf("		BOOTCFGKICKER_UNLOCK\n\n");
	} else {
		dev->BootCfgKicker_Status = BOOTCFGKICKER_LOCK;
		//printf("		BOOTCFGKICKER_LOCK\n\n");
	}
}

void call_signal_func(conf_object_t* opaque, int int_number,int core_number){
	dsc_c6678_device *dev = opaque->obj;
	general_signal_intf* general_signal = dev->general_signal;

	if (general_signal) {
		general_signal->event_signal(dev->core, int_number, &core_number);
	}

	return;
}

static exception_t dsc_c6678_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	dsc_c6678_device *dev = opaque->obj;
	switch (offset) {
	case JTAGID:
		 *(uint32_t *)buf = dev->regs.jtagid;
		break;
	case DEVSTAT:
		*(uint32_t *)buf = dev->regs.devstat;
		break;
	case KICK0:
		*(uint32_t *)buf = dev->regs.kick0;
		break;
	case KICK1:
		*(uint32_t *)buf = dev->regs.kick1;
		break;
	case DSP_BOOT_ADDR0:
		*(uint32_t *)buf = dev->regs.dsp_boot_addr0;
		break;
	case DSP_BOOT_ADDR1:
		*(uint32_t *)buf = dev->regs.dsp_boot_addr1;
		break;
	case DSP_BOOT_ADDR2:
		*(uint32_t *)buf = dev->regs.dsp_boot_addr2;
		break;
	case DSP_BOOT_ADDR3:
		*(uint32_t *)buf = dev->regs.dsp_boot_addr3;
		break;
	case DSP_BOOT_ADDR4:
		*(uint32_t *)buf = dev->regs.dsp_boot_addr4;
		break;
	case DSP_BOOT_ADDR5:
		*(uint32_t *)buf = dev->regs.dsp_boot_addr5;
		break;
	case DSP_BOOT_ADDR6:
		*(uint32_t *)buf = dev->regs.dsp_boot_addr6;
		break;
	case DSP_BOOT_ADDR7:
		*(uint32_t *)buf = dev->regs.dsp_boot_addr7;
		break;
	case MACID1:
		*(uint32_t *)buf = dev->regs.macid1;
		break;
	case MACID2:
		*(uint32_t *)buf = dev->regs.macid2;
		break;
	case LRSTNMIPINSTAT_CLR:
		*(uint32_t *)buf = dev->regs.lrstnmipinstat_clr;
		break;
	case RESET_STAT_CLR:
		*(uint32_t *)buf = dev->regs.reset_stat_clr;
		break;
	case BOOTCOMPLETE:
		*(uint32_t *)buf = dev->regs.bootcomplete;
		break;
	case RESET_STAT:
		*(uint32_t *)buf = dev->regs.reset_stat;
		break;
	case LRSTNMIPINSTAT:
		*(uint32_t *)buf = dev->regs.lrstnmipinstat;
		break;
	case DEVCFG:
		*(uint32_t *)buf = dev->regs.devcfg;
		break;
	case PWRSTATECTL:
		*(uint32_t *)buf = dev->regs.pwrstatectl;
		break;
	case SRIO_SERDES_STS:
		*(uint32_t *)buf = dev->regs.srio_serdes_sts;
		break;
	case SMGII_SERDES_STS:
		*(uint32_t *)buf = dev->regs.smgii_serdes_sts;
		break;
	case PCIE_SERDES_STS:
		*(uint32_t *)buf = dev->regs.pcie_serdes_sts;
		break;
	case HYPERLINK_SERDES_STS:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_sts;
		break;
	case NMIGR0:
		*(uint32_t *)buf = dev->regs.nmigr[0];
		break;
	case NMIGR1:
		*(uint32_t *)buf = dev->regs.nmigr[1];
		break;
	case NMIGR2:
		*(uint32_t *)buf = dev->regs.nmigr[2];
		break;
	case NMIGR3:
		*(uint32_t *)buf = dev->regs.nmigr[3];
		break;
	case NMIGR4:
		*(uint32_t *)buf = dev->regs.nmigr[4];
		break;
	case NMIGR5:
		*(uint32_t *)buf = dev->regs.nmigr[5];
		break;
	case NMIGR6:
		*(uint32_t *)buf = dev->regs.nmigr[6];
		break;
	case NMIGR7:
		*(uint32_t *)buf = dev->regs.nmigr[7];
		break;
	case IPCGR0:
		*(uint32_t *)buf = dev->regs.ipcgr[0] | 0xfffffff0;
		break;
	case IPCGR1:
		*(uint32_t *)buf = dev->regs.ipcgr[1] | 0xfffffff0;
		break;
	case IPCGR2:
		*(uint32_t *)buf = dev->regs.ipcgr[2] | 0xfffffff0;
		break;
	case IPCGR3:
		*(uint32_t *)buf = dev->regs.ipcgr[3] | 0xfffffff0;
		break;
	case IPCGR4:
		*(uint32_t *)buf = dev->regs.ipcgr[4] | 0xfffffff0;
		break;
	case IPCGR5:
		*(uint32_t *)buf = dev->regs.ipcgr[5] | 0xfffffff0;
		break;
	case IPCGR6:
		*(uint32_t *)buf = dev->regs.ipcgr[6] | 0xfffffff0;
		break;
	case IPCGR7:
		*(uint32_t *)buf = dev->regs.ipcgr[7] | 0xfffffff0;
		break;
	case IPCGRH:
		*(uint32_t *)buf = dev->regs.ipcgrh;
		break;
	case IPCAR0:
		*(uint32_t *)buf = dev->regs.ipcar[0] | 0xfffffff0;
		break;
	case IPCAR1:
		*(uint32_t *)buf = dev->regs.ipcar[1] | 0xfffffff0;
		break;
	case IPCAR2:
		*(uint32_t *)buf = dev->regs.ipcar[2] | 0xfffffff0;
		break;
	case IPCAR3:
		*(uint32_t *)buf = dev->regs.ipcar[3] | 0xfffffff0;
		break;
	case IPCAR4:
		*(uint32_t *)buf = dev->regs.ipcar[4] | 0xfffffff0;
		break;
	case IPCAR5:
		*(uint32_t *)buf = dev->regs.ipcar[5] | 0xfffffff0;
		break;
	case IPCAR6:
		*(uint32_t *)buf = dev->regs.ipcar[6] | 0xfffffff0;
		break;
	case IPCAR7:
		*(uint32_t *)buf = dev->regs.ipcar[7] | 0xfffffff0;
		break;
	case IPCARH:
		*(uint32_t *)buf = dev->regs.ipcarh;
		break;
	case TINPSEL:
		*(uint32_t *)buf = dev->regs.tinpsel;
		break;
	case TOUTPSEL:
		*(uint32_t *)buf = dev->regs.toutpsel;
		break;
	case RSTMUX0:
		*(uint32_t *)buf = dev->regs.rstmux[0];
		break;
	case RSTMUX1:
		*(uint32_t *)buf = dev->regs.rstmux[1];
		break;
	case RSTMUX2:
		*(uint32_t *)buf = dev->regs.rstmux[2];
		break;
	case RSTMUX3:
		*(uint32_t *)buf = dev->regs.rstmux[3];
		break;
	case RSTMUX4:
		*(uint32_t *)buf = dev->regs.rstmux[4];
		break;
	case RSTMUX5:
		*(uint32_t *)buf = dev->regs.rstmux[5];
		break;
	case RSTMUX6:
		*(uint32_t *)buf = dev->regs.rstmux[6];
		break;
	case RSTMUX7:
		*(uint32_t *)buf = dev->regs.rstmux[7];
		break;
	case MAINPLLCTL0:
		*(uint32_t *)buf = dev->regs.mainpllctl0.all;
		break;
	case MAINPLLCTL1:
		*(uint32_t *)buf = dev->regs.mainpllctl1.all;
		break;
	case DDR3PLLCTL0:
		*(uint32_t *)buf = dev->regs.ddr3pllctl0;
		break;
	case DDR3PLLCTL1:
		*(uint32_t *)buf = dev->regs.ddr3pllctl1;
		break;
	case PASSPLLCTL0:
		*(uint32_t *)buf = dev->regs.passpllctl0;
		break;
	case PASSPLLCTL1:
		*(uint32_t *)buf = dev->regs.passpllctl1;
		break;
	case SGMII_SERDES_CFGPLL:
		*(uint32_t *)buf = dev->regs.sgmii_serdes_cfgpll;
		break;
	case SGMII_SERDES_CFGRX0:
		*(uint32_t *)buf = dev->regs.sgmii_serdes_cfgrx0;
		break;
	case SGMII_SERDES_CFGTX0:
		*(uint32_t *)buf = dev->regs.sgmii_serdes_cfgtx0;
		break;
	case SGMII_SERDES_CFGRX1:
		*(uint32_t *)buf = dev->regs.sgmii_serdes_cfgrx1;
		break;
	case SGMII_SERDES_CFGTX1:
		*(uint32_t *)buf = dev->regs.sgmii_serdes_cfgtx1;
		break;
	case PCIE_SERDES_CFGPLL:
		*(uint32_t *)buf = dev->regs.pcie_serdes_cfgpll;
		break;
	case SRIO_SERDES_CFGPLL:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgpll;
		break;
	case SRIO_SERDES_CFGRX0:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgrx0;
		break;
	case SRIO_SERDES_CFGTX0:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgtx0;
		break;
	case SRIO_SERDES_CFGRX1:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgrx1;
		break;
	case SRIO_SERDES_CFGTX1:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgtx1;
		break;
	case SRIO_SERDES_CFGRX2:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgrx2;
		break;
	case SRIO_SERDES_CFGTX2:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgtx2;
		break;
	case SRIO_SERDES_CFGRX3:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgrx3;
		break;
	case SRIO_SERDES_CFGTX3:
		*(uint32_t *)buf = dev->regs.srio_serdes_cfgtx3;
		break;
	case DSP_SUSP_CTL:
		*(uint32_t *)buf = dev->regs.dsp_susp_ctl;
		break;
	case HYPERLINK_SERDES_CFGPLL:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgpll;
		break;
	case HYPERLINK_SERDES_CFGRX0:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgrx0;
		break;
	case HYPERLINK_SERDES_CFGTX0:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgtx0;
		break;
	case HYPERLINK_SERDES_CFGRX1:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgrx1;
		break;
	case HYPERLINK_SERDES_CFGTX1:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgtx1;
		break;
	case HYPERLINK_SERDES_CFGRX2:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgrx2;
		break;
	case HYPERLINK_SERDES_CFGTX2:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgtx2;
		break;
	case HYPERLINK_SERDES_CFGRX3:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgrx3;
		break;
	case HYPERLINK_SERDES_CFGTX3:
		*(uint32_t *)buf = dev->regs.hyperlink_serdes_cfgtx3;
		break;
	case DEVSPEED:
		*(uint32_t *)buf = dev->regs.devspeed;
		break;
	case CHIP_MISC_CTL:
		*(uint32_t *)buf = dev->regs.chip_misc_ctl;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in dsc_c6678\n", offset);
		break;
	}
	//printf("%s  offset: 0x02620%03x buf: 0x%08x\n", __FUNCTION__, offset, *(uint32_t *)buf);
	return No_exp;
}

static exception_t dsc_c6678_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	dsc_c6678_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	int core_number = -1;
	int int_number = dev->interrupt_number;

	//printf("%s offset: 0x02620%03x buf: 0x%08x\n", __FUNCTION__, offset, val);

	switch (offset) {
	case JTAGID:
		// 寄存器不可写
		// dev->regs.jtagid = val;
		break;
	case DEVSTAT:
		//printf("\n===== Set DEVSTAT: 0x%X\n", val);
		dev->regs.devstat = val;
		break;
	case KICK0:
		dev->regs.kick0 = val;
		Check_BootCfg(dev);
		break;
	case KICK1:
		dev->regs.kick1 = val;
		Check_BootCfg(dev);
		break;
	case DSP_BOOT_ADDR0:
		dev->regs.dsp_boot_addr0 = val;
		break;
	case DSP_BOOT_ADDR1:
		dev->regs.dsp_boot_addr1 = val;
		break;
	case DSP_BOOT_ADDR2:
		dev->regs.dsp_boot_addr2 = val;
		break;
	case DSP_BOOT_ADDR3:
		dev->regs.dsp_boot_addr3 = val;
		break;
	case DSP_BOOT_ADDR4:
		dev->regs.dsp_boot_addr4 = val;
		break;
	case DSP_BOOT_ADDR5:
		dev->regs.dsp_boot_addr5 = val;
		break;
	case DSP_BOOT_ADDR6:
		dev->regs.dsp_boot_addr6 = val;
		break;
	case DSP_BOOT_ADDR7:
		dev->regs.dsp_boot_addr7 = val;
		break;
	case MACID1:
		dev->regs.macid1 = val;
		break;
	case MACID2:
		dev->regs.macid2 = val;
		break;
	case LRSTNMIPINSTAT_CLR:
		dev->regs.lrstnmipinstat_clr = val;
		break;
	case RESET_STAT_CLR:
		dev->regs.reset_stat_clr = val;
		break;
	case BOOTCOMPLETE:
		dev->regs.bootcomplete = val;
		break;
	case RESET_STAT:
		dev->regs.reset_stat = val;
		break;
	case LRSTNMIPINSTAT:
		dev->regs.lrstnmipinstat = val;
		break;
	case DEVCFG:
		dev->regs.devcfg = val;
		break;
	case PWRSTATECTL:
		dev->regs.pwrstatectl = val;
		break;
	case SRIO_SERDES_STS:
		dev->regs.srio_serdes_sts = val;
		break;
	case SMGII_SERDES_STS:
		dev->regs.smgii_serdes_sts = val;
		break;
	case PCIE_SERDES_STS:
		dev->regs.pcie_serdes_sts = val;
		break;
	case HYPERLINK_SERDES_STS:
		dev->regs.hyperlink_serdes_sts = val;
		break;
	case NMIGR0:
		dev->regs.nmigr[0] = val;
		break;
	case NMIGR1:
		dev->regs.nmigr[1] = val;
		break;
	case NMIGR2:
		dev->regs.nmigr[2] = val;
		break;
	case NMIGR3:
		dev->regs.nmigr[3] = val;
		break;
	case NMIGR4:
		dev->regs.nmigr[4] = val;
		break;
	case NMIGR5:
		dev->regs.nmigr[5] = val;
		break;
	case NMIGR6:
		dev->regs.nmigr[6] = val;
		break;
	case NMIGR7:
		dev->regs.nmigr[7] = val;
		break;
	case IPCGR0:
		dev->regs.ipcgr[0] |= val;
		dev->regs.ipcar[0] |= (val | 0xfffffff0); 
		core_number = (dev->regs.ipcgr[0] & 0x1) ? 0 : -1;
		break;
	case IPCGR1:
		dev->regs.ipcgr[1] |= val;
		dev->regs.ipcar[1] |= (val | 0xfffffff0); 
		core_number  = (dev->regs.ipcgr[1] & 0x1) ? 1 : -1;
		break;
	case IPCGR2:
		dev->regs.ipcgr[2] |= val;
		dev->regs.ipcar[2] |= (val | 0xfffffff0); 
		core_number  = (dev->regs.ipcgr[2] & 0x1) ? 2 : -1;
		break;
	case IPCGR3:
		dev->regs.ipcgr[3] |= val;
		dev->regs.ipcar[3] |= (val | 0xfffffff0); 
		core_number  = (dev->regs.ipcgr[3] & 0x1) ? 3 : -1;
		break;
	case IPCGR4:
		dev->regs.ipcgr[4] |= val;
		dev->regs.ipcar[4] |= (val | 0xfffffff0); 
		core_number  = (dev->regs.ipcgr[4] & 0x1) ? 4 : -1;
		break;
	case IPCGR5:
		dev->regs.ipcgr[5] |= val;
		dev->regs.ipcar[5] |= (val | 0xfffffff0); 
		core_number  = (dev->regs.ipcgr[5] & 0x1) ? 5 : -1;
		break;
	case IPCGR6:
		dev->regs.ipcgr[6] |= val;
		dev->regs.ipcar[6] |= (val | 0xfffffff0); 
		core_number  = (dev->regs.ipcgr[6] & 0x1) ? 6 : -1;
		break;
	case IPCGR7:
		dev->regs.ipcgr[7] |= val;
		dev->regs.ipcar[7] |= (val | 0xfffffff0); 
		core_number  = (dev->regs.ipcgr[7] & 0x1) ? 7 : -1;
		break;
	case IPCGRH:
		dev->regs.ipcgrh = val;
		break;
	case IPCAR0:
		dev->regs.ipcar[0] &= ~(val | 0xfffffff0);
		dev->regs.ipcgr[0] &= ~(val | 0xfffffff0);
		break;
	case IPCAR1:
		dev->regs.ipcar[1] &= ~(val | 0xfffffff0);
		dev->regs.ipcgr[1] &= ~(val | 0xfffffff0);
		break;
	case IPCAR2:
		dev->regs.ipcar[2] &= ~(val | 0xfffffff0);
		dev->regs.ipcgr[2] &= ~(val | 0xfffffff0);
		break;
	case IPCAR3:
		dev->regs.ipcar[3] &= ~(val | 0xfffffff0);
		dev->regs.ipcgr[3] &= ~(val | 0xfffffff0);
		break;
	case IPCAR4:
		dev->regs.ipcar[4] &= ~(val | 0xfffffff0);
		dev->regs.ipcgr[4] &= ~(val | 0xfffffff0);
		break;
	case IPCAR5:
		dev->regs.ipcar[5] &= ~(val | 0xfffffff0);
		dev->regs.ipcgr[5] &= ~(val | 0xfffffff0);
		break;
	case IPCAR6:
		dev->regs.ipcar[6] &= ~(val | 0xfffffff0);
		dev->regs.ipcgr[6] &= ~(val | 0xfffffff0);
		break;
	case IPCAR7:
		dev->regs.ipcar[7] &= ~(val | 0xfffffff0);
		dev->regs.ipcgr[7] &= ~(val | 0xfffffff0);
		break;
	case IPCARH:
		dev->regs.ipcarh = val;
		break;
	case TINPSEL:
		dev->regs.tinpsel = val;
		break;
	case TOUTPSEL:
		dev->regs.toutpsel = val;
		break;
	case RSTMUX0:
		dev->regs.rstmux[0] = val;
		break;
	case RSTMUX1:
		dev->regs.rstmux[1] = val;
		break;
	case RSTMUX2:
		dev->regs.rstmux[2] = val;
		break;
	case RSTMUX3:
		dev->regs.rstmux[3] = val;
		break;
	case RSTMUX4:
		dev->regs.rstmux[4] = val;
		break;
	case RSTMUX5:
		dev->regs.rstmux[5] = val;
		break;
	case RSTMUX6:
		dev->regs.rstmux[6] = val;
		break;
	case RSTMUX7:
		dev->regs.rstmux[7] = val;
		break;
	case MAINPLLCTL0:
		dev->regs.mainpllctl0.all = val;
		break;
	case MAINPLLCTL1:
		dev->regs.mainpllctl1.all = val;
		//printf("		MAINPLLCTL1: 0x%x ENSAT: 0x%x\n\n", val, dev->regs.mainpllctl1.ENSAT);
		break;
	case DDR3PLLCTL0:
		dev->regs.ddr3pllctl0 = val;
		break;
	case DDR3PLLCTL1:
		dev->regs.ddr3pllctl1 = val;
		break;
	case PASSPLLCTL0:
		dev->regs.passpllctl0 = val;
		break;
	case PASSPLLCTL1:
		dev->regs.passpllctl1 = val;
		break;
	case SGMII_SERDES_CFGPLL:
		dev->regs.sgmii_serdes_cfgpll = val;
		break;
	case SGMII_SERDES_CFGRX0:
		dev->regs.sgmii_serdes_cfgrx0 = val;
		break;
	case SGMII_SERDES_CFGTX0:
		dev->regs.sgmii_serdes_cfgtx0 = val;
		break;
	case SGMII_SERDES_CFGRX1:
		dev->regs.sgmii_serdes_cfgrx1 = val;
		break;
	case SGMII_SERDES_CFGTX1:
		dev->regs.sgmii_serdes_cfgtx1 = val;
		break;
	case PCIE_SERDES_CFGPLL:
		dev->regs.pcie_serdes_cfgpll = val;
		break;
	case SRIO_SERDES_CFGPLL:
		dev->regs.srio_serdes_cfgpll = val;
		break;
	case SRIO_SERDES_CFGRX0:
		dev->regs.srio_serdes_cfgrx0 = val;
		if (dev->regs.srio_serdes_cfgrx0 & 0x1) {
			printf("%s Enable this receiver 0\n", __func__);
		}
		break;
	case SRIO_SERDES_CFGTX0:
		dev->regs.srio_serdes_cfgtx0 = val;
		if (dev->regs.srio_serdes_cfgtx0 & 0x1) {
			printf("%s Enable this transmitter 0\n", __func__);
		}
		break;
	case SRIO_SERDES_CFGRX1:
		dev->regs.srio_serdes_cfgrx1 = val;
		if (dev->regs.srio_serdes_cfgrx1 & 0x1) {
			printf("%s Enable this receiver 1\n", __func__);
		}
		break;
	case SRIO_SERDES_CFGTX1:
		dev->regs.srio_serdes_cfgtx1 = val;
		if (dev->regs.srio_serdes_cfgtx1 & 0x1) {
			printf("%s Enable this transmitter 1\n", __func__);
		}
		break;
	case SRIO_SERDES_CFGRX2:
		dev->regs.srio_serdes_cfgrx2 = val;
		if (dev->regs.srio_serdes_cfgrx2 & 0x1) {
			printf("%s Enable this receiver 2\n", __func__);
		}
		break;
	case SRIO_SERDES_CFGTX2:
		dev->regs.srio_serdes_cfgtx2 = val;
		if (dev->regs.srio_serdes_cfgtx2 & 0x1) {
			printf("%s Enable this transmitter 2\n", __func__);
		}
		break;
	case SRIO_SERDES_CFGRX3:
		dev->regs.srio_serdes_cfgrx3 = val;
		if (dev->regs.srio_serdes_cfgrx3 & 0x1) {
			printf("%s Enable this receiver 3\n", __func__);
		}
		break;
	case SRIO_SERDES_CFGTX3:
		dev->regs.srio_serdes_cfgtx3 = val;
		if (dev->regs.srio_serdes_cfgtx3 & 0x1) {
			printf("%s Enable this transmitter 3\n", __func__);
		}
		break;
	case DSP_SUSP_CTL:
		dev->regs.dsp_susp_ctl = val;
		break;
	case HYPERLINK_SERDES_CFGPLL:
		dev->regs.hyperlink_serdes_cfgpll = val;
		break;
	case HYPERLINK_SERDES_CFGRX0:
		dev->regs.hyperlink_serdes_cfgrx0 = val;
		break;
	case HYPERLINK_SERDES_CFGTX0:
		dev->regs.hyperlink_serdes_cfgtx0 = val;
		break;
	case HYPERLINK_SERDES_CFGRX1:
		dev->regs.hyperlink_serdes_cfgrx1 = val;
		break;
	case HYPERLINK_SERDES_CFGTX1:
		dev->regs.hyperlink_serdes_cfgtx1 = val;
		break;
	case HYPERLINK_SERDES_CFGRX2:
		dev->regs.hyperlink_serdes_cfgrx2 = val;
		break;
	case HYPERLINK_SERDES_CFGTX2:
		dev->regs.hyperlink_serdes_cfgtx2 = val;
		break;
	case HYPERLINK_SERDES_CFGRX3:
		dev->regs.hyperlink_serdes_cfgrx3 = val;
		break;
	case HYPERLINK_SERDES_CFGTX3:
		dev->regs.hyperlink_serdes_cfgtx3 = val;
		break;
	case DEVSPEED:
		dev->regs.devspeed = val;
		break;
	case CHIP_MISC_CTL:
		dev->regs.chip_misc_ctl = val;
		break;
	default:
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in dsc_c6678\n", offset);
		break;
	}

	//when write ipcgr check interrupt  send event and clear interrupt bit
	if ((offset >= 0x240 && offset <= 0x25C) && core_number != -1) {
		dev->regs.ipcgr[core_number] |= 0xfffffff0;
		//printf("do event_signal to core %d %d\n", core_number, int_number);
		call_signal_func(opaque, int_number, core_number);
	}

	return No_exp;
}

static char* dsc_c6678_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t dsc_c6678_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	dsc_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t dsc_c6678_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}

static uint32_t dsc_c6678_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	dsc_c6678_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t dsc_c6678_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for (i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	dsc_c6678_device *dev = obj->obj;
	dev->core = obj2;
	dev->general_signal = (general_signal_intf *)SKY_get_iface(dev->core, GENERAL_SIGNAL_INTF_NAME);

	return No_exp;
}
 
static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	dsc_c6678_device *dev = obj->obj;
	*obj2 = dev->core;
	*port = NULL;
	return No_exp;
}

static conf_object_t* new_dsc_c6678(char *obj_name)
{
	dsc_c6678_device* dev = skyeye_mm_zero(sizeof(dsc_c6678_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->jtagid = 0x0009E02F;  // JTAG ID Register Value

	regs->kick0  = 0x0;	 // Kicker Mechanism Registers
	regs->kick1  = 0x0;
	dev->BootCfgKicker_Status = BOOTCFGKICKER_LOCK;

	dev->regs.pcie_serdes_cfgpll = 0x19C;
	dev->regs.srio_serdes_sts = 1;
	dev->regs.pcie_serdes_sts = 1;

	int i;
	for (i = 0; i < 8; i++) {
		regs->ipcgr[i] =0;
		regs->ipcar[i] = 0;
	}

	return dev->obj;
}

static exception_t free_dsc_c6678(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj,	 attr_value_t* idx){
	dsc_c6678_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj,	  attr_value_t* value, attr_value_t* idx){
	dsc_c6678_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static ipc_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL,set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of  timer");
}

void init_dsc_c6678(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6678_dsc",
		.class_desc = "c6678 Device State Control",
		.new_instance = new_dsc_c6678,
		.free_instance = free_dsc_c6678
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = dsc_c6678_read,
		.write = dsc_c6678_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = dsc_c6678_get_regvalue_by_id,
		.get_regname_by_id = dsc_c6678_get_regname_by_id,
		.set_regvalue_by_id = dsc_c6678_set_regvalue_by_id,
		.get_regnum = dsc_c6678_get_regnum,
		.get_regid_by_name = dsc_c6678_get_regid_by_name
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
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	ipc_register_attribute(clss);
}
