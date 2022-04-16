
/*************************************************************************
	> File Name    : armv8_helper.h
	> Author       : jiachao
	> Mail         : jiachao@163.com
	> Created Time : 2019/9/23 11:05:50
************************************************************************/

#ifndef _ARMV8_HELPER_H
#define _ARMV8_HELPER_H

extern inline uint16_t revbit16(uint16_t x);
extern inline uint32_t revbit32(uint32_t x);
extern inline uint64_t revbit64(uint64_t x);
extern inline int64_t gen_sar(uint64_t data, int shift);
extern inline int64_t gen_sextract64(uint64_t value, int ofs, int len);
extern inline int64_t gen_extract64(uint64_t value, int ofs, int len);
extern uint64_t gen_deposit64(uint64_t rd_val, uint64_t rn_val, int ofs, int len);
extern uint64_t gen_deposit_z64(uint64_t rn_val, int ofs, int len);
extern inline int64_t sextract64(uint64_t value, int start, int length);
extern inline int64_t SignExtend64(uint64_t data, int size);
extern inline uint64_t GetShiftValue(uint64_t Data, int shift);
extern inline int clz32(uint32_t val);
extern inline int ctz32(uint32_t val);
extern inline int clz64(uint64_t val);
extern inline int cls32(uint32_t val);
extern inline int cls64(uint64_t val);
extern inline uint64_t bitmask64(uint32_t length);
extern inline uint64_t bitfield_replicate(uint64_t mask, uint32_t e);

#ifdef __cplusplus
extern "C"
{
#endif
    extern inline uint32_t DecodeBitMasks(uint64_t * result, uint32_t n, uint32_t imms, uint32_t immr);
#ifdef __cplusplus
}
#endif
extern inline uint32_t Poly32Mod2(uint64_t Data, uint32_t poly, int size);
extern inline uint32_t ROR32(uint32_t Data, int amount);
extern inline uint64_t ROR64(uint64_t Data, int amount);
extern inline uint32_t extract32(uint32_t value, int start, int length);

#ifdef __WIN32__
#define BIT64_MAX_VALUE 0xFFFFFFFFFFFFFFFFULL
#define BIT32_MAX_VALUE 0xFFFFFFFFUL
#else
#define BIT64_MAX_VALUE 0xFFFFFFFFFFFFFFFFUL
#define BIT32_MAX_VALUE 0xFFFFFFFFUL
#endif

#define SignExtend32(Data, n)	((Data & (1 << (n - 1))) ? (Data | (BIT32_MAX_VALUE << n)) : Data)
#define BIT32_SHIFT(Data, n)  Data = BIT32_MAX_VALUE >> n << n
#define BIT64_SHIFT(Data, n)  Data = BIT64_MAX_VALUE >> n << n

#define DEBUG 0
#if DEBUG
#define DEBUG_INSN	 printf("In %s.Line:%d. PC = 0x%llx Instr = 0x%x\n", __func__, __LINE__, state->A64_PC, state->instr);
#else
#define DEBUG_INSN
#endif
#endif
