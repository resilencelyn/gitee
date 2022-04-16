#ifndef __loongson_PWM_H__
#define __loongson_PWM_H__

// All equipment register names, the list order and struct registers registers inside order is kept consistent
static char* regs_name[] =
{
	"pwm_cfg",
	"pwm_intclr",
	NULL
};

// Register offset
#define PWM_CFG     0x0
#define PWM_INTCLR  0x4

// device register structure
struct registers
{
	uint32_t pwm_cfg;
	uint32_t pwm_intclr;
};


// Device object structure
struct loongson_pwm_device_t
{
	
	conf_object_t* obj;
	
	// Interface objects and methods;
	
	struct registers regs;
	
};
typedef struct loongson_pwm_device_t loongson_pwm_device;

#endif
