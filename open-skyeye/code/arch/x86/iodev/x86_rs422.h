#ifndef _BX_RS422_H
#define _BX_RS422_H

#if BX_SUPPORT_RS422

#define RS422_BASE 0x9700
#define RS422_LEN  0x100

#define CARD_NUM    2
#define CHANNEL_NUM 4
#define COM_FIFO_SIZE 2048

class bx_rs422_c : public bx_rs422_stub_c {
public:
  bx_rs422_c();
  virtual ~bx_rs422_c() {}
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

typedef struct config_data
{
   unsigned int  m_Baud;    
   unsigned char m_StopBits;
   unsigned char m_DataLen;
   unsigned char m_ParitySet;
   unsigned char m_IntPoll;/*1ÎªÖÐ¶Ï,0Îª²éÑ¯*/	
}config_data_t;

typedef struct {
	unsigned char *send_buf;
	unsigned char *recv_buf;
	uint32_t send_cnt;
	uint32_t recv_cnt;
	uint32_t send_length;
	uint32_t send_status;
	uint32_t recv_length;
	uint32_t recv_status;
	uint32_t com_cmd;
	uint32_t int_status; //ÖÐ¶Ï×´Ì¬¼Ä´æÆ÷
	uint32_t int_clr_reg; //ÇåÖÐ¶Ï¼Ä´æÆ÷
}com_fifo_t;

typedef struct {
	uint32_t com_id;
	config_data_t config_data;
	com_fifo_t com_fifo[CARD_NUM * CHANNEL_NUM];
} x86_rs422_reg_t;

typedef struct x86_rs422_device{
	conf_object_t 	*obj;                
	x86_rs422_reg_t* regs;

	int comHandle[CARD_NUM][CHANNEL_NUM];
	Rs422_bus_intf *bus_iface;
	conf_object_t *bus_obj;
	int Rs422_ID[CARD_NUM * CHANNEL_NUM];
	int id_cnt;
	int interrupt_number;
	RS422_FIFO *recv_fifo;
} x86_rs422_dev;

static char* regs_name[] = {
	"com_id",
	"send_length",
	"recv_length",
	"send_status",
	"recv_status",
	"com_cmd",
	"baud",
	"stopbit",
	"data_len",
	"parity",
	"intpoll",
	"com_tx_fifo",
	"com_rx_fifo",
	NULL
};

#define COM_HANDLE_START  0x0
#define COM_HANDLE_END  0x20

#define COM_ID          0x24
#define SEND_LENGTH     0x28
#define RECV_LENGTH     0x2c
#define SEND_STATUS     0x30
#define RECV_STATUS     0x34
#define COM_CMD         0x38
#define BAUD            0x3c
#define STOPBIT         0x40
#define DATALEN         0x44
#define PARITY          0x48
#define INTPOLL         0x4c
#define INT_STATUS      0x50
#define INT_CLR_REG     0x54

#define COM_TX_FIFO     0x60
#define COM_RX_FIFO     0x80

#endif

#endif
