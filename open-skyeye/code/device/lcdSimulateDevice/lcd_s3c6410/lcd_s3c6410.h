#ifndef __LCD_S3C6410_H__
#define __LCD_S3C6410_H__
struct s3c6410_fb_device;
typedef struct fb_state {
	struct s3c6410_fb_device* dev;
	//    DisplayState*  ds;
	int      pixel_format;
	int      bytes_per_pixel;
	uint32_t fb_base;
	uint32_t base_valid : 1;
	uint32_t need_update : 1;
	uint32_t need_int : 1;
	uint32_t set_rotation : 2;
	uint32_t blank : 1;
	uint32_t int_status;
	uint32_t int_enable;
	int      rotation;   /* 0, 1, 2 or 3 */
	int      dpi;
}fb_state_t;
typedef struct fb_reg{
	uint32 vidcon[3];
	uint32 vidtcon[3];
	uint32 wincon[5];
	uint32 vidosd[5][3];
	uint32 vidosd1d;
	uint32 vidosd2d;
	uint32 vidw00add0b0;
	uint32 vidw00add0b1;
	uint32 vidw01add0b0;
	uint32 vidw01add0b1;
	uint32 vidw02add0;
	uint32 vidw03add0;
	uint32 vidw04add0;
	uint32 vidw00add1b0;
	uint32 vidw00add1b1;
	uint32 vidw01add2;
	uint32 vidw02add2;
	uint32 vidw03add2;
	uint32 vidw03add1;
	uint32 vid02add1;
	uint32 vidw01add1b0;
	uint32 vidw01add1b1;
	uint32 vidw_buf_size[5];
	uint32 vidintcon0;
	uint32 vidintcon1;
	uint32 wkeycon[8];
	uint32 winmap[5];
	uint32 wpalcon;
	uint32 tgigcon;
	uint32 ituifcon0;
	uint32 sifccon[3];
	uint32 ldi_cmd[12];
	uint32 w2pdata[8];
	uint32 w3pdata[8];
	uint32 w4pdata[2];
	uint32 dithmode;
}fb_reg_t;
char* regs_name[] = {
	"vidcon[0]",
	"vidcon[1]",
	"vidcon[2]",
	"uint32 vidtcon[0]",
	"uint32 vidtcon[1]",
	"uint32 vidtcon[2]",
	"uint32 wincon[0]",
	"uint32 wincon[1]",
	"uint32 wincon[2]",
	"uint32 wincon[3]",
	"uint32 wincon[4]",
	"vidosd[0][0]"
	"vidosd[0][1]"
	"vidosd[0][2]"
	"vidosd[1][0]"
	"vidosd[1][1]"
	"vidosd[1][2]"
	"vidosd[2][0]"
	"vidosd[2][1]"
	"vidosd[2][2]"
	"vidosd[3][0]"
	"vidosd[3][1]"
	"vidosd[3][2]"
	"vidosd[4][0]"
	"vidosd[4][1]"
	"vidosd[4][2]"
	"vidosd1d",
	"vidosd2d",
	"vidw00add0b0",
	"vidw00add0b1",
	"vidw01add0b0",
	"vidw01add0b1",
	"vidw02add0",
	"vidw03add0",
	"vidw04add0",
	"vidw00add1b0",
	"vidw00add1b1",
	"vidw01add2",
	"vidw02add2",
	"vidw03add2",
	"vidw03add1",
	"vid02add1",
	"vidw01add1b0",
	"vidw01add1b1",
	"vidw_buf_size[0]",
	"vidw_buf_size[1]",
	"vidw_buf_size[2]",
	"vidw_buf_size[3]",
	"vidw_buf_size[4]",
	"vidintcon0",
	"vidintcon1",
	"wkeycon[0]",
	"wkeycon[1]",
	"wkeycon[2]",
	"wkeycon[3]",
	"wkeycon[4]",
	"wkeycon[5]",
	"wkeycon[6]",
	"wkeycon[7]",
	"winmap[0]",
	"winmap[1]",
	"winmap[2]",
	"winmap[3]",
	"winmap[4]",
	"wpalcon",
	"tgigcon",
	"ituifcon0",
	"sifccon[0]",
	"sifccon[1]",
	"sifccon[2]",
	"ldi_cmd[0]",
	"ldi_cmd[1]",
	"ldi_cmd[2]",
	"ldi_cmd[3]",
	"ldi_cmd[4]",
	"ldi_cmd[5]",
	"ldi_cmd[6]",
	"ldi_cmd[7]",
	"ldi_cmd[8]",
	"ldi_cmd[9]",
	"ldi_cmd[10]",
	"ldi_cmd[11]",
	"w2pdata[0]",
	"w2pdata[1]",
	"w2pdata[2]",
	"w2pdata[3]",
	"w2pdata[4]",
	"w2pdata[5]",
	"w2pdata[6]",
	"w2pdata[7]",
	"w3pdata[0]",
	"w3pdata[1]",
	"w3pdata[2]",
	"w3pdata[3]",
	"w3pdata[4]",
	"w3pdata[5]",
	"w3pdata[6]",
	"w3pdata[7]",
	"w4pdata[0]",
	"w4pdata[1]",
	"dithmode",
	NULL
};
typedef struct s3c6410_fb_device{
	conf_object_t* obj;
	fb_reg_t* regs;
	lcd_surface_t* surface;
	int line_no;
	fb_state_t* state;
	conf_object_t* signal_target;
	general_signal_intf* master;
	attr_value_t* lcd_ctrl;
	char **regs_name;
}s3c6410_fb_device;
#endif
