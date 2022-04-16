#ifndef __C6678_N25Q128_H__
#define __C6678_N25Q128_H__

/* All equipment register names, the list order and struct registers registers inside order is kept consistent */
static char* regs_name[] =
{
	NULL
};

/* register offset */
#define  REG_N25Q128_READ_OPERATION            0x0
#define  REG_N25Q128_WRITE_OPERATION           0x8
#define  REG_N25Q128_STATUS_OPERATION          0x10
#define  REG_N25Q128_FLAGSTATUS_OPERATION      0x18
#define  REG_N25Q128_PROGRAM_OPERATION         0x20
#define  REG_N25Q128_ERASE_OPERATION           0x28

/* N25Q128 Flash description */
#define  N25Q128_FLASHSIZE                       0x1000000 /* 16M bytes */
#define  N25Q128_FLASHSECTORCOUNT                    0x100 /* 256 */
#define  N25Q128_FLASHSECTORSIZE                   0x10000 /* 65536 bytes */
#define  N25Q128_FLASHSECTORSIZE_BIT                  0x10 /* 16 bits */
#define  N25Q128_FLASHSUBSECTORCOUNT                0x1000 /* 4096 */
#define  N25Q128_FLASHSUBSECTORSIZE                 0x1000 /* 4096 bytes */
#define  N25Q128_FLASHSUBSECTORSIZE_BIT                0xC /* 12 */
#define  N25Q128_FLASHPAGECOUNT                    0x10000 /* 65536 */
#define  N25Q128_FLASHPAGESIZE                       0x100 /* 256 bytes */
#define  N25Q128_FLASHOTPSIZE                         0x40 /* 64 bytes */
#define  N25Q128_FLASHADDRESSMASK                     0xFF
#define  N25Q128_FLASH_ERASE_VALUE                    0xFF

/* InstructionsCode */
enum
{
	/* RESET Operations */
	N25Q128_FLASH_INS_REN                    = 0x66,/* reset enable */
	N25Q128_FLASH_INS_RMEM                   = 0x99,/* reset memory */

	/* IDENTIFICATION Operations */
	N25Q128_FLASH_INS_RDID                   = 0x9F,/* read Identification */
	N25Q128_FLASH_INS_RDID_ALT               = 0x9E,/* read Identification */
	N25Q128_FLASH_INS_MULT_IO_RDID           = 0xAF,/* read multiple i/o read id */
	N25Q128_FLASH_INS_DISCOVER_PARAMETER     = 0x5A,/* read serial flash discovery parameter */

	/* READ operations */
	N25Q128_FLASH_INS_READ                   = 0x03,/* read data bytes */
	N25Q128_FLASH_INS_FAST_READ              = 0x0B,/* read data bytes at higher speed */
	N25Q128_FLASH_INS_DOFR                   = 0x3B,/* dual output Fast Read */
	N25Q128_FLASH_INS_DIOFR                  = 0x0B,/* dual input output Fast Read */
	N25Q128_FLASH_INS_DIOFR_ALT1             = 0x3B,/* dual input output Fast Read */
	N25Q128_FLASH_INS_DIOFR_ALT2             = 0xBB,/* dual input output Fast Read */
	N25Q128_FLASH_INS_QOFR                   = 0x6B,/* quad output Fast Read */
	N25Q128_FLASH_INS_QIOFR                  = 0x0B,/* quad output Fast Read */
	N25Q128_FLASH_INS_QIOFR_ALT1             = 0x6B,/* quad input/output Fast Read */
	N25Q128_FLASH_INS_QIOFR_ALT2             = 0xEB,/* quad input output Fast Read */

	/* WRITE operations */
	N25Q128_FLASH_INS_WREN                   = 0x06,/* write enable */
	N25Q128_FLASH_INS_WRDI                   = 0x04,/* write disable */

	/* REGISTER operations */
	N25Q128_FLASH_INS_RDSR                   = 0x05,/* read status register */
	N25Q128_FLASH_INS_WRSR                   = 0x01,/* write status register */
	N25Q128_FLASH_INS_RDLR                   = 0xE8,/* read lock register */
	N25Q128_FLASH_INS_CMD_WRLR               = 0xE5,/* write lock register */
	N25Q128_FLASH_INS_RFSR                   = 0x70,/* read flag status register */
	N25Q128_FLASH_INS_CLFSR                  = 0x50,/* clear flag status register */
	N25Q128_FLASH_INS_RDNVCR                 = 0xB5,/* read non volatile configuration register */
	N25Q128_FLASH_INS_WRNVCR                 = 0xB1,/* write non volatile configuration register */
	N25Q128_FLASH_INS_RDVCR                  = 0x85,/* read volatile configuration register */
	N25Q128_FLASH_INS_WRVCR                  = 0x81,/* write volatile configuration register */
	N25Q128_FLASH_INS_RDVECR                 = 0x65,/* read volatile enhanced configuration register */
	N25Q128_FLASH_INS_WRVECR                 = 0x61,/* write volatile enhanced configuration register */

	/* PROGRAM operations */
	N25Q128_FLASH_INS_PP                     = 0x02,/* PAGE PROGRAM */
	N25Q128_FLASH_INS_DIPP                   = 0xA2,/* DUAL INPUT FAST PROGRAM */
	N25Q128_FLASH_INS_DIEPP                  = 0x02,/* EXTENDED DUAL INPUT FAST PROGRAM */
	N25Q128_FLASH_INS_DIEPP_ALT1             = 0xA2,/* EXTENDED DUAL INPUT FAST PROGRAM (alt 1) */
	N25Q128_FLASH_INS_DIEPP_ALT2             = 0xD2,/* EXTENDED DUAL INPUT FAST PROGRAM (alt 2) */
	N25Q128_FLASH_INS_QIPP                   = 0x32,/* QUAD INPUT FAST PROGRAM */
	N25Q128_FLASH_INS_QIEPP                  = 0x02,/* EXTENDED QUAD INPUT FAST PROGRAM */
	N25Q128_FLASH_INS_QIEPP_ALT1             = 0x32,/* EXTENDED QUAD INPUT FAST PROGRAM (alt 1) */
	N25Q128_FLASH_INS_QIEPP_ALT2             = 0x38,/* EXTENDED QUAD INPUT FAST PROGRAM */

	/* ERASE operations */
	N25Q128_FLASH_INS_SSE                    = 0x20,/* sub sector erase */
	N25Q128_FLASH_INS_SE                     = 0xD8,/* sector erase */
	N25Q128_FLASH_INS_DE                     = 0xC4,/* die erase */

	/* OTP operations */
	N25Q128_FLASH_INS_RDOTP                  = 0x4B,/* read OTP array */
	N25Q128_FLASH_INS_PROTP                  = 0x42,/* program OTP array */
};

/*device registers structure*/
struct registers
{
	uint8_t reg_n25q128_read_opearation;
	uint8_t reg_n25q128_write_opearation;
	uint8_t reg_n25q128_status;
	uint8_t reg_n25q128_flagstatus;
	uint8_t reg_n25q128_program_operation;
	uint8_t reg_n25q128_erase_operation;
};

/*Device object structure*/
struct c6678_n25q128_device_t
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
typedef struct c6678_n25q128_device_t c6678_n25q128_device;


#endif
