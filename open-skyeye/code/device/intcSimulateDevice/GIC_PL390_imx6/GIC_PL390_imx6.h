#ifndef __GIC_PL390_IMX6_H__
#define __GIC_PL390_IMX6_H__

void init_GIC_PL390_CPU_Interface(void);
void init_GIC_PL390_Distributor(void);

struct Distributor_registers
{
    union {
        uint32_t all;
        struct {
            uint32_t Enable     :1;
            uint32_t Reserved   :31;
        };
    } icddcr;

    union {
        uint32_t all;
        struct {
            uint32_t it_lines_number  :5;
            uint32_t cpu_number       :3;
            uint32_t SBZ_0            :2;
            uint32_t TZ               :1;
            uint32_t LSPI             :5;
            uint32_t SBZ_1            :16
        };
    } icdictr;

    union {
        uint32_t all;
        struct {
            uint32_t implementer      :12;
            uint32_t rev_num          :12;
            uint32_t impl_ver         :8;
        };
    } icdiidr;

    union {
        uint32_t all;
        struct {
            uint32_t sgi_security_if  :16;
            uint32_t ppi_security_if  :16;
        };
    } icdisr[8];
    uint32_t spi_security[1024 / 32];

    union {
        uint32_t all;
        struct {
            uint32_t                :16;
            uint32_t ppi_enable_if  :16;
        };
    } icdiser[8];
    uint32_t spi_enable[1024 / 32];

    union {
        uint32_t all;
        struct {
            uint32_t                :16;
            uint32_t ppi_enable_if  :16;
        };
    } icdicer[8];

    union {
        uint32_t all;
        struct {
            uint32_t sgi_pending_if  :16;
            uint32_t ppi_pending_if  :16;
        };
    } icdispr[8];
    uint32_t spi_pending[1024];

    union {
        uint32_t all;
        struct {
            uint32_t sgi_pending_if  :16;
            uint32_t ppi_pending_if  :16;
        };
    } icdicpr[8];

    union {
        uint32_t all;
        struct {
            uint32_t sgi_active_if  :16;
            uint32_t ppi_active_if  :16;
        };
    } icdabr[8];
    uint32_t spi_active[1024];

    struct {
        uint32_t priority_sgi_INTID_if[4];
        uint32_t priority_ppi_INTID_if[4];
        uint32_t priority_spi_INTID[(1024 - 32)/4];
    } icdipr[8];

    struct {
        uint32_t all;
        uint32_t targets_spi_INTID[(1024-32)/4];
    }icdiptr[8];

    union {
        uint32_t all;
    } icdicr[8];
    uint32_t int_config[(1024-32)/4];

	uint32_t ppi;
	uint32_t spi;
	uint32_t legacy_int[8];
	uint32_t match_d[8];
	uint32_t enable_d[8];
	uint32_t icdsgir;
	uint32_t periph_id_8;
	uint32_t periph_id_7;
	uint32_t periph_id_6;
	uint32_t periph_id_5;
	uint32_t periph_id_4;
	uint32_t periph_id_3;
	uint32_t periph_id_2;
	uint32_t periph_id_1;
	uint32_t periph_id_0;
	uint32_t component_id_3;
	uint32_t component_id_2;
	uint32_t component_id_1;
	uint32_t component_id_0;
};

struct CPU_Interface_registers
{
    union {
        uint32_t all;
        struct {
            uint32_t EnableGrp0    :1;
            uint32_t EnableGrp1    :1;
            uint32_t AckCtl        :1;
            uint32_t FIQEn         :1;
            uint32_t CBPR          :1;
            uint32_t FIQBypDisGrp0 :1;
            uint32_t IRQBypDisGrp0 :1;
            uint32_t FIQBypDisGrp1 :1;
            uint32_t IRQBypDisGrp1 :1;
            uint32_t EOImodeS      :1;
            uint32_t EOImodeNS     :1;
            uint32_t Reserved      :21;
        };
    } iccicr[8];

    union {
        uint32_t all;
        struct {
            uint32_t Priority   :8;
            uint32_t Reserved   :24;
        };
    } iccpmr[8];

    union {
        uint32_t all;
        struct {
            uint32_t Binary_point   :3;
            uint32_t Reserved       :29;
        };
    } iccbpr[8];

    union {
        uint32_t all;
        struct {
            uint32_t Interrupt_ID   :10;
            uint32_t CPUID          :3;
            uint32_t Reserved       :19;
        };
    } icciar;

    union {
        uint32_t all;
        struct {
            uint32_t EOIINTID       :10;
            uint32_t CPUID          :3;
            uint32_t Reserved       :19;
        };
    } icceoir;

	uint32_t iccrpr;
	uint32_t icchpir;
	uint32_t iccabpr;
	uint32_t integ_en_c[8];
	uint32_t interrupt_out[8];
	uint32_t match_c[8];
	uint32_t enable_c[8];
	uint32_t icciidr;
	uint32_t periph_id_8;
	uint32_t periph_id_7;
	uint32_t periph_id_6;
	uint32_t periph_id_5;
	uint32_t periph_id_4;
	uint32_t periph_id_3;
	uint32_t periph_id_2;
	uint32_t periph_id_1;
	uint32_t periph_id_0;
	uint32_t component_id_3;
	uint32_t component_id_2;
	uint32_t component_id_1;
	uint32_t component_id_0;
};

struct imx6_device
{
    uint32_t core_number;

	conf_object_t* cpu_obj;
	conf_object_t* dis_obj;
	
	core_signal_intf* signal_iface;
	conf_object_t* signal;

    uint32_t FIFO;
	
	struct CPU_Interface_registers cpu_regs;
	struct Distributor_registers dist_regs;
};

typedef struct imx6_device GIC_PL390_imx6_device;

// Distributor

static char* dist_regs_name[] = 
{
	"ICDDCR",
	"ICDICTR",
	"ICDIIDR",
	"ICDISR0",
	"ICDISR1",
	"ICDISR2",
	"ICDISR3",
	"ICDISR4",
	"ICDISR5",
	"ICDISR6",
	"ICDISR7",
	"ICDISER0",
	"ICDISER1",
	"ICDISER2",
	"ICDISER3",
	"ICDISER4",
	"ICDISER5",
	"ICDISER6",
	"ICDISER7",
	"ICDICER0",
	"ICDICER1",
	"ICDICER2",
	"ICDICER3",
	"ICDICER4",
	"ICDICER5",
	"ICDICER6",
	"ICDICER7",
	"ICDISPR0",
	"ICDISPR1",
	"ICDISPR2",
	"ICDISPR3",
	"ICDISPR4",
	"ICDISPR5",
	"ICDISPR6",
	"ICDISPR7",
	"ICDICPR0",
	"ICDICPR1",
	"ICDICPR2",
	"ICDICPR3",
	"ICDICPR4",
	"ICDICPR5",
	"ICDICPR6",
	"ICDICPR7",
	"ICDABR0",
	"ICDABR1",
	"ICDABR2",
	"ICDABR3",
	"ICDABR4",
	"ICDABR5",
	"ICDABR6",
	"ICDABR7",
	"ICDIPR0",
	"ICDIPR1",
	"ICDIPR2",
	"ICDIPR3",
	"ICDIPR4",
	"ICDIPR5",
	"ICDIPR6",
	"ICDIPR7",
	"ICDIPTR0",
	"ICDIPTR1",
	"ICDIPTR2",
	"ICDIPTR3",
	"ICDIPTR4",
	"ICDIPTR5",
	"ICDIPTR6",
	"ICDIPTR7",
	"ICDICR0",
	"ICDICR1",
	"ICDICR2",
	"ICDICR3",
	"ICDICR4",
	"ICDICR5",
	"ICDICR6",
	"ICDICR7",
	"PPI",
	"SPI",
	"legacy_int0",
	"legacy_int1",
	"legacy_int2",
	"legacy_int3",
	"legacy_int4",
	"legacy_int5",
	"legacy_int6",
	"legacy_int7",
	"match_d0",
	"match_d1",
	"match_d2",
	"match_d3",
	"match_d4",
	"match_d5",
	"match_d6",
	"match_d7",
	"enable_d0",
	"enable_d1",
	"enable_d2",
	"enable_d3",
	"enable_d4",
	"enable_d5",
	"enable_d6",
	"enable_d7",
	"ICDSGIR",
	"periph_id_8",
	"periph_id_7",
	"periph_id_6",
	"periph_id_5",
	"periph_id_4",
	"periph_id_3",
	"periph_id_2",
	"periph_id_1",
	"periph_id_0",
	"component_id_3",
	"component_id_2",
	"component_id_1",
	"component_id_0",
	NULL
};

#define ICDDCR 0x0
#define ICDICTR 0x4
#define ICDIIDR 0x8
#define ICDISR 0x80
#define ICDISER 0x100
#define ICDICER 0x180
#define ICDISPR 0x200
#define ICDICPR 0x280
#define ICDABR 0x300
#define ICDIPR 0x400
#define ICDIPTR 0x800
#define ICDICR 0xC00
#define PPI 0xD00
#define SPI 0xD04
#define LEGACY_INT 0xDD4
#define MATCH_D 0xDE0
#define ENABLE_D 0xDE4
#define ICDSGIR 0xF00
#define PERIPH_ID_8 0xFC0
#define PERIPH_ID_7 0xFD0
#define PERIPH_ID_6 0xFD4
#define PERIPH_ID_5 0xFD8
#define PERIPH_ID_4 0xFDC
#define PERIPH_ID_3 0xFE0
#define PERIPH_ID_2 0xFE4
#define PERIPH_ID_1 0xFE8
#define PERIPH_ID_0 0xFEC
#define COMPONENT_ID_3 0xFF0
#define COMPONENT_ID_2 0xFF4
#define COMPONENT_ID_1 0xFF8
#define COMPONENT_ID_0 0xFFC

// CPU_Interface
static char* cpu_regs_name[] = 
{
	"ICCICR",
	"ICCPMR",
	"ICCBPR",
	"ICCIAR",
	"ICCEOIR",
	"ICCRPR",
	"ICCHPIR",
	"ICCABPR",
	"integ_en_c",
	"interrupt_out",
	"match_c",
	"enable_c",
	"ICCIIDR",
	"periph_id_8",
	"periph_id_7",
	"periph_id_6",
	"periph_id_5",
	"periph_id_4",
	"periph_id_3",
	"periph_id_2",
	"periph_id_1",
	"periph_id_0",
	"component_id_3",
	"component_id_2",
	"component_id_1",
	"component_id_0",
	NULL
};

#define ICCICR 0x0
#define ICCPMR 0x4
#define ICCBPR 0x8
#define ICCIAR 0xC
#define ICCEOIR 0x10
#define ICCRPR 0x14
#define ICCHPIR 0x18
#define ICCABPR 0x1C
#define INTEG_EN_C 0x40
#define INTERRUPT_OUT 0x44
#define MATCH_C 0x50
#define ENABLE_C 0x54
#define ICCIIDR 0xFC
//#define PERIPH_ID_8 0xFC0
//#define PERIPH_ID_7 0xFD0
//#define PERIPH_ID_6 0xFD4
//#define PERIPH_ID_5 0xFD8
//#define PERIPH_ID_4 0xFDC
//#define PERIPH_ID_3 0xFE0
//#define PERIPH_ID_2 0xFE4
//#define PERIPH_ID_1 0xFE8
//#define PERIPH_ID_0 0xFEC
//#define COMPONENT_ID_3 0xFF0
//#define COMPONENT_ID_2 0xFF4
//#define COMPONENT_ID_1 0xFF8
//#define COMPONENT_ID_0 0xFFC

#endif
