#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include "lcd_omapl138.h"
#include "skyeye_ram.h"

#define GET_WIN_W(reg) (((((reg) >> 4) & 0x3f) + 1) * 16)
#define GET_WIN_H(reg) (((reg) & 0x3ff) + 1)

static void lcd_raster_show_frame(void *opaque)
{
	lcd_omapl138_device *dev = ((conf_object_t *)opaque)->obj;

	fb_t *fb = NULL;
	int format, ret = 0;
	SDL_Rect s_rect, d_rect;
	get_page_t *page;
	char *p;
	unsigned char *pixel_data;
	uint32_t fb_base;

	/* FIXME: here is two fb buffer:ping-bang buff, but we only use fb0 */
	/* fb_base = dev->fb_index == 0 ? dev->regs.lcddma_fb0_base : dev->regs.lcddma_fb1_base; */
	fb_base = dev->regs.lcddma_fb0_base;
	if (dev->memory_iface)
		page = dev->memory_iface->get_page(dev->memory, fb_base);

	/* --------------------------------------------------------------------------------- */
	/* raster_ctrl[PLM(20:21)]: 0-palette and data, 1-palette only, 2-data only */
	if (BITS(dev->regs.raster_ctrl, 20, 21) == 0x0) {
		fb = (fb_t *)(page->page_base + (fb_base & 0xfff));
		if (BITS(*((uint16_t *)fb), 12, 14) == 3)/* 8-BPP */ {
			ret = get_format_and_pixels(dev->sdl_ctx,
					fb->fb_512.palette_entry,
					fb->fb_512.pixel_data,
					dev->regs.raster_ctrl, &format);
		} else {
			ret = get_format_and_pixels(dev->sdl_ctx,
					fb->fb_32.palette_entry,
					fb->fb_32.pixel_data,
					dev->regs.raster_ctrl, &format);
		}
	} else if (BITS(dev->regs.raster_ctrl, 20, 21) == 0x1) {
		p = (char *)(page->page_base + (fb_base & 0xfff));
		uint16_t palette_entry_0 = *((uint16_t *)p);

		if (BITS(palette_entry_0, 12, 14) == 3)/* 8-BPP */ {
			memcpy(dev->palette_hold, p, 512);
		} else/* 1, 2, 4, 12, 16-BPP */ {
			memcpy(dev->palette_hold, p, 32);
		}
		SETS(dev->regs.lcd_stat, 6, 6, 0x1);
		if (dev->signal_iface != NULL && BITS(dev->regs.raster_ctrl, 4, 4) == 1)
			dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
		return;
	} else if (BITS(dev->regs.raster_ctrl, 20, 21) == 0x2) {
		pixel_data = (unsigned char *)(page->page_base + (fb_base & 0xfff));

		if (BITS(dev->palette_hold[0], 12, 14) == 3)/* 8-BPP */ {
			ret = get_format_and_pixels(dev->sdl_ctx,
					dev->palette_hold,
					pixel_data,
					dev->regs.raster_ctrl, &format);
		} else/* 1, 2, 4, 12, 16-BPP */ {
			ret = get_format_and_pixels(dev->sdl_ctx,
					dev->palette_hold,
					pixel_data,
					dev->regs.raster_ctrl, &format);
		}
	}

	if (ret == -1) {
		/* set lcd_stat[SYNC(2)] */
		SETS(dev->regs.lcd_stat, 2, 2, 0x1);
		/* raster_ctrl[SL_EN(5)] */
		if (dev->signal_iface != NULL && BITS(dev->regs.raster_ctrl, 5, 5) == 1)
			dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);
		return;
	}

	/* set lcd_stat[DONE(0)] */
	SETS(dev->regs.lcd_stat, 0, 0, 0x1);

	/* FIXME: here is two fb buffer:ping-bang buff, but we only use fb0 */
#if 0
	if (dev->fb_index == 0) {
		SETS(dev->regs.lcd_stat, 8, 8, 0x1);
	} else {
		SETS(dev->regs.lcd_stat, 9, 9, 0x1);
	}
#endif
	SETS(dev->regs.lcd_stat, 8, 8, 0x1);
	/* raster_ctrl[DONE_EN(3)] */
	if (dev->signal_iface != NULL && BITS(dev->regs.lcddma_ctrl, 2, 2) == 1)
		dev->signal_iface->raise_signal(dev->signal, dev->interrupt_number);

	/* --------------------------------------------------------------------------------- */

	if (format != dev->sdl_ctx->format || GET_WIN_W(dev->regs.raster_timing_0) != dev->sdl_ctx->w
			|| GET_WIN_H(dev->regs.raster_timing_1) != dev->sdl_ctx->h) {
		dev->sdl_ctx->format = format;
		dev->sdl_ctx->w = GET_WIN_W(dev->regs.raster_timing_0);
		dev->sdl_ctx->h = GET_WIN_H(dev->regs.raster_timing_1);

		SDL_DestroyTexture(dev->sdl_ctx->texture);
		dev->sdl_ctx->texture = SDL_CreateTexture(dev->sdl_ctx->renderer,
				dev->sdl_ctx->format,
				SDL_TEXTUREACCESS_STREAMING,
				dev->sdl_ctx->w, dev->sdl_ctx->h);
	}

	if (SDL_GetWindowFlags(dev->sdl_ctx->window) == SDL_WINDOW_HIDDEN)
		SDL_ShowWindow(dev->sdl_ctx->window);

	SDL_PollEvent(&dev->sdl_ctx->event);
	if (dev->sdl_ctx->event.type == SDL_QUIT) {
		sdl_free(dev->sdl_ctx);
		dev->stop = 1;
		return;
	}

	s_rect.x = 0;
	s_rect.y = 0;
	s_rect.w = dev->sdl_ctx->w;
	s_rect.h = dev->sdl_ctx->h;

	d_rect.x = W_SIZE / 2 - dev->sdl_ctx->w / 2;
	d_rect.y = H_SIZE / 2 - dev->sdl_ctx->h / 2;
	d_rect.w = dev->sdl_ctx->w;
	d_rect.h = dev->sdl_ctx->h;

	SDL_UpdateTexture(dev->sdl_ctx->texture, NULL, dev->sdl_ctx->pixels, dev->sdl_ctx->w * 16 / 8);
	SDL_RenderClear(dev->sdl_ctx->renderer);
	SDL_RenderCopy(dev->sdl_ctx->renderer, dev->sdl_ctx->texture, &s_rect, &d_rect);
	SDL_RenderPresent(dev->sdl_ctx->renderer);
}
static void lcd_show_frame(void *opaque)
{
	lcd_omapl138_device *dev = ((conf_object_t *)opaque)->obj;

	if (dev->stop) return;
	/* sdl render frame and init must be in the same thread */
	if (dev->flag == 0 && dev->w && dev->h) {
		dev->sdl_ctx = sdl_create();
		///it must be set w = 480 h = 272, put sdl_init when write raster_timing_0///
		sdl_init(dev->sdl_ctx, GET_WIN_W(dev->regs.raster_timing_0),
				GET_WIN_H(dev->regs.raster_timing_1));
		dev->flag = 1;
	}
	/* lcd_ctrl[0]: 0-LIDD mode, lidd_ctrl[8]: LIDD DMA_EN */
	if (BITS(dev->regs.lcd_ctrl, 0, 0) == 0 && BITS(dev->regs.lidd_ctrl, 8, 8) == 1) {
		/* FIXME: LIDD DMA mode */
		return;
	}
	if (BITS(dev->regs.lcd_ctrl, 0, 0) == 0) {
		/* FIXME: LIDD mode */
		return;
	}

	/* lcd_ctrl[0]: 1-LIDD mode, raster_ctrl[0]: RASTER_EN */
	if (BITS(dev->regs.lcd_ctrl, 0, 0) == 1 && BITS(dev->regs.raster_ctrl, 0, 0) == 1) {
		lcd_raster_show_frame(opaque);
		return;
	}
}

static exception_t lcd_omapl138_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	lcd_omapl138_device *dev = opaque->obj;
	uint32_t *val = (uint32_t *)buf;
	switch (offset) {
		case REVID:
			*val = dev->regs.revid;
			break;
		case LCD_CTRL:
			*val = dev->regs.lcd_ctrl;
			break;
		case LCD_STAT:
			*val = dev->regs.lcd_stat;
			break;
		case LIDD_CTRL:
			*val = dev->regs.lidd_ctrl;
			break;
		case LIDD_CS0_CONF:
			*val = dev->regs.lidd_cs0_conf;
			break;
		case LIDD_CS0_ADDR:
			*val = dev->regs.lidd_cs0_addr;
			break;
		case LIDD_CS0_DATA:
			*val = dev->regs.lidd_cs0_data;
			break;
		case LIDD_CS1_CONF:
			*val = dev->regs.lidd_cs1_conf;
			break;
		case LIDD_CS1_ADDR:
			*val = dev->regs.lidd_cs1_addr;
			break;
		case LIDD_CS1_DATA:
			*val = dev->regs.lidd_cs1_data;
			break;
		case RASTER_CTRL:
			*val = dev->regs.raster_ctrl;
			break;
		case RASTER_TIMING_0:
			*val = dev->regs.raster_timing_0;
			break;
		case RASTER_TIMING_1:
			*val = dev->regs.raster_timing_1;
			break;
		case RASTER_TIMING_2:
			*val = dev->regs.raster_timing_2;
			break;
		case RASTER_SUBPANEL:
			*val = dev->regs.raster_subpanel;
			break;
		case LCDDMA_CTRL:
			*val = dev->regs.lcddma_ctrl;
			break;
		case LCDDMA_FB0_BASE:
			*val = dev->regs.lcddma_fb0_base;
			break;
		case LCDDMA_FB0_CEILING:
			*val = dev->regs.lcddma_fb0_ceiling;
			break;
		case LCDDMA_FB1_BASE:
			*val = dev->regs.lcddma_fb1_base;
			break;
		case LCDDMA_FB1_CEILING:
			*val = dev->regs.lcddma_fb1_ceiling;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in lcd_omapl138\n", offset);
			break;
	}
	return 0;
}
static exception_t lcd_omapl138_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	lcd_omapl138_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset) {
		case LCD_CTRL:
			dev->regs.lcd_ctrl = val;
			break;
		case LCD_STAT:
			dev->regs.lcd_stat &= ~val;
			////also clear interrupt///
			break;
		case LIDD_CTRL:
			if (BITS(dev->regs.lcd_ctrl, 0, 0) == 0)
				dev->regs.lidd_ctrl = val;
			break;
		case LIDD_CS0_CONF:
			dev->regs.lidd_cs0_conf = val;
			break;
		case LIDD_CS0_ADDR:
			dev->regs.lidd_cs0_addr = val;
			if (BITS(dev->regs.lcd_ctrl, 0, 0) == 0) {
				if (dev->lcd_obj != NULL && dev->lcd_intf != NULL) {
					dev->lcd_intf->set_cs_addr(dev->lcd_obj, dev->regs.lidd_cs0_addr);
				}
			}
			break;
		case LIDD_CS0_DATA:
			dev->regs.lidd_cs0_data = val;
			if (BITS(dev->regs.lcd_ctrl, 0, 0) == 0) {
				if (dev->lcd_obj != NULL && dev->lcd_intf != NULL) {
					dev->lcd_intf->set_cs_data(dev->lcd_obj, dev->regs.lidd_cs0_data);
				}
			}
			break;
		case LIDD_CS1_CONF:
			dev->regs.lidd_cs1_conf = val;
			break;
		case LIDD_CS1_ADDR:
			dev->regs.lidd_cs1_addr = val;
			break;
		case LIDD_CS1_DATA:
			dev->regs.lidd_cs1_data = val;
			break;
		case RASTER_CTRL:
			dev->regs.raster_ctrl = val;
			/* lcd_ctrl[0]: 0-LIDD mode, 1-RASTER mode */
			if (BITS(dev->regs.lcd_ctrl, 0, 0) == 1) {
				/* raster_ctrl[0]: 0-disabel, 1-enable */
				if (BITS(dev->regs.raster_ctrl, 0, 0) == 0) {
					/* clear lcd_stat[FUF(5)] */
					CLRS(dev->regs.lcd_stat, 5, 5);
					/* clear lcd_stat[SYNC(2)] */
					CLRS(dev->regs.lcd_stat, 2, 2);
					/* clear lcd_stat[DONE(0)],[EOF0(8)], [EOF1(9)] */
					CLRS(dev->regs.lcd_stat, 0, 0);
					CLRS(dev->regs.lcd_stat, 8, 8);
					CLRS(dev->regs.lcd_stat, 9, 9);
					if (BITS(dev->regs.raster_ctrl, 20, 21) == 1) {
						/* clear lcd_stat[PL(6)] */
						CLRS(dev->regs.lcd_stat, 6, 6);
					}
				} else if (BITS(dev->regs.raster_ctrl, 0, 0) == 1) {
					/* clear lcd_stat[DONE(0)],[EOF0(8)], [EOF1(9)] */
					CLRS(dev->regs.lcd_stat, 0, 0);
					CLRS(dev->regs.lcd_stat, 8, 8);
					CLRS(dev->regs.lcd_stat, 9, 9);
				}
			}
			break;
		case RASTER_TIMING_0:
			dev->regs.raster_timing_0 = val;
			dev->w = GET_WIN_W(dev->regs.raster_timing_0);
			break;
		case RASTER_TIMING_1:
			dev->regs.raster_timing_1 = val;
			dev->h = GET_WIN_H(dev->regs.raster_timing_1);
			break;
		case RASTER_TIMING_2:
			dev->regs.raster_timing_2 = val;
			break;
		case RASTER_SUBPANEL:
			dev->regs.raster_subpanel = val;
			break;
		case LCDDMA_CTRL:
			dev->regs.lcddma_ctrl = val;
			break;
		case LCDDMA_FB0_BASE:
			dev->regs.lcddma_fb0_base = val;
			break;
		case LCDDMA_FB0_CEILING:
			dev->regs.lcddma_fb0_ceiling = val;
			break;
		case LCDDMA_FB1_BASE:
			dev->regs.lcddma_fb1_base = val;
			break;
		case LCDDMA_FB1_CEILING:
			dev->regs.lcddma_fb1_ceiling = val;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in lcd_omapl138\n", offset);
			break;
	}
	return 0;
}
static char* lcd_omapl138_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t lcd_omapl138_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	lcd_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t lcd_omapl138_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t lcd_omapl138_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	lcd_omapl138_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t lcd_omapl138_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	lcd_omapl138_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);
	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	lcd_omapl138_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	lcd_omapl138_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	lcd_omapl138_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static void uart_omapl138_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of systimer");
}
static exception_t general_memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	lcd_omapl138_device *dev = obj->obj;
	dev->memory = obj2;
	dev->memory_iface = (memory_space_intf *)SKY_get_iface(dev->memory, MEMORY_SPACE_INTF_NAME);

	if (dev->memory_iface == NULL){
		if (dev->memory == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_memory_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	lcd_omapl138_device *dev = obj->obj;
	*obj2 = dev->memory;
	*port = NULL;
	return No_exp;
}
static exception_t lidd_lcd_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	lcd_omapl138_device *dev = obj->obj;
	dev->lcd_obj = obj2;

	dev->lcd_intf = (lidd_lcd_intf *)SKY_get_iface(dev->lcd_obj, LIDD_LCD_INTF_NAME);

	if (dev->lcd_intf == NULL){
		if (dev->lcd_obj == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", LIDD_LCD_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", LIDD_LCD_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}
static exception_t lidd_lcd_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	lcd_omapl138_device *dev = obj->obj;
	*obj2 = dev->lcd_obj;
	*port = NULL;
	return No_exp;
}
static conf_object_t* new_lcd_omapl138(char *obj_name)
{
	lcd_omapl138_device* dev = skyeye_mm_zero(sizeof(lcd_omapl138_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	regs->revid		= 0x4c100100;
	regs->lidd_cs0_conf	= 1 << 2 | 1 << 6 | 1 << 17 | 1 << 21;

	RWLOCK_INIT(dev->lock);

	dev->fb_index = 0;

	create_thread_scheduler(1000, Periodic_sched, lcd_show_frame, (void *)dev->obj, &dev->scheduler_id);

	return dev->obj;
}
static exception_t free_lcd_omapl138(conf_object_t* opaque)
{
	lcd_omapl138_device *dev = opaque->obj;

	RWLOCK_DESTROY(dev->lock);

	if (dev->sdl_ctx)
		sdl_free(dev->sdl_ctx);

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}
void init_lcd_omapl138(void)
{
	static skyeye_class_t class_data = {
		.class_name = "omapl138_lcd",
		.class_desc = "omapl138_lcd",
		.new_instance = new_lcd_omapl138,
		.free_instance = free_lcd_omapl138,
		.module_type = SKYML_ONCHIP(SC_omapl138),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory = {
		.read = lcd_omapl138_read,
		.write = lcd_omapl138_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = lcd_omapl138_get_regvalue_by_id,
		.get_regname_by_id = lcd_omapl138_get_regname_by_id,
		.set_regvalue_by_id = lcd_omapl138_set_regvalue_by_id,
		.get_regnum = lcd_omapl138_get_regnum,
		.get_regid_by_name = lcd_omapl138_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
	};
	static const struct ConnectDescription connects[] = {
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
		(struct ConnectDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.set = general_memory_space_set,
			.get = general_memory_space_get,
		},
		(struct ConnectDescription) {
			.name = LIDD_LCD_INTF_NAME,
			.set = lidd_lcd_space_set,
			.get = lidd_lcd_space_get,
		}
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	uart_omapl138_register_attribute(clss);
}
