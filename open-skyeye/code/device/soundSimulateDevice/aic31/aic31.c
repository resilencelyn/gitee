/* Copyright (C) 
* 2014 - Michael.Kang blackfin.kang@gmail.com
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/
/**
* @file aic31.c
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2014-12-04
*/

#include <stdio.h>
#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h> 
#include <skyeye_exec.h>
#include <skyeye_cell.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_dev_intf.h>
#include <skyeye_dev_bus_intf.h>
#include "skyeye_uart.h"
#include "skyeye_thread.h"
#include "skyeye_sound.h"
#include "aic31.h"

static exception_t aic31_read(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{
	aic31_dev *dev=  (aic31_dev *)opaque->obj;
	struct sound_device *snd_dev = (struct sound_device*)dev->sound;
	
	switch(offset)
	{
		case 0 :
			*(uint8_t*)buf = dev->regs.aic31_reg0;		 
			break;
		case 1 :
			*(uint8_t*)buf = dev->regs.aic31_reg1;		 
			break;
		case 2 :
			*(uint8_t*)buf = dev->regs.aic31_reg2;		 
			break;
		case 3 :
			*(uint8_t*)buf = dev->regs.aic31_reg3;		 
			break;
		case 4 :
			*(uint8_t*)buf = dev->regs.aic31_reg4;		 
			break;
		case 5 :
			*(uint8_t*)buf = dev->regs.aic31_reg5;		 
			break;
		case 6 :
			*(uint8_t*)buf = dev->regs.aic31_reg6;		 
			break;
		case 7 :
			*(uint8_t*)buf = dev->regs.aic31_reg7;		 
			break;
		case 8 :
			*(uint8_t*)buf = dev->regs.aic31_reg8;		 
			break;
		case 9 :
			*(uint8_t*)buf = dev->regs.aic31_reg9;		 
			break;
		case 10 :
			*(uint8_t*)buf = dev->regs.aic31_reg10;		 
			break;
		case 11 :
			*(uint8_t*)buf = dev->regs.aic31_reg11;		 
			break;
		case 12 :
			*(uint8_t*)buf = dev->regs.aic31_reg12;		 
			break;
		case 13 :
			*(uint8_t*)buf = dev->regs.aic31_reg13;		 
			break;
		case 14 :
			*(uint8_t*)buf = dev->regs.aic31_reg14;		 
			break;
		case 15 :
			*(uint8_t*)buf = dev->regs.aic31_reg15;		 
			break;
		case 16 :
			*(uint8_t*)buf = dev->regs.aic31_reg16;		 
			break;
		case 17 :
			*(uint8_t*)buf = dev->regs.aic31_reg17;		 
			break;
		case 18 :
			*(uint8_t*)buf = dev->regs.aic31_reg18;		 
			break;
		case 19 :
			*(uint8_t*)buf = dev->regs.aic31_reg19;		 
			break;
		case 20 :
			*(uint8_t*)buf = dev->regs.aic31_reg20;		 
			break;
		case 21 :
			*(uint8_t*)buf = dev->regs.aic31_reg21;		 
			break;
		case 22 :
			*(uint8_t*)buf = dev->regs.aic31_reg22;		 
			break;
		case 23 :
			*(uint8_t*)buf = dev->regs.aic31_reg23;		 
			break;
		case 24 :
			*(uint8_t*)buf = dev->regs.aic31_reg24;		 
			break;
		case 25 :
			*(uint8_t*)buf = dev->regs.aic31_reg25;		 
			break;
		case 26 :
			*(uint8_t*)buf = dev->regs.aic31_reg26;		 
			break;
		case 27 :
			*(uint8_t*)buf = dev->regs.aic31_reg27;		 
			break;
		case 28 :
			*(uint8_t*)buf = dev->regs.aic31_reg28;		 
			break;
		case 29 :
			*(uint8_t*)buf = dev->regs.aic31_reg29;		 
			break;
		case 30 :
			*(uint8_t*)buf = dev->regs.aic31_reg30;		 
			break;
		case 31 :
			*(uint8_t*)buf = dev->regs.aic31_reg31;		 
			break;
		case 32 :
			*(uint8_t*)buf = dev->regs.aic31_reg32;		 
			break;
		case 33 :
			*(uint8_t*)buf = dev->regs.aic31_reg33;		 
			break;
		case 34 :
			*(uint8_t*)buf = dev->regs.aic31_reg34;		 
			break;
		case 35 :
			*(uint8_t*)buf = dev->regs.aic31_reg35;		 
			break;
		case 36 :
			*(uint8_t*)buf = dev->regs.aic31_reg36;		 
			break;
		case 37 :
			*(uint8_t*)buf = dev->regs.aic31_reg37;		 
			break;
		case 38 :
			*(uint8_t*)buf = dev->regs.aic31_reg38;		 
			break;
		case 39 :
			*(uint8_t*)buf = dev->regs.aic31_reg39;		 
			break;
		case 40 :
			*(uint8_t*)buf = dev->regs.aic31_reg40;		 
			break;
		case 41 :
			*(uint8_t*)buf = dev->regs.aic31_reg41;		 
			break;
		case 42 :
			*(uint8_t*)buf = dev->regs.aic31_reg42;		 
			break;
		case 43 :
			*(uint8_t*)buf = dev->regs.aic31_reg43;		 
			break;
		case 44 :
			*(uint8_t*)buf = dev->regs.aic31_reg44;		 
			break;
		case 45 :
			*(uint8_t*)buf = dev->regs.aic31_reg45;		 
			break;
		case 46 :
			*(uint8_t*)buf = dev->regs.aic31_reg46;		 
			break;
		case 47 :
			*(uint8_t*)buf = dev->regs.aic31_reg47;		 
			break;
		case 48 :
			*(uint8_t*)buf = dev->regs.aic31_reg48;		 
			break;
		case 49 :
			*(uint8_t*)buf = dev->regs.aic31_reg49;		 
			break;
		case 50 :
			*(uint8_t*)buf = dev->regs.aic31_reg50;		 
			break;
		case 51 :
			*(uint8_t*)buf = dev->regs.aic31_reg51;		 
			break;
		case 52 :
			*(uint8_t*)buf = dev->regs.aic31_reg52;		 
			break;
		case 53 :
			*(uint8_t*)buf = dev->regs.aic31_reg53;		 
			break;
		case 54 :
			*(uint8_t*)buf = dev->regs.aic31_reg54;		 
			break;
		case 55 :
			*(uint8_t*)buf = dev->regs.aic31_reg55;		 
			break;
		case 56 :
			*(uint8_t*)buf = dev->regs.aic31_reg56;		 
			break;
		case 57 :
			*(uint8_t*)buf = dev->regs.aic31_reg57;		 
			break;
		case 58 :
			*(uint8_t*)buf = dev->regs.aic31_reg58;		 
			break;
		case 59 :
			*(uint8_t*)buf = dev->regs.aic31_reg59;		 
			break;
		case 60 :
			*(uint8_t*)buf = dev->regs.aic31_reg60;		 
			break;
		case 61 :
			*(uint8_t*)buf = dev->regs.aic31_reg61;		 
			break;
		case 62 :
			*(uint8_t*)buf = dev->regs.aic31_reg62;		 
			break;
		case 63 :
			*(uint8_t*)buf = dev->regs.aic31_reg63;		 
			break;
		case 64 :
			*(uint8_t*)buf = dev->regs.aic31_reg64;		 
			break;
		case 65 :
			*(uint8_t*)buf = dev->regs.aic31_reg65;		 
			break;
		case 66 :
			*(uint8_t*)buf = dev->regs.aic31_reg66;		 
			break;
		case 67 :
			*(uint8_t*)buf = dev->regs.aic31_reg67;		 
			break;
		case 68 :
			*(uint8_t*)buf = dev->regs.aic31_reg68;		 
			break;
		case 69 :
			*(uint8_t*)buf = dev->regs.aic31_reg69;		 
			break;
		case 70 :
			*(uint8_t*)buf = dev->regs.aic31_reg70;		 
			break;
		case 71 :
			*(uint8_t*)buf = dev->regs.aic31_reg71;		 
			break;
		case 72 :
			*(uint8_t*)buf = dev->regs.aic31_reg72;		 
			break;
		case 73 :
			*(uint8_t*)buf = dev->regs.aic31_reg73;		 
			break;
		case 74 :
			*(uint8_t*)buf = dev->regs.aic31_reg74;		 
			break;
		case 75 :
			*(uint8_t*)buf = dev->regs.aic31_reg75;		 
			break;
		case 76 :
			*(uint8_t*)buf = dev->regs.aic31_reg76;		 
			break;
		case 77 :
			*(uint8_t*)buf = dev->regs.aic31_reg77;		 
			break;
		case 78 :
			*(uint8_t*)buf = dev->regs.aic31_reg78;		 
			break;
		case 79 :
			*(uint8_t*)buf = dev->regs.aic31_reg79;		 
			break;
		case 80 :
			*(uint8_t*)buf = dev->regs.aic31_reg80;		 
			break;
		case 81 :
			*(uint8_t*)buf = dev->regs.aic31_reg81;		 
			break;
		case 82 :
			*(uint8_t*)buf = dev->regs.aic31_reg82;		 
			break;
		case 83 :
			*(uint8_t*)buf = dev->regs.aic31_reg83;		 
			break;
		case 84 :
			*(uint8_t*)buf = dev->regs.aic31_reg84;		 
			break;
		case 85 :
			*(uint8_t*)buf = dev->regs.aic31_reg85;		 
			break;
		case 86 :
			*(uint8_t*)buf = dev->regs.aic31_reg86;		 
			break;
		case 87 :
			*(uint8_t*)buf = dev->regs.aic31_reg87;		 
			break;
		case 88 :
			*(uint8_t*)buf = dev->regs.aic31_reg88;		 
			break;
		case 89 :
			*(uint8_t*)buf = dev->regs.aic31_reg89;		 
			break;
		case 90 :
			*(uint8_t*)buf = dev->regs.aic31_reg90;		 
			break;
		case 91 :
			*(uint8_t*)buf = dev->regs.aic31_reg91;		 
			break;
		case 92 :
			*(uint8_t*)buf = dev->regs.aic31_reg92;		 
			break;
		case 93 :
			*(uint8_t*)buf = dev->regs.aic31_reg93;		 
			break;
		case 94 :
			*(uint8_t*)buf = dev->regs.aic31_reg94;		 
			break;
		case 95 :
			*(uint8_t*)buf = dev->regs.aic31_reg95;		 
			break;
		case 96 :
			*(uint8_t*)buf = dev->regs.aic31_reg96;		 
			break;
		case 97 :
			*(uint8_t*)buf = dev->regs.aic31_reg97;		 
			break;
		case 98 :
			*(uint8_t*)buf = dev->regs.aic31_reg98;		 
			break;
		case 99 :
			*(uint8_t*)buf = dev->regs.aic31_reg99;		 
			break;
		case 100 :
			*(uint8_t*)buf = dev->regs.aic31_reg100;		 
			break;
		case 101 :
			*(uint8_t*)buf = dev->regs.aic31_reg101;		 
			break;
		case 102 :
			*(uint8_t*)buf = dev->regs.aic31_reg102;		 
			break;
		default:
			break;
	}       
	//printf("In %s,offset:%x, buf:%x\n", __func__,offset, *(uint8_t *)buf);
	return  No_exp;
}

void set_sample_rate(aic31_dev* dev, int val){
	int adc_val = (val >> 4) & 0xF;
	int dac_val = val & 0xF;
	switch(adc_val){
		case 0:
			dev->adc_fs =  dev->fs / 1; 
			break;
		case 1:
			dev->adc_fs =  dev->fs / 1.5; 
			break;
		case 2:
			dev->adc_fs =  dev->fs / 2; 
			break;
		case 3:
			dev->adc_fs =  dev->fs / 2.5; 
			break;
		case 4:
			dev->adc_fs =  dev->fs / 3; 
			break;
		case 5:
			dev->adc_fs =  dev->fs / 3.5; 
			break;
		case 6:
			dev->adc_fs =  dev->fs / 4; 
			break;
		case 7:
			dev->adc_fs =  dev->fs / 4.5; 
			break;
		case 8:
			dev->adc_fs =  dev->fs / 5; 
			break;
		case 9:
			dev->adc_fs =  dev->fs / 5.5; 
			break;
		case 10:
			dev->adc_fs =  dev->fs / 6; 
			break;
		default:
			break;
	}
	switch(dac_val){
		case 0:
			dev->dac_fs =  dev->fs / 1; 
			break;
		case 1:
			dev->dac_fs =  dev->fs / 1.5; 
			break;
		case 2:
			dev->dac_fs =  dev->fs / 2; 
			break;
		case 3:
			dev->dac_fs =  dev->fs / 2.5; 
			break;
		case 4:
			dev->dac_fs =  dev->fs / 3; 
			break;
		case 5:
			dev->dac_fs =  dev->fs / 3.5; 
			break;
		case 6:
			dev->dac_fs =  dev->fs / 4; 
			break;
		case 7:
			dev->dac_fs =  dev->fs / 4.5; 
			break;
		case 8:
			dev->dac_fs =  dev->fs / 5; 
			break;
		case 9:
			dev->dac_fs =  dev->fs / 5.5; 
			break;
		case 10:
			dev->dac_fs =  dev->fs / 6; 
			break;
		default:
			break;
	}
}

static exception_t aic31_write(conf_object_t* opaque, generic_address_t offset, void *buf,  size_t count)
{

	aic31_dev *dev=  (aic31_dev *)opaque->obj;
	struct sound_device *snd_dev = (struct sound_device*)dev->sound;
	uint8_t val = *(uint8_t*)buf;
//	printf("In %s,offset:%x, val:%x\n", __func__,offset, val);
	switch(offset)
	{
		case 0 :
			dev->regs.aic31_reg0 = val;		 
			break;
		case 1 :
			dev->regs.aic31_reg1 = val;		 
			break;
		case 2 :
			dev->regs.aic31_reg2 = val;		 
			dev->fs_ref = val;
			break;
		case 3 :
			dev->regs.aic31_reg3 = val;		 
			dev->pll_p = val & 0x7;
			break;
		case 4 :
			dev->regs.aic31_reg4 = val;		 
			dev->pll_j = (val & 0xFC) >> 2;
			break;
		case 5 :
			dev->regs.aic31_reg5 = val;		 
			break;
		case 6 :
			dev->regs.aic31_reg6 = val;		 
			dev->pll_d = (val & 0xFC) >> 2;
			break;
		case 7 :
			dev->regs.aic31_reg7 = val;		 
			if ((val >> 7) & 0x1)
				dev->fs = 44100;
			else 
				dev->fs = 48000;
			
			break;
		case 8 :
			dev->regs.aic31_reg8 = val;		 
		//	if (snd_dev->sound_open != NULL) (*(snd_dev->sound_open))(snd_dev);
			break;
		case 9 :
			dev->regs.aic31_reg9 = val;		 
			dev->transfer_mode = (val >> 6) & 0x3; 
			switch (dev->transfer_mode){
				case 0:
					//I2S mode
					snd_dev->channels = 2;
					break;
				case 1:
					//DSP mode
					break;
				case 2:
					//right justified mode
					break;
				case 3:
					//left justified mode
					break;
				default:
					break;
			}
			switch ((val >> 4) & 0x3){
				case 0:
					dev->word_length = 16; 
					break;
				case 1:
					dev->word_length = 20; 
					break;
				case 2:
					dev->word_length = 24; 
					break;
				case 3:
					dev->word_length = 32; 
					break;
				default:
					break;
			}
			snd_dev->bits_per_sample = dev->word_length;
			
			break;
		case 10 :
			dev->regs.aic31_reg10 = val;		 
			dev->data_offset = val;
			break;
		case 11 :
			dev->regs.aic31_reg11 = val;		 
			break;
		case 12 :
			dev->regs.aic31_reg12 = val;		 
			break;
		case 13 :
			dev->regs.aic31_reg13 = val;		 
			break;
		case 14 :
			dev->regs.aic31_reg14 = val;		 
			break;
		case 15 :
			dev->regs.aic31_reg15 = val;		 
			break;
		case 16 :
			dev->regs.aic31_reg16 = val;		 
			break;
		case 17 :
			dev->regs.aic31_reg17 = val;		 
			break;
		case 18 :
			dev->regs.aic31_reg18 = val;		 
			break;
		case 19 :
			dev->regs.aic31_reg19 = val;		 
			break;
		case 20 :
			dev->regs.aic31_reg20 = val;		 
			break;
		case 21 :
			dev->regs.aic31_reg21 = val;		 
			break;
		case 22 :
			dev->regs.aic31_reg22 = val;		 
			break;
		case 23 :
			dev->regs.aic31_reg23 = val;		 
			break;
		case 24 :
			dev->regs.aic31_reg24 = val;		 
			break;
		case 25 :
			dev->regs.aic31_reg25 = val;		 
			break;
		case 26 :
			dev->regs.aic31_reg26 = val;		 
			break;
		case 27 :
			dev->regs.aic31_reg27 = val;		 
			break;
		case 28 :
			dev->regs.aic31_reg28 = val;		 
			break;
		case 29 :
			dev->regs.aic31_reg29 = val;		 
			break;
		case 30 :
			dev->regs.aic31_reg30 = val;		 
			break;
		case 31 :
			dev->regs.aic31_reg31 = val;		 
			break;
		case 32 :
			dev->regs.aic31_reg32 = val;		 
			break;
		case 33 :
			dev->regs.aic31_reg33 = val;		 
			break;
		case 34 :
			dev->regs.aic31_reg34 = val;		 
			break;
		case 35 :
			dev->regs.aic31_reg35 = val;		 
			break;
		case 36 :
			dev->regs.aic31_reg36 = val;		 
			break;
		case 37 :
			dev->regs.aic31_reg37 = val;		 
			break;
		case 38 :
			dev->regs.aic31_reg38 = val;		 
			break;
		case 39 :
			dev->regs.aic31_reg39 = val;		 
			break;
		case 40 :
			dev->regs.aic31_reg40 = val;		 
			break;
		case 41 :
			dev->regs.aic31_reg41 = val;		 
			break;
		case 42 :
			dev->regs.aic31_reg42 = val;		 
			break;
		case 43 :
			dev->regs.aic31_reg43 = val;		 
			break;
		case 44 :
			dev->regs.aic31_reg44 = val;		 
			break;
		case 45 :
			dev->regs.aic31_reg45 = val;		 
			break;
		case 46 :
			dev->regs.aic31_reg46 = val;		 
			break;
		case 47 :
			dev->regs.aic31_reg47 = val;		 
			break;
		case 48 :
			dev->regs.aic31_reg48 = val;		 
			break;
		case 49 :
			dev->regs.aic31_reg49 = val;		 
			break;
		case 50 :
			dev->regs.aic31_reg50 = val;		 
			break;
		case 51 :
			dev->regs.aic31_reg51 = val;		 
			break;
		case 52 :
			dev->regs.aic31_reg52 = val;		 
			break;
		case 53 :
			dev->regs.aic31_reg53 = val;		 
			break;
		case 54 :
			dev->regs.aic31_reg54 = val;		 
			break;
		case 55 :
			dev->regs.aic31_reg55 = val;		 
			break;
		case 56 :
			dev->regs.aic31_reg56 = val;		 
			break;
		case 57 :
			dev->regs.aic31_reg57 = val;		 
			break;
		case 58 :
			dev->regs.aic31_reg58 = val;		 
			break;
		case 59 :
			dev->regs.aic31_reg59 = val;		 
			break;
		case 60 :
			dev->regs.aic31_reg60 = val;		 
			break;
		case 61 :
			dev->regs.aic31_reg61 = val;		 
			break;
		case 62 :
			dev->regs.aic31_reg62 = val;		 
			break;
		case 63 :
			dev->regs.aic31_reg63 = val;		 
			break;
		case 64 :
			dev->regs.aic31_reg64 = val;		 
			break;
		case 65 :
			dev->regs.aic31_reg65 = val;		 
			break;
		case 66 :
			dev->regs.aic31_reg66 = val;		 
			break;
		case 67 :
			dev->regs.aic31_reg67 = val;		 
			break;
		case 68 :
			dev->regs.aic31_reg68 = val;		 
			break;
		case 69 :
			dev->regs.aic31_reg69 = val;		 
			break;
		case 70 :
			dev->regs.aic31_reg70 = val;		 
			break;
		case 71 :
			dev->regs.aic31_reg71 = val;		 
			break;
		case 72 :
			dev->regs.aic31_reg72 = val;		 
			break;
		case 73 :
			dev->regs.aic31_reg73 = val;		 
			break;
		case 74 :
			dev->regs.aic31_reg74 = val;		 
			break;
		case 75 :
			dev->regs.aic31_reg75 = val;		 
			break;
		case 76 :
			dev->regs.aic31_reg76 = val;		 
			break;
		case 77 :
			dev->regs.aic31_reg77 = val;		 
			break;
		case 78 :
			dev->regs.aic31_reg78 = val;		 
			break;
		case 79 :
			dev->regs.aic31_reg79 = val;		 
			break;
		case 80 :
			dev->regs.aic31_reg80 = val;		 
			break;
		case 81 :
			dev->regs.aic31_reg81 = val;		 
			break;
		case 82 :
			dev->regs.aic31_reg82 = val;		 
			break;
		case 83 :
			dev->regs.aic31_reg83 = val;		 
			break;
		case 84 :
			dev->regs.aic31_reg84 = val;		 
			break;
		case 85 :
			dev->regs.aic31_reg85 = val;		 
			break;
		case 86 :
			dev->regs.aic31_reg86 = val;		 
			break;
		case 87 :
			dev->regs.aic31_reg87 = val;		 
			break;
		case 88 :
			dev->regs.aic31_reg88 = val;		 
			break;
		case 89 :
			dev->regs.aic31_reg89 = val;		 
			break;
		case 90 :
			dev->regs.aic31_reg90 = val;		 
			break;
		case 91 :
			dev->regs.aic31_reg91 = val;		 
			break;
		case 92 :
			dev->regs.aic31_reg92 = val;		 
			break;
		case 93 :
			dev->regs.aic31_reg93 = val;		 
			break;
		case 94 :
			dev->regs.aic31_reg94 = val;		 
			break;
		case 95 :
			dev->regs.aic31_reg95 = val;		 
			break;
		case 96 :
			dev->regs.aic31_reg96 = val;		 
			break;
		case 97 :
			dev->regs.aic31_reg97 = val;		 
			break;
		case 98 :
			dev->regs.aic31_reg98 = val;		 
			break;
		case 99 :
			dev->regs.aic31_reg99 = val;		 
			break;
		case 100 :
			dev->regs.aic31_reg100 = val;		 
			break;
		case 101 :
			dev->regs.aic31_reg101 = val;		 
			break;
		case 102 :
			dev->regs.aic31_reg102 = val;		 
			break;
		default:
			break;
	}
	return  No_exp;
}
static int aic31_i2c_set_state(conf_object_t* opaque, i2c_device_state_t state, uint8_t address)
{
	aic31_dev *dev=  (aic31_dev *)opaque->obj;
	return 0;
}

static uint8_t aic31_i2c_read_data(conf_object_t *opaque){
	aic31_dev *dev=  (aic31_dev *)opaque->obj;
	char regDate;
	int regAddr = dev->slave_buf[0];

	aic31_read(opaque, regAddr, &regDate, 1);
	dev->cnt = 0;
	
	return regDate;
}

static uint8_t aic31_i2c_write_data(conf_object_t *opaque, uint8_t value){
	aic31_dev *dev=  (aic31_dev *)opaque->obj;
	int regAddr, regDate;

	dev->slave_buf[dev->cnt] = value;
	dev->cnt ++;

	if (dev->cnt == 2){
		dev->cnt = 0;
		regAddr = dev->slave_buf[0];
		regDate = dev->slave_buf[1];
		aic31_write(opaque, regAddr, &regDate, 1);
	}
		
	return 0;
}

static uint8_t aic31_i2c_get_address(conf_object_t *opaque)
{
	aic31_dev *dev=  (aic31_dev *)opaque->obj;
	return 0;
}
exception_t aic31_write_sound(conf_object_t *opaque, void *buf, size_t length){
	aic31_dev *dev=  (aic31_dev *)opaque->obj;
	struct sound_device *snd_dev = (struct sound_device*)dev->sound;
	
	set_sample_rate(dev, dev->fs_ref);
	snd_dev->samples_per_sec = dev->dac_fs;
	
	if (-1 == (*(snd_dev->sound_write))(snd_dev, buf, length)){
		skyeye_log(Error_log, __func__, "sound_write failed.\n");
		return -1;
	}	
	return No_exp;
}
exception_t aic31_read_sound(conf_object_t *opaque, void *buf, size_t length){
	aic31_dev *dev=  (aic31_dev *)opaque->obj;
	struct sound_device *snd_dev = (struct sound_device*)dev->sound;

	set_sample_rate(dev, dev->fs_ref);
	snd_dev->samples_per_sec = dev->adc_fs;

	if (-1 == (*(snd_dev->sound_read))(snd_dev, buf, length)){
		skyeye_log(Error_log, __func__, "sound_write failed.\n");
		return -1;
	}	
	return No_exp;
}

static conf_object_t* new_aic31(char* obj_name)
{
	aic31_dev* aic31 = skyeye_mm_zero(sizeof(aic31_dev));
	struct sound_device *snd_dev;
	snd_dev = (struct sound_device*)malloc(sizeof(struct sound_device));
	if (snd_dev == NULL) return -1;

	memset(snd_dev, 0, sizeof(struct sound_device));

	aic31->obj = new_conf_object(obj_name, aic31);
	aic31->cnt = 0;
	aic31->slave_buf[0] = 0;
	aic31->slave_buf[1] = 0;
	aic31->send_buf[MAX_BUFFER_SIZE] = 0;
	aic31->fs = 0; 
	aic31->mclk = 24000000; //24mhz

	snd_dev->mod = 0;
	snd_dev->channels = 0;
	snd_dev->bits_per_sample = 0;
	snd_dev->samples_per_sec = 0; 

	snd_dev->sound_open = pcm_sound_open;
	snd_dev->sound_close = pcm_sound_close;
	snd_dev->sound_update = pcm_sound_update;
	snd_dev->sound_read = pcm_sound_read;
	snd_dev->sound_write = pcm_sound_write;
	aic31->sound = snd_dev;

	return aic31->obj;
}

static exception_t reset_aic31(conf_object_t* opaque, const char* args)
{
	aic31_dev *dev = opaque->obj;
	return No_exp;        
}


static exception_t free_aic31(conf_object_t* opaque)
{
	aic31_dev *dev = opaque->obj;
	struct sound_device *snd_dev = (struct sound_device*)dev->sound;
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	//if (snd_dev->sound_close != NULL) (*(snd_dev->sound_close))(snd_dev);

	return No_exp;
}

static exception_t get_attr_i2c_master(conf_object_t *obj, conf_object_t **obj2, const char **port, int index){
	aic31_dev *dev = obj->obj;
	*obj2 = dev->i2c_bus_obj;
	*port = NULL;
	return No_exp;
}
static exception_t set_attr_i2c_master(conf_object_t *obj, conf_object_t *obj2, const char *port, int index){
	aic31_dev *dev = obj->obj;
	dev->i2c_bus_obj = obj2;
	dev->i2c_bus_iface = (i2c_bus_intf *)SKY_get_iface(dev->i2c_bus_obj, I2C_BUS_INTF_NAME);
	if (dev->i2c_bus_iface == NULL){
		if (dev->i2c_bus_obj == NULL)
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", I2C_BUS_INTF_NAME, obj2);
		else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", I2C_BUS_INTF_NAME, obj2->objname);
	}
	return No_exp;
}

void init_aic31(){
	static skyeye_class_t class_data = {
		.class_name = "aic31",
		.class_desc = "aic31",
		.new_instance = new_aic31,
		.reset_instance = reset_aic31,
		.free_instance = free_aic31,
		.get_attr = NULL,
		.set_attr = NULL,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* class = SKY_register_device_class(class_data.class_name, &class_data);
	/* Register io function to the object */
	static const memory_space_intf io_memory = {
		.read = aic31_read,
		.write = aic31_write,
	};
	SKY_register_iface(class, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const sound_intf sound_iface = {
		.read_sound = aic31_read_sound,
		.write_sound = aic31_write_sound,
	};
	SKY_register_iface(class, SOUND_INTF, &sound_iface);

	static const i2c_device_intf i2c_dev_iface = {
		.set_state = aic31_i2c_set_state,
		.write_data = aic31_i2c_write_data,
		.read_data = aic31_i2c_read_data,
		.get_address = aic31_i2c_get_address,
	};
	SKY_register_iface(class, I2C_DEVICE_INTF_NAME, &i2c_dev_iface);


	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = I2C_DEVICE_INTF_NAME,
			.iface = &i2c_dev_iface
		},
		(struct InterfaceDescription)
		{
			.name = SOUND_INTF,
			.iface = &sound_iface
		}
	};
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
		(struct ConnectDescription) {
			.name = I2C_BUS_INTF_NAME,
			.set = set_attr_i2c_master,
			.get = get_attr_i2c_master,
		}
	};
	class_register_ifaces(class, ifaces);
	class_register_connects(class, connects);
}

