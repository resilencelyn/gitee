#ifndef _BX_BU_61580_H
#define _BX_BU_61580_H

#if BX_SUPPORT_BU_61580

#include <skyeye_lock.h>

#define PointBase  	0x6000
#define RAM_PointBase 	0x7000

typedef struct
{
    network_mesg_t*     pFirst;
    network_mesg_t*     pLast; 
    network_mesg_t*     pIn;
    network_mesg_t*     pOut;
    unsigned int   Length;
    unsigned int   Enteres;
    RWLOCK_T lock;
    char *name;
}BUFIFO;

class bx_bu_61580_c : public bx_bu_61580_stub_c {
public:
  bx_bu_61580_c();
  virtual ~bx_bu_61580_c() {}
  virtual void init(void);
  virtual void reset (unsigned type) {}
  void *obj;
  void *mem_obj;
private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u addr, unsigned io_len);
  void write(Bit32u addr, Bit32u dvalue, unsigned io_len);


  static Bit32u read_ram_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_ram_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
   Bit32u mem_read(Bit32u addr, unsigned io_len);
  void mem_write(Bit32u addr, Bit32u dvalue, unsigned io_len);
};

typedef struct{
	uint16_t msg_status_word; 
	uint16_t msg_time_tag_word; 
	uint16_t msg_gap_time_word; 
	uint16_t msg_block_pointer; 
}bc_msg_des_block_t;


typedef struct{
	uint16_t rt_data_block_table[100][2];
	uint32_t index;
}rt_data_block_table_t;



typedef struct{
	mesg_type_t mesg_type;
	uint32_t data_cnt; 
	uint32_t data_counter; 
}mark_object_t;

inline uint32_t btr5(void*arg){
	mark_object_t *mark_object = (mark_object_t *)arg;
	return  2 + mark_object->data_cnt;
}

inline uint32_t btr9(void*arg){
	mark_object_t *mark_object = (mark_object_t *)arg;
	return  3 + mark_object->data_cnt;
}

inline uint32_t rtr11(void*arg){
	mark_object_t *mark_object = (mark_object_t *)arg;
	return  5 + mark_object->data_cnt;
}

inline uint32_t rtb4(void*arg){
	mark_object_t *mark_object = (mark_object_t *)arg;
	return 0;
}

inline uint32_t br3(void*arg){
	mark_object_t *mark_object = (mark_object_t *)arg;
	return  2 + mark_object->data_cnt;
}

int32_t data_pos_base[MESG_TYPE_CNT] = {\
 2,//BC_TO_RT,
 5,//RT_TO_RT,
 4,//RT_TO_BC,
 5,//RT_TO_RT_BROADCAST,
 1,//BROADCAST,
-1,//MODE_CODE_NO_DATA,
 4,//TX_MODE_CODE_WITH_DATA,
 2,//RX_MODE_CODE_WITH_DATA,
-1,//BROADCAST_MODE_CODE_NO_DATA,
 2,//BROADCAST_MODE_CODE_WITH_DATA,
};
inline uint32_t d_c(void* arg){
	mark_object_t *mark_object = (mark_object_t *)arg;
	return data_pos_base[mark_object->mesg_type] + mark_object->data_counter;
}

#define NF 0
typedef uint32_t(*calc_func_t)(void*);
calc_func_t calc_func_table[MESG_TYPE_CNT][WORD_TYPE_CNT] = {\
//0   1    2     3   4    5   6   7   8   9    10  11
 {NF, d_c, d_c, NF, NF,btr5, NF, NF, NF, btr9,NF, NF},    //BC_TO_RT,
 {NF, d_c, d_c, NF, NF,  NF, NF, NF, NF, NF,  NF, rtr11}, //RT_TO_RT,
 {NF, d_c, d_c, NF, NF,  NF, NF, NF, NF, NF,  NF, NF},    //RT_TO_BC,
 {NF, d_c, d_c, NF, NF,  NF, NF, NF, NF, NF,  NF, NF},    //RT_TO_RT_BROADCAST,
 {NF, d_c, d_c, br3,NF,  NF, NF, NF, NF, NF,  NF, NF},    //BROADCAST,
 {NF, NF,  NF,  NF, NF,  NF, NF, NF, NF, NF,  NF, NF},    //MODE_CODE_NO_DATA,
 {NF, NF,  NF,  NF, NF,  NF, NF, NF, NF, NF,  NF, NF},    //TX_MODE_CODE_WITH_DATA,
 {NF, NF,  NF,  NF, NF,  NF, NF, NF, NF, NF,  NF, NF},    //RX_MODE_CODE_WITH_DATA,
 {NF, NF,  NF,  NF, NF,  NF, NF, NF, NF, NF,  NF, NF},    //BROADCAST_MODE_CODE_NO_DATA,
 {NF, NF,  NF,  NF, NF,  NF, NF, NF, NF, NF,  NF, NF},    //BROADCAST_MODE_CODE_NO_DATA,
};

#define NP -1 //no pos 
#define NC -2 //need to calculation
int32_t word_type_pos_table [MESG_TYPE_CNT][WORD_TYPE_CNT] = {\
//0  1  2   3   4   5   6   7   8   9   10  11
 {1,NC, NC, NP, NP, NC, NP, NP, NP, NC, NP, NP}, //BC_TO_RT,
 {1,NC, NC, NP, NP, NP,  3, NP, NP, NP,  4, NC}, //RT_TO_RT,
 {1,NC, NC, NP, NP, NP,  2, NP, NP,  3, NP, NP}, //RT_TO_BC,
 {1,NC, NC, NP, NP, NP,  3, NP, NP, NP, NP, NP}, //RT_TO_RT_BROADCAST,
 {1,NC, NC, NC, NP, NP, NP, NP, NP, NP, NP, NP}, //BROADCAST,
 {1,NP, NP, NP, NP, NP, NP, NP,  2,  3, NP, NP}, //MODE_CODE_NO_DATA,
 {1, 4, NP, NP, NP, NP, NP, NP,  2,  3, NP, NP}, //TX_MODE_CODE_WITH_DATA,
 {1, 2, NP, NP, 3 , NP, NP, NP, NP,  4, NP, NP}, //RX_MODE_CODE_WITH_DATA,
 {1,NP, NP, NP, NP, NP, NP,  2, NP, NP, NP, NP}, //BROADCAST_MODE_CODE_NO_DATA,
 {1, 2, NP, NP,  3, NP, NP, NP, NP, NP, NP, NP}, //BROADCAST_MODE_CODE_WITH_DATA,
};

typedef struct{
	uint32_t data_length;
	uint32_t sub_addr;
	uint32_t rx_bx_flag;  //bx = 1, rx = 0 
	uint16_t *buffer;
}recv_mesg_t;

typedef struct{
	uint32_t data_block_pointer;
	mesg_type_t mesg_type;
	uint32_t data_cnt;
	uint32_t counter;
}subaddr_op_t;

typedef struct{
	uint32_t data_block_pointer; //指向数据块
	uint32_t data_cnt;//数据长度
}rt_subaddr_t;

typedef struct{
	uint32_t intr_mode; //中断模式
}subaddr_ctrlwd_t;

typedef struct{
	int sub_addr;
	int tx_rx_flag;
	int bx_flag;
	int data_length;
	uint16_t *ram_buffer;
}vector_table_t;

typedef struct {
	int 	 cnt;
	uint16_t *buffer;
}subaddr_sendData_t;
#define SUBADDR_CNT 128
#define RX_SUBADDR_CNT 32 
#define BX_SUBADDR_CNT 32 
#define TX_SUBADDR_CNT 32 
#define SUBADDR_CW_CNT 32 

#define VECTOR_ACE_NUM 16

typedef struct 
{
	conf_object_t *obj;
	struct registers{
		uint16_t imr;
		uint16_t cr1;
		uint16_t cr2;
		uint16_t sr_sp;
		uint16_t bcw_rscw;
		uint16_t ttr;
		uint16_t isr;
		uint16_t cr3;
		uint16_t cr4;
		uint16_t cr5;
		uint16_t dsar;
		uint16_t ftrr;
		uint16_t mtrr;
		uint16_t ft_ls_tr;
		uint16_t swr;
		uint16_t bitwr;
		uint16_t tmr0;
		uint16_t tmr1;
		uint16_t tmr2;
		uint16_t tmr3;
		uint16_t tmr4;
		uint16_t tmr5;
		uint16_t tmr6;
		uint16_t tmr7;
	}regs;
	uint32_t irq_number;
	struct {
		core_signal_intf *signal_iface;
		conf_object_t *signal;
	} c6k_signal;
	struct {
		general_signal_intf *signal_iface;
		conf_object_t *signal;
	} c28_signal;
	struct {
		general_signal_intf *signal_iface;
		conf_object_t *signal;
	} sparc_signal;
	int core_signal;
	conf_object_t *std_1553b_linker;
	std_1553b_intf *std_1553b_linker_iface;

	conf_object_t *uart_file;
	uart_file_intf *uart_file_iface;

	uint16_t *ram_ptr;
	uint8_t *wt_ptr;
	uint8_t subaddr_op[SUBADDR_CNT]; 
	subaddr_op_t  subaddr_op_data[SUBADDR_CNT];
	mark_object_t mark_object_a[0x40]; //for BC stack A
	mark_object_t mark_object_b[0x40]; //for BC stack B

	int rt_ram_subaddr_ptr;
	int ace_vector;
	vector_table_t vector_table[VECTOR_ACE_NUM];
	int service_request;
	rt_subaddr_t  rx_subaddr_table[RX_SUBADDR_CNT];//RT接收表
	rt_subaddr_t  bx_subaddr_table[BX_SUBADDR_CNT];//RT广播接收表
	rt_subaddr_t  tx_subaddr_table[TX_SUBADDR_CNT];//RT发送表
	subaddr_ctrlwd_t  subaddr_ctrlwd_table[SUBADDR_CW_CNT];//RT子地址控制字表

	network_mesg_t network_mesg;
	conf_object_t* network;
	network_bus_intf* network_iface;
	RWLOCK_T lock;
	RWLOCK_T signal_lock;
	BUFIFO *recv_fifo;
    	get_aocs_intf* aocs_iface;
	conf_object_t* aocs;

	conf_object_t* bridge;
	bu61580_bridge_intf* bridge_iface;
	subaddr_sendData_t subaddr_sendData[TX_SUBADDR_CNT];
	int rt_addr;
	int connect_rt_addr;
}bu_61580_dev;

#define SPARC_CORE 1
#define C6K_CORE   2
#define C28_CORE   3
static char* regs_name[] = {
};

#define IMR  		0
#define CR1             1
#define CR2             2
#define SR_SP           3
#define BCW_RSCW        4
#define TTR             5
#define ISR             6
#define CR3             7
#define CR4             8
#define CR5             9
#define DSAR            10
#define FTRR            11
#define MTRR            12
#define FT_LS_TR        13
#define SWR             14
#define BITWR           15
#define TMR0            16
#define TMR1            17
#define TMR2            18
#define TMR3            19
#define TMR4            20
#define TMR5            21
#define TMR6            22
#define TMR7            23

#define BU61580_MODE (((dev->regs.cr1 & 0xc000) >> 14) &0x3)
#define GET_MODE_FROM_VALUE(val)  (((val & 0xc000) >> 14) &0x3)
#define GET_ADDRESS_FROM_VALUE(val)  ((val  >> 1) &0x1f)
#define IS_ENHANCED_MODE ((dev->regs.cr3 >> 14) & 1)
#define IS_AREA_A (((dev->regs.cr1 >> 13) & 0x1) == 0)
#define IS_AREA_B (((dev->regs.cr1 >> 13) & 0x1) == 1)
#define AREA  ((dev->regs.cr1 >> 13) & 0x1)
#define RT_ADDRESS ((dev->regs.cr5 >> 1) & 0x1f)
#define GET_STATUS_WORD(addr,err,test,service,broadcast,busy,subsys,get,terminal) \
	(((addr & 0x1f) << 11)|(err << 10)|(test << 9)|(service << 8)|(broadcast << 4)|(busy << 3)|(subsys << 2)|(get << 1)| terminal)


#define BC_MESG_CNT 0x101 
#define BC_STACK_POINTER_A 0x100
#define BC_STACK_POINTER_B 0x104
#define BC_STACK_BASE_A 0x0
#define BC_STACK_BASE_B 0xF00
#define BC_SP_A (dev->ram_ptr[BC_STACK_POINTER_A])
#define BC_SP_B (dev->ram_ptr[BC_STACK_POINTER_B])
#define BC_SP ((AREA == 0) ? BC_SP_A : BC_SP_B)

inline void init_mark_object(bu_61580_dev *dev, uint32_t bc_mark, mesg_type_t type, uint32_t data_cnt)
{
	if(IS_AREA_A){
			dev->mark_object_a[bc_mark / 4].mesg_type = type;
			dev->mark_object_a[bc_mark / 4].data_cnt = data_cnt;
			dev->mark_object_a[bc_mark / 4].data_counter = 0;
	}
	else{
			dev->mark_object_b[(bc_mark - BC_STACK_BASE_B) / 4].mesg_type = type;
			dev->mark_object_b[(bc_mark - BC_STACK_BASE_B) / 4].data_cnt = data_cnt;
			dev->mark_object_b[(bc_mark - BC_STACK_BASE_B) / 4].data_counter = 0;
	}

}

inline mark_object_t *get_mark_object(bu_61580_dev *dev, uint32_t bc_mark)
{
	if(IS_AREA_A){
			return dev->mark_object_a + (bc_mark / 4);
	}
	else{
			return dev->mark_object_b + ((bc_mark - BC_STACK_BASE_B) / 4);
	}
}



#define INIT_MARK_OBJECT(bc_mark, type, data_cnt) init_mark_object(dev, bc_mark, type, data_cnt)
#define GET_MARK_OBJECT(bc_mark) get_mark_object(dev, bc_mark)

inline void increate_bc_sp(bu_61580_dev *dev, uint32_t step){
	if(IS_AREA_A)

		dev->ram_ptr[BC_STACK_POINTER_A] += step;
	else
		dev->ram_ptr[BC_STACK_POINTER_B] += step;
	return;
}
#define INCREASE_BC_SP (increate_bc_sp(dev, 1))
#define INCREASE_BC_SP4 (increate_bc_sp(dev, 4))


#define RT_STACK_POINTER_A 0x100
#define RT_STACK_POINTER_B 0x104

#define MDCODE_SELTAB_BASE  0x108 //模式码选择中断表 0x108~0x10F 
#define MDCODE_DATA1_BASE   0x110 //模式码数据 0x110~0x11F 
#define MDCODE_DATA2_BASE   0x121 //模式码数据 0x121~0x13F 
#define ACE_VECTOR_ADDR     0x120 //模式码矢量字

#define RT_SUBADDR_R_A_BASE 0x140 //area A,receive lookup table
#define RT_SUBADDR_W_A_BASE 0x160 //area A,transmit lookup table
#define RT_SUBADDR_B_A_BASE 0x180 //area A,broadcast loopup table optional
#define RT_SUBADDR_C_A_BASE 0x1A0 //area A,subaddress control word loopup table option

#define RT_SUBADDR_R_B_BASE 0x1C0 //area B,receive lookup table
#define RT_SUBADDR_W_B_BASE 0x1E0 //area B,transmit lookup table
#define RT_SUBADDR_B_B_BASE 0x200 //area B,broadcast loopup table optional
#define RT_SUBADDR_C_B_BASE 0x220 //area B,subaddress control word loopup table option

#define RT_SUBADDR_W_START_MEMBASE 0x800 //transmit Memory Start Address 
#define RT_SUBADDR_W_END_MEMBASE 0xc00 //transmit Memory End Address 

#define RT_DATA_BLOCK_START 0x260 //data block 0 ~ data block 100
#define RT_DATA_BLOCK_END   0x1000
#define RT_SUBADDR_R_BASE ((AREA == 0) ? RT_SUBADDR_R_A_BASE : RT_SUBADDR_R_B_BASE)
#define RT_SUBADDR_W_BASE ((AREA == 0) ? RT_SUBADDR_W_A_BASE : RT_SUBADDR_W_B_BASE)
#define RT_SUBADDR_B_BASE ((AREA == 0) ? RT_SUBADDR_B_A_BASE : RT_SUBADDR_B_B_BASE)
#define RT_SUBADDR_C_BASE ((AREA == 0) ? RT_SUBADDR_C_A_BASE : RT_SUBADDR_C_B_BASE)


#define RT_SP_A (dev->ram_ptr[RT_STACK_POINTER_A])
#define RT_SP_B (dev->ram_ptr[RT_STACK_POINTER_B])
#define RT_SP ((AREA == 0) ? RT_SP_A : RT_SP_B)

#define SP_OFFSET_0_LOW 	0x0
#define SP_OFFSET_0_HIGH 	0x2
#define SP_OFFSET_1_LOW 	0x4
#define SP_OFFSET_1_HIGH 	0x6
#define SP_OFFSET_2_LOW 	0x8
#define SP_OFFSET_2_HIGH 	0xa
#define SP_OFFSET_3_LOW 	0xc
#define SP_OFFSET_3_HIGH 	0xe
inline void increate_rt_sp(bu_61580_dev *dev){
	if(IS_AREA_A)

		RT_SP_A += 1;
	else
		RT_SP_B += 1;
	return;

}

#define INCREASE_RT_SP (increate_rt_sp(dev))


//command word
#define COMMAND_RT_ADDR(word) ((word >> 11) & 0x1f)
#define COMMAND_T_R(word) ((word >> 10) & 0x1)
#define COMMAND_SUB_ADDR(word) (((word) >> 5) & 0x1f)
#define COMMAND_METHOD(word) ((word >> 5) & 0x1f)
#define COMMAND_DATA_CNT(word) (word & 0x1f)
#define COMMAND_METHOD_CODE(word) (word & 0x1f)

#define RT_RECEIVE 0
#define RT_SEND 1

//Mode Code Type //0(T/R) 00000(MODE CODE)
#define DYNAMIC_BUS_CONTROL                     0x0  
#define SYNCHRONIZE                             0x1  
#define TRANSMIT_STATUS_WORD                    0x2  
#define INITIATE_SELF_TEST      		0x3  
#define TRANSMIT_SHUTDOWN      			0x4  
#define OVERRIDE_TRANSMIT_SHUTDOWN      	0x5  
#define INHIBIT_TERMINAL_FLAG      		0x6  
#define OVERRIDE_INHIBIT_TERMINAL_FLAG      	0x7  
#define RESET_REMODE_TERMINAL      		0x8  
#define TRANSMIT_VECTOR_WORD      		0x10  
#define SYNCHRONIZE_WITH_DATA      		0x11  
#define TRANSMIT_LAST_COMMAND      		0x12  
#define TRANSMIT_BIT_WORD      			0x13  
#define SELECTED_TRANSMIT_SHUTDOWN              0x14  
#define OVERRIDE_SELECTED_TRANSMIT_SHUTDOWN     0x15  

//803矢量字格式 (仅限使用在803项目中)
#define V_WKMDCHG       0 //工作状态改变请求发送 子地址：21
#define V_CTURM         1 //遥测请求发送 子地址：3
#define V_PKGRM         2 //打包遥测请求发送 子地址：4
#define V_FASTRM        3 //快遥请求发送 子地址：16
#define V_BACKUP        4 //备份数据请求发送 子地址：14
#define V_RESTORE       5 //请求恢复上次备份数据 子地址：14
#define V_KERCTRL       6 //编队控制信息 子地址:11
#define V_KERDATA       7 //编队数据包 子地址：10
#define V_EPHEM         8 //本星星历数据 子地址：8
#define V_ANSDEVINFO    9 //测距测速信息 子地址：19
#define V_RELAPOS       10 //相对定位信息 子地址：6
#define V_LOCALPOS      11 //绝对定位信息 子地址：7
#define V_MEMDUMP       12 //内存下卸数据请求发送 子地址：18

#define DEFAULT_RAM_SIZE 0x8000
#define MAX_DATA_LENGTH  2048
#define NOT_IN_USE 0
#define IN_USE 1

typedef struct 
{
	conf_object_t *obj;
	conf_object_t *bu61580;
	bu61580_ram_intf *bu61580_ram_iface;
}bu_61580_ram_dev;


/*
 * configure 1 register 
 * */
//bc function
#define RT_BC_MT   			0x8000
#define MT_BC_RT  			0x4000
#define CURRENT_AREA_B_A 		0x2000
#define MESSAGE_STOP_ON_ERROR 		0x1000
#define FRAME_STOP_ON_ERROR 		0x800
#define STATUS_SET_STOP_ON_MESSAGE 	0x400
#define STATUS_SET_STOP_ON_FRAME 	0x200
#define FRAME_AUTO_REPEAT 		0x100
#define EXTERNAL_TRIGGER_ENABLED 	0x80
#define INTERNAL_TRIGGER_ENABLED 	0x40
#define INTERMESSAGE_GAP_TIMER_ENABLED	0x20
#define CR1_RETRY_ENABLED 		0x10
#define DOUBLED_SINGLE_RETRY 		0x8
#define BC_ENABLED  			0x4
#define BC_FRAME_IN_PROGRESS  		0x2
#define BC_MESSAGE_IN_PROGRESS 		0x1


//rt without alternate  status 
#define LOGIC1                          0x8000
#define LOBIC0                          0x4000
#define CURRENT_AREA_B_A                0x2000
#define MESSAGE_MONITOR_ENABLED 	0x1000
#define DYNAMIC_BUS_CONTROL_ACCEPTANCE  0x800
#define RT_BUSY                      	0x400
#define RT_SERVICE_REQUEST 		0x200
#define SUBSYSTEM_FLAG 			0x100
#define RTFLAG 				0x80
#define RT_MESSAGE_IN_PROGRESS		0x1

//rt with alternate status
#define RT_LOGIC1                  	0x8000
#define RT_LOGIC0  			0x4000
#define CURRENT_AREA_A_B 		0x2000
#define S10 				0x800
#define S09  				0x400
#define S08  				0x200
#define S07 				0x100
#define S06 				0x80
#define S05 				0x40
#define S04 				0x20
#define S03  				0x10
#define S02 				0x8
#define S01 				0x4
#define S00 				0x2
#define RT_MESSAGE_IN_PROGRESS          0x1


//MONITOR FUNCTION
#define MONITOR_LOGIC0  		0x8000
#define MONITOR_LOGIC1                  0x4000     
#define CURRENT_AREA_B_A                0x2000
#define MESSAGE_MONITOR_ENABLED         0x1000
#define TRIGGER_ENABLED_WORD 		0x800
#define START_ON_TRIGGER 		0x400
#define STOP_ON_TRIGGER  		0x200
#define EXTERNAL_TRIGGER_ENABLED 	0x80
#define MONITOR_ENABLED 		0x4
#define MONITOR_TRIGGERED 		0x2
#define MONITOR_ACTIVE   		0x1


/*
 * configure 2 register 
 * */
#define ENHANCED_INTERRUPTS                     0x8000
#define RAM_PARITY_ENABLE                       0x4000
#define BUSY_LOOKUP_TABLE_ENABLE  		0x2000
#define RX_SA_DOUBLE_BUFFER_ENABLE  		0x1000
#define OVERWRITE_INVALID_DATA                  0x800
#define WORD_BOUNDARY_DISABLE   		0x400
#define TIME_TAG_RESOLUTION2   			0x200
#define TIME_TAG_RESOLUTION1   			0x100
#define TIME_TAG_RESOLUTION0   			0x80
#define CLEAR_TIME_TAG_ON_SYNCHRONIZE  		0x40
#define LOAD_TIME_TAG_ON_SYNCHRONIZE  		0x20
#define INTERRUPT_STATUS_AUTO_CLEAR   		0x10
#define LEVEL_PULSE_INTERRUPT_REQUEST  		0x8
#define CLEAR_SERVICE_REQUEST 			0x4
#define ENHANCED_RT_MEMORY_MANAGEMENT 		0x2
#define SEPARATE_BROADCAST_DATA       		0x1     


/*
 * start/reset register 
 * */
#define BC_MT_STOP_ON_MESSAGE   		0x40
#define BC_STOP_ON_FRAME   			0x20
#define TIME_TAG_TEST_CLOCK 			0x10
#define TIME_TAG_RESET 				0x8
#define INTERRUPT_RESET 			0x4
#define BC_MT_START 				0x2
#define RESET                       		0x1

/*
 * bc/rt command stack pointer register
 * */
#define COMMAND_STACK_POINTER15                 0x8000
#define COMMAND_STACK_POINTER14                 0x4000
#define COMMAND_STACK_POINTER13                 0x2000
#define COMMAND_STACK_POINTER12                 0x1000
#define COMMAND_STACK_POINTER11  		0x800
#define COMMAND_STACK_POINTER10  		0x400
#define COMMAND_STACK_POINTER9 			0x200
#define COMMAND_STACK_POINTER8  		0x100
#define COMMAND_STACK_POINTER7  		0x80
#define COMMAND_STACK_POINTER6 			0x40
#define COMMAND_STACK_POINTER5  		0x20
#define COMMAND_STACK_POINTER4 			0x10
#define COMMAND_STACK_POINTER3 			0x8
#define COMMAND_STACK_POINTER2 			0x4
#define COMMAND_STACK_POINTER1 			0x2
#define COMMAND_STACK_POINTER0  		0x1


/*
 * bc control word register /rt subaddress control word register  
 * */

//bc control word register 
#define M_E_MASK  			0x4000
#define SERVICE_REQUEST_BIT_MASK  	0x2000
#define SUBSYS_BUSY_BIT_MASK   		0x1000
#define SUBSYS_FLAG_BIT_MASK 		0x800
#define TERMINAL_FLAG_BIT_MASK  	0x400
#define RESERVED_BITS_MASK  		0x200
#define BC_RETRY_ENABLED  		0x100
#define BUS_CHANNEL_A_B  		0x80
#define OFF_LINE_SELF_TEST  		0x40
#define MASK_BROADCAST_BIT 		0x20
#define EOM_INTERRUPT_ENABLE  		0x10
#define A_B_SELECT  			0x8
#define MODE_CODE_FORMAT 		0x4
#define BROADCAST_FORMAT 		0x2
#define RT_RT_FORMAT    		0x1


//rt subaddress control word register 
#define RX_DOUBLE_BUFFER_ENABLE         0x8000
#define TX_EOM_INT                      0x4000
#define TX_CIRC_BUF_INT                 0x2000
#define TX_MEMORY_MANAGEMENT2           0x1000
#define TX_MEMORY_MANAGEMENT1  	        0x800
#define TX_MEMORY_MANAGEMENT0   	0x400
#define RX_EOM_INT 			0x200
#define RX_CIRC_BUF_INT  		0x100
#define RX_MEMORY_MANAGEMENT2  		0x80
#define RX_MEMORY_MANAGEMENT1  		0x40
#define RX_MEMORY_MANAGEMENT0  		0x20
#define BCST_EOM_INT  			0x10
#define BCST_CIRC_BUF_INT  		0x8
#define BCST_MEMORY_MANAGEMENT2 	0x4
#define BCST_MEMORY_MANAGEMENT1 	0x2
#define BCST_MEMORY_MANAGEMENT0   	0x1

/*time tag register
 * */
#define TIME_TAG15  		0x8000
#define TIME_TAG14   		0x4000
#define TIME_TAG13  		0x2000
#define TIME_TAG12  		0x1000
#define TIME_TAG11  		0x800
#define TIME_TAG10  		0x400
#define TIME_TAG9 		0x200
#define TIME_TAG8 		0x100
#define TIME_TAG7 		0x80
#define TIME_TAG6  		0x40
#define TIME_TAG5 		0x20
#define TIME_TAG4 		0x10
#define TIME_TAG3 		0x8
#define TIME_TAG2 		0x4
#define TIME_TAG1  		0x2
#define TIME_TAG0       	0x1

/*interrupt status register
 * */
#define MASK_INTERRUPT  				0x8000
#define RAM_PARITY_ERROR  				0x4000
#define BC_RT_TRANSMITTER_TIMEOUT  			0x2000
#define BC_RT_COMMAND_STACK_ROLLOVER   			0x1000
#define MT_COMMAND_STACK_ROLLOVER 			0x800
#define MT_DATA_STACK_ROLLOVER 				0x400
#define HS_FAIL 					0x200
#define BC_RETRY     					0x100
#define RT_ADDRESS_PARITY_ERROR 			0x80
#define TIME_TAG_ROLLOVER 				0x40
#define RT_CIRCULAR_BUFFER_ROLLOVER 			0x20
#define BC_CONTROL_WORD_EOM   				0x10
#define RT_SUBADDRESS_CONTROL_WORD_EOM 			0x10
#define BC_END_OF_FRAME 				0x8
#define FORMAT_ERROR 					0x4
#define BC_STATUS_SET 					0x2
#define RT_MODE_CODE        				0x2
#define MT_PATTERN_TRIGGER				0x2
#define END_OF_MESSAGE					0x1

/*configure register 3
 * */
#define ENHANCED_MODE_ENABLE                    0x8000
#define BC_RT_COMMAND_STACK SIZE1               0x4000
#define BC_RT_COMMAND STACK SIZE0               0x2000
#define MT_COMMAND_STACK_SIZE1                  0x1000
#define MT_COMMAND_STACK_SIZE0  		0x800
#define MT_DATA_STACK_SIZE2  			0x400
#define MT_DATA_STACK_SIZE1  			0x200
#define MT_DATA_STACK_SIZE0  			0x100
#define ILLEGALIZATION_DISABLED 		0x80
#define OVERRIDE_MODE_T_R_ERROR  		0x40
#define ALTERNATE_STATUS_WORD_ENABLE 		0x20
#define ILLEGAL_RX_TRANSFER_DISABLE 		0x10
#define BUSY_RX_TRANSFER_DISABLE  		0x8
#define RTFAIL_FLAG_WRAP_ENABLE 		0x4
#define A_MODE_CODES_ENABLE 			0x2
#define ENHANCED_MODE_CODE_HANDLING  		0x1


/*configure register 4
 * */
#define EXTERNAL_BIT_WORD_ENABLE                0x8000
#define INHIBIT_BIT_WORD_IF_BUSY                0x4000
#define MODE_COMMAND_OVERRIDE_BUSY              0x2000
#define EXPANDED_BC_CONTROL_WORD_ENABLE         0x1000
#define BROADCAST_MASK_ENABLE_XOR  		0x800
#define RETRY_IF_A_AND_M_E 			0x400
#define RETRY_IF_STATUS_SET 			0x200
#define ST_RETRY_ALT_SAME_BUS 			0x100
#define ND_RETRY_ALT_SAME_BUS  		0x80
#define VALID_M_E_NO DATA 			0x40
#define VALID_BUSY_NO_DATA 			0x20
#define MT_TAG_GAP_OPTION   			0x10
#define LATCH_RT_ADRRESS_WITH_CONFIG5 		0x8
#define TEST_MODE2 				0x4
#define TEST_MODE1 				0x2
#define TEST_MODE0               		0x1


/*configure register 5
 * */
#define MHZ12_CLOCK_SELECT  			0x8000
#define CR5_LOGIC0  				0x4000
#define EXTERNAL_TX_INHIBIT_A 			0x2000
#define EXTERNAL_TX_INHIBIT_B  			0x1000
#define EXPANDED_CROSSING ENABLED  		0x800
#define RESPONSE_TIMEOUT_SELECT_1  		0x400
#define RESPONSE_TIMEOUT_SELECT_0 		0x200
#define GAP_CHECK_ENABLED 			0x100
#define BROADCAST_DISABLED 			0x80
#define RT_ADDRESS_LATCH_TRANSPARENT 		0x40
#define RT_ADDRESS4 				0x20
#define RT_ADDRESS3 				0x10
#define RT_ADDRESS2 				0x8
#define RT_ADDRESS1 				0x4
#define RT_ADDRESS0  				0x2
#define RT_ADDRESS_PARITY 			0x1


/* DATA STACK ADDRESS REGISTER 
 * */
#define MONITOR_DATA_STACK_ADDRESS_15		0x8000
#define MONITOR_DATA_STACK_ADDRESS_14		0x4000
#define MONITOR_DATA_STACK_ADDRESS_13		0x2000
#define MONITOR_DATA_STACK_ADDRESS_12		0x1000
#define MONITOR_DATA_STACK_ADDRESS_11		0x800
#define MONITOR_DATA_STACK_ADDRESS_10 		0x400
#define MONITOR_DATA_STACK_ADDRESS_9 		0x200
#define MONITOR_DATA_STACK_ADDRESS_8  		0x100
#define MONITOR_DATA_STACK_ADDRESS_7 		0x80
#define MONITOR_DATA_STACK_ADDRESS_6 		0x40
#define MONITOR_DATA_STACK_ADDRESS_5  		0x20
#define MONITOR_DATA_STACK_ADDRESS_4  		0x10
#define MONITOR_DATA_STACK_ADDRESS_3  		0x8
#define MONITOR_DATA_STACK_ADDRESS_2 		0x4
#define MONITOR_DATA_STACK_ADDRESS_1  		0x2
#define MONITOR_DATA_STACK_ADDRESS_0   		0x1

/* BC FRAME TIME REMAINING REGISTER 
 * */
#define BC_FRAME_TIME_REMAINING_15		0x8000
#define BC_FRAME_TIME_REMAINING_14		0x4000
#define BC_FRAME_TIME_REMAINING_13		0x2000
#define BC_FRAME_TIME_REMAINING_12		0x1000
#define BC_FRAME_TIME_REMAINING_11		0x800
#define BC_FRAME_TIME_REMAINING_10		0x400
#define BC_FRAME_TIME_REMAINING_9		0x200
#define BC_FRAME_TIME_REMAINING_8		0x100
#define BC_FRAME_TIME_REMAINING_7		0x80
#define BC_FRAME_TIME_REMAINING_6		0x40
#define BC_FRAME_TIME_REMAINING_5		0x20
#define BC_FRAME_TIME_REMAINING_4		0x10
#define BC_FRAME_TIME_REMAINING_3		0x8
#define BC_FRAME_TIME_REMAINING_2		0x4
#define BC_FRAME_TIME_REMAINING_1		0x2
#define BC_FRAME_TIME_REMAINING_0  		0x1


/*BC MESSAGE TIME REMAINING REGISTER
 * */
#define BC_MESSAGE_TIME_REMAINING_15            0x8000
#define BC_MESSAGE_TIME_REMAINING_14            0x4000
#define BC_MESSAGE_TIME_REMAINING_13            0x2000
#define BC_MESSAGE_TIME_REMAINING_12            0x1000
#define BC_MESSAGE_TIME_REMAINING_11            0x800    
#define BC_MESSAGE_TIME_REMAINING_10            0x400    
#define BC_MESSAGE_TIME_REMAINING_9             0x200    
#define BC_MESSAGE_TIME_REMAINING_8             0x100    
#define BC_MESSAGE_TIME_REMAINING_7             0x80      
#define BC_MESSAGE_TIME_REMAINING_6             0x40      
#define BC_MESSAGE_TIME_REMAINING_5             0x20      
#define BC_MESSAGE_TIME_REMAINING_4             0x10      
#define BC_MESSAGE_TIME_REMAINING_3             0x8      
#define BC_MESSAGE_TIME_REMAINING_2             0x4      
#define BC_MESSAGE_TIME_REMAINING_1   		0x2     
#define BC_MESSAGE_TIME_REMAINING_0   		0x1     


/*BC Frame Time||RT Last Command||T trigger register
 * */
#define BIT15					0x8000
#define BIT14                                   0x4000
#define BIT13                                   0x2000
#define BIT12                                   0x1000
#define BIT11                                   0x800 
#define BIT10                                   0x400 
#define BIT9                                    0x200 
#define BIT8                                    0x100 
#define BIT7                                    0x80  
#define BIT6                                    0x40  
#define BIT5                                    0x20  
#define BIT4                                    0x10  
#define BIT3                                    0x8   
#define BIT2			             	0x4   
#define BIT1			             	0x2   
#define BIT0			             	0x1   

/*RT status word register
 * */
#define MESSAGE_ERROR				0x400
#define INSTRUMENTATION				0x200
#define STATUS_SERVICE_REQUEST			0x100 
#define BROADCAST_COMMAND_RECEIVED		0x10
#define STATUS_BUSY				0x8
#define STATUS_SUBSYSTEM_FLAG			0x4
#define DYNAMIC_BUS_CONTROL_ACCEPT		0x2
#define TERMINAL_FLAG		           	0x1

/*RT BIT word register
 * */
#define TRANSMITTER_TIMEOUT			0x8000
#define LOOP_TEST_FAILURE_B                     0x4000
#define LOOP_TEST_FAILURE_A                     0x2000
#define HANDSHAKE_FAILURE                       0x1000
#define TRANSMITTER_SHUTDOWN_B                  0x800 
#define TRANSMITTER_SHUTDOWN_A                  0x400 
#define TERMINAL_FLAG_INHIBITED                 0x200 
#define CHANNEL_B_A                             0x100 
#define HIGH_WORD_COUNT                         0x80  
#define LOW_WORD_COUNT                          0x40  
#define INCORRECT_SYNC_RECEIVED                 0x20  
#define PARITY_MANCHESTER_ERROR_RECEIVED        0x10  
#define RT_RT_GAP_SYNCH_ADDRESS_ERROR           0x8   
#define RT_RT_NO_RESPONSE_ERROR                 0x4   
#define RT_RT_2ND_COMMAND_WORD_ERROR            0x2   
#define COMMAND_WORD_CONTENTS_ERROR             0x1   


#define NEG_SWAP_HALF_WORD(a) (0xFFFF - (a & 0xFFFF))

#define BSWAP_HALF(a) ((a<<8) | (a>>8))

#define LEN 1
#define STACK_A         0x0 * LEN   //1553B栈 A 地址
#define STACKPOINT_A    0x100 * LEN //1553B栈 A 指针地址
#define MESSCOUNT_A     0x101 * LEN //消息计数地址
#define INI_STACKA_P    0x102 * LEN //初始堆栈A指针
#define INI_COUNT_A     0x103 * LEN //初始消息计数A

#define STACK_B         0x104 * LEN   //1553B栈 B 地址
#define STACKPOINT_B    0x105 * LEN //1553B栈 B 指针地址
#define MESSCOUNT_B     0x106 * LEN //消息计数地址 B
#define INI_STACKB_P    0x107 * LEN //初始堆栈B指针
#define INI_COUNT_B     0xf00 * LEN //初始消息计数B

#define SUBADDR_MAX_DATA_NUM 0x20


#endif

static exception_t bu_61580_ram_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count);
static exception_t bu_61580_ram_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count);
#endif
