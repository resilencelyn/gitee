#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sdl_pixel.h"

#define BITS(val, a, b) (((val) >> (a)) & ((1 << (1 + (b) - (a))) - 1))

/* 16-Entry Palette/Buffer Format(1, 2, 4, 8 BPP) TFT or STN mode
 * Bit 15  14  13  12  11  10  9  8  7  6  5  4  3  2  1  0
 * ----------------------------------------------------------
 * |Unused|    BPP    |    Red(R)  |  Green(G)  |  Blue(B)  |
 * ----------------------------------------------------------
 * Monochrome mode
 * ----------------------------------------------------------
 * |Unused|    Unsed	  		        |  Mono(M)  |
 * ----------------------------------------------------------
 * 4 MSB unused, we convert 1, 2, 4, 8, 12-BPP to ARGB4444 format
 * with alpha bits is 1111, so it will display same as RGB444.
 * FIXME: In STN color mode, we should gray-scaler/serializer
 * 	  In TFT color mode, we should not gray-scaler/serializer.
 * 	  We implement Monochrome mode use GRAY2ARGB to convert
 */
static int convert_bpp_1_color(uint16_t palette_entry[], unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	char *pos;
	unsigned char *begin;

	pix_data_size = ctx->w * ctx->h / 8;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size * 8 * 2);
	pos = ctx->pixels;

	begin = pixel_data;

	for (i = 0; i < pix_data_size; i++) {
		memcpy(pos, &palette_entry[BITS(begin[i], 7, 7)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 6, 6)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 5, 5)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 4, 4)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 3, 3)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 2, 2)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 1, 1)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 0, 0)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
	}

	return 0;
}
#define GRAY2ARGB(grayscale) 0xf << 12 | (grayscale) << 8 | (grayscale) << 4 | (grayscale)
static int convert_bpp_1_mono(uint16_t palette_entry[], unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	char *pos;
	unsigned char *begin;
	uint16_t argb;

	pix_data_size = ctx->w * ctx->h / 8;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size * 8 * 2);
	pos = ctx->pixels;

	begin = pixel_data;

	for (i = 0; i < pix_data_size; i++) {
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 7, 7)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 6, 6)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 5, 5)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 4, 4)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 3, 3)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 2, 2)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 1, 1)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 0, 0)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
	}

	return 0;
}
static int convert_bpp_2_color(uint16_t palette_entry[], unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	char *pos;
	unsigned char *begin;

	pix_data_size = ctx->w * ctx->h / 4;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size * 4 * 2);
	pos = ctx->pixels;

	begin = pixel_data;

	for (i = 0; i < pix_data_size; i++) {
		memcpy(pos, &palette_entry[BITS(begin[i], 6, 7)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 4, 5)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 2, 3)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 0, 1)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
	}

	return 0;
}
static int convert_bpp_2_mono(uint16_t palette_entry[], unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	char *pos;
	unsigned char *begin;
	uint16_t argb;

	pix_data_size = ctx->w * ctx->h / 4;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size * 4 * 2);
	pos = ctx->pixels;

	begin = pixel_data;

	for (i = 0; i < pix_data_size; i++) {
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 6, 7)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 4, 5)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 2, 3)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 0, 1)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
	}

	return 0;
}
static int convert_bpp_4_color(uint16_t palette_entry[], unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	char *pos;
	unsigned char *begin;

	pix_data_size = ctx->w * ctx->h / 2;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size * 2 * 2);
	pos = ctx->pixels;

	begin = pixel_data;

	for (i = 0; i < pix_data_size; i++) {
		memcpy(pos, &palette_entry[BITS(begin[i], 4, 7)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
		memcpy(pos, &palette_entry[BITS(begin[i], 0, 3)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
	}

	return 0;
}
static int convert_bpp_4_mono(uint16_t palette_entry[], unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	char *pos;
	unsigned char *begin;
	uint16_t argb;

	pix_data_size = ctx->w * ctx->h / 2;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size * 2 * 2);
	pos = ctx->pixels;

	begin = pixel_data;

	for (i = 0; i < pix_data_size; i++) {
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 4, 7)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 0, 3)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
	}

	return 0;
}
static int convert_bpp_8_color(uint16_t palette_entry[], unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	char *pos;
	unsigned char *begin;

	pix_data_size = ctx->w * ctx->h;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size * 2);
	pos = ctx->pixels;

	begin = pixel_data;

	for (i = 0; i < pix_data_size; i++) {
		memcpy(pos, &palette_entry[BITS(begin[i], 0, 7)], 2);
		pos[1] |= 0xf << 4;
		pos += 2;
	}

	return 0;
}
static int convert_bpp_8_mono(uint16_t palette_entry[], unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	char *pos;
	unsigned char *begin;
	uint16_t argb;

	pix_data_size = ctx->w * ctx->h;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size * 2);
	pos = ctx->pixels;

	begin = pixel_data;

	for (i = 0; i < pix_data_size; i++) {
		argb = GRAY2ARGB(palette_entry[BITS(begin[i], 0, 7)] & 0xf);
		memcpy(pos, &argb, 2);
		pos += 2;
	}

	return 0;
}
static int convert_bpp_12_color(unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size, i;
	unsigned char *pos;
	unsigned char *ppixel_data;

	pix_data_size = ctx->w * ctx->h * 2;

	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size);
	pos = ctx->pixels;

	ppixel_data = pixel_data;

	/* 12-BPP format: 4 MSB unused, we convert 12-BPP to ARGB4444 format
	 * with alpha bits is 1111, so it will display same as RGB444.
	 * --------------------------------------------------------------
	 * |  Unused	|	R	|	G	|	B	|
	 * --------------------------------------------------------------
	 */
	for (i = 0; i < pix_data_size; i++) {
		pos[i] = i % 2 == 0 ? ppixel_data[i] : ppixel_data[i] | 0xf << 4;
	}

	return 0;
}
static int convert_bpp_16_color(unsigned char *pixel_data, sdl_ctx_t *ctx)
{
	int pix_data_size;

	pix_data_size = ctx->w * ctx->h * 2;
	ctx->pixels = (char *)realloc(ctx->pixels, pix_data_size);

	memcpy(ctx->pixels, pixel_data, pix_data_size);

	return 0;
}
/* ------------------------------------------------------------------------------ */
#define BPP_1		1
#define BPP_2		2
#define BPP_4		4
#define BPP_8		8
#define BPP_12		12
#define BPP_16		16

#define _TFT_STN	7
#define _STN_565	24

/* raster_ctrl[MONO_COLOR(1)]: 1-Monochrome mode and */
/* raster_ctrl[TFT_STN(7)]: 0-passive(STN), 1-active(TFT) */
int get_format_and_pixels(sdl_ctx_t *ctx, uint16_t palette_entry[],
		unsigned char *pixel_data, uint32_t raster_ctrl, int *format)
{
	switch ((palette_entry[0] >> 12) & 0x7) {
		case 0:
			*format = SDL_PIXELFORMAT_ARGB4444;
			if (BITS(raster_ctrl, 1, 1) == 1 && BITS(raster_ctrl, 7, 7) == 0)
				convert_bpp_1_mono(palette_entry, pixel_data, ctx);
			else
				convert_bpp_1_color(palette_entry, pixel_data, ctx);
			break;
		case 1:
			*format = SDL_PIXELFORMAT_ARGB4444;
			if (BITS(raster_ctrl, 1, 1) == 1 && BITS(raster_ctrl, 7, 7) == 0)
				convert_bpp_2_mono(palette_entry, pixel_data, ctx);
			else
				convert_bpp_2_color(palette_entry, pixel_data, ctx);
			break;
		case 2:
			*format = SDL_PIXELFORMAT_ARGB4444;
			if (BITS(raster_ctrl, 1, 1) == 1 && BITS(raster_ctrl, 7, 7) == 0)
				convert_bpp_4_mono(palette_entry, pixel_data, ctx);
			else
				convert_bpp_4_color(palette_entry, pixel_data, ctx);
			break;
		case 3:
			*format = SDL_PIXELFORMAT_ARGB4444;
			if (BITS(raster_ctrl, 1, 1) == 1 && BITS(raster_ctrl, 7, 7) == 0)
				convert_bpp_8_mono(palette_entry, pixel_data, ctx);
			else
				convert_bpp_8_color(palette_entry, pixel_data, ctx);
			break;
		case 4 ... 7:
			if (BITS(raster_ctrl, _TFT_STN, _TFT_STN) == 0
					&& BITS(raster_ctrl, _STN_565, _STN_565) == 0) {
				*format = SDL_PIXELFORMAT_ARGB4444;
				convert_bpp_12_color(pixel_data, ctx);
				break;
			}
			if (BITS(raster_ctrl, _TFT_STN, _TFT_STN) == 0
					&& BITS(raster_ctrl, _STN_565, _STN_565) == 1) {
				*format = SDL_PIXELFORMAT_UNKNOWN;
				convert_bpp_16_color(pixel_data, ctx);
				break;
			}
			if (BITS(raster_ctrl, _TFT_STN, _TFT_STN) == 1) {
				*format = SDL_PIXELFORMAT_RGB565;
				convert_bpp_16_color(pixel_data, ctx);
				break;
			}
			break;
		default:
			return -1;
	}
	return 0;
}

int sdl_init(sdl_ctx_t *ctx, int w, int h)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

        ctx->window = SDL_CreateWindow("omapl138 lcd",
                        SDL_WINDOWPOS_UNDEFINED,
                        SDL_WINDOWPOS_UNDEFINED,
                        W_SIZE, H_SIZE,
			SDL_WINDOW_HIDDEN);
	if (!ctx->window) {
		printf("SDL: could not create window - exiting: %s\n", SDL_GetError());
		return -1;
	}

        ctx->renderer = SDL_CreateRenderer(ctx->window, -1, 0);
	ctx->texture = SDL_CreateTexture(ctx->renderer, ctx->format, SDL_TEXTUREACCESS_STREAMING, w, h);

	ctx->w = w;
	ctx->h = h;

	ctx->pixels = NULL;

	return 0;
}

sdl_ctx_t *sdl_create(void)
{
	sdl_ctx_t *sdl_ctx;

	if ((sdl_ctx = malloc(sizeof(sdl_ctx_t))) == NULL) {
		perror("malloc");
		return NULL;
	}

	sdl_ctx->format = SDL_PIXELFORMAT_RGB565;

	return sdl_ctx;
}
void sdl_free(sdl_ctx_t *ctx)
{
	free(ctx->pixels);
	SDL_DestroyTexture(ctx->texture);
	SDL_DestroyRenderer(ctx->renderer);
	SDL_DestroyWindow(ctx->window);
	SDL_Quit();
}
