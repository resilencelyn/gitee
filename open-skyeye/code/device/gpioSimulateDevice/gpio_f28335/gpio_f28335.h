/* Copyright (C) 
* 2011 - Michael.Kang blackfin.kang@gmail.com
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
* @file gpio_f28335.h
* @brief The definition of system controller for f28335
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __SYSCTRL_F28335_H__
#define __SYSCTRL_F28335_H__

//----------------------------------------------------
// GPIO A control register bit definitions */
struct GPACTRL_BITS {        // bits   description
   uint16_t QUALPRD0:8;        // 7:0    Qual period
   uint16_t QUALPRD1:8;        // 15:8   Qual period
   uint16_t QUALPRD2:8;        // 23:16  Qual period
   uint16_t QUALPRD3:8;        // 31:24  Qual period
};

union GPACTRL_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPA_LSB;
			uint16_t GPA_MSB;
		};
	};
   struct GPACTRL_BITS bit;
};

//----------------------------------------------------
// GPIO B control register bit definitions */
struct GPBCTRL_BITS {        // bits   description
   uint16_t QUALPRD0:8;        // 7:0    Qual period
   uint16_t QUALPRD1:8;        // 15:8   Qual period
   uint16_t QUALPRD2:8;        // 23:16  Qual period
   uint16_t QUALPRD3:8;        // 31:24
};

union GPBCTRL_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPB_LSB;
			uint16_t GPB_MSB;
		};
	};
   struct GPBCTRL_BITS bit;
};

//----------------------------------------------------
// GPIO A Qual/MUX select register bit definitions */
struct GPA1_BITS {            // bits   description
   uint16_t GPIO0:2;            // 1:0    GPIO0
   uint16_t GPIO1:2;            // 3:2    GPIO1
   uint16_t GPIO2:2;            // 5:4    GPIO2
   uint16_t GPIO3:2;            // 7:6    GPIO3
   uint16_t GPIO4:2;            // 9:8    GPIO4
   uint16_t GPIO5:2;            // 11:10  GPIO5
   uint16_t GPIO6:2;            // 13:12  GPIO6
   uint16_t GPIO7:2;            // 15:14  GPIO7
   uint16_t GPIO8:2;            // 17:16  GPIO8
   uint16_t GPIO9:2;            // 19:18  GPIO9
   uint16_t GPIO10:2;           // 21:20  GPIO10
   uint16_t GPIO11:2;           // 23:22  GPIO11
   uint16_t GPIO12:2;           // 25:24  GPIO12
   uint16_t GPIO13:2;           // 27:26  GPIO13
   uint16_t GPIO14:2;           // 29:28  GPIO14
   uint16_t GPIO15:2;           // 31:30  GPIO15
};


struct GPA2_BITS {            // bits   description
   uint16_t GPIO16:2;           // 1:0    GPIO16
   uint16_t GPIO17:2;           // 3:2    GPIO17
   uint16_t GPIO18:2;           // 5:4    GPIO18
   uint16_t GPIO19:2;           // 7:6    GPIO19
   uint16_t GPIO20:2;           // 9:8    GPIO20
   uint16_t GPIO21:2;           // 11:10  GPIO21
   uint16_t GPIO22:2;           // 13:12  GPIO22
   uint16_t GPIO23:2;           // 15:14  GPIO23
   uint16_t GPIO24:2;           // 17:16  GPIO24
   uint16_t GPIO25:2;           // 19:18  GPIO25
   uint16_t GPIO26:2;           // 21:20  GPIO26
   uint16_t GPIO27:2;           // 23:22  GPIO27
   uint16_t GPIO28:2;           // 25:24  GPIO28
   uint16_t GPIO29:2;           // 27:26  GPIO29
   uint16_t GPIO30:2;           // 29:28  GPIO30
   uint16_t GPIO31:2;           // 31:30  GPIO31
};

struct GPB1_BITS {            // bits   description
   uint16_t GPIO32:2;           // 1:0    GPIO32
   uint16_t GPIO33:2;           // 3:2    GPIO33
   uint16_t GPIO34:2;           // 5:4    GPIO34
   uint16_t GPIO35:2;           // 7:6    GPIO35
   uint16_t GPIO36:2;           // 9:8    GPIO36
   uint16_t GPIO37:2;           // 11:10  GPIO37
   uint16_t GPIO38:2;           // 13:12  GPIO38
   uint16_t GPIO39:2;           // 15:14  GPIO39
   uint16_t GPIO40:2;           // 17:16  GPIO40
   uint16_t GPIO41:2;           // 19:16  GPIO41
   uint16_t GPIO42:2;           // 21:20  GPIO42
   uint16_t GPIO43:2;           // 23:22  GPIO43
   uint16_t GPIO44:2;           // 25:24  GPIO44
   uint16_t GPIO45:2;           // 27:26  GPIO45
   uint16_t GPIO46:2;           // 29:28  GPIO46
   uint16_t GPIO47:2;           // 31:30  GPIO47
};

struct GPB2_BITS  {           // bits   description
   uint16_t GPIO48:2;           // 1:0    GPIO48
   uint16_t GPIO49:2;           // 3:2    GPIO49
   uint16_t GPIO50:2;           // 5:4    GPIO50
   uint16_t GPIO51:2;           // 7:6    GPIO51
   uint16_t GPIO52:2;           // 9:8    GPIO52
   uint16_t GPIO53:2;           // 11:10  GPIO53
   uint16_t GPIO54:2;           // 13:12  GPIO54
   uint16_t GPIO55:2;           // 15:14  GPIO55
   uint16_t GPIO56:2;           // 17:16  GPIO56
   uint16_t GPIO57:2;           // 19:18  GPIO57
   uint16_t GPIO58:2;           // 21:20  GPIO58
   uint16_t GPIO59:2;           // 23:22  GPIO59
   uint16_t GPIO60:2;           // 25:24  GPIO60
   uint16_t GPIO61:2;           // 27:26  GPIO61
   uint16_t GPIO62:2;           // 29:28  GPIO62
   uint16_t GPIO63:2;           // 31:30  GPIO63
};

struct GPC1_BITS {            // bits   description
   uint16_t GPIO64:2;           // 1:0    GPIO64
   uint16_t GPIO65:2;           // 3:2    GPIO65
   uint16_t GPIO66:2;           // 5:4    GPIO66
   uint16_t GPIO67:2;           // 7:6    GPIO67
   uint16_t GPIO68:2;           // 9:8    GPIO68
   uint16_t GPIO69:2;           // 11:10  GPIO69
   uint16_t GPIO70:2;           // 13:12  GPIO70
   uint16_t GPIO71:2;           // 15:14  GPIO71
   uint16_t GPIO72:2;           // 17:16  GPIO72
   uint16_t GPIO73:2;           // 19:18  GPIO73
   uint16_t GPIO74:2;           // 21:20  GPIO74
   uint16_t GPIO75:2;           // 23:22  GPIO75
   uint16_t GPIO76:2;           // 25:24  GPIO76
   uint16_t GPIO77:2;           // 27:26  GPIO77
   uint16_t GPIO78:2;           // 29:28  GPIO78
   uint16_t GPIO79:2;           // 31:30  GPIO79
};


struct GPC2_BITS {            // bits   description
   uint16_t GPIO80:2;           // 1:0    GPIO80
   uint16_t GPIO81:2;           // 3:2    GPIO81
   uint16_t GPIO82:2;           // 5:4    GPIO82
   uint16_t GPIO83:2;           // 7:6    GPIO83
   uint16_t GPIO84:2;           // 9:8    GPIO84
   uint16_t GPIO85:2;           // 11:10  GPIO85
   uint16_t GPIO86:2;           // 13:12  GPIO86
   uint16_t GPIO87:2;           // 15:14  GPIO87
   uint16_t rsvd:16;            // 31:16  reserved
};


union GPA1_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPA1_LSB;
			uint16_t GPA1_MSB;
		};
	};
   struct GPA1_BITS    bit;
};

union GPA2_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPA2_LSB;
			uint16_t GPA2_MSB;
		};
	};
   struct GPA2_BITS    bit;
};

union GPB1_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPB1_LSB;
			uint16_t GPB1_MSB;
		};
	};
   struct GPB1_BITS    bit;
};

union GPB2_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPB2_LSB;
			uint16_t GPB2_MSB;
		};
	};
   struct GPB2_BITS    bit;
};

union GPC1_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPC1_LSB;
			uint16_t GPC1_MSB;
		};
	};
   struct GPC1_BITS    bit;
};

union GPC2_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPC2_LSB;
			uint16_t GPC2_MSB;
		};
	};
   struct GPC2_BITS    bit;
};

//----------------------------------------------------
// GPIO A DIR/TOGGLE/SET/CLEAR register bit definitions */
struct GPADAT_BITS {          // bits   description
   uint16_t GPIO0:1;            // 0      GPIO0
   uint16_t GPIO1:1;            // 1      GPIO1
   uint16_t GPIO2:1;            // 2      GPIO2
   uint16_t GPIO3:1;            // 3      GPIO3
   uint16_t GPIO4:1;            // 4      GPIO4
   uint16_t GPIO5:1;            // 5      GPIO5
   uint16_t GPIO6:1;            // 6      GPIO6
   uint16_t GPIO7:1;            // 7      GPIO7
   uint16_t GPIO8:1;            // 8      GPIO8
   uint16_t GPIO9:1;            // 9      GPIO9
   uint16_t GPIO10:1;           // 10     GPIO10
   uint16_t GPIO11:1;           // 11     GPIO11
   uint16_t GPIO12:1;           // 12     GPIO12
   uint16_t GPIO13:1;           // 13     GPIO13
   uint16_t GPIO14:1;           // 14     GPIO14
   uint16_t GPIO15:1;           // 15     GPIO15
   uint16_t GPIO16:1;           // 16     GPIO16
   uint16_t GPIO17:1;           // 17     GPIO17
   uint16_t GPIO18:1;           // 18     GPIO18
   uint16_t GPIO19:1;           // 19     GPIO19
   uint16_t GPIO20:1;           // 20     GPIO20
   uint16_t GPIO21:1;           // 21     GPIO21
   uint16_t GPIO22:1;           // 22     GPIO22
   uint16_t GPIO23:1;           // 23     GPIO23
   uint16_t GPIO24:1;           // 24     GPIO24
   uint16_t GPIO25:1;           // 25     GPIO25
   uint16_t GPIO26:1;           // 26     GPIO26
   uint16_t GPIO27:1;           // 27     GPIO27
   uint16_t GPIO28:1;           // 28     GPIO28
   uint16_t GPIO29:1;           // 29     GPIO29
   uint16_t GPIO30:1;           // 30     GPIO30
   uint16_t GPIO31:1;           // 31     GPIO31
};

struct GPBDAT_BITS {          // bits   description
   uint16_t GPIO32:1;           // 0      GPIO32
   uint16_t GPIO33:1;           // 1      GPIO33
   uint16_t GPIO34:1;           // 2      GPIO34
   uint16_t GPIO35:1;           // 3      GPIO35
   uint16_t GPIO36:1;           // 4      GPIO36
   uint16_t GPIO37:1;           // 5      GPIO37
   uint16_t GPIO38:1;           // 6      GPIO38
   uint16_t GPIO39:1;           // 7      GPIO39
   uint16_t GPIO40:1;           // 8      GPIO40
   uint16_t GPIO41:1;           // 9      GPIO41
   uint16_t GPIO42:1;           // 10     GPIO42
   uint16_t GPIO43:1;           // 11     GPIO43
   uint16_t GPIO44:1;           // 12     GPIO44
   uint16_t GPIO45:1;           // 13     GPIO45
   uint16_t GPIO46:1;           // 14     GPIO46
   uint16_t GPIO47:1;           // 15     GPIO47
   uint16_t GPIO48:1;           // 16     GPIO48
   uint16_t GPIO49:1;           // 17     GPIO49
   uint16_t GPIO50:1;           // 18     GPIO50
   uint16_t GPIO51:1;           // 19     GPIO51
   uint16_t GPIO52:1;           // 20     GPIO52
   uint16_t GPIO53:1;           // 21     GPIO53
   uint16_t GPIO54:1;           // 22     GPIO54
   uint16_t GPIO55:1;           // 23     GPIO55
   uint16_t GPIO56:1;           // 24     GPIO56
   uint16_t GPIO57:1;           // 25     GPIO57
   uint16_t GPIO58:1;           // 26     GPIO58
   uint16_t GPIO59:1;           // 27     GPIO59
   uint16_t GPIO60:1;           // 28     GPIO60
   uint16_t GPIO61:1;           // 29     GPIO61
   uint16_t GPIO62:1;           // 30     GPIO62
   uint16_t GPIO63:1;           // 31     GPIO63
};

struct GPCDAT_BITS {          // bits   description
   uint16_t GPIO64:1;           // 0      GPIO64
   uint16_t GPIO65:1;           // 1      GPIO65
   uint16_t GPIO66:1;           // 2      GPIO66
   uint16_t GPIO67:1;           // 3      GPIO67
   uint16_t GPIO68:1;           // 4      GPIO68
   uint16_t GPIO69:1;           // 5      GPIO69
   uint16_t GPIO70:1;           // 6      GPIO70
   uint16_t GPIO71:1;           // 7      GPIO71
   uint16_t GPIO72:1;           // 8      GPIO72
   uint16_t GPIO73:1;           // 9      GPIO73
   uint16_t GPIO74:1;           // 10     GPIO74
   uint16_t GPIO75:1;           // 11     GPIO75
   uint16_t GPIO76:1;           // 12     GPIO76
   uint16_t GPIO77:1;           // 13     GPIO77
   uint16_t GPIO78:1;           // 14     GPIO78
   uint16_t GPIO79:1;           // 15     GPIO79
   uint16_t GPIO80:1;           // 16     GPIO80
   uint16_t GPIO81:1;           // 17     GPIO81
   uint16_t GPIO82:1;           // 18     GPIO82
   uint16_t GPIO83:1;           // 19     GPIO83
   uint16_t GPIO84:1;           // 20     GPIO84
   uint16_t GPIO85:1;           // 21     GPIO85
   uint16_t GPIO86:1;           // 22     GPIO86
   uint16_t GPIO87:1;           // 23     GPIO87
   uint16_t rsvd1:8;            // 31:24  reserved
};


union GPADAT_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPA_LSB;
			uint16_t GPA_MSB;
		};
	};
   struct GPADAT_BITS  bit;
};

union GPBDAT_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPB_LSB;
			uint16_t GPB_MSB;
		};
	};
   struct GPBDAT_BITS  bit;
};

union GPCDAT_REG {
	union{
   		uint32_t              all;
		struct {
			uint16_t GPC_LSB;
			uint16_t GPC_MSB;
		};
	};
   struct GPCDAT_BITS  bit;
};

//----------------------------------------------------
// GPIO Xint1/XINT2/XNMI select register bit definitions */
struct GPIOXINT_BITS {        // bits   description
    uint16_t GPIOSEL:5;         // 4:0    Select GPIO interrupt input source
    uint16_t rsvd1:11;          // 15:5   reserved
};

union GPIOXINT_REG {
   uint16_t                all;
   struct GPIOXINT_BITS  bit;
};


typedef struct GPIO_REGS {
   union  GPACTRL_REG  GPACTRL;   // GPIO A Control Register (GPIO0 to 31)
   union  GPA1_REG     GPAQSEL1;  // GPIO A Qualifier Select 1 Register (GPIO0 to 15)
   union  GPA2_REG     GPAQSEL2;  // GPIO A Qualifier Select 2 Register (GPIO16 to 31)
   union  GPA1_REG     GPAMUX1;   // GPIO A Mux 1 Register (GPIO0 to 15)
   union  GPA2_REG     GPAMUX2;   // GPIO A Mux 2 Register (GPIO16 to 31)
   union  GPADAT_REG   GPADIR;    // GPIO A Direction Register (GPIO0 to 31)
   union  GPADAT_REG   GPAPUD;    // GPIO A Pull Up Disable Register (GPIO0 to 31)
   union  GPBCTRL_REG  GPBCTRL;   // GPIO B Control Register (GPIO32 to 63)
   union  GPB1_REG     GPBQSEL1;  // GPIO B Qualifier Select 1 Register (GPIO32 to 47)
   union  GPB2_REG     GPBQSEL2;  // GPIO B Qualifier Select 2 Register (GPIO48 to 63)
   union  GPB1_REG     GPBMUX1;   // GPIO B Mux 1 Register (GPIO32 to 47)
   union  GPB2_REG     GPBMUX2;   // GPIO B Mux 2 Register (GPIO48 to 63)
   union  GPBDAT_REG   GPBDIR;    // GPIO B Direction Register (GPIO32 to 63)
   union  GPBDAT_REG   GPBPUD;    // GPIO B Pull Up Disable Register (GPIO32 to 63)
   union  GPC1_REG     GPCMUX1;   // GPIO C Mux 1 Register (GPIO64 to 79)
   union  GPC2_REG     GPCMUX2;   // GPIO C Mux 2 Register (GPIO80 to 95)
   union  GPCDAT_REG   GPCDIR;    // GPIO C Direction Register (GPIO64 to 95)
   union  GPCDAT_REG   GPCPUD;    // GPIO C Pull Up Disable Register (GPIO64 to 95)
   union  GPADAT_REG   GPADAT;       // GPIO Data Register (GPIO0 to 31)
   union  GPADAT_REG   GPASET;       // GPIO Data Set Register (GPIO0 to 31)
   union  GPADAT_REG   GPACLEAR;     // GPIO Data Clear Register (GPIO0 to 31)
   union  GPADAT_REG   GPATOGGLE;    // GPIO Data Toggle Register (GPIO0 to 31)
   union  GPBDAT_REG   GPBDAT;       // GPIO Data Register (GPIO32 to 63)
   union  GPBDAT_REG   GPBSET;       // GPIO Data Set Register (GPIO32 to 63)
   union  GPBDAT_REG   GPBCLEAR;     // GPIO Data Clear Register (GPIO32 to 63)
   union  GPBDAT_REG   GPBTOGGLE;    // GPIO Data Toggle Register (GPIO32 to 63)
   union  GPCDAT_REG   GPCDAT;       // GPIO Data Register (GPIO64 to 95)
   union  GPCDAT_REG   GPCSET;       // GPIO Data Set Register (GPIO64 to 95)
   union  GPCDAT_REG   GPCCLEAR;     // GPIO Data Clear Register (GPIO64 to 95)
   union  GPCDAT_REG   GPCTOGGLE;    // GPIO Data Toggle Register (GPIO64 to 95)
   union  GPIOXINT_REG   GPIOXINT1SEL; // XINT1 GPIO Input Selection
   union  GPIOXINT_REG   GPIOXINT2SEL; // XINT2 GPIO Input Selection
   union  GPIOXINT_REG   GPIOXNMISEL;  // XNMI_Xint13 GPIO Input Selection
   union  GPIOXINT_REG   GPIOXINT3SEL; // XINT3 GPIO Input Selection
   union  GPIOXINT_REG   GPIOXINT4SEL; // XINT4 GPIO Input Selection
   union  GPIOXINT_REG   GPIOXINT5SEL; // XINT5 GPIO Input Selection
   union  GPIOXINT_REG   GPIOXINT6SEL; // XINT6 GPIO Input Selection
   union  GPIOXINT_REG   GPIOXINT7SEL; // XINT7 GPIO Input Selection
   union  GPADAT_REG     GPIOLPMSEL;   // Low power modes GP I/O input select
}gpio_reg_t;
static char* regs_name[] = {
	"GPACTRL_LOW",
	"GPACTRL_HIGH",
	"GPAQSEL1_LOW",
	"GPAQSEL1_HIGH",
	"GPAQSEL2_LOW",
	"GPAQSEL2_HIGH",
	"GPAMUX1_LOW",
	"GPAMUX1_HIGH",
	"GPAMUX2_LOW",
	"GPAMUX2_HIGH",
	"GPADIR_LOW",
	"GPADIR_HIGH",
	"GPAPUD_LOW",
	"GPAPUD_HIGH",
	"GPBCTRL_LOW",    //0x6f90
	"GPBCTRL_HIGH",    //0x6f90
	"GPBQSEL1_LOW",
	"GPBQSEL1_HIGH",
	"GPBQSEL2_LOW",
	"GPBQSEL2_HIGH",
	"GPBMUX1_LOW",
	"GPBMUX1_HIGH",
	"GPBMUX2_LOW",
	"GPBMUX2_HIGH",
	"GPBDIR_LOW",
	"GPBDIR_HIGH",
	"GPBPUD_LOW",
	"GPBPUD_HIGH",

	"GPCMUX1_LOW",   
	"GPCMUX1_HIGH",   
	"GPCMUX2_LOW",
	"GPCMUX2_HIGH",
	"GPCDIR_LOW",
	"GPCDIR_HIGH",
	"GPCPUD_LOW",
	"GPCPUD_HIGH",
	"GPADAT_LOW",    
	"GPADAT_HIGH",    
	"GPASET_LOW",
	"GPASET_HIGH",
	"GPACLEAR_LOW",
	"GPACLEAR_HIGH",
	"GPATOGGLE_LOW",
	"GPATOGGLE_HIGH",

	"GPBDAT_LOW",
	"GPBDAT_HIGH",
	"GPBSET_LOW",
	"GPBSET_HIGH",
	"GPBCLEAR_LOW",
	"GPBCLEAR_HIGH",
	"GPBTOGGLE_LOW",
	"GPBTOGGLE_HIGH",

	"GPCDAT_LOW",
	"GPCDAT_HIGH",
	"GPCSET_LOW",
	"GPCSET_HIGH",
	"GPCCLEAR_LOW",
	"GPCCLEAR_HIGH",
	"GPCTOGGLE_LOW",
	"GPCTOGGLE_HIGH",

	"GPIOXINT1",   
	"GPIOXINT2",   
	"GPIOXNMI",
	"GPIOXINT3",   
	"GPIOXINT4",   
	"GPIOXINT5",   
	"GPIOXINT6",   
	"GPIOXINT7",   
	"GPIOLPM",   
	NULL
};
#define MAX_DEVICE_NUM 38

#define MAX_GPIO_NUM 88
typedef struct {
	int high_pin;
	int low_pin;
	int mode;
	int wake_up;
}gpio_group_t;

typedef struct{
	gpio_connect_device_intf* connector_device_iface;
	conf_object_t* dev;
}gpio_connect_dev_t;

typedef struct{
	gpio_connect_dev_t device[MAX_DEVICE_NUM];
	int dev_num;
}gpio_connect_dev_list_t;

typedef struct f28335_gpio_device{
	conf_object_t* obj;
	gpio_reg_t* regs;
	conf_object_t* gpio_connector;
	skyeye_gpio_connector_intf* connector_iface; 

	general_signal_intf *signal_iface;
	conf_object_t* signal;

	gpio_connect_dev_list_t dev_list;
	conf_object_t *gpio_gate_a;
	skyeye_gate_a_intf *gate_a_iface;
	conf_object_t *gpio_gate_b;
	skyeye_gate_b_intf *gate_b_iface;

	gpio_group_t gpio_group[MAX_GPIO_NUM];
	char channel[8];
	int cnt;
	char** regs_name;
	uint32_t pin_num;
	char *circuit_display;
	RWLOCK_T lock;

	conf_object_t* gpio_link;
    	gpio_linker_pin_intf* gpio_linker_intf;
	char *connect_list;
    	int connect_quanaity;
    	int connect_pin[128];
}f28335_gpio_device;

#define HIGH 1
#define LOW  0

#define OUTPUT_MODE 1
#define INPUT_MODE  0

static uint32_t regs_offset[] = {
	0x0000,
	0x0001,
	0x0002,
	0x0003,
	0x0004,
	0x0005,
	0x0006,
	0x0007,
	0x0008,
	0x0009,
	0x000a,
	0x000b,
	0x000c,
	0x000d,
	0x0010,
	0x0011,
	0x0012,
	0x0013,
	0x0014,
	0x0015,
	0x0016,
	0x0017,
	0x0018,
	0x0019,
	0x001a,
	0x001b,
	0x001c,
	0x001d,
	0x0026,
	0x0027,
	0x0028,
	0x0029,
	0x002a,
	0x002b,
	0x002c,
	0x002d,
	0x0040,
	0x0041,
	0x0042,
	0x0043,
	0x0044,
	0x0045,
	0x0046,
	0x0047,
	0x0048,
	0x0049,
	0x004a,
	0x004b,
	0x004c,
	0x004d,
	0x004e,
	0x004f,
	0x0050,
	0x0051,
	0x0052,
	0x0053,
	0x0054,
	0x0055,
	0x0056,
	0x0057,
	0x0060,
	0x0061,
	0x0062,
	0x0063,
	0x0064,
	0x0065,
	0x0066,
	0x0067,
	0x0068,
	
};
#define GPA_SET_LOW     0x42	
#define GPA_SET_HIGH    0x43	
#define GPA_CLEAR_LOW 	0x44
#define GPA_CLEAR_HIGH 	0x45
#define GPA_TOGGLE_LOW  0x46	
#define GPA_TOGGLE_HIGH 0x47	
#define GPB_SET_LOW 	0x4a
#define GPB_SET_HIGH 	0x4b
#define GPB_CLEAR_LOW 	0x4c
#define GPB_CLEAR_HIGH 	0x4d
#define GPB_TOGGLE_LOW 	0x4e
#define GPB_TOGGLE_HIGH 0x4f
#define GPC_SET_LOW 	0x52
#define GPC_SET_HIGH 	0x53
#define GPC_CLEAR_LOW 	0x54
#define GPC_CLEAR_HIGH 	0x55
#define GPC_TOGGLE_LOW  0x56	
#define GPC_TOGGLE_HIGH 0x57	

#define REG_NUM 69

//----------------------------------------------------
//
#define GPIOA(ptr, REG, index) \
	{ \
		switch(index) \
		{ \
			case 0: \
				(*ptr) = regs->REG.bit.GPIO0; \
				break; \
			case 1: \
				(*ptr) = regs->REG.bit.GPIO1; \
				break; \
			case 2: \
				(*ptr) = regs->REG.bit.GPIO2; \
				break; \
			case 3: \
				(*ptr) = regs->REG.bit.GPIO3; \
				break; \
			case 4: \
				(*ptr) = regs->REG.bit.GPIO4; \
				break; \
			case 5: \
				(*ptr) = regs->REG.bit.GPIO5; \
				break; \
			case 6: \
				(*ptr) = regs->REG.bit.GPIO6; \
				break; \
			case 7: \
				(*ptr) = regs->REG.bit.GPIO7; \
				break; \
			case 8: \
				(*ptr) = regs->REG.bit.GPIO8; \
				break; \
			case 9: \
				(*ptr) = regs->REG.bit.GPIO9; \
				break; \
			case 10: \
				(*ptr) = regs->REG.bit.GPIO10; \
				break; \
			case 11: \
				(*ptr) = regs->REG.bit.GPIO11; \
				break; \
			case 12: \
				(*ptr) = regs->REG.bit.GPIO12; \
				break; \
			case 13: \
				(*ptr) = regs->REG.bit.GPIO13; \
				break; \
			case 14: \
				(*ptr) = regs->REG.bit.GPIO14; \
				break; \
			case 15: \
				(*ptr) = regs->REG.bit.GPIO15; \
				break; \
			case 16: \
				(*ptr) = regs->REG.bit.GPIO16; \
				break; \
			case 17: \
				(*ptr) = regs->REG.bit.GPIO17; \
				break; \
			case 18: \
				(*ptr) = regs->REG.bit.GPIO18; \
				break; \
			case 19: \
				(*ptr) = regs->REG.bit.GPIO19; \
				break; \
			case 20: \
				(*ptr) = regs->REG.bit.GPIO20; \
				break; \
			case 21: \
				(*ptr) = regs->REG.bit.GPIO21; \
				break; \
			case 22: \
				(*ptr) = regs->REG.bit.GPIO22; \
				break; \
			case 23: \
				(*ptr) = regs->REG.bit.GPIO23; \
				break; \
			case 24: \
				(*ptr) = regs->REG.bit.GPIO24; \
				break; \
			case 25: \
				(*ptr) = regs->REG.bit.GPIO25; \
				break; \
			case 26: \
				(*ptr) = regs->REG.bit.GPIO26; \
				break; \
			case 27: \
				(*ptr) = regs->REG.bit.GPIO27; \
				break; \
			case 28: \
				(*ptr) = regs->REG.bit.GPIO28; \
				break; \
			case 29: \
				(*ptr) = regs->REG.bit.GPIO29; \
				break; \
			case 30: \
				(*ptr) = regs->REG.bit.GPIO30; \
				break; \
			case 31: \
				(*ptr) = regs->REG.bit.GPIO31; \
				break; \
			default: \
				break; \
		} \
	}

#define GPIOB(ptr, REG, index) \
	{ \
		switch(index) \
		{ \
			case 32: \
				(*ptr) = regs->REG.bit.GPIO32; \
				break; \
			case 33: \
				(*ptr) = regs->REG.bit.GPIO33; \
				break; \
			case 34: \
				(*ptr) = regs->REG.bit.GPIO34; \
				break; \
			case 35: \
				(*ptr) = regs->REG.bit.GPIO35; \
				break; \
			case 36: \
				(*ptr) = regs->REG.bit.GPIO36; \
				break; \
			case 37: \
				(*ptr) = regs->REG.bit.GPIO37; \
				break; \
			case 38: \
				(*ptr) = regs->REG.bit.GPIO38; \
				break; \
			case 39: \
				(*ptr) = regs->REG.bit.GPIO39; \
				break; \
			case 40: \
				(*ptr) = regs->REG.bit.GPIO40; \
				break; \
			case 41: \
				(*ptr) = regs->REG.bit.GPIO41; \
				break; \
			case 42: \
				(*ptr) = regs->REG.bit.GPIO42; \
				break; \
			case 43: \
				(*ptr) = regs->REG.bit.GPIO43; \
				break; \
			case 44: \
				(*ptr) = regs->REG.bit.GPIO44; \
				break; \
			case 45: \
				(*ptr) = regs->REG.bit.GPIO45; \
				break; \
			case 46: \
				(*ptr) = regs->REG.bit.GPIO46; \
				break; \
			case 47: \
				(*ptr) = regs->REG.bit.GPIO47; \
				break; \
			case 48: \
				(*ptr) = regs->REG.bit.GPIO48; \
				break; \
			case 49: \
				(*ptr) = regs->REG.bit.GPIO49; \
				break; \
			case 50: \
				(*ptr) = regs->REG.bit.GPIO50; \
				break; \
			case 51: \
				(*ptr) = regs->REG.bit.GPIO51; \
				break; \
			case 52: \
				(*ptr) = regs->REG.bit.GPIO52; \
				break; \
			case 53: \
				(*ptr) = regs->REG.bit.GPIO53; \
				break; \
			case 54: \
				(*ptr) = regs->REG.bit.GPIO54; \
				break; \
			case 55: \
				(*ptr) = regs->REG.bit.GPIO55; \
				break; \
			case 56: \
				(*ptr) = regs->REG.bit.GPIO56; \
				break; \
			case 57: \
				(*ptr) = regs->REG.bit.GPIO57; \
				break; \
			case 58: \
				(*ptr) = regs->REG.bit.GPIO58; \
				break; \
			case 59: \
				(*ptr) = regs->REG.bit.GPIO59; \
				break; \
			case 60: \
				(*ptr) = regs->REG.bit.GPIO60; \
				break; \
			case 61: \
				(*ptr) = regs->REG.bit.GPIO61; \
				break; \
			case 62: \
				(*ptr) = regs->REG.bit.GPIO62; \
				break; \
			case 63: \
				(*ptr) = regs->REG.bit.GPIO63; \
				break; \
			default: \
				break; \
		} \
	}

#define GPIOC(ptr, REG, index) \
	{ \
		switch(index) \
		{ \
			case 64: \
				(*ptr) = regs->REG.bit.GPIO64; \
				break; \
			case 65: \
				(*ptr) = regs->REG.bit.GPIO65; \
				break; \
			case 66: \
				(*ptr) = regs->REG.bit.GPIO66; \
				break; \
			case 67: \
				(*ptr) = regs->REG.bit.GPIO67; \
				break; \
			case 68: \
				(*ptr) = regs->REG.bit.GPIO68; \
				break; \
			case 69: \
				(*ptr) = regs->REG.bit.GPIO69; \
				break; \
			case 70: \
				(*ptr) = regs->REG.bit.GPIO70; \
				break; \
			case 71: \
				(*ptr) = regs->REG.bit.GPIO71; \
				break; \
			case 72: \
				(*ptr) = regs->REG.bit.GPIO72; \
				break; \
			case 73: \
				(*ptr) = regs->REG.bit.GPIO73; \
				break; \
			case 74: \
				(*ptr) = regs->REG.bit.GPIO74; \
				break; \
			case 75: \
				(*ptr) = regs->REG.bit.GPIO75; \
				break; \
			case 76: \
				(*ptr) = regs->REG.bit.GPIO76; \
				break; \
			case 77: \
				(*ptr) = regs->REG.bit.GPIO77; \
				break; \
			case 78: \
				(*ptr) = regs->REG.bit.GPIO78; \
				break; \
			case 79: \
				(*ptr) = regs->REG.bit.GPIO79; \
				break; \
			case 80: \
				(*ptr) = regs->REG.bit.GPIO80; \
				break; \
			case 81: \
				(*ptr) = regs->REG.bit.GPIO81; \
				break; \
			case 82: \
				(*ptr) = regs->REG.bit.GPIO82; \
				break; \
			case 83: \
				(*ptr) = regs->REG.bit.GPIO83; \
				break; \
			case 84: \
				(*ptr) = regs->REG.bit.GPIO84; \
				break; \
			case 85: \
				(*ptr) = regs->REG.bit.GPIO85; \
				break; \
			case 86: \
				(*ptr) = regs->REG.bit.GPIO86; \
				break; \
			case 87: \
				(*ptr) = regs->REG.bit.GPIO87; \
				break; \
			default: \
				break; \
		} \
	}

//----------------------------------------------------
//
#define GPIOA_WR(ptr, REG, index) \
	{ \
		switch(index) \
		{ \
			case 0: \
				 regs->REG.bit.GPIO0 = (*ptr); \
				break; \
			case 1: \
				regs->REG.bit.GPIO1 = (*ptr); \
				break; \
			case 2: \
				regs->REG.bit.GPIO2 = (*ptr); \
				break; \
			case 3: \
				regs->REG.bit.GPIO3 = (*ptr); \
				break; \
			case 4: \
				regs->REG.bit.GPIO4 = (*ptr); \
				break; \
			case 5: \
				regs->REG.bit.GPIO5 = (*ptr); \
				break; \
			case 6: \
				regs->REG.bit.GPIO6 = (*ptr); \
				break; \
			case 7: \
				regs->REG.bit.GPIO7 = (*ptr); \
				break; \
			case 8: \
				regs->REG.bit.GPIO8 = (*ptr); \
				break; \
			case 9: \
				regs->REG.bit.GPIO9 = (*ptr); \
				break; \
			case 10: \
				regs->REG.bit.GPIO10 = (*ptr); \
				break; \
			case 11: \
				regs->REG.bit.GPIO11 = (*ptr); \
				break; \
			case 12: \
				regs->REG.bit.GPIO12 = (*ptr); \
				break; \
			case 13: \
				regs->REG.bit.GPIO13 = (*ptr); \
				break; \
			case 14: \
				regs->REG.bit.GPIO14 = (*ptr); \
				break; \
			case 15: \
				regs->REG.bit.GPIO15 = (*ptr); \
				break; \
			case 16: \
				regs->REG.bit.GPIO16 = (*ptr); \
				break; \
			case 17: \
				regs->REG.bit.GPIO17 = (*ptr); \
				break; \
			case 18: \
				regs->REG.bit.GPIO18 = (*ptr); \
				break; \
			case 19: \
				regs->REG.bit.GPIO19 = (*ptr); \
				break; \
			case 20: \
				regs->REG.bit.GPIO20 = (*ptr); \
				break; \
			case 21: \
				regs->REG.bit.GPIO21 = (*ptr); \
				break; \
			case 22: \
				regs->REG.bit.GPIO22 = (*ptr); \
				break; \
			case 23: \
				regs->REG.bit.GPIO23 = (*ptr); \
				break; \
			case 24: \
				regs->REG.bit.GPIO24 = (*ptr); \
				break; \
			case 25: \
				regs->REG.bit.GPIO25 = (*ptr); \
				break; \
			case 26: \
				regs->REG.bit.GPIO26 = (*ptr); \
				break; \
			case 27: \
				regs->REG.bit.GPIO27 = (*ptr); \
				break; \
			case 28: \
				regs->REG.bit.GPIO28 = (*ptr); \
				break; \
			case 29: \
				regs->REG.bit.GPIO29 = (*ptr); \
				break; \
			case 30: \
				regs->REG.bit.GPIO30 = (*ptr); \
				break; \
			case 31: \
				regs->REG.bit.GPIO31 = (*ptr); \
				break; \
			default: \
				break; \
		} \
	}

#define GPIOB_WR(ptr, REG, index) \
	{ \
		switch(index) \
		{ \
			case 32: \
				regs->REG.bit.GPIO32 = (*ptr); \
				break; \
			case 33: \
				regs->REG.bit.GPIO33 = (*ptr); \
				break; \
			case 34: \
				regs->REG.bit.GPIO34 = (*ptr); \
				break; \
			case 35: \
				regs->REG.bit.GPIO35 = (*ptr); \
				break; \
			case 36: \
				regs->REG.bit.GPIO36 = (*ptr); \
				break; \
			case 37: \
				regs->REG.bit.GPIO37 = (*ptr); \
				break; \
			case 38: \
				regs->REG.bit.GPIO38 = (*ptr); \
				break; \
			case 39: \
				regs->REG.bit.GPIO39 = (*ptr); \
				break; \
			case 40: \
				regs->REG.bit.GPIO40 = (*ptr); \
				break; \
			case 41: \
				regs->REG.bit.GPIO41 = (*ptr); \
				break; \
			case 42: \
				regs->REG.bit.GPIO42 = (*ptr); \
				break; \
			case 43: \
				regs->REG.bit.GPIO43 = (*ptr); \
				break; \
			case 44: \
				regs->REG.bit.GPIO44 = (*ptr); \
				break; \
			case 45: \
				regs->REG.bit.GPIO45 = (*ptr); \
				break; \
			case 46: \
				regs->REG.bit.GPIO46 = (*ptr); \
				break; \
			case 47: \
				regs->REG.bit.GPIO47 = (*ptr); \
				break; \
			case 48: \
				regs->REG.bit.GPIO48 = (*ptr); \
				break; \
			case 49: \
				regs->REG.bit.GPIO49 = (*ptr); \
				break; \
			case 50: \
				regs->REG.bit.GPIO50 = (*ptr); \
				break; \
			case 51: \
				regs->REG.bit.GPIO51 = (*ptr); \
				break; \
			case 52: \
				regs->REG.bit.GPIO52 = (*ptr); \
				break; \
			case 53: \
				regs->REG.bit.GPIO53 = (*ptr); \
				break; \
			case 54: \
				regs->REG.bit.GPIO54 = (*ptr); \
				break; \
			case 55: \
				regs->REG.bit.GPIO55 = (*ptr); \
				break; \
			case 56: \
				regs->REG.bit.GPIO56 = (*ptr); \
				break; \
			case 57: \
				regs->REG.bit.GPIO57 = (*ptr); \
				break; \
			case 58: \
				regs->REG.bit.GPIO58 = (*ptr); \
				break; \
			case 59: \
				regs->REG.bit.GPIO59 = (*ptr); \
				break; \
			case 60: \
				regs->REG.bit.GPIO60 = (*ptr); \
				break; \
			case 61: \
				regs->REG.bit.GPIO61 = (*ptr); \
				break; \
			case 62: \
				regs->REG.bit.GPIO62 = (*ptr); \
				break; \
			case 63: \
				regs->REG.bit.GPIO63 = (*ptr); \
				break; \
			default: \
				break; \
		} \
	}

#define GPIOC_WR(ptr, REG, index) \
	{ \
		switch(index) \
		{ \
			case 64: \
				regs->REG.bit.GPIO64 = (*ptr); \
				break; \
			case 65: \
				regs->REG.bit.GPIO65 = (*ptr); \
				break; \
			case 66: \
				regs->REG.bit.GPIO66 = (*ptr); \
				break; \
			case 67: \
				regs->REG.bit.GPIO67 = (*ptr); \
				break; \
			case 68: \
				regs->REG.bit.GPIO68 = (*ptr); \
				break; \
			case 69: \
				regs->REG.bit.GPIO69 = (*ptr); \
				break; \
			case 70: \
				regs->REG.bit.GPIO70 = (*ptr); \
				break; \
			case 71: \
				regs->REG.bit.GPIO71 = (*ptr); \
				break; \
			case 72: \
				regs->REG.bit.GPIO72 = (*ptr); \
				break; \
			case 73: \
				regs->REG.bit.GPIO73 = (*ptr); \
				break; \
			case 74: \
				regs->REG.bit.GPIO74 = (*ptr); \
				break; \
			case 75: \
				regs->REG.bit.GPIO75 = (*ptr); \
				break; \
			case 76: \
				regs->REG.bit.GPIO76 = (*ptr); \
				break; \
			case 77: \
				regs->REG.bit.GPIO77 = (*ptr); \
				break; \
			case 78: \
				regs->REG.bit.GPIO78 = (*ptr); \
				break; \
			case 79: \
				regs->REG.bit.GPIO79 = (*ptr); \
				break; \
			case 80: \
				regs->REG.bit.GPIO80 = (*ptr); \
				break; \
			case 81: \
				regs->REG.bit.GPIO81 = (*ptr); \
				break; \
			case 82: \
				regs->REG.bit.GPIO82 = (*ptr); \
				break; \
			case 83: \
				regs->REG.bit.GPIO83 = (*ptr); \
				break; \
			case 84: \
				regs->REG.bit.GPIO84 = (*ptr); \
				break; \
			case 85: \
				regs->REG.bit.GPIO85 = (*ptr); \
				break; \
			case 86: \
				regs->REG.bit.GPIO86 = (*ptr); \
				break; \
			case 87: \
				regs->REG.bit.GPIO87 = (*ptr); \
				break; \
			default: \
				break; \
		} \
	}

#define OUTOFRANGE (-1)

// electrical character of pins */
typedef enum pin_type {
	INPUT = 0,
	OUTPUT,
} pin_type_t;

#endif
