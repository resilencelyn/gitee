#ifndef __SEMAPHORE_C6713_H__
#define __SEMAPHORE_C6713_H__
struct registers
{
	
	uint32_t sem_pid;
	
	uint32_t sem_rst_run;
	
	uint32_t sem_eoi;
	
	uint32_t sem_direct[64];
	
	uint32_t sem_indirect[64];
	
	uint32_t sem_query[64];
	
	uint32_t semflagl[16];
	
	uint32_t semflagl_clear[16];
	
	uint32_t semflagh[16];
	
	uint32_t semflagh_clear[16];
	
	uint32_t semflagl_set[16];
	
	uint32_t semflagh_set[16];
	
	uint32_t semerr;
	
	uint32_t semerr_clear;
	
	uint32_t semerr_set;
	
};


struct c6713_device 
{
	
	conf_object_t* obj;

    conf_object_t* general_core;
    general_signal_intf* general_signal;

    uint32_t event_number;
    
    // 信号量队列
    uint32_t sem_query_index[32];
    uint32_t sem_query[32][64];
    // 信号量寄存器
    uint32_t sem_r[64];
	
	struct registers regs;
	
    int is_stom;
    int reg_val;
};
typedef struct c6713_device semaphore_c6713_device;
static char* regs_name[] =
{
	"sem_pid",
	"sem_rst_run",
	"sem_eoi",
	"sem_direct0",
	"sem_direct1",
	"sem_direct2",
	"sem_direct3",
	"sem_direct4",
	"sem_direct5",
	"sem_direct6",
	"sem_direct7",
	"sem_direct8",
	"sem_direct9",
	"sem_direct10",
	"sem_direct11",
	"sem_direct12",
	"sem_direct13",
	"sem_direct14",
	"sem_direct15",
	"sem_direct16",
	"sem_direct17",
	"sem_direct18",
	"sem_direct19",
	"sem_direct20",
	"sem_direct21",
	"sem_direct22",
	"sem_direct23",
	"sem_direct24",
	"sem_direct25",
	"sem_direct26",
	"sem_direct27",
	"sem_direct28",
	"sem_direct29",
	"sem_direct30",
	"sem_direct31",
	"sem_direct32",
	"sem_direct33",
	"sem_direct34",
	"sem_direct35",
	"sem_direct36",
	"sem_direct37",
	"sem_direct38",
	"sem_direct39",
	"sem_direct40",
	"sem_direct41",
	"sem_direct42",
	"sem_direct43",
	"sem_direct44",
	"sem_direct45",
	"sem_direct46",
	"sem_direct47",
	"sem_direct48",
	"sem_direct49",
	"sem_direct50",
	"sem_direct51",
	"sem_direct52",
	"sem_direct53",
	"sem_direct54",
	"sem_direct55",
	"sem_direct56",
	"sem_direct57",
	"sem_direct58",
	"sem_direct59",
	"sem_direct60",
	"sem_direct61",
	"sem_direct62",
	"sem_direct63",
	"sem_indirect0",
	"sem_indirect1",
	"sem_indirect2",
	"sem_indirect3",
	"sem_indirect4",
	"sem_indirect5",
	"sem_indirect6",
	"sem_indirect7",
	"sem_indirect8",
	"sem_indirect9",
	"sem_indirect10",
	"sem_indirect11",
	"sem_indirect12",
	"sem_indirect13",
	"sem_indirect14",
	"sem_indirect15",
	"sem_indirect16",
	"sem_indirect17",
	"sem_indirect18",
	"sem_indirect19",
	"sem_indirect20",
	"sem_indirect21",
	"sem_indirect22",
	"sem_indirect23",
	"sem_indirect24",
	"sem_indirect25",
	"sem_indirect26",
	"sem_indirect27",
	"sem_indirect28",
	"sem_indirect29",
	"sem_indirect30",
	"sem_indirect31",
	"sem_indirect32",
	"sem_indirect33",
	"sem_indirect34",
	"sem_indirect35",
	"sem_indirect36",
	"sem_indirect37",
	"sem_indirect38",
	"sem_indirect39",
	"sem_indirect40",
	"sem_indirect41",
	"sem_indirect42",
	"sem_indirect43",
	"sem_indirect44",
	"sem_indirect45",
	"sem_indirect46",
	"sem_indirect47",
	"sem_indirect48",
	"sem_indirect49",
	"sem_indirect50",
	"sem_indirect51",
	"sem_indirect52",
	"sem_indirect53",
	"sem_indirect54",
	"sem_indirect55",
	"sem_indirect56",
	"sem_indirect57",
	"sem_indirect58",
	"sem_indirect59",
	"sem_indirect60",
	"sem_indirect61",
	"sem_indirect62",
	"sem_indirect63",
	"sem_query0",
	"sem_query1",
	"sem_query2",
	"sem_query3",
	"sem_query4",
	"sem_query5",
	"sem_query6",
	"sem_query7",
	"sem_query8",
	"sem_query9",
	"sem_query10",
	"sem_query11",
	"sem_query12",
	"sem_query13",
	"sem_query14",
	"sem_query15",
	"sem_query16",
	"sem_query17",
	"sem_query18",
	"sem_query19",
	"sem_query20",
	"sem_query21",
	"sem_query22",
	"sem_query23",
	"sem_query24",
	"sem_query25",
	"sem_query26",
	"sem_query27",
	"sem_query28",
	"sem_query29",
	"sem_query30",
	"sem_query31",
	"sem_query32",
	"sem_query33",
	"sem_query34",
	"sem_query35",
	"sem_query36",
	"sem_query37",
	"sem_query38",
	"sem_query39",
	"sem_query40",
	"sem_query41",
	"sem_query42",
	"sem_query43",
	"sem_query44",
	"sem_query45",
	"sem_query46",
	"sem_query47",
	"sem_query48",
	"sem_query49",
	"sem_query50",
	"sem_query51",
	"sem_query52",
	"sem_query53",
	"sem_query54",
	"sem_query55",
	"sem_query56",
	"sem_query57",
	"sem_query58",
	"sem_query59",
	"sem_query60",
	"sem_query61",
	"sem_query62",
	"sem_query63",
	"semflagl0",
	"semflagl1",
	"semflagl2",
	"semflagl3",
	"semflagl4",
	"semflagl5",
	"semflagl6",
	"semflagl7",
	"semflagl8",
	"semflagl9",
	"semflagl10",
	"semflagl11",
	"semflagl12",
	"semflagl13",
	"semflagl14",
	"semflagl15",
	"semflagl_clear0",
	"semflagl_clear1",
	"semflagl_clear2",
	"semflagl_clear3",
	"semflagl_clear4",
	"semflagl_clear5",
	"semflagl_clear6",
	"semflagl_clear7",
	"semflagl_clear8",
	"semflagl_clear9",
	"semflagl_clear10",
	"semflagl_clear11",
	"semflagl_clear12",
	"semflagl_clear13",
	"semflagl_clear14",
	"semflagl_clear15",
	"semflagh0",
	"semflagh1",
	"semflagh2",
	"semflagh3",
	"semflagh4",
	"semflagh5",
	"semflagh6",
	"semflagh7",
	"semflagh8",
	"semflagh9",
	"semflagh10",
	"semflagh11",
	"semflagh12",
	"semflagh13",
	"semflagh14",
	"semflagh15",
	"semflagh_clear0",
	"semflagh_clear1",
	"semflagh_clear2",
	"semflagh_clear3",
	"semflagh_clear4",
	"semflagh_clear5",
	"semflagh_clear6",
	"semflagh_clear7",
	"semflagh_clear8",
	"semflagh_clear9",
	"semflagh_clear10",
	"semflagh_clear11",
	"semflagh_clear12",
	"semflagh_clear13",
	"semflagh_clear14",
	"semflagh_clear15",
	"semflagl_set0",
	"semflagl_set1",
	"semflagl_set2",
	"semflagl_set3",
	"semflagl_set4",
	"semflagl_set5",
	"semflagl_set6",
	"semflagl_set7",
	"semflagl_set8",
	"semflagl_set9",
	"semflagl_set10",
	"semflagl_set11",
	"semflagl_set12",
	"semflagl_set13",
	"semflagl_set14",
	"semflagl_set15",
	"semflagh_set0",
	"semflagh_set1",
	"semflagh_set2",
	"semflagh_set3",
	"semflagh_set4",
	"semflagh_set5",
	"semflagh_set6",
	"semflagh_set7",
	"semflagh_set8",
	"semflagh_set9",
	"semflagh_set10",
	"semflagh_set11",
	"semflagh_set12",
	"semflagh_set13",
	"semflagh_set14",
	"semflagh_set15",
	"semerr",
	"semerr_clear",
	"semerr_set",
	NULL
};
#define SEM_PID         0x0
#define SEM_RST_RUN     0x8
#define SEM_EOI         0xc
#define SEM_DIRECT      0x100
#define SEM_INDIRECT    0x200
#define SEM_QUERY       0x300
#define SEMFLAGL        0x400
#define SEMFLAGL_CLEAR  0x400
#define SEMFLAGH        0x440
#define SEMFLAGH_CLEAR  0x440
#define SEMFLAGL_SET    0x480
#define SEMFLAGH_SET    0x4c0
#define SEMERR          0x500
#define SEMERR_CLEAR    0x504
#define SEMERR_SET      0x508
#endif
