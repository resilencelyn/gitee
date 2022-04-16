#ifndef __UPP_LINKER_H__ 
#define __UPP_LINKER_H__ 

struct upp_linker 
{
	
	conf_object_t* obj;

    conf_object_t* upp[2];

    upp_data_signal_intf* upp_data_signal[2];
	
};
typedef struct upp_linker upp_linker_t;
#endif  
