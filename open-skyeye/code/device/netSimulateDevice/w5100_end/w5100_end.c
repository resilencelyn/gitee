#include <skyeye_lock.h>
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
#define DEBUG
#include <skyeye_log.h>

#include "socket_operation.h"
#include "w5100_end.h"

// 处理端口命令
static Port_Command_Handle(conf_object_t *opaque, uint32_t port_num)
{
	w5100_end_device *dev = opaque->obj;

	debug_printf("\n\n%s\n\n", __func__);
	debug_printf("==: Port_Num: %d\n\n", port_num);
	switch (dev->regs.s[port_num].cr) {
	case Sn_CR_OPEN:
	{
		debug_printf("Sn_CR_OPEN\n");
		// 判断网络模式
		switch (dev->regs.s[port_num].mr & 0xF) {
		case Sn_MR_TCP:
			debug_printf("  Sn_MR_TCP\n");
			dev->regs.s[port_num].sr = SOCK_INIT;

			// TCP协议
			net_socket(opaque, port_num, ENET_SOCKET_TYPE_STREAM);
			break;
		case Sn_MR_UDP:
			debug_printf("  Sn_MR_UDP\n");
			dev->regs.s[port_num].sr = SOCK_UDP;

			// UDP协议
			net_socket(opaque, port_num, ENET_SOCKET_TYPE_DATAGRAM);
			break;
		case Sn_MR_IPRAW:
			debug_printf("  Sn_MR_IPRAW\n");
			dev->regs.s[port_num].sr = SOCK_IPRAW;
			break;
		case Sn_MR_MACRAW: // 端口0独有模式
			debug_printf("  Sn_MR_MACRAW\n");
			dev->regs.s[port_num].sr = SOCK_MACRAW;
			break;
		case Sn_MR_PPPOE:  // 端口0独有模式
			debug_printf("  Sn_MR_PPPOE\n");
			dev->regs.s[port_num].sr = SOCK_PPPOE;
			break;
		}

		break;
	}
	case Sn_CR_LISTEN:
		debug_printf("Sn_CR_LISTEN\n");
		if (dev->regs.s[port_num].sr == SOCK_INIT) {
			dev->regs.s[port_num].sr = SOCK_LISTEN;
		/*
		 * 当端口建立连接时将产生 SOCK_ESTABLISHED 状态
		 * 在这种状态下可以发送和接收 TCP 数据
		 */
		}
		break;
	case Sn_CR_CONNECT:
		debug_printf("Sn_CR_CONNECT\n");
		if (dev->regs.s[port_num].sr == SOCK_INIT) {
			dev->regs.s[port_num].sr = SOCK_SYNSENT;
		/*
		 * 当端口建立连接时将产生 SOCK_ESTABLISHED 状态
		 * 在这种状态下可以发送和接收 TCP 数据
		 */
		}
		break;
	case Sn_CR_DISCON:
		debug_printf("Sn_CR_DISCON\n");
		break;
	case Sn_CR_CLOSE:
		debug_printf("Sn_CR_CLOSE\n");
		dev->regs.s[port_num].sr = SOCK_CLOSED;
		break;
	case Sn_CR_SEND:
	{
		debug_printf("Sn_CR_SEND\n");

		// 判断网络模式
		switch (dev->regs.s[port_num].mr & 0xF) {
		case Sn_MR_TCP:  // 发送TCP数据
			debug_printf("  Sn_MR_TCP\n");
			dev->regs.s[port_num].sr = SOCK_INIT;

			// TCP发送没有实现
			break;
		case Sn_MR_UDP:
		{
			debug_printf("  Mode: UDP\n");
			dev->regs.s[port_num].sr = SOCK_UDP;

			debug_printf("send: %s\n", dev->end_port[port_num].tx_buff);
			int i;
			for (i = 0; i < 5; i++) {
				debug_printf("i:%d 0x%x\n", i, dev->end_port[port_num].tx_buff[i]);
			}

			uint32_t send_len;
			// 发送长度, 暂定为: tx_wr1 | tx_wr0
			send_len = dev->regs.s[port_num].tx_wr1 | (dev->regs.s[port_num].tx_wr0 << 8);
			// 发送网络数据包
			net_sendto(opaque, port_num, dev->end_port[port_num].tx_buff, send_len);

			// 更新缓存大小
			dev->regs.s[port_num].tx_wr1 = 0;
			dev->regs.s[port_num].tx_wr0 = 0;

			// [临时改动] 表示数据已经发送工程
			dev->regs.s[port_num].ir = Sn_IR_SEND_OK;

			break;
		}
		default:
			break;
		}

		break;
	}
	case Sn_CR_SEND_MAC:
		debug_printf("Sn_CR_SEND_MAC\n");
		break;
	case Sn_CR_SEND_KEEP:
		debug_printf("Sn_CR_SEND_KEEP\n");
		break;
	case Sn_CR_RECV:
		debug_printf("Sn_CR_RECV\n");
		// 更新缓存指针位置
		//dev->regs.s[port_num].rx_rd1 = 0;
		//dev->regs.s[port_num].rx_rd0 = 0;

		break;
	default:
		break;
	}

	debug_printf("\n=======================\n\n");
}

static exception_t w5100_end_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	w5100_end_device *dev = opaque->obj;
	switch (offset)
	{
		case MR         :
			*(uint8_t *)buf = dev->regs.mr         ;
			break;
		case GAR0       :
			*(uint8_t *)buf = dev->regs.gar0       ;
			break;
		case GAR1       :
			*(uint8_t *)buf = dev->regs.gar1       ;
			break;
		case GAR2       :
			*(uint8_t *)buf = dev->regs.gar2       ;
			break;
		case GAR3       :
			*(uint8_t *)buf = dev->regs.gar3       ;
			break;
		case SUBR0      :
			*(uint8_t *)buf = dev->regs.subr0      ;
			break;
		case SUBR1      :
			*(uint8_t *)buf = dev->regs.subr1      ;
			break;
		case SUBR2      :
			*(uint8_t *)buf = dev->regs.subr2      ;
			break;
		case SUBR3      :
			*(uint8_t *)buf = dev->regs.subr3      ;
			break;
		case SHAR0      :
			*(uint8_t *)buf = dev->regs.shar0      ;
			break;
		case SHAR1      :
			*(uint8_t *)buf = dev->regs.shar1      ;
			break;
		case SHAR2      :
			*(uint8_t *)buf = dev->regs.shar2      ;
			break;
		case SHAR3      :
			*(uint8_t *)buf = dev->regs.shar3      ;
			break;
		case SHAR4      :
			*(uint8_t *)buf = dev->regs.shar4      ;
			break;
		case SHAR5      :
			*(uint8_t *)buf = dev->regs.shar5      ;
			break;
		case SIPR0      :
			*(uint8_t *)buf = dev->regs.sipr0      ;
			break;
		case SIPR1      :
			*(uint8_t *)buf = dev->regs.sipr1      ;
			break;
		case SIPR2      :
			*(uint8_t *)buf = dev->regs.sipr2      ;
			break;
		case SIPR3      :
			*(uint8_t *)buf = dev->regs.sipr3      ;
			break;
		case IR         :
			*(uint8_t *)buf = dev->regs.ir         ;
			break;
		case IMR        :
			*(uint8_t *)buf = dev->regs.imr        ;
			break;
		case RTR0       :
			*(uint8_t *)buf = dev->regs.rtr0       ;
			break;
		case RTR1       :
			*(uint8_t *)buf = dev->regs.rtr1       ;
			break;
		case RCR        :
			*(uint8_t *)buf = dev->regs.rcr        ;
			break;
		case RMSR       :
			*(uint8_t *)buf = dev->regs.rmsr       ;
			break;
		case TMSR       :
			*(uint8_t *)buf = dev->regs.tmsr       ;
			break;
		case PATR0      :
			*(uint8_t *)buf = dev->regs.patr0      ;
			break;
		case PATR1      :
			*(uint8_t *)buf = dev->regs.patr1      ;
			break;
		case PTIMER     :
			*(uint8_t *)buf = dev->regs.ptimer     ;
			break;
		case PMAGIC     :
			*(uint8_t *)buf = dev->regs.pmagic     ;
			break;
		case UIPR0      :
			*(uint8_t *)buf = dev->regs.uipr0      ;
			break;
		case UIPR1      :
			*(uint8_t *)buf = dev->regs.uipr1      ;
			break;
		case UIPR2      :
			*(uint8_t *)buf = dev->regs.uipr2      ;
			break;
		case UIPR3      :
			*(uint8_t *)buf = dev->regs.uipr3      ;
			break;
		case UPORT0     :
			*(uint8_t *)buf = dev->regs.uport0     ;
			break;
		case UPORT1     :
			*(uint8_t *)buf = dev->regs.uport1     ;
			break;
		default: {
			if ((offset >= 0x0400) && (offset <= 0x0729))
			{
				uint32_t ex_offset;
				uint32_t port_num;

				port_num  = (offset - 0x0400) / 0x100;
				ex_offset = (offset - 0x0400) % 0x100;

				switch (ex_offset) {
				case Sn_MR      :
					*(uint8_t *)buf = dev->regs.s[port_num].mr      ;
					break;
				case Sn_CR      :
					*(uint8_t *)buf = dev->regs.s[port_num].cr      ;
					break;
				case Sn_IR      :
					*(uint8_t *)buf = dev->regs.s[port_num].ir      ;
					break;
				case Sn_SR      :
					*(uint8_t *)buf = dev->regs.s[port_num].sr      ;
					break;
				case Sn_PORT0   :
					*(uint8_t *)buf = dev->regs.s[port_num].port0   ;
					break;
				case Sn_PORT1   :
					*(uint8_t *)buf = dev->regs.s[port_num].port1   ;
					break;
				case Sn_DHAR0   :
					*(uint8_t *)buf = dev->regs.s[port_num].dhar0   ;
					break;
				case Sn_DHAR1   :
					*(uint8_t *)buf = dev->regs.s[port_num].dhar1   ;
					break;
				case Sn_DHAR2   :
					*(uint8_t *)buf = dev->regs.s[port_num].dhar2   ;
					break;
				case Sn_DHAR3   :
					*(uint8_t *)buf = dev->regs.s[port_num].dhar3   ;
					break;
				case Sn_DHAR4   :
					*(uint8_t *)buf = dev->regs.s[port_num].dhar4   ;
					break;
				case Sn_DHAR5   :
					*(uint8_t *)buf = dev->regs.s[port_num].dhar5   ;
					break;
				case Sn_DIPR0   :
					*(uint8_t *)buf = dev->regs.s[port_num].dipr0   ;
					break;
				case Sn_DIPR1   :
					*(uint8_t *)buf = dev->regs.s[port_num].dipr1   ;
					break;
				case Sn_DIPR2   :
					*(uint8_t *)buf = dev->regs.s[port_num].dipr2   ;
					break;
				case Sn_DIPR3   :
					*(uint8_t *)buf = dev->regs.s[port_num].dipr3   ;
					break;
				case Sn_DPORT0  :
					*(uint8_t *)buf = dev->regs.s[port_num].dport0  ;
					break;
				case Sn_DPORT1  :
					*(uint8_t *)buf = dev->regs.s[port_num].dport1  ;
					break;
				case Sn_MSSR0   :
					*(uint8_t *)buf = dev->regs.s[port_num].mssr0   ;
					break;
				case Sn_MSSR1   :
					*(uint8_t *)buf = dev->regs.s[port_num].mssr1   ;
					break;
				case Sn_PROTO   :
					*(uint8_t *)buf = dev->regs.s[port_num].proto   ;
					break;
				case Sn_TOS     :
					*(uint8_t *)buf = dev->regs.s[port_num].tos     ;
					break;
				case Sn_TTL     :
					*(uint8_t *)buf = dev->regs.s[port_num].ttl     ;
					break;
				case Sn_TX_FSR0 :
					*(uint8_t *)buf = dev->regs.s[port_num].tx_fsr0 ;
					break;
				case Sn_TX_FSR1 :
					*(uint8_t *)buf = dev->regs.s[port_num].tx_fsr1 ;
					break;
				case Sn_TX_RD0  :
					*(uint8_t *)buf = dev->regs.s[port_num].tx_rd0  ;
					break;
				case Sn_TX_RD1  :
					*(uint8_t *)buf = dev->regs.s[port_num].tx_rd1  ;
					break;
				case Sn_TX_WR0  :
					*(uint8_t *)buf = dev->regs.s[port_num].tx_wr0  ;
					break;
				case Sn_TX_WR1  :
					*(uint8_t *)buf = dev->regs.s[port_num].tx_wr1  ;
					break;
				case Sn_RX_RSR0 :
					*(uint8_t *)buf = dev->regs.s[port_num].rx_rsr0 ;
					break;
				case Sn_RX_RSR1 :
					*(uint8_t *)buf = dev->regs.s[port_num].rx_rsr1 ;
					break;
				case Sn_RX_RD0  :
					*(uint8_t *)buf = dev->regs.s[port_num].rx_rd0  ;
					break;
				case Sn_RX_RD1  :
					*(uint8_t *)buf = dev->regs.s[port_num].rx_rd1  ;
					break;
				default:
					skyeye_log(Error_log, __FUNCTION__, "[port]: Can not read the register at 0x%x in w5100_end\n", offset);
					break;
				}
			} else if ((offset >= 0x4000) && (offset < 0x6000)) {
				// 发送缓存区间
				skyeye_log(Error_log, __FUNCTION__, "Wrete read [TX memory] at 0x%x in w5100_end\n", offset);

			} else if ((offset >= 0x6000)                     && (offset < dev->end_port[0].rx_offset)) {
				// 指示端口接收数据缓冲区中接收数据的字节数
				uint32_t rx_rsr = dev->regs.s[0].rx_rsr1 | (dev->regs.s[0].rx_rsr0 << 8);

				// 每读取一个字节, 递减一次(为零就不减了)
				if (rx_rsr) {
					rx_rsr --;
				}
				dev->regs.s[0].rx_rsr0   = (rx_rsr>>8) & 0xFF;
				dev->regs.s[0].rx_rsr1   = (rx_rsr   ) & 0xFF;

				// 接收缓存区间 0
				*(uint8_t *)buf = dev->rx_tx_memory[offset-0x4000] & 0xFF;
			} else if ((offset >= dev->end_port[0].rx_offset) && (offset < dev->end_port[1].rx_offset)) {
				// 指示端口接收数据缓冲区中接收数据的字节数
				uint32_t rx_rsr = dev->regs.s[1].rx_rsr1 | (dev->regs.s[1].rx_rsr0 << 8);

				// 每读取一个字节, 递减一次(为零就不减了)
				if (rx_rsr) {
					rx_rsr --;
				}
				dev->regs.s[1].rx_rsr0   = (rx_rsr>>8) & 0xFF;
				dev->regs.s[1].rx_rsr1   = (rx_rsr   ) & 0xFF;

				// 接收缓存区间 1
				*(uint8_t *)buf = dev->rx_tx_memory[offset-0x4000] & 0xFF;
			} else if ((offset >= dev->end_port[1].rx_offset) && (offset < dev->end_port[2].rx_offset)) {
				static uint32_t kkkk = 0;
				// 指示端口接收数据缓冲区中接收数据的字节数
				uint32_t rx_rsr = dev->regs.s[2].rx_rsr1 | (dev->regs.s[2].rx_rsr0 << 8);

				// 每读取一个字节, 递减一次(为零就不减了)
				if (rx_rsr) {
					rx_rsr --;
				}
				dev->regs.s[2].rx_rsr0   = (rx_rsr>>8) & 0xFF;
				dev->regs.s[2].rx_rsr1   = (rx_rsr   ) & 0xFF;

				// 接收缓存区间 2
				*(uint8_t *)buf = dev->rx_tx_memory[offset-0x4000] & 0xFF;
			} else if ((offset >= dev->end_port[2].rx_offset) && (offset < 0x8000)) {
				// 指示端口接收数据缓冲区中接收数据的字节数
				uint32_t rx_rsr = dev->regs.s[3].rx_rsr1 | (dev->regs.s[3].rx_rsr0 << 8);

				// 每读取一个字节, 递减一次(为零就不减了)
				if (rx_rsr) {
					rx_rsr --;
				}
				dev->regs.s[3].rx_rsr0   = (rx_rsr>>8) & 0xFF;
				dev->regs.s[3].rx_rsr1   = (rx_rsr   ) & 0xFF;

				// 接收缓存区间 3
				*(uint8_t *)buf = dev->rx_tx_memory[offset-0x4000] & 0xFF;
			} else {
				skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in w5100_end\n", offset);
			}
			break;
		}
	}

	//printf("%s  offset: 0x%04x buf: 0x%x, count: %d\n", __func__, offset, *(uint8_t *)buf, count);
	return 0;
}

static exception_t w5100_end_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	w5100_end_device *dev = opaque->obj;
	uint32_t val = *(uint8_t *)buf;

	//printf("%s offset: 0x%04x buf: 0x%x count: %d\n", __func__, offset, *(uint8_t *)buf, count);
	switch (offset)
	{
		case MR         :
			dev->regs.mr          = val;
			// 7: RST, 4: PB, 3: PPPoE, 1: AI, 0: IND
			if (dev->regs.mr & MR_RST) {
			}
			break;
		case GAR0       :
			dev->regs.gar0        = val;
			break;
		case GAR1       :
			dev->regs.gar1        = val;
			break;
		case GAR2       :
			dev->regs.gar2        = val;
			break;
		case GAR3       :
			dev->regs.gar3        = val;
			break;
		case SUBR0      :
			dev->regs.subr0       = val;
			break;
		case SUBR1      :
			dev->regs.subr1       = val;
			break;
		case SUBR2      :
			dev->regs.subr2       = val;
			break;
		case SUBR3      :
			dev->regs.subr3       = val;
			break;
		case SHAR0      :
			dev->regs.shar0       = val;
			break;
		case SHAR1      :
			dev->regs.shar1       = val;
			break;
		case SHAR2      :
			dev->regs.shar2       = val;
			break;
		case SHAR3      :
			dev->regs.shar3       = val;
			break;
		case SHAR4      :
			dev->regs.shar4       = val;
			break;
		case SHAR5      :
			dev->regs.shar5       = val;
			break;
		case SIPR0      :
			dev->regs.sipr0       = val;
			break;
		case SIPR1      :
			dev->regs.sipr1       = val;
			break;
		case SIPR2      :
			dev->regs.sipr2       = val;
			break;
		case SIPR3      :
			dev->regs.sipr3       = val;
			break;
		case IR         :
			dev->regs.ir          = val;
			break;
		case IMR        :
			dev->regs.imr         = val;
			break;
		case RTR0       :
			dev->regs.rtr0        = val;
			break;
		case RTR1       :
			dev->regs.rtr1        = val;
			break;
		case RCR        :
			dev->regs.rcr         = val;
			break;
		case RMSR       : // RX Memory Size Register
		{
			dev->regs.rmsr        = val;
			int l_ui32_i;
			uint32_t curr_offset = 0;
			for (l_ui32_i = 0; l_ui32_i < MAX_SOCK_NUM; l_ui32_i++) {
				if (curr_offset < 0x2000) {
					uint32_t buf_size;
					switch ((dev->regs.rmsr > (l_ui32_i * 2)) & BUF_SIZE_MASK) {
					case BUF_1K_FLAG:
						buf_size = BUF_1K_DEF;
						break;
					case BUF_2K_FLAG:
						buf_size = BUF_2K_DEF;
						break;
					case BUF_4K_FLAG:
						buf_size = BUF_4K_DEF;
						break;
					case BUF_8K_FLAG:
						buf_size = BUF_8K_DEF;
						break;
					}
					// 设置缓存大小
					dev->end_port[l_ui32_i].rx_size = buf_size;

					// 根据分配的大小, 指向缓存区的位置
					dev->end_port[l_ui32_i].rx_buff = &(dev->rx_tx_memory[0x2000+curr_offset]);

					curr_offset += buf_size;

					// 计算接收数据偏移界限
					dev->end_port[l_ui32_i].rx_offset = curr_offset + 0x6000;
				}
			}
			break;
		}
		case TMSR       : // TX Memory Size Register
		{
			dev->regs.tmsr        = val;
			int l_ui32_i;
			uint32_t curr_offset = 0;
			for (l_ui32_i = 0; l_ui32_i < MAX_SOCK_NUM; l_ui32_i++) {
				if (curr_offset < 0x2000) {
					uint32_t buf_size;
					switch ((dev->regs.tmsr > (l_ui32_i * 2)) & BUF_SIZE_MASK) {
					case BUF_1K_FLAG:
						buf_size = BUF_1K_DEF;
						break;
					case BUF_2K_FLAG:
						buf_size = BUF_2K_DEF;
						break;
					case BUF_4K_FLAG:
						buf_size = BUF_4K_DEF;
						break;
					case BUF_8K_FLAG:
						buf_size = BUF_8K_DEF;
						break;
					}

					// 发送缓存的空间大小, 这个可以直接设置好, 在设备里面不需要实现发送缓存的
					// 与Sn_TX_RSR有一定的区别
					dev->regs.s[l_ui32_i].tx_fsr0   = (buf_size>>8) & 0xFF;
					dev->regs.s[l_ui32_i].tx_fsr1   = (buf_size   ) & 0xFF;

					// 设置缓存大小
					dev->end_port[l_ui32_i].tx_size = buf_size;

					// 根据分配的大小, 指向缓存区的位置
					dev->end_port[l_ui32_i].tx_buff = &(dev->rx_tx_memory[curr_offset]);

					curr_offset += buf_size;
				}
			}
			break;
		}
		case PATR0      :
			dev->regs.patr0       = val;
			break;
		case PATR1      :
			dev->regs.patr1       = val;
			break;
		case PTIMER     :
			dev->regs.ptimer      = val;
			break;
		case PMAGIC     :
			dev->regs.pmagic      = val;
			break;
		case UIPR0      :
			dev->regs.uipr0       = val;
			break;
		case UIPR1      :
			dev->regs.uipr1       = val;
			break;
		case UIPR2      :
			dev->regs.uipr2       = val;
			break;
		case UIPR3      :
			dev->regs.uipr3       = val;
			break;
		case UPORT0     :
			dev->regs.uport0      = val;
			break;
		case UPORT1     :
			dev->regs.uport1      = val;
			break;

		default: {

			if ((offset >= 0x0400) && (offset <= 0x0729))
			{
				uint32_t ex_offset;
				uint32_t port_num;

				port_num  = (offset - 0x0400) / 0x100;
				ex_offset = (offset - 0x0400) % 0x100;

				switch (ex_offset) {
				case Sn_MR      :
				{
					// 端口模式设置
					dev->regs.s[port_num].mr       = val;
					break;
				}
				case Sn_CR      :
					dev->regs.s[port_num].cr       = val;

					// 处理命令
					Port_Command_Handle(opaque, port_num);
					// 确认命令后, 清零操作
					dev->regs.s[port_num].cr       = 0;
					break;
				case Sn_IR      :
				{
					// 写1清零
					dev->regs.s[port_num].ir      &= ~(val);
					break;
				}
				case Sn_SR      :
					dev->regs.s[port_num].sr       = val;
					break;

				// 端口n源端口寄存器
				// TCP/UDP 模式下设定的端口号
				case Sn_PORT0   :
					dev->regs.s[port_num].port0    = val;
					break;
				case Sn_PORT1   :
					dev->regs.s[port_num].port1    = val;
					break;

				// 端口n目标物理地址寄存器
				case Sn_DHAR0   :
					dev->regs.s[port_num].dhar0    = val;
					break;
				case Sn_DHAR1   :
					dev->regs.s[port_num].dhar1    = val;
					break;
				case Sn_DHAR2   :
					dev->regs.s[port_num].dhar2    = val;
					break;
				case Sn_DHAR3   :
					dev->regs.s[port_num].dhar3    = val;
					break;
				case Sn_DHAR4   :
					dev->regs.s[port_num].dhar4    = val;
					break;
				case Sn_DHAR5   :
					dev->regs.s[port_num].dhar5    = val;
					break;

				/* 端口n目标IP地址寄存器
				 * 在TCP模式下, 该寄存器设置端口的目的IP地址
				 * 在TCP模式下, 收到对段的ARP响应后, 该寄存器才确定为用户写入的值
				 *    没有收到对端的ARP响应, 该寄存器复位
				 */
				case Sn_DIPR0   :
					dev->regs.s[port_num].dipr0    = val;
					break;
				case Sn_DIPR1   :
					dev->regs.s[port_num].dipr1    = val;
					break;
				case Sn_DIPR2   :
					dev->regs.s[port_num].dipr2    = val;
					break;
				case Sn_DIPR3   :
					dev->regs.s[port_num].dipr3    = val;
					break;

				// 端口n目标端口寄存器
				case Sn_DPORT0  :
					dev->regs.s[port_num].dport0   = val;
					break;
				case Sn_DPORT1  :
					dev->regs.s[port_num].dport1   = val;
					break;
				case Sn_MSSR0   :
					dev->regs.s[port_num].mssr0    = val;
					break;
				case Sn_MSSR1   :
					dev->regs.s[port_num].mssr1    = val;
					break;
				case Sn_PROTO   :
					dev->regs.s[port_num].proto    = val;
					break;
				case Sn_TOS     :
					dev->regs.s[port_num].tos      = val;
					break;
				case Sn_TTL     :
					dev->regs.s[port_num].ttl      = val;
					break;
				case Sn_TX_FSR0 :
					dev->regs.s[port_num].tx_fsr0  = val;
					break;
				case Sn_TX_FSR1 :
					dev->regs.s[port_num].tx_fsr1  = val;
					break;
				case Sn_TX_RD0  :
					dev->regs.s[port_num].tx_rd0   = val;
					break;
				case Sn_TX_RD1  :
					dev->regs.s[port_num].tx_rd1   = val;
					break;
				case Sn_TX_WR0  :
					dev->regs.s[port_num].tx_wr0   = val;
					break;
				case Sn_TX_WR1  :
					dev->regs.s[port_num].tx_wr1   = val;
					break;
				case Sn_RX_RSR0 :
					dev->regs.s[port_num].rx_rsr0  = val;
					break;
				case Sn_RX_RSR1 :
					dev->regs.s[port_num].rx_rsr1  = val;
					break;
				case Sn_RX_RD0  :
					dev->regs.s[port_num].rx_rd0   = val;
					break;
				case Sn_RX_RD1  : {
					dev->regs.s[port_num].rx_rd1   = val;
					uint32_t read_offset = dev->regs.s[port_num].rx_rd1 | (dev->regs.s[port_num].rx_rd0 << 8);
					uint32_t offset_size = dev->end_port[port_num].rx_size;   // 通道最大空间
					if (read_offset > offset_size) {
						read_offset = read_offset - offset_size;
						dev->regs.s[port_num].rx_rd0   = (read_offset>>8) & 0xFF;
						dev->regs.s[port_num].rx_rd1   = (read_offset   ) & 0xFF;
					}
				}
					break;
				default:
					skyeye_log(Error_log, __FUNCTION__, "[port]: Cannot write the register at 0x%x in w5100_end\n", offset);
					break;
				}
			} else if ((offset >= 0x4000) && (offset < 0x6000)) {
				// 发送缓存区间
				dev->rx_tx_memory[offset-0x4000] = (uint8_t)(val & 0xFF);
			} else if ((offset >= 0x6000) && (offset < 0x8000)) {
				// 接收缓存区间
				skyeye_log(Error_log, __FUNCTION__, "Cannot write [RX memory] at 0x%x in w5100_end\n", offset);
			} else {
				skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in w5100_end\n", offset);
			}
			break;
		}
	}

	return 0;
}


static exception_t w5100_end_read2(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	w5100_end_device *dev = opaque->obj;

	switch (count) {
	case 1: {
		uint8_t * data  = &dev->dt_eth_cfg;
		*(uint8_t *)buf = data[offset];
		break;
	}
	case 2: {
		uint16_t * data  = &dev->dt_eth_cfg;
		*(uint16_t *)buf = data[offset/2];
		break;
	}
	case 4: {
		uint32_t * data  = &dev->dt_eth_cfg;
		*(uint32_t *)buf = data[offset/4];
		break;
	}
	default:
		break;
	}

	//printf("%s  offset: 0x%04x buf: 0x%x count: %d\n", __func__, offset, *(uint32_t *)buf, count);
	return 0;
}

static exception_t w5100_end_write2(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	w5100_end_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;

	//printf("%s offset: 0x%04x buf: 0x%x count: %d\n", __func__, offset, *(uint32_t *)buf, count);

	switch (count) {
	case 1: {
		uint8_t * data  = &dev->dt_eth_cfg;
		data[offset]     = *(uint8_t *)buf;
		break;
	}
	case 2: {
		uint16_t * data  = &dev->dt_eth_cfg;
		data[offset/2]   = *(uint16_t *)buf;
		break;
	}
	case 4: {
		uint32_t * data  = &dev->dt_eth_cfg;
		data[offset/4]   = *(uint32_t *)buf;
		break;
	}
	default:
		break;
	}
	return 0;
}

static char* w5100_end_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t w5100_end_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	w5100_end_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t w5100_end_get_regnum(conf_object_t *conf_obj)
{
	return sizeof(struct registers) / 4;
}

static uint32_t w5100_end_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	w5100_end_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t w5100_end_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

void *order_memcpy(void *dest, const void *src, size_t n)
{
	if ((src == NULL) || (dest == NULL)) {
		return NULL;
	}

	while ( n-- ) {
		((char *)dest)[n] = *((char *)src++);
	}

	return dest;
}

static conf_object_t* new_w5100_end(const char *obj_name)
{
	w5100_end_device* dev = skyeye_mm_zero(sizeof(w5100_end_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	net_initialize();

#if 0
	// 设置网卡配置信息

	// IP: 192.168.0.103
	strcpy(dev->dt_eth_cfg.MagicIPString,  "IP:");
	order_memcpy(dev->dt_eth_cfg.IpAddr,   "\xC0\xA8\x00\x67", 4);

	// Mac: 0C-84-DC-7B-A3-1C
	strcpy(dev->dt_eth_cfg.MagicMacString, "MAC:");
	order_memcpy(dev->dt_eth_cfg.MacAddr,  "\x0C\x84\xDC\x7B\xA3\x1C", 6);

	// Port: 1100
	strcpy(dev->dt_eth_cfg.MagicDisplayPortString, "PORT:");
	order_memcpy(dev->dt_eth_cfg.Port,             "\x04\x4C", 2);
#endif

	RWLOCK_INIT(dev->w5100_end_lock);

	//RW_WRLOCK(dev->w5100_end_lock);
	// do anything
	//RW_UNLOCK(dev->w5100_end_lock);

	return dev->obj;
}

static exception_t config_w5100_end(conf_object_t *opaque)
{
	w5100_end_device *dev = (w5100_end_device *)opaque->obj;

	// 发送中断测试(暂时没有设计中断)
	//system_register_timer_handler(dev->obj, 50, (time_sched_t)time_callback, SCHED_MS | SCHED_NORMAL);

}

static exception_t free_w5100_end(conf_object_t* opaque)
{
	net_deinitialize();

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static attr_value_t get_attr_W5100_IP(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	w5100_end_device *dev = conf_obj->obj;
	attr_value_t W5100_IP = SKY_make_attr_string(dev->W5100_IP);
	return W5100_IP;
}

static exception_t set_attr_W5100_IP(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	w5100_end_device *dev = conf_obj->obj;
	dev->W5100_IP = SKY_attr_string(*value);

	// +4防止sscanf溢出
	uint8_t ip[4+4] = {0};

	sscanf(dev->W5100_IP, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);

	// 设置IP地址
	strcpy(dev->dt_eth_cfg.MagicIPString, "IP:");
	memcpy(dev->dt_eth_cfg.IpAddr, ip, 4);

	return No_exp;
}

static attr_value_t get_attr_W5100_MAC(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	w5100_end_device *dev = conf_obj->obj;
	attr_value_t W5100_MAC = SKY_make_attr_string(dev->W5100_MAC);
	return W5100_MAC;
}

static exception_t set_attr_W5100_MAC(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
	w5100_end_device *dev = conf_obj->obj;
	dev->W5100_MAC = SKY_attr_string(*value);

	// +4防止sscanf溢出
	unsigned char mac[6+4] = {0};

	sscanf(dev->W5100_MAC, "%x-%x-%x-%x-%x-%x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

	// 设置MAP地址
	strcpy(dev->dt_eth_cfg.MagicMacString, "MAC:");
	memcpy(dev->dt_eth_cfg.MacAddr,  mac, 6);

	return No_exp;
}

static attr_value_t get_attr_display_port(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
	w5100_end_device *dev = conf_obj->obj;
	attr_value_t display_port = SKY_make_attr_uinteger(dev->display_port);
	return display_port;
}

static exception_t set_attr_display_port(void* arg, conf_object_t* conf_obj,attr_value_t* value, attr_value_t* idx)
{
	w5100_end_device *dev = conf_obj->obj;
	dev->display_port = SKY_attr_uinteger(*value);

	uint16_t display_port = dev->display_port;
	// 设置端口号
	strcpy(dev->dt_eth_cfg.MagicDisplayPortString, "PORT:");
	memcpy(dev->dt_eth_cfg.Port,                   &display_port, 2);

	return No_exp;
}

static void class_register_attribute(conf_class_t* clss) {
	SKY_register_attribute(clss,
		"W5100_IP",
		get_attr_W5100_IP, NULL,
		set_attr_W5100_IP, NULL,
		SKY_Attr_Optional,
		"string",
		"W5100_IP");

	SKY_register_attribute(clss,
		"W5100_MAC",
		get_attr_W5100_MAC, NULL,
		set_attr_W5100_MAC, NULL,
		SKY_Attr_Optional,
		"string",
		"W5100_MAC");

	SKY_register_attribute(clss,
		"display_port",
		get_attr_display_port, NULL,
		set_attr_display_port, NULL,
		SKY_Attr_Optional,
		"uinteger",
		"server port");

	return;
}

void init_w5100_end(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "w5100_end",
		.class_desc = "w5100_end",
		.new_instance = new_w5100_end,
		.config_instance = config_w5100_end,
		.free_instance = free_w5100_end,
		.module_type = SKYML_OFFCHIP,
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory[] =
	{
		// 下标0: 网卡端口配置寄存器
		{
			.read = w5100_end_read,
			.write = w5100_end_write,
		},
		// 下标1: 以太网配置寄存器
		{
			.read = w5100_end_read2,
			.write = w5100_end_write2,
		},
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = w5100_end_get_regvalue_by_id,
		.get_regname_by_id = w5100_end_get_regname_by_id,
		.set_regvalue_by_id = w5100_end_set_regvalue_by_id,
		.get_regnum = w5100_end_get_regnum,
		.get_regid_by_name = w5100_end_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = 
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
	};
	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
	class_register_attribute(clss);
}
