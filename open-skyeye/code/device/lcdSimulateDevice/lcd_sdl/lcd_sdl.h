#ifndef __LCD__H__ 
#define __LCD__H__ 


#define LCD_TITLE_LEN_LIMIT 256
struct _device{ 
	
	conf_object_t* obj;
	lcd_surface_t surface;   
	uint32_t  screen_width;  //screen width
	uint32_t  screen_height; //screen height
	uint32_t  image_width;   //image width
	uint32_t  image_height;  //image heigth
	SDL_Texture *p_texture;  //texture pointer
	SDL_Renderer *p_render;  //render pointer
	SDL_Window *p_screen;    //SDL window pointer
	uint8_t p_framebuffer;   //framebuffer pointer
	uint32_t pixformat;
	char lcd_title[LCD_TITLE_LEN_LIMIT];
	int32_t lcd_run;
	int32_t lcd_stoped;
	
};
typedef struct _device lcd_device;
#endif  
