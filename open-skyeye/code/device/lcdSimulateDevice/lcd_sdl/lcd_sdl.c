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
#include <skyeye_lcd_intf.h>
#include <skyeye_log.h>
#include "SDL.h"
#include "lcd_sdl.h"


#define DEBUG



//显示线程
int SDLCALL video_process(void * data)
{
	return 0;
	lcd_device* dev = ((conf_object_t *)data)->obj;
	int ret; 
	while (dev->lcd_run)
	{
		/*
		//使用像素值更新纹理
		ret = SDL_UpdateTexture(dev->p_texture, NULL, dev->surface.lcd_addr_begin, dev->image_width * 4);
		//清理渲染器
		ret = SDL_RenderClear(dev->p_render);
		//从纹理复制到渲染器
		SDL_RenderCopy(dev->p_render, dev->p_texture, NULL, NULL);
		//更新渲染器显示
		SDL_RenderPresent(dev->p_render);
		*/

		/*
		SDL_Event event;
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			SDL_MinimizeWindow(dev->p_screen);
			break;
		}

		SDL_Delay(100);
		*/
	}
	dev->lcd_stoped = 1;

	return 0;
}

static void *start_routine(void *arg)
{

	lcd_device* dev = (lcd_device*)arg;
	int ret;
	while (dev->lcd_run)
	{
		//使用像素值更新纹理
		ret = SDL_UpdateTexture(dev->p_texture, NULL, dev->surface.lcd_addr_begin, dev->image_width * 4);
		//清理渲染器
		ret = SDL_RenderClear(dev->p_render);
		//从纹理复制到渲染器
		SDL_RenderCopy(dev->p_render, dev->p_texture, NULL, NULL);
		//更新渲染器显示
		SDL_RenderPresent(dev->p_render);

		SDL_Event event;
		/*
		SDL_WaitEvent(&event);
		switch (event.type)
		{
			case SDL_QUIT:
				SDL_MinimizeWindow(dev->p_screen);
				break;
		}
		*/
		 while( SDL_PollEvent(&event)){
        
        		switch (event.type){
            			case SDL_QUIT:
                			printf("Stop !\n");
                			break;
        		}
		}
		 usleep(100);
	}
	dev->lcd_stoped = 1;
}

void stop_lcd_process(conf_object_t *obj)
{
	lcd_device *dev = obj->obj;
	dev->lcd_run = 0;
	while(dev->lcd_stoped == 0){
		usleep(1);
	}
}

void lcd_free(conf_object_t *obj)
{
	lcd_device *dev = obj->obj;

	stop_lcd_process(obj);
	SDL_DestroyTexture(dev->p_texture);
	SDL_DestroyRenderer(dev->p_render);
	SDL_DestroyWindow(dev->p_screen);
	SDL_Quit();
}

void lcd_init(conf_object_t *obj)
{
	lcd_device* dev = obj->obj;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL Timer SubSystem: %s\n", SDL_GetError());
		SDL_Quit();
	}



	//创建窗口
	dev->p_screen = SDL_CreateWindow(dev->lcd_title,SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		dev->screen_width, dev->screen_height, SDL_WINDOW_MOUSE_FOCUS);
	if (NULL == dev->p_screen)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s\n", SDL_GetError());
		SDL_Quit();
	}
	//创建渲染器
	dev->p_render = SDL_CreateRenderer(dev->p_screen, -1, 0);
	if (NULL == dev->p_render)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s\n", SDL_GetError());
		SDL_Quit();
	}
	//创建纹理
	dev->p_texture = SDL_CreateTexture(dev->p_render, dev->pixformat, SDL_TEXTUREACCESS_STREAMING, dev->image_width, dev->image_height);
	if (NULL == dev->p_texture)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError());
		SDL_Quit();
	}

	//启动线程数据更新
	//SDL_Thread *video_thread = SDL_CreateThread(video_process, NULL, obj);

	uint32_t tid;
	pthread_create(&tid, NULL, start_routine, dev);
}


static int32_t gtk_lcd_open(conf_object_t *obj, lcd_surface_t* surface)
{
	lcd_device *dev = obj->obj;
	memmove(&(dev->surface), surface, sizeof(lcd_surface_t));
	lcd_surface_t *p_surface = &(dev->surface);
	dev->screen_width = p_surface->width;
	dev->screen_height = p_surface->height;
	dev->image_width = p_surface->width;
	dev->image_height = p_surface->height;
	dev->p_framebuffer = p_surface->lcd_addr_begin;
	dev->pixformat = SDL_PIXELFORMAT_RGBA8888;//像素格式
	snprintf(dev->lcd_title, LCD_TITLE_LEN_LIMIT, "SKYEYE LCD TERM  [%s -> %s]", surface->ctrl_name, obj->objname);
	lcd_init(dev->obj);

	return 0;
}
static conf_object_t* new_lcd(char *obj_name)
{
	lcd_device* dev = skyeye_mm_zero(sizeof(lcd_device));
	dev->obj = new_conf_object(obj_name, dev);
	dev->p_texture = NULL;
	dev->p_render = NULL;
	dev->lcd_run = 1;
	dev->lcd_stoped = 0;

	return dev->obj;
}
static exception_t free_lcd(conf_object_t* opaque)
{
	lcd_free(opaque);
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_lcd(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "lcd",
		.class_desc = "lcd",
		.new_instance = new_lcd,
		.free_instance = free_lcd,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const lcd_control_intf lcd_ctrl = {
		.lcd_open = gtk_lcd_open,
		.lcd_close = NULL,
		.lcd_update = NULL,
		.lcd_filter_read = NULL,
		.lcd_filter_write = NULL,
		.lcd_lookup_color = NULL,
	};
	SKY_register_iface(clss, LCD_CTRL_INTF_NAME, &lcd_ctrl);


	static const struct InterfaceDescription ifaces[] = 
	{
		(struct InterfaceDescription) {
			.name = LCD_CTRL_INTF_NAME,
			.iface = &lcd_ctrl,
		},

	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
