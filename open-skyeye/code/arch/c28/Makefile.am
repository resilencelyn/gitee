c28_comm = common/c28_arch_interface.c  common/exec_insn.c common/c28x_core.c common/c28x_insn_iface.c
pkglib_LTLIBRARIES = 
DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS @COMMON_CFLAGS@
INCLUDES = -I./common @COMMON_INCLUDES@

#c2812 core
c2812_core = c2812_core/c2812_core.c c2812_core/c2812_module.c
libc2812_core_la_SOURCES = $(c28_comm) $(c2812_core)
libc2812_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libc2812_core.la

if WIN32
libc2812_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
endif

#f28335 core
f28335_core = f28335_core/f28335_core.c f28335_core/f28335_module.c
libf28335_core_la_SOURCES = $(c28_comm) $(f28335_core)
libf28335_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libf28335_core.la

if WIN32
libf28335_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
endif
