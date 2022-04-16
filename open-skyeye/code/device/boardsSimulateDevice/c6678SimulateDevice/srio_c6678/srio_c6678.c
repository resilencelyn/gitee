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
#include "srio_c6678.h"

static void general_signal_raise(conf_object_t *opaque, int int_number, int core_number)
{
	srio_c6678_device *dev = opaque->obj;
	general_signal_intf* general_signal = dev->general_signal;
	general_signal->event_signal(dev->general_core, int_number, &core_number);
}

static exception_t srio_c6678_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	srio_c6678_device *dev = opaque->obj;

	if (offset == PID) {
		*(uint32_t *)buf = dev->regs.pid;
	} else if (offset == PCR) {
		*(uint32_t *)buf = dev->regs.pcr;
	} else if (offset == PER_SET_CNTL) {
		*(uint32_t *)buf = dev->regs.per_set_cntl;
	} else if (offset == PER_SET_CNTL1) {
		*(uint32_t *)buf = dev->regs.per_set_cntl1;
	} else if (offset == GBL_EN) {
		*(uint32_t *)buf = dev->regs.gbl_en;
	} else if (offset == GBL_EN_STAT) {
		*(uint32_t *)buf = dev->regs.gbl_en_stat;
	} else if ((offset >= BLK0_EN) &&(offset <= BLK9_EN_STAT)) {
		uint32_t num = offset - BLK0_EN;
		if ((num % 8) == 0) {
			*(uint32_t *)buf = dev->regs.blk[num / 8]._en;
		} else {
			*(uint32_t *)buf = dev->regs.blk[num / 8]._en_stat;
		}
	} else if (offset == BLK9_EN_STAT) {
		*(uint32_t *)buf = dev->regs.blk9_en_stat;
	} else if ((offset >= MULTIID_REG1) &&(offset <= MULTIID_REG8)) {
		*(uint32_t *)buf = dev->regs.multiid_reg[(offset - MULTIID_REG1) / 4];
	} else if ((offset >= PF_16B_CNTL0) && (offset <= PF_8B_CNTL7)) {
		uint32_t num = offset - BLK0_EN;
		if ((num % 8) == 0) {
			*(uint32_t *)buf = dev->regs.pf_16b_cntl[num / 8];
		} else {
			*(uint32_t *)buf = dev->regs.pf_8b_cntl[num / 8];
		}
	} else if (offset == RESERVED) {
		*(uint32_t *)buf = dev->regs.reserved;
	} else if (offset == DOORBELL0_ICSR) {
		*(uint32_t *)buf = dev->regs.doorbell[0]._icsr;
	} else if (offset == DOORBELL0_ICCR) {
		*(uint32_t *)buf = dev->regs.doorbell[0]._iccr;
	} else if (offset == DOORBELL1_ICSR) {
		*(uint32_t *)buf = dev->regs.doorbell[1]._icsr;
	} else if (offset == DOORBELL1_ICCR) {
		*(uint32_t *)buf = dev->regs.doorbell[1]._iccr;
	} else if (offset == DOORBELL2_ICSR) {
		*(uint32_t *)buf = dev->regs.doorbell[2]._icsr;
	} else if (offset == DOORBELL2_ICCR) {
		*(uint32_t *)buf = dev->regs.doorbell[2]._iccr;
	} else if (offset == DOORBELL3_ICSR) {
		*(uint32_t *)buf = dev->regs.doorbell[3]._icsr;
	} else if (offset == DOORBELL3_ICCR) {
		*(uint32_t *)buf = dev->regs.doorbell[3]._iccr;
	} else if (offset == LSU0_ICSR) {
		*(uint32_t *)buf = dev->regs.lsu0_icsr;
	} else if (offset == LSU0_ICCR) {
		*(uint32_t *)buf = dev->regs.lsu0_iccr;
	} else if (offset == LSU1_ICSR) {
		*(uint32_t *)buf = dev->regs.lsu1_icsr;
	} else if (offset == LSU1_ICCR) {
		*(uint32_t *)buf = dev->regs.lsu1_iccr;
	} else if (offset == ERR_RST_EVNT_ICSR) {
		*(uint32_t *)buf = dev->regs.err_rst_evnt_icsr;
	} else if (offset == ERR_RST_EVNT_ICCR) {
		*(uint32_t *)buf = dev->regs.err_rst_evnt_iccr;
	} else if (offset == DOORBELL0_ICRR) {
		*(uint32_t *)buf = dev->regs.doorbell[0]._icrr;
	} else if (offset == DOORBELL0_ICRR2) {
		*(uint32_t *)buf = dev->regs.doorbell[0]._icrr2;
	} else if (offset == DOORBELL1_ICRR) {
		*(uint32_t *)buf = dev->regs.doorbell[1]._icrr;
	} else if (offset == DOORBELL1_ICRR2) {
		*(uint32_t *)buf = dev->regs.doorbell[1]._icrr2;
	} else if (offset == DOORBELL2_ICRR) {
		*(uint32_t *)buf = dev->regs.doorbell[2]._icrr;
	} else if (offset == DOORBELL2_ICRR2) {
		*(uint32_t *)buf = dev->regs.doorbell[2]._icrr2;
	} else if (offset == DOORBELL3_ICRR) {
		*(uint32_t *)buf = dev->regs.doorbell[3]._icrr;
	} else if (offset == DOORBELL3_ICRR2) {
		*(uint32_t *)buf = dev->regs.doorbell[3]._icrr2;
	} else if (offset == LSU0_ICRR1) {
		*(uint32_t *)buf = dev->regs.lsu0_icrr1;
	} else if (offset == LSU0_ICRR2) {
		*(uint32_t *)buf = dev->regs.lsu0_icrr2;
	} else if (offset == LSU0_ICRR3) {
		*(uint32_t *)buf = dev->regs.lsu0_icrr3;
	} else if (offset == LSU0_ICRR4) {
		*(uint32_t *)buf = dev->regs.lsu0_icrr4;
	} else if (offset == LSU1_ICRR1) {
		*(uint32_t *)buf = dev->regs.lsu1_icrr1;
	} else if (offset == ERR_RST_EVNT_ICRR) {
		*(uint32_t *)buf = dev->regs.err_rst_evnt_icrr;
	} else if (offset == ERR_RST_EVNT_ICRR2) {
		*(uint32_t *)buf = dev->regs.err_rst_evnt_icrr2;
	} else if (offset == ERR_RST_EVNT_ICRR3) {
		*(uint32_t *)buf = dev->regs.err_rst_evnt_icrr3;
	} else if (offset == INTERRUPT_CTL) {
		*(uint32_t *)buf = dev->regs.interrupt_ctl;
	} else if ((offset == INTDST0_DECODE) &&(offset == INTDST23_DECODE)) {
		*(uint32_t *)buf = dev->regs.intdst[(offset - INTDST0_DECODE) / 4]._decode;
	} else if ((offset == INTDST0_RATE_CNTL) && (offset == INTDST15_RATE_CNTL)) {
		*(uint32_t *)buf = dev->regs.intdst[(offset - INTDST0_RATE_CNTL) / 4]._rate_cntl;
	} else if (offset == INTDST_RATE_DIS) {
		*(uint32_t *)buf = dev->regs.intdst_rate_dis;

	} else if ((offset >= RXU_MAP00_L) && (offset <= RXU_MAP63_QID)) {
		uint32_t num = offset - RXU_MAP00_L;
		uint32_t t = num % 12;
		uint32_t i = num / 12;

		switch (t){
		case 0:
			*(uint32_t *)buf = dev->regs.rxu_map[i]._l;
			break;
		case 4:
			*(uint32_t *)buf = dev->regs.rxu_map[i]._h;
			break;
		case 8:
			*(uint32_t *)buf = dev->regs.rxu_map[i]._qid;
			break;
		}
	} else if ((offset >= RXU_TYPE9_MAP00_0) && (offset <= RXU_TYPE9_MAP63_2)) {
		uint32_t num = offset - RXU_TYPE9_MAP00_0;
		uint32_t t = num % 12;
		uint32_t i = num / 12;
		switch (t){
		case 0:
			*(uint32_t *)buf = dev->regs.rxu_type9_map[i]._[0];
			break;
		case 4:
			*(uint32_t *)buf = dev->regs.rxu_type9_map[i]._[1];
			break;
		case 8:
			*(uint32_t *)buf = dev->regs.rxu_type9_map[i]._[2];
			break;
		}
	} else if ((offset >= LSU1_REG0) && (offset <= LSU8_REG6)) {
		uint32_t num = offset - LSU1_REG0;
		uint32_t t = num % 28;  // 7(0-6) * 4 = 28
		uint32_t i = num / 28;
		i += 1;                 // index: 1 to 8; 0 is NULL
		switch (t) {
		case 0:
			*(uint32_t *)buf = dev->regs.lsu[i]._reg[0];
			break;
		case 4:
			*(uint32_t *)buf = dev->regs.lsu[i]._reg[1];
			break;
		case 8:
			*(uint32_t *)buf = dev->regs.lsu[i]._reg[2];
			break;
		case 12:
			*(uint32_t *)buf = dev->regs.lsu[i]._reg[3];
			break;
		case 16:
			*(uint32_t *)buf = dev->regs.lsu[i]._reg[4];
			break;
		case 20:
			*(uint32_t *)buf = dev->regs.lsu[i]._reg[5];
			break;
		case 24:
			*(uint32_t *)buf = dev->regs.lsu[i]._reg[7];
			break;
		}
	} else if (offset == LSU_SETUP_REG0) {
		*(uint32_t *)buf = dev->regs.lsu_setup_reg0;
	} else if (offset == LSU_SETUP_REG1) {
		*(uint32_t *)buf = dev->regs.lsu_setup_reg1;
	} else if ((offset >= LSU_STAT_REG0) && (offset <= LSU_STAT_REG5)) {
		*(uint32_t *)buf = dev->regs.lsu_stat_reg[(offset - LSU_STAT_REG0) / 4];
	} else if ((offset >= LSU_FLOW_MASKS0) && (offset <= LSU_FLOW_MASKS3)) {
		*(uint32_t *)buf = dev->regs.lsu_flow_masks[(offset - LSU_FLOW_MASKS0) / 4];
	} else if (offset == SUPRVSR_ID) {
		*(uint32_t *)buf = dev->regs.suprvsr_id;
	} else if ((offset >= FLOW_CNTL0) && (offset <= FLOW_CNTL15)) {
		*(uint32_t *)buf = dev->regs.flow_cntl[(offset - FLOW_CNTL0) / 4];
	} else if ((offset >= TX_CPPI_FLOW_MASKS0) && (offset <= TX_CPPI_FLOW_MASKS7)) {
		*(uint32_t *)buf = dev->regs.tx_cppi_flow_masks[(offset - TX_CPPI_FLOW_MASKS0) / 4];
	} else if ((offset >= TX_QUEUE_SCH_INFO1) && (offset <= TX_QUEUE_SCH_INFO4)) {
		*(uint32_t *)buf = dev->regs.tx_queue_sch_info[((offset - TX_QUEUE_SCH_INFO1) / 4) + 1];
	} else if ((offset >= GARBAGE_COLL_QID0) && (offset <= GARBAGE_COLL_QID2)) {
		*(uint32_t *)buf = dev->regs.garbage_coll_qid[(offset - GARBAGE_COLL_QID0) / 4];
	} else if ((offset >= TCHAN_GCFG_REG_A0) && (offset <= TCHAN_GCFG_REG_B15)) {
		uint32_t num = offset - TCHAN_GCFG_REG_A0;
		uint32_t t = num % 8;
		uint32_t i = num / 8;
		switch (t) {
		case 0:
			*(uint32_t *)buf = dev->regs.tchan_gcfg_reg_a[i];
			break;
		case 4:
			*(uint32_t *)buf = dev->regs.tchan_gcfg_reg_b[i];
			break;
		}
	} else if ((offset >= RCHAN_GCFG_REG_A0) && (offset <= RCHAN_GCFG_REG_A15)) {
		*(uint32_t *)buf = dev->regs.rchan_gcfg_reg_a[(offset - RCHAN_GCFG_REG_A0) / 4];
	} else if ((offset >= TCHAN_SCHED_CFG_REG0) && (offset <= TCHAN_SCHED_CFG_REG15)) {
		*(uint32_t *)buf = dev->regs.tchan_sched_cfg_reg[(offset - TCHAN_SCHED_CFG_REG0) / 4];
	} else if ((offset >= RFLOW_CFG_REG_A0) && (offset <= RFLOW_CFG_REG_H19)) {
		uint32_t num = offset - RFLOW_CFG_REG_A0;
		uint32_t t = num % 32;
		uint32_t i = num / 32;
		switch (t) {
		case 0:
			*(uint32_t *)buf = dev->regs.rflow_cfg_reg_a[i];
			break;
		case 4:
			*(uint32_t *)buf = dev->regs.rflow_cfg_reg_b[i];
			break;
		case 8:
			*(uint32_t *)buf = dev->regs.rflow_cfg_reg_c[i];
			break;
		case 12:
			*(uint32_t *)buf = dev->regs.rflow_cfg_reg_d[i];
			break;
		case 16:
			*(uint32_t *)buf = dev->regs.rflow_cfg_reg_e[i];
			break;
		case 20:
			*(uint32_t *)buf = dev->regs.rflow_cfg_reg_f[i];
			break;
		case 24:
			*(uint32_t *)buf = dev->regs.rflow_cfg_reg_g[i];
			break;
		case 28:
			*(uint32_t *)buf = dev->regs.rflow_cfg_reg_h[i];
			break;
		}
	} else if (offset == DEV_ID) {
		*(uint32_t *)buf = dev->regs.dev_id;
	} else if (offset == DEV_INFO) {
		*(uint32_t *)buf = dev->regs.dev_info;
	} else if (offset == ASBLY_ID) {
		*(uint32_t *)buf = dev->regs.asbly_id;
	} else if (offset == ASBLY_INFO) {
		*(uint32_t *)buf = dev->regs.asbly_info;
	} else if (offset == PE_FEAT) {
		*(uint32_t *)buf = dev->regs.pe_feat;
	} else if (offset == SW_PORT) {
		*(uint32_t *)buf = dev->regs.sw_port;
	} else if (offset == SRC_OP) {
		*(uint32_t *)buf = dev->regs.src_op;
	} else if (offset == DEST_OP) {
		*(uint32_t *)buf = dev->regs.dest_op;
	} else if (offset == DS_INFO) {
		*(uint32_t *)buf = dev->regs.ds_info;
	} else if (offset == DS_LL_CTL) {
		*(uint32_t *)buf = dev->regs.ds_ll_ctl;
	} else if (offset == PE_LL_CTL) {
		*(uint32_t *)buf = dev->regs.pe_ll_ctl;
	} else if (offset == LCL_CFG_HBAR) {
		*(uint32_t *)buf = dev->regs.lcl_cfg_hbar;
	} else if (offset == LCL_CFG_BAR) {
		*(uint32_t *)buf = dev->regs.lcl_cfg_bar;
	} else if (offset == BASE_ID) {
		*(uint32_t *)buf = dev->regs.base_id;
	} else if (offset == HOST_BASE_ID_LOCK) {
		*(uint32_t *)buf = dev->regs.host_base_id_lock;
	} else if (offset == COMP_TAG) {
		*(uint32_t *)buf = dev->regs.comp_tag;
	} else if (offset == SP_MB_HEAD) {
		*(uint32_t *)buf = dev->regs.sp_mb_head;
	} else if (offset == SP_LT_CTL) {
		*(uint32_t *)buf = dev->regs.sp_lt_ctl;
	} else if (offset == SP_RT_CTL) {
		*(uint32_t *)buf = dev->regs.sp_rt_ctl;
	} else if (offset == SP_GEN_CTL) {
		*(uint32_t *)buf = dev->regs.sp_gen_ctl;
	} else if ((offset >= SP0_LM_REQ) && (offset <= SP3_CTL)) {
		uint32_t num = offset - SP0_LM_REQ;
		uint32_t t = num % 28;
		uint32_t i = num / 28;
		switch (t) {
		case 0:
			*(uint32_t *)buf = dev->regs.sp[i]._lm_req;
			break;
		case 4:
			*(uint32_t *)buf = dev->regs.sp[i]._lm_resp;
			break;
		case 8:
			*(uint32_t *)buf = dev->regs.sp[i]._ackid_stat;
			break;
		case 12:
		case 16:
			break;
		case 20:
			*(uint32_t *)buf = dev->regs.sp[i]._ctl2;
			break;
		case 24:
			*(uint32_t *)buf = dev->regs.sp[i]._err_stat;
			break;
		case 28:
			*(uint32_t *)buf = dev->regs.sp[i]._ctl;
			break;
		}
	} else if (offset == ERR_RPT_BH) {
		*(uint32_t *)buf = dev->regs.err_rpt_bh;
	} else if (offset == ERR_DET) {
		*(uint32_t *)buf = dev->regs.err_det;
	} else if (offset == ERR_EN) {
		*(uint32_t *)buf = dev->regs.err_en;
	} else if (offset == H_ADDR_CAPT) {
		*(uint32_t *)buf = dev->regs.h_addr_capt;
	} else if (offset == ADDR_CAPT) {
		*(uint32_t *)buf = dev->regs.addr_capt;
	} else if (offset == ID_CAPT) {
		*(uint32_t *)buf = dev->regs.id_capt;
	} else if (offset == CTRL_CAPT) {
		*(uint32_t *)buf = dev->regs.ctrl_capt;
	} else if (offset == PW_TGT_ID) {
		*(uint32_t *)buf = dev->regs.pw_tgt_id;
	} else if ((offset >= SP0_ERR_DET) && (offset <= SP3_ERR_THRESH)) {
		uint32_t num = offset - SP0_ERR_DET;
		uint32_t t = num % 36; // (9 * 4)
		uint32_t i = num / 36;    // (9 * 4)
		switch (t) {
		case 0:  // 0*4
			*(uint32_t *)buf = dev->regs.sp[i]._err_det;
			break;
		case 4:  // 1*4
			*(uint32_t *)buf = dev->regs.sp[i]._rate_en;
			break;
		case 8:  // 2*4
			*(uint32_t *)buf = dev->regs.sp[i]._err_attr_capt_dbg0;
			break;
		case 12:  // 3*4
			*(uint32_t *)buf = dev->regs.sp[i]._err_capt_0_dbg1;
			break;
		case 16:  // 4*4
			*(uint32_t *)buf = dev->regs.sp[i]._err_capt_1_dbg2;
			break;
		case 20:  // 5*4
			*(uint32_t *)buf = dev->regs.sp[i]._err_capt_2_dbg3;
			break;
		case 24:  // 6*4
			*(uint32_t *)buf = dev->regs.sp[i]._err_capt_3_dbg4;
			break;
		case 28:  // 7*4
			*(uint32_t *)buf = dev->regs.sp[i]._err_rate;
			break;
		case 32:  // 8*4
			*(uint32_t *)buf = dev->regs.sp[i]._err_thresh;
			break;
		}
	} else if ((offset >= LANE0_STAT0) && (offset <= LANE3_STAT0)) {
		uint32_t num = offset - LANE0_STAT0;
		uint32_t t = num % 8;
		uint32_t i = num / 8;

		*(uint32_t *)buf = dev->regs.lane[i]._stat[t / 4];
	} else if (offset == PLM_BH) {
		*(uint32_t *)buf = dev->regs.plm_bh;
	} else if (offset == PLM_SP0_IMP_SPEC_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp0_imp_spec_ctl;
	} else if (offset == RIO_PLM_SP0_PWDN_CTL) {
		*(uint32_t *)buf = dev->regs.rio_plm_sp0_pwdn_ctl;
	} else if (offset == PLM_SP0_STATUS) {
		*(uint32_t *)buf = dev->regs.plm_sp0_status;
	} else if (offset == PLM_SP0_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.plm_sp0_int_enable;
	} else if (offset == PLM_SP0_PW_ENABLE) {
		*(uint32_t *)buf = dev->regs.plm_sp0_pw_enable;
	} else if (offset == PLM_SP0_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.plm_sp0_event_gen;
	} else if (offset == PLM_SP0_ALL_INT_EN) {
		*(uint32_t *)buf = dev->regs.plm_sp0_all_int_en;
	} else if (offset == PLM_SP0_ALL_PW_EN) {
		*(uint32_t *)buf = dev->regs.plm_sp0_all_pw_en;
	} else if (offset == PLM_SP0_PATH_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp0_path_ctl;
	} else if (offset == PLM_SP0_DISCOVERY_TIMER) {
		*(uint32_t *)buf = dev->regs.plm_sp0_discovery_timer;
	} else if (offset == PLM_SP0_SILENCE_TIMER) {
		*(uint32_t *)buf = dev->regs.plm_sp0_silence_timer;
	} else if (offset == PLM_SP0_VMIN_EXP) {
		*(uint32_t *)buf = dev->regs.plm_sp0_vmin_exp;
	} else if (offset == PLM_SP0_POL_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp0_pol_ctl;
	} else if (offset == PLM_SP0_DENIAL_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp0_denial_ctl;
	} else if (offset == PLM_SP0_RCVD_MECS) {
		*(uint32_t *)buf = dev->regs.plm_sp0_rcvd_mecs;
	} else if (offset == PLM_SP0_MECS_FWD) {
		*(uint32_t *)buf = dev->regs.plm_sp0_mecs_fwd;
	} else if (offset == PLM_SP0_LONG_CS_TX1) {
		*(uint32_t *)buf = dev->regs.plm_sp0_long_cs_tx1;
	} else if (offset == PLM_SP0_LONG_CS_TX2) {
		*(uint32_t *)buf = dev->regs.plm_sp0_long_cs_tx2;
	} else if (offset == PLM_SP1_IMP_SPEC_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp1_imp_spec_ctl;
	} else if (offset == RIO_PLM_SP1_PWDN_CTL) {
		*(uint32_t *)buf = dev->regs.rio_plm_sp1_pwdn_ctl;
	} else if (offset == PLM_SP1_STATUS) {
		*(uint32_t *)buf = dev->regs.plm_sp1_status;
	} else if (offset == PLM_SP1_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.plm_sp1_int_enable;
	} else if (offset == PLM_SP1_PW_ENABLE) {
		*(uint32_t *)buf = dev->regs.plm_sp1_pw_enable;
	} else if (offset == PLM_SP1_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.plm_sp1_event_gen;
	} else if (offset == PLM_SP1_ALL_INT_EN) {
		*(uint32_t *)buf = dev->regs.plm_sp1_all_int_en;
	} else if (offset == PLM_SP1_ALL_PW_EN) {
		*(uint32_t *)buf = dev->regs.plm_sp1_all_pw_en;
	} else if (offset == PLM_SP1_PATH_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp1_path_ctl;
	} else if (offset == PLM_SP1_DISCOVERY_TIMER) {
		*(uint32_t *)buf = dev->regs.plm_sp1_discovery_timer;
	} else if (offset == PLM_SP1_SILENCE_TIMER) {
		*(uint32_t *)buf = dev->regs.plm_sp1_silence_timer;
	} else if (offset == PLM_SP1_VMIN_EXP) {
		*(uint32_t *)buf = dev->regs.plm_sp1_vmin_exp;
	} else if (offset == PLM_SP1_POL_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp1_pol_ctl;
	} else if (offset == PLM_SP1_DENIAL_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp1_denial_ctl;
	} else if (offset == PLM_SP1_RCVD_MECS) {
		*(uint32_t *)buf = dev->regs.plm_sp1_rcvd_mecs;
	} else if (offset == PLM_SP1_MECS_FWD) {
		*(uint32_t *)buf = dev->regs.plm_sp1_mecs_fwd;
	} else if (offset == PLM_SP1_LONG_CS_TX1) {
		*(uint32_t *)buf = dev->regs.plm_sp1_long_cs_tx1;
	} else if (offset == PLM_SP1_LONG_CS_TX2) {
		*(uint32_t *)buf = dev->regs.plm_sp1_long_cs_tx2;
	} else if (offset == PLM_SP2_IMP_SPEC_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp2_imp_spec_ctl;
	} else if (offset == PLM_SP2_PWDN_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp2_pwdn_ctl;
	} else if (offset == PLM_SP2_STATUS) {
		*(uint32_t *)buf = dev->regs.plm_sp2_status;
	} else if (offset == PLM_SP2_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.plm_sp2_int_enable;
	} else if (offset == PLM_SP2_PW_ENABLE) {
		*(uint32_t *)buf = dev->regs.plm_sp2_pw_enable;
	} else if (offset == PLM_SP2_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.plm_sp2_event_gen;
	} else if (offset == PLM_SP2_ALL_INT_EN) {
		*(uint32_t *)buf = dev->regs.plm_sp2_all_int_en;
	} else if (offset == PLM_SP2_ALL_PW_EN) {
		*(uint32_t *)buf = dev->regs.plm_sp2_all_pw_en;
	} else if (offset == PLM_SP2_PATH_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp2_path_ctl;
	} else if (offset == PLM_SP2_DISCOVERY_TIMER) {
		*(uint32_t *)buf = dev->regs.plm_sp2_discovery_timer;
	} else if (offset == PLM_SP2_SILENCE_TIMER) {
		*(uint32_t *)buf = dev->regs.plm_sp2_silence_timer;
	} else if (offset == PLM_SP2_VMIN_EXP) {
		*(uint32_t *)buf = dev->regs.plm_sp2_vmin_exp;
	} else if (offset == PLM_SP2_POL_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp2_pol_ctl;
	} else if (offset == PLM_SP2_DENIAL_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp2_denial_ctl;
	} else if (offset == PLM_SP2_RCVD_MECS) {
		*(uint32_t *)buf = dev->regs.plm_sp2_rcvd_mecs;
	} else if (offset == PLM_SP2_MECS_FWD) {
		*(uint32_t *)buf = dev->regs.plm_sp2_mecs_fwd;
	} else if (offset == PLM_SP2_LONG_CS_TX1) {
		*(uint32_t *)buf = dev->regs.plm_sp2_long_cs_tx1;
	} else if (offset == PLM_SP2_LONG_CS_TX2) {
		*(uint32_t *)buf = dev->regs.plm_sp2_long_cs_tx2;
	} else if (offset == PLM_SP3_IMP_SPEC_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp3_imp_spec_ctl;
	} else if (offset == PLM_SP3_PWDN_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp3_pwdn_ctl;
	} else if (offset == PLM_SP3_STATUS) {
		*(uint32_t *)buf = dev->regs.plm_sp3_status;
	} else if (offset == PLM_SP3_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.plm_sp3_int_enable;
	} else if (offset == PLM_SP3_PW_ENABLE) {
		*(uint32_t *)buf = dev->regs.plm_sp3_pw_enable;
	} else if (offset == PLM_SP3_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.plm_sp3_event_gen;
	} else if (offset == PLM_SP3_ALL_INT_EN) {
		*(uint32_t *)buf = dev->regs.plm_sp3_all_int_en;
	} else if (offset == PLM_SP3_ALL_PW_EN) {
		*(uint32_t *)buf = dev->regs.plm_sp3_all_pw_en;
	} else if (offset == PLM_SP3_PATH_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp3_path_ctl;
	} else if (offset == PLM_SP3_DISCOVERY_TIMER) {
		*(uint32_t *)buf = dev->regs.plm_sp3_discovery_timer;
	} else if (offset == PLM_SP3_SILENCE_TIMER) {
		*(uint32_t *)buf = dev->regs.plm_sp3_silence_timer;
	} else if (offset == PLM_SP3_VMIN_EXP) {
		*(uint32_t *)buf = dev->regs.plm_sp3_vmin_exp;
	} else if (offset == PLM_SP3_POL_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp3_pol_ctl;
	} else if (offset == PLM_SP3_DENIAL_CTL) {
		*(uint32_t *)buf = dev->regs.plm_sp3_denial_ctl;
	} else if (offset == PLM_SP3_RCVD_MECS) {
		*(uint32_t *)buf = dev->regs.plm_sp3_rcvd_mecs;
	} else if (offset == PLM_SP3_MECS_FWD) {
		*(uint32_t *)buf = dev->regs.plm_sp3_mecs_fwd;
	} else if (offset == PLM_SP3_LONG_CS_TX1) {
		*(uint32_t *)buf = dev->regs.plm_sp3_long_cs_tx1;
	} else if (offset == PLM_SP3_LONG_CS_TX2) {
		*(uint32_t *)buf = dev->regs.plm_sp3_long_cs_tx2;
	} else if (offset == TLM_BH) {
		*(uint32_t *)buf = dev->regs.tlm_bh;
	} else if ((offset >= TLM_SP0_CONTROL) && (offset <= TLM_SP3_BRR_3_PATTERN_MATCH)) {
		uint32_t num = offset - TLM_SP0_CONTROL;
		uint32_t t = num % 52;   // 13 * 4
		uint32_t i = num / 52;
		switch (t) {
		case 0:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._control;
			break;
		case 4:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._status;
			break;
		case 8:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._int_enable;
			break;
		case 12:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._pw_enable;
			break;
		case 16:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._event_gen;
			break;
		case 20:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._brr_0_ctl;
			break;
		case 24:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._brr_0_pattern_match;
			break;
		case 28:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._brr_1_ctl;
			break;
		case 32:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._brr_1_pattern_match;
			break;
		case 36:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._brr_2_ctl;
			break;
		case 40:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._brr_2_pattern_match;
			break;
		case 44:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._brr_3_ctl;
			break;
		case 48:
			*(uint32_t *)buf = dev->regs.tlm_sp[i]._brr_3_pattern_match;
			break;
		}
	} else if (offset == PBM_BH) {
		*(uint32_t *)buf = dev->regs.pbm_bh;
	} else if (offset == PBM_SP0_CONTROL) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_control;
	} else if (offset == PBM_SP0_STATUS) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_status;
	} else if (offset == PBM_SP0_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_int_enable;
	} else if (offset == PBM_SP0_PW_ENABLE) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_pw_enable;
	} else if (offset == PBM_SP0_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_event_gen;
	} else if (offset == PBM_SP0_IG_RESOURCES) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_ig_resources;
	} else if (offset == PBM_SP0_EG_RESOURCES) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_eg_resources;
	} else if (offset == PBM_SP0_IG_WATERMARK0) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_ig_watermark0;
	} else if (offset == PBM_SP0_IG_WATERMARK1) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_ig_watermark1;
	} else if (offset == PBM_SP0_IG_WATERMARK2) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_ig_watermark2;
	} else if (offset == PBM_SP0_IG_WATERMARK3) {
		*(uint32_t *)buf = dev->regs.pbm_sp0_ig_watermark3;
	} else if (offset == PBM_SP1_CONTROL) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_control;
	} else if (offset == PBM_SP1_STATUS) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_status;
	} else if (offset == PBM_SP1_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_int_enable;
	} else if (offset == PBM_SP1_PW_ENABLE) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_pw_enable;
	} else if (offset == PBM_SP1_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_event_gen;
	} else if (offset == PBM_SP1_IG_RESOURCES) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_ig_resources;
	} else if (offset == PBM_SP1_EG_RESOURCES) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_eg_resources;
	} else if (offset == PBM_SP1_IG_WATERMARK0) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_ig_watermark0;
	} else if (offset == PBM_SP1_IG_WATERMARK1) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_ig_watermark1;
	} else if (offset == PBM_SP1_IG_WATERMARK2) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_ig_watermark2;
	} else if (offset == PBM_SP1_IG_WATERMARK3) {
		*(uint32_t *)buf = dev->regs.pbm_sp1_ig_watermark3;
	} else if (offset == PBM_SP2_CONTROL) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_control;
	} else if (offset == PBM_SP2_STATUS) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_status;
	} else if (offset == PBM_SP2_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_int_enable;
	} else if (offset == PBM_SP2_PW_ENABLE) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_pw_enable;
	} else if (offset == PBM_SP2_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_event_gen;
	} else if (offset == PBM_SP2_IG_RESOURCES) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_ig_resources;
	} else if (offset == PBM_SP2_EG_RESOURCES) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_eg_resources;
	} else if (offset == PBM_SP2_IG_WATERMARK0) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_ig_watermark0;
	} else if (offset == PBM_SP2_IG_WATERMARK1) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_ig_watermark1;
	} else if (offset == PBM_SP2_IG_WATERMARK2) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_ig_watermark2;
	} else if (offset == PBM_SP2_IG_WATERMARK3) {
		*(uint32_t *)buf = dev->regs.pbm_sp2_ig_watermark3;
	} else if (offset == PBM_SP3_CONTROL) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_control;
	} else if (offset == PBM_SP3_STATUS) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_status;
	} else if (offset == PBM_SP3_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_int_enable;
	} else if (offset == PBM_SP3_PW_ENABLE) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_pw_enable;
	} else if (offset == PBM_SP3_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_event_gen;
	} else if (offset == PBM_SP3_IG_RESOURCES) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_ig_resources;
	} else if (offset == PBM_SP3_EG_RESOURCES) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_eg_resources;
	} else if (offset == PBM_SP3_IG_WATERMARK0) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_ig_watermark0;
	} else if (offset == PBM_SP3_IG_WATERMARK1) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_ig_watermark1;
	} else if (offset == PBM_SP3_IG_WATERMARK2) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_ig_watermark2;
	} else if (offset == PBM_SP3_IG_WATERMARK3) {
		*(uint32_t *)buf = dev->regs.pbm_sp3_ig_watermark3;
	} else if (offset == EM_BH) {
		*(uint32_t *)buf = dev->regs.em_bh;
	} else if (offset == EM_INT_STAT) {
		*(uint32_t *)buf = dev->regs.em_int_stat;
	} else if (offset == EM_INT_ENABLE) {
		*(uint32_t *)buf = dev->regs.em_int_enable;
	} else if (offset == EM_INT_PORT_STAT) {
		*(uint32_t *)buf = dev->regs.em_int_port_stat;
	} else if (offset == EM_PW_STAT) {
		*(uint32_t *)buf = dev->regs.em_pw_stat;
	} else if (offset == EM_PW_EN) {
		*(uint32_t *)buf = dev->regs.em_pw_en;
	} else if (offset == EM_PW_PORT_STAT) {
		*(uint32_t *)buf = dev->regs.em_pw_port_stat;
	} else if (offset == EM_DEV_INT_EN) {
		*(uint32_t *)buf = dev->regs.em_dev_int_en;
	} else if (offset == EM_DEV_PW_EN) {
		*(uint32_t *)buf = dev->regs.em_dev_pw_en;
	} else if (offset == EM_MECS_STAT) {
		*(uint32_t *)buf = dev->regs.em_mecs_stat;
	} else if (offset == EM_MECS_INT_EN) {
		*(uint32_t *)buf = dev->regs.em_mecs_int_en;
	} else if (offset == EM_MECS_CAP_EN) {
		*(uint32_t *)buf = dev->regs.em_mecs_cap_en;
	} else if (offset == EM_MECS_TRIG_EN) {
		*(uint32_t *)buf = dev->regs.em_mecs_trig_en;
	} else if (offset == EM_MECS_REQ) {
		*(uint32_t *)buf = dev->regs.em_mecs_req;
	} else if (offset == EM_MECS_PORT_STAT) {
		*(uint32_t *)buf = dev->regs.em_mecs_port_stat;
	} else if (offset == EM_RST_PORT_STAT) {
		*(uint32_t *)buf = dev->regs.em_rst_port_stat;
	} else if (offset == EM_RST_INT_EN) {
		*(uint32_t *)buf = dev->regs.em_rst_int_en;
	} else if (offset == EM_RST_PW_EN) {
		*(uint32_t *)buf = dev->regs.em_rst_pw_en;
	} else if (offset == PW_BH) {
		*(uint32_t *)buf = dev->regs.pw_bh;
	} else if (offset == PW_CTL) {
		*(uint32_t *)buf = dev->regs.pw_ctl;
	} else if (offset == PW_ROUTE) {
		*(uint32_t *)buf = dev->regs.pw_route;
	} else if (offset == PW_RX_STAT) {
		*(uint32_t *)buf = dev->regs.pw_rx_stat;
	} else if (offset == RIO_PW_RX_EVENT_GEN) {
		*(uint32_t *)buf = dev->regs.rio_pw_rx_event_gen;
	} else if (offset == PW_RX_CAPT0) {
		*(uint32_t *)buf = dev->regs.pw_rx_capt0;
	} else if (offset == PW_RX_CAPT1) {
		*(uint32_t *)buf = dev->regs.pw_rx_capt1;
	} else if (offset == PW_RX_CAPT2) {
		*(uint32_t *)buf = dev->regs.pw_rx_capt2;
	} else if (offset == PW_RX_CAPT3) {
		*(uint32_t *)buf = dev->regs.pw_rx_capt3;
	} else if (offset == LLM_BH) {
		*(uint32_t *)buf = dev->regs.llm_bh;
	} else if (offset == WHITEBOARD) {
		*(uint32_t *)buf = dev->regs.whiteboard;
	} else if (offset == PORT_NUMBER) {
		*(uint32_t *)buf = dev->regs.port_number;
	} else if (offset == PRESCALAR_SRV_CLK) {
		*(uint32_t *)buf = dev->regs.prescalar_srv_clk;
	} else if (offset == REG_RST_CTL) {
		*(uint32_t *)buf = dev->regs.reg_rst_ctl;
	} else if (offset == LOCAL_ERR_DET) {
		*(uint32_t *)buf = dev->regs.local_err_det;
	} else if (offset == LOCAL_ERR_EN) {
		*(uint32_t *)buf = dev->regs.local_err_en;
	} else if (offset == LOCAL_H_ADDR_CAPT) {
		*(uint32_t *)buf = dev->regs.local_h_addr_capt;
	} else if (offset == LOCAL_ADDR_CAPT) {
		*(uint32_t *)buf = dev->regs.local_addr_capt;
	} else if (offset == LOCAL_ID_CAPT) {
		*(uint32_t *)buf = dev->regs.local_id_capt;
	} else if (offset == LOCAL_CTRL_CAPT) {
		*(uint32_t *)buf = dev->regs.local_ctrl_capt;
	} else if (offset == FABRIC_HDR) {
		*(uint32_t *)buf = dev->regs.fabric_hdr;
	} else if (offset == FABRIC_CSR) {
		*(uint32_t *)buf = dev->regs.fabric_csr;
	} else if (offset == SP0_FABRIC_STATUS) {
		*(uint32_t *)buf = dev->regs.sp0_fabric_status;
	} else if (offset == SP1_FABRIC_STATUS) {
		*(uint32_t *)buf = dev->regs.sp1_fabric_status;
	} else if (offset == SP2_FABRIC_STATUS) {
		*(uint32_t *)buf = dev->regs.sp2_fabric_status;
	} else if (offset == SP3_FABRIC_STATUS) {
		*(uint32_t *)buf = dev->regs.sp3_fabric_status;
	} else {
		skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in srio_c6678\n", offset);
	}
	//printf("%s  offset: 0x%05x buf: 0x%08x\n", __func__, offset, *(uint32_t *)buf);
	return 0;
}

static exception_t srio_c6678_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	srio_c6678_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("%s offset: 0x%05x buf: 0x%08x\n", __func__, offset, *(uint32_t *)buf);

	if (offset == PID) {
		dev->regs.pid = val;
	} else if (offset == PCR) {
		dev->regs.pcr = val;
	} else if (offset == PER_SET_CNTL) {
		dev->regs.per_set_cntl = val;
	} else if (offset == PER_SET_CNTL1) {
		dev->regs.per_set_cntl1 = val;
	} else if (offset == GBL_EN) {
		dev->regs.gbl_en = val;
	} else if (offset == GBL_EN_STAT) {
		dev->regs.gbl_en_stat = val;
	} else if ((offset >= BLK0_EN) &&(offset <= BLK9_EN_STAT)) {
		uint32_t num = offset - BLK0_EN;
		if ((num % 8) == 0) {
			dev->regs.blk[num / 8]._en = val;
		} else {
			dev->regs.blk[num / 8]._en_stat = val;
		}
	} else if (offset == BLK9_EN_STAT) {
		dev->regs.blk9_en_stat = val;
	} else if ((offset >= MULTIID_REG1) &&(offset <= MULTIID_REG8)) {
		dev->regs.multiid_reg[(offset - MULTIID_REG1) / 4] = val;
	} else if ((offset >= PF_16B_CNTL0) && (offset <= PF_8B_CNTL7)) {
		uint32_t num = offset - PF_16B_CNTL0;
		if ((num % 8) == 0) {
			*(uint32_t *)buf = dev->regs.pf_16b_cntl[num / 8] = val;
		} else {
			*(uint32_t *)buf = dev->regs.pf_8b_cntl[num / 8] = val;
		}
	} else if (offset == RESERVED) {
		dev->regs.reserved = val;
	} else if (offset == DOORBELL0_ICSR) {
		dev->regs.doorbell[0]._icsr = val;
	} else if (offset == DOORBELL0_ICCR) {
		dev->regs.doorbell[0]._iccr = val;
	} else if (offset == DOORBELL1_ICSR) {
		dev->regs.doorbell[1]._icsr = val;
	} else if (offset == DOORBELL1_ICCR) {
		dev->regs.doorbell[1]._iccr = val;
	} else if (offset == DOORBELL2_ICSR) {
		dev->regs.doorbell[2]._icsr = val;
	} else if (offset == DOORBELL2_ICCR) {
		dev->regs.doorbell[2]._iccr = val;
	} else if (offset == DOORBELL3_ICSR) {
		dev->regs.doorbell[3]._icsr = val;
	} else if (offset == DOORBELL3_ICCR) {
		dev->regs.doorbell[3]._iccr = val;
	} else if (offset == LSU0_ICSR) {
		dev->regs.lsu0_icsr = val;		// LSU0 ÖÐ¶Ï×´Ì¬¼Ä´æÆ÷
	} else if (offset == LSU0_ICCR) {
		dev->regs.lsu0_iccr = val;		// LSU0 ÖÐ¶ÏÇåÀí¼Ä´æÆ÷
	} else if (offset == LSU1_ICSR) {
		dev->regs.lsu1_icsr = val;		// LSU1 ÖÐ¶Ï×´Ì¬¼Ä´æÆ÷
	} else if (offset == LSU1_ICCR) {
		dev->regs.lsu1_iccr = val;		// LSU1 ÖÐ¶ÏÇåÀí¼Ä´æÆ÷
	} else if (offset == ERR_RST_EVNT_ICSR) {
		dev->regs.err_rst_evnt_icsr = val;
	} else if (offset == ERR_RST_EVNT_ICCR) {
		dev->regs.err_rst_evnt_iccr = val;
	} else if (offset == DOORBELL0_ICRR) {
		dev->regs.doorbell[0]._icrr = val;
	} else if (offset == DOORBELL0_ICRR2) {
		dev->regs.doorbell[0]._icrr2 = val;
	} else if (offset == DOORBELL1_ICRR) {
		dev->regs.doorbell[1]._icrr = val;
	} else if (offset == DOORBELL1_ICRR2) {
		dev->regs.doorbell[1]._icrr2 = val;
	} else if (offset == DOORBELL2_ICRR) {
		dev->regs.doorbell[2]._icrr = val;
	} else if (offset == DOORBELL2_ICRR2) {
		dev->regs.doorbell[2]._icrr2 = val;
	} else if (offset == DOORBELL3_ICRR) {
		dev->regs.doorbell[3]._icrr = val;
	} else if (offset == DOORBELL3_ICRR2) {
		dev->regs.doorbell[3]._icrr2 = val;
	} else if (offset == LSU0_ICRR1) {
		dev->regs.lsu0_icrr1 = val;
	} else if (offset == LSU0_ICRR2) {
		dev->regs.lsu0_icrr2 = val;
	} else if (offset == LSU0_ICRR3) {
		dev->regs.lsu0_icrr3 = val;
	} else if (offset == LSU0_ICRR4) {
		dev->regs.lsu0_icrr4 = val;
	} else if (offset == LSU1_ICRR1) {
		dev->regs.lsu1_icrr1 = val;
	} else if (offset == ERR_RST_EVNT_ICRR) {
		dev->regs.err_rst_evnt_icrr = val;
	} else if (offset == ERR_RST_EVNT_ICRR2) {
		dev->regs.err_rst_evnt_icrr2 = val;
	} else if (offset == ERR_RST_EVNT_ICRR3) {
		dev->regs.err_rst_evnt_icrr3 = val;
	} else if (offset == INTERRUPT_CTL) {
		dev->regs.interrupt_ctl = val;
	} else if ((offset == INTDST0_DECODE) &&(offset == INTDST23_DECODE)) {
		dev->regs.intdst[(offset - INTDST0_DECODE) / 4]._decode = val;
	} else if ((offset == INTDST0_RATE_CNTL) && (offset == INTDST15_RATE_CNTL)) {
		dev->regs.intdst[(offset - INTDST0_RATE_CNTL) / 4]._rate_cntl = val;
	} else if (offset == INTDST1_RATE_CNTL) {
		dev->regs.intdst[1]._rate_cntl = val;
	} else if (offset == INTDST2_RATE_CNTL) {
		dev->regs.intdst[2]._rate_cntl = val;
	} else if (offset == INTDST3_RATE_CNTL) {
		dev->regs.intdst[3]._rate_cntl = val;
	} else if (offset == INTDST4_RATE_CNTL) {
		dev->regs.intdst[4]._rate_cntl = val;
	} else if (offset == INTDST5_RATE_CNTL) {
		dev->regs.intdst[5]._rate_cntl = val;
	} else if (offset == INTDST6_RATE_CNTL) {
		dev->regs.intdst[6]._rate_cntl = val;
	} else if (offset == INTDST7_RATE_CNTL) {
		dev->regs.intdst[7]._rate_cntl = val;
	} else if (offset == INTDST8_RATE_CNTL) {
		dev->regs.intdst[8]._rate_cntl = val;
	} else if (offset == INTDST9_RATE_CNTL) {
		dev->regs.intdst[9]._rate_cntl = val;
	} else if (offset == INTDST10_RATE_CNTL) {
		dev->regs.intdst[10]._rate_cntl = val;
	} else if (offset == INTDST11_RATE_CNTL) {
		dev->regs.intdst[11]._rate_cntl = val;
	} else if (offset == INTDST12_RATE_CNTL) {
		dev->regs.intdst[12]._rate_cntl = val;
	} else if (offset == INTDST13_RATE_CNTL) {
		dev->regs.intdst[13]._rate_cntl = val;
	} else if (offset == INTDST14_RATE_CNTL) {
		dev->regs.intdst[14]._rate_cntl = val;
	} else if (offset == INTDST15_RATE_CNTL) {
		dev->regs.intdst[15]._rate_cntl = val;
	} else if (offset == INTDST_RATE_DIS) {
		dev->regs.intdst_rate_dis = val;
	} else if ((offset >= RXU_MAP00_L) && (offset <= RXU_MAP63_QID)) {
		uint32_t num = offset - RXU_MAP00_L;
		uint32_t t = num % 12;
		uint32_t i = num / 12;

		switch (t){
		case 0:
			dev->regs.rxu_map[i]._l = val;
			break;
		case 4:
			dev->regs.rxu_map[i]._h = val;
			break;
		case 8:
			dev->regs.rxu_map[i]._qid = val;
			break;
		}
	} else if ((offset >= RXU_TYPE9_MAP00_0) && (offset == RXU_TYPE9_MAP63_2)) {
		uint32_t num = offset - RXU_TYPE9_MAP00_0;
		uint32_t t = num % 12;
		uint32_t i = num / 12;
		switch (t){
		case 0:
			dev->regs.rxu_type9_map[i]._[0] = val;
			break;
		case 4:
			dev->regs.rxu_type9_map[i]._[1] = val;
			break;
		case 8:
			dev->regs.rxu_type9_map[i]._[2] = val;
			break;
		}
	} else if ((offset >= LSU1_REG0) && (offset <= LSU8_REG6))	 {
		uint32_t num = offset - LSU1_REG0;
		uint32_t t = num % 28;  // 7(0-6) * 4 = 28
		uint32_t i = num / 28;
		i += 1;                 // index: 1 to 8; 0 is NULL
		switch (t){
		case 0:
			dev->regs.lsu[i]._reg[0] = val;
			break;
		case 4:
			dev->regs.lsu[i]._reg[1] = val;
			break;
		case 8:
			dev->regs.lsu[i]._reg[2] = val;
			break;
		case 12:
			dev->regs.lsu[i]._reg[3] = val;
			break;
		case 16:
			dev->regs.lsu[i]._reg[4] = val;
			break;
		case 20:
			dev->regs.lsu[i]._reg[5] = val;
			break;
		case 24:
			dev->regs.lsu[i]._reg[7] = val;
			break;
		}
	} else if (offset == LSU_SETUP_REG0) {
		dev->regs.lsu_setup_reg0 = val;
	} else if (offset == LSU_SETUP_REG1) {
		dev->regs.lsu_setup_reg1 = val;
	} else if ((offset >= LSU_STAT_REG0) && (offset <= LSU_STAT_REG5)) {
		dev->regs.lsu_stat_reg[(offset - LSU_STAT_REG0) / 4] = val;
	} else if ((offset >= LSU_FLOW_MASKS0) && (offset <= LSU_FLOW_MASKS3)) {
		dev->regs.lsu_flow_masks[(offset - LSU_FLOW_MASKS0) / 4] = val;
	} else if (offset == SUPRVSR_ID) {
		dev->regs.suprvsr_id = val;
	} else if ((offset >= FLOW_CNTL0) && (offset <= FLOW_CNTL15)) {
		dev->regs.flow_cntl[(offset - FLOW_CNTL0) / 4] = val;
	} else if ((offset >= TX_CPPI_FLOW_MASKS0) && (offset <= TX_CPPI_FLOW_MASKS7)) {
		dev->regs.tx_cppi_flow_masks[(offset - TX_CPPI_FLOW_MASKS0) / 4] = val;
	} else if ((offset >= TX_QUEUE_SCH_INFO1) && (offset <= TX_QUEUE_SCH_INFO4)) {
		dev->regs.tx_queue_sch_info[((offset - TX_QUEUE_SCH_INFO1) / 4) + 1] = val;
	} else if ((offset >= GARBAGE_COLL_QID0) && (offset <= GARBAGE_COLL_QID2)) {
		dev->regs.garbage_coll_qid[(offset - GARBAGE_COLL_QID0) / 4] = val;
	} else if ((offset >= TCHAN_GCFG_REG_A0) && (offset <= TCHAN_GCFG_REG_B15)) {
		uint32_t num = offset - TCHAN_GCFG_REG_A0;
		uint32_t t = num % 8;
		uint32_t i = num / 8;
		switch (t){
		case 0:
			dev->regs.tchan_gcfg_reg_a[i] = val;
			break;
		case 4:
			dev->regs.tchan_gcfg_reg_b[i] = val;
			break;
		}
	} else if ((offset >= RCHAN_GCFG_REG_A0) && (offset <= RCHAN_GCFG_REG_A15)) {
		dev->regs.rchan_gcfg_reg_a[(offset - RCHAN_GCFG_REG_A0) / 4] = val;
	} else if ((offset >= TCHAN_SCHED_CFG_REG0) && (offset <= TCHAN_SCHED_CFG_REG15)) {
		dev->regs.tchan_sched_cfg_reg[(offset - TCHAN_SCHED_CFG_REG0) / 4] = val;
	} else if ((offset >= RFLOW_CFG_REG_A0) && (offset <= RFLOW_CFG_REG_H19)) {
		uint32_t num = offset - RFLOW_CFG_REG_A0;
		uint32_t t = num % 32;
		uint32_t i = num / 32;
		switch (t) {
		case 0:
			dev->regs.rflow_cfg_reg_a[i] = val;
			break;
		case 4:
			dev->regs.rflow_cfg_reg_b[i] = val;
			break;
		case 8:
			dev->regs.rflow_cfg_reg_c[i] = val;
			break;
		case 12:
			dev->regs.rflow_cfg_reg_d[i] = val;
			break;
		case 16:
			dev->regs.rflow_cfg_reg_e[i] = val;
			break;
		case 20:
			dev->regs.rflow_cfg_reg_f[i] = val;
			break;
		case 24:
			dev->regs.rflow_cfg_reg_g[i] = val;
			break;
		case 28:
			dev->regs.rflow_cfg_reg_h[i] = val;
			break;
		}
	} else if (offset == DEV_ID) {
		dev->regs.dev_id = val;
	} else if (offset == DEV_INFO) {
		dev->regs.dev_info = val;
	} else if (offset == ASBLY_ID) {
		dev->regs.asbly_id = val;
	} else if (offset == ASBLY_INFO) {
		dev->regs.asbly_info = val;
	} else if (offset == PE_FEAT) {
		dev->regs.pe_feat = val;
	} else if (offset == SW_PORT) {
		dev->regs.sw_port = val;
	} else if (offset == SRC_OP) {
		dev->regs.src_op = val;
	} else if (offset == DEST_OP) {
		dev->regs.dest_op = val;
	} else if (offset == DS_INFO) {
		dev->regs.ds_info = val;
	} else if (offset == DS_LL_CTL) {
		dev->regs.ds_ll_ctl = val;
	} else if (offset == PE_LL_CTL) {
		dev->regs.pe_ll_ctl = val;
	} else if (offset == LCL_CFG_HBAR) {
		dev->regs.lcl_cfg_hbar = val;
	} else if (offset == LCL_CFG_BAR) {
		dev->regs.lcl_cfg_bar = val;
	} else if (offset == BASE_ID) {
		dev->regs.base_id = val;
	} else if (offset == HOST_BASE_ID_LOCK) {
		dev->regs.host_base_id_lock = val;
	} else if (offset == COMP_TAG) {
		dev->regs.comp_tag = val;
	} else if (offset == SP_MB_HEAD) {
		dev->regs.sp_mb_head = val;
	} else if (offset == SP_LT_CTL) {
		dev->regs.sp_lt_ctl = val;
	} else if (offset == SP_RT_CTL) {
		dev->regs.sp_rt_ctl = val;
	} else if (offset == SP_GEN_CTL) {
		dev->regs.sp_gen_ctl = val;
	} else if ((offset >= SP0_LM_REQ) && (offset <= SP3_CTL)) {
		uint32_t num = offset - SP0_LM_REQ;
		uint32_t t = num % 28;
		uint32_t i = num / 28;
		switch (t) {
		case 0:
			dev->regs.sp[i]._lm_req = val;
			break;
		case 4:
			dev->regs.sp[i]._lm_resp = val;
			break;
		case 8:
			dev->regs.sp[i]._ackid_stat = val;
			break;
		case 12:
		case 16:
			break;
		case 20:
			dev->regs.sp[i]._ctl2 = val;
			break;
		case 24:
			dev->regs.sp[i]._err_stat = val;
			exit(-1);
			break;
		case 28:
			dev->regs.sp[i]._ctl = val;
			break;
		}
	} else if (offset == ERR_RPT_BH) {
		dev->regs.err_rpt_bh = val;
	} else if (offset == ERR_DET) {
		dev->regs.err_det = val;
	} else if (offset == ERR_EN) {
		dev->regs.err_en = val;
	} else if (offset == H_ADDR_CAPT) {
		dev->regs.h_addr_capt = val;
	} else if (offset == ADDR_CAPT) {
		dev->regs.addr_capt = val;
	} else if (offset == ID_CAPT) {
		dev->regs.id_capt = val;
	} else if (offset == CTRL_CAPT) {
		dev->regs.ctrl_capt = val;
	} else if (offset == PW_TGT_ID) {
		dev->regs.pw_tgt_id = val;
	} else if ((offset >= SP0_ERR_DET) && (offset <= SP3_ERR_THRESH)) {
		uint32_t num = offset - SP0_ERR_DET;
		uint32_t t = num % 36; // (9 * 4)
		uint32_t i = num / 36; // (9 * 4)
		switch (t) {
		case 0:  // 0*4
			dev->regs.sp[i]._err_det = val;
			break;
		case 4:  // 1*4
			dev->regs.sp[i]._rate_en = val;
			break;
		case 8:  // 2*4
			dev->regs.sp[i]._err_attr_capt_dbg0 = val;
			break;
		case 12:  // 3*4
			dev->regs.sp[i]._err_capt_0_dbg1 = val;
			break;
		case 16:  // 4*4
			dev->regs.sp[i]._err_capt_1_dbg2 = val;
			break;
		case 20:  // 5*4
			dev->regs.sp[i]._err_capt_2_dbg3 = val;
			break;
		case 24:  // 6*4
			dev->regs.sp[i]._err_capt_3_dbg4 = val;
			break;
		case 28:  // 7*4
			dev->regs.sp[i]._err_rate = val;
			break;
		case 32:  // 8*4
			dev->regs.sp[i]._err_thresh = val;
			break;
		}
	} else if ((offset >= LANE0_STAT0) && (offset <= LANE3_STAT0)) {
		uint32_t num = offset - LANE0_STAT0;
		uint32_t t = num % 8;
		uint32_t i = num / 8;

		dev->regs.lane[i]._stat[t / 4] = val;
	} else if (offset == PLM_BH) {
		dev->regs.plm_bh = val;
	} else if (offset == PLM_SP0_IMP_SPEC_CTL) {
		dev->regs.plm_sp0_imp_spec_ctl = val;
	} else if (offset == RIO_PLM_SP0_PWDN_CTL) {
		dev->regs.rio_plm_sp0_pwdn_ctl = val;
	} else if (offset == PLM_SP0_STATUS) {
		dev->regs.plm_sp0_status = val;
	} else if (offset == PLM_SP0_INT_ENABLE) {
		dev->regs.plm_sp0_int_enable = val;
	} else if (offset == PLM_SP0_PW_ENABLE) {
		dev->regs.plm_sp0_pw_enable = val;
	} else if (offset == PLM_SP0_EVENT_GEN) {
		dev->regs.plm_sp0_event_gen = val;
	} else if (offset == PLM_SP0_ALL_INT_EN) {
		dev->regs.plm_sp0_all_int_en = val;
	} else if (offset == PLM_SP0_ALL_PW_EN) {
		dev->regs.plm_sp0_all_pw_en = val;
	} else if (offset == PLM_SP0_PATH_CTL) {
		dev->regs.plm_sp0_path_ctl = val;
	} else if (offset == PLM_SP0_DISCOVERY_TIMER) {
		dev->regs.plm_sp0_discovery_timer = val;
	} else if (offset == PLM_SP0_SILENCE_TIMER) {
		dev->regs.plm_sp0_silence_timer = val;
	} else if (offset == PLM_SP0_VMIN_EXP) {
		dev->regs.plm_sp0_vmin_exp = val;
	} else if (offset == PLM_SP0_POL_CTL) {
		dev->regs.plm_sp0_pol_ctl = val;
	} else if (offset == PLM_SP0_DENIAL_CTL) {
		dev->regs.plm_sp0_denial_ctl = val;
	} else if (offset == PLM_SP0_RCVD_MECS) {
		dev->regs.plm_sp0_rcvd_mecs = val;
	} else if (offset == PLM_SP0_MECS_FWD) {
		dev->regs.plm_sp0_mecs_fwd = val;
	} else if (offset == PLM_SP0_LONG_CS_TX1) {
		dev->regs.plm_sp0_long_cs_tx1 = val;
	} else if (offset == PLM_SP0_LONG_CS_TX2) {
		dev->regs.plm_sp0_long_cs_tx2 = val;
	} else if (offset == PLM_SP1_IMP_SPEC_CTL) {
		dev->regs.plm_sp1_imp_spec_ctl = val;
	} else if (offset == RIO_PLM_SP1_PWDN_CTL) {
		dev->regs.rio_plm_sp1_pwdn_ctl = val;
	} else if (offset == PLM_SP1_STATUS) {
		dev->regs.plm_sp1_status = val;
	} else if (offset == PLM_SP1_INT_ENABLE) {
		dev->regs.plm_sp1_int_enable = val;
	} else if (offset == PLM_SP1_PW_ENABLE) {
		dev->regs.plm_sp1_pw_enable = val;
	} else if (offset == PLM_SP1_EVENT_GEN) {
		dev->regs.plm_sp1_event_gen = val;
	} else if (offset == PLM_SP1_ALL_INT_EN) {
		dev->regs.plm_sp1_all_int_en = val;
	} else if (offset == PLM_SP1_ALL_PW_EN) {
		dev->regs.plm_sp1_all_pw_en = val;
	} else if (offset == PLM_SP1_PATH_CTL) {
		dev->regs.plm_sp1_path_ctl = val;
	} else if (offset == PLM_SP1_DISCOVERY_TIMER) {
		dev->regs.plm_sp1_discovery_timer = val;
	} else if (offset == PLM_SP1_SILENCE_TIMER) {
		dev->regs.plm_sp1_silence_timer = val;
	} else if (offset == PLM_SP1_VMIN_EXP) {
		dev->regs.plm_sp1_vmin_exp = val;
	} else if (offset == PLM_SP1_POL_CTL) {
		dev->regs.plm_sp1_pol_ctl = val;
	} else if (offset == PLM_SP1_DENIAL_CTL) {
		dev->regs.plm_sp1_denial_ctl = val;
	} else if (offset == PLM_SP1_RCVD_MECS) {
		dev->regs.plm_sp1_rcvd_mecs = val;
	} else if (offset == PLM_SP1_MECS_FWD) {
		dev->regs.plm_sp1_mecs_fwd = val;
	} else if (offset == PLM_SP1_LONG_CS_TX1) {
		dev->regs.plm_sp1_long_cs_tx1 = val;
	} else if (offset == PLM_SP1_LONG_CS_TX2) {
		dev->regs.plm_sp1_long_cs_tx2 = val;
	} else if (offset == PLM_SP2_IMP_SPEC_CTL) {
		dev->regs.plm_sp2_imp_spec_ctl = val;
	} else if (offset == PLM_SP2_PWDN_CTL) {
		dev->regs.plm_sp2_pwdn_ctl = val;
	} else if (offset == PLM_SP2_STATUS) {
		dev->regs.plm_sp2_status = val;
	} else if (offset == PLM_SP2_INT_ENABLE) {
		dev->regs.plm_sp2_int_enable = val;
	} else if (offset == PLM_SP2_PW_ENABLE) {
		dev->regs.plm_sp2_pw_enable = val;
	} else if (offset == PLM_SP2_EVENT_GEN) {
		dev->regs.plm_sp2_event_gen = val;
	} else if (offset == PLM_SP2_ALL_INT_EN) {
		dev->regs.plm_sp2_all_int_en = val;
	} else if (offset == PLM_SP2_ALL_PW_EN) {
		dev->regs.plm_sp2_all_pw_en = val;
	} else if (offset == PLM_SP2_PATH_CTL) {
		dev->regs.plm_sp2_path_ctl = val;
	} else if (offset == PLM_SP2_DISCOVERY_TIMER) {
		dev->regs.plm_sp2_discovery_timer = val;
	} else if (offset == PLM_SP2_SILENCE_TIMER) {
		dev->regs.plm_sp2_silence_timer = val;
	} else if (offset == PLM_SP2_VMIN_EXP) {
		dev->regs.plm_sp2_vmin_exp = val;
	} else if (offset == PLM_SP2_POL_CTL) {
		dev->regs.plm_sp2_pol_ctl = val;
	} else if (offset == PLM_SP2_DENIAL_CTL) {
		dev->regs.plm_sp2_denial_ctl = val;
	} else if (offset == PLM_SP2_RCVD_MECS) {
		dev->regs.plm_sp2_rcvd_mecs = val;
	} else if (offset == PLM_SP2_MECS_FWD) {
		dev->regs.plm_sp2_mecs_fwd = val;
	} else if (offset == PLM_SP2_LONG_CS_TX1) {
		dev->regs.plm_sp2_long_cs_tx1 = val;
	} else if (offset == PLM_SP2_LONG_CS_TX2) {
		dev->regs.plm_sp2_long_cs_tx2 = val;
	} else if (offset == PLM_SP3_IMP_SPEC_CTL) {
		dev->regs.plm_sp3_imp_spec_ctl = val;
	} else if (offset == PLM_SP3_PWDN_CTL) {
		dev->regs.plm_sp3_pwdn_ctl = val;
	} else if (offset == PLM_SP3_STATUS) {
		dev->regs.plm_sp3_status = val;
	} else if (offset == PLM_SP3_INT_ENABLE) {
		dev->regs.plm_sp3_int_enable = val;
	} else if (offset == PLM_SP3_PW_ENABLE) {
		dev->regs.plm_sp3_pw_enable = val;
	} else if (offset == PLM_SP3_EVENT_GEN) {
		dev->regs.plm_sp3_event_gen = val;
	} else if (offset == PLM_SP3_ALL_INT_EN) {
		dev->regs.plm_sp3_all_int_en = val;
	} else if (offset == PLM_SP3_ALL_PW_EN) {
		dev->regs.plm_sp3_all_pw_en = val;
	} else if (offset == PLM_SP3_PATH_CTL) {
		dev->regs.plm_sp3_path_ctl = val;
	} else if (offset == PLM_SP3_DISCOVERY_TIMER) {
		dev->regs.plm_sp3_discovery_timer = val;
	} else if (offset == PLM_SP3_SILENCE_TIMER) {
		dev->regs.plm_sp3_silence_timer = val;
	} else if (offset == PLM_SP3_VMIN_EXP) {
		dev->regs.plm_sp3_vmin_exp = val;
	} else if (offset == PLM_SP3_POL_CTL) {
		dev->regs.plm_sp3_pol_ctl = val;
	} else if (offset == PLM_SP3_DENIAL_CTL) {
		dev->regs.plm_sp3_denial_ctl = val;
	} else if (offset == PLM_SP3_RCVD_MECS) {
		dev->regs.plm_sp3_rcvd_mecs = val;
	} else if (offset == PLM_SP3_MECS_FWD) {
		dev->regs.plm_sp3_mecs_fwd = val;
	} else if (offset == PLM_SP3_LONG_CS_TX1) {
		dev->regs.plm_sp3_long_cs_tx1 = val;
	} else if (offset == PLM_SP3_LONG_CS_TX2) {
		dev->regs.plm_sp3_long_cs_tx2 = val;
	} else if (offset == TLM_BH) {
		dev->regs.tlm_bh = val;
	} else if ((offset >= TLM_SP0_CONTROL) && (offset <= TLM_SP3_BRR_3_PATTERN_MATCH)) {
		uint32_t num = offset - TLM_SP0_CONTROL;
		uint32_t t = num % 52;   // 13 * 4
		uint32_t i = num / 52;
		switch (t) {
		case 0:
			dev->regs.tlm_sp[i]._control = val;
			break;
		case 4:
			dev->regs.tlm_sp[i]._status = val;
			break;
		case 8:
			dev->regs.tlm_sp[i]._int_enable = val;
			break;
		case 12:
			dev->regs.tlm_sp[i]._pw_enable = val;
			break;
		case 16:
			dev->regs.tlm_sp[i]._event_gen = val;
			break;
		case 20:
			dev->regs.tlm_sp[i]._brr_0_ctl = val;
			break;
		case 24:
			dev->regs.tlm_sp[i]._brr_0_pattern_match = val;
			break;
		case 28:
			dev->regs.tlm_sp[i]._brr_1_ctl = val;
			break;
		case 32:
			dev->regs.tlm_sp[i]._brr_1_pattern_match = val;
			break;
		case 36:
			dev->regs.tlm_sp[i]._brr_2_ctl = val;
			break;
		case 40:
			dev->regs.tlm_sp[i]._brr_2_pattern_match = val;
			break;
		case 44:
			dev->regs.tlm_sp[i]._brr_3_ctl = val;
			break;
		case 48:
			dev->regs.tlm_sp[i]._brr_3_pattern_match = val;
			break;
		}
	} else if (offset == PBM_BH) {
		dev->regs.pbm_bh = val;
	} else if (offset == PBM_SP0_CONTROL) {
		dev->regs.pbm_sp0_control = val;
	} else if (offset == PBM_SP0_STATUS) {
		dev->regs.pbm_sp0_status = val;
	} else if (offset == PBM_SP0_INT_ENABLE) {
		dev->regs.pbm_sp0_int_enable = val;
	} else if (offset == PBM_SP0_PW_ENABLE) {
		dev->regs.pbm_sp0_pw_enable = val;
	} else if (offset == PBM_SP0_EVENT_GEN) {
		dev->regs.pbm_sp0_event_gen = val;
	} else if (offset == PBM_SP0_IG_RESOURCES) {
		dev->regs.pbm_sp0_ig_resources = val;
	} else if (offset == PBM_SP0_EG_RESOURCES) {
		dev->regs.pbm_sp0_eg_resources = val;
	} else if (offset == PBM_SP0_IG_WATERMARK0) {
		dev->regs.pbm_sp0_ig_watermark0 = val;
	} else if (offset == PBM_SP0_IG_WATERMARK1) {
		dev->regs.pbm_sp0_ig_watermark1 = val;
	} else if (offset == PBM_SP0_IG_WATERMARK2) {
		dev->regs.pbm_sp0_ig_watermark2 = val;
	} else if (offset == PBM_SP0_IG_WATERMARK3) {
		dev->regs.pbm_sp0_ig_watermark3 = val;
	} else if (offset == PBM_SP1_CONTROL) {
		dev->regs.pbm_sp1_control = val;
	} else if (offset == PBM_SP1_STATUS) {
		dev->regs.pbm_sp1_status = val;
	} else if (offset == PBM_SP1_INT_ENABLE) {
		dev->regs.pbm_sp1_int_enable = val;
	} else if (offset == PBM_SP1_PW_ENABLE) {
		dev->regs.pbm_sp1_pw_enable = val;
	} else if (offset == PBM_SP1_EVENT_GEN) {
		dev->regs.pbm_sp1_event_gen = val;
	} else if (offset == PBM_SP1_IG_RESOURCES) {
		dev->regs.pbm_sp1_ig_resources = val;
	} else if (offset == PBM_SP1_EG_RESOURCES) {
		dev->regs.pbm_sp1_eg_resources = val;
	} else if (offset == PBM_SP1_IG_WATERMARK0) {
		dev->regs.pbm_sp1_ig_watermark0 = val;
	} else if (offset == PBM_SP1_IG_WATERMARK1) {
		dev->regs.pbm_sp1_ig_watermark1 = val;
	} else if (offset == PBM_SP1_IG_WATERMARK2) {
		dev->regs.pbm_sp1_ig_watermark2 = val;
	} else if (offset == PBM_SP1_IG_WATERMARK3) {
		dev->regs.pbm_sp1_ig_watermark3 = val;
	} else if (offset == PBM_SP2_CONTROL) {
		dev->regs.pbm_sp2_control = val;
	} else if (offset == PBM_SP2_STATUS) {
		dev->regs.pbm_sp2_status = val;
	} else if (offset == PBM_SP2_INT_ENABLE) {
		dev->regs.pbm_sp2_int_enable = val;
	} else if (offset == PBM_SP2_PW_ENABLE) {
		dev->regs.pbm_sp2_pw_enable = val;
	} else if (offset == PBM_SP2_EVENT_GEN) {
		dev->regs.pbm_sp2_event_gen = val;
	} else if (offset == PBM_SP2_IG_RESOURCES) {
		dev->regs.pbm_sp2_ig_resources = val;
	} else if (offset == PBM_SP2_EG_RESOURCES) {
		dev->regs.pbm_sp2_eg_resources = val;
	} else if (offset == PBM_SP2_IG_WATERMARK0) {
		dev->regs.pbm_sp2_ig_watermark0 = val;
	} else if (offset == PBM_SP2_IG_WATERMARK1) {
		dev->regs.pbm_sp2_ig_watermark1 = val;
	} else if (offset == PBM_SP2_IG_WATERMARK2) {
		dev->regs.pbm_sp2_ig_watermark2 = val;
	} else if (offset == PBM_SP2_IG_WATERMARK3) {
		dev->regs.pbm_sp2_ig_watermark3 = val;
	} else if (offset == PBM_SP3_CONTROL) {
		dev->regs.pbm_sp3_control = val;
	} else if (offset == PBM_SP3_STATUS) {
		dev->regs.pbm_sp3_status = val;
	} else if (offset == PBM_SP3_INT_ENABLE) {
		dev->regs.pbm_sp3_int_enable = val;
	} else if (offset == PBM_SP3_PW_ENABLE) {
		dev->regs.pbm_sp3_pw_enable = val;
	} else if (offset == PBM_SP3_EVENT_GEN) {
		dev->regs.pbm_sp3_event_gen = val;
	} else if (offset == PBM_SP3_IG_RESOURCES) {
		dev->regs.pbm_sp3_ig_resources = val;
	} else if (offset == PBM_SP3_EG_RESOURCES) {
		dev->regs.pbm_sp3_eg_resources = val;
	} else if (offset == PBM_SP3_IG_WATERMARK0) {
		dev->regs.pbm_sp3_ig_watermark0 = val;
	} else if (offset == PBM_SP3_IG_WATERMARK1) {
		dev->regs.pbm_sp3_ig_watermark1 = val;
	} else if (offset == PBM_SP3_IG_WATERMARK2) {
		dev->regs.pbm_sp3_ig_watermark2 = val;
	} else if (offset == PBM_SP3_IG_WATERMARK3) {
		dev->regs.pbm_sp3_ig_watermark3 = val;
	} else if (offset == EM_BH) {
		dev->regs.em_bh = val;
	} else if (offset == EM_INT_STAT) {
		dev->regs.em_int_stat = val;
	} else if (offset == EM_INT_ENABLE) {
		dev->regs.em_int_enable = val;
	} else if (offset == EM_INT_PORT_STAT) {
		dev->regs.em_int_port_stat = val;
	} else if (offset == EM_PW_STAT) {
		dev->regs.em_pw_stat = val;
	} else if (offset == EM_PW_EN) {
		dev->regs.em_pw_en = val;
	} else if (offset == EM_PW_PORT_STAT) {
		dev->regs.em_pw_port_stat = val;
	} else if (offset == EM_DEV_INT_EN) {
		dev->regs.em_dev_int_en = val;
	} else if (offset == EM_DEV_PW_EN) {
		dev->regs.em_dev_pw_en = val;
	} else if (offset == EM_MECS_STAT) {
		dev->regs.em_mecs_stat = val;
	} else if (offset == EM_MECS_INT_EN) {
		dev->regs.em_mecs_int_en = val;
	} else if (offset == EM_MECS_CAP_EN) {
		dev->regs.em_mecs_cap_en = val;
	} else if (offset == EM_MECS_TRIG_EN) {
		dev->regs.em_mecs_trig_en = val;
	} else if (offset == EM_MECS_REQ) {
		dev->regs.em_mecs_req = val;
	} else if (offset == EM_MECS_PORT_STAT) {
		dev->regs.em_mecs_port_stat = val;
	} else if (offset == EM_RST_PORT_STAT) {
		dev->regs.em_rst_port_stat = val;
	} else if (offset == EM_RST_INT_EN) {
		dev->regs.em_rst_int_en = val;
	} else if (offset == EM_RST_PW_EN) {
		dev->regs.em_rst_pw_en = val;
	} else if (offset == PW_BH) {
		dev->regs.pw_bh = val;
	} else if (offset == PW_CTL) {
		dev->regs.pw_ctl = val;
	} else if (offset == PW_ROUTE) {
		dev->regs.pw_route = val;
	} else if (offset == PW_RX_STAT) {
		dev->regs.pw_rx_stat = val;
	} else if (offset == RIO_PW_RX_EVENT_GEN) {
		dev->regs.rio_pw_rx_event_gen = val;
	} else if (offset == PW_RX_CAPT0) {
		dev->regs.pw_rx_capt0 = val;
	} else if (offset == PW_RX_CAPT1) {
		dev->regs.pw_rx_capt1 = val;
	} else if (offset == PW_RX_CAPT2) {
		dev->regs.pw_rx_capt2 = val;
	} else if (offset == PW_RX_CAPT3) {
		dev->regs.pw_rx_capt3 = val;
	} else if (offset == LLM_BH) {
		dev->regs.llm_bh = val;
	} else if (offset == WHITEBOARD) {
		dev->regs.whiteboard = val;
	} else if (offset == PORT_NUMBER) {
		dev->regs.port_number = val;
	} else if (offset == PRESCALAR_SRV_CLK) {
		dev->regs.prescalar_srv_clk = val;
	} else if (offset == REG_RST_CTL) {
		dev->regs.reg_rst_ctl = val;
	} else if (offset == LOCAL_ERR_DET) {
		dev->regs.local_err_det = val;
	} else if (offset == LOCAL_ERR_EN) {
		dev->regs.local_err_en = val;
	} else if (offset == LOCAL_H_ADDR_CAPT) {
		dev->regs.local_h_addr_capt = val;
	} else if (offset == LOCAL_ADDR_CAPT) {
		dev->regs.local_addr_capt = val;
	} else if (offset == LOCAL_ID_CAPT) {
		dev->regs.local_id_capt = val;
	} else if (offset == LOCAL_CTRL_CAPT) {
		dev->regs.local_ctrl_capt = val;
	} else if (offset == FABRIC_HDR) {
		dev->regs.fabric_hdr = val;
	} else if (offset == FABRIC_CSR) {
		dev->regs.fabric_csr = val;
	} else if (offset == SP0_FABRIC_STATUS) {
		dev->regs.sp0_fabric_status = val;
	} else if (offset == SP1_FABRIC_STATUS) {
		dev->regs.sp1_fabric_status = val;
	} else if (offset == SP2_FABRIC_STATUS) {
		dev->regs.sp2_fabric_status = val;
	} else if (offset == SP3_FABRIC_STATUS) {
		dev->regs.sp3_fabric_status = val;
	//default) {
	} else {
		skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in srio_c6678\n", offset);
	}
	return 0;
}

void srio_packet_dma_write(conf_object_t *obj, srio_msg_t msg, int intd_trigger)
{
	srio_c6678_device *dev = obj->obj;
	int i;
	if ((msg.ide == 0) && (msg.dlc != 0) && (msg.dlc <= 4096)) {
		if (dev->image_iface) {
			for (i = 0; i < msg.dlc; i++) {
				dev->image_iface->write(dev->image, msg.addr + i, &(msg.data[i]), 1);
			}
			if (intd_trigger == 1) {
				int core = 0;
				general_signal_raise((conf_object_t *)obj, 0x53, core);
			}
		}
	}
}

static exception_t receive_data(conf_object_t *obj, void *buf, uint32_t cnt, int intd_trigger)
{
	srio_c6678_device *dev = obj->obj;

	srio_msg_t msg;
	memcpy(&msg, buf, cnt);

/*
	printf("id: %d ide: %d dlc: %d\n", msg.id, msg.ide, msg.dlc);

	char *str = (char *)msg.data;
	int i = 0;
	for (i = 0; i < msg.dlc; i++) {
		printf("0x%02x  ", *str++ & 0xff);
		if ((i + 1) % 16 == 0) printf("\n");
	}
	printf("\n");
*/
	srio_packet_dma_write(obj, msg, intd_trigger);

	return No_exp;
}

static conf_object_t* new_srio_c6678(char *obj_name)
{
	srio_c6678_device* dev = skyeye_mm_zero(sizeof(srio_c6678_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	int i;
	for (i = 0; i < 4; i++) {
		regs->sp[i]._err_stat = 0x2;
	}

	return dev->obj;
}

static exception_t free_srio_c6678(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t memory_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	srio_c6678_device *dev = obj->obj;
	dev->image = obj2;
	dev->image_iface = (memory_space_intf*)SKY_get_iface(dev->image, MEMORY_SPACE_INTF_NAME);
	return No_exp;
}

static exception_t memory_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	srio_c6678_device *dev = obj->obj;
	*obj2 = dev->image;
	*port = NULL;
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	srio_c6678_device *dev = obj->obj;
	dev->general_core = obj2;
	dev->general_signal = (general_signal_intf *)SKY_get_iface(dev->general_core, GENERAL_SIGNAL_INTF_NAME);
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	srio_c6678_device *dev = obj->obj;
	*obj2 = dev->general_core;
	*port = NULL;
	return No_exp;
}

void init_srio_c6678(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6678_srio",
		.class_desc = "c6678_srio",
		.new_instance = new_srio_c6678,
		.free_instance = free_srio_c6678
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = srio_c6678_read,
		.write = srio_c6678_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_srio_device_t srio_device = {
		.write_data = receive_data,
	};
	SKY_register_iface(clss, SRIO_DEVICE_INTERFACE, &srio_device);

	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SRIO_DEVICE_INTERFACE,
			.iface = &srio_device
		},
	};
	static const struct ConnectDescription connects[] =
	{
		(struct ConnectDescription ){
			.name = MEMORY_SPACE_INTF_NAME,
			.set = memory_set,
			.get = memory_get,
		},
		(struct ConnectDescription ){
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
