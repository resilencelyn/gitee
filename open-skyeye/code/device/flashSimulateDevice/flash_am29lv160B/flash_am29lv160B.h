#ifndef __FLASH_AM29LV160B_H__ 
#define __FLASH_AM29LV160B_H__ 
#include <stdio.h>
struct registers 
{
	uint32_t addr;
	uint32_t data;
};

struct flash_am29lv160B_device_t 
{
	conf_object_t* obj;
	memory_space_intf* image_space;
	conf_object_t* image;
	int n_regs;
	int erase_val;
	int cnt;
	endian_t endian;
	struct registers regs[32];
	
};

#define CHIP_SIZE   0x80000//暂时定位8个64k扇区
#define SECTOR_SIZE 0x10000

typedef struct flash_am29lv160B_device_t flash_am29lv160B_device;
static char* regs_name[] =
{
	NULL
};
#define CMD_CHECK(io, n, o, v)				\
	(io->regs[n].addr == o && io->regs[n].data == v)

#define CMD_WORD_PROGRAM(io)				\
	(io->cnt == 3 &&				\
	 CMD_CHECK(io, 0, 0xaaa, 0xaa) &&		\
	 CMD_CHECK(io, 1, 0x555, 0x55) &&		\
	 CMD_CHECK(io, 2, 0xaaa, 0xa0))

#define CMD_ERASE(io)					\
	(io->cnt == 5 &&				\
	 CMD_CHECK(io, 0, 0xaaa, 0xaa) &&		\
	 CMD_CHECK(io, 1, 0x555, 0x55) &&		\
	 CMD_CHECK(io, 2, 0xaaa, 0x80) &&		\
	 CMD_CHECK(io, 3, 0xaaa, 0xaa) &&		\
	 CMD_CHECK(io, 4, 0x555, 0x55))
#endif
