/* Copyright (C)
 * 2013 - Michael.Kang blackfin.kang@gmail.com
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
* @file c6k_cpu.h
* @brief the core definition of c6k
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __C6K_CPU_H__
#define __C6K_CPU_H__
#include <stdint.h>
#include <skyeye_types.h>
#include <skyeye_lock.h>
#include <skyeye_signal.h>
#include <skyeye_addr_space.h>
#include <skyeye_core.h>
#define SPLOOP_BUFFER_SIZE 32

typedef enum
{
    GPR_A = 0,
    GPR_B,
    GPR_GROUP
} gpr_t;

#define GPR_NUM_IN_GROUP 32

#define DELAY_SLOT_NUM 12
//#define REGW_DELAY_SLOT 8
#define BRANCH_DELAY_SLOT 5
#define BRANCH_EVENT_ID 0x1
#define MEM_READ_DELAY_SLOT 4
#define MEM_WRITE_DELAY_SLOT 4
#define MEM_ACCESS_BUF_SIZE 4
#define MEM_RD_EVENT_ID 0x2
#define MEM_WR_EVENT_ID 0x4
#define REGW_EVENT_ID 0x8

#define MPYI_DELAY_SLOT 8
#define INTDP_DELAY_SLOT 4
#define DPTRUNC_DELAY_SLOT 3
#define CMPEQDP_DELAY_SLOT 1
#define CMPLTDP_DELAY_SLOT 1
#define CMPGTDP_DELAY_SLOT 1
#define SUBDP_DELAY_SLOT 6
#define SUBSP_DELAY_SLOT 3
#define ADDDP_DELAY_SLOT 6
#define REGW_BUF_SIZE 16
#define RCPDP_DELAY_SLOT 1
#define MPY_DELAY_SLOT 1
#define MPYU_DELAY_SLOT 1
#define MPYUS_DELAY_SLOT 1
#define MPYLHU_DELAY_SLOT 1
#define MPYSU_DELAY_SLOT 1
#define MPYHU_DELAY_SLOT 1
#define MPYDP_DELAY_SLOT 9
#define MVD_DELAY_SLOT 3
#define MPYLI_DELAY_SLOT 3
#define MPY32_DELAY_SLOT 3
#define FMPYDP_DELAY_SLOT 3
#define MPYID_DELAY_SLOT 9
#define MPYIH_DELAY_SLOT 3

#define GIE_BIT 0x1
#define NMIE_BIT 0x2

enum sploop_types
{
    SPLOOP = 0x1,
    SPLOOPD = 0x3,
    SPLOOPW = 0x2,
};

struct c6k_core;
struct insn_cache;
typedef enum rw_flag
{
    RD_FLAG = 1,
    WR_FLAG
} rw_flag_t;

typedef enum handler_type_t
{
    UPDATE_NOTHING = 0,
    UPDATE_REG_BY_IMM,
    UPDATE_REG_BY_MEM,
    UPDATE_MEM,
    UPDATE_PC,
} handler_index_t;

struct delay_slot_handle
{
    uint32_t count;
    struct delay_slot
    {
        uint32_t index;
        uint32_t width;
        union value
        {
            uint32_t imm;
            uint32_t from_addr;
        } value;
        union target
        {
            uint32_t reg_id;
            uint32_t to_addr;
        } target;
    } slots[DELAY_SLOT_NUM];
};

typedef enum
{
    AMR = 0,
    GSR,
    GFPGFR,
    ICR,
    IER,
    IFR,
    IRP,
    ISR,
    NRP,
    PCE1,
    DIER,
    DNUM,
    ECR,
    EFR,
    GPLYA,
    GPLYB,
    IERR,
    ILC,
    ITSR,
    TSR,
    FADCR,
    FMCR,
    CSR,
    ISTP,
    TSCL,
    TSCH,
    SSR,
    RILC,
    NTSR,

    INTFLAG,
    PFC_REG,
    PFC_BRANCH,
    C6K_PC_REG = PFC_BRANCH + DELAY_SLOT_NUM,
    SLOT_ID,
    SLOT_MASK,
    C6K_SPR_SIZE
} c6k_spr_t;

typedef struct c6k_core
{
    conf_object_t *obj;
    /* General Register */
    uint32_t gpr[GPR_GROUP][GPR_NUM_IN_GROUP];

    /* Control Register */
    uint32_t amr;
    uint32_t gsr;
    uint32_t gfpgfr;
    uint32_t icr;
    uint32_t ier;
    uint32_t ifr;
    uint32_t irp;
    uint32_t isr;
    uint32_t nrp;
    uint32_t pce1;
    uint32_t dier;
    uint32_t dnum;
    uint32_t ecr;
    uint32_t efr;
    uint32_t gplya;
    uint32_t gplyb;
    uint32_t ierr;
    uint32_t ilc;
    uint32_t itsr;
    uint32_t tsr;
    /* for c67x core */
    uint32_t fadcr;
    uint32_t fmcr;
    uint32_t csr;
    uint32_t istp;
    uint32_t tscl;
    uint32_t tsch;
    uint32_t ssr;
    uint32_t rilc;
    uint32_t ntsr;
    int intr_flag;
    uint32_t pfc, pfc_branch[DELAY_SLOT_NUM];
    uint32_t pc;
    uint32_t curr_slot;
    uint32_t slot_mask;
    struct delay_slot_handle ds_handles[DELAY_SLOT_NUM];

    uint64_t stop_step;
    uint32_t pc_bb_base;
    uint64_t cycle_num, insn_num;
    uint32_t delay_slot, delay_slot_cycle[DELAY_SLOT_NUM], curr_slot_id, mem_access_slot_id;
    /* save the result of memory access */
    uint64_t mem_access_result[DELAY_SLOT_NUM][MEM_ACCESS_BUF_SIZE];
    /* also store the rw info and other information in mem_access_reg
     * [0:7] store the register index, and [8:15] will store RW info
     */
    uint32_t mem_access_reg[DELAY_SLOT_NUM][MEM_ACCESS_BUF_SIZE];
    uint32_t mem_access_addr[DELAY_SLOT_NUM][MEM_ACCESS_BUF_SIZE];
    uint32_t mem_access_buf_pos[DELAY_SLOT_NUM];
    /* the address of write operation */
    uint32_t mem_wr_reg[DELAY_SLOT_NUM][MEM_ACCESS_BUF_SIZE];
    uint64_t mem_wr_result[DELAY_SLOT_NUM][MEM_ACCESS_BUF_SIZE];
    uint32_t mem_wr_addr[DELAY_SLOT_NUM][MEM_ACCESS_BUF_SIZE];
    uint32_t mem_wr_buf_pos[DELAY_SLOT_NUM];

    uint32_t regw_regno_buf[DELAY_SLOT_NUM][REGW_BUF_SIZE];
    uint32_t regw_regval_buf[DELAY_SLOT_NUM][REGW_BUF_SIZE];
    uint32_t regw_buf_pos[DELAY_SLOT_NUM];

    uint32_t regw_regno_buf_l[REGW_BUF_SIZE + 1];
    uint32_t regw_regval_buf_l[REGW_BUF_SIZE + 1];
    uint32_t regw_addr_l[REGW_BUF_SIZE + 1];
    uint32_t regw_buf_pos_l;
    uint32_t regw_regno_buf_h[REGW_BUF_SIZE + 1];
    uint32_t regw_regval_buf_h[REGW_BUF_SIZE + 1];
    uint32_t regw_addr_h[REGW_BUF_SIZE + 1];
    uint32_t regw_buf_pos_h;
    uint32_t memr_reg[DELAY_SLOT_NUM + 1];
    uint32_t memr_addr[DELAY_SLOT_NUM + 1];
    uint32_t memr_reg_addr[DELAY_SLOT_NUM + 1];
    uint32_t memr_buf_pos;
    uint32_t memw_reg[DELAY_SLOT_NUM + 1];
    uint64_t memw_result[DELAY_SLOT_NUM + 1];
    uint32_t memw_addr[DELAY_SLOT_NUM + 1];
    uint32_t memw_reg_addr[DELAY_SLOT_NUM + 1];
    uint32_t memw_buf_pos;
    uint32_t br_target;

    uint32_t header;
    uint32_t parallel;

    uint32_t buffer_pos, buffer_index;
    uint32_t sploop_begin, sploop_end, sploop_outside;
    uint32_t sploop_begin_paralleled;
    uint32_t sploop_flag, sploop_halt;
    uint32_t sploopw_cond;
    /* store the instruction */

    /* upper 32bit will be the header */
    void *bb_sploop_buffer[32];
    uint64_t legacy_sploop_buffer[32];
    uint32_t sploop_pc[32];
    uint32_t sploop_ii, sploop_curr_ii;
    uint32_t spmask;
    generic_address_t spmask_begin, spmask_end;

    /* save the result of WB */
    uint32_t wb_result[GPR_NUM_IN_GROUP * GPR_GROUP];
    uint32_t wb_index[GPR_NUM_IN_GROUP * GPR_GROUP];
    uint32_t wb_result_pos;
    uint32_t wb_flag;

    RWLOCK_T ifr_lock;

    addr_space_t *addr_space;
    uint32_t load_mask;
    generic_address_t load_addr;
    generic_address_t code_start_addr, code_end_addr;
    memory_space_intf *memory_space_iface;
    conf_object_t *memory_space;
    memory_space_intf *c6k_memory_intf;
    breakpoint_mgt_t bp_manager;
    conf_object_t *dyncom_cpu;

    RWLOCK_T fetch_lock;
    void *fetch_context;
    void *cache;
    int32_t pending_nops;               // pending implied nops if insn's in parallel
    void (*write_buffer) (struct c6k_core *, int, uint32_t);
    void (*register_branch_event) (struct c6k_core *, generic_address_t);
    void (*register_regw_event_l) (struct c6k_core *, uint32_t, int, int);
    void (*register_regw_event_h) (struct c6k_core *, uint32_t, int, int);
    void (*register_memory_event) (struct c6k_core *, uint64_t, uint32_t,
                                   generic_address_t, enum rw_flag, bool_t, uint32_t);
      uint32_t(*exec_insn_cache) (struct c6k_core *, struct insn_cache *);
    void (*cycle_inc) (struct c6k_core *, int);
    void (*add_pending_nops) (struct c6k_core *, int);

    uint32_t wake_flag;
    volatile uint32_t run_flag;
    core_running_mode_t mode;
    int debug_mode;
    int codecov;
    uint32_t freq_hz;
    uint32_t insn_cache_size;
    char exception_name[100];

} c6k_core_t;

#define WRITE_BUFFER		core->write_buffer
#define REGISTER_BRANCH_EVENT	core->register_branch_event
#define REGISTER_REGW_EVENT_L	core->register_regw_event_l
#define REGISTER_REGW_EVENT_H	core->register_regw_event_h
#define REGISTER_MEMORY_EVENT	core->register_memory_event
#define CYCLE_INC		core->cycle_inc
#define ADD_PENDING_NOPS	core->add_pending_nops

#define INIT_BR_TARGET(core) (core)->br_target = BAD_ADDR
#define INIT_WB_BUF(core) (core)->wb_result_pos = 0
#define INIT_REGW_BUF_L(core) (core)->regw_buf_pos_l = 0
#define INIT_REGW_BUF_H(core) (core)->regw_buf_pos_h = 0
#define INIT_MEMR_BUF(core) (core)->memr_buf_pos = 0
#define INIT_MEMW_BUF(core) (core)->memw_buf_pos = 0
#define REGW_PUSH_L(core, value, regno) { \
	assert((core)->regw_buf_pos_l < REGW_BUF_SIZE); \
	(core)->regw_regno_buf_l[(core)->regw_buf_pos_l] = (regno); \
	(core)->regw_regval_buf_l[(core)->regw_buf_pos_l] = (value); \
	(core)->regw_addr_l[(core)->regw_buf_pos_l] = (core)->pc; \
	(core)->regw_buf_pos_l++; }
#define REGW_POP_L(core, regaddr) { \
    uint32_t i, pos, rg, rn; \
    for (pos = 0; pos < (core)->regw_buf_pos_l; pos++) { \
        if((core)->regw_addr_l[pos] == regaddr){\
            rg = (core)->regw_regno_buf_l[pos] / GPR_NUM_IN_GROUP; \
                 rn = (core)->regw_regno_buf_l[pos] % GPR_NUM_IN_GROUP; \
                 (core)->gpr[rg][rn] = (core)->regw_regval_buf_l[pos]; \
                 for (i = pos; i < (core)->regw_buf_pos_l; i++) { \
                     (core)->regw_regno_buf_l[i] = (core)->regw_regno_buf_l[i + 1]; \
                     (core)->regw_addr_l[i] = (core)->regw_addr_l[i + 1]; \
                     (core)->regw_regval_buf_l[i] = (core)->regw_regval_buf_l[i + 1];\
                 }\
            (core)->regw_buf_pos_l--; \
            break;\
        }\
    }\
}
#define REGW_PUSH_H(core, value, regno) { \
	assert((core)->regw_buf_pos_h < REGW_BUF_SIZE); \
	(core)->regw_regno_buf_h[(core)->regw_buf_pos_h] = (regno); \
	(core)->regw_regval_buf_h[(core)->regw_buf_pos_h] = (value); \
	(core)->regw_addr_h[(core)->regw_buf_pos_h] = (core)->pc; \
	(core)->regw_buf_pos_h++; }
#define REGW_POP_H(core, regaddr) { \
    uint32_t i, pos, rg, rn; \
    for (pos = 0; pos < (core)->regw_buf_pos_h; pos++) { \
        if((core)->regw_addr_h[pos] == regaddr){\
            rg = (core)->regw_regno_buf_h[pos] / GPR_NUM_IN_GROUP; \
            rn = (core)->regw_regno_buf_h[pos] % GPR_NUM_IN_GROUP; \
            (core)->gpr[rg][rn] = (core)->regw_regval_buf_h[pos]; \
            for (i = pos; i < (core)->regw_buf_pos_h; i++) { \
                (core)->regw_regno_buf_h[i] = (core)->regw_regno_buf_h[i + 1]; \
                (core)->regw_addr_h[i] = (core)->regw_addr_h[i + 1]; \
                (core)->regw_regval_buf_h[i] = (core)->regw_regval_buf_h[i + 1];\
            }\
            (core)->regw_buf_pos_h--; \
            break;\
        }\
    }\
}
#define MEMR_PUSH(core, reg, addr) { \
	assert((core)->memr_buf_pos < DELAY_SLOT_NUM); \
	(core)->memr_reg[(core)->memr_buf_pos] = (reg); \
	(core)->memr_addr[(core)->memr_buf_pos] = (addr); \
	(core)->memr_reg_addr[(core)->memr_buf_pos] = (core)->pc; \
	(core)->memr_buf_pos++; }
#define SHOULD_MEMR_POP(core, regaddr) \
	(((core)->memr_buf_pos > 0) && ((core)->memr_reg_addr[0] == (regaddr)))
#define MEMR_POP(core, reg, addr) { \
	uint32_t i; \
	(reg) = (core)->memr_reg[0]; \
	(addr) = (core)->memr_addr[0]; \
	for (i = 0; i < (core)->memr_buf_pos; i++) { \
		(core)->memr_reg[i] = (core)->memr_reg[i + 1]; \
		(core)->memr_addr[i] = (core)->memr_addr[i + 1]; \
		(core)->memr_reg_addr[i] = (core)->memr_reg_addr[i + 1]; } \
	(core)->memr_buf_pos--; }
#define MEMW_PUSH(core, reg, addr, result) { \
	assert((core)->memw_buf_pos < DELAY_SLOT_NUM); \
	(core)->memw_reg[(core)->memw_buf_pos] = (reg); \
	(core)->memw_addr[(core)->memw_buf_pos] = (addr); \
	(core)->memw_result[(core)->memw_buf_pos] = (result); \
	(core)->memw_reg_addr[(core)->memw_buf_pos] = (core)->pc; \
	(core)->memw_buf_pos++; }
#define SHOULD_MEMW_POP(core, regaddr) \
	(((core)->memw_buf_pos > 0) && ((core)->memw_reg_addr[0] == (regaddr)))
#define MEMW_POP(core, reg, addr, result) { \
	uint32_t i; \
	(reg) = (core)->memw_reg[0]; \
	(addr) = (core)->memw_addr[0]; \
	(result) = (core)->memw_result[0]; \
	for (i = 0; i < (core)->memw_buf_pos; i++) { \
		(core)->memw_reg[i] = (core)->memw_reg[i + 1]; \
		(core)->memw_addr[i] = (core)->memw_addr[i + 1]; \
		(core)->memw_result[i] = (core)->memw_result[i + 1]; \
		(core)->memw_reg_addr[i] = (core)->memw_reg_addr[i + 1]; } \
	(core)->memw_buf_pos--; }

#define INIT_BUFFERS(core) { \
	INIT_BR_TARGET(core); \
	INIT_WB_BUF(core); \
	INIT_REGW_BUF_L(core); \
	INIT_REGW_BUF_H(core); \
	INIT_MEMR_BUF(core); \
	INIT_MEMW_BUF(core); }

#define BK0 1
#define BK1 2

typedef enum
{
    L1_UNIT = 0,
    L2_UNIT,
    S1_UNIT,
    S2_UNIT,
    M1_UNIT,
    M2_UNIT,
    D1_UNIT,
    D2_UNIT
} function_unit_t;

#define FP_SIZE 8
#define BAD_ADDR 0xFFFFFFFF

#define TSR_RESET(core) 		(core)->tsr = 0x0
#define CSR_RESET(core) 		(core)->csr = 0x0

#define CSR_SET_GIE(core, value)	(core)->csr = (value) ? (core)->csr | 0x1 : (core)->csr & 0xFFFFFFFE
#define TSR_SET_GIE(core, value)	(core)->tsr = (value) ? (core)->tsr | 0x1 : (core)->tsr & 0xFFFFFFFE
#define TSR_GET_GIE(core)		(core)->tsr &0x1
#define TSR_SET_SGIE(core, value)	(core)->tsr = (value) ? (core)->tsr | 0x2 : (core)->tsr & 0xFFFFFFFD
#define TSR_GET_SGIE(core)		((core)->tsr >> 1) & 0x1

#ifdef __cplusplus
extern "C"
{
#endif

    exception_t c6k_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
    exception_t c6k_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
    get_page_t *c6k_space_get_page(conf_object_t * obj, generic_address_t addr);
    exception_t c6k_multicore_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
    exception_t c6k_multicore_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
    get_page_t *c6k_multicore_space_get_page(conf_object_t * obj, generic_address_t addr);

    void c6k_stepi(conf_object_t * obj, uint64_t steps);
    void c6k_step_once(conf_object_t * conf_obj);
    uint32_t c6k_per_cpu_step(conf_object_t * obj);
    exception_t c6k_signal(conf_object_t * conf_obj, interrupt_signal_t * signal);
    conf_object_t *new_c6k_core(const char *obj_name);

    exception_t config_c6k_core(conf_object_t * obj);
    void free_c6k_core(conf_object_t * dev);
    void c6k_register_attribute(conf_class_t * clss);
    char *c6k_disassemble(conf_object_t * obj, generic_address_t addr);
    char *c6k_get_architecture(conf_object_t * obj);
    int c6k_insert_bp(conf_object_t * conf_obj, generic_address_t addr);
    int c6k_delete_bp(conf_object_t * conf_obj, generic_address_t addr);
    uint32_t c6k_get_bp_numbers(conf_object_t * conf_obj);
    uint32_t c6k_get_bp_addr_by_id(conf_object_t * conf_obj, int id);
    int c6k_get_trigger(conf_object_t * conf_obj);
    bool_t c6k_check_bp_trigger(conf_object_t * conf_obj);
    int c6k_clear_bp_trigger(conf_object_t * conf_obj);
    exception_t c6k_memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index);
    exception_t c6k_memory_space_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index);
    void c6k_set_pc(conf_object_t * obj, generic_address_t addr);
    void c6k_set_textsection_info(conf_object_t * obj, generic_address_t code_start_addr,
                                  generic_address_t code_end_addr);
    void c6k_do_pre_translate(conf_object_t * obj);
    generic_address_t c6k_get_pc(conf_object_t * obj);
    uint64_t c6k_get_step(conf_object_t * conf_obj);
    endian_t c6k_get_endian(conf_object_t * conf_obj);
    align_t c6k_get_alignment(conf_object_t * obj);
    void c6k_reset(conf_object_t * obj);
    exception_t reset_c6k_core(conf_object_t * obj, const char *parameter);

    uint32_t c6k_get_address_width(conf_object_t * obj);
    void c6k_get_cpu_run_status(conf_object_t * obj, uint32_t * cpu_freq, uint64_t * cpu_insn_num);
    void c6k_set_memory_watch_status(conf_object_t * obj, uint32_t addr, int status);
    int c6k_get_cpu_exception_num(conf_object_t * obj);
    void c6k_set_cpu_exception_by_id(conf_object_t * obj, int exception_id);
    char *c6k_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id);
    struct insn_cache *fetch_packet_to_cache(conf_object_t * conf_obj, uint32_t pc);

#define C6K_IRQ_NUM 16
#ifdef __cplusplus
}
#endif
#endif
