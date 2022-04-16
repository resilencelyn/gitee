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
* @file c6k_decode.h
* @brief some definition for decoder
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-07-31
*/

#ifndef __DECODER_H__
#define __DECODER_H__
#include <inttypes.h>
#include "c6k_cpu.h"
typedef struct _opcode_fields
{
    uint32_t op1_11;
    uint32_t op2_11;
    uint32_t op4_6;
    uint32_t op5_11;
    uint32_t op6_11;
    uint32_t op6_9;
    uint32_t op7_11;
    uint32_t op7_12;

    uint32_t src13_15;
    uint32_t src13_16;
    uint32_t src13_17;

    uint32_t src18_22;
    uint32_t src16_27;

    uint32_t dst23_27;
    uint32_t dst24_27;
    uint32_t s;
    uint32_t x;
    uint32_t y;
    uint32_t p;
    uint32_t sc;
    uint32_t ii;
    uint32_t mode;

    uint32_t offset;
    uint32_t base;

    uint32_t scst7_22;
    uint32_t scst7_27;
    uint32_t scst16_22;
    uint32_t scst18_22;

    uint32_t cst7_22;

    uint32_t ucst13_15;
    uint32_t ucst8_22;

    uint32_t csta;
    uint32_t cstb;
    uint32_t crlo18_22;
    uint32_t crlo23_27;
    uint32_t fstg_fcyc;

    /* exec_unit is used to specify the insn execution unit
     * this is mainly referenced by spmask to filter out masked insns
     */
    uint16_t exec_unit;
    uint16_t has_compact_fields;
    uint32_t compact_p:1, compact_sat:1, compact_br:1, compact_dsz:3, compact_rs:1, compact_prot:1;
} opcode_fields_t;

typedef int (*insn_action_t) (c6k_core_t * core, uint32_t insn, opcode_fields_t * fields);
struct instruction_set_encoding_item
{
    const char *name;
    int attribute_value;
    int version;
    int content[21];

    uint64_t delay_slot;

    /* in compact mode, LD/ST are encoded together,
     * but they have different memory accessing delay,
     * st takes 2 cycles delay, and ld takes 4
     */
#define DOUBLE_SLOT		0x8000000000000000LL
#define MIXED_LS_SLOT		0x4000000000000000LL
#define LD_SLOT			0x2000000000000000LL
#define ST_SLOT			0x1000000000000000LL
#define B_SLOT			0x0800000000000000LL    // no implied nops
#define BNOP_SLOT		0x0400000000000000LL    // pending nops
#define CALLP_SLOT		0x0200000000000000LL    // implied 5 nops
#define NOP_SLOT		0x0100000000000000LL
#define KPC_SLOT		0x0080000000000000LL
#define SGL_SLOT		0x0040000000000000LL
#define SGH_SLOT		0x0020000000000000LL
#define SGDW_SLOT		0x0010000000000000LL    // double registers in the same cycle
#define FOUR_SLOT		0x0008000000000000LL    // Four registers in the same cycle
#define BR			0x0000000000F00000LL
#define SGL			0x00000000000F0000LL
#define SGH 			0x000000000000F000LL
#define ST 			0x0000000000000F00LL
#define LD 			0x00000000000000F0LL
#define NP 			0x000000000000000FLL

#define GET_CNT_OF(slot, stype, offset)	((slot) & (stype)) >> ((offset))
#define GET_NP_CNT(slot)  GET_CNT_OF((slot), NP, 0)
#define GET_LD_CNT(slot)  GET_CNT_OF((slot), LD, 4)
#define GET_ST_CNT(slot)  GET_CNT_OF((slot), ST, 8)
#define GET_SGH_CNT(slot) GET_CNT_OF((slot), SGH, 12)
#define GET_SGL_CNT(slot) GET_CNT_OF((slot), SGL, 16)
#define GET_BR_CNT(slot) GET_CNT_OF((slot), BR, 20)
#define SET_AND_ACCUM_CNT_OF(slot, sname, stype, num, offset) { \
	(slot) |= (sname); \
	(slot) = ((((((slot) & (stype)) >> (offset)) + (num)) << (offset)) & (stype)) | ((slot) & ~(stype)); }
#define SET_NP_SLOT(slot, num)	SET_AND_ACCUM_CNT_OF((slot), NOP_SLOT, NP, (num), 0)
#define SET_LD_SLOT(slot, num)	SET_AND_ACCUM_CNT_OF((slot), LD_SLOT, LD, (num), 4)
#define SET_ST_SLOT(slot, num)	SET_AND_ACCUM_CNT_OF((slot), ST_SLOT, ST, (num), 8)
#define SET_SGL_SLOT(slot, num)	SET_AND_ACCUM_CNT_OF((slot), SGL_SLOT, SGL, (num), 16)
#define SET_SGH_SLOT(slot, num)	SET_AND_ACCUM_CNT_OF((slot), SGH_SLOT, SGH, (num), 12)
#define SET_BR_SLOT(slot, num)  SET_AND_ACCUM_CNT_OF((slot), B_SLOT, BR, (num), 20)
#define CLR_SLOT_OF(slot, sname, stype) { \
	(slot) = (slot) & (~(stype)); (slot) &= ~(sname); }
#define CLR_NP_SLOT(slot) CLR_SLOT_OF((slot), NOP_SLOT, NP)
#define CLR_LD_SLOT(slot) CLR_SLOT_OF((slot), LD_SLOT, LD)
#define CLR_ST_SLOT(slot) CLR_SLOT_OF((slot), ST_SLOT, ST)
#define CLR_BR_SLOT(slot) CLR_SLOT_OF((slot), B_SLOT, BR)
#define CLR_SGL_SLOT(slot) CLR_SLOT_OF((slot), SGL_SLOT, SGL)
#define CLR_SGH_SLOT(slot) CLR_SLOT_OF((slot), SGH_SLOT, SGH)
#define SLOT_TYPE_MASK	0xFFFFFFFF00000000LL
#define SLOT_CNT_MASK	0x00000000FFFFFFFFLL
};

typedef struct instruction_set_encoding_item ISEITEM;

enum DECODE_STATUS
{
    DECODE_SUCCESS,
    DECODE_FAILURE
};

enum INSN_EXECUTE_UNIT
{
    UNIT_L1 = 0x1,
    UNIT_L2 = 0x2,
    UNIT_S1 = 0x4,
    UNIT_S2 = 0x8,
    UNIT_D1 = 0x10,
    UNIT_D2 = 0x20,
    UNIT_M1 = 0x40,
    UNIT_M2 = 0x80,
};

#define ASSIGN_EXEC_UNIT(fields, unit, side)	(fields)->exec_unit = (1 << ((unit) * 2) + (s))

#define ASSEMBLE_COMPACT_SPMASK(insn) \
	(((insn) & 0x1) | (((insn) >> 6) & 0xe) | (((insn) >> 10) & 0x30))

#define BITS(a,b) ((insn >> (a)) & ((1 << (1+(b)-(a)))-1))
#define BIT(n) ((instr >> (n)) & 1)
#define SIGN_EXTEND(cst, n) ((cst & (1 << (n - 1))) ? (cst | (0xFFFFFFFF << n)) : cst)
#define DSZ(header) ((header >> 16) & 0x7)

#define INSN_LDS_UNDER_PROTECTED(fields) ((fields)->has_compact_fields && (fields)->compact_prot)

enum insn_types
{
    INSN_REGULAR = 1,
    INSN_COMPACT,
    INSN_HEADER,
    INSN_CHECKPOINT_WB,
    INSN_CHECKPOINT_LD,
    INSN_CHECKPOINT_ST,
    INSN_CHECKPOINT_B,
    INSN_CHECKPOINT_REGWL,
    INSN_CHECKPOINT_REGWH,
};

#define BR_INSN_IN_UNIT_S	0x72627369      // isbr
#define NONBR_INSN_IN_UNIT_S	0x72626f63      // nobr

typedef struct insn_cache
{
    uint32_t insn;
    insn_action_t action;
    uint32_t addr;
    opcode_fields_t fields;
    uint32_t header;
    enum insn_types type;

    uint32_t parallel;
    uint32_t index;
    uint32_t size;

    /*
     * uint32_t log_point;
     * uint32_t br_dsq_idx;
     * uint32_t br_buf_pos;
     * uint32_t br_reg_addr[DELAY_SLOT_NUM];
     * uint64_t br_dsq[DELAY_SLOT_NUM];
     */
} insn_cache_t;

#define EXTEND_SIGN(var, bits)  \
	(((typeof((var)))1) << ((bits) - 1)) & (var) ? ((var) | (((typeof((var)))-1) << (bits))) : (var)

#define TRANS_COMPACT_FIELDS(fields, p, exp) {	\
		uint32_t insn = (exp);	\
		fields.has_compact_fields = 1; \
		fields.compact_p = (p); \
		fields.compact_sat = BITS(0, 0); \
		fields.compact_br = BITS(1, 1);	\
		fields.compact_dsz = BITS(2, 4);	\
		fields.compact_rs = BITS(5, 5);	\
		fields.compact_prot = BITS(6, 6); }

#define GET_DW_FULL(reg_even, reg_odd, varname, type) \
		type varname;{ \
		uint64_t *p = (uint64_t*)&varname; \
		uint64_t tmp = ((uint64_t)reg_odd) << 32; \
		tmp |= (uint64_t)reg_even; \
		*p = tmp; }
#define GET_DW_DOUBLE(reg_even, reg_odd, varname)	\
		GET_DW_FULL(reg_even, reg_odd, varname, double)
#define GET_DW_UINTEGER(reg_even, reg_odd, varname)	\
		GET_DW_FULL(reg_even, reg_odd, varname, uint64_t)
#define CVT_DW_TO_WORDS(db, word_low, word_high) \
		uint32_t word_low, word_high; { \
		uint64_t *p = (uint64_t*)&db; \
		word_low = (uint32_t)*p; \
		word_high = (uint32_t)(*p >> 32); }
#define OPSIZE_IS_DW(insn)	((insn >> 28) == 0x1)

typedef enum bb_type
{
    SIMPLE_BB,
    LEGACY_BB,
} bb_type_t;

typedef struct basic_block
{
    bb_type_t type;
    insn_cache_t *start;
    insn_cache_t *end;
    uint32_t len;
    uint32_t sploop_include;
} basic_block_t;

typedef struct fetch_context
{
#define INVALID_CURR_BB(fc)	((fc)->cache_size - 1)
    uint64_t delay_slot_queue[DELAY_SLOT_NUM];
    uint32_t curr_dsq_idx;
    uint32_t block_complete;
    uint32_t curr_bb;
    insn_cache_t *curr_ic, *curr_bb_ic;
    insn_cache_t *insn_cache;
    basic_block_t *bb_cache;
    uint32_t cache_size;
    uint32_t in_parallel;
    uint32_t memr_reg_addr[DELAY_SLOT_NUM + 1];
    uint32_t memr_buf_pos;
    uint32_t memw_reg_addr[DELAY_SLOT_NUM + 1];
    uint32_t memw_buf_pos;
    uint32_t regwl_reg_addr[DELAY_SLOT_NUM + 1];
    uint32_t regwl_cycle[DELAY_SLOT_NUM + 1];
    uint32_t regwl_buf_pos;
    uint32_t regwh_reg_addr[DELAY_SLOT_NUM + 1];
    uint32_t regwh_cycle[DELAY_SLOT_NUM + 1];
    uint32_t regwh_buf_pos;
    uint32_t br_reg_addr[DELAY_SLOT_NUM + 1];
    uint32_t br_buf_pos;
} fetch_context_t;

#define SET_PARALLEL(fc, p) (fc)->in_parallel = (p)
#define IS_PARALLELED(fc) ((fc)->in_parallel)
#define INIT_FC(fc, be) { \
	uint32_t i; \
	(fc)->curr_dsq_idx = 0; \
	(fc)->memr_buf_pos = 0; \
	(fc)->memw_buf_pos = 0; \
	(fc)->regwl_buf_pos = 0; \
	(fc)->regwh_buf_pos = 0; \
	for (i = 0; i < DELAY_SLOT_NUM; i++) {\
		(fc)->delay_slot_queue[i] = 0; \
		(fc)->memr_reg_addr[i] = 0; \
		(fc)->memw_reg_addr[i] = 0; \
		(fc)->regwl_reg_addr[i] = 0; \
		(fc)->regwh_reg_addr[i] = 0; \
		(fc)->br_reg_addr[i] = 0; } \
	(fc)->bb_cache[(be)].start = (fc)->curr_ic; \
	(fc)->bb_cache[(be)].sploop_include = 0; \
	(fc)->block_complete = 0; }

#define CACHE_SIZE 0x100000
#define CACHE_FULL -1

//#define NEXT_IC(fc) (fc)->curr_ic++
static inline int INC_CURR_IC(fetch_context_t * fc)
{
    if (fc->curr_ic < (fc->insn_cache + (fc->cache_size - 1)))
    {
        fc->curr_ic++;
        return 0;
    } else
    {
        //cache is full, clear bb cache and insn cache, set curr_ic to insn_cache first addr
        memset(fc->bb_cache, 0, sizeof (basic_block_t) * fc->cache_size);
        memset(fc->insn_cache, 0, sizeof (insn_cache_t) * fc->cache_size);
        fc->curr_ic = fc->insn_cache;
        skyeye_log(Info_log, __FUNCTION__, "insn_cache is full!\n");
        return CACHE_FULL;
    }
}

#define GET_CURR_IC(fc) (fc->curr_ic)
#define SET_CURR_BB(fc, addr) (fc)->curr_bb = (addr)
#define GET_CURR_BB(fc) (fc)->curr_bb
#define SET_CURR_BB_TYPE(fc, t) (fc)->bb_cache[(fc)->curr_bb].type = (t);
#define SET_BB_TYPE(fc, t,addr) (fc)->bb_cache[addr].type = (t);
#define SET_BB_SPLOOP(fc, t,addr) (fc)->bb_cache[addr].sploop_include = (t);
#define GET_CURR_BB_TYPE(fc) ((fc)->bb_cache[(fc)->curr_bb].type)
#define GET_BB_TYPE(fc,addr) ((fc)->bb_cache[addr].type)
#define GET_BB_SPLOOP(fc,addr) ((fc)->bb_cache[addr].sploop_include)
#define GET_BB_ENTRY(fc, addr) (fc)->bb_cache[(addr)].start
#define GET_BB_TAIL(fc, addr) (fc)->bb_cache[(addr)].end
#define SET_BB_TAIL(fc, cache,addr) (fc)->bb_cache[addr].end = (cache);
#define GET_CURR_BB_ENTRY(fc) (fc)->bb_cache[(fc)->curr_bb].start
#define SET_CURR_BB_TAIL(fc, cache) (fc)->bb_cache[(fc)->curr_bb].end = (cache);
#define GET_CURR_BB_TAIL(fc) ((fc)->bb_cache[(fc)->curr_bb].end)
#define SET_BB_IC(fc, fptc) (fc)->curr_bb_ic = (fptc);
#define GET_BB_IC(fc) ((fc)->curr_bb_ic)
#define NEXT_BB_IC(fc)	(fc)->curr_bb_ic++
#define BB_BEGIN(fc) (fc)->block_complete = 0;
#define BB_END(fc) (fc)->block_complete = 1;
#define BB_COMPLETED(fc) ((fc)->block_complete == 1)
#define FC_MEMR_PUSH(fc, regaddr) { \
	assert((fc)->memr_buf_pos < DELAY_SLOT_NUM); \
	(fc)->memr_reg_addr[(fc)->memr_buf_pos] = (regaddr); \
	(fc)->memr_buf_pos++; }
#define FC_MEMR_POP(fc, addr) { \
	uint32_t i; \
	(addr) = (fc)->memr_reg_addr[0]; \
	for (i = 0; i < (fc)->memr_buf_pos; i++) { \
		(fc)->memr_reg_addr[i] = (fc)->memr_reg_addr[i + 1]; } \
	(fc)->memr_buf_pos--; }
#define FC_MEMW_PUSH(fc, regaddr) { \
	assert((fc)->memw_buf_pos < DELAY_SLOT_NUM); \
	(fc)->memw_reg_addr[(fc)->memw_buf_pos] = (regaddr); \
	(fc)->memw_buf_pos++; }

#define FC_MEMW_POP(fc, addr) { \
	uint32_t i; \
	(addr) = (fc)->memw_reg_addr[0]; \
	for (i = 0; i < (fc)->memw_buf_pos; i++) { \
		(fc)->memw_reg_addr[i] = (fc)->memw_reg_addr[i + 1]; } \
	(fc)->memw_buf_pos--; }

#define FC_REGWL_PUSH(fc, regaddr, push_cycle) { \
	assert((fc)->regwl_buf_pos < DELAY_SLOT_NUM); \
	(fc)->regwl_reg_addr[(fc)->regwl_buf_pos] = (regaddr); \
    (fc)->regwl_cycle[(fc)->regwl_buf_pos] = (push_cycle); \
	(fc)->regwl_buf_pos++; }
#define FC_REGWL_POP(fc, addr, curr_cycle) { \
    uint32_t i, pos; \
    for(pos = 0; pos < (fc)->regwl_buf_pos; pos++){\
        if(curr_cycle == (fc)->regwl_cycle[pos]){ \
            (addr) = (fc)->regwl_reg_addr[pos]; \
            for (i = pos; i < (fc)->regwl_buf_pos; i++) { \
                (fc)->regwl_reg_addr[i] = (fc)->regwl_reg_addr[i + 1]; \
                (fc)->regwl_cycle[i] = (fc)->regwl_cycle[i + 1];  \
            }\
            (fc)->regwl_buf_pos--; \
            break;\
        }\
    }\
}
#define FC_REGWH_PUSH(fc, regaddr, push_cycle) { \
	assert((fc)->regwh_buf_pos < DELAY_SLOT_NUM); \
	(fc)->regwh_reg_addr[(fc)->regwh_buf_pos] = (regaddr); \
    (fc)->regwh_cycle[(fc)->regwh_buf_pos] = (push_cycle); \
	(fc)->regwh_buf_pos++; }

#define FC_REGWH_POP(fc, addr, curr_cycle) { \
    uint32_t i, pos; \
    for(pos = 0; pos < (fc)->regwh_buf_pos; pos++){\
        if(curr_cycle == (fc)->regwh_cycle[pos]){ \
            (addr) = (fc)->regwh_reg_addr[pos]; \
            for (i = pos; i < (fc)->regwh_buf_pos; i++) { \
                (fc)->regwh_reg_addr[i] = (fc)->regwh_reg_addr[i + 1]; \
                (fc)->regwh_cycle[i] = (fc)->regwh_cycle[i + 1];  \
            }\
            (fc)->regwh_buf_pos--; \
            break;\
        }\
    }\
}
#define FC_BR_PUSH(fc, regaddr) { \
	assert((fc)->br_buf_pos < DELAY_SLOT_NUM); \
	(fc)->br_reg_addr[(fc)->br_buf_pos] = (regaddr); \
	(fc)->br_buf_pos++; }
#define FC_BR_POP(fc, addr) { \
	uint32_t i; \
	(addr) = (fc)->br_reg_addr[0]; \
	for (i = 0; i < (fc)->br_buf_pos; i++) { \
		(fc)->br_reg_addr[i] = (fc)->br_reg_addr[i + 1]; } \
	(fc)->br_buf_pos--; }

#define INSN_CACHE_SIZE (65536*10)
//#define INSN_CACHE_SIZE (0x1FF000)
#define HASH_BB(addr) ((addr >> 2) % INSN_CACHE_SIZE)
static inline uint32_t HASH(fetch_context_t * fc, uint32_t addr)
{
    insn_cache_t *bb_start = GET_BB_ENTRY(fc, HASH_BB(addr));

    if (bb_start == NULL)               /* cache is blank */
        return HASH_BB(addr);
    if (bb_start->addr != addr)
    {
        /* HASH conflicated, we need to invalided all bb cache */
        memset(fc->bb_cache, 0, sizeof (basic_block_t) * fc->cache_size);
        DBG("!!!!!!!!!! cache conflicated, bb_start=0x%x, addr=0x%x\n", bb_start->addr, addr);
    }
    return HASH_BB(addr);
}

#ifdef __cplusplus
extern "C"
{
#endif

    insn_action_t get_insn_action(c6k_core_t * core, uint32_t insn, int32_t * index, int64_t * delay_slot);
    insn_action_t get_insn16_action(c6k_core_t * core, uint32_t insn, int br, int32_t * index, int64_t * delay_slot);
    uint32_t bb_exec_insn_cache(c6k_core_t * core, insn_cache_t * cache);
    uint32_t legacy_exec_insn_cache(c6k_core_t * core, insn_cache_t * cache);

    int decode_instr(uint32_t insn, int32_t * idx, ISEITEM * table, int table_len, int br, int64_t * delay_slot);
    void write_buffer(c6k_core_t * core, int regno, uint32_t result);
    void legacy_register_branch_event(c6k_core_t * core, generic_address_t target_addr);
    void bb_register_branch_event(c6k_core_t * core, generic_address_t target_addr);
    void legacy_register_memory_event(c6k_core_t * core, uint64_t result, uint32_t reg_no,
                                      generic_address_t addr, rw_flag_t rw_flag, bool_t sign_ext, uint32_t width);
    void bb_register_memory_event(c6k_core_t * core, uint64_t result, uint32_t reg_no,
                                  generic_address_t addr, rw_flag_t rw_flag, bool_t sign_ext, uint32_t width);
    void legacy_register_regw_event(c6k_core_t * core, uint32_t value, int regno, int unused);
    void bb_register_regw_event_l(c6k_core_t * core, uint32_t value, int regno, int unused);
    void bb_register_regw_event_h(c6k_core_t * core, uint32_t value, int regno, int unused);
    void legacy_cycle_inc(c6k_core_t * core, int n);
    void bb_cycle_inc(c6k_core_t * core, int n);
    int exec_16b_insn(c6k_core_t * core, uint32_t insn, opcode_fields_t * fields);
    void lds_save_result(c6k_core_t * core, opcode_fields_t * fields, uint64_t result,
                                uint32_t reg_no, generic_address_t addr,
                                rw_flag_t rw_flag, bool_t sign_ext, uint32_t width);
    void legacy_add_pending_nops(c6k_core_t * core, int cycles);
    void bb_add_pending_nops(c6k_core_t * core, int cycles);
    void check_event(c6k_core_t * core);
    int exec_write_back(c6k_core_t * core, uint32_t unused, opcode_fields_t * unused2);
    int exec_regw_l(c6k_core_t * core, uint32_t num, opcode_fields_t * unused);
    int exec_regw_h(c6k_core_t * core, uint32_t num, opcode_fields_t * unused);
    int exec_read_memory(c6k_core_t * core, uint32_t num, opcode_fields_t * unused);
    int exec_write_memory(c6k_core_t * core, uint32_t num, opcode_fields_t * unused);
    void invalidate_bb_cache(c6k_core_t * core);
    int check_amr_mode(c6k_core_t * core, int s, int src_mode);

    int trans_opcode_fields(uint32_t insn, opcode_fields_t * fields);
    int get_insn_index(uint32_t insn);
    int get_insn_number();
    char *get_insn_name(int index);
    int32_t get_insn_delay_slot(uint32_t insn);
    int get_insn16_index(uint32_t insn);
    int get_insn16_number();
    char *get_insn16_name(int index);
    int32_t get_insn16_delay_slot(uint32_t insn);
    int core_read(c6k_core_t * core, int width, generic_address_t addr, uint32_t * result);
    int core_write(c6k_core_t * core, int width, generic_address_t addr, uint32_t value);

#ifdef __cplusplus
}
#endif

#define MSBn(utype, src, n) \
	({assert(sizeof(utype) == sizeof(src)); \
	 (utype)(src) >> (sizeof((src)) * 8 - (n));})
#define LSBn(utype, src, n) \
	({assert(sizeof(utype) == sizeof(src)); \
	 utype mask = n >= sizeof(src) * 8 ? (utype)-1 : (utype)(1 << (n)) - 1; \
	 (src) & mask;})

typedef struct
{
    unsigned high;
    unsigned low;
} dword;

#define DOUBLE64_TO_DWORD(from, to)                                        \
do {                                                                    \
    to.high = (unsigned) ((*((unsigned long long  *) (&from)) >> 32) & 0xffffffff);   \
    to.low  = (unsigned) (*((unsigned long long  *) (&from)) & 0xffffffff);           \
} while (0)

#define DWORD_TO_DOUBLE64(from, to)                              \
do {                                                          \
    unsigned long long  _tmp;                                            \
    _tmp = (unsigned long long ) from.high << 32 | from.low;             \
    to = *((double *) (&_tmp));                              \
} while (0)

static inline int snan_chk_dp(dword op)
{

    if (((op.high & 0x7ff80000) == 0x7ff00000) && ((op.high & 0x0007ffff) || op.low))
    {
        return 1;
    } else
    {
        return 0;
    }
}

static inline int qnan_chk_dp(dword op)
{

    if ((op.high & 0x7ff80000) == 0x7ff80000)
    {
        return 1;
    } else
    {
        return 0;
    }
}

static inline int extract_exp_dp(dword op)
{
    return ((op.high & 0x7ff00000) >> 20);
}

static inline int inf_chk_dp(dword op)
{

    if (((op.high & 0x7ff00000) == 0x7ff00000) && !(op.high & 0x000fffff) && !op.low)
    {
        return 1;
    } else
    {
        return 0;
    }
}
static inline int denorm_chk_dp(dword op)
{

    if (!(op.high & 0x7ff00000) && ((op.high & 0x000fffff) || op.low))
    {
        return 1;
    } else
    {
        return 0;
    }
}

static inline dword extract_man_dp(dword op)
{

    dword result;

    result.low = op.low;
    result.high = (op.high & 0x000fffff) | 0x00100000;
    return (result);
}

static inline unsigned rcp(unsigned index)
{
    static unsigned char reciprocalTable[] = {
        0x00, 0xfe, 0xfc, 0xfa, 0xf8, 0xf6, 0xf4, 0xf2,
        0xf0, 0xef, 0xed, 0xeb, 0xe9, 0xe7, 0xe5, 0xe4,
        0xe2, 0xe0, 0xde, 0xdd, 0xdb, 0xd9, 0xd7, 0xd6,
        0xd4, 0xd2, 0xd1, 0xcf, 0xce, 0xcc, 0xca, 0xc9,
        0xc7, 0xc6, 0xc4, 0xc2, 0xc1, 0xbf, 0xbe, 0xbc,
        0xbb, 0xb9, 0xb8, 0xb6, 0xb5, 0xb3, 0xb2, 0xb1,
        0xaf, 0xae, 0xac, 0xab, 0xaa, 0xa8, 0xa7, 0xa5,
        0xa4, 0xa3, 0xa1, 0xa0, 0x9f, 0x9d, 0x9c, 0x9b,
        0x9a, 0x98, 0x97, 0x96, 0x95, 0x93, 0x92, 0x91,
        0x90, 0x8e, 0x8d, 0x8c, 0x8b, 0x8a, 0x88, 0x87,
        0x86, 0x85, 0x84, 0x83, 0x82, 0x80, 0x7f, 0x7e,
        0x7d, 0x7c, 0x7b, 0x7a, 0x79, 0x78, 0x76, 0x75,
        0x74, 0x73, 0x72, 0x71, 0x70, 0x6f, 0x6e, 0x6d,
        0x6c, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65,
        0x64, 0x63, 0x62, 0x61, 0x60, 0x5f, 0x5e, 0x5e,
        0x5d, 0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56,
        0x55, 0x54, 0x54, 0x53, 0x52, 0x51, 0x50, 0x4f,
        0x4e, 0x4e, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x49,
        0x48, 0x47, 0x46, 0x45, 0x44, 0x44, 0x43, 0x42,
        0x41, 0x40, 0x40, 0x3f, 0x3e, 0x3d, 0x3d, 0x3c,
        0x3b, 0x3a, 0x3a, 0x39, 0x38, 0x37, 0x37, 0x36,
        0x35, 0x34, 0x34, 0x33, 0x32, 0x32, 0x31, 0x30,
        0x2f, 0x2f, 0x2e, 0x2d, 0x2d, 0x2c, 0x2b, 0x2b,
        0x2a, 0x29, 0x29, 0x28, 0x27, 0x27, 0x26, 0x25,
        0x25, 0x24, 0x23, 0x23, 0x22, 0x21, 0x21, 0x20,
        0x1f, 0x1f, 0x1e, 0x1e, 0x1d, 0x1c, 0x1c, 0x1b,
        0x1a, 0x1a, 0x19, 0x19, 0x18, 0x17, 0x17, 0x16,
        0x16, 0x15, 0x15, 0x14, 0x13, 0x13, 0x12, 0x12,
        0x11, 0x10, 0x10, 0x0f, 0x0f, 0x0e, 0x0e, 0x0d,
        0x0d, 0x0c, 0x0b, 0x0b, 0x0a, 0x0a, 0x09, 0x09,
        0x08, 0x08, 0x07, 0x07, 0x06, 0x06, 0x05, 0x05,
        0x04, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01
    };

    return (reciprocalTable[index]);
}

static unsigned rcpsq(unsigned index)
{
    static unsigned char sqrtReciprocalTable[] = {
        0x6a, 0x69, 0x69, 0x68, 0x67, 0x67, 0x66, 0x65,
        0x65, 0x64, 0x63, 0x63, 0x62, 0x61, 0x61, 0x60,
        0x5f, 0x5f, 0x5e, 0x5d, 0x5d, 0x5c, 0x5b, 0x5b,
        0x5a, 0x5a, 0x59, 0x58, 0x58, 0x57, 0x57, 0x56,
        0x55, 0x55, 0x54, 0x54, 0x53, 0x52, 0x52, 0x51,
        0x51, 0x50, 0x50, 0x4f, 0x4e, 0x4e, 0x4d, 0x4d,
        0x4c, 0x4c, 0x4b, 0x4b, 0x4a, 0x4a, 0x49, 0x48,
        0x48, 0x47, 0x47, 0x46, 0x46, 0x45, 0x45, 0x44,
        0x44, 0x43, 0x43, 0x42, 0x42, 0x41, 0x41, 0x40,
        0x40, 0x3f, 0x3f, 0x3e, 0x3e, 0x3d, 0x3d, 0x3c,
        0x3c, 0x3c, 0x3b, 0x3b, 0x3a, 0x3a, 0x39, 0x39,
        0x38, 0x38, 0x37, 0x37, 0x37, 0x36, 0x36, 0x35,
        0x35, 0x34, 0x34, 0x33, 0x33, 0x33, 0x32, 0x32,
        0x31, 0x31, 0x30, 0x30, 0x30, 0x2f, 0x2f, 0x2e,
        0x2e, 0x2e, 0x2d, 0x2d, 0x2c, 0x2c, 0x2c, 0x2b,
        0x2b, 0x2a, 0x2a, 0x2a, 0x29, 0x29, 0x28, 0x28,
        0x28, 0x27, 0x27, 0x26, 0x26, 0x26, 0x25, 0x25,
        0x25, 0x24, 0x24, 0x23, 0x23, 0x23, 0x22, 0x22,
        0x22, 0x21, 0x21, 0x21, 0x20, 0x20, 0x1f, 0x1f,
        0x1f, 0x1e, 0x1e, 0x1e, 0x1d, 0x1d, 0x1d, 0x1c,
        0x1c, 0x1c, 0x1b, 0x1b, 0x1b, 0x1a, 0x1a, 0x1a,
        0x19, 0x19, 0x19, 0x18, 0x18, 0x18, 0x17, 0x17,
        0x17, 0x16, 0x16, 0x16, 0x15, 0x15, 0x15, 0x14,
        0x14, 0x14, 0x14, 0x13, 0x13, 0x13, 0x12, 0x12,
        0x12, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10, 0x10,
        0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e, 0x0d, 0x0d,
        0x0d, 0x0d, 0x0c, 0x0c, 0x0c, 0x0b, 0x0b, 0x0b,
        0x0b, 0x0a, 0x0a, 0x0a, 0x0a, 0x09, 0x09, 0x09,
        0x08, 0x08, 0x08, 0x08, 0x07, 0x07, 0x07, 0x06,
        0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x05, 0x04,
        0x04, 0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x02,
        0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x00,
        0x00, 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9,
        0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf3, 0xf2,
        0xf1, 0xf0, 0xef, 0xee, 0xed, 0xec, 0xeb, 0xea,
        0xea, 0xe9, 0xe8, 0xe7, 0xe6, 0xe5, 0xe4, 0xe4,
        0xe3, 0xe2, 0xe1, 0xe0, 0xdf, 0xdf, 0xde, 0xdd,
        0xdc, 0xdb, 0xdb, 0xda, 0xd9, 0xd8, 0xd7, 0xd7,
        0xd6, 0xd5, 0xd4, 0xd4, 0xd3, 0xd2, 0xd1, 0xd1,
        0xd0, 0xcf, 0xce, 0xce, 0xcd, 0xcc, 0xcb, 0xcb,
        0xca, 0xc9, 0xc9, 0xc8, 0xc7, 0xc6, 0xc6, 0xc5,
        0xc4, 0xc4, 0xc3, 0xc2, 0xc2, 0xc1, 0xc0, 0xc0,
        0xbf, 0xbe, 0xbe, 0xbd, 0xbc, 0xbc, 0xbb, 0xba,
        0xba, 0xb9, 0xb8, 0xb8, 0xb7, 0xb7, 0xb6, 0xb5,
        0xb5, 0xb4, 0xb3, 0xb3, 0xb2, 0xb2, 0xb1, 0xb0,
        0xb0, 0xaf, 0xaf, 0xae, 0xad, 0xad, 0xac, 0xac,
        0xab, 0xaa, 0xaa, 0xa9, 0xa9, 0xa8, 0xa8, 0xa7,
        0xa6, 0xa6, 0xa5, 0xa5, 0xa4, 0xa4, 0xa3, 0xa3,
        0xa2, 0xa2, 0xa1, 0xa0, 0xa0, 0x9f, 0x9f, 0x9e,
        0x9e, 0x9d, 0x9d, 0x9c, 0x9c, 0x9b, 0x9b, 0x9a,
        0x9a, 0x99, 0x99, 0x98, 0x98, 0x97, 0x97, 0x96,
        0x96, 0x95, 0x95, 0x94, 0x94, 0x93, 0x93, 0x92,
        0x92, 0x91, 0x91, 0x90, 0x90, 0x8f, 0x8f, 0x8e,
        0x8e, 0x8d, 0x8d, 0x8c, 0x8c, 0x8c, 0x8b, 0x8b,
        0x8a, 0x8a, 0x89, 0x89, 0x88, 0x88, 0x87, 0x87,
        0x87, 0x86, 0x86, 0x85, 0x85, 0x84, 0x84, 0x83,
        0x83, 0x83, 0x82, 0x82, 0x81, 0x81, 0x80, 0x80,
        0x80, 0x7f, 0x7f, 0x7e, 0x7e, 0x7e, 0x7d, 0x7d,
        0x7c, 0x7c, 0x7b, 0x7b, 0x7b, 0x7a, 0x7a, 0x79,
        0x79, 0x79, 0x78, 0x78, 0x77, 0x77, 0x77, 0x76,
        0x76, 0x76, 0x75, 0x75, 0x74, 0x74, 0x74, 0x73,
        0x73, 0x72, 0x72, 0x72, 0x71, 0x71, 0x71, 0x70,
        0x70, 0x6f, 0x6f, 0x6f, 0x6e, 0x6e, 0x6e, 0x6d,
        0x6d, 0x6d, 0x6c, 0x6c, 0x6b, 0x6b, 0x6b, 0x6a
    };

    return (sqrtReciprocalTable[index]);
}

#endif
