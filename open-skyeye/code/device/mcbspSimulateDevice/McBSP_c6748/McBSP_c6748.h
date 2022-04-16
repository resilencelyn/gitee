#ifndef __MCBSP_C6748_H__ 
#define __MCBSP_C6748_H__ 

#include "McBSP_fifo.h"
typedef struct reg_spcr{
    uint32_t rrst       :1;
    uint32_t rrdy       :1;
    uint32_t rfull      :1;
    uint32_t rsyncerr   :1;
    uint32_t rintm      :2;
    uint32_t reserved_1   :1;
    uint32_t dxena      :1;
    uint32_t reserved_2 :3;
    uint32_t clkstp     :2;
    uint32_t rjust      :2;
    uint32_t dlb        :1;
    uint32_t xrst       :1;
    uint32_t xrdy       :1;
    uint32_t xempty     :1;
    uint32_t xsyncerr   :1;
    uint32_t xintm      :2;
    uint32_t grst       :1;
    uint32_t frst       :1;
    uint32_t soft       :1;
    uint32_t freerun    :1;
    uint32_t reserved_3 :6;
}reg_spcr_t;
union spcr_reg{
    uint32_t    all;
    reg_spcr_t  bit;
};

typedef struct reg_rcr{
    uint32_t reserved_1 :4;
    uint32_t rwdrevrs   :1;
    uint32_t rwdlen1    :3;
    uint32_t rfrlen1    :7;
    uint32_t reserved_2 :1;
    uint32_t rdatdly    :2;
    uint32_t rfig       :1;
    uint32_t rcompand   :2;
    uint32_t rwdlen2    :3;
    uint32_t rfrlen2    :7;
    uint32_t rphase     :1;
}reg_rcr_t;
union rcr_reg{
    uint32_t    all;
    reg_rcr_t   bit;
};

typedef struct reg_xcr{
    uint32_t xeserved_1 :4;
    uint32_t xwdrevrs   :1;
    uint32_t xwdlen1    :3;
    uint32_t xfrlen1    :7;
    uint32_t xeserved_2 :1;
    uint32_t xdatdly    :2;
    uint32_t xfig       :1;
    uint32_t xcompand   :2;
    uint32_t xwdlen2    :3;
    uint32_t xfrlen2    :7;
    uint32_t xphase     :1;
}reg_xcr_t;
union xcr_reg{
    uint32_t    all;
    reg_xcr_t   bit;
};

typedef struct reg_srgr{
    uint32_t clkgdv     :8;
    uint32_t fwid       :8;
    uint32_t fper       :12;
    uint32_t fsgm       :1;
    uint32_t clksm      :1;
    uint32_t clksp      :1;
    uint32_t gsync      :1;
}reg_srgr_t;
union srgr_reg{
    uint32_t    all;
    reg_srgr_t  bit;
};

typedef struct reg_mcr{
    uint32_t rmcm       :1;
    uint32_t reserved_1 :1;
    uint32_t rcblk      :3;
    uint32_t rpablk     :2;
    uint32_t rpbblk     :2;
    uint32_t rmcme      :1;
    uint32_t reserved_2 :6;
    uint32_t xmcm       :2;
    uint32_t xcblk      :3;
    uint32_t xpablk     :2;
    uint32_t xpbblk     :2;
    uint32_t xmcme      :1;
    uint32_t reserved_3 :6;
}reg_mcr_t;
union mcr_reg{
    uint32_t    all;
    reg_mcr_t   bit;
};

typedef struct reg_wfifoctl{
    uint32_t wnumdma    :8;
    uint32_t wnumevt    :8;
    uint32_t wena       :1;
    uint32_t reserved_1 :15;
}reg_wfifoctl_t;
union wififoctl_reg{
    uint32_t    all;
    reg_wfifoctl_t bit;
};

typedef struct reg_rfifoctl{
    uint32_t rnumdma    :8;
    uint32_t rnumevt    :8;
    uint32_t rena       :1;
    uint32_t reserved_1 :15;
}reg_rfifoctl_t;
union rififoctl_reg{
    uint32_t    all;
    reg_rfifoctl_t bit;
};


struct registers 
{
	uint32_t drr;
	uint32_t dxr;
	union spcr_reg  spcr;
	union rcr_reg   rcr;
	union xcr_reg   xcr;
	union srgr_reg  srgr;
	union mcr_reg   mcr;
	uint32_t rcere0;
	uint32_t xcere0;
	uint32_t pcr;
	uint32_t rcere1;
	uint32_t xcere1;
	uint32_t rcere2;
	uint32_t xcere2;
	uint32_t rcere3;
	uint32_t xcere3;
	uint32_t bfiforev;
	union wififoctl_reg wfifoctl;
	uint32_t wfifosts;
	union rififoctl_reg rfifoctl;
	uint32_t rfifosts;
};

struct c6748_device 
{
	conf_object_t* obj;
	general_signal_intf *signal_iface;
	conf_object_t* signal;
	struct registers regs;
	conf_object_t* McBSP_linker;
    McBSP_linker_data_connect_intf *McBSP_linker_intf;
    FIFO *receive_fifo;
    int dev_number;
    int sche_rr;
    int sche_xr;
    conf_object_t* edma;
    conf_object_t* fifo;
    edma_device_event_t* edma_event;
    fifo_device_event_t* fifo_event;

    conf_object_t* fifo_data;
    fifo_data_intf* fifo_data_intf;

    uint32_t rint_event;
    uint32_t xint_event;
    uint32_t channel_T;
    uint32_t channel_R;
};
typedef struct c6748_device McBSP_c6748_device;
static char* regs_name[] = 
{
	"drr",
	"dxr",
	"spcr",
	"rcr",
	"xcr",
	"srgr",
	"mcr",
	"rcere0",
	"xcere0",
	"pcr",
	"rcere1",
	"xcere1",
	"rcere2",
	"xcere2",
	"rcere3",
	"xcere3",
	"bfiforev",
	"wfifoctl",
	"wfifosts",
	"rfifoctl",
	"rfifosts",
	NULL
};
#define DRR 0x0000
#define DXR 0x0004
#define SPCR 0x0008
#define RCR 0x000c
#define XCR 0x0010
#define SRGR 0x0014
#define MCR 0x0018
#define RCERE0 0x001c
#define XCERE0 0x0020
#define PCR 0x0024
#define RCERE1 0x0028
#define XCERE1 0x002c
#define RCERE2 0x0030
#define XCERE2 0x0034
#define RCERE3 0x0038
#define XCERE3 0x003c
#define BFIFOREV 0x0800
#define WFIFOCTL 0x0810
#define WFIFOSTS 0x0814
#define RFIFOCTL 0x0818
#define RFIFOSTS 0x081c
#define XBUF 0x081c
#endif  
