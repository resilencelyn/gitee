ppc_comm = common/ppc_alu.c common/ppc_cpu.c common/ppc_exc.c  common/ppc_mmu.c    common/ppc_vec.c \
common/ppc_arch_interface.c  common/ppc_dec.c       common/ppc_fpu.c  common/ppc_opc.c \
common/ppc_io.c   common/ppc_tools.c   common/ppc_spe.c  common/ppc_alu_vle.c   common/ppc_opc_vle.c  \
common/ppc_mmu_vle.c
#ppc_dyncom = dyncom/ppc_dyncom_run.cpp dyncom/ppc_dyncom_translate.cpp dyncom/ppc_dyncom_interface.cpp dyncom/ppc_dyncom_instr_group1.cpp dyncom/ppc_dyncom_instr_group2.cpp dyncom/ppc_dyncom_instr_group_f1.cpp dyncom/ppc_dyncom_instr_group_f2.cpp dyncom/ppc_dyncom_instr_main.cpp dyncom/ppc_dyncom_instr_groupv.cpp dyncom/ppc_dyncom_dec.cpp dyncom/ppc_dyncom_diff_log.cpp dyncom/ppc_dyncom_parallel.cpp dyncom/mmu/mmu_dyncom.cpp dyncom/ppc_dyncom_tlb.cpp dyncom/ppc_dyncom_instr_misc.cpp dyncom/ppc_dyncom_instr_spe.cpp

ppc_linux_syscall = linux_syscall/ppc_syscall.c

ppc_e600 = e600/e600.c e600/e600_module.c
libe600_core_la_SOURCES = $(ppc_comm) $(ppc_linux_syscall) $(ppc_e600)
#if LLVM_EXIST
#libe600_core_la_SOURCES += $(ppc_dyncom)
#endif

ppc_mpc603e = mpc603e/g2_core.c  mpc603e/g2_core_module.c
libmpc603e_core_la_SOURCES = $(ppc_comm) $(ppc_linux_syscall) $(ppc_mpc603e)
#if LLVM_EXIST
#libmpc603e_core_la_SOURCES += $(ppc_dyncom)
#endif

ppc_e300 = e300/e300.c e300/e300_module.c
libe300_core_la_SOURCES = $(ppc_comm) $(ppc_linux_syscall) $(ppc_e300)
#if LLVM_EXIST
#libe300_core_la_SOURCES += $(ppc_dyncom)
#endif

ppc_e500 = e500/e500.c e500/e500_module.c
libe500_core_la_SOURCES = $(ppc_comm) $(ppc_linux_syscall) $(ppc_e500)
#if LLVM_EXIST
#libe500_core_la_SOURCES += $(ppc_dyncom)
#endif

ppc_e200 = e200/e200.c e200/e200_module.c
libe200_core_la_SOURCES = $(ppc_comm) $(ppc_linux_syscall) $(ppc_e200)
#if LLVM_EXIST
#libe200_core_la_SOURCES += $(ppc_dyncom)
#endif

pkglib_LTLIBRARIES = libe600_core.la
pkglib_LTLIBRARIES += libmpc603e_core.la
pkglib_LTLIBRARIES += libe300_core.la
pkglib_LTLIBRARIES += libe500_core.la
pkglib_LTLIBRARIES += libe200_core.la
# we will link the common module with main
libe600_core_la_LDFLAGS = -module
libmpc603e_core_la_LDFLAGS = -module
libe300_core_la_LDFLAGS = -module
libe500_core_la_LDFLAGS = -module
libe200_core_la_LDFLAGS = -module
if WIN32
WIN_LDFLAGS= -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -lmsys-1.0 -Xlinker -lmsvcrt -Xlinker -lmsys-1.0 -Xlinker -lkernel32 -Xlinker -lole32 -Xlinker -lkernel32 -Xlinker -lgdi32 -Xlinker -luuid -Xlinker -luser32  -Xlinker -lshell32 -Xlinker --export-all-symbol

libe600_core_la_LDFLAGS += $(WIN_LDFLAGS)
libmpc603e_core_la_LDFLAGS += $(WIN_LDFLAGS)
libe300_core_la_LDFLAGS += $(WIN_LDFLAGS)
libe500_core_la_LDFLAGS += $(WIN_LDFLAGS)
libe200_core_la_LDFLAGS += $(WIN_LDFLAGS)
endif

DEFS = -DMODET -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0
DEFS += @COMMON_CFLAGS@
#if LLVM_EXIST
#DEFS += -DLLVM_EXIST
#endif

INCLUDES = -I$(top_srcdir)/common -I$(top_srcdir)/dyncom/ -I$(top_srcdir)/arch/ppc/common -I$(top_srcdir)/arch/ppc/linux_syscall -I$(top_srcdir)/utils/debugger @COMMON_INCLUDES@
#if LLVM_EXIST
#INCLUDES += @LLVM_CFLAGS@
#endif
