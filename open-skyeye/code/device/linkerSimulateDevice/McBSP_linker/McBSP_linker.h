#ifndef __MCBSP_LINKER_H__ 
#define __MCBSP_LINKER_H__ 

struct McBSP_linker
{
	
	conf_object_t* obj;
	
	conf_object_t* signal;

	core_signal_intf* signal_iface;

    conf_object_t* McBSP_device[2];

    McBSP_data_receive_intf* McBSP_device_intf[2];
	
};
typedef struct McBSP_linker McBSP_linker_t;
#endif  
