/* Copyright (C) 
 * 
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
 * @file rs422_uart.c
 * @brief The implementation of system controller
 * @author 
 * @version 78.77
 */

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
#include <skyeye_data_intf.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_uart.h>
#include "skyeye_system.h"
#include <sim_control.h>

#include "rs422_uart.h"

static void count_clock_function(conf_object_t *opaque)
{
    rs422_uart_device *dev = opaque->obj;
    dev->uart_time.time += 20;
}

/*计算星敏同步信号个数*/
static void sync_signal_callback(conf_object_t *opaque)
{
    rs422_uart_device *dev = opaque->obj;
    if (dev->sync_enable[0])
        dev->sync_cnt[0] ++;

    if (dev->sync_enable[1])
        dev->sync_cnt[1] ++;

    if (dev->sync_enable[2])
        dev->sync_cnt[2] ++;

    if (dev->sync_enable[3])
        dev->sync_cnt[3] ++;
}

static int pulse_linker_recv_pulse(conf_object_t *opaque, pulse_msg_t* msg, int io_num, int io_id)
{
    rs422_uart_device *dev = opaque->obj;
    return 0;
}

/*发送高电平脉冲信号*/
static void send_high_pulse(conf_object_t *opaque, int ucChNo, int cmd)
{
    rs422_uart_device *dev = opaque->obj;
    pulse_msg_t msg;
    int aocs_status;

    if(dev->pulse_linker)
    {
        msg.ucCmd = cmd;
        msg.ucMode = 1;
        msg.ucChNo = ucChNo;//st1a正脉冲计数
        msg.fPeriod = 1.0;//每1s发送一次脉冲
        msg.fWidth = 0.001;//脉宽为1ms

        if (dev->aocs_iface)
        {
            aocs_status = dev->aocs_iface->get_aocs_status(dev->aocs);
            if (aocs_status)
                dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_PULSE_NUM, 1);
        } else
        {
            dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_PULSE_NUM, 1);
        }
    }
}

/*发送低电平脉冲信号*/
static void send_low_pulse(conf_object_t *opaque, int ucChNo, int cmd){
    rs422_uart_device *dev = opaque->obj;
    pulse_msg_t msg;
    int aocs_status;

    if(dev->pulse_linker)
    {
        msg.ucCmd = cmd;
        msg.ucMode = 0;
        msg.ucChNo = ucChNo;//st1a负脉冲计数
        msg.fPeriod = 1.0;//每1s发送一次脉冲
        msg.fWidth = 0.001;//脉宽为1ms

        if (dev->aocs_iface)
        {
            aocs_status = dev->aocs_iface->get_aocs_status(dev->aocs); 
            if (aocs_status)
                dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_PULSE_NUM, 1);
        } else
        {
            dev->pulse_linker_iface->send_pulse(dev->pulse_linker, opaque, &msg, IO_PULSE_NUM, 1);
        }
    }
}

static int rs422_linker_recv_data(conf_object_t *opaque, void* buf, int length, int port_id)
{
    rs422_uart_device *dev = opaque->obj;
    rs422_uart_reg_t* regs = dev->regs;
    uint32_t i, sum = 0;
    if (port_id > 21)
    {
        printf("In %s, receive data from uart port_id = %d beyond the max id\n", __func__, port_id);
        return -1;
    }

    if (port_id <= 16)
    {
        memcpy(dev->uart_fifo[0].recv_buf, buf, length);
        regs->general_uart_recv_length = length;
    } else if (port_id == BK_19200)
    {
        memcpy(dev->uart_fifo[1].recv_buf, buf, length);
        regs->bk_uart_recv_length = length;
    } else if (port_id == ST1A)
    {
        memcpy(dev->uart_fifo[2].recv_buf, buf, length);
        regs->st1_recv_length = length;
    } else if (port_id == ST1B)
    {
        memcpy(dev->uart_fifo[3].recv_buf, buf, length);
        regs->st2_recv_length = length;
    } else if (port_id == ST2)
    {
        memcpy(dev->uart_fifo[4].recv_buf, buf, length);
        regs->st3_recv_length = length;
    } else if (port_id == ST3)
    {
        memcpy(dev->uart_fifo[5].recv_buf, buf, length);
        regs->st4_recv_length = length;
    } else
    {
        //No this port id
    }

    return 0;
}

static exception_t rs422_uart_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct rs422_uart_device *dev = opaque->obj;
    rs422_uart_reg_t* regs = dev->regs;

    switch(offset)
    {
        case COM_IDX:
            *(uint32_t *)buf = regs->com_idx;
            break;
        case GENERAL_UART_COMMAND:
            *(uint32_t *)buf = regs->general_uart_command;
            break;
        case GENERAL_UART_SEND_STATUS:
            *(uint32_t *)buf = regs->general_uart_send_status;
            break;
        case GENERAL_UART_RECV_LENGTH:
            *(uint32_t *)buf = regs->general_uart_recv_length;
            break;
        case GENERAL_UART_RECV_STATUS:
            *(uint32_t *)buf = regs->general_uart_recv_status;
            break;
        case BK_UART_COMMAND:
            *(uint32_t *)buf = regs->bk_uart_command;
            break;
        case BK_UART_SEND_STATUS:
            *(uint32_t *)buf = regs->bk_uart_send_status;
            break;
        case BK_UART_RECV_LENGTH:
            *(uint32_t *)buf = regs->bk_uart_recv_length;
            break;
        case BK_UART_RECV_STATUS:
            *(uint32_t *)buf = regs->bk_uart_recv_status;
            break;
        case ST1_COMMAND_REG:
            *(uint32_t *)buf = regs->st1_command_reg;
            break;
        case ST1_SEND_STATUS:
            *(uint32_t *)buf = regs->st1_send_status;
            break;
        case ST1_RECV_LENGTH:
            *(uint32_t *)buf = regs->st1_recv_length;
            break;
        case ST2_COMMAND_REG:
            *(uint32_t *)buf = regs->st2_command_reg;
            break;
        case ST2_SEND_STATUS:
            *(uint32_t *)buf = regs->st2_send_status;
            break;
        case ST2_RECV_LENGTH:
            *(uint32_t *)buf = regs->st2_recv_length;
            break;
        case ST3_COMMAND_REG:
            *(uint32_t *)buf = regs->st3_command_reg;
            break;
        case ST3_SEND_STATUS:
            *(uint32_t *)buf = regs->st3_send_status;
            break;
        case ST3_RECV_LENGTH:
            *(uint32_t *)buf = regs->st3_recv_length;
            break;
        case ST4_COMMAND_REG:
            *(uint32_t *)buf = regs->st4_command_reg;
            break;
        case ST4_SEND_STATUS:
            *(uint32_t *)buf = regs->st4_send_status;
            break;
        case ST4_RECV_LENGTH:
            *(uint32_t *)buf = regs->st4_recv_length;
            break;
        case ST1_SYNC_SIGNAL:
            *(uint32_t *)buf = regs->st1_sync_signal;
            break;
        case ST2_SYNC_SIGNAL:
            *(uint32_t *)buf = regs->st2_sync_signal;
            break;
        case ST3_SYNC_SIGNAL:
            *(uint32_t *)buf = regs->st3_sync_signal;
            break;
        case ST4_SYNC_SIGNAL:
            *(uint32_t *)buf = regs->st4_sync_signal;
            break;
        case ST1_RTS_SIGNAL:
            *(uint32_t *)buf = regs->st1_rts_signal;
            break;
        case ST2_RTS_SIGNAL:
            *(uint32_t *)buf = regs->st2_rts_signal;
            break;
        case ST3_RTS_SIGNAL:
            *(uint32_t *)buf = regs->st3_rts_signal;
            break;
        case ST4_RTS_SIGNAL:
            *(uint32_t *)buf = regs->st4_rts_signal;
            break;
        case ST1_RTS_COUNT_LOW:
            *(uint16_t *)buf = regs->st1_rts_signal_count.low;
            break;
        case ST1_RTS_COUNT_HIGH:
            *(uint16_t *)buf = regs->st1_rts_signal_count.high;
            break;
        case ST1_RTS_TIME_LSB_LOW:
            *(uint16_t *)buf = regs->st1_rts_signal_time.lsb_low;
            break;
        case ST1_RTS_TIME_LSB_HIGH:
            *(uint16_t *)buf = regs->st1_rts_signal_time.lsb_high;
            break;
        case ST1_RTS_TIME_MSB_LOW:
            *(uint16_t *)buf = regs->st1_rts_signal_time.msb_low;
            break;
        case ST2_RTS_COUNT_LOW:
            *(uint16_t *)buf = regs->st2_rts_signal_count.low;
            break;
        case ST2_RTS_COUNT_HIGH:
            *(uint16_t *)buf = regs->st2_rts_signal_count.high;
            break;
        case ST2_RTS_TIME_LSB_LOW:
            *(uint16_t *)buf = regs->st2_rts_signal_time.lsb_low;
            break;
        case ST2_RTS_TIME_LSB_HIGH:
            *(uint16_t *)buf = regs->st2_rts_signal_time.lsb_high;
            break;
        case ST2_RTS_TIME_MSB_LOW:
            *(uint16_t *)buf = regs->st2_rts_signal_time.msb_low;
            break;
        case ST3_RTS_COUNT_LOW:
            *(uint16_t *)buf = regs->st3_rts_signal_count.low;
            break;
        case ST3_RTS_COUNT_HIGH:
            *(uint16_t *)buf = regs->st3_rts_signal_count.high;
            break;
        case ST3_RTS_TIME_LSB_LOW:
            *(uint32_t *)buf = regs->st3_rts_signal_time.lsb_low;
            break;
        case ST3_RTS_TIME_LSB_HIGH:
            *(uint32_t *)buf = regs->st3_rts_signal_time.lsb_high;
            break;
        case ST3_RTS_TIME_MSB_LOW:
            *(uint32_t *)buf = regs->st3_rts_signal_time.msb_low;
            break;
        case ST4_RTS_COUNT_LOW:
            *(uint16_t *)buf = regs->st4_rts_signal_count.low;
            break;
        case ST4_RTS_COUNT_HIGH:
            *(uint16_t *)buf = regs->st4_rts_signal_count.high;
            break;
        case ST4_RTS_TIME_LSB_LOW:
            *(uint16_t *)buf = regs->st4_rts_signal_time.lsb_low;
            break;
        case ST4_RTS_TIME_LSB_HIGH:
            *(uint16_t *)buf = regs->st4_rts_signal_time.lsb_high;
            break;
        case ST4_RTS_TIME_MSB_LOW:
            *(uint16_t *)buf = regs->st4_rts_signal_time.msb_low;
            break;
        case ST1_SIGNAL_COUNT_LOW:
            *(uint16_t *)buf = regs->st1_signal_count.low;
            break;
        case ST1_SIGNAL_COUNT_HIGH:
            *(uint16_t *)buf = regs->st1_signal_count.high;
            break;
        case ST2_SIGNAL_COUNT_LOW:
            *(uint16_t *)buf = regs->st2_signal_count.low;
            break;
        case ST3_SIGNAL_COUNT_LOW:
            *(uint16_t *)buf = regs->st3_signal_count.low;
            break;
        case ST3_SIGNAL_COUNT_HIGH:
            *(uint16_t *)buf = regs->st3_signal_count.high;
            break;
        case ST4_SIGNAL_COUNT_LOW:
            *(uint16_t *)buf = regs->st4_signal_count.low;
            break;
        case ST4_SIGNAL_COUNT_HIGH:
            *(uint16_t *)buf = regs->st4_signal_count.high;
            break;
        case UART_TIME1:
            *(uint16_t *)buf = dev->uart_time.time1;
            break;
        case UART_TIME2:
            *(uint16_t *)buf = dev->uart_time.time2;
            break;
        case UART_TIME3:
            *(uint16_t *)buf = dev->uart_time.time3;
            break;
        case BASE_TIME_CMD:
            *(uint32_t *)buf = regs->base_time_cmd_reg;
            break;
        case ADJUST_CLOCK:
            *(uint32_t *)buf = regs->adjust_clock;
            break;
        case GPS_TIME_1:
            *(uint32_t *)buf = regs->gps_fault_time_reg_1;
            break;
        case GPS_TIME_2:
            *(uint32_t *)buf = regs->gps_fault_time_reg_2;
            break;
        case GPS_TIME_3:
            *(uint32_t *)buf = regs->gps_fault_time_reg_3;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in rs422_uart\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}

static exception_t rs422_uart_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    struct rs422_uart_device *dev = opaque->obj;
    rs422_uart_reg_t* regs = dev->regs;
    uint32_t val = *(uint32_t*)buf;
    int com_idx, ucChNo, aocs_status;
    if (dev->aocs_iface)
        aocs_status = dev->aocs_iface->get_aocs_status(dev->aocs);

    switch(offset) {
        case COM_IDX:
            regs->com_idx = val;
            break;
        case GENERAL_UART_COMMAND:
            regs->general_uart_command = val; //公用串口

            if (val == 0){
                dev->uart_fifo[0].send_cnt = 0;
                regs->general_uart_send_status = 0; //发送结束状态位清零	
            }else if (val == 1){
                //启动发送	
                dev->uart_fifo[0].send_length = dev->uart_fifo[0].send_cnt;
                com_idx = regs->com_idx;

                if (dev->aocs_iface){
                    if (dev->rs422_linker_iface != NULL && aocs_status)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[0].send_buf, dev->uart_fifo[0].send_length, com_idx);
                    }
                } else
                {
                    if (dev->rs422_linker_iface != NULL)
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[0].send_buf, dev->uart_fifo[0].send_length, com_idx);
                }

                dev->uart_fifo[0].send_cnt = 0;
            } else if (val == 2)
            {
                regs->general_uart_recv_length = 0;//串口接收复位，复位接收数据计数器，奇偶校验结果
                dev->uart_fifo[0].recv_cnt = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case GENERAL_UART_SEND_STATUS:
            regs->general_uart_send_status = val;
            break;
        case GENERAL_UART_RECV_LENGTH:
            regs->general_uart_recv_length = val;
            break;
        case GENERAL_UART_RECV_STATUS:
            regs->general_uart_recv_status = val;//串口通讯故障  0为正常
            break;
        case BK_UART_COMMAND:
            regs->bk_uart_command = val; //19.2K串口
            if (val == 0){
                dev->uart_fifo[1].send_cnt = 0;
                regs->bk_uart_send_status = 0; //发送结束状态位清零	
            } else if (val == 1)
            {
                //启动发送	
                dev->uart_fifo[1].send_length = dev->uart_fifo[1].send_cnt;

                if (dev->aocs_iface)
                {
                    if (dev->rs422_linker_iface != NULL && aocs_status)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[1].send_buf, dev->uart_fifo[1].send_length, 17);
                    }
                } else
                {
                    if (dev->rs422_linker_iface != NULL)
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[1].send_buf, dev->uart_fifo[1].send_length, 17);
                }

                dev->uart_fifo[1].send_cnt = 0;
            } else if (val == 2)
            {
                regs->bk_uart_recv_length = 0;//串口接收复位，复位接收数据计数器，奇偶校验结果
                dev->uart_fifo[1].recv_cnt = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case BK_UART_SEND_STATUS:
            regs->bk_uart_send_status = val;
            break;
        case BK_UART_RECV_LENGTH:
            regs->bk_uart_recv_length = val;
            break;
        case BK_UART_RECV_STATUS:
            regs->bk_uart_recv_status = val;
            break;
        case ST1_COMMAND_REG:
            regs->st1_command_reg = val; //星敏串口1
            if (val == 0)
            {
                dev->uart_fifo[2].send_cnt = 0;
                regs->st1_send_status = 0; //发送结束状态位清零	
            } else if (val == 1)
            {
                //启动发送
                dev->uart_fifo[2].send_length = dev->uart_fifo[2].send_cnt;
                if (dev->aocs_iface)
                {
                    if (dev->rs422_linker_iface != NULL && aocs_status)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[2].send_buf, dev->uart_fifo[2].send_length, ST1A);
                    }
                } else
                {
                    if (dev->rs422_linker_iface != NULL){
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[2].send_buf, dev->uart_fifo[2].send_length, ST1A);
                    }
                }
            } else if (val == 2)
            {
                regs->st1_recv_length = 0;//串口接收复位，复位接收数据计数器，奇偶校验结果
                dev->uart_fifo[2].recv_cnt = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case ST1_SEND_STATUS:
            regs->st1_send_status = val;
            break;
        case ST1_RECV_LENGTH:
            regs->st1_recv_length = val;
            break;
        case ST2_COMMAND_REG:
            regs->st2_command_reg = val;//星敏串口2
            if (val == 0)
            {
                dev->uart_fifo[3].send_cnt = 0;
                regs->st2_send_status = 0; //发送结束状态位清零	
            } else if (val == 1)
            {
                //启动发送
                dev->uart_fifo[3].send_length = dev->uart_fifo[3].send_cnt;

                if (dev->aocs_iface)
                {
                    if (dev->rs422_linker_iface != NULL && aocs_status)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[3].send_buf, dev->uart_fifo[3].send_length, ST1B);
                    }
                } else
                {
                    if (dev->rs422_linker_iface != NULL)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[3].send_buf, dev->uart_fifo[3].send_length, ST1B);
                    }
                }
            } else if (val == 2)
            {
                regs->st2_recv_length = 0;//串口接收复位，复位接收数据计数器，奇偶校验结果
                dev->uart_fifo[3].recv_cnt = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case ST2_SEND_STATUS:
            regs->st2_send_status = val;
            break;
        case ST2_RECV_LENGTH:
            regs->st2_recv_length = val;
            break;
        case ST3_COMMAND_REG:
            regs->st3_command_reg = val;//星敏串口3
            if (val == 0)
            {
                dev->uart_fifo[4].send_cnt = 0;
                regs->st3_send_status = 0; //发送结束状态位清零
            } else if (val == 1)
            {
                //启动发送	
                dev->uart_fifo[4].send_length = dev->uart_fifo[4].send_cnt;
                if (dev->aocs_iface)
                {
                    if (dev->rs422_linker_iface != NULL && aocs_status)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[4].send_buf, dev->uart_fifo[4].send_length, ST2);
                    }
                } else
                {
                    if (dev->rs422_linker_iface != NULL)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[4].send_buf, dev->uart_fifo[4].send_length, ST2);
                    }
                }

            } else if (val == 2)
            {
                regs->st3_recv_length = 0;//串口接收复位，复位接收数据计数器，奇偶校验结果
                dev->uart_fifo[4].recv_cnt = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case ST3_SEND_STATUS:
            regs->st3_send_status = val;
            break;
        case ST3_RECV_LENGTH:
            regs->st3_recv_length = val;
            break;
        case ST4_COMMAND_REG:
            regs->st4_command_reg = val;
            if (val == 0)
            {
                dev->uart_fifo[5].send_cnt = 0;
                regs->st4_send_status = 0; //发送结束状态位清零	
            } else if (val == 1)
            {
                //启动发送
                dev->uart_fifo[5].send_length = dev->uart_fifo[5].send_cnt;
                if (dev->aocs_iface)
                {
                    if (dev->rs422_linker_iface != NULL && aocs_status)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[5].send_buf, dev->uart_fifo[5].send_length, ST3);
                    }
                } else
                {
                    if (dev->rs422_linker_iface != NULL)
                    {
                        dev->rs422_linker_iface->send_data(dev->rs422_obj, opaque, dev->uart_fifo[5].send_buf, dev->uart_fifo[5].send_length, ST3);
                    }
                }
            } else if (val == 2)
            {
                regs->st4_recv_length = 0;//串口接收复位，复位接收数据计数器，奇偶校验结果
                dev->uart_fifo[5].recv_cnt = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case ST4_SEND_STATUS:
            regs->st4_send_status = val;
            break;
        case ST4_RECV_LENGTH:
            regs->st4_recv_length = val;
            break;
        case ST1_SYNC_SIGNAL:
            regs->st1_sync_signal = val;
            if (val == 1)
            {
                //同步信号开始
                if (dev->sync_enable[0] == 0)
                {
                    ucChNo = 0;
                    send_high_pulse(opaque, ucChNo, 2);
                    ucChNo = 1;
                    send_low_pulse(opaque, ucChNo, 2);
                    dev->sync_enable[0] = 1;
                    if (dev->sync_handle == -1)
                    {
                        dev->sync_handle = system_register_timer_handler(opaque, 1000, (time_sched_t)sync_signal_callback, SCHED_MS | SCHED_NORMAL);
                    }
                }

            } else if (val == 2)
            {
                //同步信号停止
                dev->sync_enable[0] = 0;
                ucChNo = 0;
                send_high_pulse(opaque, ucChNo, 3);
                ucChNo = 1;
                send_low_pulse(opaque, ucChNo, 3);
            } else if (val == 3)
            {
                regs->st1_rts_signal_count.rts_reg = 0; //清零同步信号个数，同步信号自动开始
                dev->sync_cnt[0] = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the ST1 signal register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case ST2_SYNC_SIGNAL:
            regs->st2_sync_signal = val;
            if (val == 1)
            {
                //同步信号开始
                if (dev->sync_enable[1] == 0){
                    ucChNo = 2;
                    send_high_pulse(opaque, ucChNo, 2);
                    ucChNo = 3;
                    send_low_pulse(opaque, ucChNo, 2);
                    dev->sync_enable[1] = 1;
                }
            } else if (val == 2)
            {
                //同步信号停止
                dev->sync_enable[1] = 0;
                ucChNo = 2;
                send_high_pulse(opaque, ucChNo, 3);
                ucChNo = 3;
                send_low_pulse(opaque, ucChNo, 3);
            } else if (val == 3)
            {
                regs->st2_rts_signal_count.rts_reg = 0; //清零同步信号个数，同步信号自动开始
                dev->sync_cnt[1] = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the ST2 signal register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case ST3_SYNC_SIGNAL:
            regs->st3_sync_signal = val;
            if (val == 1)
            {
                //同步信号开始
                if (dev->sync_enable[2] == 0)
                {
                    ucChNo = 4;
                    send_high_pulse(opaque, ucChNo, 2);
                    ucChNo = 5;
                    send_low_pulse(opaque, ucChNo, 2);
                    dev->sync_enable[2] = 1;
                }
            } else if (val == 2)
            {
                //同步信号停止
                dev->sync_enable[2] = 0;
                ucChNo = 4;
                send_high_pulse(opaque, ucChNo, 3);
                ucChNo = 5;
                send_low_pulse(opaque, ucChNo, 3);
            } else if (val == 3)
            {
                regs->st3_rts_signal_count.rts_reg = 0; //清零同步信号个数，同步信号自动开始
                dev->sync_cnt[2] = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the ST3 signal register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case ST4_SYNC_SIGNAL:
            regs->st4_sync_signal = val;
            if (val == 1)
            {
                //同步信号开始
                if (dev->sync_enable[3] == 0)
                {
                    ucChNo = 6;
                    send_high_pulse(opaque, ucChNo, 2);
                    ucChNo = 7;
                    send_low_pulse(opaque, ucChNo, 2);
                    dev->sync_enable[3] = 1;
                }
            } else if (val == 2)
            {
                //同步信号停止
                dev->sync_enable[3] = 0;
                ucChNo = 6;
                send_high_pulse(opaque, ucChNo, 3);
                ucChNo = 7;
                send_low_pulse(opaque, ucChNo, 3);
            } else if (val == 3)
            {
                regs->st4_rts_signal_count.rts_reg = 0; //清零同步信号个数，同步信号自动开始
                dev->sync_cnt[3] = 0;
            } else
            {
                skyeye_log(Error_log, __FUNCTION__, "Write the ST4 signal register invalid data %x at %d\n", val, __LINE__);
            }
            break;
        case ST1_RTS_SIGNAL:
            if (val == 1){
                //RTS signal HIGH
            }else if (val == 0){
                //RTS signal LOW
                //在第一个下降沿下把时间和计数器拷贝到对应的锁存寄存器中
                if (regs->st1_rts_signal){
                    regs->st1_rts_signal_time.lsb_low = dev->uart_time.time1;
                    regs->st1_rts_signal_time.lsb_high = dev->uart_time.time2;
                    regs->st1_rts_signal_time.msb_low = dev->uart_time.time3;
                    regs->st1_signal_count.signal_reg = dev->sync_cnt[0];

                }
            }
            regs->st1_rts_signal = val;
            break;
        case ST2_RTS_SIGNAL:
            if (val == 1)
            {
                //RTS signal HIGH
            } else if (val == 0)
            {
                //RTS signal LOW
                //在第一个下降沿下把时间和计数器拷贝到对应的锁存寄存器中
                if (regs->st2_rts_signal)
                {
                    regs->st2_rts_signal_time.lsb_low = dev->uart_time.time1;
                    regs->st2_rts_signal_time.lsb_high = dev->uart_time.time2;
                    regs->st2_rts_signal_time.msb_low = dev->uart_time.time3;
                    regs->st2_signal_count.signal_reg = dev->sync_cnt[1];
                }
            }
            regs->st2_rts_signal = val;
            break;
        case ST3_RTS_SIGNAL:
            if (val == 1)
            {
                //RTS signal HIGH
            }else if (val == 0)
            {
                //RTS signal LOW
                //在第一个下降沿下把时间和计数器拷贝到对应的锁存寄存器中
                if (regs->st3_rts_signal)
                {
                    regs->st3_rts_signal_time.lsb_low = dev->uart_time.time1;
                    regs->st3_rts_signal_time.lsb_high = dev->uart_time.time2;
                    regs->st3_rts_signal_time.msb_low = dev->uart_time.time3;
                    regs->st3_signal_count.signal_reg = dev->sync_cnt[2];
                }
            }
            regs->st3_rts_signal = val;
            break;
        case ST4_RTS_SIGNAL:
            if (val == 1)
            {
                //RTS signal HIGH
            }else if (val == 0)
            {
                //RTS signal LOW
                //在第一个下降沿下把时间和计数器拷贝到对应的锁存寄存器中
                if (regs->st4_rts_signal)
                {
                    regs->st4_rts_signal_time.lsb_low = dev->uart_time.time1;
                    regs->st4_rts_signal_time.lsb_high = dev->uart_time.time2;
                    regs->st4_rts_signal_time.msb_low = dev->uart_time.time3;
                    regs->st4_signal_count.signal_reg = dev->sync_cnt[3];
                }
            }
            regs->st4_rts_signal = val;
            break;
        case ST1_RTS_COUNT_LOW:
            regs->st1_rts_signal_count.low = val & 0xFFFF;
            break;
        case ST1_RTS_COUNT_HIGH:
            regs->st1_rts_signal_count.high = val >> 16;
            break;
        case ST1_RTS_TIME_LSB_LOW:
            regs->st1_rts_signal_time.lsb_low = val;
            break;
        case ST1_RTS_TIME_LSB_HIGH:
            regs->st1_rts_signal_time.lsb_high = val;
            break;
        case ST1_RTS_TIME_MSB_LOW:
            regs->st1_rts_signal_time.msb_low = val;
            break;
        case ST2_RTS_COUNT_LOW:
            regs->st2_rts_signal_count.low = val;
            break;
        case ST2_RTS_COUNT_HIGH:
            regs->st2_rts_signal_count.high = val;
            break;
        case ST2_RTS_TIME_LSB_LOW:
            regs->st2_rts_signal_time.lsb_low = val;
            break;
        case ST2_RTS_TIME_LSB_HIGH:
            regs->st2_rts_signal_time.lsb_high = val;
            regs->st2_signal_count.high = val >> 16;
            break;
        case ST2_RTS_TIME_MSB_LOW:
            regs->st2_rts_signal_time.msb_low = val;
            break;
        case ST3_RTS_COUNT_LOW:
            regs->st3_rts_signal_count.low = val;
            break;
        case ST3_RTS_COUNT_HIGH:
            regs->st3_rts_signal_count.high = val;
            break;
        case ST3_RTS_TIME_LSB_LOW:
            regs->st3_rts_signal_time.lsb_low = val;
            break;
        case ST3_RTS_TIME_LSB_HIGH:
            regs->st3_rts_signal_time.lsb_high = val;
            break;
        case ST3_RTS_TIME_MSB_LOW:
            regs->st3_rts_signal_time.msb_low = val;
            break;
        case ST4_RTS_COUNT_LOW:
            regs->st4_rts_signal_count.low = val;
            break;
        case ST4_RTS_COUNT_HIGH:
            regs->st4_rts_signal_count.high = val;
            break;
        case ST4_RTS_TIME_LSB_LOW:
            regs->st4_rts_signal_time.lsb_low = val;
            break;
        case ST4_RTS_TIME_LSB_HIGH:
            regs->st4_rts_signal_time.lsb_high = val;
            break;
        case ST4_RTS_TIME_MSB_LOW:
            regs->st4_rts_signal_time.msb_low = val;
            break;
        case ST1_SIGNAL_COUNT_LOW:
            regs->st1_signal_count.low = val & 0xFFFF;
            break;
        case ST1_SIGNAL_COUNT_HIGH:
            regs->st1_signal_count.high = val >> 16;
            break;
        case ST2_SIGNAL_COUNT_LOW:
            regs->st2_signal_count.low = val & 0xFFFF;
            break;
        case ST3_SIGNAL_COUNT_LOW:
            regs->st3_signal_count.low = val & 0xFFFF;
            break;
        case ST3_SIGNAL_COUNT_HIGH:
            regs->st3_signal_count.high = val >> 16;
            break;
        case ST4_SIGNAL_COUNT_LOW:
            regs->st4_signal_count.low = val & 0xFFFF;
            break;
        case ST4_SIGNAL_COUNT_HIGH:
            regs->st4_signal_count.high = val >> 16;
            break;
        case UART_TIME1:
            dev->uart_time.time1 = val;
            break;
        case UART_TIME2:
            dev->uart_time.time2 = val;
            break;
        case UART_TIME3:
            dev->uart_time.time3 = val;
            break;
        case BASE_TIME_CMD:
            regs->base_time_cmd_reg = val;
            if (dev->handle == -1)
            {
                dev->handle = system_register_timer_handler(opaque, 2000, (time_sched_t)count_clock_function, SCHED_US | SCHED_NORMAL);
            }
            break;
        case ADJUST_CLOCK:
            regs->adjust_clock = val;
            if (regs->adjust_clock == 0xAABB)
            {
                dev->uart_time.time += 1;
            } else if (regs->adjust_clock == 0x5544)
            {
                dev->uart_time.time -= 1;
            } else
            {
                //do nothing
            }
            break;
        case GPS_TIME_1:
            regs->gps_fault_time_reg_1 = val;
            break;
        case GPS_TIME_2:
            regs->gps_fault_time_reg_2 = val;
            break;
        case GPS_TIME_3:
            regs->gps_fault_time_reg_3 = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not write the register at 0x%x in rs422_uart\n", offset);
            return Invarg_exp;
    }
    return No_exp;
}

static char* rs422_uart_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
    return regs_name[id];
}

static uint32_t rs422_uart_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
    rs422_uart_device *dev = conf_obj->obj;
    rs422_uart_reg_t* regs = dev->regs;
    uint32_t* regs_value = (uint32_t*)regs + id;

    return *regs_value;
}

static exception_t rs422_uart_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
    rs422_uart_device *dev = conf_obj->obj;
    rs422_uart_reg_t* regs = dev->regs;
    *((uint32_t*)regs + id) = value;

    return No_exp;
}

static uint32_t rs422_uart_get_regnum(conf_object_t* conf_obj)
{
    rs422_uart_device *dev = conf_obj->obj;
    rs422_uart_reg_t* regs = dev->regs;

    return  sizeof(rs422_uart_reg_t) / 4;
}

static uint32_t rs422_uart_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
    rs422_uart_device *dev = conf_obj->obj;
    rs422_uart_reg_t* regs = dev->regs;
    uint32_t regnum;
    regnum = sizeof(rs422_uart_reg_t) / 4;
    int i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }

    return regnum;
}

static conf_object_t* new_rs422_uart(const char* obj_name)
{
    rs422_uart_device* dev = skyeye_mm_zero(sizeof(rs422_uart_device));
    rs422_uart_reg_t* regs =  skyeye_mm_zero(sizeof(rs422_uart_reg_t));
    dev->obj = new_conf_object(obj_name, dev);
    /* init rs422_uart regs */
    dev->regs = regs;
    int i;

    for(i = 0; i < 6; i++)
    {
        dev->uart_fifo[i].send_buf = skyeye_mm_zero(FIFO_SIZE);
        dev->uart_fifo[i].recv_buf = skyeye_mm_zero(FIFO_SIZE);
        dev->uart_fifo[i].send_cnt = 0;
        dev->uart_fifo[i].recv_cnt = 0;
        dev->uart_fifo[i].send_length = 0;
    }

    dev->uart_time.time = 0;
    dev->time_cnt = 0;
    for (i = 0; i < 4; i++)
    {
        dev->sync_cnt[i] = 1;
        dev->sync_enable[i] = 0;
    }

    dev->sync_handle = -1;
    dev->handle = -1;
    return dev->obj;
}

static exception_t free_rs422_uart(conf_object_t* conf_obj)
{
    rs422_uart_device *dev = conf_obj->obj;
    int i;
    skyeye_free(dev->regs);
    skyeye_free(dev->obj);

    for(i = 0; i < 6; i++)
    {
        skyeye_free(dev->uart_fifo[i].send_buf);
        skyeye_free(dev->uart_fifo[i].recv_buf);
    }
    skyeye_free(dev);

    return No_exp;
}

static class_register_attribute(conf_class_t* clss)
{
}

static exception_t rs422_uart_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    rs422_uart_device* dev = (rs422_uart_device*)(obj->obj);
    dev->rs422_obj = obj2;
    dev->rs422_linker_iface = (rs422_linker_intf*)SKY_get_iface(dev->rs422_obj, RS422_LINKER_INTF);

    if (dev->rs422_linker_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'rs422_linker_iface' interface from %s fail.", obj2->objname);
    }

    return No_exp;
}

static exception_t rs422_uart_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    rs422_uart_device* dev = (rs422_uart_device*)(obj->obj);
    *obj2 = dev->rs422_obj;
    *port = NULL;

    return No_exp;
}

static exception_t pulse_linker_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    rs422_uart_device* dev = (rs422_uart_device*)(obj->obj);
    dev->pulse_linker = obj2;
    dev->pulse_linker_iface = (pulse_linker_intf*)SKY_get_iface(dev->pulse_linker, PULSE_LINKER_INTF);

    if (dev->pulse_linker_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'pulse_linker_iface' interface from %s fail.", obj2->objname);
    }

    return No_exp;
}

static exception_t pulse_linker_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    rs422_uart_device* dev = (rs422_uart_device*)(obj->obj);
    *obj2 = dev->pulse_linker;
    *port = NULL;

    return No_exp;
}

static exception_t aocs_intf_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    rs422_uart_device* dev = (obj->obj);
    dev->aocs = obj2;
    dev->aocs_iface = (get_aocs_intf*)SKY_get_iface(dev->aocs, GET_AOCS_INTF);

    if (dev->aocs_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'aocs_iface' interface from %s fail.", obj2->objname);
    }

    return No_exp;
}

static exception_t aocs_intf_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    rs422_uart_device* dev = (obj->obj);
    *obj2 = dev->aocs;
    *port = NULL;

    return No_exp;
}

void init_rs422_uart(){
    static skyeye_class_t class_data =
    {
        .class_name = "rs422_uart",
        .class_desc = "rs422_uart",
        .new_instance = new_rs422_uart,
        .free_instance = free_rs422_uart,
        .get_attr = NULL,
        .set_attr = NULL 
    };

    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const memory_space_intf io_memory =
    {
        .read = rs422_uart_read,
        .write = rs422_uart_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = rs422_uart_get_regvalue_by_id,
        .get_regname_by_id = rs422_uart_get_regname_by_id,
        .set_regvalue_by_id = rs422_uart_set_regvalue_by_id,
        .get_regnum = rs422_uart_get_regnum,
        .get_regid_by_name = rs422_uart_get_regid_by_name,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const rs422_dev_intf rs422_dev_interface =
    {
        .receive_data = rs422_linker_recv_data,
    };
    SKY_register_iface(clss, RS422_DEV_INTF, &rs422_dev_interface);

    static const pulse_dev_intf pulse_dev_interface =
    {
        .receive_pulse = pulse_linker_recv_pulse,
    };
    SKY_register_iface(clss, PULSE_DEV_INTF, &pulse_dev_interface);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf,
        },
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        },
        (struct InterfaceDescription)
        {
            .name = PULSE_DEV_INTF,
            .iface = &pulse_dev_interface,
        },
        (struct InterfaceDescription)
        {
            .name = RS422_DEV_INTF,
            .iface = &rs422_dev_interface,
        }
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = RS422_LINKER_INTF,
            .set = rs422_uart_set,
            .get = rs422_uart_get,
        },
        (struct ConnectDescription )
        {
            .name = PULSE_LINKER_INTF,
            .set = pulse_linker_set,
            .get = pulse_linker_get,
        },
        (struct ConnectDescription )
        {
            .name = GET_AOCS_INTF,
            .set = aocs_intf_set,
            .get = aocs_intf_get,
        }
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    class_register_attribute(clss);
    return;
}

void output_data_to_file(rs422_ram_dev* dev, int value)
{
    if (dev->uart_file)
    {
        char tmp[32] = {0};
        if (dev->flag)
        {
            sprintf(tmp, "%s:", "Rs422 uart output");
            dev->uart_file_iface->write(dev->uart_file, tmp, strlen(tmp));
            sprintf(tmp, "\n", strlen(tmp));
            dev->uart_file_iface->write(dev->uart_file, tmp, strlen(tmp));
            dev->flag = 0;
        }
        sprintf(tmp, "0x%x ", value & 0xFF);
        dev->uart_file_iface->write(dev->uart_file, tmp, strlen(tmp));
    }
}

static exception_t rs422_ram_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    rs422_ram_dev *dev = opaque->obj;
    rs422_uart_device *uart_dev = dev->rs422->obj;
    uint8_t val = *(uint8_t*)buf;
    int send_cnt;
    //output_data_to_file(dev, val);

    //公用串口写发送数据 0x10002000 - 0x100023f8  uart_fifo[0]
    if (offset >= UART_CHANNEL0_SEND_RAM_START
            && offset <= UART_CHANNEL0_SEND_RAM_END)
    {
        send_cnt = uart_dev->uart_fifo[0].send_cnt;
        uart_dev->uart_fifo[0].send_buf[send_cnt] = val;
        uart_dev->uart_fifo[0].send_cnt ++;
        return No_exp;
    }

    //19.2K串口写发送数据 0x10003000 - 0x100033f8 uart_fifo[1]
    if (offset >= UART_CHANNEL1_SEND_RAM_START 
            && offset <= UART_CHANNEL1_SEND_RAM_END)
    {
        send_cnt = uart_dev->uart_fifo[1].send_cnt;
        uart_dev->uart_fifo[1].send_buf[send_cnt] = val;
        uart_dev->uart_fifo[1].send_cnt ++;
        return No_exp;
    }

    //独立串口1写发送数据 0x10004000 - 0x100047f8 uart_fifo[2]
    if (offset >= UART_CHANNEL2_SEND_RAM_START
            && offset <= UART_CHANNEL2_SEND_RAM_END)
    {
        send_cnt = uart_dev->uart_fifo[2].send_cnt;
        uart_dev->uart_fifo[2].send_buf[send_cnt] = val;
        uart_dev->uart_fifo[2].send_cnt ++;
        return No_exp;
    }

    //独立串口2写发送数据 0x10004c00 - 0x100053f8 uart_fifo[3]
    if (offset >= UART_CHANNEL3_SEND_RAM_START
            && offset <= UART_CHANNEL3_SEND_RAM_END)
    {
        send_cnt = uart_dev->uart_fifo[3].send_cnt;
        uart_dev->uart_fifo[3].send_buf[send_cnt] = val;
        uart_dev->uart_fifo[3].send_cnt ++;
        return No_exp;
    }

    //独立串口3写发送数据 0x10005800 - 0x10005ff8 uart_fifo[4]
    if (offset >= UART_CHANNEL4_SEND_RAM_START
            && offset <= UART_CHANNEL4_SEND_RAM_END)
    {
        send_cnt = uart_dev->uart_fifo[4].send_cnt;
        uart_dev->uart_fifo[4].send_buf[send_cnt] = val;
        uart_dev->uart_fifo[4].send_cnt ++;
        return No_exp;
    }

    //独立串口4写发送数据 0x10006270 - 0x10006bf8 uart_fifo[5]
    if (offset >= UART_CHANNEL5_SEND_RAM_START
            && offset <= UART_CHANNEL5_SEND_RAM_END)
    {
        send_cnt = uart_dev->uart_fifo[5].send_cnt;
        uart_dev->uart_fifo[5].send_buf[send_cnt] = val;
        uart_dev->uart_fifo[5].send_cnt ++;
        return No_exp;
    }

    return No_exp;
}

static exception_t rs422_ram_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    rs422_ram_dev *dev = opaque->obj;
    rs422_uart_device *uart_dev = dev->rs422->obj;
    int recv_cnt;
    //公用串口写接收数据 0x10002800 - 0x10002bf8
    if (offset >= UART_CHANNEL0_RECV_RAM_START
            && offset <= UART_CHANNEL0_RECV_RAM_END)
    {
        recv_cnt = uart_dev->uart_fifo[0].recv_cnt;
        *(uint8_t *)buf = uart_dev->uart_fifo[0].recv_buf[recv_cnt] & 0xFF;
        uart_dev->uart_fifo[0].recv_cnt ++;
        return No_exp;

    }

    //19.2K串口写接收数据 0x10003800 - 0x10003bf8
    if (offset >= UART_CHANNEL1_RECV_RAM_START
            && offset <= UART_CHANNEL1_RECV_RAM_END)
    {
        recv_cnt = uart_dev->uart_fifo[1].recv_cnt;
        *(uint8_t *)buf = uart_dev->uart_fifo[1].recv_buf[recv_cnt] & 0xFF;
        uart_dev->uart_fifo[1].recv_cnt ++;
        return No_exp;
    }

    //独立串口1写接收数据 0x10007000 - 0x10008ff8
    if (offset >= UART_CHANNEL2_RECV_RAM_START
            && offset <= UART_CHANNEL2_RECV_RAM_END)
    {
        recv_cnt = uart_dev->uart_fifo[2].recv_cnt;
        *(uint8_t *)buf = uart_dev->uart_fifo[2].recv_buf[recv_cnt] & 0xFF;
        uart_dev->uart_fifo[2].recv_cnt ++;
        return No_exp;
    }

    //独立串口2写接收数据 0x10009400 - 0x1000b3f8
    if (offset >= UART_CHANNEL3_RECV_RAM_START
            && offset <= UART_CHANNEL3_RECV_RAM_END)
    {

        recv_cnt = uart_dev->uart_fifo[3].recv_cnt;
        *(uint8_t *)buf = uart_dev->uart_fifo[3].recv_buf[recv_cnt] & 0xFF;
        uart_dev->uart_fifo[3].recv_cnt ++;
        return No_exp;
    }

    //独立串口3写接收数据 0x1000b800 - 0x1000d7f8
    if (offset >= UART_CHANNEL4_RECV_RAM_START
            && offset <= UART_CHANNEL4_RECV_RAM_END)
    {
        recv_cnt = uart_dev->uart_fifo[4].recv_cnt;
        *(uint8_t *)buf = uart_dev->uart_fifo[4].recv_buf[recv_cnt] & 0xFF;
        uart_dev->uart_fifo[4].recv_cnt ++;
        return No_exp;
    }

    //独立串口4写接收数据 0x1000dc00 - 0x1000fbf8
    if (offset >= UART_CHANNEL5_RECV_RAM_START
            && offset <= UART_CHANNEL5_RECV_RAM_END)
    {
        recv_cnt = uart_dev->uart_fifo[5].recv_cnt;
        *(uint8_t *)buf = uart_dev->uart_fifo[5].recv_buf[recv_cnt] & 0xFF;
        uart_dev->uart_fifo[5].recv_cnt ++;
        return No_exp;
    }

    return No_exp;
}

static conf_object_t* new_rs422_ram(const char* obj_name)
{
    rs422_ram_dev* rs422_ram = skyeye_mm_zero(sizeof(rs422_ram_dev));
    rs422_ram->obj = new_conf_object(obj_name, rs422_ram);
    rs422_ram->flag = 1;

    return rs422_ram->obj;
}

static exception_t free_rs422_ram(conf_object_t* conf_obj)
{
    rs422_ram_dev* dev = conf_obj->obj;
    skyeye_free(dev->obj);
    skyeye_free(dev);
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    rs422_ram_dev* dev = (rs422_ram_dev*)(obj->obj);
    dev->uart_file = obj2;
    dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);

    if (dev->uart_file_iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get 'uart_file_intf' interface from %s fail.", obj2->objname);
        return Not_found_exp;
    }
    return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    rs422_ram_dev* dev = (rs422_ram_dev*)(obj->obj);
    *obj2 = dev->uart_file;
    *port = NULL;
    return No_exp;
}

static attr_value_t get_attr_rs422_object(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    rs422_ram_dev *dev = (rs422_ram_dev*)(conf_obj->obj);
    attr_value_t attr = SKY_make_attr_object(dev->rs422);
    return attr;
}

static exception_t set_attr_rs422_object(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    rs422_ram_dev *dev = (rs422_ram_dev*)(conf_obj->obj);
    dev->rs422 = SKY_attr_object(*value);
    return No_exp;
}

static rs422_ram_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "rs422_device", get_attr_rs422_object, NULL, set_attr_rs422_object, NULL, SKY_Attr_Optional, "object", "get rs422 device object");
}

void init_rs422_ram()
{
    static skyeye_class_t class_data =
    {
        .class_name = "rs422_ram",
        .class_desc = "rs422_ram",
        .new_instance = new_rs422_ram,
        .free_instance = free_rs422_ram,
        .get_attr = NULL,
        .set_attr = NULL,
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

    static const memory_space_intf io_memory =
    {
        .read = rs422_ram_read,
        .write = rs422_ram_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        }
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = UART_FILE_INTF,
            .set = uart_file_set,
            .get = uart_file_get,
        },
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    rs422_ram_register_attribute(clss);
    return;
}
