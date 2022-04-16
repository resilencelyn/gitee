
/*************************************************************************
  > File Name    : armv8_exception.h
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/8/13 10:07:45
 ************************************************************************/

#ifndef _ARMV8_EXCEPTION_H
#define _ARMV8_EXCEPTION_H

#include "arch64_memory.h"
typedef unsigned int Exception;

enum ArchVersion
{
    ARMv8p0 = 0,
    ARMv8p1,
    ARMv8p2,
    ARMv8p3,
    ARMv8p4,
    ARMv8p5,
};

#define Default_ArchVersion		ARMv8p0

#define HasArchVersion(version)		(version & Default_ArchVersion) == version

#define IMPLEMENTATION_DEFINED 1

#define Bits(data, m, n)	((data<<(31-(n))) >> ((31-(n))+(m)))
/* Valid Syndrome Register EC field values */
enum Exception_t
{
    EC_UNCATEGORIZED = 0x00,
    EC_WFX_TRAP = 0x01,
    EC_CP15RTTRAP = 0x03,
    EC_CP15RRTTRAP = 0x04,
    EC_CP14RTTRAP = 0x05,
    EC_CP14DTTRAP = 0x06,
    EC_ADVSIMDFPACCESSTRAP = 0x07,
    EC_FPIDTRAP = 0x08,
    EC_PACTRAP = 0x09,
    EC_CP14RRTTRAP = 0x0c,
    EC_ILLEGALSTATE = 0x0e,
    EC_AA32_SVC = 0x11,
    EC_AA32_HVC = 0x12,
    EC_AA32_SMC = 0x13,
    EC_AA64_SVC = 0x15,
    EC_AA64_HVC = 0x16,
    EC_AA64_SMC = 0x17,
    EC_SYSTEMREGISTERTRAP = 0x18,
    EC_SVEACCESSTRAP = 0x19,
    EC_INSNABORT = 0x20,
    EC_INSNABORT_SAME_EL = 0x21,
    EC_PCALIGNMENT = 0x22,
    EC_DATAABORT = 0x24,
    EC_DATAABORT_SAME_EL = 0x25,
    EC_SPALIGNMENT = 0x26,
    EC_AA32_FPTRAP = 0x28,
    EC_AA64_FPTRAP = 0x2c,
    EC_SERROR = 0x2f,
    EC_BREAKPOINT = 0x30,
    EC_BREAKPOINT_SAME_EL = 0x31,
    EC_SOFTWARESTEP = 0x32,
    EC_SOFTWARESTEP_SAME_EL = 0x33,
    EC_WATCHPOINT = 0x34,
    EC_WATCHPOINT_SAME_EL = 0x35,
    EC_AA32_BKPT = 0x38,
    EC_VECTORCATCH = 0x3a,
    EC_AA64_BKPT = 0x3c,
};

enum
{
    Exception_Uncategorized = 0,        // Uncategorized or unknown reason
    Exception_WFxTrap,                  // Trapped WFI or WFE instruction
    Exception_CP15RTTrap,               // Trapped AArch32 MCR or MRC access to CP15
    Exception_CP15RRTTrap,              // Trapped AArch32 MCRR or MRRC access to CP15
    Exception_CP14RTTrap,               // Trapped AArch32 MCR or MRC access to CP14
    Exception_CP14DTTrap,               // Trapped AArch32 LDC or STC access to CP14
    Exception_AdvSIMDFPAccessTrap,      // HCPTR-trapped access to SIMD or FP
    Exception_FPIDTrap,                 // Trapped access to SIMD or FP ID register
    // Trapped BXJ instruction not supported in Armv8
    Exception_PACTrap,                  // Trapped invalid PAC use
    Exception_CP14RRTTrap,              // Trapped MRRC access to CP14 from AArch32
    Exception_IllegalState,             // Illegal Execution state
    Exception_SupervisorCall,           // Supervisor Call
    Exception_HypervisorCall,           // Hypervisor Call
    Exception_MonitorCall,              // Monitor Call or Trapped SMC instruction
    Exception_SystemRegisterTrap,       // Trapped MRS or MSR system register access
    Exception_ERetTrap,                 // Trapped invalid ERET use
    Exception_InstructionAbort,         // Instruction Abort or Prefetch Abort
    Exception_PCAlignment,              // PC alignment fault
    Exception_DataAbort,                // Data Abort
    Exception_NV2DataAbort,             // Data abort at EL1 reported as being from EL2
    Exception_SPAlignment,              // SP alignment fault
    Exception_FPTrappedException,       // IEEE trapped FP exception
    Exception_SError,                   // SError interrupt
    Exception_Breakpoint,               // (Hardware) Breakpoint
    Exception_SoftwareStep,             // Software Step
    Exception_Watchpoint,               // Watchpoint
    Exception_NV2Watchpoint,            // Watchpoint at EL1 reported as being from EL2
    Exception_SoftwareBreakpoint,       // Software Breakpoint Instruction
    Exception_VectorCatch,              // AArch32 Vector Catch
    Exception_IRQ,                      // IRQ interrupt
    Exception_SVEAccessTrap,            // HCPTR trapped access to SVE
    Exception_BranchTarget,             // Branch Target Identification
    Exception_FIQ                       // FIQ interrupt
};

#define ARM_EL_EC_SHIFT 26
#define ARM_EL_IL_SHIFT 25
#define ARM_EL_ISV_SHIFT 24
#define ARM_EL_IL (1 << ARM_EL_IL_SHIFT)
#define ARM_EL_ISV (1 << ARM_EL_ISV_SHIFT)

/* Utility functions for constructing various kinds of syndrome value.
 * Note that in general we follow the AArch64 syndrome values; in a
 * few cases the value in HSR for exceptions taken to AArch32 Hyp
 * mode differs slightly, so if we ever implemented Hyp mode then the
 * syndrome value would need some massaging on exception entry.
 * (One example of this is that AArch64 defaults to IL bit set for
 * exceptions which don't specifically indicate information about the
 * trapping instruction, whereas AArch32 defaults to IL bit clear.)
 */
static inline ARMword syn_uncategorized(void)
{
    return (EC_UNCATEGORIZED << ARM_EL_EC_SHIFT) | ARM_EL_IL;
}

static inline ARMword syn_aa64_svc(ARMword imm16)
{
    return (EC_AA64_SVC << ARM_EL_EC_SHIFT) | ARM_EL_IL | (imm16 & 0xffff);
}

static inline ARMword syn_aa64_hvc(ARMword imm16)
{
    return (EC_AA64_HVC << ARM_EL_EC_SHIFT) | ARM_EL_IL | (imm16 & 0xffff);
}

static inline ARMword syn_aa64_smc(ARMword imm16)
{
    return (EC_AA64_SMC << ARM_EL_EC_SHIFT) | ARM_EL_IL | (imm16 & 0xffff);
}

static inline ARMword syn_aa32_svc(ARMword imm16, ARMword is_16bit)
{
    return (EC_AA32_SVC << ARM_EL_EC_SHIFT) | (imm16 & 0xffff) | (is_16bit ? 0 : ARM_EL_IL);
}

static inline ARMword syn_aa32_hvc(ARMword imm16)
{
    return (EC_AA32_HVC << ARM_EL_EC_SHIFT) | ARM_EL_IL | (imm16 & 0xffff);
}

static inline ARMword syn_aa32_smc(void)
{
    return (EC_AA32_SMC << ARM_EL_EC_SHIFT) | ARM_EL_IL;
}

static inline ARMword syn_aa64_bkpt(ARMword imm16)
{
    return (EC_AA64_BKPT << ARM_EL_EC_SHIFT) | ARM_EL_IL | (imm16 & 0xffff);
}

static inline ARMword syn_aa32_bkpt(ARMword imm16, ARMword is_16bit)
{
    return (EC_AA32_BKPT << ARM_EL_EC_SHIFT) | (imm16 & 0xffff) | (is_16bit ? 0 : ARM_EL_IL);
}

static inline ARMword syn_aa64_sysregtrap(int op0, int op1, int op2, int crn, int crm, int rt, int isread)
{
    return (EC_SYSTEMREGISTERTRAP << ARM_EL_EC_SHIFT) | ARM_EL_IL
        | (op0 << 20) | (op2 << 17) | (op1 << 14) | (crn << 10) | (rt << 5) | (crm << 1) | isread;
}

static inline ARMword syn_cp14_rt_trap(int cv, int cond, int opc1, int opc2,
                                       int crn, int crm, int rt, int isread, ARMword is_16bit)
{
    return (EC_CP14RTTRAP << ARM_EL_EC_SHIFT)
        | (is_16bit ? 0 : ARM_EL_IL)
        | (cv << 24) | (cond << 20) | (opc2 << 17) | (opc1 << 14) | (crn << 10) | (rt << 5) | (crm << 1) | isread;
}

static inline ARMword syn_cp15_rt_trap(int cv, int cond, int opc1, int opc2,
                                       int crn, int crm, int rt, int isread, ARMword is_16bit)
{
    return (EC_CP15RTTRAP << ARM_EL_EC_SHIFT)
        | (is_16bit ? 0 : ARM_EL_IL)
        | (cv << 24) | (cond << 20) | (opc2 << 17) | (opc1 << 14) | (crn << 10) | (rt << 5) | (crm << 1) | isread;
}

static inline ARMword syn_cp14_rrt_trap(int cv, int cond, int opc1, int crm,
                                        int rt, int rt2, int isread, ARMword is_16bit)
{
    return (EC_CP14RRTTRAP << ARM_EL_EC_SHIFT)
        | (is_16bit ? 0 : ARM_EL_IL)
        | (cv << 24) | (cond << 20) | (opc1 << 16) | (rt2 << 10) | (rt << 5) | (crm << 1) | isread;
}

static inline ARMword syn_cp15_rrt_trap(int cv, int cond, int opc1, int crm,
                                        int rt, int rt2, int isread, ARMword is_16bit)
{
    return (EC_CP15RRTTRAP << ARM_EL_EC_SHIFT)
        | (is_16bit ? 0 : ARM_EL_IL)
        | (cv << 24) | (cond << 20) | (opc1 << 16) | (rt2 << 10) | (rt << 5) | (crm << 1) | isread;
}

static inline ARMword syn_fp_access_trap(int cv, int cond, ARMword is_16bit)
{
    return (EC_ADVSIMDFPACCESSTRAP << ARM_EL_EC_SHIFT) | (is_16bit ? 0 : ARM_EL_IL) | (cv << 24) | (cond << 20);
}

static inline ARMword syn_sve_access_trap(void)
{
    return EC_SVEACCESSTRAP << ARM_EL_EC_SHIFT;
}

static inline ARMword syn_insn_abort(int same_el, int ea, int s1ptw, int fsc)
{
    return (EC_INSNABORT << ARM_EL_EC_SHIFT) | (same_el << ARM_EL_EC_SHIFT)
        | ARM_EL_IL | (ea << 9) | (s1ptw << 7) | fsc;
}

static inline ARMword syn_data_abort_no_iss(int same_el, int ea, int cm, int s1ptw, int wnr, int fsc)
{
    return (EC_DATAABORT << ARM_EL_EC_SHIFT) | (same_el << ARM_EL_EC_SHIFT)
        | ARM_EL_IL | (ea << 9) | (cm << 8) | (s1ptw << 7) | (wnr << 6) | fsc;
}

static inline ARMword syn_data_abort_with_iss(int same_el,
                                              int sas, int sse, int srt,
                                              int sf, int ar,
                                              int ea, int cm, int s1ptw, int wnr, int fsc, ARMword is_16bit)
{
    return (EC_DATAABORT << ARM_EL_EC_SHIFT) | (same_el << ARM_EL_EC_SHIFT)
        | (is_16bit ? 0 : ARM_EL_IL)
        | ARM_EL_ISV | (sas << 22) | (sse << 21) | (srt << 16)
        | (sf << 15) | (ar << 14) | (ea << 9) | (cm << 8) | (s1ptw << 7) | (wnr << 6) | fsc;
}

typedef struct ExceptionRecord_t
{
    Exception exceptype;                //Exception class
    ARMword syndrome;                   //Syndrome record
    ARMdword vaddress;                  //Virtual fault address
    ARMword ipavalid;                   //Physical fault address for second stage faults is valid
    ARMword NS;                         // Physical fault address for second stage faults is Non-secure or secure
    ARMdword ipaddress;                 //Physical fault address for second stage faults bit(52)
} ExceptionRecord;

void addPACGA(ARMul_State * state, ARMdword operand1, ARMdword operand2);
void TrapPACUse(ARMul_State * state, int target_el);
static ExceptionRecord ExceptionSyndrome(Exception exceptype);

ARMword EL2Enabled(ARMul_State * state);
bool_t HaveEL(int el);
ARMword GetPSRFromPSTATE(ARMul_State * state);
ARMword HighestELUsingAArch32();
ARMword IsSecureBelowEL3();
ARMword UsingAArch32(ARMul_State * state);
ARMword IsSecureEL2Enabled(ARMul_State * state);
ARMword ELUSingAArch32(int el);
inline bool_t ELIsInHost(ARMul_State * state, ARMword el);

void set_insn_syndrome(ARMul_State * state, ARMword syn);
void systemAccessTrap(ARMul_State * state, int target_el, int ec, ARMdword return_pc);
void updatePEState(ARMul_State * state, ARMword isRead, ARMword regIndex, ARMword Rt);
void check_interrupt(ARMul_State * state);
void checkForERetTrap(ARMul_State * state, bool_t eret_with_pac, bool_t pac_uses_key_a);
static TakeException(ARMul_State * state, int target_el, ExceptionRecord exception, ARMdword exception_return,
                     ARMword vect_offset);
void ExceptionReturn(ARMul_State * state, ARMdword target, ARMword spsr);

void setELR(ARMul_State * state, int target_el, ARMword value);
ARMdword getELR(ARMul_State * state, int target_el);

ARMword getSPSR(ARMul_State * state);
void setSPSR(ARMul_State * state, ARMword spsr);
bool_t AllocationTagAccessIsEnable(ARMul_State * state);
ARMword ChooseNonExcludedTag(ARMword tag, ARMword offset, uint16_t exclude);
ARMdword AddressWithAllocationTag(ARMul_State * state, ARMdword address, ARMword allocation_tag);
inline bool_t HasS2Translation(ARMul_State * state);
inline SCTLR_reg_t getSCTLR(ARMul_State * state);
inline ARMword AccessUsesEL(ARMul_State * state, AccType accType);
inline bool_t S1CacheDisabled(ARMul_State * state, AccType accType);
inline bool_t S2CacheDisabled(ARMul_State * state, AccType accType);
#endif
