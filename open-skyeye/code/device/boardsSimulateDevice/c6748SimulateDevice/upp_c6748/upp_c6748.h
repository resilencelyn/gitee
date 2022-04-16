#ifndef __UPP_C6748_H__ 
#define __UPP_C6748_H__ 
#include "upp_fifo.h"

typedef struct reg_uppcr{
    uint32_t free               :1;
    uint32_t soft               :1;
    uint32_t rtemu              :1;
    uint32_t en                 :1;
    uint32_t swrst              :1;
    uint32_t reserved_1         :2;
    uint32_t db                 :1;
    uint32_t reserved_2         :24;
}reg_uppcr_t;
union uppcr_reg{
    uint32_t        all;
    reg_uppcr_t     bit;
};

typedef struct reg_updlb{
    uint32_t reserved_1         :12;
    uint32_t ab                 :1;
    uint32_t ba                 :1;
    uint32_t reserved_2         :18;
}reg_updlb_t;
union updlb_reg{
    uint32_t        all;
    reg_updlb_t     bit;
};

typedef struct reg_upctl{
    uint32_t mode               :2;
    uint32_t chn                :1;
    uint32_t sdrtxil            :1;
    uint32_t ddrdemux           :1;
    uint32_t reserved_1         :11;
    uint32_t dra                :1;
    uint32_t iwa                :1;
    uint32_t dpwa               :3;
    uint32_t dpfa               :2;
    uint32_t reserved_2         :1;
    uint32_t drb                :1;
    uint32_t iwb                :1;
    uint32_t dpwb               :3;
    uint32_t dpfb               :2;
    uint32_t reserved_3         :1;
}reg_upctl_t;
union upctl_reg{
    uint32_t    all;
    reg_upctl_t bit;
};

typedef struct reg_upicr{
    uint32_t startpola          :1;
    uint32_t enapola            :1;
    uint32_t waitpola           :1;
    uint32_t starta             :1;
    uint32_t enaa               :1;
    uint32_t waita              :1;
    uint32_t reserved_1         :2;
    uint32_t clkdiva            :4;
    uint32_t clkinva            :1;
    uint32_t trisa              :1;
    uint32_t reserved_2         :2;
    uint32_t startpolb          :1;
    uint32_t enapolb            :1;
    uint32_t waitpolb           :1;
    uint32_t startb             :1;
    uint32_t enab               :1;
    uint32_t waitb              :1;
    uint32_t reserved_3         :2;
    uint32_t clkdivb            :4;
    uint32_t clkinvb            :1;
    uint32_t trisb              :1;
    uint32_t reserved_4         :2;
}reg_upicr_t;
union upicr_reg{
    uint32_t    all;
    reg_upicr_t bit;
};

typedef struct reg_upivr{
    uint32_t vala               :16;
    uint32_t valb               :16;
}reg_upivr_t;
union upivr_reg{
    uint32_t    all;
    reg_upivr_t bit;
};

typedef struct reg_uptcr{
    uint32_t rdsizei            :2;
    uint32_t reserved_1         :6;
    uint32_t rdsizeq            :2;
    uint32_t reserved_2         :6;
    uint32_t txsizea            :2;
    uint32_t reserved_3         :6;
    uint32_t txsizeb            :2;
    uint32_t reserved_4         :6;
}reg_uptcr_t;
union uptcr_reg{
    uint32_t    all;
    reg_uptcr_t bit;
};

typedef struct reg_upixx{
    uint32_t dpei               :1;
    uint32_t uori               :1;
    uint32_t erri               :1;
    uint32_t eowi               :1;
    uint32_t eoli               :1;
    uint32_t reserved_1         :3;
    uint32_t dpeq               :1;
    uint32_t uorq               :1;
    uint32_t errq               :1;
    uint32_t eowq               :1;
    uint32_t eolq               :1;
    uint32_t reserved_2         :19;
}reg_upixx_t;
union upixx_reg{
    uint32_t    all;
    reg_upixx_t bit;
};

typedef struct reg_upid0{
    uint32_t addr               :3;
    uint32_t addrh              :29;
}reg_upid0_t;
union upid0_reg{
    uint32_t    all;
    reg_upid0_t bit;
};

typedef struct reg_upid1{
    uint32_t bcnt               :1;
    uint32_t bcnth              :15;
    uint32_t lncnt              :16;
}reg_upid1_t;
union upid1_reg{
    uint32_t    all;
    reg_upid1_t bit;
};

typedef struct reg_upid2{
    uint32_t lnoffset           :3;
    uint32_t lnoffseth          :13;
    uint32_t reserved_1         :16;
}reg_upid2_t;
union upid2_reg{
    uint32_t    all;
    reg_upid2_t bit;
};

typedef struct reg_upis1{
    uint32_t bcnt               :16;
    uint32_t lncnt              :16;
}reg_upis1_t;
union upis1_reg{
    uint32_t    all;
    reg_upis1_t bit;
};

typedef struct reg_upis2{
    uint32_t act                :1;
    uint32_t pend               :1;
    uint32_t reserved_1         :2;
    uint32_t wm                 :4;
    uint32_t reserved_2         :24;
}reg_upis2_t;
union upis2_reg{
    uint32_t    all;
    reg_upis2_t bit;
};


struct registers 
{
	uint32_t            uppid;
	union uppcr_reg     uppcr;
	union updlb_reg     updlb;
	union upctl_reg     upctl;
	union upicr_reg     upicr;
	union upivr_reg     upivr;
	union uptcr_reg     uptcr;
    union upixx_reg     upisr;
    union upixx_reg     upier;
    union upixx_reg     upies;
    union upixx_reg     upiec;
	uint32_t            upeoi;
	union upid0_reg     upid0;
	union upid1_reg     upid1;
	union upid2_reg     upid2;
	uint32_t            upis0;
	union upis1_reg     upis1;
	union upis2_reg     upis2;
	union upid0_reg     upqd0;
	union upid1_reg     upqd1;
	union upid2_reg     upqd2;
	uint32_t            upqs0;
	union upis1_reg     upqs1;
	union upis2_reg     upqs2;
};


struct c6748_device 
{
	
	conf_object_t* obj;
	
	general_signal_intf *signal_iface;
	
	conf_object_t* signal;
	
    conf_object_t* image;

    memory_space_intf* image_space;

    conf_object_t* upp_linker;

    upp_linker_data_tranf_intf *upp_linker_intf;

	struct registers regs;

    FIFO *channel_fifo[2];

    //int sche_id[2];
	time_handle_t* sche_id[2];

    //int dma_id[2];
	time_handle_t* dma_id[2];

    uint32_t size[2];

    uint32_t line[2];

    int isr_status;
	int dev_enable;
};
typedef struct c6748_device upp_c6748_device;
static char* regs_name[] = 
{
	"UPPID",
	"UPPCR",
	"UPDLB",
	"UPCTL",
	"UPICR",
	"UPIVR",
	"UPTCR",
	"UPISR",
	"UPIER",
	"UPIES",
	"UPIEC",
	"UPEOI",
	"UPID0",
	"UPID1",
	"UPID2",
	"UPIS0",
	"UPIS1",
	"UPIS2",
	"UPQD0",
	"UPQD1",
	"UPQD2",
	"UPQS0",
	"UPQS1",
	"UPQS2",
	NULL
};
#define UPP_INT 94

#define UPPID 0x00
#define UPPCR 0x04
#define UPDLB 0x08
#define UPCTL 0x10
#define UPICR 0x14
#define UPIVR 0x18
#define UPTCR 0x1c
#define UPISR 0x20
#define UPIER 0x24
#define UPIES 0x28
#define UPIEC 0x2c
#define UPEOI 0x30
#define UPID0 0x40
#define UPID1 0x44
#define UPID2 0x48
#define UPIS0 0x50
#define UPIS1 0x54
#define UPIS2 0x58
#define UPQD0 0x60
#define UPQD1 0x64
#define UPQD2 0x68
#define UPQS0 0x70
#define UPQS1 0x74
#define UPQS2 0x78
#endif  
