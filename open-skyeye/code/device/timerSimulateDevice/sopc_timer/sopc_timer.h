#ifndef __SOPC_TIMER_H__
#define __SOPC_TIMER_H__

typedef struct __timer_reg
{
	uint32_t TimerLoadCount;
	uint32_t TimerCurrentValue;
	uint32_t TimerControlReg;
	uint32_t TimerEOI;
	uint32_t TimerIntStatus;
}timer_reg;

struct registers
{
	uint32_t TimersIntStatus;
	uint32_t TimersEOI;
	uint32_t TimersRawIntStatus;
	uint32_t TimersCompVersion;
	timer_reg timer[4];
};

struct sopc_timer_device_t
{
	conf_object_t* obj;
	struct {
		int                     num;
		conf_object_t          *obj;
		general_signal_intf    *intf;
	}signal;
	struct registers regs;
	int time;
	int stepCount;
};

typedef struct sopc_timer_device_t sopc_timer_device;

#define LOAD_COUNT_REG       0x0
#define CURRENT_VALUE_REG    0x4
#define CONTROL_REG          0x8
#define EOI_REG              0xc
#define INT_STATUS_REG       0x10

/*Timer Registers Base--0x68a00000 Offset*/
#define TIMERIntStatus       0xa0
#define TIMEREOI             0xa4
#define TIMERRawIntStatus    0xa8
#define TIMERS_COMP_VERSION  0xac

#define Timer1LoadCount      0x00
#define Timer1CurrentValue   0x04
#define Timer1ControlReg     0x08
#define Timer1EOI            0x0c
#define Timer1IntStatus      0x10

#define Timer2LoadCount      0x14
#define Timer2CurrentValue   0x18
#define Timer2ControlReg     0x1c
#define Timer2EOI            0x20
#define Timer2IntStatus      0x24

#define Timer3LoadCount      0x28
#define Timer3CurrentValue   0x2c
#define Timer3ControlReg     0x30
#define Timer3EOI            0x34
#define Timer3IntStatus      0x38

#define Timer4LoadCount      0x3c
#define Timer4CurrentValue   0x40
#define Timer4ControlReg     0x44
#define Timer4EOI            0x48
#define Timer4IntStatus      0x4c

#define TIMER_END_ADDR       0x50
static char* regs_name[] =
{
	"TimerIntStatus",
	"TimerEOI",
	"TimersRawIntStatus",
	"TIMERS_COMP_VERSION",
	"Timer1LoadCount",
	"Timer1CurrentValue",
	"Timer1ControlReg",
	"Timer1EOI",
	"Timer1IntStatus",
	"Timer2LoadCount",
	"Timer2CurrentValue",
	"Timer2ControlReg",
	"Timer2EOI",
	"Timer2IntStatus",
	"Timer3LoadCount",
	"Timer3CurrentValue",
	"Timer3ControlReg",
	"Timer3EOI",
	"Timer3IntStatus",
	"Timer4LoadCount",
	"Timer4CurrentValue",
	"Timer4ControlReg",
	"Timer4EOI",
	"Timer4IntStatus",
	NULL
};

static int regs_offset[] =
{
	TIMERIntStatus,
	TIMEREOI,
	TIMERRawIntStatus,
	TIMERS_COMP_VERSION,
	Timer1LoadCount,
	Timer1CurrentValue,
	Timer1ControlReg,
	Timer1EOI,
	Timer1IntStatus,
	Timer2LoadCount,
	Timer2CurrentValue,
	Timer2ControlReg,
	Timer2EOI,
	Timer2IntStatus,
	Timer3LoadCount,
	Timer3CurrentValue,
	Timer3ControlReg,
	Timer3EOI,
	Timer3IntStatus,
	Timer4LoadCount,
	Timer4CurrentValue,
	Timer4ControlReg,
	Timer4EOI,
	Timer4IntStatus,
};

#define TIMER_ENABLE    1
#define TIMER_INT_MASK  4

#define TIMER_INT_NUM   11

/*system configuration */
#define CONFIG_AHB_CLK						(100000000) // system ahb clock rate
#define CONFIG_APB_CLK						(CONFIG_AHB_CLK/2) // system apb clock rate
#endif
