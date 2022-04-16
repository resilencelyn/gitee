#ifndef __ARMR4_MPU_TEST_H__
#define __ARMR4_MPU_TEST_H__

struct registers
{
	uint32_t reg1;
	uint32_t reg2;
	uint32_t reg3;
	uint32_t reg4;
};

struct armr4_mpu_test_device_t
{
	conf_object_t* obj;

	struct registers regs;

	uint32_t test_mode;
};
typedef struct armr4_mpu_test_device_t armr4_mpu_test_device;

static char* regs_name[] =
{
	"reg1",
	"reg2",
	"reg3",
	"reg4",
	NULL
};

#define REG1 0x0
#define REG2 0x4
#define REG3 0x8
#define REG4 0xC
#endif
