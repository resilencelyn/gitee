#ifndef __FIFO_MCBSP_C6748_H__ 
#define __FIFO_MCBSP_C6748_H__ 
#include "../McBSP_c6748/McBSP_fifo.h"

struct registers 
{
	
	uint32_t xbuf;
	
};


struct McBSP_c6748_device 
{

	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

    conf_object_t* edma;
    edma_device_event_t* edma_event;

    conf_object_t* fifo_data;
    fifo_data_intf* fifo_data_intf;

	memory_space_intf* image_space;
	conf_object_t* image;

	struct registers regs;

    FIFO *wfifo;
    FIFO *rfifo;

    uint32_t xevt;
    uint32_t revt;
    uint32_t channel;
    uint32_t channel_T;
    uint32_t channel_R;
    int fifo_w;
    int dev_number;
};

typedef struct McBSP_c6748_device fifo_McBSP_c6748_device;

static char* regs_name[] = 
{
	"XBUF",
	NULL
};
#define XBUF 0x0
#endif  
