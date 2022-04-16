base_mach = base/skyeye_mach_base_module.c base/skyeye_mach_base.c

libbase_mach_la_SOURCES = $(base_mach)
libbase_mach_la_LDFLAGS = -module

pkglib_LTLIBRARIES = libbase_mach.la
if WIN32
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
libbase_mach_la_LDFLAGS += $(WIN32_LD_COMMON)
endif
DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 @COMMON_CFLAGS@

INCLUDES = -I$(top_srcdir)/common @COMMON_INCLUDES@

