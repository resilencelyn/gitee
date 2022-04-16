#ifndef _BX_AD_H
#define _BX_AD_H

#if BX_SUPPORT_AD

#define AD_BASE 0x9500
#define AD_LEN 0x100
#define AD_CH_NUM 4

class bx_ad_c : public bx_ad_stub_c {
public:
  bx_ad_c();
  virtual ~bx_ad_c() {}
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
	uint32_t ad_mode;
	uint32_t ad_data[4];
}ad_reg_t;

typedef struct x86_ad_device{
	conf_object_t *obj;                
	ad_reg_t* ad_regs;
}x86_ad_dev;

static char* regs_name[] = {
	"ad_mode",
	"ad_data0",
	"ad_data1",
	"ad_data2",
	"ad_data3",
	NULL
};

#endif
#endif
