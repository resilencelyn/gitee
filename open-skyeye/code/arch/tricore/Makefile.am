tricore_comm = common/exec_insn.c common/cpu.c common/helper.c
pkglib_LTLIBRARIES =
DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS @COMMON_CFLAGS@
INCLUDES = -I./common @COMMON_INCLUDES@

#TriCore cpu
tricore_cpu = tricore_cpu/tricore_cpu.c tricore_cpu/tricore_cpu_module.c
libtricore_cpu_la_SOURCES = $(tricore_comm) $(tricore_cpu)
libtricore_cpu_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libtricore_cpu.la

if WIN32
libtricore_cpu_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
endif
