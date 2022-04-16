#ifndef __P2020_L2_CACHE_H__
#define __P2020_L2_CACHE_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"L2CTL",
	"L2CEWAR0",
	"L2CEWCR0",
	"L2CEWAR1",
	"L2CEWCR1",
	"L2CEWAR2",
	"L2CEWCR2",
	"L2CEWAR3",
	"L2CEWCR3",
	NULL
};

// Register offset
#define L2_Cache_L2CTL 		0x00
#define L2_Cache_L2CEWAR0 	0x10
#define L2_Cache_L2CEWCR0 	0x18
#define L2_Cache_L2CEWAR1 	0x20
#define L2_Cache_L2CEWCR1 	0x28
#define L2_Cache_L2CEWAR2 	0x30
#define L2_Cache_L2CEWCR2 	0x38
#define L2_Cache_L2CEWAR3 	0x40
#define L2_Cache_L2CEWCR3 	0x48

#define L2_CTL_L2I 			30
#define L2_CTL_INIT			0x20000000
#define L2_Status_CLEAR		0x1
#define L2_Status_NotCLEAR	0x0

// device register structure
struct registers
{
	uint32_t L2CTL;
	uint32_t L2CEWAR0;
	uint32_t L2CEWCR0;
	uint32_t L2CEWAR1;
	uint32_t L2CEWCR1;
	uint32_t L2CEWAR2;
	uint32_t L2CEWCR2;
	uint32_t L2CEWAR3;
	uint32_t L2CEWCR3;
};


// Device object structure
struct p2020_L2_cache_device_t
{
	
	conf_object_t* obj;
	
	// Interface objects and methods;
	
	struct registers regs;
	
};
typedef struct p2020_L2_cache_device_t p2020_L2_cache_device;

#endif
