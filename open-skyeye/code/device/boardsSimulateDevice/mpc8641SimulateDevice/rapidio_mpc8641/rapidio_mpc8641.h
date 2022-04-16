/*
 * =====================================================================================
 *
 *       Filename:  leon2_uart.h
 *
 *    Description:  UART variables
 *
 *        Version:  1.0
 *        Created:  20/05/08 15:53:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef _MPC8641_RAPIDIO_H_
#define _MPC8641_RAPIDIO_H_

//Architectural regiseters types
typedef struct arch_reg{
	uint32_t didcar;	//Architectual registers types
	uint32_t dicar;
	uint32_t aidcar;
	uint32_t aicar;
	uint32_t pefcar;
	uint32_t socar;
	uint32_t docar;
	uint32_t mcsr;
	uint32_t pwdcsr;
	uint32_t pellccsr;
	uint32_t lcsba1csr;
	uint32_t bdidcsr;
	uint32_t hbdidlcsr;
	uint32_t ctcsr;

	uint32_t om0sr;		//outbound message 0 status register
	uint32_t im0sr;		//inbound message 0 status register
	uint32_t msg_data;

/*
	uint32_t pmbh0;		//Extended Features Space --- 1x/4x LP-Sertal registers types
	uint32_t pltoccsr;
	uint32_t prtoccsr;
	uint32_t gccsr;
	uint32_t lmreqcsr;
	uint32_t lmrespcsr;
	uint32_t lascsr;
	uint32_t escsr;
	uint32_t ccsr;
	
	uint32_t erbh;		//Extended Features Space --- Error Reporting,Logical registers types
	uint32_t ltledcsr;
	uint32_t ltleecsr;
	uint32_t ltlaccsr;
	uint32_t ltldidccsr;
	uint32_t ltlcccsr;
	
	uint32_t edcsr;		//Extended Features Space --- Error Reporting,Physical registers types
	uint32_t erecsr;
	uint32_t ecacsr;
	uint32_t pcseccsr0;
	uint32_t peccsr1;
	uint32_t peccsr2;
	uint32_t peccsr3;
	uint32_t ercsr;
	uint32_t ertcsr;
*/
}arch_reg_t;



//registers name
static const char* arch_regs_name[] = {
	"ARCH_DIDCAR",		//Architectural
	"ARCH_DICAR",
	"ARCH_AIDCAR",
	"ARCH_AICAR",
	"ARCH_PEFCAR",
	"ARCH_SOCAR",
	"ARCH_DOCAR",
	"ARCH_MCSR",
	"ARCH_PWDCSR",
	"ARCH_PELLCCSR",
	"ARCH_LCSBA1CSR",
	"ARCH_BDIDCSR",
	"ARCH_HBDIDLCSR",
	"ARCH_CTCSR",
	"MU_OM0SR",
	"MU_IM0SR",
	"MSG_DATA",
/*
	"EFS_PMBH0",		//Extended Features Space
	"EFS_PLTOCCSR",
	"EFS_PRTOCCSR",
	"EFS_GCCSR",
	"EFS_LMREQCSR",
	"EFS_LMRESPCSR",
	"EFS_LASCSR",
	"EFS_ESCSR",
	"EFS_CCSR",

	"EFS_ERBH",
	"EFS_LTLEDCSR",
	"EFS_LTLEECSR",
	"EFS_LTLACCSR",
	"EFS_LTLDIDCCSR",
	"EFS_LTLCCCSR",

	"EFS_EDCSR",
	"EFS_ERECSR",
	"EFS_ECACSR",
	"EFS_PECCSR1",
	"EFS_PECCSR2",
	"EFS_PECCSR3",
	"EFS_ERCSR",
	"EFS_ERTCSR",
*/
};


//mpc8641 rapidio device type
typedef struct mpc8641_rapidio_dev{
	conf_object_t* conf_obj;
	arch_reg_t arch_regs;

	conf_object_t *tsi578_linker;
	tsi578_linker_intf *tsi578_linker_iface;
	
	int irq_number;
	char* arch_regs_name;
}mpc8641_rapidio_dev_t;


/*
 * Directly Addressable Registers
 * Architectural registers offset
 */
#define ARCH_DIDCAR		0x00
#define ARCH_DICAR		0x04
#define ARCH_AIDCAR		0x08
#define ARCH_AICAR		0x0c
#define ARCH_PEFCAR		0x10
#define ARCH_SOCAR		0x18
#define ARCH_DOCAR		0x1c
#define ARCH_MCSR		0x40
#define ARCH_PWDCSR 	0x44
#define ARCH_PELLCCSR	0x4c
#define ARCH_LCSBA1CSR	0x5c
#define ARCH_BDIDCSR	0x60
#define ARCH_HBDIDLCSR	0x68
#define ARCH_CTCSR		0x6c
#define MSG_DATA		0x6d
#define MU_OM0SR		0x000d3004
#define MU_IM0SR		0x000d3064


/*
 * ARCH is at 0xc_00nn
 */
//#define SINGLE_COER /*DUAL_CORE*/
//#ifdef  SINGLE_COER
//#define rPAIDIO_ARCH_DIDCAR		0x70100002
//#else
//#define rPAIDIO_ARCH_DIDCAR		0x70110002
//#endif
//#
//#ifdef  SINGLE_CORE
//#define rPAIDIO_ARCH_DICAR		0x80900000
//#else
//#define rPAIDIO_ARCH_DICAR		0x80900100
//#endif

#define rPAIDIO_ARCH_DIDCAR		0x70100002  /* Device identity capability register */
#define rPAIDIO_ARCH_DICAR		0x80900000  /* Device information capability register */
#define rPAIDIO_ARCH_AIDCAR		0x00000000  /* Assembly identity capability register */
#define rPAIDIO_ARCH_AICAR		0x00000000  /* Assembly information capability registr */
#define rPAIDIO_ARCH_PEFCAR		0xe0f80009  /* Processing element features capability register */
#define rPAIDIO_ARCH_SOCAR		0x0600fcf0  /* Source operations capability register */
#define rPAIDIO_ARCH_DOCAR		0x0000fcf4  /* Destination operations capability register */
#define rPAIDIO_ARCH_MCSR		0x20200000  /* Mailbox command and status register */
#define rPAIDIO_ARCH_PWDCSR 	0x20000020  /* Port-Write and doorbell command and status register */
#define rPAIDIO_ARCH_PELLCCSR	0x00000001  /* Processing element logical layer control command and status register */
#define rPAIDIO_ARCH_LCSBA1CSR	0x00000000  /* Local configuration space base address 1 command and status register */
#define rPAIDIO_ARCH_BDIDCSR	0x00000000  /* Base device ID command and status register */
#define rPAIDIO_ARCH_HBDIDLCSR	0x0000ffff  /* Host base device ID lock command and status register */
#define rPAIDIO_ARCH_CTCSR		0x00000000  /* Component tag command and status register */
#define rPAIDIO_MU_OM0SR		0x00000000  /* Outbound message 0 status register */
#define rPAIDIO_MU_IM0SR		0x00000020  /* Inbound message 0 status register */

/*
*	Extended Features Space registers offset
*/
/*
#define EFS_PMBH0		0x000c0100
#define EFS_PLTOCCSR	0x000c0120
#define EFS_PRTOCCSR	0x000c0124
#define EFS_GCCSR		0x000c013C
#define EFS_LMREQCSR	0x000c0140
#define EFS_LMRESPCSR	0x000c0144
#define EFS_LASCSR		0x000c0148
#define EFS_ESCSR		0x000c0158
#define EFS_CCSR		0x000c015C

#define EFS_ERBH		0x000c0600
#define EFS_LTLEDCSR	0x000c0608
#define EFS_LTLEECSR	0x000c060c
#define EFS_LTLACCSR	0x000c0614
#define EFS_LTLDIDCCSR	0x000c0618
#define EFS_LTLCCCSR	0x000c061c

#define EFS_EDCSR		0x000c0640
#define EFS_ERECSR		0x000c0644
#define EFS_ECACSR		0x000c0648
#define EFS_PCSECCSR0	0x000c064c
#define EFS_PECCSR1		0x000c0650
#define EFS_PECCSR2		0x000c0654
#define EFS_PECCSR3		0x000c0658
#define EFS_ERCSR		0x000c0668
#define EFS_ERTCSR		0x000c066c
*/

#endif
