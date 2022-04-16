#ifndef __NAND_C6748_H__ 
#define __NAND_C6748_H__ 
#include "nand_fifo.h"
struct registers 
{
	
	uint8 data;
	
	uint8 addr;
	
	uint8 cmd;
	
};

typedef enum
{
	NF_NOSTATUS,
	NF_readID_1st,
	NF_readID_2nd,
	NF_readID_3rd,
	NF_readID_4th,
	NF_readID_5th,
	NF_readID_finish
}
readidstatus;

typedef enum
{
	NF_noread,
	NF_addr_1st,
	NF_addr_2nd,
	NF_addr_3rd,
	NF_addr_4th,
	NF_addr_5th,
	NF_addr_finish
}
readstatus;

typedef enum
{
	NF_PROG,
	NF_PROG2
}
programstatus;

typedef enum
{
	NF_NONE,
	NF_DATAREADSTART,
	NF_DATAREAD,
	NF_DATAREADID,
	NF_DATAREADDOWN,
	NF_RESET,
	NF_BLOCKERASESTART,
	NF_BLOCKERASES,
	NF_ERASE,
	NF_READSTATUS,
	NF_DATAWRITE,
	NF_PAGEWRITESTART,
	NF_PAGEWRITEEND,
	NF_PROGRAM,
	NF_PROGRAM2
}
cmdtype;

struct properties
{
	uint32 pagesize;
	uint32 pagenum;
	uint32 blocknum;
	uint16 cursor;
};

struct c6748_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	skyeye_emif_intf* emif_iface;
	conf_object_t* emif;
	
	conf_object_t* signal;
	
	struct registers regs;
	uint64 address;

	readidstatus readid_st;
	readstatus read_st;
	cmdtype cmd_t;
	programstatus pro_st;
	
	struct properties pro;

	uint32 ***blocks;

	uint8 ID[5];
	int transfer_type;
	FIFO *read_fifo;
};

typedef struct c6748_device nand_c6748_device;
static char* regs_name[] = 
{
	"data",
	"addr",
	"cmd",
	NULL
};

#define DEFAULE_TRANSFER 0x0
#define DMA_TRANSFER 0x1
#define FIFO_LENGTH 4096

#define DATA 0x0
#define ADDR 0x8
#define CMD 0x10

#define NAND_CMD_READ			0x00
#define NAND_CMD_READ_CYC2		0x30
#define NAND_CMD_READID			0x90
#define NAND_CMD_RESET			0xff
#define NAND_CMD_PROGRAM		0x80
#define NAND_CMD_PROGRAM_CYC2	0x10
#define NAND_CMD_ERASE			0x60
#define NAND_CMD_ERASE_CYC2		0xd0
#define NAND_CMD_READ_STATUS	0x70
#define NAND_CMD_RANDOM			0x05
#define NAND_CMD_RANDOM_CYC2	0xe0

#define NAND_PAGESIZE			(2048 + 64)
#define NAND_PAGE_OF_BLK		(64)
#define NAND_BLOCK_OF_DEVICE	(4096)
#define BLANK					(0xff)

#define BLOCKADDRESSMASK		(0xfff)
#define PAGEADDRESSMASK			(0x3f)
#define COLUMADDRESSMASK		(0x7ff)

#define BLOCKADDRESSOFFSET		(22)
#define PAGEADDRESSOFFSET		(16)

#define ADDRESSOFFSET8BIT		(8)
#define	ADDRESSOFFSET16BIT		(16)
#define ADDRESSOFFSET24BIT		(24)
#define	ADDRESSOFFSET32BIT		(32) 

#endif  
