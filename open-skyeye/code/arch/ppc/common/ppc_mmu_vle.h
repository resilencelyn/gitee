
#ifndef __PPC_MMU_VLE_H__
#define __PPC_MMU_VLE_H__

/*vle instruction*/
void ppc_vle_opc_lbz(e500_core_t *current_core);
void ppc_vle_se_opc_lbz();
void ppc_vle_opc_lbzu();
void ppc_vle_opc_lha();
void ppc_vle_opc_lhau();
void ppc_vle_opc_lhz();
void ppc_vle_opc_se_lhz();
void ppc_vle_opc_lhzu();
void ppc_vle_opc_li();
void ppc_vle_opc_lis();
void ppc_vle_se_opc_li();
void ppc_vle_opc_lmw();
void ppc_vle_opc_lwz(e500_core_t *current_core);
void ppc_vle_se_opc_lwz();
void ppc_vle_opc_lwzu();
void ppc_vle_opc_stb(e500_core_t *current_core);
void ppc_vle_se_opc_stb();
void ppc_vle_opc_stbu();
void ppc_vle_opc_sth(e500_core_t *current_core);
void ppc_vle_se_opc_sth();
void ppc_vle_opc_sthu();
void ppc_vle_opc_stmw();
void ppc_vle_opc_stw(e500_core_t *current_core);
void ppc_vle_se_opc_stw();
void ppc_vle_opc_stwu();


#endif

