#utils_config = config/skyeye_config.c  config/skyeye_options.c

utils_debugger = debugger/arch_regdefs.c debugger/gdbserver.c debugger/gdb_tracepoint.c  debugger/skyeye2gdb.c debugger/gdbserver_module.c debugger/arm_regdefs.c debugger/bfin_regdefs.c debugger/cf_regdefs.c debugger/mips_regdefs.c debugger/ppc_regdefs.c debugger/sparc_regdefs.c debugger/ti_c64x_regdefs.c debugger/ti_c28x_regdefs.c debugger/x86_regdefs.c

utils_disassembler = disassembler/dis_asm.c disassembler/dis_asm_module.c
utils_disassembler_new = disassembler/disassemble.c disassembler/symbol.c disassembler/bfd_target.c disassembler/disassemble_module.c

utils_code_cov = code_cov/code_cov.c code_cov/code_cov_module.c

utils_log = log/log.c log/log_module.c

utils_bus_log = bus_log/bus_log.c bus_log/bus_log_module.c bus_log/bus_ops.c

utils_pmon = perf_monitor/pmon.c perf_monitor/pmon_module.c

utils_shutdown = shutdown/shutdown_module.c

utils_os = os_supp/os_module.c  os_supp/skyeye_os.c

utils_systemc = systemc/lt_target.cpp systemc/lt_transform.cpp systemc/sc_skyeye_arm.cpp systemc/sc_init.cpp systemc/systemc_module.c systemc/lt_top.cpp

utils_pytimer = pytimer/pytimer.c

if WIN32
utils_backtrace = bugreport/backtrace_windows.c
else
utils_backtrace = bugreport/backtrace_linux.c
endif

libgdbserver_la_SOURCES = $(utils_debugger)
libgdbserver_la_LDFLAGS = -module

libcodecov_la_SOURCES = $(utils_code_cov)   
libcodecov_la_LDFLAGS = -module

liblog_la_SOURCES = $(utils_log)   
liblog_la_LDFLAGS = -module

#libpmon_la_SOURCES = $(utils_pmon)   
#libpmon_la_LDFLAGS = -module

#libbus_log_la_SOURCES = $(utils_bus_log)
#libbus_log_la_LDFLAGS = -module

libshutdown_la_SOURCES = $(utils_shutdown)
libshutdown_la_LDFLAGS = -module

libdisasm_la_SOURCES = $(utils_disassembler)
libdisasm_la_LDFLAGS = -module

libdisasem_la_SOURCES = $(utils_disassembler_new)
libdisasem_la_LDFLAGS = -module

libos_supp_la_SOURCES = $(utils_os)
libos_supp_la_LDFLAGS = -module

pkglib_LTLIBRARIES = libcodecov.la liblog.la libshutdown.la libos_supp.la  libgdbserver.la
utils_fault_inject = fault_inject/fault_inject.c fault_inject/fault_inject_module.c
libfault_inject_la_SOURCES = $(utils_fault_inject)
libfault_inject_la_LDFLAGS = -module
pkglib_LTLIBRARIES += libfault_inject.la


#libsystemc_la_SOURCES = $(utils_systemc)

libpytimer_la_SOURCES = $(utils_pytimer)
libpytimer_la_LDFLAGS = -module

libbacktrace_la_SOURCES = $(utils_backtrace)
libbacktrace_la_LDFLAGS = -module

if WIN32
WIN32_LD_COMMON = -no-undefined -shared -Xlinker -L../common/.libs/ -L/mingw/lib  -L/mingw/third-party-lib -Xlinker -lcommon -Xlinker --export-all-symbol
libgdbserver_la_LDFLAGS += $(WIN32_LD_COMMON)
#libbus_log_la_LDFLAGS += $(WIN32_LD_COMMON)
libdisasm_la_LDFLAGS += -Xlinker /local/third-party/lib/libopcodes.a -Xlinker /local/third-party/lib/libbfd.a -Xlinker /local/third-party/lib/libiberty.a -Xlinker --export-all-symbol $(WIN32_LD_COMMON) 
libdisasem_la_LDFLAGS += -Xlinker /local/third-party/lib/libopcodes.a -Xlinker /local/third-party/lib/libbfd.a -Xlinker /local/third-party/lib/libiberty.a -Xlinker --export-all-symbol $(WIN32_LD_COMMON)
libos_supp_la_LDFLAGS += $(WIN32_LD_COMMON)
#libpmon_la_LDFLAGS += $(WIN32_LD_COMMON)
libshutdown_la_LDFLAGS += $(WIN32_LD_COMMON)
libcodecov_la_LDFLAGS  += $(WIN32_LD_COMMON)
liblog_la_LDFLAGS += $(WIN32_LD_COMMON)
libfault_inject_la_LDFLAGS += $(WIN32_LD_COMMON)
libpytimer_la_LDFLAGS += $(WIN32_LD_COMMON)
#libbacktrace_la_LDFLAGS += -Xlinker /local/third-party/lib/libbfd.a -Xlinker --export-all-symbol -Xlinker --enable-auto-image-base $(WIN32_LD_COMMON)
libbacktrace_la_CFLAGS = -I $(top_srcdir)/third-party/drmingw/include
libbacktrace_la_LDFLAGS += $(WIN32_LD_COMMON)
else
#libdisasm_la_LDFLAGS += -L./$(top_builddir)/third-party/prebuilt/lib/  -lopcodes -lbfd -liberty
#libdisasem_la_LDFLAGS += -L./$(top_builddir)/third-party/prebuilt/lib/ -lopcodes -lbfd -liberty
libdisasm_la_LDFLAGS += -L$(prefix)/lib/  -lopcodes -lbfd -liberty
libdisasem_la_LDFLAGS += -L$(prefix)/lib/ -lopcodes -lbfd -liberty
endif

#pkglib_LTLIBRARIES = libutils.la libgdbserver.la

pkglib_LTLIBRARIES += libdisasm.la libdisasem.la libpytimer.la
pkglib_LTLIBRARIES += libbacktrace.la
skyeye_bin = \"$(prefix)/bin/\"

DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -DSKYEYE_BIN=$(skyeye_bin) @COMMON_CFLAGS@

if WIN32
INCLUDES = -I/local/third-party/include/ @COMMON_INCLUDES@ `python3m-config --cflags`
else
INCLUDES =  -I$(top_srcdir)/third-party/include/ -I$(top_srcdir)/third-party/prebuilt/include  -I$(top_builddir)/third-party/prebuilt/include -I$(top_srcdir)/common @COMMON_INCLUDES@ `python3m-config --cflags` -I/usr/include/python3.5m/
endif

if SYSTEMC_ENABLE
#libarm_la_LDFLAGS += -L$(SYSTEMC_DIR)/lib-linux64 -lsystemc
#libsystemc_la_LDFLAGS = -L$(SYSTEMC_DIR)/lib-linux64 -lsystemc
#INCLUDES += -I$(SYSTEMC_DIR)/include
#DEFS += -DSC_INCLUDE_DYNAMIC_PROCESSES  -DSC_USE_PTHREADS -DSYSTEMC_ENABLE
#pkglib_LTLIBRARIES += libsystemc.la
endif
