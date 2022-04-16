#ifndef __CCM_IMX6Q_H__ 
#define __CCM_IMX6Q_H__ 
struct registers 
{
	
	uint32_t ccm_ccr;
	
	uint32_t ccm_ccdr;
	
	uint32_t ccm_csr;
	
	uint32_t ccm_ccsr;
	
	uint32_t ccm_cacrr;
	
	uint32_t ccm_cbcdr;
	
	uint32_t ccm_cbcmr;
	
	uint32_t ccm_cscmr1;
	
	uint32_t ccm_cscmr2;
	
	uint32_t ccm_cscdr1;
	
	uint32_t ccm_cs1cdr;
	
	uint32_t ccm_cs2cdr;
	
	uint32_t ccm_cdcdr;
	
	uint32_t ccm_chsccdr;
	
	uint32_t ccm_cscdr2;
	
	uint32_t ccm_cscdr3;
	
	uint32_t ccm_cwdr;
	
	uint32_t ccm_cdhipr;
	
	uint32_t ccm_clpcr;
	
	uint32_t ccm_cisr;
	
	uint32_t ccm_cimr;
	
	uint32_t ccm_ccosr;
	
	uint32_t ccm_cgpr;
	
	uint32_t ccm_ccgr0;
	
	uint32_t ccm_ccgr1;
	
	uint32_t ccm_ccgr2;
	
	uint32_t ccm_ccgr3;
	
	uint32_t ccm_ccgr4;
	
	uint32_t ccm_ccgr5;
	
	uint32_t ccm_ccgr6;
	
	uint32_t ccm_cmeor;
	
};


struct imx6q_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct imx6q_device ccm_imx6q_device;
static char* regs_name[] = 
{
	"CCM_CCR",
	"CCM_CCDR",
	"CCM_CSR",
	"CCM_CCSR",
	"CCM_CACRR",
	"CCM_CBCDR",
	"CCM_CBCMR",
	"CCM_CSCMR1",
	"CCM_CSCMR2",
	"CCM_CSCDR1",
	"CCM_CS1CDR",
	"CCM_CS2CDR",
	"CCM_CDCDR",
	"CCM_CHSCCDR",
	"CCM_CSCDR2",
	"CCM_CSCDR3",
	"CCM_CWDR",
	"CCM_CDHIPR",
	"CCM_CLPCR",
	"CCM_CISR",
	"CCM_CIMR",
	"CCM_CCOSR",
	"CCM_CGPR",
	"CCM_CCGR0",
	"CCM_CCGR1",
	"CCM_CCGR2",
	"CCM_CCGR3",
	"CCM_CCGR4",
	"CCM_CCGR5",
	"CCM_CCGR6",
	"CCM_CMEOR",
	NULL
};
#define CCM_CCR 0x00
#define CCM_CCDR 0x04
#define CCM_CSR 0x08
#define CCM_CCSR 0x0C
#define CCM_CACRR 0x10
#define CCM_CBCDR 0x14
#define CCM_CBCMR 0x18
#define CCM_CSCMR1 0x1C
#define CCM_CSCMR2 0x20
#define CCM_CSCDR1 0x24
#define CCM_CS1CDR 0x28
#define CCM_CS2CDR 0x2C
#define CCM_CDCDR 0x30
#define CCM_CHSCCDR 0x34
#define CCM_CSCDR2 0x38
#define CCM_CSCDR3 0x3C
#define CCM_CWDR 0x44
#define CCM_CDHIPR 0x48
#define CCM_CLPCR 0x54
#define CCM_CISR 0x58
#define CCM_CIMR 0x5C
#define CCM_CCOSR 0x60
#define CCM_CGPR 0x64
#define CCM_CCGR0 0x68
#define CCM_CCGR1 0x6C
#define CCM_CCGR2 0x70
#define CCM_CCGR3 0x74
#define CCM_CCGR4 0x78
#define CCM_CCGR5 0x7C
#define CCM_CCGR6 0x80
#define CCM_CMEOR 0x88
#endif  
