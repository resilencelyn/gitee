/*************************************************************************
  > TriCore simulation for SkyEye: implement instructions execute
  > File Name    : exec_insn.c
  > Author       : jiachao
  > Mail         : jiachao@digiproto.com
  > Created Time : 2021/10/26 09:42:09
 ************************************************************************/
#include "cpu.h"
#include "helper.h"
#include "tricore_opcode.h"

static inline void gen_addi_d(conf_object_t* opaque, int rd, int rs1, int32_t imm)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    core->gpr_d[rd] = core->gpr_d[rs1] + imm;
}

static inline void gen_addi_a(conf_object_t* opaque, int rd, int rs1, int32_t imm)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;

    core->gpr_a[rd] = core->gpr_a[rs1] + imm;
}

static inline void gen_condi_add_d(conf_object_t* opaque, TCGCond cond, int r1, int32_t imm, int r3, int r4)
{
}

/*
 * Functions for decoding instructions
 */

static void decode_src_opc(conf_object_t* opaque, uint32_t opcode, int op1)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    int r1;
    int32_t const4;
    uint32_t temp, temp2;

    r1 = MASK_OP_SRC_S1D(opcode);
    const4 = MASK_OP_SRC_CONST4_SEXT(opcode);

    switch (op1)
    {
        case OPC1_16_SRC_ADD:
            gen_addi_d(opaque, r1, r1, const4);
            break;
        case OPC1_16_SRC_ADD_A15:
            gen_addi_d(opaque, r1, 15, const4);
            break;
        case OPC1_16_SRC_ADD_15A:
            gen_addi_d(opaque, 15, r1, const4);
            break;
        case OPC1_16_SRC_ADD_A:
            gen_addi_a(opaque, r1, r1, const4);
            break;
        case OPC1_16_SRC_CADD:
            gen_condi_add_d(opaque, TCG_COND_NE, r1, const4, r1, 15);
            break;
        case OPC1_16_SRC_CADDN:
            gen_condi_add_d(opaque, TCG_COND_EQ, r1, const4, r1, 15);
            break;
        case OPC1_16_SRC_CMOV:
            temp = 0;
            temp2 = const4;
            //tcg_gen_movcond_tl(TCG_COND_NE, cpu_gpr_d[r1], cpu_gpr_d[15], temp,
                           //temp2, cpu_gpr_d[r1]);
            break;
        case OPC1_16_SRC_CMOVN:
            temp = 0;
            temp2 = const4;
            //tcg_gen_movcond_tl(TCG_COND_EQ, cpu_gpr_d[r1], cpu_gpr_d[15], temp,
                           //temp2, cpu_gpr_d[r1]);
            break;
        case OPC1_16_SRC_EQ:
            //tcg_gen_setcondi_tl(TCG_COND_EQ, &core->gpr_d[15], &core->gpr_d[r1],
            //                    const4);
            break;
        case OPC1_16_SRC_LT:
            //tcg_gen_setcondi_tl(TCG_COND_LT, &core->gpr_d[15], &core->gpr_d[r1],
            //                    const4);
            break;
        case OPC1_16_SRC_MOV:
            //tcg_gen_movi_tl(&core->gpr_d[r1], const4);
            break;
        case OPC1_16_SRC_MOV_A:
            const4 = MASK_OP_SRC_CONST4(opcode);
            //tcg_gen_movi_tl(&core->gpr_a[r1], const4);
            break;
        case OPC1_16_SRC_MOV_E:
            /*
               if (tricore_feature(env, TRICORE_FEATURE_16)) {
               tcg_gen_movi_tl(&core->gpr_d[r1], const4);
               tcg_gen_sari_tl(&core->gpr_d[r1+1], &core->gpr_d[r1], 31);
               } else {
               generate_trap(ctx, TRAPC_INSN_ERR, TIN2_IOPC);
               }
               */
            break;
        case OPC1_16_SRC_SH:
            //gen_shi(&core->gpr_d[r1], &core->gpr_d[r1], const4);
            break;
        case OPC1_16_SRC_SHA:
            //gen_shaci(&core->gpr_d[r1], &core->gpr_d[r1], const4);
            break;
        default:
            //generate_trap(ctx, TRAPC_INSN_ERR, TIN2_IOPC);
            break;
    }
}

static void decode_16Bit_opc(conf_object_t* opaque, uint32_t opcode)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    int op1, r1, r2;
    int32_t const16;
    int32_t address;
    uint32_t temp;

    op1 = MASK_OP_MAJOR(opcode);

    /* handle ADDSC.A opcode only being 6 bit long */
    if ((op1 & 0x3f) == OPC1_16_SRRS_ADDSC_A)
    {
        op1 = OPC1_16_SRRS_ADDSC_A;
    }

    switch (op1)
    {
        case OPC1_16_SRC_ADD:
        case OPC1_16_SRC_ADD_A15:
        case OPC1_16_SRC_ADD_15A:
        case OPC1_16_SRC_ADD_A:
        case OPC1_16_SRC_CADD:
        case OPC1_16_SRC_CADDN:
        case OPC1_16_SRC_CMOV:
        case OPC1_16_SRC_CMOVN:
        case OPC1_16_SRC_EQ:
        case OPC1_16_SRC_LT:
        case OPC1_16_SRC_MOV:
        case OPC1_16_SRC_MOV_A:
        case OPC1_16_SRC_MOV_E:
        case OPC1_16_SRC_SH:
        case OPC1_16_SRC_SHA:
            decode_src_opc(opaque, opcode, op1);
            break;
            /* SRR-format */
        case OPC1_16_SRR_ADD:
        case OPC1_16_SRR_ADD_A15:
        case OPC1_16_SRR_ADD_15A:
        case OPC1_16_SRR_ADD_A:
        case OPC1_16_SRR_ADDS:
        case OPC1_16_SRR_AND:
        case OPC1_16_SRR_CMOV:
        case OPC1_16_SRR_CMOVN:
        case OPC1_16_SRR_EQ:
        case OPC1_16_SRR_LT:
        case OPC1_16_SRR_MOV:
        case OPC1_16_SRR_MOV_A:
        case OPC1_16_SRR_MOV_AA:
        case OPC1_16_SRR_MOV_D:
        case OPC1_16_SRR_MUL:
        case OPC1_16_SRR_OR:
        case OPC1_16_SRR_SUB:
        case OPC1_16_SRR_SUB_A15B:
        case OPC1_16_SRR_SUB_15AB:
        case OPC1_16_SRR_SUBS:
        case OPC1_16_SRR_XOR:
            //decode_srr_opc(opaque, opcode, op1);
            break;
            /* SSR-format */
        case OPC1_16_SSR_ST_A:
        case OPC1_16_SSR_ST_A_POSTINC:
        case OPC1_16_SSR_ST_B:
        case OPC1_16_SSR_ST_B_POSTINC:
        case OPC1_16_SSR_ST_H:
        case OPC1_16_SSR_ST_H_POSTINC:
        case OPC1_16_SSR_ST_W:
        case OPC1_16_SSR_ST_W_POSTINC:
            //decode_ssr_opc(opaque, opcode, op1);
            break;
            /* SRRS-format */
        case OPC1_16_SRRS_ADDSC_A:
            r2 = MASK_OP_SRRS_S2(opcode);
            r1 = MASK_OP_SRRS_S1D(opcode);
            const16 = MASK_OP_SRRS_N(opcode);
            //temp = tcg_temp_new();
            //tcg_gen_shli_tl(temp, &core->gpr_d[15], const16);
            //tcg_gen_add_tl(&core->gpr_a[r1], &core->gpr_a[r2], temp);
            break;
            /* SLRO-format */
        case OPC1_16_SLRO_LD_A:
            r1 = MASK_OP_SLRO_D(opcode);
            const16 = MASK_OP_SLRO_OFF4(opcode);
            //gen_offset_ld(ctx, &core->gpr_a[r1], &core->gpr_a[15], const16 * 4, MO_LESL);
            break;
        case OPC1_16_SLRO_LD_BU:
            r1 = MASK_OP_SLRO_D(opcode);
            const16 = MASK_OP_SLRO_OFF4(opcode);
            //gen_offset_ld(ctx, &core->gpr_d[r1], &core->gpr_a[15], const16, MO_UB);
            break;
        case OPC1_16_SLRO_LD_H:
            r1 = MASK_OP_SLRO_D(opcode);
            const16 = MASK_OP_SLRO_OFF4(opcode);
            //gen_offset_ld(ctx, &core->gpr_d[r1], &core->gpr_a[15], const16 * 2, MO_LESW);
            break;
        case OPC1_16_SLRO_LD_W:
            r1 = MASK_OP_SLRO_D(opcode);
            const16 = MASK_OP_SLRO_OFF4(opcode);
            //gen_offset_ld(ctx, &core->gpr_d[r1], &core->gpr_a[15], const16 * 4, MO_LESL);
            break;
            /* SB-format */
        case OPC1_16_SB_CALL:
        case OPC1_16_SB_J:
        case OPC1_16_SB_JNZ:
        case OPC1_16_SB_JZ:
            address = MASK_OP_SB_DISP8_SEXT(opcode);
            //gen_compute_branch(ctx, op1, 0, 0, 0, address);
            break;
            /* SBC-format */
        case OPC1_16_SBC_JEQ:
        case OPC1_16_SBC_JNE:
            address = MASK_OP_SBC_DISP4(opcode);
            const16 = MASK_OP_SBC_CONST4_SEXT(opcode);
            //gen_compute_branch(ctx, op1, 0, 0, const16, address);
            break;
        case OPC1_16_SBC_JEQ2:
        case OPC1_16_SBC_JNE2:
            /*
               if (tricore_feature(env, TRICORE_FEATURE_16)) {
               address = MASK_OP_SBC_DISP4(ctx->opcode);
               const16 = MASK_OP_SBC_CONST4_SEXT(ctx->opcode);
               gen_compute_branch(ctx, op1, 0, 0, const16, address);
               } else {
               generate_trap(ctx, TRAPC_INSN_ERR, TIN2_IOPC);
               }
               */
            break;
            /* SBRN-format */
        case OPC1_16_SBRN_JNZ_T:
        case OPC1_16_SBRN_JZ_T:
            address = MASK_OP_SBRN_DISP4(opcode);
            const16 = MASK_OP_SBRN_N(opcode);
            //gen_compute_branch(ctx, op1, 0, 0, const16, address);
            break;
            /* SBR-format */
        case OPC1_16_SBR_JEQ2:
        case OPC1_16_SBR_JNE2:
            /*
               if (tricore_feature(env, TRICORE_FEATURE_16)) {
               r1 = MASK_OP_SBR_S2(ctx->opcode);
               address = MASK_OP_SBR_DISP4(ctx->opcode);
               gen_compute_branch(ctx, op1, r1, 0, 0, address);
               } else {
               generate_trap(ctx, TRAPC_INSN_ERR, TIN2_IOPC);
               }
               */
            break;
        case OPC1_16_SBR_JEQ:
        case OPC1_16_SBR_JGEZ:
        case OPC1_16_SBR_JGTZ:
        case OPC1_16_SBR_JLEZ:
        case OPC1_16_SBR_JLTZ:
        case OPC1_16_SBR_JNE:
        case OPC1_16_SBR_JNZ:
        case OPC1_16_SBR_JNZ_A:
        case OPC1_16_SBR_JZ:
        case OPC1_16_SBR_JZ_A:
        case OPC1_16_SBR_LOOP:
            r1 = MASK_OP_SBR_S2(opcode);
            address = MASK_OP_SBR_DISP4(opcode);
            //gen_compute_branch(ctx, op1, r1, 0, 0, address);
            break;
            /* SC-format */
        case OPC1_16_SC_AND:
        case OPC1_16_SC_BISR:
        case OPC1_16_SC_LD_A:
        case OPC1_16_SC_LD_W:
        case OPC1_16_SC_MOV:
        case OPC1_16_SC_OR:
        case OPC1_16_SC_ST_A:
        case OPC1_16_SC_ST_W:
        case OPC1_16_SC_SUB_A:
            //decode_sc_opc(opaque, opcode, op1);
            break;
            /* SLR-format */
        case OPC1_16_SLR_LD_A:
        case OPC1_16_SLR_LD_A_POSTINC:
        case OPC1_16_SLR_LD_BU:
        case OPC1_16_SLR_LD_BU_POSTINC:
        case OPC1_16_SLR_LD_H:
        case OPC1_16_SLR_LD_H_POSTINC:
        case OPC1_16_SLR_LD_W:
        case OPC1_16_SLR_LD_W_POSTINC:
            //decode_slr_opc(opaque, opcode, op1);
            break;
            /* SRO-format */
        case OPC1_16_SRO_LD_A:
        case OPC1_16_SRO_LD_BU:
        case OPC1_16_SRO_LD_H:
        case OPC1_16_SRO_LD_W:
        case OPC1_16_SRO_ST_A:
        case OPC1_16_SRO_ST_B:
        case OPC1_16_SRO_ST_H:
        case OPC1_16_SRO_ST_W:
            //decode_sro_opc(opaque, opcode, op1);
            break;
            /* SSRO-format */
        case OPC1_16_SSRO_ST_A:
            r1 = MASK_OP_SSRO_S1(opcode);
            const16 = MASK_OP_SSRO_OFF4(opcode);
            //gen_offset_st(ctx, &core->gpr_a[r1], &core->gpr_a[15], const16 * 4, MO_LESL);
            break;
        case OPC1_16_SSRO_ST_B:
            r1 = MASK_OP_SSRO_S1(opcode);
            const16 = MASK_OP_SSRO_OFF4(opcode);
            //gen_offset_st(ctx, &core->gpr_d[r1], &core->gpr_a[15], const16, MO_UB);
            break;
        case OPC1_16_SSRO_ST_H:
            r1 = MASK_OP_SSRO_S1(opcode);
            const16 = MASK_OP_SSRO_OFF4(opcode);
            //gen_offset_st(ctx, &core->gpr_d[r1], &core->gpr_a[15], const16 * 2, MO_LESW);
            break;
        case OPC1_16_SSRO_ST_W:
            r1 = MASK_OP_SSRO_S1(opcode);
            const16 = MASK_OP_SSRO_OFF4(opcode);
            //gen_offset_st(ctx, &core->gpr_d[r1], &core->gpr_a[15], const16 * 4, MO_LESL);
            break;
            /* SR-format */
        case OPCM_16_SR_SYSTEM:
            //decode_sr_system(env, ctx);
            break;
        case OPCM_16_SR_ACCU:
            //decode_sr_accu(opcode);
            break;
        case OPC1_16_SR_JI:
            r1 = MASK_OP_SR_S1D(opcode);
            //gen_compute_branch(ctx, op1, r1, 0, 0, 0);
            break;
        case OPC1_16_SR_NOT:
            r1 = MASK_OP_SR_S1D(opcode);
            //tcg_gen_not_tl(&core->gpr_d[r1], &core->gpr_d[r1]);
            break;
        default:
            //generate_trap(ctx, TRAPC_INSN_ERR, TIN2_IOPC);
            break;
    }
}

static void decode_32Bit_opc(conf_object_t* opaque, uint32_t opcode)
{
    int op1;
    int32_t r1, r2, r3;
    int32_t address, const16;
    int8_t b, const4;
    int32_t bpos;
    uint32_t temp, temp2, temp3;

    op1 = MASK_OP_MAJOR(opcode);

    /* handle JNZ.T opcode only being 7 bit long */
    if ((op1 & 0x7f) == OPCM_32_BRN_JTT)
    {
        op1 = OPCM_32_BRN_JTT;
    }

    switch (op1)
    {
        /* ABS-format */
        case OPCM_32_ABS_LDW:
            //decode_abs_ldw(env, ctx);
            break;
        case OPCM_32_ABS_LDB:
            //decode_abs_ldb(env, ctx);
            break;
        case OPCM_32_ABS_LDMST_SWAP:
            //decode_abs_ldst_swap(env, ctx);
            break;
        case OPCM_32_ABS_LDST_CONTEXT:
            //decode_abs_ldst_context(env, ctx);
            break;
        case OPCM_32_ABS_STORE:
            //decode_abs_store(env, ctx);
            break;
        case OPCM_32_ABS_STOREB_H:
            //decode_abs_storeb_h(env, ctx);
            break;
        case OPC1_32_ABS_STOREQ:
            address = MASK_OP_ABS_OFF18(opcode);
            r1 = MASK_OP_ABS_S1D(opcode);
            //temp = tcg_const_i32(EA_ABS_FORMAT(address));
            //temp2 = tcg_temp_new();

            //tcg_gen_shri_tl(temp2, &core->gpr_d[r1], 16);
            //tcg_gen_qemu_st_tl(temp2, temp, ctx->mem_idx, MO_LEUW);
            break;
        case OPC1_32_ABS_LD_Q:
            address = MASK_OP_ABS_OFF18(opcode);
            r1 = MASK_OP_ABS_S1D(opcode);
            //temp = tcg_const_i32(EA_ABS_FORMAT(address));

            //tcg_gen_qemu_ld_tl(&core->gpr_d[r1], temp, ctx->mem_idx, MO_LEUW);
            //tcg_gen_shli_tl(&core->gpr_d[r1], &core->gpr_d[r1], 16);
            break;
        case OPC1_32_ABS_LEA:
            address = MASK_OP_ABS_OFF18(opcode);
            r1 = MASK_OP_ABS_S1D(opcode);
            //tcg_gen_movi_tl(&core->gpr_a[r1], EA_ABS_FORMAT(address));
            break;
            /* ABSB-format */
        case OPC1_32_ABSB_ST_T:
            address = MASK_OP_ABS_OFF18(opcode);
            b = MASK_OP_ABSB_B(opcode);
            bpos = MASK_OP_ABSB_BPOS(opcode);

            //temp = tcg_const_i32(EA_ABS_FORMAT(address));
            //temp2 = tcg_temp_new();

            //tcg_gen_qemu_ld_tl(temp2, temp, ctx->mem_idx, MO_UB);
            //tcg_gen_andi_tl(temp2, temp2, ~(0x1u << bpos));
            //tcg_gen_ori_tl(temp2, temp2, (b << bpos));
            //tcg_gen_qemu_st_tl(temp2, temp, ctx->mem_idx, MO_UB);
            break;
            /* B-format */
        case OPC1_32_B_CALL:
        case OPC1_32_B_CALLA:
        case OPC1_32_B_FCALL:
        case OPC1_32_B_FCALLA:
        case OPC1_32_B_J:
        case OPC1_32_B_JA:
        case OPC1_32_B_JL:
        case OPC1_32_B_JLA:
            address = MASK_OP_B_DISP24_SEXT(opcode);
            //gen_compute_branch(ctx, op1, 0, 0, 0, address);
            break;
            /* Bit-format */
        case OPCM_32_BIT_ANDACC:
            //decode_bit_andacc(env, ctx);
            break;
        case OPCM_32_BIT_LOGICAL_T1:
            //decode_bit_logical_t(env, ctx);
            break;
        case OPCM_32_BIT_INSERT:
            //decode_bit_insert(env, ctx);
            break;
        case OPCM_32_BIT_LOGICAL_T2:
            //decode_bit_logical_t2(env, ctx);
            break;
        case OPCM_32_BIT_ORAND:
            //decode_bit_orand(env, ctx);
            break;
        case OPCM_32_BIT_SH_LOGIC1:
            //decode_bit_sh_logic1(env, ctx);
            break;
        case OPCM_32_BIT_SH_LOGIC2:
            //decode_bit_sh_logic2(env, ctx);
            break;
            /* BO Format */
        case OPCM_32_BO_ADDRMODE_POST_PRE_BASE:
            //decode_bo_addrmode_post_pre_base(env, ctx);
            break;
        case OPCM_32_BO_ADDRMODE_BITREVERSE_CIRCULAR:
            //decode_bo_addrmode_bitreverse_circular(env, ctx);
            break;
        case OPCM_32_BO_ADDRMODE_LD_POST_PRE_BASE:
            //decode_bo_addrmode_ld_post_pre_base(env, ctx);
            break;
        case OPCM_32_BO_ADDRMODE_LD_BITREVERSE_CIRCULAR:
            //decode_bo_addrmode_ld_bitreverse_circular(env, ctx);
            break;
        case OPCM_32_BO_ADDRMODE_STCTX_POST_PRE_BASE:
            //decode_bo_addrmode_stctx_post_pre_base(env, ctx);
            break;
        case OPCM_32_BO_ADDRMODE_LDMST_BITREVERSE_CIRCULAR:
            //decode_bo_addrmode_ldmst_bitreverse_circular(env, ctx);
            break;
            /* BOL-format */
        case OPC1_32_BOL_LD_A_LONGOFF:
        case OPC1_32_BOL_LD_W_LONGOFF:
        case OPC1_32_BOL_LEA_LONGOFF:
        case OPC1_32_BOL_ST_W_LONGOFF:
        case OPC1_32_BOL_ST_A_LONGOFF:
        case OPC1_32_BOL_LD_B_LONGOFF:
        case OPC1_32_BOL_LD_BU_LONGOFF:
        case OPC1_32_BOL_LD_H_LONGOFF:
        case OPC1_32_BOL_LD_HU_LONGOFF:
        case OPC1_32_BOL_ST_B_LONGOFF:
        case OPC1_32_BOL_ST_H_LONGOFF:
            //decode_bol_opc(env, ctx, op1);
            break;
            /* BRC Format */
        case OPCM_32_BRC_EQ_NEQ:
        case OPCM_32_BRC_GE:
        case OPCM_32_BRC_JLT:
        case OPCM_32_BRC_JNE:
            const4 = MASK_OP_BRC_CONST4_SEXT(opcode);
            address = MASK_OP_BRC_DISP15_SEXT(opcode);
            r1 = MASK_OP_BRC_S1(opcode);
            //gen_compute_branch(ctx, op1, r1, 0, const4, address);
            break;
            /* BRN Format */
        case OPCM_32_BRN_JTT:
            address = MASK_OP_BRN_DISP15_SEXT(opcode);
            r1 = MASK_OP_BRN_S1(opcode);
            //gen_compute_branch(ctx, op1, r1, 0, 0, address);
            break;
            /* BRR Format */
        case OPCM_32_BRR_EQ_NEQ:
        case OPCM_32_BRR_ADDR_EQ_NEQ:
        case OPCM_32_BRR_GE:
        case OPCM_32_BRR_JLT:
        case OPCM_32_BRR_JNE:
        case OPCM_32_BRR_JNZ:
        case OPCM_32_BRR_LOOP:
            address = MASK_OP_BRR_DISP15_SEXT(opcode);
            r2 = MASK_OP_BRR_S2(opcode);
            r1 = MASK_OP_BRR_S1(opcode);
            //gen_compute_branch(ctx, op1, r1, r2, 0, address);
            break;
            /* RC Format */
        case OPCM_32_RC_LOGICAL_SHIFT:
            //decode_rc_logical_shift(env, ctx);
            break;
        case OPCM_32_RC_ACCUMULATOR:
            //decode_rc_accumulator(env, ctx);
            break;
        case OPCM_32_RC_SERVICEROUTINE:
            //decode_rc_serviceroutine(env, ctx);
            break;
        case OPCM_32_RC_MUL:
            //decode_rc_mul(env, ctx);
            break;
            /* RCPW Format */
        case OPCM_32_RCPW_MASK_INSERT:
            //decode_rcpw_insert(env, ctx);
            break;
            /* RCRR Format */
        case OPC1_32_RCRR_INSERT:
            r1 = MASK_OP_RCRR_S1(opcode);
            r2 = MASK_OP_RCRR_S3(opcode);
            r3 = MASK_OP_RCRR_D(opcode);
            const16 = MASK_OP_RCRR_CONST4(opcode);
            /*
               temp = tcg_const_i32(const16);
               temp2 = tcg_temp_new(); // width
               temp3 = tcg_temp_new(); // pos

               CHECK_REG_PAIR(r3);

               tcg_gen_andi_tl(temp2, &core->gpr_d[r3+1], 0x1f);
               tcg_gen_andi_tl(temp3, &core->gpr_d[r3], 0x1f);

               gen_insert(&core->gpr_d[r2], &core->gpr_d[r1], temp, temp2, temp3);
               */
            break;
            /* RCRW Format */
        case OPCM_32_RCRW_MASK_INSERT:
            //decode_rcrw_insert(env, ctx);
            break;
            /* RCR Format */
        case OPCM_32_RCR_COND_SELECT:
            //decode_rcr_cond_select(env, ctx);
            break;
        case OPCM_32_RCR_MADD:
            //decode_rcr_madd(env, ctx);
            break;
        case OPCM_32_RCR_MSUB:
            //decode_rcr_msub(env, ctx);
            break;
            /* RLC Format */
        case OPC1_32_RLC_ADDI:
        case OPC1_32_RLC_ADDIH:
        case OPC1_32_RLC_ADDIH_A:
        case OPC1_32_RLC_MFCR:
        case OPC1_32_RLC_MOV:
        case OPC1_32_RLC_MOV_64:
        case OPC1_32_RLC_MOV_U:
        case OPC1_32_RLC_MOV_H:
        case OPC1_32_RLC_MOVH_A:
        case OPC1_32_RLC_MTCR:
            //decode_rlc_opc(env, ctx, op1);
            break;
            /* RR Format */
        case OPCM_32_RR_ACCUMULATOR:
            //decode_rr_accumulator(env, ctx);
            break;
        case OPCM_32_RR_LOGICAL_SHIFT:
            //decode_rr_logical_shift(env, ctx);
            break;
        case OPCM_32_RR_ADDRESS:
            //decode_rr_address(env, ctx);
            break;
        case OPCM_32_RR_IDIRECT:
            //decode_rr_idirect(env, ctx);
            break;
        case OPCM_32_RR_DIVIDE:
            //decode_rr_divide(env, ctx);
            break;
            /* RR1 Format */
        case OPCM_32_RR1_MUL:
            //decode_rr1_mul(env, ctx);
            break;
        case OPCM_32_RR1_MULQ:
            //decode_rr1_mulq(env, ctx);
            break;
            /* RR2 format */
        case OPCM_32_RR2_MUL:
            //decode_rr2_mul(env, ctx);
            break;
            /* RRPW format */
        case OPCM_32_RRPW_EXTRACT_INSERT:
            //decode_rrpw_extract_insert(env, ctx);
            break;
        case OPC1_32_RRPW_DEXTR:
            r1 = MASK_OP_RRPW_S1(opcode);
            r2 = MASK_OP_RRPW_S2(opcode);
            r3 = MASK_OP_RRPW_D(opcode);
            const16 = MASK_OP_RRPW_POS(opcode);
            /*
               if (r1 == r2) {
               tcg_gen_rotli_tl(&core->gpr_d[r3], &core->gpr_d[r1], const16);
               } else {
               temp = tcg_temp_new();
               tcg_gen_shli_tl(temp, &core->gpr_d[r1], const16);
               tcg_gen_shri_tl(&core->gpr_d[r3], &core->gpr_d[r2], 32 - const16);
               tcg_gen_or_tl(&core->gpr_d[r3], &core->gpr_d[r3], temp);
               }
               */
            break;
            /* RRR Format */
        case OPCM_32_RRR_COND_SELECT:
            //decode_rrr_cond_select(env, ctx);
            break;
        case OPCM_32_RRR_DIVIDE:
            //decode_rrr_divide(env, ctx);
            break;
            /* RRR2 Format */
        case OPCM_32_RRR2_MADD:
            //decode_rrr2_madd(env, ctx);
            break;
        case OPCM_32_RRR2_MSUB:
            //decode_rrr2_msub(env, ctx);
            break;
            /* RRR1 format */
        case OPCM_32_RRR1_MADD:
            //decode_rrr1_madd(env, ctx);
            break;
        case OPCM_32_RRR1_MADDQ_H:
            //decode_rrr1_maddq_h(env, ctx);
            break;
        case OPCM_32_RRR1_MADDSU_H:
            //decode_rrr1_maddsu_h(env, ctx);
            break;
        case OPCM_32_RRR1_MSUB_H:
            //decode_rrr1_msub(env, ctx);
            break;
        case OPCM_32_RRR1_MSUB_Q:
            //decode_rrr1_msubq_h(env, ctx);
            break;
        case OPCM_32_RRR1_MSUBAD_H:
            //decode_rrr1_msubad_h(env, ctx);
            break;
            /* RRRR format */
        case OPCM_32_RRRR_EXTRACT_INSERT:
            //decode_rrrr_extract_insert(env, ctx);
            break;
            /* RRRW format */
        case OPCM_32_RRRW_EXTRACT_INSERT:
            //decode_rrrw_extract_insert(env, ctx);
            break;
            /* SYS format */
        case OPCM_32_SYS_INTERRUPTS:
            //decode_sys_interrupts(env, ctx);
            break;
        case OPC1_32_SYS_RSTV:
            /*
               tcg_gen_movi_tl(cpu_PSW_V, 0);
               tcg_gen_mov_tl(cpu_PSW_SV, cpu_PSW_V);
               tcg_gen_mov_tl(cpu_PSW_AV, cpu_PSW_V);
               tcg_gen_mov_tl(cpu_PSW_SAV, cpu_PSW_V);
               */
            break;
        default:
            //generate_trap(ctx, TRAPC_INSN_ERR, TIN2_IOPC);
            break;
    }
}

void decode_opc_and_exec(conf_object_t* opaque, uint32_t opcode)
{
    tricore_cpu_t *core = (tricore_cpu_t *) opaque->obj;
    /* 16-Bit Instruction */
    if ((opcode & 0x1) == 0)
    {
        core->next_pc = core->PC + 2;
        decode_16Bit_opc(opaque, opcode);
        /* 32-Bit Instruction */
    } else
    {
        core->next_pc = core->PC + 4;
        decode_32Bit_opc(opaque, opcode);
    }
}
