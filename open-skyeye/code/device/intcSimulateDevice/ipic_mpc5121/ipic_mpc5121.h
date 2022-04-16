#ifndef __IPIC_MPC5121_H__ 
#define __IPIC_MPC5121_H__ 
struct registers 
{
	
	uint32_t sicfr;
	
	uint32_t sivcr;
	
	uint32_t sipnr_h;
	
	uint32_t sipnr_l;
	
	uint32_t siprr_a;
	
	uint32_t siprr_b;
	
	uint32_t siprr_c;
	
	uint32_t siprr_d;
	
	uint32_t simsr_h;
	
	uint32_t simsr_l;
	
	uint32_t sicnr;
	
	uint32_t sepnr;
	
	uint32_t smprr_a;
	
	uint32_t smprr_b;
	
	uint32_t semsr;
	
	uint32_t secnr;
	
	uint32_t sersr;
	
	uint32_t sermr;
	
	uint32_t sifcr_h;
	
	uint32_t sifcr_l;
	
	uint32_t sefcr;
	
	uint32_t serfr;
	
	uint32_t scvcr;
	
	uint32_t smvcr;
	
};


struct mpc5121_device 
{
	
	conf_object_t* obj;
	
	core_signal_intf* signal_iface;
	
	conf_object_t* signal;
	
	struct registers regs;
	
};
typedef struct mpc5121_device ipic_mpc5121_device;
static char* regs_name[] = 
{
	"sicfr",
	"sivcr",
	"sipnr_h",
	"sipnr_l",
	"siprr_a",
	"siprr_b",
	"siprr_c",
	"siprr_d",
	"simsr_h",
	"simsr_l",
	"sicnr",
	"sepnr",
	"smprr_a",
	"smprr_b",
	"semsr",
	"secnr",
	"sersr",
	"sermr",
	"sifcr_h",
	"sifcr_l",
	"sefcr",
	"serfr",
	"scvcr",
	"smvcr",
	NULL
};
#define SICFR 		0x00 //System Global Interrupt Configuration Register (SICFR) R/W 0x0000_0000 
#define SIVCR 		0x04 //System Global Interrupt Vector Register (SIVCR) R 0x0000_0000 
#define SIPNR_H 	0x08 //System Internal Interrupt Pending Register (SIPNR_H) R 0x0000_0000 
#define SIPNR_L		0x0C //System Internal Interrupt Pending Register (SIPNR_L) R 0x0000_0000 
#define SIPRR_A		0x10 //System Internal Interrupt Group A Priority Register (SIPRR_A) R/W 0x0530_9770 
#define SIPRR_B		0x14 //System Internal Interrupt Group B Priority Register (SIPRR_B) R/W 0x0530_9770 
#define SIPRR_C		0x18 //System Internal Interrupt Group C Priority Register (SIPRR_C) R/W 0x0530_9770 
#define SIPRR_D		0x1C //System Internal Interrupt Group D Priority Register (SIPRR_D) R/W 0x0530_9770 
#define SIMSR_H		0x20 //System Internal Interrupt Mask Register (SIMSR_H) R/W 0x0000_0000 
#define SIMSR_L		0x24 //System Internal Interrupt Mask Register (SIMSR_L) R/W 0x0000_0000 
#define SICNR		0x28 //System Internal Interrupt Control Register (SICNR) R/W 0x0000_0000
#define SEPNR		0x2C //System External Interrupt Pending Register (SEPNR) R/W Special 
#define SMPRR_A		0x30 //System Mixed Interrupt Group A Priority Register (SMPRR_A) R/W 0x0530_9770 
#define SMPRR_B		0x34 //System Mixed Interrupt Group B Priority Register (SMPRR_B) R/W 0x0530_9770 
#define SEMSR		0x38 //System External Interrupt Mask Register (SEMSR) R/W 0x0000_0000 
#define SECNR		0x3C //System External Interrupt Control Register (SECNR) R/W 0x0000_0000 
#define SERSR		0x40 //System Error Status Register (SERSR) R/W 0x0000_0000 
#define SERMR		0x44 //System Error Mask Register (SERMR) R/W 0xE580_0000 
#define SIFCR_H		0x50 //System Internal Interrupt Force Register (SIFCR_H) R/W 0x0000_0000 
#define SIFCR_L		0x54 //System Internal Interrupt Force Register (SIFCR_L) R/W 0x0000_0000 
#define SEFCR		0x58 //System External Interrupt Force Register (SEFCR) R/W 0x0000_0000 
#define SERFR		0x5C //System Error Force Register (SERFR) R/W 0x0000_0000 
#define SCVCR		0x60 //System critical interrupt vector register (SCVCR) R 0x0000_0000 
#define SMVCR		0x64 //System management interrupt vector register (SMVCR) R 0x0000_0000 

//the offset of interrupt source(0~95) in the SIMSR_L and SIMSR_L register
uint32_t simsr_offset_table[95] = {
-1,    //0 Error 	 
15,    //1 PCI 		 
14,    //2 PCI DMA 	 
13,    //3 PCI MU 	 
12,    //4 FEC 		 
11,    //5 PATA 		 
10,    //6 NFC 		 
9 ,    //7 LPC 		 
8 ,    //8 SDHC 		 
7 ,    //9 I2C1 		 
6 ,    //10 I2C2 	 
5 ,    //11 I2C3 	 
4 ,    //12 MSCAN1 	 
3 ,    //13 MSCAN2 	 
2 ,    //14 BDLC 	 
1 ,    //15 GPT0 	 
0 ,    //16 GPT1 	 
-1,    //17 IRQ1 	
-1,    //18 Reserved 	
-1,    //19 Reserved 	
-1,    //20 Reserved 	
-1,    //21 Reserved 	
-1,    //22 Reserved 	
-1,    //23 Reserved 	
-1,    //24 Reserved 	
-1,    //25 Reserved 	
-1,    //26 Reserved 	
-1,    //27 Reserved 	
-1,    //28 Reserved 	
-1,    //29 Reserved 	
-1,    //30 Reserved 	
-1,    //31 Reserved
31,    //32 PSC4		 	
30,    //33 PSC5 	
29,    //34 PSC6 	
28,    //35 PSC7 	
27,    //36 PSC8 	
26,    //37 PSC9 	
25,    //38 PSC10 	
24,    //39 PSC11 	
23,    //40 FIFOC 	
22,    //41 SPDIF 	
21,    //42 AXE 		
20,    //43 USB ULPI 	
19,    //44 USB UTMI 	
18,    //45 SATA 	
-1,    //46 Reserved 	
-1,    //47 Reserved 	
-1,    //48 IRQ0 	
-1,    //49 Reserved 	
-1,    //50 Reserved 	
-1,    //51 Reserved 	
-1,    //52 Reserved 	
-1,    //53 Reserved 	
-1,    //54 Reserved 	
-1,    //55 Reserved 	
-1,    //56 Reserved 	
-1,    //57 Reserved 	
-1,    //58 Reserved 	
-1,    //59 Reserved 	
-1,    //60 Reserved 	
-1,    //61 Reserved 	
-1,    //62 Reserved 	
-1,    //63 Reserved
31,    //64 DIU 		 	
30,    //65 DMA2 	
29,    //66 MBX1 	
28,    //67 VIU 		
27,    //68 PSC0 	
26,    //69 PSC1 	
25,    //70 PSC2 	
24,    //71 PSC3 	
23,    //72 GPT2 	
22,    //73 GPT3 	
21,    //74 GPT4 	
20,    //75 GPT5 	
19,    //76 GPT6 	
18,    //77 GPT7 	
17,    //78 GPIO 	
16,    //79 RTC SEC 	
15,    //80 RTC ALARM 	
14,    //81 DDR 		
13,    //82 SBA 		
12,    //83 PMC 		
11,    //84 USB ULPI WKUP
10,    //85 USB UTMI WKUP
9 ,    //86 SATA CMD 	
8 ,    //87 TEMP 105C 	
7 ,    //88 IIM 		
6 ,    //89 DDR PRIOMAN 	
5 ,    //90 MSCAN3 	
4 ,    //91 MSCAN4 	
-1,    //92 Reserved 	
-1,    //93 Reserved 	
-1,    //95 Reserved 	
-1    //95 Reserved 	
};
#endif  
