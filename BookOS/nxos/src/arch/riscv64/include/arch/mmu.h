/**
 * Copyright (c) 2018-2022, NXOS Development Team
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Contains: Memory Manage Unite 
 * 
 * Change Logs:
 * Date           Author            Notes
 * 2021-10-20     JasonHu           Init
 * 2022-1-20      JasonHu           add map & unmap
 */

#ifndef __ARCH_MMU__
#define __ARCH_MMU__

#include <xbook.h>

// page table entry (PTE) fields
#define PTE_V     0x001 // Valid
#define PTE_R     0x002 // Read
#define PTE_W     0x004 // Write
#define PTE_X     0x008 // Execute
#define PTE_U     0x010 // User
#define PTE_G     0x020 // Global
#define PTE_A     0x040 // Accessed
#define PTE_D     0x080 // Dirty
#define PTE_SOFT  0x300 // Reserved for Software
#define PTE_S     0x000 // system

#define NX_PAGE_ATTR_RWX    (PTE_X | PTE_W | PTE_R)

#define NX_PAGE_ATTR_KERNEL (PTE_V | NX_PAGE_ATTR_RWX | PTE_S | PTE_G)
#define NX_PAGE_ATTR_USER   (PTE_V | NX_PAGE_ATTR_RWX | PTE_U | PTE_G)

#endif  /* __ARCH_MMU__ */
