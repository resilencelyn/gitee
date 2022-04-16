#ifndef __SRIO_C6678_H__
#define __SRIO_C6678_H__

struct registers  {
	uint32_t pid;
	uint32_t pcr;
	uint32_t per_set_cntl;
	uint32_t per_set_cntl1;
	uint32_t gbl_en;
	uint32_t gbl_en_stat;
	struct {
		uint32_t _en_stat;
		uint32_t _en;
	} blk[10];
	uint32_t blk9_en_stat;
	uint32_t multiid_reg[8];
	uint32_t pf_16b_cntl[8];
	uint32_t pf_8b_cntl[8];
	uint32_t reserved;
	struct {
		uint32_t _icsr;
		uint32_t _iccr;
		uint32_t _icrr;
		uint32_t _icrr2;
	}doorbell[4];
	uint32_t lsu0_icsr;
	uint32_t lsu0_iccr;
	uint32_t lsu1_icsr;
	uint32_t lsu1_iccr;
	uint32_t err_rst_evnt_icsr;
	uint32_t err_rst_evnt_iccr;
	uint32_t lsu0_icrr1;
	uint32_t lsu0_icrr2;
	uint32_t lsu0_icrr3;
	uint32_t lsu0_icrr4;
	uint32_t lsu1_icrr1;
	uint32_t err_rst_evnt_icrr;
	uint32_t err_rst_evnt_icrr2;
	uint32_t err_rst_evnt_icrr3;
	uint32_t interrupt_ctl;
	struct {
		uint32_t _decode;    // 24
		uint32_t _rate_cntl; // 16
	} intdst[23];
	uint32_t intdst_rate_dis;
	struct {
		uint32_t _l;
		uint32_t _h;
		uint32_t _qid;
	} rxu_map[64];
	struct {
		uint32_t _[3];
	} rxu_type9_map[64];
	struct {
		uint32_t _reg[7];
	} lsu[9];    // index: 1 to 8; 0 is NULL
	uint32_t lsu_setup_reg0;
	uint32_t lsu_setup_reg1;
	uint32_t lsu_stat_reg[6];
	uint32_t lsu_flow_masks[4];
	uint32_t suprvsr_id;
	uint32_t flow_cntl[16];
	uint32_t tx_cppi_flow_masks[8];
	uint32_t tx_queue_sch_info[5]; // index: 1 to 4; 0 is NULL
	uint32_t garbage_coll_qid[3];
	uint32_t tchan_gcfg_reg_a[16];
	uint32_t tchan_gcfg_reg_b[16];
	uint32_t rchan_gcfg_reg_a[16];
	uint32_t tchan_sched_cfg_reg[16];
	uint32_t rflow_cfg_reg_a[20];
	uint32_t rflow_cfg_reg_b[20];
	uint32_t rflow_cfg_reg_c[20];
	uint32_t rflow_cfg_reg_d[20];
	uint32_t rflow_cfg_reg_e[20];
	uint32_t rflow_cfg_reg_f[20];
	uint32_t rflow_cfg_reg_g[20];
	uint32_t rflow_cfg_reg_h[20];
	uint32_t dev_id;
	uint32_t dev_info;
	uint32_t asbly_id;
	uint32_t asbly_info;
	uint32_t pe_feat;
	uint32_t sw_port;
	uint32_t src_op;
	uint32_t dest_op;
	uint32_t ds_info;
	uint32_t ds_ll_ctl;
	uint32_t pe_ll_ctl;
	uint32_t lcl_cfg_hbar;
	uint32_t lcl_cfg_bar;
	uint32_t base_id;
	uint32_t host_base_id_lock;
	uint32_t comp_tag;
	uint32_t sp_mb_head;
	uint32_t sp_lt_ctl;
	uint32_t sp_rt_ctl;
	uint32_t sp_gen_ctl;
	struct {
		uint32_t _lm_req;
		uint32_t _lm_resp;
		uint32_t _ackid_stat;
		uint32_t _ctl2;
		uint32_t _err_stat;
		uint32_t _ctl;

		uint32_t _err_det;
		uint32_t _rate_en;
		uint32_t _err_attr_capt_dbg0;
		uint32_t _err_capt_0_dbg1;
		uint32_t _err_capt_1_dbg2;
		uint32_t _err_capt_2_dbg3;
		uint32_t _err_capt_3_dbg4;
		uint32_t _err_rate;
		uint32_t _err_thresh;
	} sp[4];
	uint32_t err_rpt_bh;
	uint32_t err_det;
	uint32_t err_en;
	uint32_t h_addr_capt;
	uint32_t addr_capt;
	uint32_t id_capt;
	uint32_t ctrl_capt;
	uint32_t pw_tgt_id;
	struct {
		uint32_t _stat[2];
	} lane[4];
	uint32_t plm_bh;
	uint32_t plm_sp0_imp_spec_ctl;
	uint32_t rio_plm_sp0_pwdn_ctl;
	uint32_t plm_sp0_status;
	uint32_t plm_sp0_int_enable;
	uint32_t plm_sp0_pw_enable;
	uint32_t plm_sp0_event_gen;
	uint32_t plm_sp0_all_int_en;
	uint32_t plm_sp0_all_pw_en;
	uint32_t plm_sp0_path_ctl;
	uint32_t plm_sp0_discovery_timer;
	uint32_t plm_sp0_silence_timer;
	uint32_t plm_sp0_vmin_exp;
	uint32_t plm_sp0_pol_ctl;
	uint32_t plm_sp0_denial_ctl;
	uint32_t plm_sp0_rcvd_mecs;
	uint32_t plm_sp0_mecs_fwd;
	uint32_t plm_sp0_long_cs_tx1;
	uint32_t plm_sp0_long_cs_tx2;
	uint32_t plm_sp1_imp_spec_ctl;
	uint32_t rio_plm_sp1_pwdn_ctl;
	uint32_t plm_sp1_status;
	uint32_t plm_sp1_int_enable;
	uint32_t plm_sp1_pw_enable;
	uint32_t plm_sp1_event_gen;
	uint32_t plm_sp1_all_int_en;
	uint32_t plm_sp1_all_pw_en;
	uint32_t plm_sp1_path_ctl;
	uint32_t plm_sp1_discovery_timer;
	uint32_t plm_sp1_silence_timer;
	uint32_t plm_sp1_vmin_exp;
	uint32_t plm_sp1_pol_ctl;
	uint32_t plm_sp1_denial_ctl;
	uint32_t plm_sp1_rcvd_mecs;
	uint32_t plm_sp1_mecs_fwd;
	uint32_t plm_sp1_long_cs_tx1;
	uint32_t plm_sp1_long_cs_tx2;
	uint32_t plm_sp2_imp_spec_ctl;
	uint32_t plm_sp2_pwdn_ctl;
	uint32_t plm_sp2_status;
	uint32_t plm_sp2_int_enable;
	uint32_t plm_sp2_pw_enable;
	uint32_t plm_sp2_event_gen;
	uint32_t plm_sp2_all_int_en;
	uint32_t plm_sp2_all_pw_en;
	uint32_t plm_sp2_path_ctl;
	uint32_t plm_sp2_discovery_timer;
	uint32_t plm_sp2_silence_timer;
	uint32_t plm_sp2_vmin_exp;
	uint32_t plm_sp2_pol_ctl;
	uint32_t plm_sp2_denial_ctl;
	uint32_t plm_sp2_rcvd_mecs;
	uint32_t plm_sp2_mecs_fwd;
	uint32_t plm_sp2_long_cs_tx1;
	uint32_t plm_sp2_long_cs_tx2;
	uint32_t plm_sp3_imp_spec_ctl;
	uint32_t plm_sp3_pwdn_ctl;
	uint32_t plm_sp3_status;
	uint32_t plm_sp3_int_enable;
	uint32_t plm_sp3_pw_enable;
	uint32_t plm_sp3_event_gen;
	uint32_t plm_sp3_all_int_en;
	uint32_t plm_sp3_all_pw_en;
	uint32_t plm_sp3_path_ctl;
	uint32_t plm_sp3_discovery_timer;
	uint32_t plm_sp3_silence_timer;
	uint32_t plm_sp3_vmin_exp;
	uint32_t plm_sp3_pol_ctl;
	uint32_t plm_sp3_denial_ctl;
	uint32_t plm_sp3_rcvd_mecs;
	uint32_t plm_sp3_mecs_fwd;
	uint32_t plm_sp3_long_cs_tx1;
	uint32_t plm_sp3_long_cs_tx2;
	uint32_t tlm_bh;
	struct {
		uint32_t _control;
		uint32_t _status;
		uint32_t _int_enable;
		uint32_t _pw_enable;
		uint32_t _event_gen;
		uint32_t _brr_0_ctl;
		uint32_t _brr_0_pattern_match;
		uint32_t _brr_1_ctl;
		uint32_t _brr_1_pattern_match;
		uint32_t _brr_2_ctl;
		uint32_t _brr_2_pattern_match;
		uint32_t _brr_3_ctl;
		uint32_t _brr_3_pattern_match;
	} tlm_sp[4];
	uint32_t pbm_bh;
	uint32_t pbm_sp0_control;
	uint32_t pbm_sp0_status;
	uint32_t pbm_sp0_int_enable;
	uint32_t pbm_sp0_pw_enable;
	uint32_t pbm_sp0_event_gen;
	uint32_t pbm_sp0_ig_resources;
	uint32_t pbm_sp0_eg_resources;
	uint32_t pbm_sp0_ig_watermark0;
	uint32_t pbm_sp0_ig_watermark1;
	uint32_t pbm_sp0_ig_watermark2;
	uint32_t pbm_sp0_ig_watermark3;
	uint32_t pbm_sp1_control;
	uint32_t pbm_sp1_status;
	uint32_t pbm_sp1_int_enable;
	uint32_t pbm_sp1_pw_enable;
	uint32_t pbm_sp1_event_gen;
	uint32_t pbm_sp1_ig_resources;
	uint32_t pbm_sp1_eg_resources;
	uint32_t pbm_sp1_ig_watermark0;
	uint32_t pbm_sp1_ig_watermark1;
	uint32_t pbm_sp1_ig_watermark2;
	uint32_t pbm_sp1_ig_watermark3;
	uint32_t pbm_sp2_control;
	uint32_t pbm_sp2_status;
	uint32_t pbm_sp2_int_enable;
	uint32_t pbm_sp2_pw_enable;
	uint32_t pbm_sp2_event_gen;
	uint32_t pbm_sp2_ig_resources;
	uint32_t pbm_sp2_eg_resources;
	uint32_t pbm_sp2_ig_watermark0;
	uint32_t pbm_sp2_ig_watermark1;
	uint32_t pbm_sp2_ig_watermark2;
	uint32_t pbm_sp2_ig_watermark3;
	uint32_t pbm_sp3_control;
	uint32_t pbm_sp3_status;
	uint32_t pbm_sp3_int_enable;
	uint32_t pbm_sp3_pw_enable;
	uint32_t pbm_sp3_event_gen;
	uint32_t pbm_sp3_ig_resources;
	uint32_t pbm_sp3_eg_resources;
	uint32_t pbm_sp3_ig_watermark0;
	uint32_t pbm_sp3_ig_watermark1;
	uint32_t pbm_sp3_ig_watermark2;
	uint32_t pbm_sp3_ig_watermark3;
	uint32_t em_bh;
	uint32_t em_int_stat;
	uint32_t em_int_enable;
	uint32_t em_int_port_stat;
	uint32_t em_pw_stat;
	uint32_t em_pw_en;
	uint32_t em_pw_port_stat;
	uint32_t em_dev_int_en;
	uint32_t em_dev_pw_en;
	uint32_t em_mecs_stat;
	uint32_t em_mecs_int_en;
	uint32_t em_mecs_cap_en;
	uint32_t em_mecs_trig_en;
	uint32_t em_mecs_req;
	uint32_t em_mecs_port_stat;
	uint32_t em_rst_port_stat;
	uint32_t em_rst_int_en;
	uint32_t em_rst_pw_en;
	uint32_t pw_bh;
	uint32_t pw_ctl;
	uint32_t pw_route;
	uint32_t pw_rx_stat;
	uint32_t rio_pw_rx_event_gen;
	uint32_t pw_rx_capt0;
	uint32_t pw_rx_capt1;
	uint32_t pw_rx_capt2;
	uint32_t pw_rx_capt3;
	uint32_t llm_bh;
	uint32_t whiteboard;
	uint32_t port_number;
	uint32_t prescalar_srv_clk;
	uint32_t reg_rst_ctl;
	uint32_t local_err_det;
	uint32_t local_err_en;
	uint32_t local_h_addr_capt;
	uint32_t local_addr_capt;
	uint32_t local_id_capt;
	uint32_t local_ctrl_capt;
	uint32_t fabric_hdr;
	uint32_t fabric_csr;
	uint32_t sp0_fabric_status;
	uint32_t sp1_fabric_status;
	uint32_t sp2_fabric_status;
	uint32_t sp3_fabric_status;
};

typedef struct {
	uint32_t id;			// 本设备ID
	uint32_t ide;			// 接收设备ID
	uint32_t dlc;			// 数据长度
	uint32_t addr;			// 接收地址
	uint8_t  data[4096];	// 最大 256 * 16
} srio_msg_t;

struct c6678_device
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	conf_object_t* general_core;
	general_signal_intf* general_signal;

	memory_space_intf   * image_iface;
	conf_object_t       * image;

	struct registers regs;
};

typedef struct c6678_device srio_c6678_device;

#define PID								0x0000
#define PCR								0x0004
#define PER_SET_CNTL					0x0014
#define PER_SET_CNTL1					0x0018
#define GBL_EN							0x0024
#define GBL_EN_STAT						0x0028

#define BLK0_EN							0x002C
#define BLK0_EN_STAT					0x0030
#define BLK1_EN							0x0034
#define BLK1_EN_STAT					0x0038
#define BLK2_EN							0x003C
#define BLK2_EN_STAT					0x0040
#define BLK3_EN							0x0044
#define BLK3_EN_STAT					0x0048
#define BLK4_EN							0x004C
#define BLK4_EN_STAT					0x0050
#define BLK5_EN							0x0054
#define BLK5_EN_STAT					0x0058
#define BLK6_EN							0x005C
#define BLK6_EN_STAT					0x0060
#define BLK7_EN							0x0064
#define BLK7_EN_STAT					0x0068
#define BLK8_EN							0x006C
#define BLK8_EN_STAT					0x0070
#define BLK9_EN							0x0074
#define BLK9_EN_STAT					0x0078

#define MULTIID_REG1					0x00C0
#define MULTIID_REG2					0x00C4
#define MULTIID_REG3					0x00C8
#define MULTIID_REG4					0x00CC
#define MULTIID_REG5					0x00D0
#define MULTIID_REG6					0x00D4
#define MULTIID_REG7					0x00D8
#define MULTIID_REG8					0x00DC

#define PF_16B_CNTL0					0x00E0
#define PF_8B_CNTL0						0x00E4
#define PF_16B_CNTL1					0x00E8
#define PF_8B_CNTL1						0x00EC
#define PF_16B_CNTL2					0x00F0
#define PF_8B_CNTL2						0x00F4
#define PF_16B_CNTL3					0x00F8
#define PF_8B_CNTL3						0x00FC
#define PF_16B_CNTL4					0x0100
#define PF_8B_CNTL4						0x0104
#define PF_16B_CNTL5					0x0108
#define PF_8B_CNTL5						0x010C
#define PF_16B_CNTL6					0x0110
#define PF_8B_CNTL6						0x0114
#define PF_16B_CNTL7					0x0118
#define PF_8B_CNTL7						0x011C

#define RESERVED						0x0174

#define DOORBELL0_ICSR					0x0180
#define DOORBELL0_ICCR					0x0188
#define DOORBELL1_ICSR					0x0190
#define DOORBELL1_ICCR					0x0198
#define DOORBELL2_ICSR					0x01A0
#define DOORBELL2_ICCR					0x01A8
#define DOORBELL3_ICSR					0x01B0
#define DOORBELL3_ICCR					0x01B8

#define LSU0_ICSR						0x01C0
#define LSU0_ICCR						0x01C8
#define LSU1_ICSR						0x01D0
#define LSU1_ICCR						0x01D8

#define ERR_RST_EVNT_ICSR				0x01E0
#define ERR_RST_EVNT_ICCR				0x01E8

#define DOORBELL0_ICRR					0x0200
#define DOORBELL0_ICRR2					0x0204
#define DOORBELL1_ICRR					0x020C
#define DOORBELL1_ICRR2					0x0210
#define DOORBELL2_ICRR					0x0218
#define DOORBELL2_ICRR2					0x021C
#define DOORBELL3_ICRR					0x0224
#define DOORBELL3_ICRR2					0x0228

#define LSU0_ICRR1						0x0230
#define LSU0_ICRR2						0x0234
#define LSU0_ICRR3						0x0238
#define LSU0_ICRR4						0x023C
#define LSU1_ICRR1						0x0240

#define ERR_RST_EVNT_ICRR				0x0250
#define ERR_RST_EVNT_ICRR2				0x0254
#define ERR_RST_EVNT_ICRR3				0x0258

#define INTERRUPT_CTL					0x0264

#define INTDST0_DECODE					0x0270
#define INTDST1_DECODE					0x0274
#define INTDST2_DECODE					0x0278
#define INTDST3_DECODE					0x027C
#define INTDST4_DECODE					0x0280
#define INTDST5_DECODE					0x0284
#define INTDST6_DECODE					0x0288
#define INTDST7_DECODE					0x028C
#define INTDST8_DECODE					0x0290
#define INTDST9_DECODE					0x0294
#define INTDST10_DECODE					0x0298
#define INTDST11_DECODE					0x029C
#define INTDST12_DECODE					0x02A0
#define INTDST13_DECODE					0x02A4
#define INTDST14_DECODE					0x02A8
#define INTDST15_DECODE					0x02AC
#define INTDST16_DECODE					0x02B0
#define INTDST17_DECODE					0x02B4
#define INTDST18_DECODE					0x02B8
#define INTDST19_DECODE					0x02BC
#define INTDST20_DECODE					0x02C0
#define INTDST21_DECODE					0x02C4
#define INTDST22_DECODE					0x02C8
#define INTDST23_DECODE					0x02CC

#define INTDST0_RATE_CNTL				0x02D0
#define INTDST1_RATE_CNTL				0x02D4
#define INTDST2_RATE_CNTL				0x02D8
#define INTDST3_RATE_CNTL				0x02DC
#define INTDST4_RATE_CNTL				0x02E0
#define INTDST5_RATE_CNTL				0x02E4
#define INTDST6_RATE_CNTL				0x02E8
#define INTDST7_RATE_CNTL				0x02EC
#define INTDST8_RATE_CNTL				0x02F0
#define INTDST9_RATE_CNTL				0x02F4
#define INTDST10_RATE_CNTL				0x02F8
#define INTDST11_RATE_CNTL				0x02FC
#define INTDST12_RATE_CNTL				0x0300
#define INTDST13_RATE_CNTL				0x0304
#define INTDST14_RATE_CNTL				0x0308
#define INTDST15_RATE_CNTL				0x030C

#define INTDST_RATE_DIS					0x0310

#define RXU_MAP00_L						0x0400
#define RXU_MAP00_H						0x0404
#define RXU_MAP00_QID					0x0408
#define RXU_MAP01_L						0x040C
#define RXU_MAP01_H						0x0410
#define RXU_MAP01_QID					0x0414
#define RXU_MAP02_L						0x0418
#define RXU_MAP02_H						0x041C
#define RXU_MAP02_QID					0x0420
#define RXU_MAP03_L						0x0424
#define RXU_MAP03_H						0x0428
#define RXU_MAP03_QID					0x042C
#define RXU_MAP04_L						0x0430
#define RXU_MAP04_H						0x0434
#define RXU_MAP04_QID					0x0438
#define RXU_MAP05_L						0x043C
#define RXU_MAP05_H						0x0440
#define RXU_MAP05_QID					0x0444
#define RXU_MAP06_L						0x0448
#define RXU_MAP06_H						0x044C
#define RXU_MAP06_QID					0x0450
#define RXU_MAP07_L						0x0454
#define RXU_MAP07_H						0x0458
#define RXU_MAP07_QID					0x045C
#define RXU_MAP08_L						0x0460
#define RXU_MAP08_H						0x0464
#define RXU_MAP08_QID					0x0468
#define RXU_MAP09_L						0x046C
#define RXU_MAP09_H						0x0470
#define RXU_MAP09_QID					0x0474
#define RXU_MAP10_L						0x0478
#define RXU_MAP10_H						0x047C
#define RXU_MAP10_QID					0x0480
#define RXU_MAP11_L						0x0484
#define RXU_MAP11_H						0x0488
#define RXU_MAP11_QID					0x048C
#define RXU_MAP12_L						0x0490
#define RXU_MAP12_H						0x0494
#define RXU_MAP12_QID					0x0498
#define RXU_MAP13_L						0x049C
#define RXU_MAP13_H						0x04A0
#define RXU_MAP13_QID					0x04A4
#define RXU_MAP14_L						0x04A8
#define RXU_MAP14_H						0x04AC
#define RXU_MAP14_QID					0x04B0
#define RXU_MAP15_L						0x04B4
#define RXU_MAP15_H						0x04B8
#define RXU_MAP15_QID					0x04BC
#define RXU_MAP16_L						0x04C0
#define RXU_MAP16_H						0x04C4
#define RXU_MAP16_QID					0x04C8
#define RXU_MAP17_L						0x04CC
#define RXU_MAP17_H						0x04D0
#define RXU_MAP17_QID					0x04D4
#define RXU_MAP18_L						0x04D8
#define RXU_MAP18_H						0x04DC
#define RXU_MAP18_QID					0x04E0
#define RXU_MAP19_L						0x04E4
#define RXU_MAP19_H						0x04E8
#define RXU_MAP19_QID					0x04EC
#define RXU_MAP20_L						0x04F0
#define RXU_MAP20_H						0x04F4
#define RXU_MAP20_QID					0x04F8
#define RXU_MAP21_L						0x04FC
#define RXU_MAP21_H						0x0500
#define RXU_MAP21_QID					0x0504
#define RXU_MAP22_L						0x0508
#define RXU_MAP22_H						0x050C
#define RXU_MAP22_QID					0x0510
#define RXU_MAP23_L						0x0514
#define RXU_MAP23_H						0x0518
#define RXU_MAP23_QID					0x051C
#define RXU_MAP24_L						0x0520
#define RXU_MAP24_H						0x0524
#define RXU_MAP24_QID					0x0528
#define RXU_MAP25_L						0x052C
#define RXU_MAP25_H						0x0530
#define RXU_MAP25_QID					0x0534
#define RXU_MAP26_L						0x0538
#define RXU_MAP26_H						0x053C
#define RXU_MAP26_QID					0x0540
#define RXU_MAP27_L						0x0544
#define RXU_MAP27_H						0x0548
#define RXU_MAP27_QID					0x054C
#define RXU_MAP28_L						0x0550
#define RXU_MAP28_H						0x0554
#define RXU_MAP28_QID					0x0558
#define RXU_MAP29_L						0x055C
#define RXU_MAP29_H						0x0560
#define RXU_MAP29_QID					0x0564
#define RXU_MAP30_L						0x0568
#define RXU_MAP30_H						0x056C
#define RXU_MAP30_QID					0x0570
#define RXU_MAP31_L						0x0574
#define RXU_MAP31_H						0x0578
#define RXU_MAP31_QID					0x057C
#define RXU_MAP32_L						0x0580
#define RXU_MAP32_H						0x0584
#define RXU_MAP32_QID					0x0588
#define RXU_MAP33_L						0x058C
#define RXU_MAP33_H						0x0590
#define RXU_MAP33_QID					0x0594
#define RXU_MAP34_L						0x0598
#define RXU_MAP34_H						0x059C
#define RXU_MAP34_QID					0x05A0
#define RXU_MAP35_L						0x05A4
#define RXU_MAP35_H						0x05A8
#define RXU_MAP35_QID					0x05AC
#define RXU_MAP36_L						0x05B0
#define RXU_MAP36_H						0x05B4
#define RXU_MAP36_QID					0x05B8
#define RXU_MAP37_L						0x05BC
#define RXU_MAP37_H						0x05C0
#define RXU_MAP37_QID					0x05C4
#define RXU_MAP38_L						0x05C8
#define RXU_MAP38_H						0x05CC
#define RXU_MAP38_QID					0x05D0
#define RXU_MAP39_L						0x05D4
#define RXU_MAP39_H						0x05D8
#define RXU_MAP39_QID					0x05DC
#define RXU_MAP40_L						0x05E0
#define RXU_MAP40_H						0x05E4
#define RXU_MAP40_QID					0x05E8
#define RXU_MAP41_L						0x05EC
#define RXU_MAP41_H						0x05F0
#define RXU_MAP41_QID					0x05F4
#define RXU_MAP42_L						0x05F8
#define RXU_MAP42_H						0x05FC
#define RXU_MAP42_QID					0x0600
#define RXU_MAP43_L						0x0604
#define RXU_MAP43_H						0x0608
#define RXU_MAP43_QID					0x060C
#define RXU_MAP44_L						0x0610
#define RXU_MAP44_H						0x0614
#define RXU_MAP44_QID					0x0618
#define RXU_MAP45_L						0x061C
#define RXU_MAP45_H						0x0620
#define RXU_MAP45_QID					0x0624
#define RXU_MAP46_L						0x0628
#define RXU_MAP46_H						0x062C
#define RXU_MAP46_QID					0x0630
#define RXU_MAP47_L						0x0634
#define RXU_MAP47_H						0x0638
#define RXU_MAP47_QID					0x063C
#define RXU_MAP48_L						0x0640
#define RXU_MAP48_H						0x0644
#define RXU_MAP48_QID					0x0648
#define RXU_MAP49_L						0x064C
#define RXU_MAP49_H						0x0650
#define RXU_MAP49_QID					0x0654
#define RXU_MAP50_L						0x0658
#define RXU_MAP50_H						0x065C
#define RXU_MAP50_QID					0x0660
#define RXU_MAP51_L						0x0664
#define RXU_MAP51_H						0x0668
#define RXU_MAP51_QID					0x066C
#define RXU_MAP52_L						0x0670
#define RXU_MAP52_H						0x0674
#define RXU_MAP52_QID					0x0678
#define RXU_MAP53_L						0x067C
#define RXU_MAP53_H						0x0680
#define RXU_MAP53_QID					0x0684
#define RXU_MAP54_L						0x0688
#define RXU_MAP54_H						0x068C
#define RXU_MAP54_QID					0x0690
#define RXU_MAP55_L						0x0694
#define RXU_MAP55_H						0x0698
#define RXU_MAP55_QID					0x069C
#define RXU_MAP56_L						0x06A0
#define RXU_MAP56_H						0x06A4
#define RXU_MAP56_QID					0x06A8
#define RXU_MAP57_L						0x06AC
#define RXU_MAP57_H						0x06B0
#define RXU_MAP57_QID					0x06B4
#define RXU_MAP58_L						0x06B8
#define RXU_MAP58_H						0x06BC
#define RXU_MAP58_QID					0x06C0
#define RXU_MAP59_L						0x06C4
#define RXU_MAP59_H						0x06C8
#define RXU_MAP59_QID					0x06CC
#define RXU_MAP60_L						0x06D0
#define RXU_MAP60_H						0x06D4
#define RXU_MAP60_QID					0x06D8
#define RXU_MAP61_L						0x06DC
#define RXU_MAP61_H						0x06E0
#define RXU_MAP61_QID					0x06E4
#define RXU_MAP62_L						0x06E8
#define RXU_MAP62_H						0x06EC
#define RXU_MAP62_QID					0x06F0
#define RXU_MAP63_L						0x06F4
#define RXU_MAP63_H						0x06F8
#define RXU_MAP63_QID					0x06FC

#define RXU_TYPE9_MAP00_0				0x0700
#define RXU_TYPE9_MAP00_1				0x0704
#define RXU_TYPE9_MAP00_2				0x0708
#define RXU_TYPE9_MAP01_0				0x070C
#define RXU_TYPE9_MAP01_1				0x0710
#define RXU_TYPE9_MAP01_2				0x0714
#define RXU_TYPE9_MAP02_0				0x0718
#define RXU_TYPE9_MAP02_1				0x071C
#define RXU_TYPE9_MAP02_2				0x0720
#define RXU_TYPE9_MAP03_0				0x0724
#define RXU_TYPE9_MAP03_1				0x0728
#define RXU_TYPE9_MAP03_2				0x072C
#define RXU_TYPE9_MAP04_0				0x0730
#define RXU_TYPE9_MAP04_1				0x0734
#define RXU_TYPE9_MAP04_2				0x0738
#define RXU_TYPE9_MAP05_0				0x073C
#define RXU_TYPE9_MAP05_1				0x0740
#define RXU_TYPE9_MAP05_2				0x0744
#define RXU_TYPE9_MAP06_0				0x0748
#define RXU_TYPE9_MAP06_1				0x074C
#define RXU_TYPE9_MAP06_2				0x0750
#define RXU_TYPE9_MAP07_0				0x0754
#define RXU_TYPE9_MAP07_1				0x0758
#define RXU_TYPE9_MAP07_2				0x075C
#define RXU_TYPE9_MAP08_0				0x0760
#define RXU_TYPE9_MAP08_1				0x0764
#define RXU_TYPE9_MAP08_2				0x0768
#define RXU_TYPE9_MAP09_0				0x076C
#define RXU_TYPE9_MAP09_1				0x0770
#define RXU_TYPE9_MAP09_2				0x0774
#define RXU_TYPE9_MAP10_0				0x0778
#define RXU_TYPE9_MAP10_1				0x077C
#define RXU_TYPE9_MAP10_2				0x0780
#define RXU_TYPE9_MAP11_0				0x0784
#define RXU_TYPE9_MAP11_1				0x0788
#define RXU_TYPE9_MAP11_2				0x078C
#define RXU_TYPE9_MAP12_0				0x0790
#define RXU_TYPE9_MAP12_1				0x0794
#define RXU_TYPE9_MAP12_2				0x0798
#define RXU_TYPE9_MAP13_0				0x079C
#define RXU_TYPE9_MAP13_1				0x07A0
#define RXU_TYPE9_MAP13_2				0x07A4
#define RXU_TYPE9_MAP14_0				0x07A8
#define RXU_TYPE9_MAP14_1				0x07AC
#define RXU_TYPE9_MAP14_2				0x07B0
#define RXU_TYPE9_MAP15_0				0x07B4
#define RXU_TYPE9_MAP15_1				0x07B8
#define RXU_TYPE9_MAP15_2				0x07BC
#define RXU_TYPE9_MAP16_0				0x07C0
#define RXU_TYPE9_MAP16_1				0x07C4
#define RXU_TYPE9_MAP16_2				0x07C8
#define RXU_TYPE9_MAP17_0				0x07CC
#define RXU_TYPE9_MAP17_1				0x07D0
#define RXU_TYPE9_MAP17_2				0x07D4
#define RXU_TYPE9_MAP18_0				0x07D8
#define RXU_TYPE9_MAP18_1				0x07DC
#define RXU_TYPE9_MAP18_2				0x07E0
#define RXU_TYPE9_MAP19_0				0x07E4
#define RXU_TYPE9_MAP19_1				0x07E8
#define RXU_TYPE9_MAP19_2				0x07EC
#define RXU_TYPE9_MAP20_0				0x07F0
#define RXU_TYPE9_MAP20_1				0x07F4
#define RXU_TYPE9_MAP20_2				0x07F8
#define RXU_TYPE9_MAP21_0				0x07FC
#define RXU_TYPE9_MAP21_1				0x0800
#define RXU_TYPE9_MAP21_2				0x0804
#define RXU_TYPE9_MAP22_0				0x0808
#define RXU_TYPE9_MAP22_1				0x080C
#define RXU_TYPE9_MAP22_2				0x0810
#define RXU_TYPE9_MAP23_0				0x0814
#define RXU_TYPE9_MAP23_1				0x0818
#define RXU_TYPE9_MAP23_2				0x081C
#define RXU_TYPE9_MAP24_0				0x0820
#define RXU_TYPE9_MAP24_1				0x0824
#define RXU_TYPE9_MAP24_2				0x0828
#define RXU_TYPE9_MAP25_0				0x082C
#define RXU_TYPE9_MAP25_1				0x0830
#define RXU_TYPE9_MAP25_2				0x0834
#define RXU_TYPE9_MAP26_0				0x0838
#define RXU_TYPE9_MAP26_1				0x083C
#define RXU_TYPE9_MAP26_2				0x0840
#define RXU_TYPE9_MAP27_0				0x0844
#define RXU_TYPE9_MAP27_1				0x0848
#define RXU_TYPE9_MAP27_2				0x084C
#define RXU_TYPE9_MAP28_0				0x0850
#define RXU_TYPE9_MAP28_1				0x0854
#define RXU_TYPE9_MAP28_2				0x0858
#define RXU_TYPE9_MAP29_0				0x085C
#define RXU_TYPE9_MAP29_1				0x0860
#define RXU_TYPE9_MAP29_2				0x0864
#define RXU_TYPE9_MAP30_0				0x0868
#define RXU_TYPE9_MAP30_1				0x086C
#define RXU_TYPE9_MAP30_2				0x0870
#define RXU_TYPE9_MAP31_0				0x0874
#define RXU_TYPE9_MAP31_1				0x0878
#define RXU_TYPE9_MAP31_2				0x087C
#define RXU_TYPE9_MAP32_0				0x0880
#define RXU_TYPE9_MAP32_1				0x0884
#define RXU_TYPE9_MAP32_2				0x0888
#define RXU_TYPE9_MAP33_0				0x088C
#define RXU_TYPE9_MAP33_1				0x0890
#define RXU_TYPE9_MAP33_2				0x0894
#define RXU_TYPE9_MAP34_0				0x0898
#define RXU_TYPE9_MAP34_1				0x089C
#define RXU_TYPE9_MAP34_2				0x08A0
#define RXU_TYPE9_MAP35_0				0x08A4
#define RXU_TYPE9_MAP35_1				0x08A8
#define RXU_TYPE9_MAP35_2				0x08AC
#define RXU_TYPE9_MAP36_0				0x08B0
#define RXU_TYPE9_MAP36_1				0x08B4
#define RXU_TYPE9_MAP36_2				0x08B8
#define RXU_TYPE9_MAP37_0				0x08BC
#define RXU_TYPE9_MAP37_1				0x08C0
#define RXU_TYPE9_MAP37_2				0x08C4
#define RXU_TYPE9_MAP38_0				0x08C8
#define RXU_TYPE9_MAP38_1				0x08CC
#define RXU_TYPE9_MAP38_2				0x08D0
#define RXU_TYPE9_MAP39_0				0x08D4
#define RXU_TYPE9_MAP39_1				0x08D8
#define RXU_TYPE9_MAP39_2				0x08DC
#define RXU_TYPE9_MAP40_0				0x08E0
#define RXU_TYPE9_MAP40_1				0x08E4
#define RXU_TYPE9_MAP40_2				0x08E8
#define RXU_TYPE9_MAP41_0				0x08EC
#define RXU_TYPE9_MAP41_1				0x08F0
#define RXU_TYPE9_MAP41_2				0x08F4
#define RXU_TYPE9_MAP42_0				0x08F8
#define RXU_TYPE9_MAP42_1				0x08FC
#define RXU_TYPE9_MAP42_2				0x0900
#define RXU_TYPE9_MAP43_0				0x0904
#define RXU_TYPE9_MAP43_1				0x0908
#define RXU_TYPE9_MAP43_2				0x090C
#define RXU_TYPE9_MAP44_0				0x0910
#define RXU_TYPE9_MAP44_1				0x0914
#define RXU_TYPE9_MAP44_2				0x0918
#define RXU_TYPE9_MAP45_0				0x091C
#define RXU_TYPE9_MAP45_1				0x0920
#define RXU_TYPE9_MAP45_2				0x0924
#define RXU_TYPE9_MAP46_0				0x0928
#define RXU_TYPE9_MAP46_1				0x092C
#define RXU_TYPE9_MAP46_2				0x0930
#define RXU_TYPE9_MAP47_0				0x0934
#define RXU_TYPE9_MAP47_1				0x0938
#define RXU_TYPE9_MAP47_2				0x093C
#define RXU_TYPE9_MAP48_0				0x0940
#define RXU_TYPE9_MAP48_1				0x0944
#define RXU_TYPE9_MAP48_2				0x0948
#define RXU_TYPE9_MAP49_0				0x094C
#define RXU_TYPE9_MAP49_1				0x0950
#define RXU_TYPE9_MAP49_2				0x0954
#define RXU_TYPE9_MAP50_0				0x0958
#define RXU_TYPE9_MAP50_1				0x095C
#define RXU_TYPE9_MAP50_2				0x0960
#define RXU_TYPE9_MAP51_0				0x0964
#define RXU_TYPE9_MAP51_1				0x0968
#define RXU_TYPE9_MAP51_2				0x096C
#define RXU_TYPE9_MAP52_0				0x0970
#define RXU_TYPE9_MAP52_1				0x0974
#define RXU_TYPE9_MAP52_2				0x0978
#define RXU_TYPE9_MAP53_0				0x097C
#define RXU_TYPE9_MAP53_1				0x0980
#define RXU_TYPE9_MAP53_2				0x0984
#define RXU_TYPE9_MAP54_0				0x0988
#define RXU_TYPE9_MAP54_1				0x098C
#define RXU_TYPE9_MAP54_2				0x0990
#define RXU_TYPE9_MAP55_0				0x0994
#define RXU_TYPE9_MAP55_1				0x0998
#define RXU_TYPE9_MAP55_2				0x099C
#define RXU_TYPE9_MAP56_0				0x09A0
#define RXU_TYPE9_MAP56_1				0x09A4
#define RXU_TYPE9_MAP56_2				0x09A8
#define RXU_TYPE9_MAP57_0				0x09AC
#define RXU_TYPE9_MAP57_1				0x09B0
#define RXU_TYPE9_MAP57_2				0x09B4
#define RXU_TYPE9_MAP58_0				0x09B8
#define RXU_TYPE9_MAP58_1				0x09BC
#define RXU_TYPE9_MAP58_2				0x09C0
#define RXU_TYPE9_MAP59_0				0x09C4
#define RXU_TYPE9_MAP59_1				0x09C8
#define RXU_TYPE9_MAP59_2				0x09CC
#define RXU_TYPE9_MAP60_0				0x09D0
#define RXU_TYPE9_MAP60_1				0x09D4
#define RXU_TYPE9_MAP60_2				0x09D8
#define RXU_TYPE9_MAP61_0				0x09DC
#define RXU_TYPE9_MAP61_1				0x09E0
#define RXU_TYPE9_MAP61_2				0x09E4
#define RXU_TYPE9_MAP62_0				0x09E8
#define RXU_TYPE9_MAP62_1				0x09EC
#define RXU_TYPE9_MAP62_2				0x09F0
#define RXU_TYPE9_MAP63_0				0x09F4
#define RXU_TYPE9_MAP63_1				0x09F8
#define RXU_TYPE9_MAP63_2				0x09FC

#define LSU1_REG0						0x0D00
#define LSU1_REG1						0x0D04
#define LSU1_REG2						0x0D08
#define LSU1_REG3						0x0D0C
#define LSU1_REG4						0x0D10
#define LSU1_REG5						0x0D14
#define LSU1_REG6						0x0D18
#define LSU2_REG0						0x0D1C
#define LSU2_REG1						0x0D20
#define LSU2_REG2						0x0D24
#define LSU2_REG3						0x0D28
#define LSU2_REG4						0x0D2C
#define LSU2_REG5						0x0D30
#define LSU2_REG6						0x0D34
#define LSU3_REG0						0x0D38
#define LSU3_REG1						0x0D3C
#define LSU3_REG2						0x0D40
#define LSU3_REG3						0x0D44
#define LSU3_REG4						0x0D48
#define LSU3_REG5						0x0D4C
#define LSU3_REG6						0x0D50
#define LSU4_REG0						0x0D54
#define LSU4_REG1						0x0D58
#define LSU4_REG2						0x0D5C
#define LSU4_REG3						0x0D60
#define LSU4_REG4						0x0D64
#define LSU4_REG5						0x0D68
#define LSU4_REG6						0x0D6C
#define LSU5_REG0						0x0D70
#define LSU5_REG1						0x0D74
#define LSU5_REG2						0x0D78
#define LSU5_REG3						0x0D7C
#define LSU5_REG4						0x0D80
#define LSU5_REG5						0x0D84
#define LSU5_REG6						0x0D88
#define LSU6_REG0						0x0D8C
#define LSU6_REG1						0x0D90
#define LSU6_REG2						0x0D94
#define LSU6_REG3						0x0D98
#define LSU6_REG4						0x0D9C
#define LSU6_REG5						0x0DA0
#define LSU6_REG6						0x0DA4
#define LSU7_REG0						0x0DA8
#define LSU7_REG1						0x0DAC
#define LSU7_REG2						0x0DB0
#define LSU7_REG3						0x0DB4
#define LSU7_REG4						0x0DB8
#define LSU7_REG5						0x0DBC
#define LSU7_REG6						0x0DC0
#define LSU8_REG0						0x0DC4
#define LSU8_REG1						0x0DC8
#define LSU8_REG2						0x0DCC
#define LSU8_REG3						0x0DD0
#define LSU8_REG4						0x0DD4
#define LSU8_REG5						0x0DD8
#define LSU8_REG6						0x0DDC

#define LSU_SETUP_REG0					0x0DE0
#define LSU_SETUP_REG1					0x0DE4

#define LSU_STAT_REG0					0x0DE8
#define LSU_STAT_REG1					0x0DEC
#define LSU_STAT_REG2					0x0DF0
#define LSU_STAT_REG3					0x0DF4
#define LSU_STAT_REG4					0x0DF8
#define LSU_STAT_REG5					0x0DFC

#define LSU_FLOW_MASKS0					0x0E00
#define LSU_FLOW_MASKS1					0x0E04
#define LSU_FLOW_MASKS2					0x0E08
#define LSU_FLOW_MASKS3					0x0E0C

#define SUPRVSR_ID						0x0E4C

#define FLOW_CNTL0						0x0E50
#define FLOW_CNTL1						0x0E54
#define FLOW_CNTL2						0x0E58
#define FLOW_CNTL3						0x0E5C
#define FLOW_CNTL4						0x0E60
#define FLOW_CNTL5						0x0E64
#define FLOW_CNTL6						0x0E68
#define FLOW_CNTL7						0x0E6C
#define FLOW_CNTL8						0x0E70
#define FLOW_CNTL9						0x0E74
#define FLOW_CNTL10						0x0E78
#define FLOW_CNTL11						0x0E7C
#define FLOW_CNTL12						0x0E80
#define FLOW_CNTL13						0x0E84
#define FLOW_CNTL14						0x0E88
#define FLOW_CNTL15						0x0E8C

#define TX_CPPI_FLOW_MASKS0				0x0EB0
#define TX_CPPI_FLOW_MASKS1				0x0EB4
#define TX_CPPI_FLOW_MASKS2				0x0EB8
#define TX_CPPI_FLOW_MASKS3				0x0EBC
#define TX_CPPI_FLOW_MASKS4				0x0EC0
#define TX_CPPI_FLOW_MASKS5				0x0EC4
#define TX_CPPI_FLOW_MASKS6				0x0EC8
#define TX_CPPI_FLOW_MASKS7				0x0ECC

#define TX_QUEUE_SCH_INFO1				0x0ED0
#define TX_QUEUE_SCH_INFO2				0x0ED4
#define TX_QUEUE_SCH_INFO3				0x0ED8
#define TX_QUEUE_SCH_INFO4				0x0EDC

#define GARBAGE_COLL_QID0				0x0EE0
#define GARBAGE_COLL_QID1				0x0EE4
#define GARBAGE_COLL_QID2				0x0EE8

#define TCHAN_GCFG_REG_A0				0x1400
#define TCHAN_GCFG_REG_B0				0x1404
#define TCHAN_GCFG_REG_A1				0x1420
#define TCHAN_GCFG_REG_B1				0x1424
#define TCHAN_GCFG_REG_A2				0x1440
#define TCHAN_GCFG_REG_B2				0x1444
#define TCHAN_GCFG_REG_A3				0x1460
#define TCHAN_GCFG_REG_B3				0x1464
#define TCHAN_GCFG_REG_A4				0x1480
#define TCHAN_GCFG_REG_B4				0x1484
#define TCHAN_GCFG_REG_A5				0x14A0
#define TCHAN_GCFG_REG_B5				0x14A4
#define TCHAN_GCFG_REG_A6				0x14C0
#define TCHAN_GCFG_REG_B6				0x14C4
#define TCHAN_GCFG_REG_A7				0x14E0
#define TCHAN_GCFG_REG_B7				0x14E4
#define TCHAN_GCFG_REG_A8				0x1500
#define TCHAN_GCFG_REG_B8				0x1504
#define TCHAN_GCFG_REG_A9				0x1520
#define TCHAN_GCFG_REG_B9				0x1524
#define TCHAN_GCFG_REG_A10				0x1540
#define TCHAN_GCFG_REG_B10				0x1544
#define TCHAN_GCFG_REG_A11				0x1560
#define TCHAN_GCFG_REG_B11				0x1564
#define TCHAN_GCFG_REG_A12				0x1580
#define TCHAN_GCFG_REG_B12				0x1584
#define TCHAN_GCFG_REG_A13				0x15A0
#define TCHAN_GCFG_REG_B13				0x15A4
#define TCHAN_GCFG_REG_A14				0x15C0
#define TCHAN_GCFG_REG_B14				0x15C4
#define TCHAN_GCFG_REG_A15				0x15E0
#define TCHAN_GCFG_REG_B15				0x15E4

#define RCHAN_GCFG_REG_A0				0x1800
#define RCHAN_GCFG_REG_A1				0x1820
#define RCHAN_GCFG_REG_A2				0x1840
#define RCHAN_GCFG_REG_A3				0x1860
#define RCHAN_GCFG_REG_A4				0x1880
#define RCHAN_GCFG_REG_A5				0x18A0
#define RCHAN_GCFG_REG_A6				0x18C0
#define RCHAN_GCFG_REG_A7				0x18E0
#define RCHAN_GCFG_REG_A8				0x1900
#define RCHAN_GCFG_REG_A9				0x1920
#define RCHAN_GCFG_REG_A10				0x1940
#define RCHAN_GCFG_REG_A11				0x1960
#define RCHAN_GCFG_REG_A12				0x1980
#define RCHAN_GCFG_REG_A13				0x19A0
#define RCHAN_GCFG_REG_A14				0x19C0
#define RCHAN_GCFG_REG_A15				0x19E0

#define TCHAN_SCHED_CFG_REG0			0x1C00
#define TCHAN_SCHED_CFG_REG1			0x1C04
#define TCHAN_SCHED_CFG_REG2			0x1C08
#define TCHAN_SCHED_CFG_REG3			0x1C0C
#define TCHAN_SCHED_CFG_REG4			0x1C10
#define TCHAN_SCHED_CFG_REG5			0x1C14
#define TCHAN_SCHED_CFG_REG6			0x1C18
#define TCHAN_SCHED_CFG_REG7			0x1C1C
#define TCHAN_SCHED_CFG_REG8			0x1C20
#define TCHAN_SCHED_CFG_REG9			0x1C24
#define TCHAN_SCHED_CFG_REG10			0x1C28
#define TCHAN_SCHED_CFG_REG11			0x1C2C
#define TCHAN_SCHED_CFG_REG12			0x1C30
#define TCHAN_SCHED_CFG_REG13			0x1C34
#define TCHAN_SCHED_CFG_REG14			0x1C38
#define TCHAN_SCHED_CFG_REG15			0x1C3C

#define RFLOW_CFG_REG_A0				0x2000
#define RFLOW_CFG_REG_B0				0x2004
#define RFLOW_CFG_REG_C0				0x2008
#define RFLOW_CFG_REG_D0				0x200C
#define RFLOW_CFG_REG_E0				0x2010
#define RFLOW_CFG_REG_F0				0x2014
#define RFLOW_CFG_REG_G0				0x2018
#define RFLOW_CFG_REG_H0				0x201C
#define RFLOW_CFG_REG_A1				0x2020
#define RFLOW_CFG_REG_B1				0x2024
#define RFLOW_CFG_REG_C1				0x2028
#define RFLOW_CFG_REG_D1				0x202C
#define RFLOW_CFG_REG_E1				0x2030
#define RFLOW_CFG_REG_F1				0x2034
#define RFLOW_CFG_REG_G1				0x2038
#define RFLOW_CFG_REG_H1				0x203C
#define RFLOW_CFG_REG_A2				0x2040
#define RFLOW_CFG_REG_B2				0x2044
#define RFLOW_CFG_REG_C2				0x2048
#define RFLOW_CFG_REG_D2				0x204C
#define RFLOW_CFG_REG_E2				0x2050
#define RFLOW_CFG_REG_F2				0x2054
#define RFLOW_CFG_REG_G2				0x2058
#define RFLOW_CFG_REG_H2				0x205C
#define RFLOW_CFG_REG_A3				0x2060
#define RFLOW_CFG_REG_B3				0x2064
#define RFLOW_CFG_REG_C3				0x2068
#define RFLOW_CFG_REG_D3				0x206C
#define RFLOW_CFG_REG_E3				0x2070
#define RFLOW_CFG_REG_F3				0x2074
#define RFLOW_CFG_REG_G3				0x2078
#define RFLOW_CFG_REG_H3				0x207C
#define RFLOW_CFG_REG_A4				0x2080
#define RFLOW_CFG_REG_B4				0x2084
#define RFLOW_CFG_REG_C4				0x2088
#define RFLOW_CFG_REG_D4				0x208C
#define RFLOW_CFG_REG_E4				0x2090
#define RFLOW_CFG_REG_F4				0x2094
#define RFLOW_CFG_REG_G4				0x2098
#define RFLOW_CFG_REG_H4				0x209C
#define RFLOW_CFG_REG_A5				0x20A0
#define RFLOW_CFG_REG_B5				0x20A4
#define RFLOW_CFG_REG_C5				0x20A8
#define RFLOW_CFG_REG_D5				0x20Ac
#define RFLOW_CFG_REG_E5				0x20B0
#define RFLOW_CFG_REG_F5				0x20B4
#define RFLOW_CFG_REG_G5				0x20B8
#define RFLOW_CFG_REG_H5				0x20BC
#define RFLOW_CFG_REG_A6				0x20C0
#define RFLOW_CFG_REG_B6				0x20C4
#define RFLOW_CFG_REG_C6				0x20C8
#define RFLOW_CFG_REG_D6				0x20CC
#define RFLOW_CFG_REG_E6				0x20D0
#define RFLOW_CFG_REG_F6				0x20D4
#define RFLOW_CFG_REG_G6				0x20D8
#define RFLOW_CFG_REG_H6				0x20DC
#define RFLOW_CFG_REG_A7				0x20E0
#define RFLOW_CFG_REG_B7				0x20E4
#define RFLOW_CFG_REG_C7				0x20E8
#define RFLOW_CFG_REG_D7				0x20EC
#define RFLOW_CFG_REG_E7				0x20F0
#define RFLOW_CFG_REG_F7				0x20F4
#define RFLOW_CFG_REG_G7				0x20F8
#define RFLOW_CFG_REG_H7				0x20FC
#define RFLOW_CFG_REG_A8				0x2100
#define RFLOW_CFG_REG_B8				0x2104
#define RFLOW_CFG_REG_C8				0x2108
#define RFLOW_CFG_REG_D8				0x210C
#define RFLOW_CFG_REG_E8				0x2110
#define RFLOW_CFG_REG_F8				0x2114
#define RFLOW_CFG_REG_G8				0x2118
#define RFLOW_CFG_REG_H8				0x211C
#define RFLOW_CFG_REG_A9				0x2120
#define RFLOW_CFG_REG_B9				0x2124
#define RFLOW_CFG_REG_C9				0x2128
#define RFLOW_CFG_REG_D9				0x212C
#define RFLOW_CFG_REG_E9				0x2130
#define RFLOW_CFG_REG_F9				0x2134
#define RFLOW_CFG_REG_G9				0x2138
#define RFLOW_CFG_REG_H9				0x213C
#define RFLOW_CFG_REG_A10				0x2140
#define RFLOW_CFG_REG_B10				0x2144
#define RFLOW_CFG_REG_C10				0x2148
#define RFLOW_CFG_REG_D10				0x214C
#define RFLOW_CFG_REG_E10				0x2150
#define RFLOW_CFG_REG_F10				0x2154
#define RFLOW_CFG_REG_G10				0x2158
#define RFLOW_CFG_REG_H10				0x215C
#define RFLOW_CFG_REG_A11				0x2160
#define RFLOW_CFG_REG_B11				0x2164
#define RFLOW_CFG_REG_C11				0x2168
#define RFLOW_CFG_REG_D11				0x216C
#define RFLOW_CFG_REG_E11				0x2170
#define RFLOW_CFG_REG_F11				0x2174
#define RFLOW_CFG_REG_G11				0x2178
#define RFLOW_CFG_REG_H11				0x217C
#define RFLOW_CFG_REG_A12				0x2180
#define RFLOW_CFG_REG_B12				0x2184
#define RFLOW_CFG_REG_C12				0x2188
#define RFLOW_CFG_REG_D12				0x218C
#define RFLOW_CFG_REG_E12				0x2190
#define RFLOW_CFG_REG_F12				0x2194
#define RFLOW_CFG_REG_G12				0x2198
#define RFLOW_CFG_REG_H12				0x219C
#define RFLOW_CFG_REG_A13				0x21A0
#define RFLOW_CFG_REG_B13				0x21A4
#define RFLOW_CFG_REG_C13				0x21A8
#define RFLOW_CFG_REG_D13				0x21Ac
#define RFLOW_CFG_REG_E13				0x21B0
#define RFLOW_CFG_REG_F13				0x21B4
#define RFLOW_CFG_REG_G13				0x21B8
#define RFLOW_CFG_REG_H13				0x21BC
#define RFLOW_CFG_REG_A14				0x21C0
#define RFLOW_CFG_REG_B14				0x21C4
#define RFLOW_CFG_REG_C14				0x21C8
#define RFLOW_CFG_REG_D14				0x21CC
#define RFLOW_CFG_REG_E14				0x21D0
#define RFLOW_CFG_REG_F14				0x21D4
#define RFLOW_CFG_REG_G14				0x21D8
#define RFLOW_CFG_REG_H14				0x21DC
#define RFLOW_CFG_REG_A15				0x21E0
#define RFLOW_CFG_REG_B15				0x21E4
#define RFLOW_CFG_REG_C15				0x21E8
#define RFLOW_CFG_REG_D15				0x21EC
#define RFLOW_CFG_REG_E15				0x21F0
#define RFLOW_CFG_REG_F15				0x21F4
#define RFLOW_CFG_REG_G15				0x21F8
#define RFLOW_CFG_REG_H15				0x21FC
#define RFLOW_CFG_REG_A16				0x2200
#define RFLOW_CFG_REG_B16				0x2204
#define RFLOW_CFG_REG_C16				0x2208
#define RFLOW_CFG_REG_D16				0x220C
#define RFLOW_CFG_REG_E16				0x2210
#define RFLOW_CFG_REG_F16				0x2214
#define RFLOW_CFG_REG_G16				0x2218
#define RFLOW_CFG_REG_H16				0x221C
#define RFLOW_CFG_REG_A17				0x2220
#define RFLOW_CFG_REG_B17				0x2224
#define RFLOW_CFG_REG_C17				0x2228
#define RFLOW_CFG_REG_D17				0x222C
#define RFLOW_CFG_REG_E17				0x2230
#define RFLOW_CFG_REG_F17				0x2234
#define RFLOW_CFG_REG_G17				0x2238
#define RFLOW_CFG_REG_H17				0x223C
#define RFLOW_CFG_REG_A18				0x2240
#define RFLOW_CFG_REG_B18				0x2244
#define RFLOW_CFG_REG_C18				0x2248
#define RFLOW_CFG_REG_D18				0x224C
#define RFLOW_CFG_REG_E18				0x2250
#define RFLOW_CFG_REG_F18				0x2254
#define RFLOW_CFG_REG_G18				0x2258
#define RFLOW_CFG_REG_H18				0x225C
#define RFLOW_CFG_REG_A19				0x2260
#define RFLOW_CFG_REG_B19				0x2264
#define RFLOW_CFG_REG_C19				0x2268
#define RFLOW_CFG_REG_D19				0x226C
#define RFLOW_CFG_REG_E19				0x2270
#define RFLOW_CFG_REG_F19				0x2274
#define RFLOW_CFG_REG_G19				0x2278
#define RFLOW_CFG_REG_H19				0x227C

#define DEV_ID							0xB000
#define DEV_INFO						0xB004
#define ASBLY_ID						0xB008
#define ASBLY_INFO						0xB00C
#define PE_FEAT							0xB010
#define SW_PORT							0xB014
#define SRC_OP							0xB018
#define DEST_OP							0xB01C
#define DS_INFO							0xB03C
#define DS_LL_CTL						0xB048
#define PE_LL_CTL						0xB04C
#define LCL_CFG_HBAR					0xB058
#define LCL_CFG_BAR						0xB05C
#define BASE_ID							0xB060
#define HOST_BASE_ID_LOCK				0xB068
#define COMP_TAG						0xB06C
#define SP_MB_HEAD						0xB100
#define SP_LT_CTL						0xB120
#define SP_RT_CTL						0xB124
#define SP_GEN_CTL						0xB13C
#define SP0_LM_REQ						0xB140
#define SP0_LM_RESP						0xB144
#define SP0_ACKID_STAT					0xB148
#define SP0_CTL2						0xB154
#define SP0_ERR_STAT					0xB158
#define SP0_CTL							0xB15C
#define SP1_LM_REQ						0xB160
#define SP1_LM_RESP						0xB164
#define SP1_ACKID_STAT					0xB168
#define SP1_CTL2						0xB174
#define SP1_ERR_STAT					0xB178
#define SP1_CTL							0xB17C
#define SP2_LM_REQ						0xB180
#define SP2_LM_RESP						0xB184
#define SP2_ACKID_STAT					0xB188
#define SP2_CTL2						0xB194
#define SP2_ERR_STAT					0xB198
#define SP2_CTL							0xB19C
#define SP3_LM_REQ						0xB1A0
#define SP3_LM_RESP						0xB1A4
#define SP3_ACKID_STAT					0xB1A8
#define SP3_CTL2						0xB1B4
#define SP3_ERR_STAT					0xB1B8
#define SP3_CTL							0xB1BC
#define ERR_RPT_BH						0xC000
#define ERR_DET							0xC008
#define ERR_EN							0xC00C
#define H_ADDR_CAPT						0xC010
#define ADDR_CAPT						0xC014
#define ID_CAPT							0xC018
#define CTRL_CAPT						0xC01C
#define PW_TGT_ID						0xC028
#define SP0_ERR_DET						0xC040
#define SP0_RATE_EN						0xC044
#define SP0_ERR_ATTR_CAPT_DBG0			0xC048
#define SP0_ERR_CAPT_0_DBG1				0xC04C
#define SP0_ERR_CAPT_1_DBG2				0xC050
#define SP0_ERR_CAPT_2_DBG3				0xC054
#define SP0_ERR_CAPT_3_DBG4				0xC058
#define SP0_ERR_RATE					0xC068
#define SP0_ERR_THRESH					0xC06C
#define SP1_ERR_DET						0xC080
#define SP1_RATE_EN						0xC084
#define SP1_ERR_ATTR_CAPT				0xC088
#define SP1_ERR_CAPT_0					0xC08C
#define SP1_ERR_CAPT_1					0xC090
#define SP1_ERR_CAPT_2					0xC094
#define SP1_ERR_CAPT_3					0xC098
#define SP1_ERR_RATE					0xC0A8
#define SP1_ERR_THRESH					0xC0AC
#define SP2_ERR_DET						0xC0C0
#define SP2_RATE_EN						0xC0C4
#define SP2_ERR_ATTR_CAPT				0xC0C8
#define SP2_ERR_CAPT_0					0xC0CC
#define SP2_ERR_CAPT_1					0xC0D0
#define SP2_ERR_CAPT_2					0xC0D4
#define SP2_ERR_CAPT_3					0xC0D8
#define SP2_ERR_RATE					0xC0E8
#define SP2_ERR_THRESH					0xC0EC
#define SP3_ERR_DET						0xC100
#define SP3_RATE_EN						0xC104
#define SP3_ERR_ATTR_CAPT				0xC108
#define SP3_ERR_CAPT_0					0xC10C
#define SP3_ERR_CAPT_1					0xC110
#define SP3_ERR_CAPT_2					0xC114
#define SP3_ERR_CAPT_3					0xC118
#define SP3_ERR_RATE					0xC128
#define SP3_ERR_THRESH					0xC12C
#define LANE0_STAT0						0xE010
#define LANE0_STAT1						0xE014
#define LANE1_STAT0						0xE030
#define LANE1_STAT1						0xE034
#define LANE2_STAT0						0xE050
#define LANE2_STAT1						0xE054
#define LANE3_STAT0						0xE070
#define LANE3_STAT1						0xE074
#define PLM_BH							0x1B000
#define PLM_SP0_IMP_SPEC_CTL			0x1B080
#define RIO_PLM_SP0_PWDN_CTL			0x1B084
#define PLM_SP0_STATUS					0x1B090
#define PLM_SP0_INT_ENABLE				0x1B094
#define PLM_SP0_PW_ENABLE				0x1B098
#define PLM_SP0_EVENT_GEN				0x1B09C
#define PLM_SP0_ALL_INT_EN				0x1B0A0
#define PLM_SP0_ALL_PW_EN				0x1B0A4
#define PLM_SP0_PATH_CTL				0x1B0B0
#define PLM_SP0_DISCOVERY_TIMER			0x1B0B4
#define PLM_SP0_SILENCE_TIMER			0x1B0B8
#define PLM_SP0_VMIN_EXP				0x1B0BC
#define PLM_SP0_POL_CTL					0x1B0C0
#define PLM_SP0_DENIAL_CTL				0x1B0C8
#define PLM_SP0_RCVD_MECS				0x1B0D0
#define PLM_SP0_MECS_FWD				0x1B0D8
#define PLM_SP0_LONG_CS_TX1				0x1B0E0
#define PLM_SP0_LONG_CS_TX2				0x1B0E4
#define PLM_SP1_IMP_SPEC_CTL			0x1B100
#define RIO_PLM_SP1_PWDN_CTL			0x1B104
#define PLM_SP1_STATUS					0x1B110
#define PLM_SP1_INT_ENABLE				0x1B114
#define PLM_SP1_PW_ENABLE				0x1B118
#define PLM_SP1_EVENT_GEN				0x1B11C
#define PLM_SP1_ALL_INT_EN				0x1B120
#define PLM_SP1_ALL_PW_EN				0x1B124
#define PLM_SP1_PATH_CTL				0x1B130
#define PLM_SP1_DISCOVERY_TIMER			0x1B134
#define PLM_SP1_SILENCE_TIMER			0x1B138
#define PLM_SP1_VMIN_EXP				0x1B13C
#define PLM_SP1_POL_CTL					0x1B140
#define PLM_SP1_DENIAL_CTL				0x1B148
#define PLM_SP1_RCVD_MECS				0x1B150
#define PLM_SP1_MECS_FWD				0x1B158
#define PLM_SP1_LONG_CS_TX1				0x1B160
#define PLM_SP1_LONG_CS_TX2				0x1B164
#define PLM_SP2_IMP_SPEC_CTL			0x1B180
#define PLM_SP2_PWDN_CTL				0x1B184
#define PLM_SP2_STATUS					0x1B190
#define PLM_SP2_INT_ENABLE				0x1B194
#define PLM_SP2_PW_ENABLE				0x1B198
#define PLM_SP2_EVENT_GEN				0x1B19C
#define PLM_SP2_ALL_INT_EN				0x1B1A0
#define PLM_SP2_ALL_PW_EN				0x1B1A4
#define PLM_SP2_PATH_CTL				0x1B1B0
#define PLM_SP2_DISCOVERY_TIMER			0x1B1B4
#define PLM_SP2_SILENCE_TIMER			0x1B1B8
#define PLM_SP2_VMIN_EXP				0x1B1BC
#define PLM_SP2_POL_CTL					0x1B1C0
#define PLM_SP2_DENIAL_CTL				0x1B1C8
#define PLM_SP2_RCVD_MECS				0x1B1D0
#define PLM_SP2_MECS_FWD				0x1B1D8
#define PLM_SP2_LONG_CS_TX1				0x1B1E0
#define PLM_SP2_LONG_CS_TX2				0x1B1E4
#define PLM_SP3_IMP_SPEC_CTL			0x1B200
#define PLM_SP3_PWDN_CTL				0x1B204
#define PLM_SP3_STATUS					0x1B210
#define PLM_SP3_INT_ENABLE				0x1B214
#define PLM_SP3_PW_ENABLE				0x1B218
#define PLM_SP3_EVENT_GEN				0x1B21C
#define PLM_SP3_ALL_INT_EN				0x1B220
#define PLM_SP3_ALL_PW_EN				0x1B224
#define PLM_SP3_PATH_CTL				0x1B230
#define PLM_SP3_DISCOVERY_TIMER			0x1B234
#define PLM_SP3_SILENCE_TIMER			0x1B238
#define PLM_SP3_VMIN_EXP				0x1B23C
#define PLM_SP3_POL_CTL					0x1B240
#define PLM_SP3_DENIAL_CTL				0x1B248
#define PLM_SP3_RCVD_MECS				0x1B250
#define PLM_SP3_MECS_FWD				0x1B258
#define PLM_SP3_LONG_CS_TX1				0x1B260
#define PLM_SP3_LONG_CS_TX2				0x1B264
#define TLM_BH							0x1B300
#define TLM_SP0_CONTROL					0x1B380
#define TLM_SP0_STATUS					0x1B390
#define TLM_SP0_INT_ENABLE				0x1B394
#define TLM_SP0_PW_ENABLE				0x1B398
#define TLM_SP0_EVENT_GEN				0x1B39C
#define TLM_SP0_BRR_0_CTL				0x1B3A0
#define TLM_SP0_BRR_0_PATTERN_MATCH		0x1B3A4
#define TLM_SP0_BRR_1_CTL				0x1B3B0
#define TLM_SP0_BRR_1_PATTERN_MATCH		0x1B3B4
#define TLM_SP0_BRR_2_CTL				0x1B3C0
#define TLM_SP0_BRR_2_PATTERN_MATCH		0x1B3C4
#define TLM_SP0_BRR_3_CTL				0x1B3D0
#define TLM_SP0_BRR_3_PATTERN_MATCH		0x1B3D4
#define TLM_SP1_CONTROL					0x1B400
#define TLM_SP1_STATUS					0x1B410
#define TLM_SP1_INT_ENABLE				0x1B414
#define TLM_SP1_PW_ENABLE				0x1B418
#define TLM_SP1_EVENT_GEN				0x1B41C
#define TLM_SP1_BRR_0_CTL				0x1B420
#define TLM_SP1_BRR_0_PATTERN_MATCH		0x1B424
#define TLM_SP1_BRR_1_CTL				0x1B430
#define TLM_SP1_BRR_1_PATTERN_MATCH		0x1B434
#define TLM_SP1_BRR_2_CTL				0x1B440
#define TLM_SP1_BRR_2_PATTERN_MATCH		0x1B444
#define TLM_SP1_BRR_3_CTL				0x1B450
#define TLM_SP1_BRR_3_PATTERN_MATCH		0x1B454
#define TLM_SP2_CONTROL					0x1B480
#define TLM_SP2_STATUS					0x1B490
#define TLM_SP2_INT_ENABLE				0x1B494
#define TLM_SP2_PW_ENABLE				0x1B498
#define TLM_SP2_EVENT_GEN				0x1B49C
#define TLM_SP2_BRR_0_CTL				0x1B4A0
#define TLM_SP2_BRR_0_PATTERN_MATCH		0x1B4A4
#define TLM_SP2_BRR_1_CTL				0x1B4B0
#define TLM_SP2_BRR_1_PATTERN_MATCH		0x1B4B4
#define TLM_SP2_BRR_2_CTL				0x1B4C0
#define TLM_SP2_BRR_2_PATTERN_MATCH		0x1B4C4
#define TLM_SP2_BRR_3_CTL				0x1B4D0
#define TLM_SP2_BRR_3_PATTERN_MATCH		0x1B4D4
#define TLM_SP3_CONTROL					0x1B500
#define TLM_SP3_STATUS					0x1B510
#define TLM_SP3_INT_ENABLE				0x1B514
#define TLM_SP3_PW_ENABLE				0x1B518
#define TLM_SP3_EVENT_GEN				0x1B51C
#define TLM_SP3_BRR_0_CTL				0x1B520
#define TLM_SP3_BRR_0_PATTERN_MATCH		0x1B524
#define TLM_SP3_BRR_1_CTL				0x1B530
#define TLM_SP3_BRR_1_PATTERN_MATCH		0x1B534
#define TLM_SP3_BRR_2_CTL				0x1B540
#define TLM_SP3_BRR_2_PATTERN_MATCH		0x1B544
#define TLM_SP3_BRR_3_CTL				0x1B550
#define TLM_SP3_BRR_3_PATTERN_MATCH		0x1B554
#define PBM_BH							0x1B600
#define PBM_SP0_CONTROL					0x1B680
#define PBM_SP0_STATUS					0x1B690
#define PBM_SP0_INT_ENABLE				0x1B694
#define PBM_SP0_PW_ENABLE				0x1B698
#define PBM_SP0_EVENT_GEN				0x1B69C
#define PBM_SP0_IG_RESOURCES			0x1B6A0
#define PBM_SP0_EG_RESOURCES			0x1B6A4
#define PBM_SP0_IG_WATERMARK0			0x1B6B0
#define PBM_SP0_IG_WATERMARK1			0x1B6B4
#define PBM_SP0_IG_WATERMARK2			0x1B6B8
#define PBM_SP0_IG_WATERMARK3			0x1B6BC
#define PBM_SP1_CONTROL					0x1B700
#define PBM_SP1_STATUS					0x1B710
#define PBM_SP1_INT_ENABLE				0x1B714
#define PBM_SP1_PW_ENABLE				0x1B718
#define PBM_SP1_EVENT_GEN				0x1B71C
#define PBM_SP1_IG_RESOURCES			0x1B720
#define PBM_SP1_EG_RESOURCES			0x1B724
#define PBM_SP1_IG_WATERMARK0			0x1B730
#define PBM_SP1_IG_WATERMARK1			0x1B734
#define PBM_SP1_IG_WATERMARK2			0x1B738
#define PBM_SP1_IG_WATERMARK3			0x1B73C
#define PBM_SP2_CONTROL					0x1B780
#define PBM_SP2_STATUS					0x1B790
#define PBM_SP2_INT_ENABLE				0x1B794
#define PBM_SP2_PW_ENABLE				0x1B798
#define PBM_SP2_EVENT_GEN				0x1B79C
#define PBM_SP2_IG_RESOURCES			0x1B7A0
#define PBM_SP2_EG_RESOURCES			0x1B7A4
#define PBM_SP2_IG_WATERMARK0			0x1B7B0
#define PBM_SP2_IG_WATERMARK1			0x1B7B4
#define PBM_SP2_IG_WATERMARK2			0x1B7B8
#define PBM_SP2_IG_WATERMARK3			0x1B7BC
#define PBM_SP3_CONTROL					0x1B800
#define PBM_SP3_STATUS					0x1B810
#define PBM_SP3_INT_ENABLE				0x1B814
#define PBM_SP3_PW_ENABLE				0x1B818
#define PBM_SP3_EVENT_GEN				0x1B81C
#define PBM_SP3_IG_RESOURCES			0x1B820
#define PBM_SP3_EG_RESOURCES			0x1B824
#define PBM_SP3_IG_WATERMARK0			0x1B830
#define PBM_SP3_IG_WATERMARK1			0x1B834
#define PBM_SP3_IG_WATERMARK2			0x1B838
#define PBM_SP3_IG_WATERMARK3			0x1B83C
#define EM_BH							0x1B900
#define EM_INT_STAT						0x1B910
#define EM_INT_ENABLE					0x1B914
#define EM_INT_PORT_STAT				0x1B918
#define EM_PW_STAT						0x1B920
#define EM_PW_EN						0x1B924
#define EM_PW_PORT_STAT					0x1B928
#define EM_DEV_INT_EN					0x1B930
#define EM_DEV_PW_EN					0x1B934
#define EM_MECS_STAT					0x1B93C
#define EM_MECS_INT_EN					0x1B940
#define EM_MECS_CAP_EN					0x1B944
#define EM_MECS_TRIG_EN					0x1B948
#define EM_MECS_REQ						0x1B94C
#define EM_MECS_PORT_STAT				0x1B950
#define EM_RST_PORT_STAT				0x1B960
#define EM_RST_INT_EN					0x1B968
#define EM_RST_PW_EN					0x1B970
#define PW_BH							0x1BA00
#define PW_CTL							0x1BA04
#define PW_ROUTE						0x1BA08
#define PW_RX_STAT						0x1BA10
#define RIO_PW_RX_EVENT_GEN				0x1BA14
#define PW_RX_CAPT0						0x1BA20
#define PW_RX_CAPT1						0x1BA24
#define PW_RX_CAPT2						0x1BA28
#define PW_RX_CAPT3						0x1BA2C
#define LLM_BH							0x1BD00
#define WHITEBOARD						0x1BD24
#define PORT_NUMBER						0x1BD28
#define PRESCALAR_SRV_CLK				0x1BD30
#define REG_RST_CTL						0x1BD34
#define LOCAL_ERR_DET					0x1BD48
#define LOCAL_ERR_EN					0x1BD4C
#define LOCAL_H_ADDR_CAPT				0x1BD50
#define LOCAL_ADDR_CAPT					0x1BD54
#define LOCAL_ID_CAPT					0x1BD58
#define LOCAL_CTRL_CAPT					0x1BD5C
#define FABRIC_HDR						0x1BE00
#define FABRIC_CSR						0x1BE10
#define SP0_FABRIC_STATUS				0x1BE40
#define SP1_FABRIC_STATUS				0x1BE44
#define SP2_FABRIC_STATUS				0x1BE48
#define SP3_FABRIC_STATUS				0x1BE4C

#endif
