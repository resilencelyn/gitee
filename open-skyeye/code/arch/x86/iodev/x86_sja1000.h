#ifndef _BX_SJA1000_H
#define _BX_SJA1000_H

#if BX_SUPPORT_SJA1000


#include "x86_can_fifo.h"
#define SJA1000_BASE 0x9100
#define SJA1000_LEN 0x100


class bx_sja1000_c : public bx_sja1000_stub_c {
public:
  bx_sja1000_c();
  virtual ~bx_sja1000_c() {}
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

typedef struct {
	uint8_t mode;
	uint8_t sr;
	uint8_t cmd;
	uint8_t btr0, btr1;
	uint8_t ir;
	uint8_t ier;
	uint8_t output_ctrl;
	uint8_t error_code;
	uint8_t cdr;	
	uint8_t acr[4];
	uint8_t amr[4];
	uint8_t tx_fid;
	uint8_t tx_id[4];
	uint8_t tx_data[8];

	uint8_t rx_fid;
	uint8_t rx_id[4];
	uint8_t rx_data[8];
	uint8_t rcv_cnt;
	uint8_t channel_id;
}x86_sja1000_reg_t;

typedef struct cpld_reg{
	uint32_t offset;
	uint32_t data;
}cpld_reg_t;

typedef struct x86_sja1000_device{
	conf_object_t *obj;                
	x86_sja1000_reg_t* regs;
	cpld_reg_t* cpld_regs;

	conf_object_t* can_linker;
	can_linker_intf *can_linker_iface;
	can_msg_t now_msg;
	int now_msg_out;
	CAN_FIFO *rec_fifo;

	int interrupt_number;
	int can_id;
}x86_sja1000_dev;

static char* regs_name[] = {
	"mode",
	"sr",
	"cmd",
	"btr0",
	"btr1",
	"ir",
	"ier",
	"output_ctrl",
	"error_code",
	"cdr",
	"acr0",
	"acr1",
	"acr2",
	"acr3",
	"amr0",
	"amr1",
	"amr2",
	"amr3",
	"tx_fid",
	"tx_id0",
	"tx_id1",
	"tx_data0",
	"tx_data1",
	"tx_data2",
	"tx_data3",
	"tx_data4",
	"tx_data5",
	"tx_data6",
	"tx_data7",
	"rx_fid",
	"rx_id0",
	"rx_id1",
	"rx_data0",
	"rx_data1",
	"rx_data2",
	"rx_data3",
	"rx_data4",
	"rx_data5",
	"rx_data6",
	"rx_data7",
	"rcv_cnt",
	"channel_id",
	NULL
};
#define PELI_IR_RI 0x01
#define PELI_IR_TI 0x02

#endif


#endif
