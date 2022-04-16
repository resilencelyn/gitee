#ifndef _EMUL_H_
#define _EMUL_H_

#include "cache.h"
#include "cpu.h"
#include <skyeye_types.h>
#include <memory_space.h>
#include <skyeye_addr_space.h>
#include <skyeye_signal.h>

#ifdef __MINGW32__
#define sync()  _flushall()
#else
#include <unistd.h>                     /* for sync() */
#endif

/* 16 bit implementation and revision fields.  The upper 8 bits are 0x20
 * for R3000. The lower 8 bits should distinguish the
 * simulator.
 */

/* Some instruction latencies. These are for R4600. On R4700, both all
 * four multiplication latencies are decreased by four cycles.
 */
#define  mult_latency		10
#define  multu_latency	 	10
#define  div_latency		42
#define  divu_latency		42
#define  dmult_latency	 	12
#define  dmultu_latency	 	12
#define  ddiv_latency		74
#define  ddivu_latency	 	74

/* Asynchronous event constants. Each interrupt source corresponds to a
 * single bit of the (events) word. Bits 8 through 16 correspond to
 * interrupts 0 through 7. Other bits represent the three reset exceptions
 * as follows:
 */
#define cold_reset_event	 	(1 << 0)
#define soft_reset_event		(1 << 1)
#define nmi_event			(1 << 2)

/* Cache of some strategic CPU mode bits. This shortcut is used to avoid
 * parsing of the somewhat convoluted Status register during address
 * translation and instruction decoding.
 */
#define xmode 	(1 << 0)                // 64 bit mode ([USK]X & [usk]mode)
#define cmode 	(1 << 1)                // 32 bit (compatibility) mode (!xmode)
#define bmode 	(1 << 2)                // big-endian-cpu (big_endian_mem() ^ reverse_endian())
#define rmode 	(1 << 3)                // reverse-endian (RE && umode)
#define umode 	(1 << 4)                // user mode
#define smode 	(1 << 5)                // supervisor mode
#define kmode 	(1 << 6)                // kernel mode

/* Pipeline information. This is fairly ad hoc, but it is still useful */
#define nothing_special		0       // nothing special
#define branch_delay		1       // current instruction in the branch-delay slot
#define instr_addr_error	2       // instruction address error
#define branch_nodelay 		3       // syscall instruction or eret instruction or JALRC and JRC jump instructions

/* Coprocessor 1 (FPU) simulator */

//static const char* condname[16];
#define FCR0   			0
#define FP_Rev_Last  		7
#define FP_Rev_First  		0
#define FP_Imp_Last  		15
#define FP_Imp_First  		8
#define FCR31  			31
#define FP_RM_Last  		1
#define FP_RM_First  		0
#define FP_Flag_Last  		6
#define FP_Flag_First  		2
#define FP_Enable_Last  	11
#define FP_Enable_First  	7
#define FP_Cause_Last  		17
#define FP_Cause_First  	12
#define FP_C  			23
#define FP_FS  			24

/* Software IEC/IEEE floating-point rounding mode */
#define float_round_nearest_even	0
#define float_round_to_zero      	1
#define float_round_up           	2
#define float_round_down         	3

/* Software IEC/IEEE floating-point exception flags */
#define float_flag_inexact   		1
#define float_flag_underflow 		2
#define float_flag_overflow  		4
#define float_flag_divbyzero 		8
#define float_flag_invalid   		16
#define float_flag_unimplemented	32

/* An invalid ASID bit used to mark unused TLB entries */
#define invalid_asid	(1 << 6)        //Shi yang 2006-08-08

/* An global ASID bit */
#define global_asid	(1 << 6)        //Shi yang 2006-08-08

/* Geometry of the TLB lookup map (see below.) */
#define log2_tlb_map_size  		6       //Shi yang 2006-08-08
#define tlb_map_size			(1 << log2_tlb_map_size)

/* An invalid cache tag used to mark unused cache lines */
#define bad_tag	~(UInt32)0

/* An invalid ibuf tag used to marked unused icache buffers */
#define bad_ibuf_tag ~(VA)0

/* An invalid physical address returned by translate_vaddr() on cache
 * operations that specify an invalid address.
 */
#define bad_pa	~(PA)0

/* TLB access types, used to select the appropriate TLB miss handler */
#define instr_fetch			0       //instruction fetch
#define data_load			1       //data load
#define data_store			2       //data store
#define cache_op			3       //cache operations (ignore errors)

#define no_tracing			0
#define report_interrupts		1
#define report_exceptions		2
#define print_instructions		3
#define dump_gprs			4
/* The NOP guard */
#define catch_nops			128

/* if return 0, translate 
 * 0 means find the correct PA
 * 1 means some TLB exception happened
 */
#define TLB_FAIL -1
#define TLB_SUCC 0
#define TLB_EXP 1

/* A TLB Entry structure */
typedef struct TLBEntry_s
{
    struct TLBEntry_s *next;            // the hash chain
    struct TLBEntry_s *prev;            // the hash chain
    UInt16 page_mask;
    UInt32 hi;                          // VPN << 12
    UInt32 lo_reg[2];                   // EntryLo0
    UInt16 index;                       // TLB index
    UInt16 hash;                        // TLB map index
} TLBEntry;

/* The two-entry ITLB. Each entry maps a 4KB page */
typedef struct ITLBEntry_s
{
    VA vpn;                             // virtual address page number (va / 4KB)
    PA pa;                              // physical address and the caching algorithm
    Int16 asid;                         // ASID and the global bit.
} ITLBEntry;

/* Although not in real hardware, we also cache the two most recent
 * I-cache accesses, as simulating cache lookups is slow, and as many as 8
 * instructions can be fetched from one cache line.
 */
typedef struct ICacheBuffer_s
{
    VA tag;                             // address of the cache shifted by log2_icache_line
    UInt32 *line;                       // pointer to the ICache line.
} ICacheBuffer;

typedef struct MIPS_State_s
{
    // The register set.
    VA pc;                              // Program Counter
    UInt32 gpr[32];                     // General Purpose Registers
    UInt32 fpr[32];                     // Floating-Point General Purpose Registers
    UInt32 fir, fcsr;                   // Floating-point implementation register and control-status register
    UInt32 cp0[32];                     // CP0 Registers0 array
    int mt_flag;                        // 1:use mt, 0:unused
    int tc_num;                         // mt tc number
    int vpe_num;                        // mt vpe_num
    UInt32 cp0_config1;
    UInt32 cp0_config2;
    UInt32 cp0_config3;
    UInt32 cp0_config4;
    UInt32 cp0_MVPCR;                   //cp0 sel1 reg0 MVP Control
    UInt32 cp0_MVPConf0;                //cp0 sel2 reg0 MVP Conf0
    UInt32 cp0_MVPConf1;                //cp0 sel3 reg0 MVP Conf1
    UInt32 cp0_VPECR;                   //cp0 sel1 reg1 VPE control Register
    UInt32 cp0_VPEConf0;                //cp0 sel2 reg1 VPE Conf0
    UInt32 cp0_VPEConf1;                //cp0 sel3 reg1 VPE Conf1
    UInt32 cp0_YQMask;                  //cp0 sel4 reg1 YQMask
    UInt32 cp0_VPESche;                 //cp0 sel5 reg1 VPESchedule
    UInt32 cp0_VPEScheFBack;            //cp0 sel6 reg1 VPEScheduleFBack
    UInt32 cp0_VPEOpt;                  //cp0 sel7 reg1 VPEOpt
    UInt32 cp0_TCStatus;                //cp0 sel1 reg2 TCStatus
    UInt32 cp0_TCBind;                  //cp0 sel2 reg2 TCBind
    UInt32 cp0_TCRestart;               //cp0 sel3 reg2 TCRestart
    UInt32 cp0_TCHalt;                  //cp0 sel4 reg2 TCHalt
    UInt32 cp0_TCContext;               //cp0 sel5 reg2 TCHalt
    UInt32 cp0_TCSche;                  //cp0 sel6 reg2 TCSchedule
    UInt32 cp0_TCScheFBack;             //cp0 sel7 reg2 TCScheFBack
    UInt32 cp0_TCOpt;                   //cp0 sel7 reg3 TCOpt
    UInt32 cp0_SRSConf0;                //cp0 sel1 reg6 SRSConf0
    UInt32 cp0_SRSConf1;                //cp0 sel2 reg6 SRSConf1
    UInt32 cp0_SRSConf2;                //cp0 sel3 reg6 SRSConf2
    UInt32 cp0_SRSConf3;                //cp0 sel4 reg6 SRSConf3
    UInt32 cp0_SRSConf4;                //cp0 sel5 reg6 SRSConf4
    UInt32 cp0_IntCtl;                  //cp0 sel1 reg12 IntCtl
    UInt32 cp0_EBase;                   //cp0 sel1 reg15 EBase
    UInt32 cp0_SRSCtl;                  //cp0 sel2 reg12 SRSCTl
    int int_flag;

    UInt32 cp1[32];                     // CP1 Registers
    UInt32 hi, lo;                      // Multiply and Divide Registers
    int ll_bit;                         // The Load-Linked Bit
    bool_t isa_mode;
    UInt32 cycle;

    ClockValue now;
    int warm;                           //flag:warm_reset or cold_reset
    int nop_count;                      //nop instruction count

    // CPU state.
    UInt16 events;                      // external events: this also replaces Cause[15:8].
    UInt8 pipeline;                     // current instruction in the branch-delay slot
    UInt8 mode;                         // CPU mode
    int sync_bit;                       // true after executing the SYNC instruction.
    VA branch_target;                   // next PC when in the branch delay slot        
    VA prev_pc;                         // prev PC when in the branch delay slot which is used by ADDIUPC

    /* cp0[Random] is not updated on each clock cycle. Instead, its value is
     * updated when either cp0[Random] or cp0[Wired] is set, and the current
     * value is computed using the time elapsed since then.
     */
    ClockValue random_seed;
    // Similary, we lazily compute the value of the Count register.
    ClockValue count_seed;

    // ITLB data. (lru_itlb) stores the index of the least-recently used entry.
    ITLBEntry itlb[2];
    int lru_itlb;
    /* I-cache buffer data (lru_ibuf) stores the index of the least-recently
     * used entry (0 or 1).
     */
    ICacheBuffer ibuf[2];
    int lru_ibuf;

    int env;                            //The longjmp buffer used for handling exceptions.

    /* The TLB and L1 caches */
    TLBEntry tlb[tlb_size];
    ICache icache;
    DCache dcache;
    /* The TLB map used to simulate the direct-mapped TLB lookup.  There's an
     * extra entry at the end that contains the hash chain of all unused TLB
     * entries (that way, there is always exactly (tlb_size) mappings in the
     * hash table).
     */
    TLBEntry *tlb_map[tlb_map_size + 1];

    int trace_level;
    // Finally, some configuration data.
    struct conf_t
    {
        char *bus;
        int ec;
        char *ep;
        int be;
        int trace;
    } conf;

    unsigned bigendSig;
    int irq_pending;

    conf_object_t *dyncom_cpu;
    uint64_t insn_num;
    uint64_t stop_step;
    uint8_t debug_mode;

    uint32_t code_start_addr;
    uint32_t code_end_addr;
    conf_object_t *obj;
    conf_object_t *cpu;
    void (*process_exception) (struct MIPS_State_s * mstate, UInt32 cause, int vec);
    memory_space_intf *core_memory_iface;

    breakpoint_mgt_t bp_manager;

    conf_object_t *memory_space;
    memory_space_intf *memory_space_iface;
    int irq_flag;
    uint32_t active;                    //The core begin working when active is 1.
    int mmu_flag;
    int freq_hz;
} MIPS_State;

typedef MIPS_State mips_core_t;

void reset(int warm);

/* Interrupt delivery functions */
void deliver_cold_reset(MIPS_State * mstate);
void deliver_soft_reset(MIPS_State * mstate);

/* TLB operations */
void reset_tlb(MIPS_State * mstate);
void dump_tlb();
TLBEntry *probe_tlb(MIPS_State * mstate, VA va);
void set_tlb_entry(MIPS_State * mstate, int index);

int translate_vaddr(MIPS_State * mstate, VA va, int type, PA * pa);

/* Instruction cache operations */
void reset_icache(MIPS_State * mstate);
void control_icache(MIPS_State * mstate, VA va, PA pa, int op, int type);
Instr fetch(MIPS_State * mstate, VA va, PA pa);

/* Data cache operations */
void reset_dcache(MIPS_State * mstate);
void control_dcache(MIPS_State * mstate, VA va, PA pa, int op, int type);
UInt32 load_left(UInt32 x, VA va, int syscmd);  // LDL, LWL
UInt32 load_right(UInt32 x, VA va, int syscmd); // LDR, LWR
void store_left(UInt32 x, VA va, int syscmd);   // SDL, SWL
void store_right(UInt32 x, VA va, int syscmd);  // SDR, SWR

/* Complete any pending memory operations */
extern void process_reset(MIPS_State * mstate);
void process_exception(MIPS_State * mstate, UInt32 cause, int vec);
void gs32eb1_process_exception(MIPS_State * mstate, UInt32 cause, int vec);
void ls132r_process_exception(MIPS_State * mstate, UInt32 cause, int vec);
void process_coprocessor_unusable(MIPS_State * mstate, int c);
void process_syscall(MIPS_State * mstate);
void process_reserved_instruction(MIPS_State * mstate);
void process_integer_overflow(MIPS_State * mstate);
void process_address_error(MIPS_State * mstate, int type, VA va);
void process_tlb_refill(MIPS_State * mstate, int type, VA va);
void process_tlb_invalid(MIPS_State * mstate, int type, VA va);
void process_tlb_modified(MIPS_State * mstate, VA va);

/* Set the Coprocessor 0 timer interrupt */
void set_timer();

/* Coprocessor 0 register access. This is necessary as some registers
 * are computed on demand, and most registers have some read-only fields.
 */
UInt32 read_cp0(MIPS_State * mstate, int n, int sel);
void write_cp0(MIPS_State * mstate, int n, int sel, UInt32 x);

/* The main instruction decoder with satelite functions for some of the
 * more elaborate instructions.
 */
int decode(MIPS_State * mstate, Instr instr);
int decode16(MIPS_State * mstate, Instr instr);
int decode_cop0(MIPS_State * mstate, Instr instr);
int decode_cop1(MIPS_State * mstate, Instr instr);
int decode_cache(MIPS_State * mstate, Instr instr);
int decode_ldc1(MIPS_State * mstate, Instr instr);
int decode_lwc1(MIPS_State * mstate, Instr instr);
int decode_sdc1(MIPS_State * mstate, Instr instr);
int decode_swc1(MIPS_State * mstate, Instr instr);

/* Some debugging help */
void dump_gpr_registers();
void dump_fpr_registers();

/* Some declaration */
int branch_delay_slot(MIPS_State * mstate);

inline UInt32 sign_extend_UInt32(UInt32 x, int n);
inline void divide_Int32(Int32 a, Int32 b);
inline void divide_UInt32(UInt32 a, UInt32 b);

int big_endian_cpu(MIPS_State * mstate);
UInt32 swizzle_word(UInt32 x, UInt32 addr);

int tlb_hash(VA va);
int vaddr_region(VA va);
int get_random(MIPS_State * mstate);

void leave_kernel_mode(MIPS_State * mstate);
void enter_kernel_mode(MIPS_State * mstate);

void mips_register_attribute(conf_class_t * clss);
void mips_set_pc(conf_object_t * conf_obj, generic_address_t addr);
UInt32 mips_get_pc();
uint64_t mips_get_step(conf_object_t * conf_obj);
char *mips_get_regname_by_id(conf_object_t * conf_obj, uint32_t id);
uint32 mips_get_regval_by_id(conf_object_t * conf_obj, uint32_t id);
exception_t mips_set_regval_by_id(conf_object_t * conf_obj, uint32_t value, uint32_t id);
int mips_get_regnum();
void mips_stop(conf_object_t * running_core);
uint64_t gs32eb1_run(conf_object_t * running_core);
int gs32eb1_signal(conf_object_t * conf_obj, interrupt_signal_t * signal);
endian_t mips_get_endian(conf_object_t * obj);
align_t mips_get_alignment(conf_object_t * obj);
uint32_t mips_get_address_width(conf_object_t * obj);
char *mips_disassemble(conf_object_t * obj, generic_address_t addr);
char *mips_get_architecture(conf_object_t * obj);
void mips_set_textsection_info(conf_object_t * obj, generic_address_t code_start_addr, generic_address_t code_end_addr);
UInt32 mips_get_regid_by_name(conf_object_t * conf_obj, char *name);
void gs32eb1_trigger_irq(MIPS_State * mstate);
void mips_stepi(conf_object_t * obj, uint64_t steps);
exception_t mips_mem_read(conf_object_t * obj, UInt32 pa, void *data, int len);
exception_t mips_mem_write(conf_object_t * obj, UInt32 pa, void *data, int len);
int mips_insert_bp(conf_object_t * conf_obj, generic_address_t addr);
int mips_delete_bp(conf_object_t * conf_obj, generic_address_t addr);
uint32_t mips_get_bp_numbers(conf_object_t * conf_obj);
uint32_t mips_get_bp_addr_by_id(conf_object_t * conf_obj, int id);
int mips_get_trigger(conf_object_t * conf_obj);
bool_t mips_check_bp_trigger(conf_object_t * conf_obj);
int mips_clear_bp_trigger(conf_object_t * conf_obj);
exception_t mips_memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index);
exception_t mips_memory_space_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index);
void load(MIPS_State * mstate, VA va, PA pa, UInt32 * x, int size);
void store(MIPS_State * mstate, UInt32 data, VA va, PA pa, int size);

#define NO_MMU           0
#define SUPPORT_MMU      1

#endif //end of _EMUL_H__
