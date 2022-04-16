#ifndef _BX_BU_61580_H
#define _BX_BU_61580_H

#if BX_SUPPORT_BU_61580


#define PointBase 0x9000

class bx_bu_61580_c : public bx_bu_61580_stub_c {
public:
  bx_bu_61580_c();
  virtual ~bx_bu_61580_c() {}
  virtual void init(void);
  virtual void reset (unsigned type) {}
  virtual void mem_write(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data);
  virtual void mem_read(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, void *data);
  void *obj;
private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  Bit32u read(Bit32u addr, unsigned io_len);
  void write(Bit32u addr, Bit32u dvalue, unsigned io_len);
};


typedef struct CMD_REG{
	uint8_t revision : 3;
	uint8_t term_address : 5;   //RT address
	uint8_t rt_flag :1;
	uint8_t sub_address : 5;    //RT sub address
	uint8_t MessageLength : 5;
}cmd_reg_t;


typedef struct Status_REG{
	uint16_t revision : 3;
	uint8_t term_address : 5;
	uint8_t instrumention:2;
	uint8_t data_quality:2;
	uint8_t response_error :1;
	uint8_t address_error : 1;
	uint8_t broad_fun_recv : 1;
	uint8_t dedicate_fun_recv : 1;
	uint8_t busb_shutdown : 1;
	uint8_t busa_shutdown : 1;
	uint8_t term_status : 1;
}status_reg_t;

typedef struct BCStatus_REG{
	uint16_t Revision : 16;
	uint8_t bc_eom : 1;
	uint8_t bc_som : 1;
	uint8_t bc_channelb : 1;
	uint8_t bc_err : 1;
	uint8_t bc_statset : 1;
	uint8_t bc_formaterr : 1;
	uint8_t bc_resp : 1;
	uint8_t bc_loopfall : 1;
	uint8_t bc_maskstatset : 1;
	uint8_t bc_retry1 : 1;
	uint8_t bc_retry0 : 1;
	uint8_t bc_gddatatx : 1;
	uint8_t bc_wrngstadngap : 1;
	uint8_t bc_wrdcnterr : 1;
	uint8_t bc_incsync : 1;
	uint8_t bc_invalidwd : 1;
}bcstatus_reg_t;
typedef struct x86_bu61580_reg{
   int intrpt_mask;  //#1100_8002#
   int config_1;            
   int config_2;            
   int start_reset;                        // --启动/复位寄存器
   int stackptr;                       // --BC/RT命令栈点寄存器，消息描述符栈指针
   int ctrl_word;                      //--BC控制字
   //uint32_t RTSA_CTRL_WORD                         //--RT子地址控制字寄存器
   int timetag;                        //--时标寄存器
   int intrpt_sta;
   int config_3;            
   int config_4;            
   int config_5;            
   int mtd_data_stk;                   // --数据栈地址寄存器
   int bc_fmetimer;                    // --BC帧时间保留寄存器
   int bc_msgtimer;                    // --BC消息间时间保留寄存器
   int bc_frmtime;                     // --BC消息帧寄存器/RT上一个命令字
   int rt_stawd;                       // --RT状态字寄存器
   int rt_bitword;                     // --RT比特字寄存器
}x86_bu61580_reg_t;

typedef struct x86_bu61580_device{
	conf_object_t *obj;                
	x86_bu61580_reg_t* regs;
	union {
		cmd_reg_t flag;
		uint32_t value;
	}cmd_reg;
	union {
		status_reg_t flag;
		uint32_t value;
	}status_reg;
	union {
		bcstatus_reg_t flag;
		uint32_t value;
	}bcstatus_reg;
	char **regs_name;
	conf_object_t* c28_bu61580_obj;
	std_1553b_bus_intf *std_1553b_bus_iface;
	conf_object_t *std_1553b_bus_obj;
	int dev_id;
	int cmd_word;
	int data_cnt;
	int recv_length;
	uint16_t DataBlockA_offset;
	uint16_t DataBlockB_offset;
	uint16_t DataBlock;
	char *send_buf;
	char *recv_buf;
	uint16_t *ptr;
	uint16_t *stkptr;
	uint16_t stkptrA;
	uint16_t stkptrB;
	uint16_t *TabASaRx;
	uint16_t *TabASaTx;
	uint16_t *TabASaBcst;
	uint16_t *TabASaCtrl_Word;
	uint16_t *TabBSaRx;
	uint16_t *TabBSaTx;
	uint16_t *TabBSaBcst;
	uint16_t *TabBSaCtrl_Word;
	char *DataBlockA;
	char *DataBlockB;
	uint16_t *BusyLookTab;
	uint16_t slave_count;              /*the count of all RT device link the i2c bus*/
}x86_bu61580_dev;

static char* regs_name[] = {
	"Intrpt_mask",
	"Config1",
	"Config2",
	"Control",
	"Stackptr",
	"Ctrl_word",
	"Timetag",
	"Intrpt_status",
	"Config3",
	"Config4",
	"Config5",
	"Mtd_data_stk",
	"Bc_fmetimer",
	"Bc_msgtimer",
	"Bc_frmtime",
	"Rt_statusword",
	"Bc_bitword",
	NULL
};
#define RT_BIT (0x1 << 15)


#endif


#endif
