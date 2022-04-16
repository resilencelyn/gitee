
/*************************************************************************
  > File Name    : armv8_exception.c
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/8/13 10:11:40
 ************************************************************************/
#include "armdefs.h"
#include "arch64_memory.h"
#include "arch64_exception.h"
#include "arch64_decode.h"

void addPACGA(ARMul_State * state, ARMdword operand1, ARMdword operand2)
{
    bit128 APGAKey_EL1;
    ARMword TrapEL2, TrapEL3;
    ARMword currentEL = state->PSTATE.EL;

    APGAKey_EL1.lo64 = state->system_reg.APGAKeyLo_EL1;
    APGAKey_EL1.hi64 = state->system_reg.APGAKeyHi_EL1;

    switch (currentEL)
    {
        case EL0:
            TrapEL2 = EL2Enabled(state) && state->system_reg.HCR_EL2.API == 0 && (state->system_reg.HCR_EL2.TGE == 0
                                                                                  || state->system_reg.HCR_EL2.E2H ==
                                                                                  0);
            TrapEL3 = state->system_reg.SCR_EL3.API == 0 && HaveEL(EL3);
            break;
        case EL1:
            TrapEL2 = EL2Enabled(state) && state->system_reg.HCR_EL2.API == 0;
            TrapEL3 = HaveEL(EL3) && state->system_reg.SCR_EL3.API == 0;
            break;
        case EL2:
            TrapEL2 = 0;
            TrapEL3 = HaveEL(EL3) && state->system_reg.SCR_EL3.API == 0;
            break;
        case EL3:
            TrapEL2 = 0;
            TrapEL3 = 0;
            break;
        default:
            break;
    }

    if (TrapEL2)
        TrapPACUse(state, EL2);
    else if (TrapEL3)
        TrapPACUse(state, EL3);
}

void TrapPACUse(ARMul_State * state, int target_el)
{
    if (HaveEL(target_el) && target_el != EL0 && target_el >= state->PSTATE.EL)
    {
        ARMdword exception_return = state->A64_PC;
        ARMword vect_offset = 0;
        ExceptionRecord exception;

        exception = ExceptionSyndrome(EC_PACTRAP);
        TakeException(state, target_el, exception, exception_return, vect_offset);

    }
}

static ExceptionRecord ExceptionSyndrome(Exception exceptype)
{
    ExceptionRecord r;

    r.exceptype = exceptype;
    r.syndrome = 0;
    r.vaddress = 0;
    r.ipavalid = False;
    r.NS = 0;
    r.ipaddress = 0;

    return r;
}

void systemAccessTrap(ARMul_State * state, int target_el, int ec, ARMdword return_pc)
{
    if (HaveEL(target_el) && target_el != EL0 && target_el >= state->PSTATE.EL)
    {
        ARMdword exception_return = return_pc;
        ARMword vect_offset = 0;
        ExceptionRecord exception;

        exception = ExceptionSyndrome(ec);
        TakeException(state, target_el, exception, exception_return, vect_offset);

    }
}
static void SynchronizeContext(ARMul_State * state)
{

}

static bool_t IsInHost(ARMul_State * state)
{
    return (IsSecureEL2Enabled(state) || !IsSecureBelowEL3())
        && !ELUSingAArch32(EL2) && state->system_reg.HCR_EL2.E2H == 1
        && (state->PSTATE.EL == EL2 || (state->PSTATE.EL == EL0 && state->system_reg.HCR_EL2.TGE == 1));
}

static void ReportException(ARMul_State * state, ExceptionRecord exception, int target_el)
{

}

void setSPSR(ARMul_State * state, ARMword spsr)
{
    if (UsingAArch32(state))
    {
        switch (state->PSTATE.M)
        {
            case M32_FIQ:
                state->Spsr[FIQBANK] = spsr;
                break;
            case M32_IRQ:
                state->Spsr[IRQBANK] = spsr;
                break;
            case M32_Svc:
                state->Spsr[SVCBANK] = spsr;
                break;
            case M32_Monitor:
                state->Spsr[DUMMYBANK] = spsr;
                break;
            case M32_Abort:
                state->Spsr[ABORTBANK] = spsr;
                break;
            case M32_Undef:
                state->Spsr[UNDEFBANK] = spsr;
                break;
            default:
                NOT_IMPLEMENT;
                break;
        }
    } else
    {
        if (state->PSTATE.EL == EL1)
            state->system_reg.SPSR_EL1.all = spsr;
        else if (state->PSTATE.EL == EL2)
            state->system_reg.A64_SPSR_EL2.all = spsr;
        else if (state->PSTATE.EL == EL3)
            state->system_reg.A64_SPSR_EL3.all = spsr;
        else
        {
            NOT_IMPLEMENT;
        }
    }
}

ARMword getSPSR(ARMul_State * state)
{
    ARMword result;

    if (UsingAArch32(state))
    {
        switch (state->PSTATE.M)
        {
            case M32_FIQ:
                result = state->Spsr[FIQBANK];
                break;
            case M32_IRQ:
                result = state->Spsr[IRQBANK];
                break;
            case M32_Svc:
                result = state->Spsr[SVCBANK];
                break;
            case M32_Monitor:
                result = state->Spsr[DUMMYBANK];
                break;
            case M32_Abort:
                result = state->Spsr[ABORTBANK];
                break;
            case M32_Undef:
                result = state->Spsr[UNDEFBANK];
                break;
            default:
                NOT_IMPLEMENT;
                break;
        }
    } else
    {
        if (state->PSTATE.EL == EL1)
            result = (ARMword) state->system_reg.SPSR_EL1.all;
        else if (state->PSTATE.EL == EL2)
            result = (ARMword) state->system_reg.A64_SPSR_EL2.all;
        else if (state->PSTATE.EL == EL3)
            result = (ARMword) state->system_reg.A64_SPSR_EL3.all;
        else
        {
            NOT_IMPLEMENT;
        }
    }

    return result;
}

void setELR(ARMul_State * state, int target_el, ARMword value)
{
    if (target_el == EL1)
        state->system_reg.ELR_EL1 = value;
    else if (target_el == EL2)
        state->system_reg.ELR_EL2 = value;
    else if (target_el == EL3)
        state->system_reg.ELR_EL3 = value;
    else
    {
        NOT_IMPLEMENT;
    }
}

ARMdword getELR(ARMul_State * state, int target_el)
{
    if (target_el == EL1)
        return state->system_reg.ELR_EL1;
    else if (target_el == EL2)
        return state->system_reg.ELR_EL2;
    else if (target_el == EL3)
        return state->system_reg.ELR_EL3;
    else
    {
        NOT_IMPLEMENT;
    }
}

static ARMdword getVBAR(ARMul_State * state, int target_el)
{
    ARMdword vbar;

    if (target_el == EL1)
        vbar = state->system_reg.VBAR_EL1;
    else if (target_el == EL2)
        vbar = state->system_reg.VBAR_EL2;
    else if (target_el == EL3)
        vbar = state->system_reg.VBAR_EL3;
    else
    {
        NOT_IMPLEMENT;
    }

    return vbar;
}

static TakeException(ARMul_State * state, int target_el,
                     ExceptionRecord exception, ARMdword exception_return, ARMword vect_offset)
{
    SynchronizeContext(state);
    //If coming from AArch32 state, the top parts of the X[] registers might be set to zero
    if (UsingAArch32(state))
    {
        int i;

        for (i = 0; i < 31; i++)
        {
            WRITE_X_HIGH_REG(i, 0);
            WRITE_Vdh_REG(i, 0);
        }
    }

    if (target_el > state->PSTATE.EL)
    {
        bool_t lower_32;

        if (target_el == EL3)
        {
            if (EL2Enabled(state))
                lower_32 = ELUSingAArch32(EL2);
            else
                lower_32 = ELUSingAArch32(EL1);
        } else if (IsInHost(state) && state->PSTATE.EL == EL0 && target_el == EL2)
        {
            lower_32 = ELUSingAArch32(EL0);
        } else
            lower_32 = ELUSingAArch32(target_el - 1);

        vect_offset += lower_32 ? 0x600 : 0x400;
    } else if (state->PSTATE.SP == 1)
        vect_offset += 0x200;

    ARMword spsr = GetPSRFromPSTATE(state);

    if (state->PSTATE.EL == EL1 && target_el == EL1
        && EL2Enabled(state) && state->system_reg.HCR_EL2.NV0 == 0 && state->system_reg.HCR_EL2.NV1 == 1)
    {
        //spsr<3:2> = '10'
        spsr &= ~0xc;
        spsr |= 0x8;
    }

    if (exception.exceptype < Exception_IRQ)
    {
        ReportException(state, exception, target_el);
    }

    state->PSTATE.EL = target_el;
    state->PSTATE.nRW = 0;
    state->PSTATE.SP = 1;
    setSPSR(state, spsr);
    setELR(state, target_el, exception_return);
    state->PSTATE.SS = 0;
    state->PSTATE.D = 1;
    state->PSTATE.A = 1;
    state->PSTATE.I = 1;
    state->PSTATE.F = 1;
    state->PSTATE.IL = 0;

    if (UsingAArch32(state))
    {
        state->PSTATE.IT = 0;
        state->PSTATE.T = 0;
    }
    state->PSTATE.UAO = 0;
    state->PSTATE.BTYPE = 0;
    state->PSTATE.SSBS = 0;
    state->PSTATE.TCO = 1;
    ARMdword vbar = getVBAR(state, target_el);

    //branch_addr = vbar<63:11> | vect_offset<0:10>
    ARMdword branch_addr = ((vbar >> 11) << 11) | (vect_offset & 0x7ff);

    state->A64_PC = branch_addr;
}

ARMword GetPSRFromPSTATE(ARMul_State * state)
{
    ARMword spsr = 0;

    spsr |= (state->PSTATE.all & 0xf) << 28;
    spsr |= (state->PSTATE.PAN << 22);
    spsr |= (state->PSTATE.IL << 20);
    if (state->PSTATE.nRW == 1)
    {
        //AArch32 state
        spsr |= (state->PSTATE.Q << 27);
        spsr |= ((state->PSTATE.IT & 0x3) << 25);
        spsr |= (state->PSTATE.SSBS << 23);
        spsr |= (state->PSTATE.DIT << 21);
        spsr |= (state->PSTATE.Ge << 16);
        spsr |= ((state->PSTATE.IT >> 2) << 10);
        spsr |= (state->PSTATE.E << 9);
        spsr |= (state->PSTATE.A << 6);
        spsr |= (state->PSTATE.I << 7);
        spsr |= (state->PSTATE.F << 8);
        spsr |= (state->PSTATE.T << 5);
        assert((state->PSTATE.M >> 4) == state->PSTATE.nRW);
        spsr |= (state->PSTATE.M);
    } else
    {
        //AArch64 state
        spsr |= state->PSTATE.SP;
        spsr |= state->PSTATE.EL << 2;
        spsr |= state->PSTATE.nRW << 4;
        spsr |= ((state->PSTATE.all >> 4) & 0xf) << 6;  //D A I F
        spsr |= state->PSTATE.BTYPE << 10;
        spsr |= state->PSTATE.SSBS << 12;
        spsr |= state->PSTATE.SS << 21;
        spsr |= state->PSTATE.UAO << 23;
        spsr |= state->PSTATE.DIT << 24;
        spsr |= state->PSTATE.TCO << 25;
    }

    return spsr;
}

bool_t Restarting(ARMul_State * state)
{
    return state->debug_reg.EDSCR.STATUS == 1;
}

bool_t illegalExceptionReturn(ARMul_State * state, ARMword spsr)
{
    bool_t ret = False;

    return ret;
}

bool_t GenerateDebugExceptions(ARMul_State * state)
{
    bool_t ret = False;

    return ret;
}

ARMword ELFromSPSR(ARMword spsr)
{

}

bool_t AArch32_GenerateDebugExceptionsFrom(ARMul_State * state, ARMword dest, bool_t secure)
{

}

bool_t AArch64_GenerateDebugExceptionsFrom(ARMul_State * state, ARMword dest, bool_t secure, ARMword mask)
{

}

ARMword DebugTargetFrom(ARMul_State * state, bool_t secure)
{

}

bool_t DebugExceptionReturnSS(ARMul_State * state, ARMword spsr)
{
    bool_t ss_bit = False;
    bool_t enabled_at_source, enabled_at_dest, secure;
    ARMword dest, mask, ELd;

    if (state->system_reg.MDSCR_EL1.SS)
    {
        if (Restarting(state))
        {
            enabled_at_source = False;
        } else if (UsingAArch32(state))
        {
            enabled_at_source = GenerateDebugExceptions(state);
        } else
            enabled_at_source = GenerateDebugExceptions(state);

        if (illegalExceptionReturn(state, spsr))
        {
            dest = state->PSTATE.EL;
        } else
        {
            dest = ELFromSPSR(spsr);
        }

        secure = IsSecureBelowEL3() || dest == EL3;

        if (ELUSingAArch32(dest))
        {
            enabled_at_dest = AArch32_GenerateDebugExceptionsFrom(state, dest, secure);
        } else
        {
            mask = Bits(spsr, 9, 9);
            enabled_at_dest = AArch64_GenerateDebugExceptionsFrom(state, dest, secure, mask);
        }

        ELd = DebugTargetFrom(state, secure);
        if (!ELUSingAArch32(ELd) && !enabled_at_source && enabled_at_dest)
            ss_bit = Bits(spsr, 21, 21);
    }

    return ss_bit;
}

void WriteMode(ARMul_State * state, ARMword mode)
{
    ARMword el, sp = 1;

    switch (mode)
    {
        case M32_Monitor:
            el = EL3;
            break;
        case M32_Hyp:
            el = EL2;
            break;
        case M32_FIQ:
        case M32_IRQ:
        case M32_Svc:
        case M32_Abort:
        case M32_Undef:
        case M32_System:
            if (HaveEL(EL3) && HighestELUsingAArch32() && state->system_reg.SCR_EL3.NS == 0)
                el = EL3;
            else
                el = EL1;

            if (mode == M32_System)
                sp = 0;
        case M32_User:
            el = EL0;
            sp = 0;
            break;
        default:
            NOT_IMPLEMENT;
            break;
    }

    state->PSTATE.M = mode;
    state->PSTATE.EL = el;
    state->PSTATE.nRW = 1;
    state->PSTATE.SP = sp;

    return;
}

ARMword RestoredITBits(ARMul_State * state, ARMword spsr)
{
    ARMword it = Bits(spsr, 25, 26) | (Bits(spsr, 10, 15) << 2);

    if (state->PSTATE.IL == 1)
        return 0;

    if (Bits(it, 0, 3) == 0 && !Bits(it, 4, 7))
        return 0;

    ARMword itd = (state->PSTATE.EL == EL2) ? state->system_reg.SCTLR_EL2.ITD : state->system_reg.SCTLR_EL1.ITD;

    if (Bits(spsr, 5, 5) == 0 && it != 0 || (itd == 1 && Bits(it, 0, 2)))
        return 0;
    else
        return it;
}

ARMword setPSTATEFromPSR(ARMul_State * state, ARMword spsr)
{
    state->PSTATE.SS = DebugExceptionReturnSS(state, spsr);
    if (illegalExceptionReturn(state, spsr))
    {
        state->PSTATE.IL = 1;
    } else
    {
        state->PSTATE.IL = Bits(spsr, 20, 20);
        if (Bits(spsr, 4, 4))
        {
            //AArch32 state
            WriteMode(state, Bits(spsr, 0, 4));
            state->PSTATE.SSBS = Bits(spsr, 23, 23);
        } else
        {
            state->PSTATE.nRW = 0;
            state->PSTATE.EL = Bits(spsr, 2, 3);
            state->PSTATE.SP = Bits(spsr, 0, 0);
            state->PSTATE.SSBS = Bits(spsr, 12, 12);
        }
    }

    if (state->PSTATE.IL && state->PSTATE.nRW)
        spsr &= ~0x20;                  //spsr<5> = 0

    state->PSTATE.N = Bits(spsr, 28, 28);
    state->PSTATE.Z = Bits(spsr, 29, 29);
    state->PSTATE.C = Bits(spsr, 30, 30);
    state->PSTATE.V = Bits(spsr, 31, 31);

    state->PSTATE.PAN = Bits(spsr, 22, 22);

    if (state->PSTATE.nRW)
    {
        //AArch32 state No PSTATE.D in AArch32 state
        state->PSTATE.Q = Bits(spsr, 27, 27);
        state->PSTATE.IT = RestoredITBits(state, spsr);
        state->PSTATE.DIT = Restarting(state) ? Bits(spsr, 24, 24) : Bits(spsr, 21, 21);
        state->PSTATE.Ge = Bits(spsr, 16, 19);
        state->PSTATE.E = Bits(spsr, 9, 9);
        state->PSTATE.A = Bits(spsr, 6, 6);
        state->PSTATE.I = Bits(spsr, 7, 7);
        state->PSTATE.F = Bits(spsr, 8, 8);
        state->PSTATE.T = Bits(spsr, 5, 5);
    } else
    {
        //AArch64 state No PSTATE.<Q,IT,GE,E,T> in AArch64 state
        state->PSTATE.TCO = Bits(spsr, 25, 25);
        state->PSTATE.DIT = Bits(spsr, 24, 24);
        state->PSTATE.UAO = Bits(spsr, 23, 23);
        state->PSTATE.BTYPE = Bits(spsr, 10, 11);
        state->PSTATE.D = Bits(spsr, 6, 6);
        state->PSTATE.A = Bits(spsr, 7, 7);
        state->PSTATE.I = Bits(spsr, 8, 8);
        state->PSTATE.F = Bits(spsr, 9, 9);
    }
    return;
}

bool_t HaveEL(int el)
{
    //if el in {EL2, EL0} then return True
    if (el < EL3)
        return True;
    else
        return False;
}

ARMword HaveSecureEL2Ext()
{
    return HasArchVersion(ARMv8p4);
}

ARMword IsSecureBelowEL3()
{

}

ARMword HaveAnyAArch32()
{
    return IMPLEMENTATION_DEFINED;
}

ARMword HighestELUsingAArch32()
{
    if (!HaveAnyAArch32())
        return False;

    return IMPLEMENTATION_DEFINED;
}

ARMword HighestEL()
{
    if (HaveEL(EL3))
        return EL3;
    else if (HaveEL(EL2))
        return EL2;
    else
        return EL1;
}

ARMword HaveAArch32EL(int el)
{
    if (!HaveEL(el))
        return False;
    else if (!HaveAnyAArch32())
        return False;
    else if (HighestELUsingAArch32())
        return True;
    else if (el == HighestEL())
        return False;
    else if (el == EL0)
        return True;
}

ARMword ELStateUsingAArch32(int el, int secure)
{
    if (!HaveAArch32EL(el))
        return False;
    else if (secure && el == EL2)
        return False;
    else if (HighestELUsingAArch32())
        return True;
    else if (el == HighestEL())
        return False;
}

ARMword ELUSingAArch32(int el)
{
    return ELStateUsingAArch32(el, IsSecureBelowEL3());
}

ARMword UsingAArch32(ARMul_State * state)
{
    ARMword aarch32 = state->PSTATE.nRW == 1;

    if (!HaveAnyAArch32())
        aarch32 = !aarch32;

    return aarch32;
}

ARMword IsSecure(ARMul_State * state)
{
    if (HaveEL(EL3) && !UsingAArch32(state) && state->PSTATE.EL == EL3)
        return True;
    else if (HaveEL(EL3) && UsingAArch32(state) && state->PSTATE.M == M32_Monitor)
        return True;

    return IsSecureBelowEL3();
}

ARMword IsSecureEL2Enabled(ARMul_State * state)
{
    if (HaveEL(EL2) && HaveSecureEL2Ext())
    {
        if (HaveEL(EL3))
        {
            if (!ELUSingAArch32(EL3) && state->system_reg.SCR_EL3.EEL2 == 1)
                return True;
            else
                return False;
        } else
            return IsSecure(state);
    } else
        return False;
}

ARMword EL2Enabled(ARMul_State * state)
{
    return HaveEL(EL2) && (!HaveEL(EL3) || state->system_reg.SCR_EL3.NS == 1 || IsSecureEL2Enabled(state));
}

void updatePEState(ARMul_State * state, ARMword isRead, ARMword regIndex, ARMword Rt)
{
    SystemRegister_t *SysReg = &state->system_reg;
    ARMdword data;

    switch (regIndex)
    {
        case System_CurrentEL:
            data = state->PSTATE.EL << 2;
            if (isRead)
            {
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            }
            break;
        case System_DAIF:
            if (isRead)
            {
                data = (((state->PSTATE.all >> 4) & 0xf) << 6);
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            } else
            {
                Rt == 31 ? (data = state->A64_SP) : (data = state->R[Rt].X);
                state->PSTATE.all &= ~0xf0;
                state->PSTATE.all |= (data >> 6) & 0xf;
            }
            break;
        case System_DIT:
            if (isRead)
            {
                data = (state->PSTATE.DIT << 24);
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            } else
            {
                Rt == 31 ? (data = state->A64_SP) : (data = state->R[Rt].X);
                state->PSTATE.DIT = (data >> 24) & 0x1;
            }
            break;
        case System_NZCV:
            if (isRead)
            {
                data = ((state->PSTATE.all & 0xf) << 28);
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            } else
            {
                Rt == 31 ? (data = state->A64_SP) : (data = state->R[Rt].X);
                state->PSTATE.all &= ~0xf;
                state->PSTATE.all |= (data >> 28) & 0xf;
            }
            break;
        case System_PAN:
            if (isRead)
            {
                data = state->PSTATE.PAN << 22;
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            } else
            {
                Rt == 31 ? (data = state->A64_SP) : (data = state->R[Rt].X);
                state->PSTATE.PAN = (data >> 22) & 0x1;
            }
            break;
        case System_SPSel:
            if (isRead)
            {
                data = state->PSTATE.SP;
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            } else
            {
                Rt == 31 ? (data = state->A64_SP) : (data = state->R[Rt].X);
                state->PSTATE.SP = data & 0x1;
            }
            break;
        case System_SSBS:
            if (isRead)
            {
                data = state->PSTATE.SSBS << 12;
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            } else
            {
                Rt == 31 ? (data = state->A64_SP) : (data = state->R[Rt].X);
                state->PSTATE.SSBS = (data >> 12) & 0x1;
            }
            break;
        case System_TCO:
            if (isRead)
            {
                data = state->PSTATE.TCO << 25;
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            } else
            {
                Rt == 31 ? (data = state->A64_SP) : (data = state->R[Rt].X);
                state->PSTATE.TCO = (data >> 25) & 0x1;
            }
            break;
        case System_UAO:
            if (isRead)
            {
                data = state->PSTATE.UAO << 23;
                Rt == 31 ? (state->A64_SP = data) : (state->R[Rt].X = data);
            } else
            {
                Rt == 31 ? (data = state->A64_SP) : (data = state->R[Rt].X);
                state->PSTATE.UAO = (data >> 23) & 0x1;
            }
            break;
        default:
            NOT_IMPLEMENT;
            break;
    }
}

void set_insn_syndrome(ARMul_State * state, ARMword syn)
{

}

void check_interrupt(ARMul_State * state)
{
    if (!state->NirqSig && !state->PSTATE.I)
    {
        ARMdword exception_return = state->A64_PC + 4;
        ARMword vect_offset = 0x80;
        ExceptionRecord exception;

        exception = ExceptionSyndrome(Exception_IRQ);
        TakeException(state, EL1, exception, exception_return, vect_offset);
    } else if (!state->NfiqSig && !state->PSTATE.F)
    {
        ARMdword exception_return = state->A64_PC + 4;
        ARMword vect_offset = 0x100;
        ExceptionRecord exception;

        exception = ExceptionSyndrome(Exception_FIQ);
        TakeException(state, EL1, exception, exception_return, vect_offset);
    } else if (!state->NresetSig && !state->PSTATE.A)
    {
        ARMdword exception_return = state->A64_PC + 4;
        ARMword vect_offset = 0x180;
        ExceptionRecord exception;

        exception = ExceptionSyndrome(Exception_SError);
        TakeException(state, EL1, exception, exception_return, vect_offset);
    }
}

void checkForERetTrap(ARMul_State * state, bool_t eret_with_pac, bool_t pac_uses_key_a)
{
    // Non-secure EL1 execution of ERET, ERETAA, ERETAB when HCR_EL2.NV bit is set, is trapped to EL2
    bool_t route_to_el2 = state->PSTATE.EL == EL1 && EL2Enabled(state) && state->system_reg.HCR_EL2.NV0 == 1;

    if (route_to_el2)
    {
        ExceptionRecord exception;
        ARMdword exception_return = state->A64_PC;
        ARMword vect_offset = 0;

        exception = ExceptionSyndrome(Exception_ERetTrap);

        if (eret_with_pac)
        {
            exception.syndrome |= 0x2;
            if (!pac_uses_key_a)
                exception.syndrome |= 0x1;
        }

        TakeException(state, EL2, exception, exception_return, vect_offset);
    }
}

ARMdword BranchAddr(ARMul_State * state, ARMdword new_pc)
{
    //Not implement Fix me
    NOT_IMPLEMENT;
}

void clearExclusiveLocal(ARMul_State * state)
{
    // Clear the local Exclusives monitor for the specified processorid
}

void sendEventLocal(ARMul_State * state)
{
    //Set the local Event Register of this PE
    // When a PE executes the SEVL instruction, it causes this function to be executed
}

void ExceptionReturn(ARMul_State * state, ARMdword new_pc, ARMword spsr)
{
    SynchronizeContext(state);

    // Attempts to change to an illegal state will invoke the Illegal Execution state mechanism
    setPSTATEFromPSR(state, spsr);
    clearExclusiveLocal(state);
    sendEventLocal(state);

    if (state->PSTATE.IL && Bits(spsr, 4, 4) && Bits(spsr, 20, 20) == 0)
        // If the exception return is illegal, PC[63:32,1:0] are UNKNOWN
        new_pc &= 0xfffffffc;
    else if (UsingAArch32(state))
    {
        if (state->PSTATE.T)
            new_pc &= 0xfffffffe;       //T32
        else
            new_pc &= 0xfffffffc;       //A32
    } else
    {
        // ELR_ELx[63:56] might include a tag
        new_pc = BranchAddr(state, new_pc);
    }

    if (UsingAArch32(state))
        state->A64_PC = new_pc & 0xffffffff;
    else
        state->A64_PC = new_pc;
}

bool_t AllocationTagAccessIsEnable(ARMul_State * state)
{
    if (state->system_reg.SCR_EL3.ATA == 0 && (state->PSTATE.EL < EL3))
        return False;
    else if (state->system_reg.HCR_EL2.ATA == 0 && (state->PSTATE.EL < EL2)
             && EL2Enabled(state) && (state->system_reg.HCR_EL2.E2H != 1 && state->system_reg.HCR_EL2.TGE != 1))
        return False;
    else if (state->system_reg.SCTLR_EL3.ATA == 0 && state->PSTATE.EL == EL3)
        return False;
    else if (state->system_reg.SCTLR_EL2.ATA == 0 && state->PSTATE.EL == EL2)
        return False;
    else if (state->system_reg.SCTLR_EL1.ATA == 0 && state->PSTATE.EL == EL1)
        return False;
    else if (state->system_reg.SCTLR_EL1.ATA0 == 0 && (state->PSTATE.EL == EL0)
             && EL2Enabled(state) && (state->system_reg.HCR_EL2.E2H != 1 && state->system_reg.HCR_EL2.TGE != 1))
        return False;
    else if (state->system_reg.SCTLR_EL1.ATA0 == 0 && (state->PSTATE.EL == EL0)
             && !(EL2Enabled(state) && (state->system_reg.HCR_EL2.E2H != 1 && state->system_reg.HCR_EL2.TGE != 1)))
        return False;
    else
        return True;
}

ARMword ChooseNonExcludedTag(ARMword tag, ARMword offset, uint16_t exclude)
{
    if (exclude == 0xffff)
        return 0;

    if (offset == 0)
    {
        while ((exclude >> tag & 0x1) == 1)
        {
            tag += 1;
        }
    }

    while (offset-- != 0)
    {
        tag += 1;
        while ((exclude >> tag & 0x1) == 1)
        {
            tag += 1;
        }
    }

    return tag;
}

ARMdword AddressWithAllocationTag(ARMul_State * state, ARMdword address, ARMword allocation_tag)
{
    ARMword tag;

    if (AllocationTagAccessIsEnable(state))
        tag = allocation_tag;
    else
        tag = 0;

    address &= ~0xf00000000000000LL;
    address |= (ARMdword) tag << 56;

    return address;
}

inline bool_t HasS2Translation(ARMul_State * state)
{
    return (EL2Enabled(state) && (state->PSTATE.EL < EL2));
}

inline SCTLR_reg_t getSCTLR(ARMul_State * state)
{
    SCTLR_reg_t result;

    switch (state->PSTATE.EL)
    {
        case EL1:
            result.all = state->system_reg.SCTLR_EL1.all;
            break;
        case EL2:
            result.all = state->system_reg.SCTLR_EL2.all;
            break;
        case EL3:
            result.all = state->system_reg.SCTLR_EL3.all;
            break;
        default:
            printf("UnKnown Exception level!\n");
            break;
    }

    return result;
}

inline ARMword AccessUsesEL(ARMul_State * state, AccType accType)
{
    if (accType == AccType_UNPRIV)
        return EL0;
    else if (accType == AccType_NV2REGISTER)
        return EL2;
    else
        return state->PSTATE.EL;
}

inline bool_t ELIsInHost(ARMul_State * state, ARMword el)
{
    return (state->system_reg.HCR_EL2.E2H == 1) && (el == EL2 || (el == EL0 && state->system_reg.HCR_EL2.TGE == 1));
}

inline bool_t S1CacheDisabled(ARMul_State * state, AccType accType)
{
    SCTLR_reg_t sctlr;

    sctlr = getSCTLR(state);
    bool_t enable = accType == AccType_IFETCH ? sctlr.I : sctlr.C;

    return enable == 0;
}

inline bool_t S2CacheDisabled(ARMul_State * state, AccType accType)
{
    bool_t disable = accType == AccType_IFETCH ? state->system_reg.HCR_EL2.ID : state->system_reg.HCR_EL2.CD;

    return disable == 1;
}
