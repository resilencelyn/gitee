c54x_comm = common/exec_insn.c common/c54x_generic_core.c
pkglib_LTLIBRARIES = 
DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS @COMMON_CFLAGS@
INCLUDES = -I./common @COMMON_INCLUDES@

#c54x core
c54x_core = c54x_core/c54x_core.c c54x_core/c54x_module.c
libc54x_core_la_SOURCES = $(c54x_comm) $(c54x_core)
libc54x_core_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libc54x_core.la
if WIN32
libc54x_core_la_LDFLAGS += -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -L/mingw/lib --export-all-symbol
endif

