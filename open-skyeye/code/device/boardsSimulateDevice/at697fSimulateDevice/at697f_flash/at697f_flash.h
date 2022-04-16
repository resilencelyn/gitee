#ifndef __AT697F_FLASH_H__
#define __AT697F_FLASH_H__

struct flash_cmd_addr8
{
	uint8_t keyVal1;
	uint8_t keyVal2;
	uint8_t keyVal3;
	uint8_t write_flag;
	uint8_t erase_ready;
	uint8_t is_erase;
};

struct flash_cmd_addr16
{
	uint16_t keyVal1;
	uint16_t keyVal2;
	uint16_t keyVal3;
	uint16_t write_flag;
	uint16_t erase_ready;
	uint16_t is_erase;
};

struct flash_cmd_addr32
{
	uint32_t keyVal1;
	uint32_t keyVal2;
	uint32_t keyVal3;
	uint32_t write_flag;
	uint32_t erase_ready;
	uint32_t is_erase;
};

typedef struct __flash
{
	struct flash_cmd_addr8  cmd_addr8;
	struct flash_cmd_addr16 cmd_addr16;
	struct flash_cmd_addr32 cmd_addr32;
}flash_cmd_t;

struct at697f_flash_device_t
{
	conf_object_t* obj;
	memory_space_intf* image_space;
	conf_object_t* image;
	flash_cmd_t flash_cmd;
	char *filename;
	FILE* wr_fp;
	FILE* rd_fp;
};

typedef struct at697f_flash_device_t at697f_flash_device;

#define RESET_ADDR      0x0

#define RESET_VAL8      0xf0
#define Addr1_bit8      0xaaa
#define Addr2_bit8      0x555

#define Key1Val8        0xaa
#define Key2Val8        0x55
#define Key3DataVal8    0xa0
#define Key3Erase1Val8  0x80
#define Key3Erase2Val8  0x10

#define RESET_VAL16     0xf0f0
#define Addr1_bit16     (0x555<<1)
#define Addr2_bit16     (0x2aa<<1)

#define Key1Val16       0xaaaa
#define Key2Val16       0x5555

#define Key3DataVal16   0xa0a0
#define Key3Erase1Val16 0x8080
#define Key3Erase2Val16 0x1010

#define RESET_VAL32     0xf0f0f0f0
#define Addr1_bit32     (0x555<<2)
#define Addr2_bit32     (0x2aa<<2)

#define Key1Val32       0xaaaaaaaa
#define Key2Val32       0x55555555

//bm3803 flash
//#define Key3DataVal32   0xa0a0a0a0
//#define Key3Erase1Val32 0x80808080
//#define Key3Erase2Val32 0x10101010

#define CMD_UNLOCK1			(0x00AA00AA)
#define CMD_UNLOCK2			(0x00550055)

#define CMD_GET_ID			(0x00900090)
#define CMD_ERASE_SETUP		(0x00800080)
#define CMD_ERASE_CHIP		(0x00100010)
#define CMD_ERASE_SECTOR	(0x00300030)
#define CMD_PROGRAM			(0x00A000A0)
#define CMD_RESET			(0x00F000F0)

#define FLASH_SECTOR_LEN	(128*1024)
#define FLASH_SIZE          0x80000
#endif
