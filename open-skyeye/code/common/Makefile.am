# we will generate libcommon.so, and set its attribute to RTLD_LAZY|RTLD_GLOBAL) when use dlopen to load it.
common_checkpoint = checkpoint/check.c checkpoint/checkdumpobj.cpp checkpoint/checkpoint.cpp checkpoint/attrjson.cpp checkpoint/codebase64.c checkpoint/attrtojson.c
common_breakpoint = breakpoint/breakpoint.c
common_misc = misc/support.c misc/exec_info.c misc/skyeye_list.cpp  misc/skyeye_common_class.c misc/file_copy.cpp misc/asnprintf.c misc/tinyxml2.cpp misc/fifo_queue.c
if WIN32
common_misc += misc/win_udp_socket.c
endif

common_module = module/skyeye_module.c
common_ctrl = ctrl/thread_ctrl.c ctrl/sim_ctrl.c ctrl/step.c ctrl/scheduler.c ctrl/skyeye_thread.c ctrl/skyeye_cell.c ctrl/skyeye_exec.c ctrl/skyeye_io.c ctrl/skyeye_system.c ctrl/systemc_exec.c
common_conf_parser = conf_parser/skyeye_options.c conf_parser/skyeye_config.c conf_parser/misc_options.c conf_parser/conf_obj.c conf_parser/skyeye_class.c conf_parser/skyeye_interface.c conf_parser/skyeye_attr.c conf_parser/skyeye_conf_map.cpp conf_parser/skyeye_attribute.cpp conf_parser/skyeye_iface.cpp
common_log = log/Ring_fifo.c log/skyeye_log.c log/skyeye_log_group.cpp
common_loader = loader/loader_elf.c loader/loader_file.c loader/loader_coff.c loader/loader_elf_by_space.c loader/loader_hex.c
common_mm = mm/skyeye_mm.c
common_cli = cli/skyeye_command.c cli/skyeye_cli.c cli/default_command.c cli/com_command.c cli/skyeye_new_api.c
common_lic = lic/verify.c lic/lic_local.c lic/lic_network.c lic/lic_dog.c
common_portable = portable/mman.c portable/usleep.c portable/gettimeofday.c
common_preference = preference/skyeye_pref.c
common_profile = profile/symbol.c profile/bfd_target.c profile/skyeye_stat.c
common_memory = bus/bank_ops.c  bus/io.c  bus/ram.c bus/flash.c bus/skyeye_bus.c bus/bus_recoder.c bus/addr_space.c bus/mm_monitor.c bus/memory_monitor.cpp
common_fault_inject = fault_inject/fault_inject.c
common_core = core/skyeye_arch.c core/skyeye_core.cpp
common_device = device/skyeye_device.c device/pen_buffer.c device/skyeye_uart_ops.c device/skyeye_signal.c device/skyeye_register.c device/skyeye_serial.c
common_mach = mach/skyeye_mach.c
common_callback = callback/callback.c callback/global_callback.c
common_disas= disas/disas.c disas/arm-dis.c
common_dyncom= dyncom/translate_singlestep_bb.cpp dyncom/translate_singlestep.cpp dyncom/translate_all.cpp dyncom/translate.cpp dyncom/timings.cpp dyncom/tag.cpp dyncom/stat.cpp dyncom/sha1.cpp dyncom/optimize.cpp dyncom/interface.cpp dyncom/function.cpp dyncom/frontend.cpp dyncom/fp.cpp dyncom/disasm.cpp dyncom/basicblock.cpp dyncom/tlb.cpp dyncom/phys_page.cpp dyncom/profiler.cpp
common_json_conf = json_conf/json_attr.c json_conf/json_base.c json_conf/json_conf.c json_conf/json_parser.c  json_conf/json_file.c
common_disassemble = disassemble/skyeye_disassemble.c
common_fault_injection = fault_injection/fault_injection.c
common_parse_symbol = parse_symbol/parse_symbol.cpp
common_return_msg = return_msg/return_msg.c

pkglib_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = $(common_module) $(common_misc) $(common_breakpoint) $(common_ctrl) $(common_portable) $(common_preference) $(common_core) $(common_conf_parser) $(common_log) $(common_cli) $(common_lic) $(common_mm) $(common_mach) $(common_device) $(common_memory) $(common_loader) $(common_callback) $(common_profile) $(common_checkpoint) $(common_disas) $(common_json_conf) $(common_disassemble) $(common_fault_injection) $(common_parse_symbol) $(common_return_msg)

libcommon_la_LIBADD =

#if LLVM_EXIST
#libcommon_la_SOURCES += $(common_dyncom)
#endif

libcommon_la_LDFLAGS = @JSON_LIBS@
#libcommon_la_LDFLAGS += `pkg-config --libs`
if WIN32
libcommon_la_LDFLAGS += -no-undefined -shared -Xlinker /local/third-party/lib/libbfd.a -Xlinker /local/third-party/lib/libltdlc.a -Xlinker /local/third-party/lib/libiberty.a -Xlinker --export-all-symbol   -Xlinker /mingw/libs/libpython35.a -lws2_32
libcommon_la_LDFLAGS += -L$(top_srcdir)/third-party/license/lib_win -lRSADecrypt
else
#libcommon_la_LIBADD += -L$(top_builddir)/third-party/prebuilt/lib -lbfd -liberty -lltdl
libcommon_la_LIBADD += -L$(prefix)/lib -lbfd -liberty -lltdl
libcommon_la_LDFLAGS += `python3m-config --libs`
endif
# we will link the common module with main
#libcommon_la_LDFLAGS = -module

skyeye_module_dir = \"$(prefix)/lib/skyeye\"
skyeye_bin = \"$(prefix)/bin/\"

DEFS = -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 -DSKYEYE_MODULE_DIR=$(skyeye_module_dir) -DSKYEYE_BIN=$(skyeye_bin) @COMMON_CFLAGS@ -std=gnu++0x

#INCLUDES =  @COMMON_INCLUDES@ -I$(top_srcdir)/third-party/include/ -I$(top_srcdir)/third-party/bfd/ -I$(top_builddir)/third-party/bfd/ -I$(top_srcdir)/third-party @INCLTDL@
if WIN32
INCLUDES =  @COMMON_INCLUDES@ @JSON_CFLAGS@ -I${top_srcdir}/libltdl  -I/local/third-party/include/ -I$(top_srcdir)/third-party/license/lib_win  `python3m-config --cflags`
else
INCLUDES =  @COMMON_INCLUDES@ @JSON_CFLAGS@ -I$(top_srcdir)/third-party/include/ -I$(top_srcdir)/third-party -I${top_srcdir}/libltdl  `python3m-config --cflags` -I/usr/include/python3.5m
endif
#if LLVM_EXIST
#INCLUDES+= @LLVM_CFLAGS@
#if WIN32
#libcommon_la_LDFLAGS += -Xlinker `echo "@LLVM_LDFLAGS@" | sed 's/ / -Xlinker /g'` -L@LLVM_LIBDIR@
#else
#libcommon_la_LDFLAGS += @LLVM_LDFLAGS@ -lrt -L@LLVM_LIBDIR@
#endif #win32
#endif

if LCD
INCLUDES += @LCD_CFLAGS@
endif 
