#ifndef __LCD_OMAPL138_H__
#define __LCD_OMAPL138_H__

#include <stdint.h>
#include <skyeye_lock.h>
#include "sdl_pixel.h"

#define MASK(a, b) ({uint32_t mask = (b) - (a) == 31 ? 0xffffffff : ((1u << ((b) - (a) + 1)) - 1); mask;})
#define CLRS(val, a, b) (val) = (val) & ~(MASK(a, b) << (a))
#define SETS(val, a, b, v) do {		\
	CLRS(val, a, b);		\
	(val) = (val) | (v) << (a);	\
} while(0)
#define BITS(val, a, b) (((val) >> (a)) & ((1 << (1 + (b) - (a))) - 1))

/* ----------------------------------------------------- */
struct registers {
	uint32_t revid;
	uint32_t lcd_ctrl;
	uint32_t lcd_stat;
	uint32_t lidd_ctrl;
	uint32_t lidd_cs0_conf;
	uint32_t lidd_cs0_addr;
	uint32_t lidd_cs0_data;
	uint32_t lidd_cs1_conf;
	uint32_t lidd_cs1_addr;
	uint32_t lidd_cs1_data;
	uint32_t raster_ctrl;
	uint32_t raster_timing_0;
	uint32_t raster_timing_1;
	uint32_t raster_timing_2;
	uint32_t raster_subpanel;
	uint32_t lcddma_ctrl;
	uint32_t lcddma_fb0_base;
	uint32_t lcddma_fb0_ceiling;
	uint32_t lcddma_fb1_base;
	uint32_t lcddma_fb1_ceiling;
};

struct omapl138_device {
	conf_object_t* obj;

	general_signal_intf* signal_iface;
	conf_object_t* signal;

	struct registers regs;
	int interrupt_number;
	int scheduler_id;
	int fb_index;

	memory_space_intf* memory_iface;
	conf_object_t* memory;

	conf_object_t *lcd_obj;
	lidd_lcd_intf *lcd_intf;

	RWLOCK_T lock;
	sdl_ctx_t *sdl_ctx;
	int w, h;
	int flag;
	int stop;

	uint16_t palette_hold[256];
};
typedef struct omapl138_device lcd_omapl138_device;

static char* regs_name[] = {
	"revid",
	"lcd_ctrl",
	"lcd_stat",
	"lidd_ctrl",
	"lidd_cs0_conf",
	"lidd_cs0_addr",
	"lidd_cs0_data",
	"lidd_cs1_conf",
	"lidd_cs1_addr",
	"lidd_cs1_data",
	"raster_ctrl",
	"raster_timing_0",
	"raster_timing_1",
	"raster_timing_2",
	"raster_subpanel",
	"lcddma_ctrl",
	"lcddma_fb0_base",
	"lcddma_fb0_ceiling",
	"lcddma_fb1_base",
	"lcddma_fb1_ceiling",
	NULL
};
#define REVID 0x000
#define LCD_CTRL 0x004
#define LCD_STAT 0x008
#define LIDD_CTRL 0x00c
#define LIDD_CS0_CONF 0x010
#define LIDD_CS0_ADDR 0x014
#define LIDD_CS0_DATA 0x018
#define LIDD_CS1_CONF 0x01c
#define LIDD_CS1_ADDR 0x020
#define LIDD_CS1_DATA 0x024
#define RASTER_CTRL 0x028
#define RASTER_TIMING_0 0x02c
#define RASTER_TIMING_1 0x030
#define RASTER_TIMING_2 0x034
#define RASTER_SUBPANEL 0x038
#define LCDDMA_CTRL 0x040
#define LCDDMA_FB0_BASE 0x044
#define LCDDMA_FB0_CEILING 0x048
#define LCDDMA_FB1_BASE 0x04c
#define LCDDMA_FB1_CEILING 0x050
#endif
