
pkglib_LTLIBRARIES =
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 @COMMON_CFLAGS@

INCLUDES = -I$(top_srcdir)/common @COMMON_INCLUDES@

mpc8245 = mpc8245/mpc8245_module.c mpc8245/mpc8245.c
libmpc8245_la_SOURCES = $(mpc8245)
libmpc8245_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libmpc8245.la
if WIN32
libmpc8245_la_LDFLAGS += $(WIN32_LD_COMMON)
endif

mpc5121 = mpc5121/mpc5121_module.c mpc5121/mpc5121.c
libmpc5121_la_SOURCES = $(mpc5121)
libmpc5121_la_LDFLAGS = -module

mpc8560 = mpc8560/mpc8560_module.c mpc8560/skyeye_mach_mpc8560.c mpc8560/mpc8560_boot_linux.c
libmpc8560_la_SOURCES = $(mpc8560)
libmpc8560_la_LDFLAGS = -module

mpc8572 = mpc8572/mpc8572_module.c mpc8572/skyeye_mach_mpc8572.c mpc8572/mpc8572_boot_linux.c
libmpc8572_la_SOURCES = $(mpc8572)
libmpc8572_la_LDFLAGS = -module

mpc8641d = mpc8641d/mpc8641d_module.c mpc8641d/skyeye_mach_mpc8641d.c mpc8641d/mpc8641d_boot_linux.c mpc8641d/mpc8641d_boot_vxworks.c
libmpc8641d_la_SOURCES = $(mpc8641d)
libmpc8641d_la_LDFLAGS = -module

p1010 = p1010/p1010_module.c p1010/skyeye_mach_p1010.c
libp1010_la_SOURCES = $(p1010)
libp1010_la_LDFLAGS = -module

mpc8378 = mpc8378/mpc8378_module.c mpc8378/mpc8378.c 
libmpc8378_la_SOURCES = $(mpc8378)
libmpc8378_la_LDFLAGS = -module

t2080 = t2080/t2080.c t2080/t2080_module.c
libt2080_la_SOURCES = $(t2080)
libt2080_la_LDFLAGS = -module

mpc755 = mpc755/mpc755.c mpc755/mpc755_module.c
libmpc755_la_SOURCES = $(mpc755)
libmpc755_la_LDFLAGS = -module

t4240 = t4240/t4240.c t4240/t4240_module.c
libt4240_la_SOURCES = $(t4240)
libt4240_la_LDFLAGS = -module

pkglib_LTLIBRARIES += libmpc5121.la
pkglib_LTLIBRARIES += libmpc8560.la
pkglib_LTLIBRARIES += libmpc8572.la
pkglib_LTLIBRARIES += libmpc8641d.la
pkglib_LTLIBRARIES += libp1010.la
pkglib_LTLIBRARIES += libmpc8378.la
pkglib_LTLIBRARIES += libt2080.la
pkglib_LTLIBRARIES += libmpc755.la
pkglib_LTLIBRARIES += libt4240.la
if WIN32
libmpc5121_la_LDFLAGS += $(WIN32_LD_COMMON)
libmpc8560_la_LDFLAGS += $(WIN32_LD_COMMON)
libmpc8572_la_LDFLAGS += $(WIN32_LD_COMMON)
libmpc8641d_la_LDFLAGS += $(WIN32_LD_COMMON)
libp1010_la_LDFLAGS += $(WIN32_LD_COMMON)
libmpc8378_la_LDFLAGS += $(WIN32_LD_COMMON)
libt2080_la_LDFLAGS += $(WIN32_LD_COMMON)
libmpc755_la_LDFLAGS += $(WIN32_LD_COMMON)
libt4240_la_LDFLAGS += $(WIN32_LD_COMMON)
endif



