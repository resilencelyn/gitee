
#SUBDIRS =  arch/arm/ arch/bfin arch/coldfire arch/mips arch/ppc/ arch/x86/ device/ utils/ arch/sparc/ libltdl common/ soc/arm/ 
SUBDIRS =  common/
bin_PROGRAMS = 

if BUILD_ANDROID
include Makefile.android
endif

SUBDIRS +=  device/ utils/
if BUILD_ARM
SUBDIRS += arch/arm
SUBDIRS += soc/arm
endif

if BUILD_BFIN
SUBDIRS += arch/bfin
endif


if BUILD_COLDFIRE
SUBDIRS += arch/coldfire
endif


if BUILD_MIPS
SUBDIRS += arch/mips
endif


if BUILD_SPARC
SUBDIRS += arch/sparc
SUBDIRS += soc/sparc
endif


if BUILD_PPC
SUBDIRS += arch/ppc
SUBDIRS +=soc/ppc
endif


if BUILD_X86
SUBDIRS += arch/x86
endif


if BUILD_C28
SUBDIRS += arch/c28 soc/c28
endif

if BUILD_C55
SUBDIRS += arch/c55 soc/c55
endif

if BUILD_C54
SUBDIRS += arch/c54 soc/c54
endif

if BUILD_TRICORE
SUBDIRS += arch/tricore
endif

if BUILD_C6KPLUS
SUBDIRS += arch/c6kplus
endif

if BUILD_DEFAULT
SUBDIRS += arch/ppc arch/arm soc/arm arch/sparc  soc/c6k soc/sparc arch/c28 soc/c28 arch/c55 soc/c55 arch/c54 soc/c54 arch/x86  arch/c6kplus soc/c6kplus soc/ppc soc/base_mach soc/sopc arch/mips arch/tricore
default_targets=arm, ppc, sparc, c28, c55, c54, x86, c6kplus, mips, tricore
endif

if BUILD_ALL
SUBDIRS += arch/arm/ soc/arm arch/bfin arch/coldfire arch/mips arch/ppc/ arch/x86/ arch/sparc  soc/c6k soc/sparc arch/c28 soc/c28 arch/c55 soc/c55 arch/c54 soc/c54 arch/c6kplus soc/ppc soc/base_mach soc/sopc arch/tricore
endif

DIST_SUBDIRS = $(SUBDIRS)


#AM_LDFLAGS = $(LIBLS) --start-group $(top_srcdir)/arch/arm/libarm.a $(top_srcdir)/arch/bfin/libbfin.a $(top_srcdir)/arch/coldfire/libcoldfire.a $(top_srcdir)/arch/mips/libmips.a $(top_srcdir)/arch/ppc/libppc.a $(top_srcdir)/device/libdev.a $(top_srcdir)/utils/libutils.a --end-group
#AM_LDFLAGS = $(LIBLS) -L$(top_srcdir)/arch/arm/ -L$(top_srcdir)/arch/bfin/ -L$(top_srcdir)/arch/coldfire/ -L$(top_srcdir)/arch/mips/ -L$(top_srcdir)/device/ -L$(top_srcdir)/utils/ --start-group  -larm -lbfin -lcoldfire  -lmips -L$(top_srcdir)/arch/ppc/ -lppc -ldev -lutils --end-group

BACKTRACE=-L./utils/ -lbacktrace

skyeye_LDADD =  -L./common -lcommon $(BACKTRACE) @JSON_LIBS@ @PYTHON_LIBS@
skyeye_ui_LDADD =  -L./common -lcommon $(BACKTRACE)
skyeye_common_LDADD =  -L./common -lcommon $(BACKTRACE)
#skyeye_LDADD =  
#skyeye_LDADD =  $(top_srcdir)/arch/arm/libarm.a $(top_srcdir)/device/libdev.a $(top_srcdir)/arch/mips/libmips.a $(top_srcdir)/arch/ppc/libppc.a $(top_srcdir)/arch/bfin/libbfin.a $(top_srcdir)/arch/mips/libmips.a $(top_srcdir)/arch/coldfire/libcoldfire.a $(top_srcdir)/arch/sparc/libsparc.a $(top_srcdir)/memory/libmemory.a $(top_srcdir)/third-party/bfd/libbfd.a $(top_srcdir)/third-party/libiberty/libiberty.a $(top_srcdir)/third-party/readline/libreadline.a

# workaround getopt issue on Cygwin platform
#skyeye_LDADD = -lc 
# add curses library for ubentu system.
# skyeye_LDADD += -lncurses

# readline library need ncurses

# some functions in "libutils.a" which is required by other libs
#skyeye_LDADD += $(top_srcdir)/utils/libutils.a
#skyeye_LDADD += 
#skyeye_LDFLAGS = -L./common -lcommon
#if LLVM_EXIST
##skyeye_LDFLAGS += `llvm-config --libs ` -lrt -L`llvm-config --libdir`
#endif

INCLUDES = @COMMON_INCLUDES@ -Ithird-party/bfd/ -Iutils/code_cov/ @PYTHON_INCLUDE@

DEFS = -DMODET -g -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 @COMMON_CFLAGS@  @PYTHON_CFLAGS@
if WIN32
AM_CFLAGS = -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wl,--large-address-aware
else
AM_CFLAGS = -W -Wall -Wstrict-prototypes -Wmissing-prototypes 
endif
ACLOCAL_AMFLAGS=-I m4

bin_PROGRAMS += skyeye
skyeye_SOURCES = utils/main/skyeye.c


bin_PROGRAMS += skyeye_ui
skyeye_ui_SOURCES = utils/main/skyeye_gui.c

bin_PROGRAMS += skyeye_common
skyeye_common_SOURCES = utils/main/skyeye_common.c

if SYSTEMC_ENABLE
INCLUDES += -Iutils/systemc/ -Iutils/systemc/bus -Iutils/systemc/device -I$(SYSTEMC_DIR)/include/
DEFS += -DSC_INCLUDE_DYNAMIC_PROCESSES  -DSC_USE_PTHREADS -DSYSTEMC_ENABLE
bin_PROGRAMS += skyeye_systemc
skyeye_systemc_SOURCES = utils/systemc/skyeye_systemc.cpp utils/systemc/bus/lt_bus_target.cpp utils/systemc/bus/lt_bus_initiator.cpp utils/systemc/device/lt_core_initiator.cpp utils/systemc/device/lt_mem_target.cpp utils/systemc/device/lt_uart_target.cpp  utils/systemc/sc_skyeye_arm.cpp utils/systemc/sc_init.cpp  utils/systemc/lt_top.cpp utils/systemc/skyeye_device.cpp
skyeye_systemc_LDFLAGS = -L$(SYSTEMC_DIR)/lib-linux64 -lsystemc -L$(top_builddir)/common -lcommon

INCLUDES += -Iutils/new_systemc/at_1_phase/include  -Iutils/new_systemc/common/include 
bin_PROGRAMS += skyeye_new_systemc
skyeye_new_systemc_SOURCES = utils/new_systemc/common/src/apb_cpu_bridge.cpp \
			 utils/new_systemc/common/src/sc_cpu.cpp  \
			 utils/new_systemc/common/src/memory.cpp  \
			 utils/new_systemc/common/src/traffic_generator.cpp \
			 utils/new_systemc/common/src/report.cpp \
			 utils/new_systemc/common/src/select_initiator.cpp \
			 utils/new_systemc/common/src/at_target_1_phase.cpp \
			 utils/new_systemc/at_1_phase/src/apb_cpu_bridge_test_device.cpp \
			 utils/new_systemc/at_1_phase/src/at_1_phase_top.cpp \
			 utils/new_systemc/at_1_phase/src/initiator_top.cpp \
			 utils/new_systemc/at_1_phase/src/device.cpp \
			 utils/new_systemc/init.cpp \
			 utils/new_systemc/skyeye_new_systemc.cpp 

skyeye_new_systemc_LDFLAGS = -L$(SYSTEMC_DIR)/lib-linux64 -lsystemc -L$(top_builddir)/common -lcommon

endif

if WIN32
bin_PROGRAMS += uart_instance prof_convert zlg_can_console
uart_instance_SOURCES = utils/uart_console/uart_console.c
zlg_can_console_SOURCES = utils/zlg_can_console/zlg_can_console.c
zlg_can_console_LDADD = -L$(top_srcdir)/third-party/libcan/lib_win -lcontrolcan
mknandflashdump_SOURCES =
prof_convert_SOURCES = utils/code_cov/prof_convert.c
uart_instance_LDADD = -lgw32c
else
bin_PROGRAMS += uart_instance mknandflashdump prof_convert zlg_can_console
uart_instance_SOURCES = utils/uart_console/uart_console.c
zlg_can_console_SOURCES = utils/zlg_can_console/zlg_can_console.c
mknandflashdump_SOURCES = utils/nandflash_dump/mknandflashdump.c
prof_convert_SOURCES = utils/code_cov/prof_convert.c
endif
if LCD
#skyeye_LDADD += @LCD_LIBS@
endif
if LINUX
#all:third-party/libiberty/libiberty.a third-party/bfd/.libs/libbfd.so third-party/opcodes/.libs/libopcodes.so
endif

bin_PROGRAMS += bugreport
bugreport_SOURCES = utils/bugreport/skyeye_bugreport.c
if WIN32
bugreport_LDFLAGS = /mingw/libs/libpython35.a -g
else
bugreport_LDFLAGS = `python3m-config --libs`
endif

skyeye_gui:
	cd gui/qt4_src && rm -f skyeye_gui && $(MAKE)
	cp -f gui/qt4_src/skyeye-gui $(prefix)/bin/

#third-party/bfd/.libs/libbfd.so:
#	mkdir -p third-party/bfd/ && cd third-party/bfd/ && ./../../$(top_srcdir)/third-party/bfd/configure --prefix=$(prefix) --disable-werror --enable-shared --enable-targets=all;$(MAKE);

#third-party/libiberty/libiberty.a:
#	mkdir -p third-party/libiberty && cd third-party/libiberty && ./../../$(srcdir)/third-party/libiberty/configure --prefix=$(prefix) --enable-shared ;$(MAKE);

#third-party/opcodes/.libs/libopcodes.so:
#	mkdir -p third-party/opcodes/ && cd third-party/opcodes/ && ./../../$(top_srcdir)/third-party/opcodes/configure --prefix=$(prefix) --disable-werror --enable-shared --enable-targets=all;$(MAKE);

#libltdl/.lib/libltdl.a:
#	mkdir -p libltdl/ && cd libltdl/ && ../$(top_srcdir)/libltdl/configure;$(MAKE);

lib:
if WIN32
else
#	mkdir -p third-party/libiberty && cd third-party/libiberty && ./../../$(srcdir)/third-party/libiberty/configure --prefix=$(prefix) --enable-shared ;$(MAKE);
#	mkdir -p third-party/bfd/ && cd third-party/bfd/ && ./../../$(top_srcdir)/third-party/bfd/configure --prefix=$(prefix) --disable-werror --enable-shared --enable-targets=all;$(MAKE);
#	mkdir -p third-party/opcodes/ && cd third-party/opcodes/ && ./../../$(top_srcdir)/third-party/opcodes/configure --prefix=$(prefix) --disable-werror --enable-shared --enable-targets=all;$(MAKE);
#	mkdir -p libltdl/ && cd libltdl/ && ../$(top_srcdir)/libltdl/configure;$(MAKE);
endif
#cd $(top_srcdir)/third-party/bfd/ && ./configure --prefix=$(prefix) --disable-werror --enable-shared --enable-targets=all;make;
#cd $(top_srcdir)/third-party/opcodes/ && ./configure --prefix=$(prefix) --disable-werror --enable-shared --enable-targets=all;make;
#cd $(top_srcdir)/third-party/readline/ && ./configure --prefix=$(prefix) --enable-shared;make;

build_lib:
#	cd $(top_builddir)/third-party/libiberty && $(MAKE);
#	cd $(top_builddir)/third-party/bfd/ && $(MAKE);
#	cd $(top_builddir)third-party/opcodes/ && $(MAKE);
#	cd $(top_builddir)/libltdl/ && make;

install_lib:
if WIN32
#	cp $(top_builddir)/third-party/bfd/.libs/libbfd.dll.a  $(prefix)/lib/skyeye/
#	cp $(top_builddir)/third-party/bfd/.libs/libbfd.la $(prefix)/lib/skyeye/
#	cp $(top_builddir)/third-party/bfd/.libs/libbfd-2-18-50-20070907.dll  $(prefix)/lib/skyeye/
else
#	cd $(top_builddir)/third-party/libiberty && $(MAKE) install
#	cd $(top_builddir)/third-party/bfd/ && $(MAKE) install
#	cd $(top_builddir)/third-party/opcodes/ && $(MAKE) install
	mkdir -p $(prefix)
	cp -r -d $(top_srcdir)/third-party/prebuilt/*  $(prefix)/ && ldconfig -n $(prefix)/lib
if LINUX_64
	cp -r -d $(top_srcdir)/third-party/sdl2.0/lib_linux/lib* $(prefix)/lib && ldconfig -n $(prefix)/lib
	cp -r -d $(top_srcdir)/third-party/libusb1.0/lib_linux/libusb-1.0.so* $(prefix)/lib && ldconfig -n $(prefix)/lib
else
	cp -r -d $(top_srcdir)/third-party/sdl2.0/lib_linux32/lib* $(prefix)/lib && ldconfig -n $(prefix)/lib
	cp -r -d $(top_srcdir)/third-party/libusb1.0/lib_linux32/libusb-1.0.so* $(prefix)/lib && ldconfig -n $(prefix)/lib
endif

endif

clean_lib:
	cd $(top_builddir)/third-party/libiberty && make distclean
	cd $(top_builddir)/third-party/bfd/ && make distclean
	cd $(top_builddir)/third-party/opcodes/ && make distclean
	cd $(top_builddir)/libltdl/ && make distclean;
install-data-hook:
	rm -f -r $(prefix)/testsuite && mkdir $(prefix)/testsuite
if WIN32
	mkdir -p $(prefix)/testsuite/arm_hello && mkdir -p $(prefix)/bin
	mv $(prefix)/lib/skyeye/libpytimer-0.dll $(prefix)/bin
	mv $(prefix)/lib/skyeye/libbacktrace-0.dll $(prefix)/bin
	cp -a $(top_builddir)/common/.libs/libcommon-0.dll $(prefix)/lib/skyeye/
	cp -a $(top_builddir)/common/.libs/libcommon-0.dll $(prefix)/bin/
	cp -a $(top_srcdir)/third-party/sdl2.0/lib_win/SDL2.dll $(prefix)/bin/
	cp -a $(top_srcdir)/third-party/libusb1.0/lib_win/libusb-1.0.dll $(prefix)/bin/
	cp -a $(top_srcdir)/third-party/drmingw/lib_win32/exchndl.dll $(prefix)/bin/
	cp -a $(top_srcdir)/third-party/drmingw/lib_win32/mgwhelp.dll $(prefix)/bin/
if BUILD_ANDROID
	cp -a $(top_builddir)/android/objs/emulator_common/.libs/libemulator_common-0.dll $(prefix)/bin/
	cp -a $(top_builddir)/android/objs/emulator_libui/.libs/libemulator_libui-0.dll $(prefix)/bin/
endif
endif
	cp -a $(top_srcdir)/testsuite/arm_hello $(prefix)/testsuite/
	cp -a $(top_srcdir)/testsuite/sparc_hello $(prefix)/testsuite/

	cp -a $(top_srcdir)/utils/pycli/*.py $(prefix)/bin/
	cp -a $(top_srcdir)/utils/pycli/*.def $(prefix)/bin/
	cp -a $(top_srcdir)/utils/pycli/*.json $(prefix)/bin/
	cp -a $(top_srcdir)/utils/pycli/picture $(prefix)/bin/
	cp -a $(top_srcdir)/utils/pycli/doc $(prefix)/bin/
	cp -a $(top_srcdir)/utils/pycli/command $(prefix)/bin/
	cp -a $(top_srcdir)/utils/pycli/grpc_package $(prefix)/bin/
	cp -a $(top_srcdir)/kerneldlls $(prefix)/bin
	cp -a $(top_srcdir)/common/include $(prefix)/include
	mkdir -p $(prefix)/bin/soc
	rm -f -r $(prefix)/lib/skyeye/*.a $(prefix)/lib/skyeye/*.la
#	rm -f -r $(prefix)/conf && mkdir $(prefix)/conf
#	cp -a $(top_srcdir)/conf/* $(prefix)/conf

# workaround for bfd library. we need to reconfigure it.
all-local:
#	cd gui/qt4_src && $(MAKE) $(AM_MAKEFLAGS) all
#	cd third-party/bfd/ && ./configure --prefix=$(prefix) --disable-werror --enable-shared --enable-targets=all;make;
#	cd third-party/opcodes/ && ./configure --prefix=$(prefix) --disable-werror --enable-shared --enable-targets=all;make;make install
install-exec-local:
#	cd third-party/bfd/ && make install
#	cd third-party/opcodes/ && make install
