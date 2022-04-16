#include "ppc_cpu.h"
#include "ppc_alu_vle.h"
#include "ppc_dec_vle.h"
#include "ppc_alu.h"
#include "ppc_tools.h"
#include "ppc_dec.h"
#include "ppc_opc.h"

static inline uint32 ppc_mask_vle(int MB, int ME)
{
    uint32 mask;

    if (MB <= ME)
    {
        if (ME - MB == 31)
        {
            mask = 0xffffffff;
        } else
        {
            mask = ((1 << (ME - MB + 1)) - 1) << (31 - ME);
        }
    } else
    {
        mask = ppc_word_rotl((1 << (32 - MB + ME + 1)) - 1, 31 - ME);
    }
    return mask;
}

uint32_t ppc_sci8_cac(e500_core_t *current_core, uint8_t addi_F, uint8_t addi_scl)
{
    uint32_t imm;

    switch(addi_scl)
    {
        case 0:
            if (addi_F == 0)
            {
                imm = ui8(current_core->current_opc) & 0xFF;
            } else
            {
                imm = 0xFFFFFF00 | (ui8(current_core->current_opc) << 0);
            }
            break;
        case 1:
            if (addi_F == 0)
            {
                imm = (ui8(current_core->current_opc) << 8) & 0xFF00;
            } else
            {
                imm = 0xFFFF00FF | (ui8(current_core->current_opc) << 8);
            }
            break;
        case 2:
            if (addi_F == 0)
            {
                 imm = (ui8(current_core->current_opc) << 16) & 0xFF0000;
            } else
            {
                imm = 0xFF00FFFF | (ui8(current_core->current_opc) << 16);
            }
        default:
            if (addi_F == 0)
            {
                 imm = (ui8(current_core->current_opc) << 24) & 0xFF000000;
            } else
            {
                imm = 0x00FFFFFF | (ui8(current_core->current_opc) << 24);
            }
            break;
    }

    return imm;
}

void ppc_rc_update(e500_core_t *current_core, uint32_t imm)
{
    int lt,gt,eq;
    int64_t result = (int64_t)imm;

    lt = result < 0 ? 1 : 0;
    gt = result > 0 ? 1 : 0;
    eq = result == 0 ? 1 : 0;

    current_core->cr &= 0xFFFFFFF1;
    current_core->cr |= lt << 31 | gt << 30 | eq << 29;
}

void ppc_vle_opc_add16i(e500_core_t *current_core)
{
    uint32_t imm;
    PPC_VLE_OPC_FORM_D_SI(current_core->current_opc, imm);

    current_core->gpr[rd(current_core->current_opc)] = current_core->gpr[ra(current_core->current_opc)] + imm;
}

void ppc_vle_opc_add2i(e500_core_t *current_core)
{
    uint32_t sih = si_h(current_core->current_opc);
    uint32_t sil = si_l(current_core->current_opc);
    uint32_t imm = ((sil << 11) | (sih));
    uint32_t sum;

    if(imm & 0x8000)
    {
        imm |= 0xFFFF0000;
    }

    sum = current_core->gpr[ra(current_core->current_opc)] + imm;
    ppc_rc_update(current_core, sum);
    current_core->gpr[ra(current_core->current_opc)] = sum;
}

void ppc_vle_opc_add2is(e500_core_t *current_core)
{
    uint32_t sil = si_l(current_core->current_opc);
    uint32_t sih = si_h(current_core->current_opc);
    uint32_t imm = ((sil << 11) | (sih));
    uint32_t sum = current_core->gpr[ra(current_core->current_opc)] + (imm << 16 & 0xFFFF0000);

    current_core->gpr[ra(current_core->current_opc)] = sum;
}

void ppc_vle_opc_addi(e500_core_t *current_core)
{
    uint8_t addi_scl = scl(current_core->current_opc);
    uint8_t addi_F = bit(current_core->current_opc, 21);
    uint32_t Rc = bit(current_core->current_opc, 20);
    uint32_t imm = ppc_sci8_cac(current_core, addi_F, addi_scl);
    uint32_t sum = imm + current_core->gpr[ra(current_core->current_opc)];

    if (Rc == 1)
    {
        ppc_rc_update(current_core, sum);
    }
    current_core->gpr[rd(current_core->current_opc)] = sum;
}

void ppc_vle_opc_addic(e500_core_t *current_core)
{
    int lt,gt,eq;

    uint32_t Rc = bit(current_core->current_opc, 20);
    uint8_t addi_scl = scl(current_core->current_opc);
    uint8_t addi_F = bit(current_core->current_opc, 21);
    uint32_t imm = ppc_sci8_cac(current_core, addi_F, addi_scl);
    uint32_t sum = imm + current_core->gpr[ra(current_core->current_opc)];

    if(Rc == 1)
    {
        ppc_rc_update(current_core, sum);
    }

    current_core->gpr[rd(current_core->current_opc)] = sum;

    if (current_core->gpr[rd(current_core->current_opc)] < imm)
    {
        current_core->xer |= XER_CA;
    } else
    {
        current_core->xer &= ~XER_CA;
    }
}

void ppc_vle_opc_and2i(e500_core_t *current_core)
{
    uint32_t ui0_4 = bits(current_core->current_opc, 15, 11);
    uint32_t ui5_15 = bits(current_core->current_opc, 31, 21);
    uint32_t rt =  bits(current_core->current_opc, 10, 6);
    uint32_t b = (ui0_4 <<  11) | (ui5_15 & 0x7FF);
    uint32_t result = current_core->gpr[rt] & b;

    current_core->gpr[rt] = result;

    // update cr0 flags
    ppc_update_cr0(current_core, result);
}

void ppc_vle_opc_and2is(e500_core_t *current_core)
{
    uint32_t ui0_4 = bits(current_core->current_opc, 15, 11);
    uint32_t ui5_15 = bits(current_core->current_opc, 31, 21);
    uint32_t b = ((ui0_4 << 27) | (ui5_15 << 16) & 0xFFFF0000);
    uint32_t result = current_core->gpr[rd(current_core->current_opc)] & b;

    current_core->gpr[rd(current_core->current_opc)] = result;
}

void ppc_vle_opc_andi(e500_core_t *current_core)
{
    uint8_t andi_scl = scl(current_core->current_opc);
    uint8_t andi_F = bit(current_core->current_opc, 21);
    uint32_t Rc = bit(current_core->current_opc, 20);
    uint32_t b = ppc_sci8_cac(current_core, andi_F, andi_scl);
    uint32_t result = current_core->gpr[rs(current_core->current_opc)] & b;

    if (Rc == 1)
    {
        ppc_rc_update(current_core, result);
    }
    current_core->gpr[ra(current_core->current_opc)] = result;
}

void ppc_vle_se_opc_add(e500_core_t *current_core)
{
    current_core->gpr[rx(current_core->current_opc)] =
        current_core->gpr[rx(current_core->current_opc)] + current_core->gpr[ry(current_core->current_opc)];
}

void ppc_vle_se_opc_addi(e500_core_t *current_core)
{
    current_core->gpr[rx(current_core->current_opc)] =
        current_core->gpr[rx(current_core->current_opc)] + oim5(current_core->current_opc) + 1;
}

void ppc_vle_se_opc_and(e500_core_t *current_core)
{
    uint32_t b = current_core->gpr[ry(current_core->current_opc)];
    uint32_t Rc = bit(current_core->current_opc, 7);
    uint32_t result = current_core->gpr[rx(current_core->current_opc)] & b;

    if (Rc == 1)
    {
         ppc_rc_update(current_core, result);
    }

    current_core->gpr[rx(current_core->current_opc)] = result;
}

void ppc_vle_se_opc_andc(e500_core_t *current_core)
{
    uint32_t b = ~current_core->gpr[ry(current_core->current_opc)];
    uint32_t result = current_core->gpr[rx(current_core->current_opc)] & b;

    current_core->gpr[rx(current_core->current_opc)] = result;
}

void ppc_vle_se_opc_andi(e500_core_t *current_core)
{
    uint32_t b = ui5(current_core->current_opc);
    uint32_t result = current_core->gpr[rx(current_core->current_opc)] & b;

    current_core->gpr[rx(current_core->current_opc)] = result;
}

void ppc_vle_opc_cmp16i(e500_core_t *current_core)
{
    uint32_t cmp_sil = si_l(current_core->current_opc);
    uint32_t cmp_sih = si_h(current_core->current_opc);
    sint32 a = current_core->gpr[ra(current_core->current_opc)];
    sint32 b = 0;
    uint32_t si = cmp_sil << 11 | cmp_sih;
    uint32_t c;

    if(si & 0x8000)
    {
        si |= 0xFFFF0000;
    }

    b = (sint32)si;

    if (a < b)
    {
        c = 8;
    } else if(a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_opc_cmph(e500_core_t *current_core)
{
    uint32_t ra_48_63 = current_core->gpr[ra(current_core->current_opc)] & 0xFFFF;
    uint32_t rb_48_63 = current_core->gpr[rb(current_core->current_opc)] & 0xFFFF;
    uint8_t crd = _crd(current_core->current_opc);
    uint32_t c = 0;
    sint32 a, b = 0;

    if (ra_48_63 & 0x8000)
    {
        ra_48_63 |= 0xFFFF0000;
    }

    if (rb_48_63 & 0x8000)
    {
        rb_48_63 |= 0xFFFF0000;
    }

    a = (sint32)ra_48_63;
    b = (sint32)rb_48_63;

    if (a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }
    crd = 7 - crd;

    current_core->cr &= ppc_cmp_and_mask[crd];
    current_core->cr |= c << (crd * 4);
}

void ppc_vle_opc_cmph16i(e500_core_t *current_core)
{
    uint32_t ra_48_63 = current_core->gpr[ra(current_core->current_opc)] & 0xFFFF;
    uint32_t cmph16i_sil = si_l(current_core->current_opc);
    uint32_t cmph16i_sih = si_h(current_core->current_opc);
    uint32_t cmph16i_si = cmph16i_sil << 11 | cmph16i_sih;
    uint32_t c = 0;
    sint32 a, b = 0;

    if(ra_48_63 & 0x8000)
    {
        ra_48_63 |= 0xFFFF0000;
    }

    if(cmph16i_si & 0x8000)
    {
        cmph16i_si |= 0xFFFF0000;
    }

    a = ra_48_63;
    b = cmph16i_si;

    if (a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_opc_cmphl(e500_core_t *current_core)
{
    uint32_t a = current_core->gpr[ra(current_core->current_opc)] & 0xFFFF;
    uint32_t b = current_core->gpr[rb(current_core->current_opc)] & 0xFFFF;
    uint8_t crd = _crd(current_core->current_opc);
    uint32_t c = 0;

    if(a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }
    crd = 7 - crd;

    current_core->cr &= ppc_cmp_and_mask[crd];
    current_core->cr |= c << (crd * 4);
}

void ppc_vle_opc_cmphl16i(e500_core_t *current_core)
{
    uint32_t a = current_core->gpr[ra(current_core->current_opc)]  & 0xFFFF;
    uint32_t b = ((ui_l(current_core->current_opc) << 11)
            | (ui_h(current_core->current_opc)));
    uint32_t c = 0;

    if (a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_opc_cmpi(e500_core_t *current_core)
{
    uint32_t cmpi_F = bit(current_core->current_opc, 21);
    uint32_t cmpi_scl = scl(current_core->current_opc);
    uint32_t crd32 = _crd32(current_core->current_opc);
    sint32 a = current_core->gpr[ra(current_core->current_opc)];
    sint32 b = ppc_sci8_cac(current_core, cmpi_F, cmpi_scl);
    uint32_t c = 0;

    if(a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }
    crd32 = 7 - crd32;

    current_core->cr &= ppc_cmp_and_mask[crd32];
    current_core->cr |= c << (crd32 * 4);
}

void ppc_vle_opc_cmpl16i(e500_core_t *current_core)
{
    uint32_t a = current_core->gpr[ra(current_core->current_opc)];
    uint32_t ui = ((ui_l(current_core->current_opc) << 11)
            | (ui_h(current_core->current_opc)));
    uint32_t c;
    uint32_t b;

    b = ui;

    if (a < b)
    {
        c = 8;
    } else if(a > b)
    {
        c= 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_opc_cmpli(e500_core_t *current_core)
{
    uint32_t cmp_F = bit(current_core->current_opc, 21);
    uint32_t cmp_scl = bits(current_core->current_opc, 23, 22);
    uint32_t a = current_core->gpr[ra(current_core->current_opc)];
    uint32_t b = ppc_sci8_cac(current_core, cmp_F, cmp_scl);
    uint8_t crd32 = _crd32(current_core->current_opc);
    uint32_t c;

    if (a < b)
    {
        c = 8;
    } else if(a > b)
    {
        c= 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }
    crd32 = 7 - crd32;

    current_core->cr &= ppc_cmp_and_mask[crd32];
    current_core->cr |= c << (crd32 * 4);
}

void ppc_vle_se_opc_cmp(e500_core_t *current_core)
{
    int32_t a = current_core->gpr[rx(current_core->current_opc)];
    int32_t b = current_core->gpr[ry(current_core->current_opc)];
    uint32_t c = 0;

    if(a < b)
    {
        c = 8;
    } else if(a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_se_opc_cmph(e500_core_t *current_core)
{
    uint32_t rx_48_63 = current_core->gpr[rx(current_core->current_opc)] & 0xFFFF;
    uint32_t ry_48_63 = current_core->gpr[ry(current_core->current_opc)] & 0xFFFF;
    sint32 a, b;
    uint32_t c = 0;

    if(rx_48_63 & 0x8000)
    {
        rx_48_63 |= 0xFFFF0000;
    }

    if(ry_48_63 & 0x8000)
    {
        ry_48_63 |= 0xFFFF0000;
    }

    a = rx_48_63;
    b = ry_48_63;

    if(a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_se_opc_cmphl(e500_core_t *current_core)
{
    uint32_t a = current_core->gpr[rx(current_core->current_opc)] & 0XFFFF;
    uint32_t b = current_core->gpr[ry(current_core->current_opc)] & 0XFFFF;
    uint32_t c = 0;

    if(a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_se_opc_cmpi(e500_core_t *current_core)
{
    uint8_t rx_val =rx(current_core->current_opc);
    sint32 a = current_core->gpr[rx_val];
    sint32 b = ui5(current_core->current_opc);
    uint32_t c = 0;

    if(a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_se_opc_cmpl(e500_core_t *current_core)
{
    uint32_t a = current_core->gpr[rx(current_core->current_opc)];
    uint32_t b = current_core->gpr[ry(current_core->current_opc)];
    uint32_t c = 0;

    if(a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_se_opc_cmpli(e500_core_t *current_core)
{
    uint32_t a = current_core->gpr[rx(current_core->current_opc)];
    uint32_t b = oim5(current_core->current_opc) + 1;
    uint32_t c;

    if(a < b)
    {
        c = 8;
    } else if (a > b)
    {
        c = 4;
    } else
    {
        c = 2;
    }

    if (current_core->xer & XER_SO)
    {
        c |= 1;
    }

    current_core->cr &= ppc_cmp_and_mask[7];
    current_core->cr |= (c << 28);
}

void ppc_vle_opc_crand(e500_core_t *current_core)
{
    uint8_t cr_ba = crba(current_core->current_opc);
    uint8_t cr_bb = crbb(current_core->current_opc);
    uint8_t cr_bd = crbd(current_core->current_opc);

    if ((current_core->cr & (1 << (31 - cr_ba))) && (current_core->cr & (1 << (31 - cr_bb))))
    {
        current_core->cr |= (1 << (31 - cr_bd));
    } else
    {
        current_core->cr &= ~(1 << (31 - cr_bd));
    }
}

void ppc_vle_opc_crandc(e500_core_t *current_core)
{
    uint8_t cr_ba = crba(current_core->current_opc);
    uint8_t cr_bb = crbb(current_core->current_opc);
    uint8_t cr_bd = crbd(current_core->current_opc);

    if ((current_core->cr & (1 << (31 - cr_ba))) && (!(current_core->cr & (1 << (31 - cr_bb)))))
    {
        current_core->cr |= (1 << (31 - cr_bd));
    } else
    {
        current_core->cr &= ~(1 << (31 - cr_bd));
    }
}

void ppc_vle_opc_creqv(e500_core_t *current_core)
{
    uint8_t cr_ba = crba(current_core->current_opc);
    uint8_t cr_bb = crbb(current_core->current_opc);
    uint8_t cr_bd = crbd(current_core->current_opc);
    uint32_t ba = (current_core->cr & (1 << (31 - cr_ba)));
    uint32_t bb = (current_core->cr & (1 << (31 - cr_bb)));

    if((ba && bb) || (!ba && !bb))
    {
        current_core->cr |= (1 << (31 - cr_bd));
    } else
    {
        current_core->cr &= ~(1 << (31 - cr_bd));
    }
}

void ppc_vle_opc_crnand(e500_core_t *current_core)
{
    uint8_t cr_ba = crba(current_core->current_opc);
    uint8_t cr_bb = crbb(current_core->current_opc);
    uint8_t cr_bd = crbd(current_core->current_opc);

    if (!((current_core->cr & (1 << (31 - cr_ba))) && (current_core->cr & (1 << (31 - cr_bb)))))
    {
        current_core->cr |= (1 << (31 - cr_bd));
    } else
    {
        current_core->cr &= ~(1 << (31 - cr_bd));
    }
}

void ppc_vle_opc_crnor(e500_core_t *current_core)
{
    uint8_t cr_ba = crba(current_core->current_opc);
    uint8_t cr_bb = crbb(current_core->current_opc);
    uint8_t cr_bd = crbd(current_core->current_opc);

    uint32 t = (1 << (31 - cr_ba)) | (1 << (31 - cr_bb));

    if (!(current_core->cr & t))
    {
        current_core->cr |= (1 << (31 - cr_bd));
    } else
    {
        current_core->cr &= ~(1 << (31 - cr_bd));
    }
}

void ppc_vle_opc_cror(e500_core_t *current_core)
{
    uint8_t cr_ba = crba(current_core->current_opc);
    uint8_t cr_bb = crbb(current_core->current_opc);
    uint8_t cr_bd = crbd(current_core->current_opc);
    uint32 t = (1 << (31 - cr_ba)) | (1 << (31 - cr_bb));

    if (current_core->cr & t)
    {
        current_core->cr |= (1 << (31 - cr_bd));
    } else
    {
        current_core->cr &= ~(1 << (31 - cr_bd));
    }
}

void ppc_vle_opc_crorc(e500_core_t *current_core)
{
    uint8_t cr_ba = crba(current_core->current_opc);
    uint8_t cr_bb = crbb(current_core->current_opc);
    uint8_t cr_bd = crbd(current_core->current_opc);

    if ((current_core->cr & (1 << (31 - cr_ba))) || !(current_core->cr & (1 << (31 - cr_bb))))
    {
        current_core->cr |= (1 << (31 - cr_bd));
    } else
    {
        current_core->cr &= ~(1 << (31 - cr_bd));
    }
}

void ppc_vle_opc_crxor(e500_core_t *current_core)
{
    uint8_t cr_ba = crba(current_core->current_opc);
    uint8_t cr_bb = crbb(current_core->current_opc);
    uint8_t cr_bd = crbd(current_core->current_opc);

    if ((!(current_core->cr & (1 << (31 - cr_ba)))
                && (current_core->cr & (1 << (31 - cr_bb))))
            || ((current_core->cr & (1 << (31 - cr_ba))) && !(current_core->cr & (1 << (31 - cr_bb)))))
    {
        current_core->cr |= (1 << (31 - cr_bd));
    } else
    {
        current_core->cr &= ~(1 << (31 - cr_bd));
    }
}

void ppc_vle_se_opc_extsb(e500_core_t *current_core)
{
    uint32_t s = (current_core->gpr[rx(current_core->current_opc)]) & 0x80;
    uint32_t prefix = (s == 1 ? 0xFFFFFF00 : 0x0);

    current_core->gpr[rx(current_core->current_opc)] =
        (prefix | (current_core->gpr[rx(current_core->current_opc)] & 0xFF));
}

void ppc_vle_se_opc_extsh(e500_core_t *current_core)
{
    uint32_t s = (current_core->gpr[rx(current_core->current_opc)]) & 0x8000;
    uint32_t prefix = (s == 1 ? 0xFFFF0000 : 0x0);

    current_core->gpr[rx(current_core->current_opc)] =
        (prefix | (current_core->gpr[rx(current_core->current_opc)] & 0xFFFF));
}

void ppc_vle_se_opc_extzb(e500_core_t *current_core)
{
    current_core->gpr[rx(current_core->current_opc)] =
        (0xFF & current_core->gpr[rx(current_core->current_opc)]);
}

void ppc_vle_se_opc_extzh(e500_core_t *current_core)
{
    current_core->gpr[rx(current_core->current_opc)] =
        (0xFFFF & current_core->gpr[rx(current_core->current_opc)]);
}

void ppc_vle_se_opc_illegal(e500_core_t *current_core)
{
    uint32_t srr1 = current_core->msr;
    uint32_t srr0 = current_core->pc;

    uint32_t nxt_ia = current_core->ivpr;
    current_core->npc = nxt_ia;

    current_core->msr &= 0xFFFFFF00;

    printf("excute Vle se_illegal insn!\n");
}

void ppc_vle_se_opc_isync(e500_core_t *current_core)
{
    //printf("excute isync insn!\n");
}

void ppc_vle_opc_mull2i(e500_core_t *current_core)
{
    uint8_t mull2i_ra = ra(current_core->current_opc);
    uint32_t sil = si_l(current_core->current_opc);
    uint32_t sih = si_h(current_core->current_opc);
    uint32_t si = (sil << 11) | sih;
    sint32 a, b;

    if(si & 0x8000)
    {
        si |= 0xFFFF0000;
    }

    a = (sint32)si;
    b = (sint32)current_core->gpr[mull2i_ra];

    uint64_t prod = a * b;
    current_core->gpr[mull2i_ra] = (uint32_t )(prod & 0xFFFFFFFF);
}

void ppc_vle_opc_mulli(e500_core_t *current_core)
{
    uint8_t mulli_ra = ra(current_core->current_opc);
    uint8_t mulli_rd = rd(current_core->current_opc);
    uint8_t mulli_F = bit(current_core->current_opc, 21);
    uint8_t mulli_scl = scl(current_core->current_opc);
    uint8_t mulli_ui8 = ui8(current_core->current_opc);
    sint32 imm = ppc_sci8_cac(current_core, mulli_F, mulli_scl);
    uint64_t prod = imm * (sint32)current_core->gpr[mulli_ra];

    current_core->gpr[mulli_rd] = (uint32_t )(prod & 0xFFFFFFFF);
}

void ppc_vle_se_opc_mullw(e500_core_t *current_core)
{
    uint8_t mullw_rx = rx(current_core->current_opc);
    uint8_t mullw_ry = ry(current_core->current_opc);
    uint64_t prod = current_core->gpr[mullw_rx] * current_core->gpr[mullw_ry];

    current_core->gpr[mullw_rx] = (uint32_t )(prod & 0xFFFFFFFF);
}

void ppc_vle_se_opc_neg(e500_core_t *current_core)
{
    uint8_t neg_rx = rx(current_core->current_opc);
    uint32_t result = ~(current_core->gpr[neg_rx]) + 1;

    current_core->gpr[neg_rx] = result;
}

void ppc_vle_se_opc_not(e500_core_t *current_core)
{
    uint8_t not_rx = rx(current_core->current_opc);
    uint32_t result = ~(current_core->gpr[not_rx]);

    current_core->gpr[not_rx] = result;
}

void ppc_vle_se_opc_or(e500_core_t *current_core)
{
    uint8_t or_rx = rx(current_core->current_opc);
    uint8_t or_ry = ry(current_core->current_opc);
    uint32_t result = current_core->gpr[or_rx] | current_core->gpr[or_ry] ;

    current_core->gpr[or_rx] = result;
}

void ppc_vle_opc_or2i(e500_core_t *current_core)
{
    uint8_t or2i_rd = rd(current_core->current_opc);
    uint32_t ui_11_15 = UI_11_15(current_core->current_opc);
    uint32_t ui_21_31 = UI_21_31(current_core->current_opc);
    uint32_t b = (ui_11_15 << 11) | ui_21_31;
    uint32_t result = current_core->gpr[or2i_rd] | b;

    current_core->gpr[or2i_rd] = result;
}

void ppc_vle_opc_or2is(e500_core_t *current_core)
{
    uint8_t or2i_rd = rd(current_core->current_opc);
    uint32_t ui_11_15 = UI_11_15(current_core->current_opc);
    uint32_t ui_21_31 = UI_21_31(current_core->current_opc);
    uint32_t  b = ((ui_11_15 << 27) | (ui_21_31 << 16)) & 0xFFFF0000;
    uint32_t result = current_core->gpr[or2i_rd] | b;

    current_core->gpr[or2i_rd] = result;
}

void ppc_vle_opc_ori(e500_core_t *current_core)
{
    uint8_t ori_rs = rs(current_core->current_opc);
    uint8_t ori_ra = ra(current_core->current_opc);
    uint8_t ori_rc = bit(current_core->current_opc, 20);
    uint8_t ori_F = bit(current_core->current_opc, 21);
    uint8_t ori_scl = scl(current_core->current_opc);
    uint8_t ori_ui8 = ui8(current_core->current_opc);
    uint32_t b = ppc_sci8_cac(current_core, ori_F, ori_scl);
    uint32_t result = current_core->gpr[ori_rs] | b;

    if(ori_rc == 1)
    {
        // update cr0 flags
        ppc_update_cr0(current_core, result);
    }
    current_core->gpr[ori_ra] = result;
}

void ppc_vle_opc_rlw(e500_core_t *current_core)
{
    uint8_t rlwx_rs = rs(current_core->current_opc);
    uint8_t rlwx_ra = ra(current_core->current_opc);
    uint8_t rlwx_rb = rb(current_core->current_opc);
    uint8_t rlwx_rc = bit(current_core->current_opc, 31);

    uint8_t n = (current_core->gpr[rlwx_rb] & 0x1F);
    uint32_t result = ppc_word_rotl(current_core->gpr[rlwx_rs], n);

    if (rlwx_rc == 1)
    {
        // update cr0 flags
        ppc_update_cr0(current_core, result);
    }
    current_core->gpr[rlwx_ra] = result;
}

void ppc_vle_opc_rlwi(e500_core_t *current_core)
{
    uint8_t rlwix_rs = rs(current_core->current_opc);
    uint8_t rlwix_ra = ra(current_core->current_opc);
    uint8_t rlwix_sh = sh(current_core->current_opc);
    uint8_t rlwix_rc = bit(current_core->current_opc, 31);

    uint8_t n = rlwix_sh;
    uint32_t result = ppc_word_rotl(current_core->gpr[rlwix_rs], n);

    if (rlwix_rc == 1)
    {
        // update cr0 flags
        ppc_update_cr0(current_core, result);
    }
    current_core->gpr[rlwix_ra] = result;
}

void ppc_vle_opc_rlwimi(e500_core_t *current_core)
{
    int rS, rA, SH, MB, ME;
    PPC_OPC_TEMPL_M(current_core->current_opc, rS, rA, SH, MB, ME);
    uint32_t r = ppc_word_rotl(current_core->gpr[rS], SH);
    uint32_t m = ppc_mask_vle(MB, ME);
    uint32_t result = ((r & m) | (current_core->gpr[rA] & (~m)));

    current_core->gpr[rA] = result;
}

void ppc_vle_opc_rlwinm(e500_core_t *current_core)
{
    int rS, rA, SH, MB, ME;
    PPC_OPC_TEMPL_M(current_core->current_opc, rS, rA, SH, MB, ME);
    uint32_t r = ppc_word_rotl(current_core->gpr[rS], SH);
    uint32_t m = ppc_mask_vle(MB, ME);
    uint32_t result = (r & m);

    current_core->gpr[rA] = result;
}

void ppc_vle_opc_slwi(e500_core_t *current_core)
{
    uint8_t slwix_rs = rs(current_core->current_opc);
    uint8_t slwix_ra = ra(current_core->current_opc);
    uint8_t slwix_sh = sh(current_core->current_opc);
    uint8_t slwix_rc = bit(current_core->current_opc, 31);

    current_core->gpr[slwix_ra] = current_core->gpr[slwix_rs] << slwix_sh;

    if (slwix_rc == 1)
    {
        // update cr0 flags
        ppc_update_cr0(current_core, current_core->gpr[slwix_ra]);
    }
}

void ppc_vle_se_opc_slw(e500_core_t *current_core)
{
    uint8_t slw_rx = rx(current_core->current_opc);
    uint8_t slw_ry = ry(current_core->current_opc);
    uint8_t n = current_core->gpr[slw_ry] & 0x3F;

    if(n & 0x20)
    {
        current_core->gpr[slw_rx]  = 0;
    } else
    {
        current_core->gpr[slw_rx]  =  current_core->gpr[slw_rx] << n;
    }
}

void ppc_vle_se_opc_slwi(e500_core_t *current_core)
{
    uint8_t slwi_rx = rx(current_core->current_opc);
    uint8_t slw_ui5 = ui5(current_core->current_opc);

    current_core->gpr[slwi_rx] = current_core->gpr[slwi_rx] << slw_ui5;
}

void ppc_vle_se_opc_sraw(e500_core_t *current_core)
{
    uint8_t sraw_rx = rx(current_core->current_opc);
    uint8_t sraw_ry = ry(current_core->current_opc);
    uint8_t n = current_core->gpr[sraw_ry] & 0x1F;
    uint32_t r = ppc_word_rotl(current_core->gpr[sraw_rx], 32 - n);
    uint32_t m, s, result;

    if((current_core->gpr[sraw_ry] & (1 << 5)) == 1)
    {
        m = 0;
    } else
    {
        m = ppc_mask_vle(n, 31);
    }

    s = (current_core->gpr[sraw_rx] >> 31) & 0x1;
    if (s == 1)
    {
         result = (r & m) | (0xFFFFFFFF & (~m));
    } else
    {
         result = (r & m);
    }

    current_core->gpr[sraw_rx] = result;
    current_core->xer &= ~(XER_CA);
    current_core->xer |= (s & (((r & ~m) != 0) ? 1 : 0)) << 29;
}

void ppc_vle_se_opc_srawi(e500_core_t *current_core)
{
    uint8_t srawi_rx = rx(current_core->current_opc);
    uint8_t srawi_ui5 = ui5(current_core->current_opc);
    uint8_t n = srawi_ui5;
    uint32_t r = ppc_word_rotl(current_core->gpr[srawi_rx], 32 - n);
    uint32_t m, s , result;

    m = ppc_mask_vle(n, 31);
    s = (current_core->gpr[srawi_rx] >> 31) & 0x1;
    if (s == 1)
    {
         result = (r & m) | (0xFFFFFFFF & (~m));
    } else
    {
         result = (r & m);
    }

    current_core->gpr[srawi_rx] = result;
    current_core->xer &= ~(XER_CA);
    current_core->xer |= (s & (((r & ~m) != 0) ? 1 : 0)) << 29;
}

void ppc_vle_opc_srwi(e500_core_t *current_core)
{
    uint8_t srwix_rs = rs(current_core->current_opc);
    uint8_t srwix_ra = ra(current_core->current_opc);
    uint8_t srwix_sh = sh(current_core->current_opc);
    uint8_t srwix_rc = bit(current_core->current_opc, 31);

    current_core->gpr[srwix_ra] = current_core->gpr[srwix_rs] >> srwix_sh;

    if (srwix_rc == 1)
    {
        ppc_update_cr0(current_core, current_core->gpr[srwix_ra]);
    }

}

void ppc_vle_se_opc_srw(e500_core_t *current_core)
{
    uint8_t srw_rx = rx(current_core->current_opc);
    uint8_t srw_ry = ry(current_core->current_opc);
    uint8_t n = current_core->gpr[srw_ry] & 0x1F;

    if((current_core->gpr[srw_ry] & (1 << 5)) == 1)
    {
        current_core->gpr[srw_rx] = 0;
    } else
    {
        current_core->gpr[srw_rx] = current_core->gpr[srw_rx] >> n;
    }
}

void ppc_vle_se_opc_srwi(e500_core_t *current_core)
{
    uint8_t srwi_rx = rx(current_core->current_opc);
    uint8_t srwi_ui5 = ui5(current_core->current_opc);

    current_core->gpr[srwi_rx] = current_core->gpr[srwi_rx] >> srwi_ui5;
}

void ppc_vle_se_opc_sub(e500_core_t *current_core)
{
    uint8_t sub_rx = rx(current_core->current_opc);
    uint8_t sub_ry = ry(current_core->current_opc);
    uint32_t sum = current_core->gpr[sub_rx] + (~(current_core->gpr[sub_ry])) + 1;

    current_core->gpr[sub_rx] = sum;
}

void ppc_vle_se_opc_subf(e500_core_t *current_core)
{
    uint8_t subf_rx = rx(current_core->current_opc);
    uint8_t subf_ry = ry(current_core->current_opc);
    uint32_t sum = (~current_core->gpr[subf_rx]) + (current_core->gpr[subf_ry]) + 1;

    current_core->gpr[subf_rx] = sum;
}

void ppc_vle_se_opc_subi(e500_core_t *current_core)
{
    uint8_t subix_rx = rx(current_core->current_opc);
    uint8_t subix_oim5 = oim5(current_core->current_opc) + 1;
    uint8_t subix_rc = bit(current_core->current_opc, 6);
    uint32_t sum = current_core->gpr[subix_rx] + ~subix_oim5 + 1;

    if(subix_rc == 1)
    {
        ppc_update_cr0(current_core, sum);
    }
    current_core->gpr[subix_rx] = sum;
}

void ppc_vle_opc_subfic(e500_core_t *current_core)
{
    uint8_t subficx_ra = ra(current_core->current_opc);
    uint8_t subficx_rd = rd(current_core->current_opc);
    uint8_t subficx_scl = scl(current_core->current_opc);
    uint8_t subficx_F = bit(current_core->current_opc, 21);
    uint8_t subficx_rc = bit(current_core->current_opc, 20);

    uint32_t imm = ppc_sci8_cac(current_core, subficx_F, subficx_scl);
    if (ppc_carry_3(~(current_core->gpr[subficx_ra]), imm, 1)) 
    {
        current_core->xer |= XER_CA;
    } else {
        current_core->xer &= ~XER_CA;
    }

    uint32_t sum = ~(current_core->gpr[subficx_ra]) + imm + 1;
    if(subficx_rc == 1)
    {
        ppc_update_cr0(current_core, sum);
    }
    current_core->gpr[subficx_rd] = sum;
}

void ppc_vle_opc_xori(e500_core_t *current_core)
{
    uint8_t xorix_rs = rs(current_core->current_opc);
    uint8_t xorix_ra = ra(current_core->current_opc);
    uint8_t xorix_rc = bit(current_core->current_opc, 20);
    uint8_t xorix_scl = scl(current_core->current_opc);
    uint8_t xorix_F = bit(current_core->current_opc, 21);
    uint32_t b = ppc_sci8_cac(current_core, xorix_F, xorix_scl);
    uint32_t result = current_core->gpr[xorix_rs] ^ b;

    if(xorix_rc == 1)
    {
        ppc_update_cr0(current_core, result);
    }
    current_core->gpr[xorix_ra] = result;
}
