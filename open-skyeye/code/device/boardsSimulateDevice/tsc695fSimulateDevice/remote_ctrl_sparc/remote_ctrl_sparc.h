/* Copyright (C) 
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file remote_ctrl_sparc.h
* @brief The definition of system controller for sparc
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __SYSTATE_sparc_H__
#define __SYSTATE_sparc_H__

typedef struct reg_ycyk_status{
	uint8_t power_status :1;
	uint8_t channel :1;
	uint8_t channel1_status :1;
	uint8_t channel2_status :1;
	uint8_t cache:1;
	uint8_t gps_status:1;
	uint8_t tcu_data_status :1;
	uint8_t tcu_instr_status :1;
	uint8_t page_num:8;
}status_t;

typedef struct remote_ctrl_reg{
	union{
		status_t flag;
		uint32_t value;
	}ycyk_state;
	uint32_t channel_check; //注数通道中断选择设置
	uint32_t dpsk_page;  //DPSK页面寄存器
	uint32_t channel1_data; //遥控通道1数据
	uint32_t channel2_data;  //遥控通道2数据
	uint32_t note_end;  //注数结束
	uint32_t TCUA_data_store;  //TCU A遥测数据存放
	uint32_t TCUB_data_store;  //TCU B遥测数据存放
	uint32_t start_recv;  //启动遥测接收
	uint32_t low_data;  //间接指令数据存储低8位
	uint32_t high_data;  //间接指令数据存储高16位
	uint32_t instr_send;  //间接指令发送
	uint16_t timer_count;  //频标计数（时钟）
	uint16_t exp_reg;  //频标计数（时钟）
}remote_ctrl_reg_t; 

static char *regs_name[] = {
	"ycyk_state",
	"channel_check",
	"dpsk_page",
	"channel1_data",
	"channel2_data",
	"note_end",
	"TCUA_data_store",
	"TCUB_data_store",
	"start_recv",
	"low_data",
	"hight_data",
	"instr_send",
	"timer_count",
	 NULL
};

static uint32_t regs_offset[] = {
	0x000,
	0x004,
	0x040,
	0x080,
	0x084,
	0x08c,
	0x0c0,
	0x0c4,
	0x0c8,
	0x0d0,
	0x0d4,
	0x0d8,
	0x140,
};
typedef struct sparc_remote_ctrl_device{
	conf_object_t* obj;
	remote_ctrl_reg_t* regs;
	char **regs_name;
	conf_object_t *signal;
	general_signal_intf *signal_iface;
	uint32_t dpsk_cache;  //DPSK延遥缓存区
	int int_number;
	int timer_sched_id;
	uint16_t *channel_fifo;
	int length;
	int page0_cnt;
	int page1_cnt;
	int page_flag;
	int off_cnt;
	char *ptr;
}sparc_remote_ctrl_device;

#endif
