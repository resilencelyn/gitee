#include <stdio.h>
#include "uc1608_sdl.h"

void set_background(ctx_t *ctx, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	ctx->b_r = r;
	ctx->b_g = g;
	ctx->b_b = b;
	ctx->b_a = a;
}
void set_frontground(ctx_t *ctx, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	ctx->r = r;
	ctx->g = g;
	ctx->b = b;
	ctx->a = a;
}
ctx_t *init_sdl_ctx(int w, int h)
{
	ctx_t *ctx;

	if (w <= 0 || h <= 0 || w > MAX_W || h > MAX_H) {
		printf("window(w, h) not illegal!\n");
		return NULL;
	}

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
                return NULL;
        }

	if ((ctx = malloc(sizeof(ctx_t))) == NULL) {
		perror("malloc");
		return NULL;
	}

	ctx->window = SDL_CreateWindow("Dot matrix lcd",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			w, h,
			0);
	ctx->renderer = SDL_CreateRenderer(ctx->window, -1, 0);
	ctx->texture = SDL_CreateTexture(ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	ctx->w = w;
	ctx->h = h;

	/* set background white */
	ctx->b_r = 0x00;
	ctx->b_g = 0x00;
	ctx->b_b = 0x00;
	ctx->b_a = 0x00;

	/* set point color */
	ctx->r = 0xff;
	ctx->g = 0xff;
	ctx->b = 0xff;
	ctx->a = 0;

	ctx->showEventType = SDL_RegisterEvents(1);

	return ctx;
}
void sdl_free_ctx(ctx_t *ctx)
{
	SDL_DestroyTexture(ctx->texture);
	SDL_DestroyRenderer(ctx->renderer);
	SDL_DestroyWindow(ctx->window);
	SDL_Quit();
}
