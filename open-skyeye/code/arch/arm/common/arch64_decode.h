
/*************************************************************************
  > File Name    : armv8_opcode.h
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/8/7 18:20:14
 ************************************************************************/

#ifndef _ARMV8_DECODE_H
#define _ARMV8_DECODE_H

#define HINTS_OPC		0x1032
#define BARRIERS_OPC	0x1033

typedef struct
{
    ARMdword lo64;
    ARMdword hi64;
} bit128;

typedef struct
{
    char *SysRegName;
    int content[5];
} SysReg_Decode_Opc;

typedef void (*arch64_opc_function) (ARMul_State * state);

void arch64_execute_insn(ARMul_State * state);
void arch64_exec_opc(ARMul_State * state);

//main opcode function
static void arm_opc_reserved(ARMul_State * state);
static void arm_opc_unallocated(ARMul_State * state);
static void arm_opc_sve(ARMul_State * state);

static void arm_opc_load_and_store(ARMul_State * state);

static void ldst_multiple_struct(ARMul_State * state);
static void ldst_single_struct(ARMul_State * state);

static void load_store_memory_tags_instr_proc(ARMul_State * state);
static void stzgm(ARMul_State * state);
static void stg(ARMul_State * state);
static void ldg(ARMul_State * state);
static void stzg(ARMul_State * state);

static void stgm(ARMul_State * state);
static void st2g(ARMul_State * state);
static void ldgm(ARMul_State * state);
static void stz2g(ARMul_State * state);

static void load_store_exclusive_instr_proc(ARMul_State * state);
static void stxrb(ARMul_State * state);
static void stxrh(ARMul_State * state);
static void stxr(ARMul_State * state);
static void stlxrb(ARMul_State * state);
static void stlxrh(ARMul_State * state);
static void stlxr(ARMul_State * state);
static void casp(ARMul_State * state);
static void stxp(ARMul_State * state);
static void stlxp(ARMul_State * state);
static void ldxp(ARMul_State * state);
static void ldaxp(ARMul_State * state);
static void ldxrb(ARMul_State * state);
static void ldxrh(ARMul_State * state);
static void ldxr(ARMul_State * state);
static void ldaxrb(ARMul_State * state);
static void ldaxrh(ARMul_State * state);
static void ldaxr(ARMul_State * state);
static void stllrb(ARMul_State * state);
static void stllrh(ARMul_State * state);
static void stllr(ARMul_State * state);
static void stlrb(ARMul_State * state);
static void stlrh(ARMul_State * state);
static void stlr(ARMul_State * state);
static void casb(ARMul_State * state);
static void cash(ARMul_State * state);
static void cas(ARMul_State * state);
static void ldlarb(ARMul_State * state);
static void ldlarh(ARMul_State * state);
static void ldlar(ARMul_State * state);
static void ldarb(ARMul_State * state);
static void ldarh(ARMul_State * state);
static void ldar(ARMul_State * state);

static void ldapr_stlr(ARMul_State * state);
static void stlurb(ARMul_State * state);
static void ldapurb(ARMul_State * state);
static void ldapursb(ARMul_State * state);
static void stlurh(ARMul_State * state);
static void ldapurh(ARMul_State * state);
static void ldapursh(ARMul_State * state);
static void stlur(ARMul_State * state);
static void ldapur(ARMul_State * state);
static void ldapursw(ARMul_State * state);

static void load_register_literal(ARMul_State * state);
static void ldr_literal(ARMul_State * state);
static void ldr_literal_simd_fp(ARMul_State * state);
static void ldrsw_literal(ARMul_State * state);
static void prfm_literal(ARMul_State * state);

static void load_store_no_allocate_pair_offset(ARMul_State * state);
static void stnp(ARMul_State * state);
static void ldnp(ARMul_State * state);
static void stnp_simd_fp(ARMul_State * state);
static void ldnp_simd_fp(ARMul_State * state);

static void load_store_register_pair(ARMul_State * state);
static void stp(ARMul_State * state);
static void ldp(ARMul_State * state);
static void stp_simd_fp(ARMul_State * state);
static void ldp_simd_fp(ARMul_State * state);

static void load_store_register_unscaled_imm(ARMul_State * state);
static void stur(ARMul_State * state);
static void ldur(ARMul_State * state);
static void ldurs(ARMul_State * state);
static void stur_simd_fp(ARMul_State * state);
static void ldur_simd_fp(ARMul_State * state);
static void prfum(ARMul_State * state);

static void load_store_register_imm9(ARMul_State * state);
static void str_imm_simd_fp(ARMul_State * state);
static void ldr_imm_simd_fp(ARMul_State * state);
static void str_imm(ARMul_State * state);
static void ldr_imm(ARMul_State * state);

static void load_store_register_unpriv(ARMul_State * state);
static void sttrb(ARMul_State * state);
static void ldtrb(ARMul_State * state);
static void ldtrsb(ARMul_State * state);
static void sttrh(ARMul_State * state);
static void ldtrh(ARMul_State * state);
static void ldtrsh(ARMul_State * state);
static void sttr(ARMul_State * state);
static void ldtr(ARMul_State * state);
static void ldtrsw(ARMul_State * state);

static void atomic_mem(ARMul_State * state);
static void ld_opc(ARMul_State * state, int opc);

static void ldapr(ARMul_State * state);
static void swp(ARMul_State * state);

static void ldadd(ARMul_State * state);
static void ldclr(ARMul_State * state);
static void ldeor(ARMul_State * state);
static void ldset(ARMul_State * state);
static void ldsmax(ARMul_State * state);
static void ldsmin(ARMul_State * state);
static void ldumax(ARMul_State * state);
static void ldumin(ARMul_State * state);

static void load_store_register_offset(ARMul_State * state);
static void str_register_simd_fp(ARMul_State * state);
static void ldr_register_simd_fp(ARMul_State * state);
static void str_register(ARMul_State * state);
static void ldr_register(ARMul_State * state);
static void prfm_register(ARMul_State * state);

static void load_store_register_pac(ARMul_State * state);
static void ldra(ARMul_State * state);

static void load_store_register_uimm(ARMul_State * state);
static void prfm_imm(ARMul_State * state);

static void arm_opc_data_process_register(ARMul_State * state);
static void arm_opc_data_process_fp_and_simd(ARMul_State * state);
static void arm_opc_data_process_immediate(ARMul_State * state);
static void arm_opc_branch_exception_system(ARMul_State * state);

//branch,Exception Generating and System instructions opcode function
static void arm_opc_uncond_branch_imm(ARMul_State * state);

static void arm_opc_cmp_and_branch_imm(ARMul_State * state);
static void cbz(ARMul_State * state);
static void cbnz(ARMul_State * state);

static void arm_opc_test_and_branch_imm(ARMul_State * state);
static void tbz(ARMul_State * state);
static void tbnz(ARMul_State * state);

static void arm_opc_cond_branch_imm(ARMul_State * state);

static void arm_opc_exception_generation(ARMul_State * state);
static void svc(ARMul_State * state);
static void hvc(ARMul_State * state);
static void smc(ARMul_State * state);
static void ibrk(ARMul_State * state);
static void hlt(ARMul_State * state);
static void dcps1(ARMul_State * state);
static void dcps2(ARMul_State * state);
static void dcps3(ARMul_State * state);

static void arm_opc_hints(ARMul_State * state);
static void nop(ARMul_State * state);
static void yield(ARMul_State * state);
static void wfe(ARMul_State * state);
static void wfi(ARMul_State * state);
static void sev(ARMul_State * state);
static void sevl(ARMul_State * state);
static void xpac_instr_proc(ARMul_State * state);
static void pacia1716_instr_proc(ARMul_State * state);
static void pacib1716_instr_proc(ARMul_State * state);
static void autia1716_instr_proc(ARMul_State * state);
static void autib1716_instr_proc(ARMul_State * state);
static void esb(ARMul_State * state);
static void psb_csync(ARMul_State * state);
static void tsb_csync(ARMul_State * state);
static void csdb(ARMul_State * state);
static void paciaz_instr_proc(ARMul_State * state);
static void paciasp_instr_proc(ARMul_State * state);
static void pacibz_instr_proc(ARMul_State * state);
static void pacibsp_instr_proc(ARMul_State * state);
static void autiaz_instr_proc(ARMul_State * state);
static void autiasp_instr_proc(ARMul_State * state);
static void autibz_instr_proc(ARMul_State * state);
static void autibsp_instr_proc(ARMul_State * state);
static void bti(ARMul_State * state);

static void arm_opc_barriers(ARMul_State * state);
static void clrex(ARMul_State * state);
static void dmb(ARMul_State * state);
static void isb(ARMul_State * state);
static void sb(ARMul_State * state);
static void dsb(ARMul_State * state);
static void ssbb(ARMul_State * state);
static void pssbb(ARMul_State * state);

static void arm_opc_pstate(ARMul_State * state);
static void cfinv(ARMul_State * state);
static void xaflag(ARMul_State * state);
static void axflag(ARMul_State * state);
static void msr_imm(ARMul_State * state);

static void arm_opc_system_instr(ARMul_State * state);
static void arm_opc_system_register_move(ARMul_State * state);

static void arm_opc_uncond_branch_register(ARMul_State * state);
static void br(ARMul_State * state);
static void blr(ARMul_State * state);
static void ret(ARMul_State * state);
static void eret(ARMul_State * state);
static void drps(ARMul_State * state);
static void bra_keyA(ARMul_State * state);
static void blra_keyA(ARMul_State * state);
static void retaa(ARMul_State * state);
static void eretaa(ARMul_State * state);
static void bra_keyB(ARMul_State * state);
static void blra_keyB(ARMul_State * state);
static void retab(ARMul_State * state);
static void eretab(ARMul_State * state);
static void braa_keyA(ARMul_State * state);
static void braa_keyB(ARMul_State * state);
static void blraa_keyA(ARMul_State * state);
static void blraa_keyB(ARMul_State * state);

//Data Process immediate
static void pc_rel_adr_instr_proc(ARMul_State * state);

static void add_sub_imm_instr_proc(ARMul_State * state);
static void add32_imm(ARMul_State * state);
static void adds32_imm(ARMul_State * state);
static void sub32_imm(ARMul_State * state);
static void subs32_imm(ARMul_State * state);
static inline ARMword A32_AddWithCarry(ARMul_State * state, ARMword x, ARMword y, ARMword carry_in, ARMword setFlags);

static void add64_imm(ARMul_State * state);
static void adds64_imm(ARMul_State * state);
static void sub64_imm(ARMul_State * state);
static void subs64_imm(ARMul_State * state);
static inline ARMdword A64_AddWithCarry(ARMul_State * state, ARMdword x, ARMdword y, ARMword carry_in,
                                        ARMword setFlags);

static void add_sub_imm_tag_instr_proc(ARMul_State * state);
static void addg(ARMul_State * state);
static void subg(ARMul_State * state);

static void logical_imm_instr_proc(ARMul_State * state);
static void and32_imm(ARMul_State * state);
static void orr32_imm(ARMul_State * state);
static void eor32_imm(ARMul_State * state);
static void ands32_imm(ARMul_State * state);
static void and64_imm(ARMul_State * state);
static void orr64_imm(ARMul_State * state);
static void eor64_imm(ARMul_State * state);
static void ands64_imm(ARMul_State * state);

static void mov_imm_instr_proc(ARMul_State * state);
static void movn_imm(ARMul_State * state);
static void movz_imm(ARMul_State * state);
static void movk_imm(ARMul_State * state);

static void bitfield_instr_proc(ARMul_State * state);
static void bfm(ARMul_State * state);

static void extract_instr_proc(ARMul_State * state);
static void extr32(ARMul_State * state);
static void extr64(ARMul_State * state);

//Data processing Register
static void data_process_register_two_src(ARMul_State * state);
static void udiv32(ARMul_State * state);
static void sdiv32(ARMul_State * state);
static void lslv32(ARMul_State * state);
static void lsrv32(ARMul_State * state);
static void asrv32(ARMul_State * state);
static void rorv32(ARMul_State * state);
static void crc32_32(ARMul_State * state);

static void udiv64(ARMul_State * state);
static void sdiv64(ARMul_State * state);
static void lslv64(ARMul_State * state);
static void lsrv64(ARMul_State * state);
static void asrv64(ARMul_State * state);
static void rorv64(ARMul_State * state);
static void crc32_64(ARMul_State * state);

static void subp(ARMul_State * state);
static void irg(ARMul_State * state);
static void gmi(ARMul_State * state);
static void pacga(ARMul_State * state);

static void data_process_register_one_src(ARMul_State * state);
static void rbit(ARMul_State * state);
static void rev16(ARMul_State * state);
static void rev(ARMul_State * state);
static void clz(ARMul_State * state);
static void cls(ARMul_State * state);
static void pacia(ARMul_State * state);
static void pacib(ARMul_State * state);
static void pacda(ARMul_State * state);
static void pacdb(ARMul_State * state);
static void autia(ARMul_State * state);
static void autib(ARMul_State * state);
static void autda(ARMul_State * state);
static void autdb(ARMul_State * state);
static void xpaci(ARMul_State * state);
static void xpacd(ARMul_State * state);

static void logical_shift_register(ARMul_State * state);
static void and_shift_register(ARMul_State * state);
static void bic_shift_register(ARMul_State * state);
static void orr_shift_register(ARMul_State * state);
static void orn_shift_register(ARMul_State * state);
static void eor_shift_register(ARMul_State * state);
static void eon_shift_register(ARMul_State * state);
static void ands_shift_register(ARMul_State * state);
static void bics_shift_register(ARMul_State * state);

static void add_sub_shift_reg(ARMul_State * state);
static void add_shift_register(ARMul_State * state);
static void adds_shift_register(ARMul_State * state);
static void sub_shift_register(ARMul_State * state);
static void subs_shift_register(ARMul_State * state);

static void add_sub_extend_reg(ARMul_State * state);
static void add_extend_register(ARMul_State * state);
static void adds_extend_register(ARMul_State * state);
static void sub_extend_register(ARMul_State * state);
static void subs_extend_register(ARMul_State * state);

static void add_sub_with_carry(ARMul_State * state);
static void adc(ARMul_State * state);
static void adcs(ARMul_State * state);
static void sbc(ARMul_State * state);
static void sbcs(ARMul_State * state);

static void rotate_right(ARMul_State * state);
static void rmif(ARMul_State * state);

static void evaluate(ARMul_State * state);
static void setf8(ARMul_State * state);
static void setf16(ARMul_State * state);

static void cond_select(ARMul_State * state);
static void csel(ARMul_State * state);
static void csinc(ARMul_State * state);
static void csinv(ARMul_State * state);
static void csneg(ARMul_State * state);

static void data_process_register_three_src(ARMul_State * state);
static void madd(ARMul_State * state);
static void msub(ARMul_State * state);
static void smaddl(ARMul_State * state);
static void smsubl(ARMul_State * state);
static void smulh(ARMul_State * state);
static void umaddl(ARMul_State * state);
static void umsubl(ARMul_State * state);
static void umulh(ARMul_State * state);

static void cond_cmp_register(ARMul_State * state);
static void ccmn_register(ARMul_State * state);
static void ccmp_register(ARMul_State * state);

static void cond_cmp_imm(ARMul_State * state);
static void ccmn_imm(ARMul_State * state);
static void ccmp_imm(ARMul_State * state);

static void ic(ARMul_State * state);

ARMword ConditionHolds(ARMul_State * state, int cond);
ARMword decode_SysRegField(SysReg_Decode_Opc * SysReg, int *index);
ARMword GetSystemRegIndex(int op0, int op1, int CRn, int CRm, int op2);

#define DECODE_OK	1
#define DECODE_ERR	0

#define ARM_OPC_GET_SYSTEM_ACCESS(L, op0, op1, CRn, CRm, op2, Rt) \
{	L = BITS(21, 21); \
	op0 = BITS(19, 19) + 2;\
	op1 = BITS(16, 18); \
	CRn = BITS(12, 15); \
	CRm = BITS(8, 11); \
	op2 = BITS(5, 7); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_IMM26(op, imm26) \
{	op = BITS(31, 31); \
	imm26 = BITS(0, 25);}

#define ARM_OPC_GET_DATA_ADR(op, immlo, immhi, Rd) \
{	op = BITS(31, 31); \
	immlo = BITS(29, 30); \
	immhi = BITS(5, 23); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_SF_OPS(sf_opS) \
{	sf_opS = BITS(29, 31);}

#define ARM_OPC_GET_DATA_IMM12(sh, imm12, Rn, Rd) \
{	sh = BITS(22, 22); \
	imm12 = BITS(10, 21); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_COND_BRANCH(o1, imm19, o0, cond) \
{	o1 = BITS(24, 24); \
	imm19 = BITS(5, 23); \
	o0 = BITS(4, 4); \
	cond = BITS(0, 3);}

#define ARM_OPC_GET_MOV_IMM16(hw, imm16, Rd) \
{	hw = BITS(21, 22); \
	imm16 = BITS(5, 20); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_DATA_PROC_REG_OPC(op0, op1, op2, op3) \
{	op0 = BITS(30, 30); \
	op1 = BITS(28, 28); \
	op2 = BITS(21, 24); \
	op3 = BITS(10, 15);}

#define ARM_OPC_GET_LD_ST_OPC(op0, op1, op2, op3, op4) \
{	op0 = BITS(28, 31); \
	op1 = BITS(26, 26); \
	op2 = BITS(23, 24); \
	op3 = BITS(16, 21); \
	op4 = BITS(10, 11);}

#define ARM_OPC_GET_DATA_PROC_SRC_OPC(sf, S, opcode) \
{	sf = BITS(31, 31); \
	S = BITS(29, 29); \
	opcode = BITS(10, 15);}

#define ARM_OPC_GET_SRC_OPC(Rm, Rn, Rd) \
{	Rm = BITS(16, 20); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_CRC_OPC(Rm, C, sz, Rn, Rd) \
{	Rm = BITS(16, 20); \
	C = BITS(12, 12); \
	sz = BITS(10, 11); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_EXTRACT_OPC(sf_opc, N, o0, imms) \
{	sf_opc = BITS(29, 31); \
	N = BITS(22, 22); \
	o0 = BITS(21, 21); \
	imms = BITS(10, 15);}

#define ARM_OPC_GET_EXC_OPC(opc, op2, LL) \
{	opc = BITS(21, 23); \
	op2 = BITS(2, 4); \
	LL = BITS(0, 1);}

#define ARM_OPC_GET_HINT_OPC(CRm, op2) \
{	CRm = BITS(8, 11); \
	op2 = BITS(5, 7);}

#define ARM_OPC_GET_BARRIERS_OPC(CRm, op2, Rt) \
{	CRm = BITS(8, 11); \
	op2 = BITS(5, 7); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_PSTATE_OPC(op1, op2, Rt) \
{	op1 = BITS(16, 18); \
	op2 = BITS(5, 7); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_PSTATE_OPC(op1, op2, Rt) \
{	op1 = BITS(16, 18); \
	op2 = BITS(5, 7); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_SYSTEM_OPC(L, op1, CRn, CRm, op2, Rt) \
{	L = BITS(21, 21); \
	op1 = BITS(16, 18); \
	CRn = BITS(12, 15); \
	CRm = BITS(8, 11); \
	op2 = BITS(5, 7); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_UNCODE_BRANCH_REG_OPC(opc, op2, op3, Rn, op4) \
{	opc = BITS(21, 24); \
	op2 = BITS(16, 20); \
	op3 = BITS(10, 15); \
	Rn = BITS(5, 9); \
	op4 = BITS(0, 4);}

#define ARM_OPC_GET_CMP_BRANCH_OPC(sf, op) \
{	sf = BITS(31, 31); \
	op = BITS(24, 24);}

#define ARM_OPC_GET_SIMD_MULT_OPC(L, opcode) \
{	L = BITS(22, 22); \
	opcode = BITS(12, 15);}

#define ARM_OPC_GET_SIMD_MULT_INDEX_OPC(L, Rm, opcode) \
{	L = BITS(22, 22); \
	Rm = BITS(16, 20); \
	opcode = BITS(12, 15);}
#define ARM_OPC_GET_SIMD_SINGLE_OPC(LR, opcode, S, size) \
{	LR = BITS(21, 22); \
	opcode = BITS(13, 15);\
	S = BITS(12, 12);\
	size = BITS(10, 11);\
}

#define ARM_OPC_GET_SIMD_SINGLE_INDEX_OPC(LR, Rm, opcode, S, size) \
{	LR = BITS(21, 22); \
	Rm = BITS(16, 20);\
	opcode = BITS(13, 15);\
	S = BITS(12, 12);\
	size = BITS(10, 11);}

#define ARM_OPC_GET_MEM_TAG_OPC(opc, imm9, op2) \
{	opc = BITS(22, 23); \
	imm9 = BITS(12, 20);\
	op2 = BITS(10, 11);}

#define ARM_OPC_GET_EXCLUSIVE_OPC(size, o2_L_o1, o0, Rt2) \
{	size = BITS(30, 31); \
	o2_L_o1 = BITS(21, 23);\
	o0 = BITS(15, 15);\
	Rt2 = BITS(10, 14);}

#define ARM_OPC_GET_LDAPR_STLR_OPC(size, opc, opcode) \
{	size = BITS(30, 31); \
	opc = BITS(22, 23); \
	opcode = size << 2 | opc;}

#define ARM_OPC_GET_LDAD_LITTERAL_OPC(opc, V, opcode) \
{	opc = BITS(30, 31); \
	V = BITS(26, 26); \
	opcode = opc << 1 | V;}

#define ARM_OPC_GET_PAIR_POST_INDEX_OPC(opc, V, L, opcode) \
{	opc = BITS(30, 31); \
	V = BITS(26, 26); \
	L = BITS(22, 22); \
	opcode = opc << 2 | V << 1 | L;}

#define ARM_OPC_GET_UNSCALED_IMM_OPC(size, V, opc, opcode) \
{	size = BITS(30, 31); \
	V = BITS(26, 26); \
	opc = BITS(22, 23); \
	opcode = size << 3 | V << 2 | opc;}

#define ARM_OPC_GET_ATOMIC_MEM_OPC(size, V, A, R, o3, opc, opcode) \
{	size = BITS(30, 31); \
	V = BITS(26, 26); \
	A = BITS(23, 23); \
	R = BITS(22, 22); \
	o3 = BITS(15, 15); \
	opc = BITS(12, 14); \
	opcode = V << 3 | A << 2 | R << 1 | o3;}

#define ARM_OPC_GET_PAC_OPC(size, V, M, W, opcode) \
{	size = BITS(30, 31); \
	V = BITS(26, 26); \
	M = BITS(23, 23); \
	W = BITS(11, 11); \
	opcode = V << 2 | M << 1 | W;}

#define ARM_OPC_GET_ONE_SRC(sf_S, opcode2, opcode, Rn) \
{	sf_S = BITS(31, 31) << 1 | BITS(29, 29); \
	opcode2 = BITS(16, 20); \
	opcode = BITS(10, 15); \
	Rn = BITS(5, 9);}

#define ARM_OPC_GET_LOGICAL_SHIFT_REG_OPC(sf, opc, N, opcode) \
{	sf = BITS(31, 31); \
	opc = BITS(29, 30); \
	N = BITS(21, 21); \
	opcode = opc << 1 | N;}

#define ARM_OPC_GET_ARITH_SHIFT_REG_OPC(sf, op_S) \
{	sf = BITS(31, 31); \
	op_S = BITS(29, 30);}

#define ARM_OPC_GET_ARITH_EXTEND_REG_OPC(sf, op_S, opt) \
{	sf = BITS(31, 31); \
	op_S = BITS(29, 30); \
	opt = BITS(22, 23);}

#define ARM_OPC_GET_ROTATE_OPC(sf, op_S, o2) \
{	sf = BITS(31, 31); \
	op_S = BITS(29, 30); \
	o2 = BITS(4, 4);}

#define ARM_OPC_GET_EVALUATE_OPC(sf, op_S, opcode2, sz, o3, mask) \
{	sf = BITS(31, 31); \
	op_S = BITS(29, 30); \
	opcode2 = BITS(15, 20); \
	sz = BITS(14, 14); \
	o3 = BITS(4, 4); \
	mask = BITS(0, 3);}

#define ARM_OPC_GET_COND_CMP_OPC(sf, op, S, o2, o3) \
{	sf = BITS(31, 31); \
	op = BITS(30, 30); \
	S = BITS(29, 29); \
	o2 = BITS(10, 10); \
	o3 = BITS(4, 4);}

#define ARM_OPC_GET_COND_SELECT_OPC(sf, op, S, op2, opcode) \
{	sf = BITS(31, 31); \
	op = BITS(30, 30); \
	S = BITS(29, 29); \
	op2 = BITS(10, 11); \
	opcode = op << 3 | S << 2 | op2;}

#define ARM_OPC_GET_THREE_SRC_OPC(sf, op54, op31, o0) \
{	sf = BITS(31, 31); \
	op54 = BITS(29, 30); \
	op31 = BITS(21, 23); \
	o0 = BITS(15, 15);}

#define ARM_OPC_GET_LDR_LITERAL_OPC(opc, imm19, Rt) \
{	opc = BITS(30, 31); \
	imm19 = BITS(5, 23); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_AND_IMM_OPC(N, immr, imms, Rn, Rd) \
{	N = BITS(22, 22); \
	immr = BITS(16, 21); \
	imms = BITS(10, 15); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_SHIFT_REG_OPC(sf, shift_type, Rm, imm6, Rn, Rd) \
{	sf = BITS(31, 31);\
	shift_type = BITS(22, 23);\
	Rm = BITS(16, 20);\
	imm6 = BITS(10, 15);\
	Rn = BITS(5, 9);\
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_EXTEND_REG_OPC(sf, Rm, option, imm3, Rn, Rd) \
{	sf = BITS(31, 31);\
	Rm = BITS(16, 20);\
	option = BITS(13, 15);\
	imm3 = BITS(10, 12);\
	Rn = BITS(5, 9);\
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_SRC_DST_OPC(sf, Rm, Rn, Rd) \
{	sf = BITS(31, 31); \
	Rm = BITS(16, 20); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_STR_OPC(size, Rm, option, S, Rn, Rt) \
{	size = BITS(30, 31); \
	Rm = BITS(16, 20); \
	option = BITS(13, 15); \
	S = BITS(12, 12); \
	Rn = BITS(5, 9); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_LD_ST_IMM_OPC(size, opc, imm9, Rn, Rt) \
{	size = BITS(30, 31); \
	opc = BITS(22, 23); \
	imm9 = BITS(12, 20); \
	Rn = BITS(5, 9); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_STP_OPC(opc, imm7, Rt2, Rn, Rt) \
{	opc = BITS(30, 31); \
	imm7 = BITS(15, 21); \
	Rt2 = BITS(10, 14); \
	Rn = BITS(5, 9); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_TBZ_OPC(b5, b40, imm14, Rt) \
{	b5 = BITS(31, 31); \
	b40 = BITS(19, 23); \
	imm14 = BITS(5, 18); \
	Rt = BITS(0, 4);}

#define ARM_OPC_GET_BFM_OPC(sf, opc, N, immr, imms, Rn, Rd) \
{	sf = BITS(31, 31); \
	opc = BITS(29, 30); \
	N = BITS(22, 22); \
	immr = BITS(16, 21); \
	imms = BITS(10, 15); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_MUL_OPC(sf, Rm, Ra, Rn, Rd) \
{	sf = BITS(31, 31); \
	Rm = BITS(16, 20); \
	Ra = BITS(10, 14); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_CSEL_OPC(sf, Rm, cond, Rn, Rd) \
{	sf = BITS(31, 31); \
	Rm = BITS(16, 20); \
	cond = BITS(12, 15); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_ONE_SRC_OPC(sf, Rn, Rd) \
{	sf = BITS(31, 31); \
	Rn = BITS(5, 9); \
	Rd = BITS(0, 4);}

#define ARM_OPC_GET_CCMP_OPC(sf, imm5, cond, Rn, nzcv) \
{	sf = BITS(31, 31); \
	imm5 = BITS(16, 20); \
	cond = BITS(12, 15); \
	Rn = BITS(5, 9); \
	nzcv = BITS(0, 3);}

#define WRITE_X_REG(Rd, Data) {assert(Rd <= 31); Rd == 31 ? (state->A64_SP = Data) : (state->R[Rd].X = Data);}
#define READ_X_REG(Rd) Rd == 31 ? state->A64_SP : state->R[Rd].X;

#define WRITE_W_REG(Rd, Data) {assert(Rd <= 31); Rd == 31 ? (state->ZR = 0) : (state->R[Rd].W = Data);}
#define READ_W_REG(Rd, Data) {assert(Rd <= 31); Rd == 31 ? (Data = 0) : (Data = state->R[Rd].W);}

#define WRITE_X_HIGH_REG(Rd, Data) {assert(Rd <= 31); Rd == 31 ? (state->ZR = 0) : (state->R[Rd].High = Data);}
#define READ_X_HIGH_REG(Rd, Data) {assert(Rd <= 31); Rd == 31 ? (Data = 0) : (Data = state->R[Rd].High);}

#define SysRegWriteValue(Rt, regValue) {assert(Rt <= 31); Rt == 31 ? (*regValue = state->A64_SP) : (*regValue = state->R[Rt].X);}
#define SysRegReadValue(Rt, regValue) {assert(Rt <= 31); Rt == 31 ? (state->A64_SP = *regValue) : (state->R[Rt].X = *regValue);}

#define WRITE_Vq_REG(Rd, Data1, Data2) {state->V[Rd].dword.d[0] = Data1;state->V[Rd].dword.d[1] = Data2;}
#define WRITE_Vdh_REG(Rd, Data) {state->V[Rd].dword.d[1] = Data;}
#define WRITE_Vd_REG(Rd, Data) {state->V[Rd].dword.d[0] = Data;}
#define WRITE_Vs_REG(Rd, Data) {state->V[Rd].word.s[0] = Data;}
#define WRITE_Vh_REG(Rd, Data) {state->V[Rd].hword.h[0] = Data;}
#define WRITE_Vb_REG(Rd, Data) {state->V[Rd].byte.b[0] = Data;}

#define READ_Vq_REG(Rd, Data1, Data2) {Data1 = state->V[Rd].dword.d[0]; Data2 = state->V[Rd].dword.d[1];}
#define READ_Vd_REG(Rd) state->V[Rd].dword.d[0]
#define READ_Vs_REG(Rd) state->V[Rd].word.s[0]
#define READ_Vh_REG(Rd) state->V[Rd].hword.h[0]
#define READ_Vb_REG(Rd) state->V[Rd].byte.b[0]

#define GET_VFP_DATA(size, Rd, Data1, Data2)	\
{	\
	assert(Rd < 32);	\
	switch(size)	\
	{	\
		case 2:	\
			Data1 = READ_Vs_REG(Rd);	\
			Data2 = 0;	\
			break;	\
		case 3:	\
			Data1 = READ_Vd_REG(Rd);	\
			Data2 = 0;	\
			break;	\
		case 4:	\
			READ_Vq_REG(Rd, Data1, Data2);	\
			break;	\
		case 5:	\
			Data1 = READ_Vh_REG(Rd);	\
			Data2 = 0;	\
			break;	\
		default:	\
			printf("Unallocated size = %d\n", size);	\
			break;	\
	}	\
}

#define SET_VFP_DATA(size, Rd, Data)	\
{	\
	assert(Rd < 32);	\
	switch(size)	\
	{	\
		case 2:	\
			WRITE_Vs_REG(Rd, (ARMword)Data.lo64);	\
			break;	\
		case 3:	\
			WRITE_Vd_REG(Rd, Data.lo64);	\
			break;	\
		case 4:	\
			WRITE_Vq_REG(Rd, Data.lo64, Data.hi64);	\
			break;	\
		case 5:	\
			WRITE_Vh_REG(Rd, (uint16_t)Data.lo64);	\
			break;	\
		default:	\
			printf("Unallocated size = %d\n", size);	\
			break;	\
	}	\
}

#define M32_User	0x10
#define M32_FIQ		0x11
#define M32_IRQ		0x12
#define M32_Svc		0x13
#define M32_Monitor 0x16
#define M32_Abort	0x17
#define M32_Hyp		0x1A
#define M32_Undef	0x1B
#define M32_System	0x1F

#define POLY_VAL	0x4C11DB7
#define POLYC_VAL	0x1EDC6F41

//#define SignExtend64(Data, n) ((Data & (1 << (n - 1))) ? (Data | (BIT64_MAX_VALUE << n)) : Data)

enum PState_EL_t
{
    EL0 = 0,
    EL1,
    EL2,
    EL3
};

enum BranchType
{
    BranchType_DIRCALL = 0,             // Direct Branch with link
    BranchType_INDCALL,                 // Indirect Branch with link
    BranchType_ERET,                    // Exception return (indirect)
    BranchType_DBGEXIT,                 // Exit from Debug state
    BranchType_RET,                     // Indirect branch with function return hint
    BranchType_DIR,                     // Direct branch
    BranchType_INDIR,                   // Indirect branch
    BranchType_EXCEPTION,               // Exception entry
    BranchType_RESET,                   // Reset
    BranchType_UNKNOWN                  // Other
};

enum a64_shift_type
{
    A64_SHIFT_TYPE_LSL = 0,
    A64_SHIFT_TYPE_LSR = 1,
    A64_SHIFT_TYPE_ASR = 2,
    A64_SHIFT_TYPE_ROR = 3
};

enum PSTATEField
{
    PSTATEField_DAIFSet = 0,
    PSTATEField_DAIFClr,
    PSTATEField_PAN,                    // Armv8.1
    PSTATEField_UAO,                    // Armv8.2
    PSTATEField_DIT,                    // Armv8.4
    PSTATEField_SSBS,
    PSTATEField_TCO,                    // Armv8.5
    PSTATEField_SP
};

struct pstate_nzcv_t
{
    bool_t nf;
    bool_t zf;
    bool_t cf;
    bool_t vf;
};

#define setPStateNZCV(pstate_nvzv)	\
{	\
	state->PSTATE.N = pstate_nvzv.nf;	\
	state->PSTATE.Z = pstate_nvzv.zf;	\
	state->PSTATE.C = pstate_nvzv.cf;	\
	state->PSTATE.V = pstate_nvzv.vf;	\
}

#define setPStateZF  (state->PSTATE.Z = 1)
#define setPStateNF  (state->PSTATE.N = 1)
#define setPStateCF  (state->PSTATE.C = 1)
#define setPStateVF  (state->PSTATE.V = 1)

#define clearPStateZF  (state->PSTATE.Z = 0)
#define clearPStateNF  (state->PSTATE.N = 0)
#define clearPStateCF  (state->PSTATE.C = 0)
#define clearPStateVF  (state->PSTATE.V = 0)

#define EL1_UMA		0x200

#define LOG2_TAG_GRANULE	0
/*
 * SCTLR_EL1/SCTLR_EL2/SCTLR_EL3 bits definitions
 */
#define CR_M		(1 << 0)        /* MMU enable                   */
#define CR_A		(1 << 1)        /* Alignment abort enable       */
#define CR_C		(1 << 2)        /* Dcache enable                */
#define CR_SA		(1 << 3)        /* Stack Alignment Check Enable */
#define CR_I		(1 << 12)       /* Icache enable                */
#define CR_WXN		(1 << 19)       /* Write Permision Imply XN     */
#define CR_EE		(1 << 25)       /* Exception (Big) Endian       */

#define GetTagFromAddress(Data) ((Data >> 56) & 0xf)

#define NOT_IMPLEMENT		skyeye_log(Error_log, __func__, "In Line:%d.Not implement this instruction!PC = 0x%llx Instr = 0x%x\n", __LINE__, state->A64_PC, state->instr);
#define UNALLOCATED_INSTR	skyeye_log(Error_log, __func__, "In Line:%d.Unallocated Instruction:0x%08x!PC = 0x%llx\n", __LINE__, instr, state->A64_PC);
#define UNDEFINED_INSTR		{ skyeye_log(Error_log, __func__, "In Line:%d.Undefined Instruction:0x%x!PC = %llx\n", __LINE__, state->instr, state->A64_PC); return ; }
#define INSTR_DECODE_ERR	skyeye_log(Error_log, __func__, "In Line:%d.Instruction:0x%08x Decode Error!\n", __LINE__, instr);

#endif
