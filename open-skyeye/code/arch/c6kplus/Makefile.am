#c6000_dyncom = dyncom/c6kplus_dyncom_run.cpp dyncom/c6kplus_dyncom_tlb.cpp  dyncom/c6kplus_dyncom_translate.cpp dyncom/c6kplus_dyncom_trap.cpp dyncom/mmu/mmu_dyncom.cpp dyncom/c6kplus_step_diff.cpp
c6000_comm = common/c6k_generic_core.c  common/exec_insn.c common/c6k_decode_16.c 

#if LLVM_EXIST
#c6000_comm += $(c6000_dyncom)
#endif
# we will link the common module with main
DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS @COMMON_CFLAGS@

#if LLVM_EXIST
#DEFS += -DLLVM_EXIST
#endif
INCLUDES = -I./common -I./mach @COMMON_INCLUDES@
pkglib_LTLIBRARIES =

c64x_core = c64x_core/c64x_core.c c64x_core/c64x_module.c
libc64x_core_la_SOURCES = $(c6000_comm) $(c64x_core)
libc64x_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libc64x_core.la

c64xplus_core = c64xplus_core/c64xplus_core.c c64xplus_core/c64xplus_module.c
libc64xplus_core_la_SOURCES = $(c6000_comm) $(c64xplus_core)
libc64xplus_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libc64xplus_core.la

c66x_core = c66x_core/c66x_core.c c66x_core/c66x_module.c
libc66x_core_la_SOURCES = $(c6000_comm) $(c66x_core)
libc66x_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libc66x_core.la

c67x_core = c67x_core/c67x_core.c c67x_core/c67x_module.c
libc67x_core_la_SOURCES = $(c6000_comm) $(c67x_core)
libc67x_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libc67x_core.la

c674x_core = c674x_core/c674x_core.c c674x_core/c674x_module.c
libc674x_core_la_SOURCES = $(c6000_comm) $(c674x_core)
libc674x_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libc674x_core.la


if WIN32
libc64x_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
libc64xplus_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
libc66x_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
libc67x_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
libc674x_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
endif
#if LLVM_EXIST
#INCLUDES += @LLVM_CFLAGS@
#endif

