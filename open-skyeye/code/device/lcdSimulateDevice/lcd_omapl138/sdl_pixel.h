#ifndef __SDL_H__
#define __SDL_H__

#include <stdint.h>
#include "SDL.h"

#define W_SIZE 720
#define H_SIZE 406

struct frame_buffer_32 {
	uint16_t palette_entry[16];
	unsigned char pixel_data[0];
};

struct frame_buffer_512 {
	uint16_t palette_entry[256];
	unsigned char pixel_data[0];
};

typedef struct frame_buffer {
	union {
		struct frame_buffer_32 fb_32;
		struct frame_buffer_512 fb_512;
	};
} fb_t;

typedef struct sdl_context {
        SDL_Renderer *renderer;
        SDL_Texture *texture;
	SDL_Window *window;
        SDL_Event event;
	int format, w, h;
	unsigned char *pixels;
} sdl_ctx_t;

sdl_ctx_t *sdl_create(void);
int sdl_init(sdl_ctx_t *ctx, int w, int h);
int get_format_and_pixels(sdl_ctx_t *ctx, uint16_t palette_entry[], unsigned char *pixel_data, uint32_t raster_ctrl, int *format);
void sdl_free(sdl_ctx_t *ctx);
#endif
