#include <math.h>
#include "ppc_cpu.h"
#include "ppc_exc.h"
#include "ppc_opc_vle.h"
#include "ppc_dec_vle.h"

extern void ppc_set_msr(e500_core_t *current_core, uint32 newmsr);

void ppc_vle_opc_b(e500_core_t *current_core)
{
    uint32_t sign_ex_bd24;
    uint32_t nxt_pc;
    uint32_t a;
    uint32_t SignJudgeBit;

    sign_ex_bd24 = bd24(current_core->current_opc);
    SignJudgeBit = sign_ex_bd24 & 0x800000;

    if (SignJudgeBit)
    {
        sign_ex_bd24 = sign_ex_bd24 << 1;
        sign_ex_bd24 |= 0xFF000000;
    } else
    {
        sign_ex_bd24 = sign_ex_bd24 << 1;
    }

    a = current_core->pc;
    nxt_pc = a + sign_ex_bd24;
    current_core->npc = nxt_pc;

    if (bit(current_core->current_opc, 31) == 1)
    {
        current_core->lr = current_core->pc + 4;
    }
}

void ppc_vle_se_opc_b(e500_core_t *current_core)
{
    uint32_t nxt_ia;
    uint32_t ext_bd8;

    ext_bd8 = bd8(current_core->current_opc) << 1;
    if (ext_bd8 & 0x100)
    {
        ext_bd8 |= 0xFFFFFF00;
    }

    nxt_ia = current_core->pc + ext_bd8;
    current_core->npc = nxt_ia;

    if (bit(current_core->current_opc, 7))
    {
        current_core->lr = current_core->pc + 2;
    }
}

void ppc_vle_opc_bc(e500_core_t *current_core)
{
    uint32_t nxt_pc;
    uint32_t ext_bd15;
    bool_t ctr_ok,cond_ok;
    uint32_t cr_bi32_mask = 1 << (31 - bi32(current_core->current_opc));
    bool_t bo32_0 = ((bo32(current_core->current_opc) & 2) ? 1 : 0);
    bool_t bo32_1 = ((bo32(current_core->current_opc) & 1) ? 1 : 0);
    bool_t cr_bi32 = ((current_core->cr & cr_bi32_mask) ? 1: 0);

    if(bo32_0)
    {
        if (current_core->ctr != 0)
        {
            current_core->ctr -= 1;
        }
    }

    ctr_ok = ((!bo32_0) | ((current_core->ctr != 0) ^ bo32_1));
    cond_ok = (bo32_0 | (cr_bi32 ^ (!bo32_1)));

    ext_bd15 = (bd15(current_core->current_opc) << 1);
    if (ext_bd15 & 0x8000)
    {
        ext_bd15 |= 0xFFFF0000;
    }

    if (ctr_ok & cond_ok)
    {
        nxt_pc = (current_core->pc + ext_bd15);
    } else
    {
        nxt_pc = current_core->pc + 4;
    }
    current_core->npc = nxt_pc;

    if (bit(current_core->current_opc, 31) == 1)
    {
        current_core->lr = current_core->pc + 4;
    }
}

void ppc_vle_se_opc_bc(e500_core_t *current_core)
{
    bool_t cond_ok;
    uint32_t nxt_ia;
    uint32_t BD8;

    cond_ok = (((current_core->cr >> (31 - bi16(current_core->current_opc))) & 0x1) ? 1: 0);
    cond_ok = cond_ok ^ (! bo16(current_core->current_opc));

    if(cond_ok)
    {

        BD8 = (uint32_t)bd8(current_core->current_opc) << 1;

        if(BD8 & 0x100)
        {
            BD8 |= 0xFFFFFF00;
        }
        nxt_ia = current_core->pc
            +BD8;
    } else
    {
        nxt_ia = current_core->pc + 2;
    }
    current_core->npc = nxt_ia;
}

void ppc_vle_se_opc_bclri(e500_core_t *current_core)
{
    uint32_t a = ui5(current_core->current_opc);
    uint32_t b = ~(1 << (31 - a));
    uint32_t result = current_core->gpr[rx(current_core->current_opc)] & b;

    current_core->gpr[rx(current_core->current_opc)] = result;
}

void ppc_vle_se_opc_bctr(e500_core_t *current_core)
{
    uint32_t nxt_ia = current_core->ctr & 0xFFFFFFFE;

    current_core->npc = nxt_ia;
    if(bit(current_core->current_opc, 15))
    {
        current_core->lr = current_core->pc + 2;
    }
}

void ppc_vle_se_opc_bgeni(e500_core_t *current_core)
{
    uint32_t a = ui5(current_core->current_opc);
    uint32_t b = (1 << (31 - a));

    current_core->gpr[rx(current_core->current_opc)] = b;
}

void ppc_vle_se_opc_blr(e500_core_t *current_core)
{
    uint32_t nxt_ia = (current_core->lr) & 0xFFFFFFFE;

    current_core->npc = nxt_ia;
    if(bit(current_core->current_opc, 15))
    {
        current_core->lr = current_core->pc + 2;
    }
}

void ppc_vle_se_opc_bmaski(e500_core_t *current_core)
{
    uint32_t a = ui5(current_core->current_opc);
    uint32_t b;

    if (a == 0)
    {
        b = 0xFFFFFFFF;
    } else
    {
        b = (int)(pow(2, a)) - 1;
    }

    current_core->gpr[rx(current_core->current_opc)] = b;
}

void ppc_vle_se_opc_bseti(e500_core_t *current_core)
{
    uint32_t a = ui5(current_core->current_opc);
    uint32_t b = (1 << (31 - a));
    uint32_t result = current_core->gpr[rx(current_core->current_opc)] | b;

    current_core->gpr[rx(current_core->current_opc)] = result;
}

void ppc_vle_se_opc_btsti(e500_core_t *current_core)
{
    uint32_t a = ui5(current_core->current_opc);
    uint32_t b = 1 << (31 - a);
    uint32_t c = current_core->gpr[rx(current_core->current_opc)] & b;
    uint32_t d = ((c == 0) ? 1 : 2);

    d = d << 1;
    if (current_core->xer & XER_SO)
    {
        d |= 1;
    }

    current_core->cr &= 0x0FFFFFFF;
    current_core->cr |= (d << 28);
}

void ppc_vle_opc_mcrf(e500_core_t *current_core)
{
    uint8_t bf = _crd(current_core->current_opc);
    uint8_t bfa = _crs(current_core->current_opc);
    uint32_t c = (current_core->cr >> (28 - bfa * 4)) & 0xF;

    current_core->cr &= ~(0xF << (28 - bf * 4));
    current_core->cr |= (c << (28 - bf * 4));
}

void ppc_vle_se_opc_mfar(e500_core_t *current_core)
{
    uint8_t mfar_rx = rx(current_core->current_opc);
    uint8_t ary = _ary(current_core->current_opc);

    current_core->gpr[mfar_rx] = current_core->gpr[ary + 8];
}

void ppc_vle_se_opc_mfctr(e500_core_t *current_core)
{
    uint8_t mfctr_rx = rx(current_core->current_opc);

    current_core->gpr[mfctr_rx] = current_core->ctr;
}

void ppc_vle_se_opc_mflr(e500_core_t *current_core)
{
    uint8_t mflr_rx = rx(current_core->current_opc);

    current_core->gpr[mflr_rx] = current_core->lr;
}

void ppc_vle_se_opc_mr(e500_core_t *current_core)
{
    uint8_t mr_rx = rx(current_core->current_opc);
    uint8_t mr_ry = ry(current_core->current_opc);

    current_core->gpr[mr_rx] = current_core->gpr[mr_ry];
}

void ppc_vle_se_opc_mtar(e500_core_t *current_core)
{
    uint8_t mtar_ry = ry(current_core->current_opc);
    uint8_t arx = _arx(current_core->current_opc);

    current_core->gpr[arx + 8] = current_core->gpr[mtar_ry];
}

void ppc_vle_se_opc_mtctr(e500_core_t *current_core)
{
    uint8_t mtctr_rx = rx(current_core->current_opc);

    current_core->ctr = current_core->gpr[mtctr_rx];

}

void ppc_vle_se_opc_mtlr(e500_core_t *current_core)
{
    uint8_t mtlr_rx = rx(current_core->current_opc);

    current_core->lr = current_core->gpr[mtlr_rx];
}

void ppc_vle_se_opc_rfci(e500_core_t *current_core)
{
    if (current_core->msr & MSR_PR)
    {
        ppc_exception(current_core, PPC_EXC_PROGRAM,
                PPC_EXC_PROGRAM_PRIV, 0);
        return;
    }

    ppc_set_msr(current_core, current_core->csrr[1]);
    current_core->npc = current_core->csrr[0] << 2;
}

void ppc_vle_se_opc_rfdi(e500_core_t *current_core)
{
    current_core->msr = current_core->dsrr[1];
    current_core->npc = current_core->dsrr[0] & 0xfffffffD;
};

void ppc_vle_se_opc_rfgi(e500_core_t *current_core)
{
    uint32_t newmsr;
    uint32_t ports;
    uint32_t msr_gs;
    uint32_t msr_we;

    newmsr = current_core->gsrr[1];

    if(current_core->msr & msr_gs)
    {
        msr_we = current_core->msr & MSR_WE;
        msr_gs = current_core->msr & MSR_GS;

        newmsr |= msr_gs;

        if (msr_we)
        {
            newmsr |= msr_we;
        } else
        {
            newmsr &= ~msr_we;
        }
        ports = 0;
        ports = current_core->msrp;

        newmsr = (ports & current_core->msr) | ((~ports) & newmsr);
    }

    current_core->msr = newmsr;
    current_core->npc = current_core->gsrr[0] & 0xfffffffe;
};

void ppc_vle_se_opc_rfi(e500_core_t *current_core)
{
    ppc_set_msr(current_core, current_core->srr[1]);
    current_core->npc = current_core->srr[0];
}

void ppc_vle_se_opc_rfmci(e500_core_t *current_core)
{
    current_core->msr = current_core->mcsrr[1];
    current_core->npc = current_core->mcsrr[0] & 0xfffffffD;
};

void ppc_vle_opc_sc(e500_core_t *current_core)
{
    current_core->srr[1] = current_core->msr;
    current_core->srr[0] = current_core->pc + 2;

    current_core->npc = ((current_core->ivpr & 0xFFFF0000)
            |(current_core->ivor[8] & 0xFFF0));
    clear_msr_special_bits(current_core);
}

void ppc_vle_se_opc_sc(e500_core_t *current_core)
{
    current_core->srr[1] = current_core->msr;
    current_core->srr[0] = current_core->pc + 2;

    current_core->npc = ((current_core->ivpr & 0xFFFF0000)
            |(current_core->ivor[8] & 0xFFF0));
    clear_msr_special_bits(current_core);

};




