#ifndef __HTEY_GPS_H__ 
#define __HTEY_GPS_H__ 
struct registers 
{
	
	uint32_t gps_tx_rx;
	
	uint32_t gps_ier;
	
	uint32_t gps_ifr;
	
	uint32_t gps_lcr;
	
	uint32_t gps_fcr;
	
	uint32_t gps_lsr;
	
	uint32_t gps_bdrh;
	
	uint32_t gps_bdrl;
	
	uint32_t gps_en;
	
	uint32_t gps_cnt;
	
};


struct GPS_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct GPS_device htey_GPS_device;
static char* regs_name[] = 
{
	"gps_tx_rx",
	"gps_ier",
	"gps_ifr",
	"gps_lcr",
	"gps_fcr",
	"gps_lsr",
	"gps_bdrh",
	"gps_bdrl",
	"gps_en",
	"gps_cnt",
	NULL
};
#define GPS_TX_RX 0x0
#define GPS_IER 0x10
#define GPS_IFR 0x20
#define GPS_LCR 0x30
#define GPS_FCR 0x40
#define GPS_LSR 0x50
#define GPS_BDRH 0x60
#define GPS_BDRL 0x70
#define GPS_EN 0x80
#define GPS_CNT 0x100
#endif  
