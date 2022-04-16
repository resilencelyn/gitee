#ifndef __UC1608_SDL_H__
#define __UC1608_SDL_H__
#include <stdint.h>
#include "SDL.h"

#define MAX_W 1024
#define MAX_H 1024

typedef struct _ctx_t {
	SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
        SDL_Event event;
	uint32_t showEventType;
	int w, h;

	uint8_t b_r, b_g, b_b, b_a;
	uint8_t r, g, b, a;
} ctx_t;
ctx_t *init_sdl_ctx(int w, int h);
void set_interval(ctx_t *ctx, int x_interval, int y_interval);
void set_newline_flag(ctx_t *ctx, int newline);
void set_background(ctx_t *ctx, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void set_frontground(ctx_t *ctx, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void sdl_free_ctx(ctx_t *ctx);
#endif
