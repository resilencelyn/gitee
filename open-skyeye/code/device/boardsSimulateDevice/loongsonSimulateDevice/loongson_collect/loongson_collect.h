#ifndef __LOONGSON_COLLECT_H__
#define __LOONGSON_COLLECT_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"freaq_ch0",
	"freaq_ch1",
	"freaq_ch2",
	"freaq_ch3",
	"freaq_ch4",
	"freaq_ch5",
	"readover_flag",
	"oc_ctrl_base",
	"valve_current_state_base",
	"side_valve_set_steps",
	"bypass_valve_set_steps",
	"side_valve_check_enable_config",
	"bypass_valve_check_enable_config",
	"pump_host_ctrl",
	"pump_backup_ctrl",
	NULL
};
/*-----------freq-------------------*/
#define READOVER_FLAG 0x0
#define FREQ_BASE 0x0
#define OC_CTRL_BASE 0x0
#define VALVE_CURRENT_STATE_BASE 0x0
#define SIDE_VALVE_SET_STEPS 0x0
#define BYPASS_VALVE_SET_STEPS 0x0
#define SIDE_VALVE_CHECK_ENABLE_CONFIG 0x0
#define BYPASS_VALVE_CHECK_ENABLE_CONFIG 0x4
#define PUMP_HOST_CTRL 0x0
#define PUMP_BACKUP_CTRL 0x0

#define VALVE_STATE_TRUN_ON 0x6
#define VALVE_STATE_TRUN_OFF 0x9


#define MAX_CH 6
#define PUMP_SPEED_STEP 0x4  
struct registers
{
	uint32_t freaq_ch[6];
	uint32_t readover_flag;
	uint32_t oc_ctrl_base;
	uint32_t valve_current_state_base;
	uint32_t side_valve_set_steps;
	uint32_t bypass_valve_set_steps;
	uint32_t side_valve_check_enable_config;
	uint32_t bypass_valve_check_enable_config;
	uint32_t pump_host_ctrl;
	uint32_t pump_backup_ctrl;
	
};

// Device object structure
struct loongson_collect_device_t
{
	conf_object_t* obj;
	ad_linker_intf *ad_linker_iface;
	conf_object_t *ad_linker;
	// Interface objects and methods;
	struct registers regs;

	int			dataType;
	uint16_t	byPassStartVal;
	uint16_t	nonbyPassStartVal;

	int			byPassThread;
	int			nonbyPassThread;

	uint16_t	byPassEndVal;
	uint16_t	nonbyPassEndVal;

	int			H_WR_Bit;
	int			H_WR_flag;
	uint32_t	H_Cur_WR_Data;
	uint32_t	H_DecreThreVal;
	uint8_t		H_RecvThreshlod[2];
	
	int			L_WR_Bit;
	int			L_WR_flag;
	uint32_t	L_Cur_WR_Data;
	uint32_t	L_DecreThreVal;
	uint8_t		L_RecvThreshlod[2];
	
	int			UH_WR_Bit;			/*UP Bus Write Bit*/
	int			UH_WR_flag;			/*UP Bus Write flag*/
	uint32_t	UH_Cur_WR_Data;		/*UP Bus Cur Write Data*/
	uint32_t	UH_DecreThreVal;	/*UP Bus Decrement Threshlod val*/
	uint8_t		UH_RecvThreshlod[2];

	int			UL_WR_Bit;
	int			UL_WR_flag;
	uint32_t	UL_Cur_WR_Data;
	uint32_t	UL_DecreThreVal;
	uint8_t		UL_RecvThreshlod[2];

};
typedef struct loongson_collect_device_t loongson_collect_device;
#define HOLFREQ_GET_TYPE	0x6
#define ELCTCTLR_GET_TYPE	0x7
#define ELECIFACE_OUT_TYPE	0x8
#define B1553_DATA_TYPE		0x9

#define TYPE_MIN	0x6

#define FILING_MID_BIT(va,vb)	((va << 1 | vb) << 26)
#define FILING_END_FBIT(val)	(val << 28)

#define bitsmask(n, m) ((~(uint32_t)0 ) >> (m) << (31 - (n) + (m)) >> (31 - (n)))
#define clear_bits(x, n, m) ((x) & ~bitsmask((n), (m)))

#endif
