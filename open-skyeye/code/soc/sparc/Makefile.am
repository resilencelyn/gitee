leon2 = leon2/leon2_module.c leon2/skyeye_mach_leon2.c

libleon2_la_SOURCES = $(leon2)
libleon2_la_LDFLAGS = -module

pkglib_LTLIBRARIES = libleon2.la
if WIN32
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
libleon2_la_LDFLAGS += $(WIN32_LD_COMMON)
endif
DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 @COMMON_CFLAGS@

INCLUDES = -I$(top_srcdir)/common @COMMON_INCLUDES@

tsc695f = tsc695f/tsc695f_module.c tsc695f/skyeye_mach_tsc695f.c

libtsc695f_la_SOURCES = $(tsc695f)
libtsc695f_la_LDFLAGS = -module

pkglib_LTLIBRARIES += libtsc695f.la
if WIN32
WIN32_LD_COMMON += -no-undefined -shared -Xlinker -L../../common/.libs/ -L/mingw/lib -Xlinker -lcommon -Xlinker --export-all-symbol
libtsc695f_la_LDFLAGS += $(WIN32_LD_COMMON)
endif
DEFS += -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 @COMMON_CFLAGS@

INCLUDES += -I$(top_srcdir)/common @COMMON_INCLUDES@
