#ifndef __UC1608_LCD_H__
#define __UC1608_LCD_H__
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include "uc1608_sdl.h"

#define BITS(val, a, b) (((val) >> (a)) & ((1 << (1 + (b) - (a))) - 1))

struct lcd_device
{
	conf_object_t* obj;

	pthread_t tid;
	sem_t sem;
	ctx_t *ctx;

	uint32_t start_line;
	uint32_t ram_addr_ctl;
	uint32_t pixel_on;
	uint32_t inverse_display;
	volatile uint32_t display_enable;
	uint32_t fixed_lines;
	uint32_t msf, mx, my;
	uint32_t mux;

	uint32_t column_addr;
	uint32_t page_addr;
};
struct pixel_data {
	int x, y;
	uint8_t r, g, b, a;
};
typedef struct lcd_device uc1608_lcd_device;
#endif
