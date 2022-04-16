sopc = sopc/skyeye_mach_sopc.c   sopc/skyeye_mach_sopc_module.c

libsopc_la_SOURCES = $(sopc)
libsopc_la_LDFLAGS = -module

if WIN32
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
libsopc_la_LDFLAGS += $(WIN32_LD_COMMON)
endif

pkglib_LTLIBRARIES = libsopc.la

INCLUDES = -I$(top_srcdir)/common @COMMON_INCLUDES@




