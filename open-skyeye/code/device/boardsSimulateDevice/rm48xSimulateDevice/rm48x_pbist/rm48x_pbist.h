#ifndef __RM48X_PBIST_H__ 
#define __RM48X_PBIST_H__ 
struct registers 
{
	uint32_t reserve0[58];
	uint32_t ramt;

	uint32_t dlr;
	uint32_t reserve1[6];
	uint32_t pact;

	uint32_t pbistid;

	uint32_t over;
	uint32_t reserve2;
	uint32_t fsrf0;
	uint32_t reserve5;
	uint32_t fsrc0;

	uint32_t fsrc1;

	uint32_t fsra0;

	uint32_t fsra1;

	uint32_t fsrdl0;
	uint32_t reserve3;
	uint32_t fsrdl1;
	uint32_t reserve4[3];
	uint32_t rom;

	uint32_t algo;

	uint32_t rinfol;

	uint32_t rinfou;
};


struct rm48x_pbist_device_t 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct rm48x_pbist_device_t rm48x_pbist_device;
static char* regs_name[] = 
{
	"RESERVE0",
	"RAMT",
	"DLR",
	"RESERVE1",
	"PACT",
	"PBISTID",
	"OVER",
	"RESERVE2",
	"FSRF0",
	"RESERVE5",
	"FSRC0",
	"FSRC1",
	"FSRA0",
	"FSRA1",
	"FSRDL0",
	"RESERVE3",
	"FSRDL1",
	"RESERVE4",
	"ROM",
	"ALGO",
	"RINFOL",
	"RINFOU",
	NULL
};
#define RESERVE0 0x0
#define RAMT 0x160
#define DLR 0x164
#define RESERVE1 0x168
#define PACT 0x180
#define PBISTID 0x184
#define OVER 0x188
#define RESERVE2 0x18c
#define FSRF0 0x190
#define RESERVE5 0x194
#define FSRC0 0x198
#define FSRC1 0x19c
#define FSRA0 0x1a0
#define FSRA1 0x1a4
#define FSRDL0 0x1a8
#define RESERVE3 0x1ac
#define FSRDL1 0x1b0
#define RESERVE4 0x1b4
#define ROM 0x1c0
#define ALGO 0x1c4
#define RINFOL 0x1c8
#define RINFOU 0x1cc
#endif  
