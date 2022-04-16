
/*************************************************************************
  > File Name    : armv8_memory.h
  > Author       : jiachao
  > Mail         : jiachao@163.com
  > Created Time : 2019/9/22 13:43:32
 ************************************************************************/

#ifndef _ARMV8_MEMORY_H
#define _ARMV8_MEMORY_H

#include "armdefs.h"
#include "arch64_decode.h"

typedef enum
{
    MemType_Normal = 0,
    MemType_Device
} MemType;

typedef enum
{
    DeviceType_GRE = 0,
    DeviceType_nGRE,
    DeviceType_nGnRE,
    DeviceType_nGnRnE
} DeviceType;

typedef struct
{
    uint8_t attrs;                      //Cacheability attributes
    uint8_t hints;                      // Allocation hints
    bool_t transient;
} MemAttrHints;

typedef struct
{
    MemType memtype;
    DeviceType device;                  // For Device memory types
    MemAttrHints inner;                 // Inner hints and attributes
    MemAttrHints outer;                 // Outer hints and attributes
    bool_t tagged;                      // Tagged access
    bool_t shareable;
    bool_t outershareable;
} MemoryAttributes;

typedef struct
{
    uint64_t address;
    bool_t NS;                          // '0' = Secure, '1' = Non-secure
} FullAddress;

#define MemAttr_NC  0                   // Non-cacheable
#define MemAttr_WT  2                   // Write-through
#define MemAttr_WB  3                   // Write-back

#define MemHint_No  0                   // No Read-Allocate, No Write-Allocate
#define MemHint_WA  1                   // No Read-Allocate, Write-Allocate
#define MemHint_RA  2                   // Read-Allocate, No Write-Allocate
#define MemHint_RWA 3                   // Read-Allocate, Write-Allocate
typedef enum
{
    AccType_NORMAL,
    AccType_VEC,                        // Normal loads and stores
    AccType_STREAM,
    AccType_VECSTREAM,                  // Streaming loads and stores
    AccType_ATOMIC,
    AccType_ATOMICRW,                   // Atomic loads and stores
    AccType_ORDERED,
    AccType_ORDEREDRW,                  // Load-Acquire and Store-Release
    AccType_ORDEREDATOMIC,              // Load-Acquire and Store-Release with atomic access
    AccType_ORDEREDATOMICRW,
    AccType_LIMITEDORDERED,             // Load-LOAcquire and Store-LORelease
    AccType_UNPRIV,                     // Load and store unprivileged
    AccType_IFETCH,                     // Instruction fetch
    AccType_PTW,                        // Page table walk
    AccType_NONFAULT,                   // Non-faulting loads
    AccType_CNOTFIRST,                  // Contiguous FF load, not first element
    AccType_NV2REGISTER,                // MRS/MSR instruction used at EL1 and which is converted
    // to a memory access that uses the EL2 translation regime
    // Other operations
    AccType_DC,                         // Data cache maintenance
    AccType_DC_UNPRIV,                  // Data cache maintenance instruction used at EL0
    AccType_IC,                         // Instruction cache maintenance
    AccType_DCZVA,                      // DC ZVA instructions
    AccType_AT,                         // Address translation
} AccType;

typedef enum
{
    Fault_None = 0,
    Fault_AccessFlag,
    Fault_Alignment,
    Fault_Background,
    Fault_Domain,
    Fault_Permission,
    Fault_Translation,
    Fault_AddressSize,
    Fault_SyncExternal,
    Fault_SyncExternalOnWalk,
    Fault_SyncParity,
    Fault_SyncParityOnWalk,
    Fault_AsyncParity,
    Fault_AsyncExternal,
    Fault_Debug,
    Fault_TLBConflict,
    Fault_BranchTarget,
    Fault_HWUpdateAccessFlag,
    Fault_Lockdown,
    Fault_Exclusive,
    Fault_ICacheMaint
} Fault;

typedef struct
{
    Fault statuscode;                   // Fault Status
    AccType acctype;                    // Type of access that faulted
    FullAddress ipaddress;              // Intermediate physical address
    bool_t s2fs1walk;                   // Is on a Stage 1 page table walk
    bool_t write;                       // TRUE for a write, FALSE for a read
    int level;                          // For translation, access flag and permission faults
    int extflag;                        // IMPLEMENTATION DEFINED syndrome for external aborts
    bool_t secondstage;                 // Is a Stage 2 abort
    int domain;                         // Domain number, AArch32 only
    int errortype;                      // [Armv8.2 RAS] AArch32 AET or AArch64 SET
    int debugmoe;                       // Debug method of entry, from AArch32 only
} FaultRecord;

typedef struct
{
    FaultRecord fault;                  // fault.statuscode indicates whether the address is valid
    MemoryAttributes memattrs;
    FullAddress paddress;
    uint64_t vaddress;
} AddressDescriptor;

typedef struct
{
    uint8_t ap;                         // Access permission bits
    uint8_t xn;                         //Execute-never bit
    uint8_t xxn;                        // [Armv8.2] Extended execute-never bit for stage 2
    uint8_t pxn;                        // Privileged execute-never bit
} Permissions;

typedef struct
{
    bool_t AF;                          // AF needs to be set
    bool_t AP;                          // AP[2] / S2AP[2] will be modified
    AddressDescriptor descaddr;         // Descriptor to be updated
} DescriptorUpdate;

typedef struct
{
    Permissions perms;
    bool_t nG;                          // '0' = Global, '1' = not Global
    int domain;                         // AArch32 only
    int GP;                             // Guarded Page
    bool_t contiguous;                  // Contiguous bit from page table
    int level;                          // AArch32 Short-descriptor format: Indicates Section/Page
    int blocksize;                      // Describes size of memory translated in KBytes
    DescriptorUpdate descupdate;        // [Armv8.1] Context for h/w update of table descriptor
    int CnP;                            // [Armv8.2] TLB entry can be shared between different PEs
    AddressDescriptor addrdesc;
} TLBRecord;

extern void check_sp_alignment(ARMul_State * state);
extern inline bool_t checkAlignment(ARMdword address, int size);
extern void clear_vec_high(ARMul_State * state, bool_t is_q, int tt, int size);
extern void do_vec_st(ARMul_State * state, int tt, int e, int address, int size);
extern void do_vec_ld(ARMul_State * state, int tt, int e, int address, int size);
extern void mem_st_i64(ARMul_State * state, ARMdword address, ARMdword data, ARMword size);
extern void mem_ld_i64(ARMul_State * state, ARMdword address, ARMdword * data, ARMword size);
extern void do_fp_st_mem(ARMul_State * state, ARMdword address, ARMword Rt, ARMword size);
extern bit128 do_fp_ld_mem(ARMul_State * state, ARMdword address, ARMword size);

extern void do_st_mem(ARMul_State * state, ARMdword address, ARMdword data, ARMword size, bool_t iss_valid,
                      ARMword iss_srt, ARMword iss_sf, ARMword iss_ar);
extern ARMdword do_ld_mem(ARMul_State * state, ARMdword address, ARMword size, bool_t is_signed, bool_t is_extended,
                          bool_t iss_valid, ARMword iss_srt, ARMword iss_sf, ARMword iss_ar);
extern bool_t ldst_compute_iss_sf(int size, bool_t is_signed, int opc);

//extern fault_t mem_read_dword(ARMul_State *state, ARMdword address, ARMdword *data);
extern fault_t mem_read_dword(ARMul_State * state, ARMdword address, ARMword * data_lo32, ARMword * data_hi32);
extern fault_t mem_read_word(ARMul_State * state, ARMdword address, ARMword * data);
extern fault_t mem_read_half(ARMul_State * state, ARMdword address, ARMword * data);
extern fault_t mem_read_byte(ARMul_State * state, ARMdword address, ARMword * data);

extern fault_t mem_write_dword(ARMul_State * state, ARMdword address, ARMdword data);
extern fault_t mem_write_word(ARMul_State * state, ARMdword address, ARMword data);
extern fault_t mem_write_half(ARMul_State * state, ARMdword address, ARMhword data);
extern fault_t mem_write_byte(ARMul_State * state, ARMdword address, ARMbyte data);

extern fault_t arch64_mem_read(ARMul_State * state, ARMdword address, AccType accType, ARMword * data_lo32,
                               ARMword * data_hi32, int count);
extern fault_t arch64_mem_write(ARMul_State * state, ARMdword address, AccType accType, ARMdword data, int count);

#define IsFault(addrDesc) (addrDesc.fault.statuscode != Fault_None)
#endif
