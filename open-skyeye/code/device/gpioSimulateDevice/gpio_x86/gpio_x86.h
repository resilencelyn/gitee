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
* @file gpio_x86.h
* @brief The definition of system controller for x86
* @author Michael.Kang blackfin.kang@gmail.com
* @version 78.77
* @date 2011-12-12
*/

#ifndef __SYSCTRL_x86_H__
#define __SYSCTRL_x86_H__

//----------------------------------------------------
// GPIO A control register bit definitions */
struct GPACTRL_BITS {        // bits   description
   uint16_t QUALPRD0:8;        // 7:0    Qual period
   uint16_t QUALPRD1:8;        // 15:8   Qual period
   uint16_t QUALPRD2:8;        // 23:16  Qual period
   uint16_t QUALPRD3:8;        // 31:24  Qual period
};

union GPACTRL_REG {
   uint32_t              all;
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
   uint32_t              all;
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
   uint32_t              all;
   struct GPA1_BITS    bit;
};

union GPA2_REG {
   uint32_t              all;
   struct GPA2_BITS    bit;
};

union GPB1_REG {
   uint32_t              all;
   struct GPB1_BITS    bit;
};

union GPB2_REG {
   uint32_t              all;
   struct GPB2_BITS    bit;
};

union GPC1_REG {
   uint32_t              all;
   struct GPC1_BITS    bit;
};

union GPC2_REG {
   uint32_t              all;
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
   uint32_t              all;
   struct GPADAT_BITS  bit;
};

union GPBDAT_REG {
   uint32_t              all;
   struct GPBDAT_BITS  bit;
};

union GPCDAT_REG {
   uint32_t              all;
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
   uint32_t            rsvd1;
   union  GPBCTRL_REG  GPBCTRL;   // GPIO B Control Register (GPIO32 to 63)
   union  GPB1_REG     GPBQSEL1;  // GPIO B Qualifier Select 1 Register (GPIO32 to 47)
   union  GPB2_REG     GPBQSEL2;  // GPIO B Qualifier Select 2 Register (GPIO48 to 63)
   union  GPB1_REG     GPBMUX1;   // GPIO B Mux 1 Register (GPIO32 to 47)
   union  GPB2_REG     GPBMUX2;   // GPIO B Mux 2 Register (GPIO48 to 63)
   union  GPBDAT_REG   GPBDIR;    // GPIO B Direction Register (GPIO32 to 63)
   union  GPBDAT_REG   GPBPUD;    // GPIO B Pull Up Disable Register (GPIO32 to 63)
   uint16_t            rsvd2[8];
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
   uint16_t            rsvd3[8];
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
/*
typedef struct gpio_reg{
	uint16_t gpactrl;    //0x6f80
	uint16_t gpaqsel1;
	uint16_t gpaqsel2;
	uint16_t gpamux1;
	uint16_t gpamux2;
	uint16_t gpadir;
	uint16_t gpapud;

	uint16_t gpbctrl;    //0x6f90
	uint16_t gpbqsel1;
	uint16_t gpbqsel2;
	uint16_t gpbmux1;
	uint16_t gpbmux2;
	uint16_t gpbdir;
	uint16_t gpbpud;

	uint16_t gpcmux1;   //0x6fa6
	uint16_t gpcmux2;
	uint16_t gpcdir;
	uint16_t gpcpud;

	uint16_t gpadat;    //0x6fc0
	uint16_t gpaset;
	uint16_t gpaclear;
	uint16_t gpatoggle;

	uint16_t gpbdat;
	uint16_t gpbset;
	uint16_t gpbclear;
	uint16_t gpbtoggle;

	uint16_t gpcdat;
	uint16_t gpcset;
	uint16_t gpcclear;
	uint16_t gpctoggle;

	uint8_t gpioxint1;   //0x6fe0
	uint8_t gpioxint2;   //0x6fe1
	uint8_t gpioxnmi;
	uint8_t gpioxint3;
	uint8_t gpioxint4;
	uint8_t gpioxint5;
	uint8_t gpioxint6;
	uint8_t gpioxint7;
	uint8_t gpiolpm;
}gpio_reg_t; 
*/
static char* regs_name[] = {
	"gpactrl",
	"gpaqsel1",
	"gpaqsel2",
	"gpamux1",
	"gpamux2",
	"gpadir",
	"gpapud",
	"gpbctrl",    //0x6f90
	"gpbqsel1",
	"gpbqsel2",
	"gpbmux1",
	"gpbmux2",
	"gpbdir",
	"gpbpud",

	"gpcmux1",   
	"gpcmux2",
	"gpcdir",
	"gpcpud",
	"gpadat",    
	"gpaset",
	"gpaclear",
	"gpatoggle",

	"gpbdat",
	"gpbset",
	"gpbclear",
	"gpbtoggle",

	"gpcdat",
	"gpcset",
	"gpcclear",
	"gpctoggle",

	"gpioxint1",   
	"gpioxint2",   
	"gpioxnmi",
	"gpioxint3",
	"gpioxint4",
	"gpioxint5",
	"gpioxint6",
	"gpioxint7",
	"gpiolpm",
	NULL
};
typedef struct x86_gpio_device{
	conf_object_t* obj;
	gpio_reg_t* regs;
	conf_object_t* gpio_connector;
	skyeye_gpio_connector_intf *connector_iface;
	conf_object_t *gpio_gate_a;
	skyeye_gate_a_intf *gate_a_iface;
	conf_object_t *gpio_gate_b;
	skyeye_gate_b_intf *gate_b_iface;
	char** regs_name;
}x86_gpio_device;

#endif
