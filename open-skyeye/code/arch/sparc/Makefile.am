
sparc_mach = mach/leon2_timer_unit.c mach/leon2_uart.c mach/mach_leon2_io.c mach/skyeye_mach_leon2.c mach/leon2_mcfg.c mach/leon2_cfg.c mach/leon2_irqctrl.c 
sparc_comm = common/sparc_arch_interface.c common/sparc_core.c common/traps.c common/iu.cpp common/helper.cpp common/tcg-target.c common/fpu.cpp
sparc_insn = instructions/i_add.cpp instructions/i_addcc.cpp instructions/i_addx.cpp instructions/i_addxcc.cpp instructions/i_and.cpp instructions/i_andcc.cpp instructions/i_andn.cpp	\
			 instructions/i_andncc.cpp instructions/i_mulscc.cpp	\
			 instructions/i_ba.cpp instructions/i_bcc.cpp instructions/i_bcs.cpp instructions/i_be.cpp instructions/i_bg.cpp instructions/i_bge.cpp instructions/i_bgu.cpp instructions/i_bl.cpp 	\
			 instructions/i_ble.cpp instructions/i_bleu.cpp instructions/i_bn.cpp instructions/i_bne.cpp instructions/i_bneg.cpp instructions/i_bpos.cpp instructions/i_bvc.cpp instructions/i_bvs.cpp	\
			 instructions/i_call.cpp instructions/i_flush.cpp instructions/i_jmpl.cpp instructions/i_ld.cpp instructions/i_ldd.cpp instructions/i_lddf.cpp instructions/i_ldf.cpp	\
			 instructions/i_ldsb.cpp instructions/i_ldsh.cpp instructions/i_ldub.cpp instructions/i_lduh.cpp instructions/i_nop.cpp instructions/i_unimp.cpp	\
			 instructions/i_or.cpp instructions/i_orcc.cpp instructions/i_orn.cpp instructions/i_rdpsr.cpp instructions/i_rdtbr.cpp instructions/i_rdwim.cpp instructions/i_rdy.cpp	\
			 instructions/i_restore.cpp instructions/i_rett.cpp instructions/i_save.cpp instructions/i_sethi.cpp instructions/i_sll.cpp instructions/i_smul.cpp	instructions/i_umul.cpp	\
			 instructions/i_sra.cpp instructions/i_srl.cpp instructions/i_stb.cpp instructions/i_st.cpp instructions/i_std.cpp instructions/i_stdf.cpp instructions/i_stf.cpp instructions/i_stfsr.cpp	\
			 instructions/i_sth.cpp instructions/i_sub.cpp instructions/i_subcc.cpp instructions/i_subx.cpp instructions/i_subxcc.cpp instructions/i_ta.cpp	\
			 instructions/i_udiv.cpp instructions/i_utils.cpp instructions/i_wrpsr.cpp instructions/i_wrtbr.cpp instructions/i_wrwim.cpp instructions/i_wry.cpp	\
			 instructions/i_xnor.cpp instructions/i_xor.cpp instructions/i_xorcc.cpp instructions/i_orncc.cpp \
			 instructions/i_stdfq.cpp instructions/i_casa.cpp instructions/i_swapa.cpp

sparc_insn += instructions/i_sdiv.cpp instructions/i_sdivcc.cpp instructions/i_tsubcc.cpp instructions/i_tsubcctv.cpp instructions/i_taddcc.cpp instructions/i_taddcctv.cpp instructions/i_smulcc.cpp instructions/i_umulcc.cpp instructions/i_udivcc.cpp instructions/i_xnorcc.cpp instructions/i_tcc.cpp instructions/i_tneg.cpp instructions/i_tne.cpp instructions/i_tleu.cpp instructions/i_ldfsr.cpp instructions/i_fdivd.cpp instructions/i_fcmpd.cpp instructions/i_fcmps.cpp instructions/i_fcmpes.cpp instructions/i_fbne.cpp instructions/i_fbug.cpp instructions/i_fbuge.cpp instructions/i_fbule.cpp instructions/i_fbo.cpp instructions/i_fbe.cpp instructions/i_fstod.cpp instructions/i_fstoi.cpp instructions/i_swap.cpp instructions/i_sta.cpp instructions/i_lda.cpp instructions/i_lduha.cpp

sparc_insn +=  instructions/i_fmovs.cpp instructions/i_fdivs.cpp instructions/i_fnegs.cpp instructions/i_fcmped.cpp instructions/i_fdtos.cpp instructions/i_fmuld.cpp instructions/i_fmuls.cpp instructions/i_fsubd.cpp instructions/i_fsubs.cpp instructions/i_faddd.cpp instructions/i_fadds.cpp  instructions/i_fitod.cpp instructions/i_fitos.cpp instructions/i_fsqrtd.cpp instructions/i_fbge.cpp instructions/i_fbl.cpp instructions/i_fble.cpp instructions/i_fbg.cpp instructions/i_fbul.cpp instructions/i_fabss.cpp instructions/i_fdtoi.cpp

sparc_mmu = common/mmu/mmu.c
sparc_v8_core = sparc_v8/sparc_v8.c sparc_v8/sparc_v8_module.c 
#sparc_dyncom = dyncom/sparc_dyncom_interface.cpp dyncom/sparc_dyncom_run.cpp \
			   dyncom/sparc_dyncom_translate.cpp dyncom/sparc_dyncom_trap.cpp \
			   dyncom/mmu/mmu_dyncom.cpp dyncom/sparc_dyncom_tlb.cpp
libsparc_v8_la_SOURCES = $(sparc_comm) $(sparc_insn) $(sparc_mmu) $(sparc_v8_core)

#if LLVM_EXIST
#libsparc_v8_la_SOURCES += $(sparc_dyncom)
#DEFS += -DLLVM_EXIST
#endif


# we will link the common module with main
libsparc_v8_la_LDFLAGS = -module

pkglib_LTLIBRARIES = libsparc_v8.la

if WIN32
libsparc_v8_la_LDFLAGS += -no-undefined -shared -module -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -lmsvcrt -Xlinker -lkernel32 -Xlinker -lole32 -Xlinker -lkernel32 -Xlinker -lgdi32 -Xlinker -luuid -Xlinker -luser32  -Xlinker -lshell32 -Xlinker --export-all-symbol
endif

sparc_v7_core = sparc_v7/sparc_v7.c sparc_v7/sparc_v7_module.c
libsparc_v7_la_SOURCES = $(sparc_comm) $(sparc_insn) $(sparc_mmu) $(sparc_v7_core)

#if LLVM_EXIST
#libsparc_v7_la_SOURCES += $(sparc_dyncom)
#DEFS += -DLLVM_EXIST
#endif


# we will link the common module with main
libsparc_v7_la_LDFLAGS = -module

pkglib_LTLIBRARIES += libsparc_v7.la

if WIN32
libsparc_v7_la_LDFLAGS += -no-undefined -shared -module -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -lmsvcrt -Xlinker -lkernel32 -Xlinker -lole32 -Xlinker -lkernel32 -Xlinker -lgdi32 -Xlinker -luuid -Xlinker -luser32  -Xlinker -lshell32 -Xlinker --export-all-symbol
endif
#DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0
#DEFS = -g -O2 -DSPARC_DEBUG -DSPARC_ENABLE_STAT
#DEFS = -g -O2 -DSPARC_DEBUG
DEFS += -g -O2 @COMMON_CFLAGS@


INCLUDES = -I$(top_srcdir)/utils/share -I$(top_srcdir)/utils/debugger -I$(top_srcdir)/utils/profile/ -I$(top_srcdir)/arch/sparc/ -I$(top_srcdir)/arch/sparc/common -I$(top_srcdir)/arch/sparc/dyncom -Icommon/ -Imach -I$(top_srcdir)/utils/debugger -I$(top_srcdir)/device/uart -I$(top_srcdir)/device/flash @COMMON_INCLUDES@
#INCLUDES = -I./common -I$(top_srcdir)/utils/config/ -I$(top_srcdir)/utils/profile/ -I$(top_srcdir)/utils/debugger -I$(top_srcdir)/device/uart @COMMON_INCLUDES@
#if LLVM_EXIST
#INCLUDES += @LLVM_CFLAGS@
#endif
