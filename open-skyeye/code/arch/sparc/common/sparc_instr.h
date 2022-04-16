#ifndef __SPARC_INSTR_H__
#define __SPARC_INSTR_H__
//#include "skyeye_dyncom.h"
//#include <dyncom/frontend.h>

/**
 *  \class sparc_instruction_t
 *  \brief  This structure defines the instruction attributes.
 *
 *  \par execute:   This is the function to be executed.
 *  \par code:      This is the instructon code.
 *  \par disassemble: This is the functions which disassembles the instruction.
 */

typedef struct _i
{
    const char *instr_name;
    int (*execute) (sparc_state_t * state, uint32 instr);
    int (*disassemble) (uint32 instr, void *);
    uint32 opcode_mask;
    int priviledge;
    int format;
    //opc_func_t *opc_func;
} sparc_instruction_t;

#ifdef __cplusplus
extern "C"
{
#endif
    void print_fpinst_RS_RS_RD(char *mnemonic, int rs1, int rs2, int rd);

    void print_fpinst_RS_RS(char *mnemonic, int rs1, int rs2);

    void print_inst_RS_RS_RD(char *mnemonic, int rs1, int rs2, int rd);

    void print_inst_RS_IM13_RD(char *mnemonic, int rs1, int im13, int rd);

    void print_inst_BICC(char *mnemonic, unsigned int addr);

    void print_inst_IMM_RD(char *mnemonic, int im, int rd);

    void print_inst_RD_IMM(char *mnemonic, int rd, int im);

    void print_inst_LS_RS_IM13_RD(char *mnemonic, int rs1, int im13, int rd);

    void print_inst_LS_RS_RS_RD(char *mnemonic, int rd, int rs1, int rs2);

    void print_inst_ST_RS_RS_RD(char *mnemonic, int rd, int rs1, int rs2);

    void print_inst_ST_RS_IM13_RD(char *mnemonic, int rd, int rs1, int im13);

    void print_inst_RD(char *mnemonic, int rd);

    sparc_return_t iu_i_register(sparc_instruction_t * i_new);
    sparc_instruction_t *iu_get_instr(conf_object_t * conf_obj, uint32 instr);

#ifdef __cplusplus
}
#endif
#endif
