/*************************************************************************
	> File Name    : helper.h
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2021/10/26 10:17:40
************************************************************************/

#ifndef _HELPER_H
#define _HELPER_H

/* same as PROT_xxx */
#define PAGE_READ      0x0001
#define PAGE_WRITE     0x0002
#define PAGE_EXEC      0x0004
#define PAGE_BITS      (PAGE_READ | PAGE_WRITE | PAGE_EXEC)
#define PAGE_VALID     0x0008

void fpu_set_state(tricore_cpu_t *core);
void psw_write(tricore_cpu_t *core, uint32_t val);
uint32_t psw_read(tricore_cpu_t *core);
#endif
