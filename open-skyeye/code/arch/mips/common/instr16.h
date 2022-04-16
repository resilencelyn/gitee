#ifndef _INSTR16_H_
#define _INSTR16_H_

#include "inttypes.h"

// 16 bit instruction decoder functions.

#define opcode(instr16) bits(instr16, 15, 11)
#define rx(instr16) bits(instr16, 10, 8)
#define ry(instr16) bits(instr16, 7, 5)
#define rz(instr16) bits(instr16, 4, 2)

/* I-Type (Immediate) */
#define imm_i(instr16) bits(instr16, 10, 0)
// Some aliases for the above.
#define offset(instr16) bits(instr16, 10, 0)

/* RI-Type */
#define imm_ri(instr16) bits(instr16, 7, 0)

/* RR-Type */
#define funct_rr(instr16) bits(instr16, 4, 0)

/* RRI-Type */
#define imm_rri(instr16) bits(instr16, 4, 0)

/* RRR-Type */
#define funct_rrr(instr16) bits(instr16, 1, 0)

/* RRI-A-Type */
#define funct_rria(instr16) bit(instr16, 4)
#define imm_rria(instr16) bits(instr16, 3, 0)

/* Shift */
#define shift_sa(instr16) bits(instr16, 4, 2)
#define funct_shift(instr16) bits(instr16, 1, 0)

/* JAL Type */
#define IS_JALX(instr16) (bit(instr16, 10) == 1)

/* I8-Type */
// same to RI
#define funct_i8(instr16) bits(instr16, 10, 8)

/* I8-MOVR32 Type */
#define mov_r32(instr16) bits(instr16, 4, 0)
#define mov_32r(instr16) (bits(instr16, 4, 3) << 3) | bits(instr16, 7, 5)
#define mov_32rz(instr16) bits(instr16, 2, 0)

/* I8-SVRS Type */
#define svrs_framesize(instr16) bits(instr16, 3, 0)
#define svrs_s(instr16) bit(instr16, 7)
#define svrs_ra(instr16) bit(instr16, 6)
#define svrs_s0(instr16) bit(instr16, 5)
#define svrs_s1(instr16) bit(instr16, 4)

/* The (op) field */
#define ADDIUSP 000
#define ADDIUPC 001
#define B 002
#define JAL 003
#define BEQZ 004
#define BNEZ 005
#define SHIFT 006

#define RRIA 010
#define ADDIU8 011
#define SLTI 012
#define SLTIU 013
#define I8 014
#define LI 015
#define CMPI 016

#define LB 020
#define LH 021
#define LWSP 022
#define LW 023
#define LBU 024
#define LHU 025
#define LWPC 026

#define SB 030
#define SH 031
#define SWSP 032
#define SW 033
#define RRR 034
#define RR 035
#define EXTEND 036

/* SHIFT Encoding of the f Field */
#define SLL 000
// reserved             = 001,
#define SRL 002
#define SRA 003

/* RRI-A Encoding of the f Field */
#define ADDIU 000

/* I8 Encoding of the funct Field */
#define BTEQZ 000
#define BTNEZ 001
#define SWRASP 002
#define ADJSP 003
#define SVRS 004
#define MOV32R 005
#define MOVR32 007

#define RESTORE 000
#define SAVE 001

/* RRR Encoding of the f Field */
#define ADDU 001
#define SUBU 003

/* RR Encoding of the Funct Field */
#define JR 000
#define SDBBP 001
#define SLT 002
#define SLTU 003
#define SLLV 004
#define BREAK 005
#define SRLV 006
#define SRAV 007

#define CMP 012
#define NEG 013
#define AND 014
#define OR 015
#define XOR 016
#define NOT 017

#define MFHI 020
#define CNVT 021
#define MFLO 022
#define MULT 030
#define MULTU 031
#define DIV 032
#define DIVU 033

#define JRRX 000
#define JRRA 001
#define JALR 002
#define JRCRX 004
#define JRCRA 005
#define JALRC 006

#define ZEB 000
#define ZEH 001
#define SEB 004
#define SEH 005

#endif // end of _INSTR16_H_
