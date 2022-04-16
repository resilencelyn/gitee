#ifndef __PPC_VLE_DEC_H__
#define __PPC_VLE_DEC_H__

#include "skyeye_types.h"
#include "ppc_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void ppc_exec_vle_opc(e500_core_t * core);
#ifdef __cplusplus
}
#endif

typedef unsigned int UInt32;

#define bit(x, n)  (((x) >> (31 - n)) & 1)

#define bits(x, n, m) \
    ({\
     UInt32 y = (x);\
     y = y << m ;\
     y = y >> (31 - (n) + (m)); \
     })

#define PPC_VLE_OPC_MAIN(opc)       (bits(opc,  5,  0))
#define PPC_VLE_OPC_EXT(opc)        (bits(opc, 20, 16))
#define PPC_VLE_OPC_EXT2(opc)       (bits(opc, 23, 21) & 0x7)
#define PPC_VLE_SE_OPC_MAIN(opc)    (bits(opc,  5,  0))
#define PPC_VLE_OPC_BIT16(opc)      (bit(opc, 16))
#define PPC_VLE_OPC_FORM_D_SI(opc, imm) {imm=(opc)&0xffff;if (imm & 0x8000) imm |= 0xffff0000;}

#define oim5(instr)         bits(instr, 11,  7)

#define rx(instr) \
    ({\
     UInt32 y = (instr);\
     y = y << 12 ;\
     y = y >> (31 - (15) + (12)); \
     if (y >= 8) \
     {\
     y = y + 16;\
     }\
     y = y;\
     })

#define ry(instr) \
    ({\
     UInt32 y = (instr);\
     y = y << 8 ;\
     y = y >> (31 - (11) + (8)); \
     if (y >= 8) \
     {\
     y = y + 16;\
     }\
     y = y;\
     })

#define rz(instr)           ry(instr)
#define si(instr)           bits(instr, 31, 16)
#define si_h(instr)         bits(instr, 31, 21)
#define si_l(instr)         bits(instr, 10,  6)
#define ui(instr)           bits(instr, 31, 16)
#define ui_h(instr)         bits(instr, 31, 21)
#define ui_l(instr)         bits(instr, 10,  6)
#define rs(instr)           bits(instr, 10,  6)
#define ra(instr)           bits(instr, 15, 11)
#define rb(instr)           bits(instr, 20, 16)
#define rd(instr)           bits(instr, 10,  6)
#define sh(instr)           bits(instr, 20, 16)
#define _crs(instr)         bits(instr, 13, 11)
#define _crd(instr)         bits(instr,  8,  6)
#define _crd32(instr)       bits(instr, 10,  9)
#define ui5(instr)          bits(instr, 11,  7)
#define ui7(instr)          bits(instr, 11,  5)
#define ui8(instr)          bits(instr, 31, 24)
#define scl(instr)          bits(instr, 23, 22)

#define crbd(instr)         bits(instr, 10,  6)
#define crba(instr)         bits(instr, 15, 11)
#define crbb(instr)         bits(instr, 20, 16)

#define LBZx_SD4(instr)     bits(instr,  7,  4)
#define LBZx_D(instr)       bits(instr, 31, 16)
#define LBZx_D8(instr)      bits(instr, 31, 24)

#define LI20_17_20(instr)   bits(instr, 20, 17)
#define LI20_11_15(instr)   bits(instr, 15, 11)
#define LI20_21_31(instr)   bits(instr, 31, 21)

#define UI_0_4(instr)       bits(instr,  4,  0)
#define UI_6_10(instr)      bits(instr, 10,  6)
#define UI_5_15(instr)      bits(instr, 15,  5)
#define UI_11_15(instr)     bits(instr, 15,  11)
#define UI_21_31(instr)     bits(instr, 31,  21)


#define bd24(instr)         bits(instr, 30, 7)
#define bd15(instr)         bits(instr, 30, 16)
#define bd8(instr)          bits(instr, 15, 8)

#define bi16(instr)         bits(instr, 7,  6)
#define bi32(instr)         bits(instr, 15, 12)
#define bo16(instr)         bit(instr, 5)
#define bo32(instr)         bits(instr, 11, 10)

#define _ary(instr)         bits(instr, 11,  8)
#define _arx(instr)         bits(instr, 15, 12)


#endif
