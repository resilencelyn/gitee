c6474 = c6474/skyeye_mach_c6474.c   c6474/skyeye_mach_c6474_module.c
c6678 = c6678/skyeye_mach_c6678.c   c6678/skyeye_mach_c6678_module.c
c6713 = c6713/skyeye_mach_c6713.c   c6713/skyeye_mach_c6713_module.c
c6415 = c6415/skyeye_mach_c6415.c   c6415/skyeye_mach_c6415_module.c
c6455 = c6455/skyeye_mach_c6455.c   c6455/skyeye_mach_c6455_module.c
c6457 = c6457/skyeye_mach_c6457.c   c6457/skyeye_mach_c6457_module.c
c6416 = c6416/skyeye_mach_c6416.c   c6416/skyeye_mach_c6416_module.c
c6446 = c6446/skyeye_mach_c6446.c   c6446/skyeye_mach_c6446_module.c
c6748 = c6748/skyeye_mach_c6748.c   c6748/skyeye_mach_c6748_module.c
c6746 = c6746/skyeye_mach_c6746.c   c6746/skyeye_mach_c6746_module.c

libc6474_la_SOURCES = $(c6474)
libc6474_la_LDFLAGS = -module

libc6678_la_SOURCES = $(c6678)
libc6678_la_LDFLAGS = -module

libc6713_la_SOURCES = $(c6713)
libc6713_la_LDFLAGS = -module

libc6415_la_SOURCES = $(c6415)
libc6415_la_LDFLAGS = -module

libc6455_la_SOURCES = $(c6455)
libc6455_la_LDFLAGS = -module

libc6457_la_SOURCES = $(c6457)
libc6457_la_LDFLAGS = -module

libc6416_la_SOURCES = $(c6416)
libc6416_la_LDFLAGS = -module

libc6446_la_SOURCES = $(c6446)
libc6446_la_LDFLAGS = -module

libc6748_la_SOURCES = $(c6748)
libc6748_la_LDFLAGS = -module

libc6746_la_SOURCES = $(c6746)
libc6746_la_LDFLAGS = -module
if WIN32
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
libc6474_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6678_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6713_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6415_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6455_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6457_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6416_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6446_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6748_la_LDFLAGS += $(WIN32_LD_COMMON)
libc6746_la_LDFLAGS += $(WIN32_LD_COMMON)
endif

pkglib_LTLIBRARIES = libc6474.la
pkglib_LTLIBRARIES += libc6678.la
pkglib_LTLIBRARIES += libc6713.la
pkglib_LTLIBRARIES += libc6415.la
pkglib_LTLIBRARIES += libc6455.la
pkglib_LTLIBRARIES += libc6457.la
pkglib_LTLIBRARIES += libc6416.la
pkglib_LTLIBRARIES += libc6446.la
pkglib_LTLIBRARIES += libc6748.la
pkglib_LTLIBRARIES += libc6746.la

INCLUDES = -I$(top_srcdir)/common @COMMON_INCLUDES@
