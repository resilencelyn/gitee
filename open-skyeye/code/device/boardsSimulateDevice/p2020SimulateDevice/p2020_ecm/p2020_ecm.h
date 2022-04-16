#ifndef __P2020_ECM_H__
#define __P2020_ECM_H__

struct registers
{
	uint32_t eebacr;
	uint32_t eebpcr;
	uint32_t eipbrr1;
	uint32_t eipbrr2;
	uint32_t eedr;
	uint32_t eeer;
	uint32_t eeatr;
	uint32_t eeladr;
	uint32_t eehadr;
};

struct p2020_ecm_device_t
{
	conf_object_t* obj;
	struct
	{
		core_exec_intf *iface;
		conf_object_t *obj;
	}core_iface[2];

	struct registers regs;
};

typedef struct p2020_ecm_device_t p2020_ecm_device;

#define EEBACR              0x00
#define EEBPCR              0x10
#define EIPBRR1             0xBF8
#define EIPBRR2             0xBFC
#define EEDR                0xE00
#define EEER                0xE08
#define EEATR               0xE0c
#define EELADR              0xE10
#define EEHADR              0xE14

static char* regs_name[] =
{
	"eebacr",
	"eebpcr",
	"eipbrr1",
	"eipbrr2",
	"eedr",
	"eeer",
	"eeatr",
	"eeladr",
	"eehadr",
	NULL
};

static int regs_offset[] =
{
	EEBACR,
	EEBPCR,
	EIPBRR1,
	EIPBRR2,
	EEDR,
	EEER,
	EEATR,
	EELADR,
	EEHADR,
};

#endif
