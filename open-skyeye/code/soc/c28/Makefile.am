c2812 = c2812/skyeye_mach_c2812.c   c2812/skyeye_mach_c2812_module.c
f28335 = f28335/skyeye_mach_f28335.c   f28335/skyeye_mach_f28335_module.c

libc2812_la_SOURCES = $(c2812)
libc2812_la_LDFLAGS = -module

libf28335_la_SOURCES = $(f28335)
libf28335_la_LDFLAGS = -module

if WIN32
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
libc2812_la_LDFLAGS += $(WIN32_LD_COMMON)
libf28335_la_LDFLAGS += $(WIN32_LD_COMMON)
endif

pkglib_LTLIBRARIES = libc2812.la
pkglib_LTLIBRARIES += libf28335.la

INCLUDES = -I$(top_srcdir)/common @COMMON_INCLUDES@




