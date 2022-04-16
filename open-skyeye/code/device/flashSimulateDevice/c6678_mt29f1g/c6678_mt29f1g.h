#ifndef __C6678_MT29F1G_H__
#define __C6678_MT29F1G_H__

/* All equipment register names, the list order and struct registers registers inside order is kept consistent */
static char* regs_name[] =
{
	NULL
};

/* Register offset */
#define REG_C6678_MT29F1G_STATUS_OPERATION               0x1
#define REG_C6678_MT29F1G_READ_OPERATION                 0x2
#define REG_C6678_MT29F1G_PROGRAM_OPERATION              0x3
#define REG_C6678_MT29F1G_ERASE_OPERATION                0x4

/* MT29F1G Flash description */
#define  MT29F1G_FLASHSIZE                         0x8000000/* 1Gbit/128Mbytes */
#define  MT29F1G_FLASHBLOCKCOUNT                       0x400/* 1024 */
#define  MT29F1G_FLASHBLOCKSIZE                      0x20000/* 128K bytes */
#define  MT29F1G_FLASHPAGECOUNT                         0x40/* 64 */
#define  MT29F1G_FLASHPAGESIZE                         0x800/* 2k bytes */
#define  MT29F1G_FLASH_ERASE_VALUE                      0xFF

/* InstructionsCode */
enum
{
	/* Reset Operations */
	MT29F1G_FLASH_INS_RESET                         = 0xFF,/* reset enable */

	/* Identification Operations */
	MT29F1G_FLASH_INS_RDID                          = 0x90,/* read Identification */
	MT29F1G_FLASH_INS_RDPARAMETERPAGE               = 0xEC,/* read parameter page */
	MT29F1G_FLASH_INS_RDUNIQUEID                    = 0xED,/* read unique id */

	/* Feature Operations */
	MT29F1G_FLASH_INS_SETFEATURES                   = 0xEF,/* set features */
	MT29F1G_FLASH_INS_GETFEATURES                   = 0xEE,/* get features */

	/* Status Operations */
	MT29F1G_FLASH_INS_RDSTATUS                      = 0x70,/* read status */

	/* Column Address Operations */
	MT29F1G_FLASH_INS_RDRANDOMDATA                  = 0x05,/* random data read */
	MT29F1G_FLASH_INS_RANDOMDATAINPUT               = 0x85,/* random data input */
	MT29F1G_FLASH_INS_PROGRAMFORINTERNALDATAINPUT   = 0x85,/* program for internal data input */

	/* Read Operations */
	MT29F1G_FLASH_INS_RDMODE                        = 0x00,/* read mode */
	MT29F1G_FLASH_INS_RDPAGE                        = 0x00,/* read page */
	MT29F1G_FLASH_INS_RDPAGECACHESEQUENTIAL         = 0x31, /* read page cache sequential */
	MT29F1G_FLASH_INS_RDPAGECACHERANDOM             = 0x00, /* read page cache random */
	MT29F1G_FLASH_INS_RDPAGECACHELAST               = 0x3F,/* read page cache last */

	/* Program operations */
	MT29F1G_FLASH_INS_PROGRAM_PAGE                  = 0x80,/* program page */
	MT29F1G_FLASH_INS_PROGRAM_PAGE_CACHE            = 0x80,/* program page cache */

	/* Erase Operations */
	MT29F1G_FLASH_INS_ERASEBLOCK                    = 0x60,/* erase block */

	/* Internal Data Move Operations */
	MT29F1G_FLASH_INS_READFORINTERNALDATAMOVE       = 0x00,/* read for internal data move */
	MT29F1G_FLASH_INS_PROGRAMFORINTERNALDATAMOVE    = 0x85,/* program for internal data move */

	/* One-Time Programmable (OTP) Operations */
	MT29F1G_FLASH_INS_OTPDATAPROGRAM                = 0x80,/* otp data program */
	MT29F1G_FLASH_INS_OTPDATAPROTECT                = 0x80,/* otp data protect */
	MT29F1G_FLASH_INS_OTPDATAREAD                   = 0x00,/* otp data read */
};

/* device register structure */
struct registers
{
	uint8_t reg_c6678_mt29f1g_status_operation;
	uint8_t reg_c6678_mt29f1g_read_operation;
	uint8_t reg_c6678_mt29f1g_program_operation;
	uint8_t reg_c6678_mt29f1g_erase_operation;
};

/* Device object structure */
struct c6678_mt29f1g_device_t
{
	conf_object_t* obj;
	memory_space_intf* image_space;
	conf_object_t* image;
	uint8_t write_flag;
	uint8_t erase_ready;
	uint8_t is_erase;
	struct registers regs;
	char *filename;
	FILE* wr_fp;
	FILE* rd_fp;
};
typedef struct c6678_mt29f1g_device_t c6678_mt29f1g_device;

#endif
