#ifndef __AT697F_IIC_H__
#define __AT697F_IIC_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"cmd",
	"iic_write",
	"iic_read",
	"status",
	NULL
};

// device register structure
struct registers
{
	uint32_t cmd;
	uint32_t iic_write;
	uint32_t iic_read;
	uint32_t status;
};

#define BUF_SIZE 0x20
#define IICdataReadlen 512

typedef struct
{
	uint16_t	syndata;
	uint8_t 	unitid;
	uint8_t 	cmdtype;
	uint16_t  length;
	unsigned char data[IICdataReadlen];
	//uint8_t   checksum;
}IICipmbcmdparadef;

// Device object structure
struct at697f_iic_device_t
{
	conf_object_t* obj;

	// Interface objects and methods;
	struct registers regs;
	uint32_t recv_buf[IICdataReadlen];
	uint32_t rx_index;
	uint32_t rx_length;

	uint32_t iic_buf[BUF_SIZE];
	uint32_t tx_index;

	FILE *fp;
	FILE *fp_e3;
	FILE *fp_e5;

};
typedef struct at697f_iic_device_t at697f_iic_device;
// Register offset
#define CMD 0x0
#define IIC_WRITE 0x4
#define IIC_READ 0x8
#define STATUS 0xc


#endif
