#ifndef __LOONGSON_FLASH_H__
#define __LOONGSON_FLASH_H__

typedef struct CMD_T_BITS
{
	uint32_t pageaddr:16;
	uint32_t reserved:12;
	uint32_t command:4;
}CMD_T_BITS;

union CMD_T
{
	CMD_T_BITS bits;
	uint32_t all;
}CMD_T;

typedef struct STS_T_BITS
{
	uint32_t pe_end:1;
	uint32_t no_permission:1;
	uint32_t ecc_error:2;
	uint32_t tbc_error:2;
	uint32_t Crdy:1;
	uint32_t VPOS_good:1;
	uint32_t VNEG_good:1;
	uint32_t reserved:3;
	uint32_t err_addr:20;
}STS_T_BITS;

union STS_T
{
	STS_T_BITS bits;
	uint32_t all;	
}STS_T;

typedef struct PET_T_BITS
{
	uint32_t ptime:3;
	uint32_t etime:3;
	uint32_t reserved1:10;
	uint32_t int_en:4;
	uint32_t reserved2:12;
}PET_T_BITS;

union PET_T
{
	PET_T_BITS bits;
	uint32_t all;
}PET_T;

// device register structure
typedef struct loongson_flash_reg_t
{
	union CMD_T cmd;
	union STS_T sts;
	union PET_T pet;
}loongson_flash_reg_t;

// Device object structure
typedef struct loongson_flash_device_t
{
	conf_object_t* obj;
	memory_space_intf* image_space;
	conf_object_t* image;
	loongson_flash_reg_t *regs;
	char *filename;
	FILE *wr_fp;
	FILE *rd_fp;
}loongson_flash_device_t;
typedef struct loongson_flash_device_t loongson_flash_device;

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"CMD",
	"STS",
	"PET",
	NULL
};

#define CMD 0x00
#define STS 0x04
#define PET 0x08
#endif
