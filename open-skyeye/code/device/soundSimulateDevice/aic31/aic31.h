#ifndef __IIC_AIC31_H
#define __IIC_AIC31_H
#include <skyeye_dev_bus_intf.h>
#include "skyeye_sound.h"

#define MAX_BUFFER_SIZE 0x4000

typedef struct AIC31 
{
	conf_object_t *obj;
	conf_object_t* signal_obj;
	struct sound_device *sound;
 
	struct registers
	{
		uint8_t aic31_reg0; /* Page Select  */
		uint8_t aic31_reg1; /* Software Reset */
		uint8_t aic31_reg2;
		uint8_t aic31_reg3;
		uint8_t aic31_reg4;
		uint8_t aic31_reg5;
		uint8_t aic31_reg6;
		uint8_t aic31_reg7;
		uint8_t aic31_reg8;
		uint8_t aic31_reg9;
		uint8_t aic31_reg10;
		uint8_t aic31_reg11;
		uint8_t aic31_reg12;
		uint8_t aic31_reg13;
		uint8_t aic31_reg14;
		uint8_t aic31_reg15;
		uint8_t aic31_reg16;
		uint8_t aic31_reg17;
		uint8_t aic31_reg18;
		uint8_t aic31_reg19;
		uint8_t aic31_reg20;
		uint8_t aic31_reg21;
		uint8_t aic31_reg22;
		uint8_t aic31_reg23;
		uint8_t aic31_reg24;
		uint8_t aic31_reg25;
		uint8_t aic31_reg26;
		uint8_t aic31_reg27;
		uint8_t aic31_reg28;
		uint8_t aic31_reg29;
		uint8_t aic31_reg30;
		uint8_t aic31_reg31;
		uint8_t aic31_reg32;
		uint8_t aic31_reg33;
		uint8_t aic31_reg34;
		uint8_t aic31_reg35;
		uint8_t aic31_reg36;
		uint8_t aic31_reg37;
		uint8_t aic31_reg38;
		uint8_t aic31_reg39;
		uint8_t aic31_reg40;
		uint8_t aic31_reg41;
		uint8_t aic31_reg42;
		uint8_t aic31_reg43;
		uint8_t aic31_reg44;
		uint8_t aic31_reg45;
		uint8_t aic31_reg46;
		uint8_t aic31_reg47;
		uint8_t aic31_reg48;
		uint8_t aic31_reg49;
		uint8_t aic31_reg50;
		uint8_t aic31_reg51;
		uint8_t aic31_reg52;
		uint8_t aic31_reg53;
		uint8_t aic31_reg54;
		uint8_t aic31_reg55;
		uint8_t aic31_reg56;
		uint8_t aic31_reg57;
		uint8_t aic31_reg58;
		uint8_t aic31_reg59;
		uint8_t aic31_reg60;
		uint8_t aic31_reg61;
		uint8_t aic31_reg62;
		uint8_t aic31_reg63;
		uint8_t aic31_reg64;
		uint8_t aic31_reg65;
		uint8_t aic31_reg66;
		uint8_t aic31_reg67;
		uint8_t aic31_reg68;
		uint8_t aic31_reg69;
		uint8_t aic31_reg70;
		uint8_t aic31_reg71;
		uint8_t aic31_reg72;
		uint8_t aic31_reg73;
		uint8_t aic31_reg74;
		uint8_t aic31_reg75;
		uint8_t aic31_reg76;
		uint8_t aic31_reg77;
		uint8_t aic31_reg78;
		uint8_t aic31_reg79;
		uint8_t aic31_reg80;
		uint8_t aic31_reg81;
		uint8_t aic31_reg82;
		uint8_t aic31_reg83;
		uint8_t aic31_reg84;
		uint8_t aic31_reg85;
		uint8_t aic31_reg86;
		uint8_t aic31_reg87;
		uint8_t aic31_reg88;
		uint8_t aic31_reg89;
		uint8_t aic31_reg90;
		uint8_t aic31_reg91;
		uint8_t aic31_reg92;
		uint8_t aic31_reg93;
		uint8_t aic31_reg94;
		uint8_t aic31_reg95;
		uint8_t aic31_reg96;
		uint8_t aic31_reg97;
		uint8_t aic31_reg98;
		uint8_t aic31_reg99;
		uint8_t aic31_reg100;
		uint8_t aic31_reg101;
		uint8_t aic31_reg102;
	}regs;
	conf_object_t* i2c_bus_obj;
	i2c_bus_intf *i2c_bus_iface;
	int transfer_mode;
	int word_length;
	int data_offset;
	int cnt;
	int buffer_cnt;
	int fs_ref;
	int dac_fs;
	int adc_fs;
	int mclk;
	int fs;
	int pll_p;
	int pll_r;
	int pll_d;
	int pll_j;
	char slave_buf[2];
	char send_buf[MAX_BUFFER_SIZE];
}aic31_dev;

#define LEFT_ADC_POWER_UP ((dev->regs.aic31_reg19 >> 2) & 0x1)
#define RIGHT_ADC_POWER_UP ((dev->regs.aic31_reg22 >> 2) & 0x1)

#define LEFT_DAC_POWER_UP ((dev->regs.aic31_reg37 >> 7) & 0x1)
#define RIGHT_DAC_POWER_UP ((dev->regs.aic31_reg37 >> 6) & 0x1)

#endif
