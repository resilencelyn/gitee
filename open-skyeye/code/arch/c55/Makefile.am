c55x_comm = common/exec_insn.c common/c55x_core.c
pkglib_LTLIBRARIES = 
DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS @COMMON_CFLAGS@
INCLUDES = -I./common @COMMON_INCLUDES@

#c5510 core
c5510_core = c5510_core/c5510_core.c c5510_core/c5510_module.c
libc5510_core_la_SOURCES = $(c55x_comm) $(c5510_core)
libc5510_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libc5510_core.la
if WIN32
libc5510_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
endif

