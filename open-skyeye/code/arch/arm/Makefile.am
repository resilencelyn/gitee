arm_comm = common/arm_arch_interface.c common/armcopro.c common/armemu.c common/arminit.c common/armmmu.c common/armos.c common/armsupp.c common/armvirt.c common/thumbemu.c common/skyeye_ram.c common/arch64_exec_insn.c common/arch64_exception.c common/arch64_vfp_insn.c common/arch64_helper.c common/arch64_memory.c
#arm_systemc = common/systemc/lt_target.cpp common/systemc/lt_top.cpp common/systemc/lt_transform.cpp common/systemc/sc_skyeye_arm.cpp common/systemc/sc_init.cpp
arm_systemc = common/systemc/lt_target.cpp common/systemc/lt_transform.cpp common/systemc/sc_skyeye_arm.cpp common/systemc/sc_init.cpp
arm_mmu = common/mmu/arm7100_mmu.c common/mmu/arm920t_mmu.c common/mmu/arm926ejs_mmu.c  common/mmu/arm1176jzf_s_mmu.c common/mmu/cache.c common/mmu/maverick.c common/mmu/rb.c common/mmu/sa_mmu.c common/mmu/tlb.c common/mmu/wb.c common/mmu/xscale_copro.c common/mmu/cortex_a9_mmu.c common/mmu/cortex_a8_mmu.c common/mpu/cortex_r4_mpu.c common/mmu/ftc662_mmu.c
arm_dbct = dbct/arm2x86.c dbct/arm2x86_coproc.c dbct/arm2x86_dp.c dbct/arm2x86_mem.c dbct/arm2x86_movl.c dbct/arm2x86_mul.c dbct/arm2x86_other.c dbct/arm2x86_psr.c dbct/arm2x86_shift.c dbct/arm2x86_test.c dbct/tb.c 
arm_mach = \
mach/skyeye_mach_lpc.c
#arm_dyncom = dyncom/arm_dyncom_interface.cpp dyncom/arm_disasm.cpp dyncom/arm_dyncom_translate.cpp dyncom/arm_dyncom_run.cpp dyncom/arm_dyncom_dec.cpp dyncom/arm_dyncom_mmu.cpp dyncom/arm_dyncom_parallel.cpp dyncom/arm_dyncom_interpreter.cpp dyncom/arm_dyncom_thumb.cpp dyncom/arm_step_diff.cpp dyncom/arm_dyncom_memory.cpp dyncom/arm_arch64_dyncom_translate.cpp  dyncom/arm_arch64_dyncom_run.cpp dyncom/arm_arch64_dyncom_dec.cpp dyncom/arm_arch64_dyncom_instr.cpp
arm_vfp = common/vfp/vfp.c common/vfp/vfpsingle.c common/vfp/vfpdouble.c
arm_arm11 = arm11/arm11_core.c
arm_cortex_m3 = cortex_m3/cortex_m3_core.c
arm_cortex_a9 = cortex_a9/cortex_a9_core.c
arm_cortex_a8 = cortex_a8/cortex_a8_core.c
arm920t = arm920t/arm920t_core.c
arm926ejs = arm926ejs/arm926ejs_core.c
arm_cortex_r4 = cortex_r4/cortex_r4_core.c
ftc662 = ftc662/ftc662_core.c
libarm_la_SOURCES = $(arm_comm) $(arm_mmu) $(arm_mach) $(arm_vfp) $(arm_arm11) $(arm_cortex_a9) $(arm_cortex_a8) $(arm_cortex_m3)
libarm_la_SOURCES += $(arm920t)
libarm_la_SOURCES += $(arm926ejs)
libarm_la_SOURCES += $(arm_cortex_r4)
libarm_la_SOURCES += $(ftc662)

libarm_la_SOURCES += common/arm_module.c
#if LLVM_EXIST
#libarm_la_SOURCES += $(arm_dyncom)
#endif

if DBCT
libarm_la_SOURCES += $(arm_dbct)
endif
libarm_la_LDFLAGS = -module

WIN_LDFLAGS = -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -lmsys-1.0 -Xlinker -lmsvcrt -Xlinker -lmsys-1.0 -Xlinker -lkernel32 -Xlinker -lole32 -Xlinker -lkernel32 -Xlinker -lgdi32 -Xlinker -luuid -Xlinker -luser32  -Xlinker -lshell32 -Xlinker --export-all-symbol
if WIN32
libarm_la_LDFLAGS += $(WIN_LDFLAGS)
endif

cortex_m0 = cortex_m0/cortex_m0_core.c cortex_m0/cortex_m0_module.c

libcortex_m0_la_SOURCES = $(arm_comm) $(arm_mmu) $(arm_mach) $(arm_vfp) $(cortex_m0)
#if LLVM_EXIST
#libcortex_m0_la_SOURCES += $(arm_dyncom)
#endif

libcortex_m0_la_LDFLAGS = -module
libcortex_mo_la_CFLAGS = -DCORTEX_M0
if WIN32
libcortex_m0_la_LDFLAGS += $(WIN_LDFLAGS)
endif
libcortex_m0_la_LIBADD = armemu32.lo

pkglib_LTLIBRARIES = libarm.la libcortex_m0.la
#noinst_LIBRARIES =      libarm.a
DEFS = -DMODET -g -O3 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -DNO_LCD @COMMON_CFLAGS@
#if LLVM_EXIST
#DEFS += -DLLVM_EXIST
#endif

libarm_la_LIBADD = armemu32.lo
INCLUDES = -I$(top_srcdir)/arch/arm/common -I$(top_srcdir)/arch/arm/mach/ @COMMON_INCLUDES@ -I$(top_srcdir)/third-party/include/
#if LLVM_EXIST
#INCLUDES += @LLVM_CFLAGS@
#endif

# Add systemc support
if SYSTEMC_ENABLE
#libarm_la_SOURCES += $(arm_systemc)
#libarm_la_LDFLAGS += -L$(SYSTEMC_DIR)/lib-linux64 -lsystemc
#INCLUDES += -I$(SYSTEMC_DIR)/include
#DEFS += -DSC_INCLUDE_DYNAMIC_PROCESSES  -DSC_USE_PTHREADS -DSYSTEMC_ENABLE
endif
armemu32.lo:common/armemu.c armemu.lo
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -DMODE32 $(DEFS) $(INCLUDES) -g -O2 -MT armemu32.lo -MD -MP -MF .deps/armemu32.Tpo -c -o armemu32.lo `test -f '$(srcdir)/common/armemu.c' || echo `$(srcdir)/common/armemu.c
#	$(CC) -DMODE32 $(DEFS) $(INCLUDES) common/armemu.c -c -o $@
