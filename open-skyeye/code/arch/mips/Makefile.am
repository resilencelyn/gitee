
mips_comm = common/cache.c   common/decoder.c common/decoder16.c   common/icache.c     common/tlb.c \
common/cp0.c     common/emul.c       common/interrupt.c            \
common/dcache.c  common/exception.c  common/mips_arch_interface.c  common/multiply.c  common/mips_regformat.c
#mips_dyncom = dyncom/mips_dyncom_run.cpp dyncom/mips_dyncom_translate.cpp dyncom/mips_dyncom_interface.cpp dyncom/mips_dyncom_dec.cpp dyncom/mips_dyncom_instr_main.cpp dyncom/mips_dyncom_instr_special.cpp dyncom/mips_dyncom_instr_regimm.cpp
mips_syscall = linux_syscall/mips_syscall.c

gs32eb1 = gs32eb1/gs32eb1.c  gs32eb1/gs32eb1_module.c
libgs32eb1_core_la_SOURCES = $(mips_comm) $(mips_syscall) $(gs32eb1)
ls132r = ls132r/ls132r.c  ls132r/ls132r_module.c
libls132r_core_la_SOURCES = $(mips_comm) $(mips_syscall) $(ls132r)
#if LLVM_EXIST
##libgs32eb1_core_la_SOURCES += $(mips_dyncom)
#endif
pkglib_LTLIBRARIES = libgs32eb1_core.la
pkglib_LTLIBRARIES += libls132r_core.la

# we will link the common module with main
libgs32eb1_core_la_LDFLAGS = -module
libls132r_core_la_LDFLAGS = -module

if WIN32
WIN_LDFLAGS= -no-undefined -shared -module -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol

libgs32eb1_core_la_LDFLAGS += $(WIN_LDFLAGS)
libls132r_core_la_LDFLAGS += $(WIN_LDFLAGS)
endif

DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS @COMMON_CFLAGS@
INCLUDES = -I./common -I$(top_srcdir)/utils/debugger -I$(top_srcdir)/device/uart @COMMON_INCLUDES@
