#ifndef __PPC_VLE_ALU_H__
#define __PPC_VLE_ALU_H__

void ppc_vle_opc_add16i(e500_core_t *current_core);
void ppc_vle_opc_add2i(e500_core_t *current_core);
void ppc_vle_opc_add2is(e500_core_t *current_core);
void ppc_vle_opc_addi(e500_core_t *current_core);
void ppc_vle_opc_addic(e500_core_t *current_core);
void ppc_vle_opc_and2i(e500_core_t *current_core);
void ppc_vle_opc_and2is(e500_core_t *current_core);
void ppc_vle_opc_andi(e500_core_t *current_core);
void ppc_vle_se_opc_add(e500_core_t *current_core);
void ppc_vle_se_opc_addi(e500_core_t *current_core);
void ppc_vle_se_opc_and(e500_core_t *current_core);
void ppc_vle_se_opc_andc(e500_core_t *current_core);
void ppc_vle_se_opc_andi(e500_core_t *current_core);

void ppc_vle_opc_cmp16i(e500_core_t *current_core);
void ppc_vle_opc_cmph(e500_core_t *current_core);
void ppc_vle_opc_cmph16i(e500_core_t *current_core);
void ppc_vle_opc_cmphl(e500_core_t *current_core);
void ppc_vle_opc_cmphl16i(e500_core_t *current_core);
void ppc_vle_opc_cmpi(e500_core_t *current_core);
void ppc_vle_opc_cmpl16i(e500_core_t *current_core);
void ppc_vle_opc_cmpli(e500_core_t *current_core);
void ppc_vle_se_opc_cmp(e500_core_t *current_core);
void ppc_vle_se_opc_cmph(e500_core_t *current_core);
void ppc_vle_se_opc_cmphl(e500_core_t *current_core);
void ppc_vle_se_opc_cmpi(e500_core_t *current_core);
void ppc_vle_se_opc_cmpl(e500_core_t *current_core);
void ppc_vle_se_opc_cmpli(e500_core_t *current_core);

void ppc_vle_opc_crand(e500_core_t *current_core);
void ppc_vle_opc_crandc(e500_core_t *current_core);
void ppc_vle_opc_creqv(e500_core_t *current_core);
void ppc_vle_opc_crnand(e500_core_t *current_core);
void ppc_vle_opc_crnor(e500_core_t *current_core);
void ppc_vle_opc_cror(e500_core_t *current_core);
void ppc_vle_opc_crorc(e500_core_t *current_core);
void ppc_vle_opc_crxor(e500_core_t *current_core);
void ppc_vle_se_opc_extsb(e500_core_t *current_core);
void ppc_vle_se_opc_extsh(e500_core_t *current_core);
void ppc_vle_se_opc_extzb(e500_core_t *current_core);
void ppc_vle_se_opc_extzh(e500_core_t *current_core);

void ppc_vle_se_opc_illegal(e500_core_t *current_core);
void ppc_vle_se_opc_isync(e500_core_t *current_core);
void ppc_vle_opc_mull2i(e500_core_t *current_core);
void ppc_vle_opc_mulli(e500_core_t *current_core);
void ppc_vle_se_opc_mullw(e500_core_t *current_core);
void ppc_vle_se_opc_neg(e500_core_t *current_core);
void ppc_vle_se_opc_not(e500_core_t *current_core);
void ppc_vle_se_opc_or(e500_core_t *current_core);
void ppc_vle_opc_or2i(e500_core_t *current_core);
void ppc_vle_opc_or2is(e500_core_t *current_core);

void ppc_vle_opc_ori(e500_core_t *current_core);
void ppc_vle_opc_rlw(e500_core_t *current_core);
void ppc_vle_opc_rlwi(e500_core_t *current_core);
void ppc_vle_opc_rlwimi(e500_core_t *current_core);
void ppc_vle_opc_rlwinm(e500_core_t *current_core);
void ppc_vle_opc_slwi(e500_core_t *current_core);
void ppc_vle_se_opc_slw(e500_core_t *current_core);
void ppc_vle_se_opc_slwi(e500_core_t *current_core);
void ppc_vle_se_opc_sraw(e500_core_t *current_core);
void ppc_vle_se_opc_srawi(e500_core_t *current_core);
void ppc_vle_opc_srwi(e500_core_t *current_core);
void ppc_vle_se_opc_srw(e500_core_t *current_core);
void ppc_vle_se_opc_srwi(e500_core_t *current_core);
void ppc_vle_se_opc_sub(e500_core_t *current_core);
void ppc_vle_se_opc_subf(e500_core_t *current_core);
void ppc_vle_se_opc_subi(e500_core_t *current_core);
void ppc_vle_opc_subfic(e500_core_t *current_core);
void ppc_vle_opc_xori(e500_core_t *current_core);

#endif

