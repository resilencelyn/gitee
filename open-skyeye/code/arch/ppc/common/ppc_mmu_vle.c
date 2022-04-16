
#include <math.h>
#include "ppc_cpu.h"
#include "ppc_exc.h"
#include "ppc_mmu.h"
#include "ppc_dec_vle.h"
#include "ppc_mmu_vle.h"

/*D-Mode*/
void ppc_vle_opc_lbz(e500_core_t *current_core)
{
    uint8_t lbz_ra = ra(current_core->current_opc);
    uint32_t lbz_d = LBZx_D(current_core->current_opc);
    uint32_t Val,a;
    int ret;

    if (lbz_ra == 0)
    {
        a = 0;
    } else
    {
        a = current_core->gpr[lbz_ra];
    }

    if(lbz_d & 0x8000)
    {
        lbz_d |= 0xFFFF0000;
    }

    ret = ppc_read_effective_byte(current_core, (a + lbz_d), (uint8_t *)&Val);
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[rd(current_core->current_opc)] = Val;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, (a + lbz_d));
    }
}

/*SD4-Mode*/
void ppc_vle_se_opc_lbz(e500_core_t *current_core)
{
    uint32_t lbz_sd4 = LBZx_SD4(current_core->current_opc);
    uint32_t a = current_core->gpr[rx(current_core->current_opc)];
    uint32_t Val;
    int ret;

    ret = ppc_read_effective_byte(current_core, (a + lbz_sd4), (uint8_t *)&Val);
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[rz(current_core->current_opc)] = Val;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, (a + lbz_sd4));
    }
}

/*D8-Mode*/
void ppc_vle_opc_lbzu(e500_core_t *current_core)
{
    uint8_t lbzu_ra = ra(current_core->current_opc);
    uint32_t lbzu_d8 = LBZx_D8(current_core->current_opc);
    uint8_t lbzu_rd = rd(current_core->current_opc);
    uint32_t Val,a;
    int ret;

    if ((lbzu_ra == 0) || (lbzu_rd == lbzu_ra))
    {
        printf("Func:%s, pc = 0x%x, opcode: 0x%x, insn excute invalid\n", __func__, current_core->pc, current_core->current_opc);
        return;
    } else
    {
        a = current_core->gpr[ra(current_core->current_opc)];
    }

    if(lbzu_d8 & 0x80)
    {
        lbzu_d8 |= 0xFFFFFF00;
    }

    ret = ppc_read_effective_byte(current_core, (a + lbzu_d8), (uint8_t *)&Val);
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[lbzu_rd] = Val;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, (a + lbzu_d8));
        return;
    }
    current_core->gpr[lbzu_ra] = (a + lbzu_d8);
}

void ppc_vle_opc_lha(e500_core_t *current_core)
{
    uint8_t lha_ra = ra(current_core->current_opc);
    uint8_t lha_rd = rd(current_core->current_opc);
    uint32_t lha_d = LBZx_D(current_core->current_opc);
    uint32_t a,Val;
    int ret;

    if(lha_ra == 0)
    {
        a = 0;
    } else
    {
        a = current_core->gpr[lha_ra];
    }

    if(lha_d & 0x8000)
    {
        lha_d |= 0xFFFF0000;
    }

    ret = ppc_read_effective_half(current_core, (a + lha_d), (uint16_t *)&Val);
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[lha_rd] = Val;
    }  else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, (a + lha_d));
        return;
    }
}

void ppc_vle_opc_lhau(e500_core_t *current_core)
{
    uint8_t lhau_ra = ra(current_core->current_opc);
    uint8_t lhau_rd = rd(current_core->current_opc);
    uint32_t lhau_d8 = LBZx_D8(current_core->current_opc);
    uint32_t a,Val;
    int ret;

    if( (lhau_ra == 0) || (lhau_rd == lhau_ra))
    {
        printf("Func:%s, pc = 0x%x, opcode: 0x%x, insn excute invalid\n", __func__, current_core->pc, current_core->current_opc);
        return;
    } else
    {
        a = current_core->gpr[ra(current_core->current_opc)];
    }

    if(lhau_d8 & 0x80)
    {
        lhau_d8 |= 0xFFFFFF00;
    }

    ret = ppc_read_effective_half(current_core, (a + lhau_d8), (uint16_t *)&Val);
    if (ret == PPC_MMU_OK)
    {
        if(Val & 0x8000)
        {
            Val |= 0xFFFF0000;
        }
        current_core->gpr[lhau_rd] = Val;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, (a + lhau_d8));
        return;
    }
    current_core->gpr[lhau_ra] = (a + lhau_d8);
}

void ppc_vle_opc_lhz(e500_core_t *current_core)
{
    uint8_t lhz_ra = ra(current_core->current_opc);
    uint8_t lhz_rd = rd(current_core->current_opc);
    uint32_t lhz_d = LBZx_D(current_core->current_opc);
    uint32_t a,Val;
    int ret;

    if(lhz_ra == 0)
    {
        a = 0;
    } else
    {
        a = current_core->gpr[lhz_ra];
    }

    if(lhz_d & 0x8000)
    {
        lhz_d |= 0xFFFF0000;
    }

    ret = ppc_read_effective_half(current_core, (a + lhz_d), (uint16_t *)&Val);
    if(ret == PPC_MMU_OK)
    {
        current_core->gpr[lhz_rd] = Val;
    }  else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, (a + lhz_d));
        return;
    }
}

void ppc_vle_opc_se_lhz(e500_core_t *current_core)
{
    uint8_t lhz_rx = rx(current_core->current_opc);
    uint8_t lhz_rz = rz(current_core->current_opc);
    uint32_t lhz_sd4 = LBZx_SD4(current_core->current_opc);
    uint32_t a,Val;
    int ret;

    a = current_core->gpr[lhz_rx];
    ret = ppc_read_effective_half(current_core, (a + (lhz_sd4 << 1)), (uint16_t *)&Val);
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[lhz_rz] = Val;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, (a + (lhz_sd4 << 1)));
        return;
    }
}

void ppc_vle_opc_lhzu(e500_core_t *current_core)
{
    uint32_t lhzu_d8 = LBZx_D8(current_core->current_opc);
    uint8_t lhzu_ra = ra(current_core->current_opc);
    uint8_t lhzu_rd = rd(current_core->current_opc);
    uint32_t a,Val;
    int ret;

    if( (lhzu_ra == 0) || (lhzu_ra == lhzu_rd))
    {
        printf("Func:%s, pc = 0x%x, opcode: 0x%x, insn excute invalid\n", __func__, current_core->pc, current_core->current_opc);
        return;
    } else
    {
        a = current_core->gpr[lhzu_ra];
    }

    if(lhzu_d8 & 0x80)
    {
        lhzu_d8 |= 0xFFFFFF00;
    }

    ret = ppc_read_effective_half(current_core, (a + lhzu_d8), (uint16_t *)&Val);
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[lhzu_rd] = Val;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, (a + lhzu_d8));
        return;
    }
    current_core->gpr[lhzu_ra] = (a + lhzu_d8);
}

void ppc_vle_opc_li(e500_core_t *current_core)
{
    uint32_t li20_0;
    uint32_t li20_1_8;
    uint32_t li20 = ((LI20_11_15(current_core->current_opc) << 11)
            | (LI20_17_20(current_core->current_opc) << 16)
            | (LI20_21_31(current_core->current_opc)));

    current_core->gpr[rd(current_core->current_opc)] = li20;
}

void ppc_vle_opc_lis(e500_core_t *current_core)
{
    uint32_t ui = (UI_11_15(current_core->current_opc) << 11)
        | UI_21_31(current_core->current_opc);
    uint32_t rt = rd(current_core->current_opc);

    current_core->gpr[rt] = ui << 16;
}

void ppc_vle_se_opc_li(e500_core_t *current_core)
{
    uint32_t li_ui7 = ui7(current_core->current_opc);

    current_core->gpr[rx(current_core->current_opc)] = li_ui7;
}

void ppc_vle_opc_lmw(e500_core_t *current_core)
{
    uint8_t lmw_ra = ra(current_core->current_opc);
    uint8_t lmw_rd = rd(current_core->current_opc);
    uint32_t lmw_d8 = LBZx_D8(current_core->current_opc);
    uint32_t EA;
    uint8_t r;

    if (lmw_d8 & 0x80)
    {
        lmw_d8 |= 0xFFFFFF00;
    }

    if (lmw_ra == 0)
    {
        EA = lmw_d8;
    } else
    {
        EA = current_core->gpr[lmw_ra] + lmw_d8;
    }

    r = lmw_rd;
    while(r <= 31)
    {
        if (ppc_read_effective_word(current_core, EA, &(current_core->gpr[r])))
        {
            printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, EA);
            return;
        }
        r++;
        EA += 4;
    };
}

void ppc_vle_opc_lwz(e500_core_t *current_core)
{
    uint8_t lwz_ra = ra(current_core->current_opc);
    uint8_t lwz_rd = rd(current_core->current_opc);
    uint32_t lwz_d = LBZx_D(current_core->current_opc);
    uint32_t a;

    if (lwz_ra == 0)
    {
        a = 0;
    } else
    {
        a = current_core->gpr[lwz_ra];
    }

    if (lwz_d & 0x8000)
    {
        lwz_d |= 0xFFFF0000;
    }

    uint32_t EA = a + lwz_d;
    if(ppc_read_effective_word(current_core, EA, &(current_core->gpr[lwz_rd])))
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, EA);
        return;
    }
}

void ppc_vle_se_opc_lwz(e500_core_t *current_core)
{
    uint8_t lwz_rz = rz(current_core->current_opc);
    uint8_t lwz_rx = rx(current_core->current_opc);
    uint32_t lwz_sd4 = LBZx_SD4(current_core->current_opc);
    uint32_t a = current_core->gpr[lwz_rx];
    uint32_t EA;

    EA = a + (lwz_sd4 << 2);

    if(ppc_read_effective_word(current_core, EA, &(current_core->gpr[lwz_rz])))
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, EA);
        return;
    }
}

void ppc_vle_opc_lwzu(e500_core_t *current_core)
{
    uint8_t lwzu_ra = ra(current_core->current_opc);
    uint8_t lwzu_rd = rd(current_core->current_opc);
    uint32_t lwzu_d8 = LBZx_D8(current_core->current_opc);
    uint32_t a,EA;

    if ((lwzu_ra == 0) || (lwzu_rd == lwzu_ra))
    {
        printf("Func:%s, pc = 0x%x, opcode: 0x%x, insn excute invalid\n", __func__, current_core->pc, current_core->current_opc);
        return;
    } else
    {
        a = current_core->gpr[lwzu_ra];
    }

    if (lwzu_d8 & 0x80)
    {
        lwzu_d8 |= 0xFFFFFF00;
    }

    EA = a + lwzu_d8;
    if (ppc_read_effective_word(current_core, EA, &(current_core->gpr[lwzu_rd])))
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc read effective address content error\n", __func__, EA);
        return;
    }
    current_core->gpr[lwzu_ra] = EA;
}

void ppc_vle_opc_stb(e500_core_t *current_core)
{
    uint8_t stb_rs = rs(current_core->current_opc);
    uint8_t stb_ra = ra(current_core->current_opc);
    uint32_t stb_d = LBZx_D(current_core->current_opc);
    uint32_t a,EA;
    int ret;

    if (stb_ra == 0)
    {
        a = 0;
    } else
    {
        a = current_core->gpr[stb_ra];
    }

    if (stb_d & 0x8000)
    {
        stb_d |= 0xFFFF0000;
    }

    EA = a + stb_d;
    ret = ppc_write_effective_byte(current_core, EA, (uint8_t)(current_core->gpr[stb_ra] & 0xFF));
    if(ret != PPC_MMU_OK)
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}

/*se instruction*/
void ppc_vle_se_opc_stb(e500_core_t *current_core)
{
    uint8_t stb_rz = rz(current_core->current_opc);
    uint8_t stb_rx = rx(current_core->current_opc);
    uint32_t stb_sd4 = LBZx_SD4(current_core->current_opc);
    uint32_t a, EA;
    int ret;

    if(stb_sd4 & 0x8)
    {
        stb_sd4 |= 0xFFFFFFF0;
    }

    a = current_core->gpr[stb_rx];
    EA = (a + stb_sd4);
    ret = ppc_write_effective_byte(current_core, EA, (uint8_t)(current_core->gpr[stb_rz] & 0xFF));
    if(ret != PPC_MMU_OK)
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}

void ppc_vle_opc_stbu(e500_core_t *current_core)
{
    uint8_t stbu_rs = rs(current_core->current_opc);
    uint8_t stbu_ra = ra(current_core->current_opc);
    uint32_t stbu_d8 = LBZx_D8(current_core->current_opc);
    uint32_t a, EA;
    int ret;

    a = current_core->gpr[stbu_ra];

    if (stbu_d8 & 0x80)
    {
        stbu_d8 |= 0xFFFFFF00;
    }

    EA = a + stbu_d8;
    ret = ppc_write_effective_byte(current_core, EA, (uint8_t)(current_core->gpr[stbu_rs] & 0xFF));
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[stbu_ra] = EA;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}

void ppc_vle_opc_sth(e500_core_t *current_core)
{
    uint8_t sth_rs = rs(current_core->current_opc);
    uint8_t sth_ra = ra(current_core->current_opc);
    uint32_t sth_d = LBZx_D(current_core->current_opc);
    uint32_t a,EA;
    int ret;

    if (sth_ra == 0)
    {
        a = 0;
    } else
    {
        a = current_core->gpr[sth_ra];
    }

    if(sth_d & 0x8000)
    {
        sth_d |= 0xFFFF0000;
    }

    EA = a + sth_d;
    ret = ppc_write_effective_half(current_core, EA, (uint16_t)(current_core->gpr[sth_ra] & 0xFFFF));
    if(ret != PPC_MMU_OK)
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}

/*se instruction*/
void ppc_vle_se_opc_sth(e500_core_t *current_core)
{
    uint8_t sth_rz = rz(current_core->current_opc);
    uint8_t sth_rx = rx(current_core->current_opc);
    uint32_t sth_sd4 = LBZx_SD4(current_core->current_opc);
    uint32_t a,EA;
    int ret;

    /*SD4-Mode*/
    a = current_core->gpr[sth_rx];
    EA = (a + (sth_sd4 << 1));
    ret = ppc_write_effective_half(current_core, EA, (uint16_t)(current_core->gpr[sth_rz] & 0xFFFF));
    if(ret != PPC_MMU_OK)
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}

void ppc_vle_opc_sthu(e500_core_t *current_core)
{
    uint8_t sthu_rs = rs(current_core->current_opc);
    uint8_t sthu_ra = ra(current_core->current_opc);
    uint32_t sthu_d8 = LBZx_D8(current_core->current_opc);
    uint32_t a,EA;
    int ret;

    if(sthu_ra == 0)
    {
        printf("Func:%s, pc = 0x%x, opcode: 0x%x, insn excute invalid\n", __func__, current_core->pc, current_core->current_opc);
        return;
    }

    if(sthu_d8 & 0x80)
    {
        sthu_d8 |= 0xFFFFFF00;
    }

    /*D8-Mode*/
    a = current_core->gpr[sthu_ra];
    EA = a + sthu_d8;
    ret = ppc_write_effective_half(current_core, EA, (uint16_t)(current_core->gpr[sthu_rs] & 0xFFFF));
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[sthu_ra] = EA;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}

void ppc_vle_opc_stmw(e500_core_t *current_core)
{
    uint8_t stmw_ra = ra(current_core->current_opc);
    uint8_t stmw_rs = rs(current_core->current_opc);
    uint32_t stmw_d8 = LBZx_D8(current_core->current_opc);
    uint32_t EA;
    uint8_t r;

    if(stmw_d8 & 0x80)
    {
        stmw_d8 |= 0xFFFFFF00;
    }

    if (stmw_ra == 0)
    {
        EA = stmw_d8;
    } else
    {
        EA = current_core->gpr[stmw_ra] + stmw_d8;
    }

    r = stmw_rs;
    while (r <= 31)
    {
        if (ppc_write_effective_word(current_core, EA, current_core->gpr[r]))
        {
            printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
            return;
        }
        r++;
        EA += 4;
    }
}

void ppc_vle_opc_stw(e500_core_t *current_core)
{
    uint8_t stw_rs = rs(current_core->current_opc);
    uint8_t stw_ra = ra(current_core->current_opc);
    uint32_t stw_d = LBZx_D(current_core->current_opc);
    uint32_t EA,a;
    int ret;

    if (stw_ra == 0)
    {
        a = 0;
    } else
    {
        a = current_core->gpr[stw_ra];
    }

    if(stw_d & 0x8000)
    {
        stw_d |= 0xFFFF0000;
    }

    EA = a + stw_d;
    ret = ppc_write_effective_word(current_core, EA, current_core->gpr[stw_rs]);
    if(ret != PPC_MMU_OK)
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}

/*se instruction*/
void ppc_vle_se_opc_stw(e500_core_t *current_core)
{
    uint8_t stw_rz = rz(current_core->current_opc);
    uint8_t stw_rx = rx(current_core->current_opc);
    uint32_t stw_sd4 = LBZx_SD4(current_core->current_opc);
    uint32_t a = current_core->gpr[stw_rx];
    uint32_t EA = (a + (stw_sd4 << 2));
    int ret;

    ret = ppc_write_effective_word(current_core, EA, current_core->gpr[stw_rz]);
    if(ret != PPC_MMU_OK)
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}

void ppc_vle_opc_stwu(e500_core_t *current_core)
{
    uint8_t stwu_rs = rs(current_core->current_opc);
    uint8_t stwu_ra = ra(current_core->current_opc);
    uint32_t stwu_d8 = LBZx_D8(current_core->current_opc);
    uint32_t EA, a;
    int ret;

    if(stwu_ra == 0)
    {
        printf("Func:%s, pc = 0x%x, opcode: 0x%x, insn excute invalid\n", __func__, current_core->pc, current_core->current_opc);
        return;
    }

    /*D8-Mode*/
    a = current_core->gpr[stwu_ra];
    if(stwu_d8 & 0x80)
    {
        stwu_d8 = 0xFFFFFF00 | stwu_d8;
    }

    EA = a + stwu_d8;
    ret = ppc_write_effective_word(current_core, EA, current_core->gpr[stwu_rs]);
    if (ret == PPC_MMU_OK)
    {
        current_core->gpr[stwu_ra] = EA;
    } else
    {
        printf("in func: %s, EffectAddr: 0x%x, ppc write effective address content error\n", __func__, EA);
    }
}
