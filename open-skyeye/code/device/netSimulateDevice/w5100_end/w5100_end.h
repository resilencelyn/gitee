#ifndef __W5100_END_H__
#define __W5100_END_H__

// 调试设备的开关
#define debug_w5100 0
#if debug_w5100
#define debug_printf printf
#else
#define debug_printf
#endif

#define SET_BIT(reg, mask) ((reg) |= (mask))
#define CLR_BIT(reg, mask) ((reg) &= ~(mask))
#define CHK_BIT(reg, mask) ((reg) & (mask))

#define MAX_SOCK_NUM 4

/* MODE register values */
#define MR_RST			0x80 /**< reset */
#define MR_PB			0x10 /**< ping block */
#define MR_PPPOE		0x08 /**< enable pppoe */
#define MR_LB			0x04 /**< little or big endian selector in indirect mode */
#define MR_AI			0x02 /**< auto-increment in indirect mode */
#define MR_IND			0x01 /**< enable indirect mode */

/* IR register values */
#define IR_CONFLICT	0x80 /**< check ip confict */
#define IR_UNREACH	0x40 /**< get the destination unreachable message in UDP sending */
#define IR_PPPoE		0x20 /**< get the PPPoE close message */
#define IR_SOCK(ch)	(0x01 << ch) /**< check socket interrupt */

/* Sn_MR values */
#define Sn_MR_CLOSE		0x00		/**< unused socket */
#define Sn_MR_TCP		0x01		/**< TCP */
#define Sn_MR_UDP		0x02		/**< UDP */
#define Sn_MR_IPRAW		0x03		/**< IP LAYER RAW SOCK */
#define Sn_MR_MACRAW	0x04		/**< MAC LAYER RAW SOCK */
#define Sn_MR_PPPOE		0x05		/**< PPPoE */
#define Sn_MR_ND		0x20		/**< No Delayed Ack(TCP) flag */
#define Sn_MR_MULTI		0x80		/**< support multicating */

/* Sn_CR values */
#define Sn_CR_OPEN		0x01		/**< initialize or open socket */
#define Sn_CR_LISTEN	0x02		/**< wait connection request in tcp mode(Server mode) */
#define Sn_CR_CONNECT	0x04		/**< send connection request in tcp mode(Client mode) */
#define Sn_CR_DISCON	0x08		/**< send closing reqeuset in tcp mode */
#define Sn_CR_CLOSE		0x10		/**< close socket */
#define Sn_CR_SEND		0x20		/**< updata txbuf pointer, send data */
#define Sn_CR_SEND_MAC	0x21		/**< send data with MAC address, so without ARP process */
#define Sn_CR_SEND_KEEP	0x22		/**<  send keep alive message */
#define Sn_CR_RECV		0x40		/**< update rxbuf pointer, recv data */

#define Sn_IR_SEND_OK	0x10		/**< complete sending */
#define Sn_IR_TIMEOUT	0x08		/**< assert timeout */
#define Sn_IR_RECV		0x04		/**< receiving data */
#define Sn_IR_DISCON	0x02		/**< closed socket */
#define Sn_IR_CON		0x01		/**< established connection */

/* Sn_SR values */
#define SOCK_CLOSED			0x00		/**< closed */
#define SOCK_INIT			0x13		/**< init state */
#define SOCK_LISTEN			0x14		/**< listen state */
#define SOCK_SYNSENT		0x15		/**< connection state */
#define SOCK_SYNRECV		0x16		/**< connection state */
#define SOCK_ESTABLISHED	0x17		/**< success to connect */
#define SOCK_FIN_WAIT		0x18		/**< closing state */
#define SOCK_CLOSING		0x1A		/**< closing state */
#define SOCK_TIME_WAIT		0x1B		/**< closing state */
#define SOCK_CLOSE_WAIT		0x1C		/**< closing state */
#define SOCK_LAST_ACK		0x1D		/**< closing state */
#define SOCK_UDP			0x22		/**< udp socket */
#define SOCK_IPRAW			0x32		/**< ip raw mode socket */
#define SOCK_MACRAW			0x42		/**< mac raw mode socket */
#define SOCK_PPPOE			0x5F		/**< pppoe socket */

/* IP PROTOCOL */
#define IPPROTO_IP          0           /**< Dummy for IP */
#define IPPROTO_ICMP        1           /**< Control message protocol */
#define IPPROTO_IGMP        2           /**< Internet group management protocol */
#define IPPROTO_GGP         3           /**< Gateway^2 (deprecated) */
#define IPPROTO_TCP         6           /**< TCP */
#define IPPROTO_PUP         12          /**< PUP */
#define IPPROTO_UDP         17          /**< UDP */
#define IPPROTO_IDP         22          /**< XNS idp */
#define IPPROTO_ND          77          /**< UNOFFICIAL net disk protocol */
#define IPPROTO_RAW         255         /**< Raw IP packet */

// 设备寄存器
struct registers
{
	uint32_t mr         ;
	uint32_t gar0       ;
	uint32_t gar1       ;
	uint32_t gar2       ;
	uint32_t gar3       ;
	uint32_t subr0      ;
	uint32_t subr1      ;
	uint32_t subr2      ;
	uint32_t subr3      ;
	uint32_t shar0      ;
	uint32_t shar1      ;
	uint32_t shar2      ;
	uint32_t shar3      ;
	uint32_t shar4      ;
	uint32_t shar5      ;
	uint32_t sipr0      ;
	uint32_t sipr1      ;
	uint32_t sipr2      ;
	uint32_t sipr3      ;
	uint32_t ir         ;
	uint32_t imr        ;
	uint32_t rtr0       ;
	uint32_t rtr1       ;
	uint32_t rcr        ;
	uint32_t rmsr       ;
	uint32_t tmsr       ;
	uint32_t patr0      ;
	uint32_t patr1      ;
	uint32_t ptimer     ;
	uint32_t pmagic     ;
	uint32_t uipr0      ;
	uint32_t uipr1      ;
	uint32_t uipr2      ;
	uint32_t uipr3      ;
	uint32_t uport0     ;
	uint32_t uport1     ;

	// 端口寄存器, 一共4个端口
	struct {
		uint32_t mr      ;
		uint32_t cr      ;
		uint32_t ir      ;
		uint32_t sr      ;
		uint32_t port0   ;
		uint32_t port1   ;
		uint32_t dhar0   ;
		uint32_t dhar1   ;
		uint32_t dhar2   ;
		uint32_t dhar3   ;
		uint32_t dhar4   ;
		uint32_t dhar5   ;
		uint32_t dipr0   ;
		uint32_t dipr1   ;
		uint32_t dipr2   ;
		uint32_t dipr3   ;
		uint32_t dport0  ;
		uint32_t dport1  ;
		uint32_t mssr0   ;
		uint32_t mssr1   ;
		uint32_t proto   ;
		uint32_t tos     ;
		uint32_t ttl     ;
		uint32_t tx_fsr0 ;
		uint32_t tx_fsr1 ;
		uint32_t tx_rd0  ;
		uint32_t tx_rd1  ;
		uint32_t tx_wr0  ;
		uint32_t tx_wr1  ;
		uint32_t rx_rsr0 ;
		uint32_t rx_rsr1 ;
		uint32_t rx_rd0  ;
		uint32_t rx_rd1  ;
	} s[MAX_SOCK_NUM];
};

typedef struct
{
	char MagicIPString[8];
	char IpAddr[4];
	char reserved1[4];
	char MagicMacString[8];
	char MacAddr[6];
	char reserved2[2];
	char MagicDisplayPortString[8];
	char Port[2];
	char reserved3[6];
} DT_ETHERNET_CFG;

struct w5100_end_device_t
{
	conf_object_t* obj;

	core_signal_intf* signal_iface;
	conf_object_t* signal;

	struct registers regs;

	// 网卡配置信息
	DT_ETHERNET_CFG dt_eth_cfg;

	char *rx_buff; // 8k 接收缓存
	char *tx_buff; // 8k 发送缓存

#define BUF_SIZE_MASK 0x3
#define BUF_1K_FLAG   0x0
#define BUF_2K_FLAG   0x1
#define BUF_4K_FLAG   0x2
#define BUF_8K_FLAG   0x3
#define BUF_1K_DEF    0x0400
#define BUF_2K_DEF    0x0800
#define BUF_4K_DEF    0x0C00
#define BUF_8K_DEF    0x1000
	struct {
		uint8_t *rx_buff;  // 1-8k 接收缓存
		uint8_t *tx_buff;  // 1-8k 发送缓存
		uint32_t rx_size;  // 分配的空间大小
		uint32_t tx_size;  // 分配的空间大小

		uint32_t rx_offset; // 接收数据偏移界限
		uint32_t rx_buf_h;  // 环形数据的头部, rx_rsr为环形数组的末尾

		int socket_fd;     // socket描述字
		pthread_t thread_id;
	} end_port[MAX_SOCK_NUM];

	uint8_t rx_tx_memory[0x4000];

	// 设备属性
	uint8_t * W5100_IP;
	uint8_t * W5100_MAC;
	uint32_t display_port;

	// 多线程锁
	RWLOCK_T w5100_end_lock;
};
typedef struct w5100_end_device_t w5100_end_device;

typedef struct {
	w5100_end_device *device_obj;
	uint32_t tmp_port_num; // 创建多线程时, 给线程任务传参
} thread_args_t;

static char* regs_name[] =
{
	"mr",
	"gar0",
	"gar1",
	"gar2",
	"gar3",
	"subr0",
	"subr1",
	"subr2",
	"subr3",
	"shar0",
	"shar1",
	"shar2",
	"shar3",
	"shar4",
	"shar5",
	"sipr0",
	"sipr1",
	"sipr2",
	"sipr3",
	"ir",
	"imr",
	"rtr0",
	"rtr1",
	"rcr",
	"rmsr",
	"tmsr",
	"patr0",
	"patr1",
	"ptimer",
	"pmagic",
	"uipr0",
	"uipr1",
	"uipr2",
	"uipr3",
	"uport0",
	"uport1",
	"s0_mr",
	"s0_cr",
	"s0_ir",
	"s0_sr",
	"s0_port0",
	"s0_port1",
	"s0_dhar0",
	"s0_dhar1",
	"s0_dhar2",
	"s0_dhar3",
	"s0_dhar4",
	"s0_dhar5",
	"s0_dipr0",
	"s0_dipr1",
	"s0_dipr2",
	"s0_dipr3",
	"s0_dport0",
	"s0_dport1",
	"s0_mssr0",
	"s0_mssr1",
	"s0_proto",
	"s0_tos",
	"s0_ttl",
	"s0_tx_fsr0",
	"s0_tx_fsr1",
	"s0_tx_rd0",
	"s0_tx_rd1",
	"s0_tx_wr0",
	"s0_tx_wr1",
	"s0_rx_rsr0",
	"s0_rx_rsr1",
	"s0_rx_rd0",
	"s0_rx_rd1",
	"s1_mr",
	"s1_cr",
	"s1_ir",
	"s1_sr",
	"s1_port0",
	"s1_port1",
	"s1_dhar0",
	"s1_dhar1",
	"s1_dhar2",
	"s1_dhar3",
	"s1_dhar4",
	"s1_dhar5",
	"s1_dipr0",
	"s1_dipr1",
	"s1_dipr2",
	"s1_dipr3",
	"s1_dport0",
	"s1_dport1",
	"s1_mssr0",
	"s1_mssr1",
	"s1_proto",
	"s1_tos",
	"s1_ttl",
	"s1_tx_fsr0",
	"s1_tx_fsr1",
	"s1_tx_rd0",
	"s1_tx_rd1",
	"s1_tx_wr0",
	"s1_tx_wr1",
	"s1_rx_rsr0",
	"s1_rx_rsr1",
	"s1_rx_rd0",
	"s1_rx_rd1",
	"s2_mr",
	"s2_cr",
	"s2_ir",
	"s2_sr",
	"s2_port0",
	"s2_port1",
	"s2_dhar0",
	"s2_dhar1",
	"s2_dhar2",
	"s2_dhar3",
	"s2_dhar4",
	"s2_dhar5",
	"s2_dipr0",
	"s2_dipr1",
	"s2_dipr2",
	"s2_dipr3",
	"s2_dport0",
	"s2_dport1",
	"s2_mssr0",
	"s2_mssr1",
	"s2_proto",
	"s2_tos",
	"s2_ttl",
	"s2_tx_fsr0",
	"s2_tx_fsr1",
	"s2_tx_rd0",
	"s2_tx_rd1",
	"s2_tx_wr0",
	"s2_tx_wr1",
	"s2_rx_rsr0",
	"s2_rx_rsr1",
	"s2_rx_rd0",
	"s2_rx_rd1",
	"s3_mr",
	"s3_cr",
	"s3_ir",
	"s3_sr",
	"s3_port0",
	"s3_port1",
	"s3_dhar0",
	"s3_dhar1",
	"s3_dhar2",
	"s3_dhar3",
	"s3_dhar4",
	"s3_dhar5",
	"s3_dipr0",
	"s3_dipr1",
	"s3_dipr2",
	"s3_dipr3",
	"s3_dport0",
	"s3_dport1",
	"s3_mssr0",
	"s3_mssr1",
	"s3_proto",
	"s3_tos",
	"s3_ttl",
	"s3_tx_fsr0",
	"s3_tx_fsr1",
	"s3_tx_rd0",
	"s3_tx_rd1",
	"s3_tx_wr0",
	"s3_tx_wr1",
	"s3_rx_rsr0",
	"s3_rx_rsr1",
	"s3_rx_rd0",
	"s3_rx_rd1",
	NULL
};

#define MR          0x0000
#define GAR0        0x0001 // 默认网关
#define GAR1        0x0002
#define GAR2        0x0003
#define GAR3        0x0004
#define SUBR0       0x0005 // 子网掩码
#define SUBR1       0x0006
#define SUBR2       0x0007
#define SUBR3       0x0008
#define SHAR0       0x0009 // MAC源硬件地址
#define SHAR1       0x000A
#define SHAR2       0x000B
#define SHAR3       0x000C
#define SHAR4       0x000D
#define SHAR5       0x000E
#define SIPR0       0x000F // 源IP地址
#define SIPR1       0x0010
#define SIPR2       0x0011
#define SIPR3       0x0012
#define IR          0x0015
#define IMR         0x0016
#define RTR0        0x0017
#define RTR1        0x0018
#define RCR         0x0019
#define RMSR        0x001A
#define TMSR        0x001B
#define PATR0       0x001C
#define PATR1       0x001D
#define PTIMER      0x0028
#define PMAGIC      0x0029
#define UIPR0       0x002A
#define UIPR1       0x002B
#define UIPR2       0x002C
#define UIPR3       0x002D
#define UPORT0      0x002E
#define UPORT1      0x002F

// 通用端口地址偏移值
#define Sn_MR       0x0000
#define Sn_CR       0x0001
#define Sn_IR       0x0002
#define Sn_SR       0x0003
#define Sn_PORT0    0x0004
#define Sn_PORT1    0x0005
#define Sn_DHAR0    0x0006
#define Sn_DHAR1    0x0007
#define Sn_DHAR2    0x0008
#define Sn_DHAR3    0x0009
#define Sn_DHAR4    0x000A
#define Sn_DHAR5    0x000B
#define Sn_DIPR0    0x000C
#define Sn_DIPR1    0x000D
#define Sn_DIPR2    0x000E
#define Sn_DIPR3    0x000F
#define Sn_DPORT0   0x0010
#define Sn_DPORT1   0x0011
#define Sn_MSSR0    0x0012
#define Sn_MSSR1    0x0013
#define Sn_PROTO    0x0014
#define Sn_TOS      0x0015
#define Sn_TTL      0x0016
#define Sn_TX_FSR0  0x0020
#define Sn_TX_FSR1  0x0021
#define Sn_TX_RD0   0x0022
#define Sn_TX_RD1   0x0023
#define Sn_TX_WR0   0x0024
#define Sn_TX_WR1   0x0025
#define Sn_RX_RSR0  0x0026
#define Sn_RX_RSR1  0x0027
#define Sn_RX_RD0   0x0028
#define Sn_RX_RD1   0x0029

#endif
