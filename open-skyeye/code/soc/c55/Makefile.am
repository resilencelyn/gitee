c55 = c55x/skyeye_mach_c55.c   c55x/skyeye_mach_c55_module.c
c5510 = c5510/skyeye_mach_c5510.c   c5510/skyeye_mach_c5510_module.c

libc55_la_SOURCES = $(c55)
libc55_la_LDFLAGS = -module

libc5510_la_SOURCES = $(c5510)
libc5510_la_LDFLAGS = -module
if WIN32
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
libc55_la_LDFLAGS += $(WIN32_LD_COMMON)
libc5510_la_LDFLAGS += $(WIN32_LD_COMMON)
endif

pkglib_LTLIBRARIES = libc55.la
pkglib_LTLIBRARIES += libc5510.la

INCLUDES = -I$(top_srcdir)/common @COMMON_INCLUDES@




