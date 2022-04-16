#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <pthread.h>
#define DEBUG
#include <skyeye_log.h>
#include "uc1608_lcd.h"

//#define SDL_DBG

#ifndef SDL_DBG
# define CMD_DBG(x...)
#else
# define CMD_DBG(x...) printf(x)
#endif

static int uc1608_lcd_set_cs_data(conf_object_t *obj, uint32_t cs_data)
{
	uc1608_lcd_device *dev = obj->obj;
	SDL_Event showEvent;
	ctx_t *ctx;
	uint8_t val = cs_data & 0xff;
	int i;
	struct pixel_data *pixels;

	if (dev->display_enable == 0)
		return -1;

	if (dev->ctx == NULL) {
		return -1;
	}

	ctx = dev->ctx;
	SDL_memset(&showEvent, 0, sizeof(showEvent));

	sem_wait(&dev->sem);

	pixels = malloc(sizeof(struct pixel_data) * 8);
	for (i = 0; i < 8; i++) {
		pixels[i].x = dev->column_addr;
		pixels[i].y = dev->page_addr * 8 + i;

		if (val & (1 << i)) {
			pixels[i].r = ctx->r;
			pixels[i].g = ctx->g;
			pixels[i].b = ctx->b;
			pixels[i].a = ctx->a;
		} else {
			pixels[i].r = ctx->b_r;
			pixels[i].g = ctx->b_g;
			pixels[i].b = ctx->b_b;
			pixels[i].a = ctx->b_a;
		}

	}

	showEvent.type = dev->ctx->showEventType;
	showEvent.user.data1 = pixels;
	SDL_PushEvent(&showEvent);

	if (BITS(dev->ram_addr_ctl, 0, 0) == 0) {
		if (dev->column_addr < 239)
			dev->column_addr++;
		if (BITS(dev->ram_addr_ctl, 2, 2) == 0) {
			if (dev->page_addr < 15)
				dev->page_addr++;
		} else if (BITS(dev->ram_addr_ctl, 2, 2) == 1) {
			if (dev->page_addr > 0)
				dev->page_addr--;
		}
	} else if (BITS(dev->ram_addr_ctl, 0, 0) == 1) {
		if (dev->column_addr < 239)
			dev->column_addr++;
		else {
			dev->column_addr = 0;

			if (BITS(dev->ram_addr_ctl, 2, 2) == 0) {
				dev->page_addr++;
				dev->page_addr %= 16;
			} else if (BITS(dev->ram_addr_ctl, 2, 2) == 1) {
				dev->page_addr = dev->page_addr == 0 ? 15 : dev->page_addr - 1;
			}
		}
	}

	return No_exp;
}
static int uc1608_lcd_set_cs_addr(conf_object_t *obj, uint32_t cs_addr)
{
	uc1608_lcd_device *dev = obj->obj;

	/* COLUMN LSB */
	if (BITS(cs_addr, 4, 7) == 0b0000) {
		dev->column_addr &= ~0xfu;
		dev->column_addr |= BITS(cs_addr, 0, 3);
		return;
	}
	/* COLUMN MSB */
	if (BITS(cs_addr, 4, 7) == 0b0001) {
		dev->column_addr &= ~(0xfu << 4);
		dev->column_addr |= BITS(cs_addr, 0, 3) << 4;
		CMD_DBG("COLUMN: %d\n", dev->column_addr);
		return;
	}
	/* MUX rate and TEMPERATURE */
	if (BITS(cs_addr, 3, 7) == 0b00100) {
		dev->mux = BITS(cs_addr, 2, 2) == 0 ? 96 : 128;
		CMD_DBG("MUX: %d\n", dev->mux);
		return;
	}
	/* POWER CTL */
	if (BITS(cs_addr, 3, 7) == 0b00101) {
		return;
	}
	/* START LINE */
	if (BITS(cs_addr, 6, 7) == 0b01) {
		dev->start_line = BITS(cs_addr, 0, 5);
		CMD_DBG("START LINE:%d\n", dev->start_line);
		return;
	}
	/* RAM ADDRESS CTL */
	if (BITS(cs_addr, 3, 7) == 0b10001) {
		dev->ram_addr_ctl = BITS(cs_addr, 0, 2);
		CMD_DBG("RAM ADDRESS CONTROL:%d\n", dev->ram_addr_ctl);
		return;
	}
	/* ALL PIX ON */
	if (BITS(cs_addr, 1, 7) == 0b1010010) {
		dev->pixel_on = BITS(cs_addr, 0, 0);
		CMD_DBG("ALL PIX ON\n");
		return;
	}
	/* INVERSE DISPLAY */
	if (BITS(cs_addr, 1, 7) == 0b1010011) {
		dev->inverse_display = BITS(cs_addr, 0, 0);
		CMD_DBG("INVERSE DISPLAY\n");
		return;
	}
	/* DISPLAY ENABLE */
	if (BITS(cs_addr, 1, 7) == 0b1010111) {
		dev->display_enable = 1;
		CMD_DBG("DISPLAY ENABLE\n");
		return;
	}
	/* FIX LINES */
	if (BITS(cs_addr, 4, 7) == 0b1001) {
		dev->fixed_lines = BITS(cs_addr, 0, 3);
		CMD_DBG("FIX LINE: %d\n", dev->fixed_lines);
		return;
	}
	/* PAGE ADDRESS */
	if (BITS(cs_addr, 4, 7) == 0b1011) {
		dev->page_addr = BITS(cs_addr, 0, 3);
		CMD_DBG("PAGE ADDRESS: %d\n", dev->page_addr);
		return;
	}
	/* MAPPING CTL */
	if (BITS(cs_addr, 4, 7) == 0b1100) {
		dev->msf = BITS(cs_addr, 0, 0);
		dev->mx = BITS(cs_addr, 2, 2);
		dev->my = BITS(cs_addr, 3, 3);
		CMD_DBG("MSF: %d\tMX: %d\tMY: %d\n", dev->msf, dev->mx, dev->my);
		return;
	}
	/* SYSTEM RESET */
	if (BITS(cs_addr, 0, 7) == 0b11100010) {
		CMD_DBG("SYSTEM RESET\n");
		return;
	}
	/* BIAS RATIO */
	if (BITS(cs_addr, 2, 7) == 0b111010) {
		return;
	}
	/* RESET CURSOR */
	if (BITS(cs_addr, 0, 7) == 0b1110110) {
		return;
	}
	/* SET CURSOR */
	if (BITS(cs_addr, 0, 7) == 0b11101111) {
		return;
	}
	CMD_DBG("==UNKNOW CMD==: 0x%x\n", cs_addr);
	return No_exp;
}
static void *start_routine(void *arg)
{
	uc1608_lcd_device *dev = (uc1608_lcd_device *)arg;
	ctx_t *ctx;
	struct pixel_data *pixels;
	int i;

	while (1) {
		if (dev->ctx == NULL) {
			if ((dev->ctx = init_sdl_ctx(240, 128)) == NULL) {
				return NULL;
			}
		}

		ctx = dev->ctx;

		SDL_WaitEvent(&ctx->event);

		if (ctx->event.type == SDL_QUIT)
			break;
		if (ctx->event.type == ctx->showEventType) {
			pixels = (struct pixel_data *)ctx->event.user.data1;
			for (i = 0; i < 8; i++) {
				SDL_SetRenderTarget(ctx->renderer, ctx->texture);
				SDL_SetRenderDrawColor(ctx->renderer, ctx->b_r, ctx->b_g, ctx->b_b, ctx->b_a);
				SDL_SetRenderDrawColor(ctx->renderer, pixels[i].r, pixels[i].g, pixels[i].b, pixels[i].a);
				SDL_RenderDrawPoint(ctx->renderer, pixels[i].x, pixels[i].y);
				SDL_SetRenderTarget(ctx->renderer, NULL);
				SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, NULL);
				SDL_RenderPresent(ctx->renderer);
			}

			free(pixels);

			sem_post(&dev->sem);
		}
	}

	sdl_free_ctx(dev->ctx);
	dev->ctx == NULL;

	return NULL;
}
static conf_object_t* new_uc1608_lcd(char *obj_name)
{
	uc1608_lcd_device* dev = skyeye_mm_zero(sizeof(uc1608_lcd_device));
	dev->obj = new_conf_object(obj_name, dev);

	dev->ram_addr_ctl = 0b001;
	dev->mux = 128;

	sem_init(&dev->sem, 0, 128 * 240 / (4 * 8));
	pthread_create(&dev->tid, NULL, start_routine, dev);
	return dev->obj;
}
static exception_t free_uc1608_lcd(conf_object_t* opaque)
{
	uc1608_lcd_device *dev = opaque->obj;
	SDL_Event quitEvent;

	quitEvent.type = SDL_QUIT;
	SDL_PushEvent(&quitEvent);

	pthread_join(dev->tid, NULL);

	if (dev->ctx)
		sdl_free_ctx(dev->ctx);

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}
void init_uc1608_lcd(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "uc1608_lcd",
		.class_desc = "uc1608_lcd",
		.new_instance = new_uc1608_lcd,
		.free_instance = free_uc1608_lcd,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static lidd_lcd_intf uc1608_lcd_intf = {
		.set_cs_data = uc1608_lcd_set_cs_data,
		.set_cs_addr = uc1608_lcd_set_cs_addr
	};
	SKY_register_iface(clss, LIDD_LCD_INTF_NAME, &uc1608_lcd_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription)
		{
			.name = LIDD_LCD_INTF_NAME,
			.iface = &uc1608_lcd_intf
		}
	};
	static const struct ConnectDescription connects[] = {
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
