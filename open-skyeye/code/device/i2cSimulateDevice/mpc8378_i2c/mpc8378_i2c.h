#ifndef __MPC8378_I2C_H__ 
#define __MPC8378_I2C_H__ 

struct registers 
{
	uint32_t adr;
	uint32_t fdr;
	uint32_t cr;
	uint32_t sr;
	uint32_t dr;
	uint32_t dfsrr;
};

struct mpc8378_i2c_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;

	conf_object_t* i2c_bus_obj;
	i2c_bus_intf* i2c_bus_iface;

	uint32_t deviceAddress;
	uint32_t blockNumber;

	struct registers regs;

#define RXBUF_MAXLEN 128
	uint8_t rxbuf[RXBUF_MAXLEN];
	uint32_t rx_index ;
	uint32_t read_flag;
};
typedef struct mpc8378_i2c_device_t mpc8378_i2c_device;
static char* regs_name[] = 
{
	"adr",
	"fdr",
	"cr",
	"sr",
	"dr",
	"dfsrr",
	NULL
};
//register_offset_define
#define ADR 0x0
#define FDR 0x4
#define CR 0x8
#define SR 0xc
#define DR 0x10
#define DFSRR 0x14
//CR cr_register_define
#define CR_MEN 0x80
#define CR_MIEN 0x20
#define CR_MSTA 0x10
#define CR_MTX 0x8
#define CR_TXAK 0x4
#define CR_RSTA 0x2
#define CR_BCST 0x1
//SR sr_register_define
#define SR_MCF 0x80
#define SR_MAAS 0x40
#define SR_MBB 0x20
#define SR_MAL 0x8
#define SR_BCSTM 0x10
#define SR_SRW 0x4
#define SR_MIF 0x2
#define SR_RXAK 0x1

#endif  

