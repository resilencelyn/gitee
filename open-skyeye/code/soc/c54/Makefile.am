c5416 = c5416/skyeye_mach_c5416.c   c5416/skyeye_mach_c5416_module.c

libc5416_la_SOURCES = $(c5416)
libc5416_la_LDFLAGS = -module
if WIN32
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
libc5416_la_LDFLAGS += $(WIN32_LD_COMMON)
endif

pkglib_LTLIBRARIES = libc5416.la

INCLUDES = -I$(top_srcdir)/common @COMMON_INCLUDES@




