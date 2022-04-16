x86_cpu = ./cpu/resolver.cc ./cpu/data_xfer8.cc ./cpu/io.cc ./cpu/rdrand.cc \
./cpu/vmcs.cc ./cpu/protect_ctrl.cc ./cpu/logical16.cc ./cpu/string.cc ./cpu/crregs.cc  \
./cpu/fetchdecode.cc ./cpu/fetchdecode64.cc ./cpu/access2.cc  ./cpu/bit32.cc ./cpu/shift64.cc\
./cpu/apic.cc  ./cpu/ctrl_xfer16.cc ./cpu/xsave.cc ./cpu/disasm.cc ./cpu/arith32.cc\
./cpu/bcd.cc ./cpu/vm8086.cc ./cpu/shift8.cc ./cpu/stack32.cc ./cpu/call_far.cc\
./cpu/cpu.cc ./cpu/aes.cc ./cpu/vapic.cc ./cpu/data_xfer32.cc ./cpu/3dnow.cc ./cpu/tasking.cc\
./cpu/segment_ctrl_pro.cc ./cpu/mult32.cc ./cpu/shift16.cc ./cpu/sha.cc ./cpu/bit.cc ./cpu/logical64.cc\
./cpu/cpuid.cc ./cpu/crc32.cc ./cpu/stack.cc ./cpu/avx/avx512_rcp14.cc ./cpu/avx/avx2.cc ./cpu/avx/avx512_conflict.cc\
./cpu/avx/avx_pfp.cc ./cpu/avx/avx_cvt.cc ./cpu/avx/avx.cc ./cpu/avx/avx512_move.cc ./cpu/avx/tbm32.cc\
./cpu/avx/avx512_pfp.cc ./cpu/avx/tbm64.cc ./cpu/avx/avx512_rsqrt14.cc ./cpu/avx/xop.cc ./cpu/avx/gather.cc\
./cpu/avx/avx512_mask32.cc ./cpu/avx/avx512_mask16.cc ./cpu/avx/avx512_mask64.cc ./cpu/avx/avx512_mask8.cc\
./cpu/avx/avx512_cvt.cc ./cpu/avx/avx512_fma.cc ./cpu/avx/avx512.cc ./cpu/avx/avx_fma.cc ./cpu/sse_move.cc\
./cpu/bit16.cc ./cpu/ctrl_xfer64.cc ./cpu/init.cc ./cpu/bmi64.cc ./cpu/bit64.cc ./cpu/logical32.cc ./cpu/exception.cc\
./cpu/arith8.cc ./cpu/logical8.cc ./cpu/sse_pfp.cc ./cpu/generic_cpuid.cc ./cpu/event.cc ./cpu/vmexit.cc\
./cpu/sse_rcp.cc ./cpu/arith64.cc ./cpu/flag_ctrl.cc ./cpu/fpu_emu.cc ./cpu/msr.cc ./cpu/data_xfer16.cc ./cpu/mmx.cc\
./cpu/stack64.cc ./cpu/smm.cc ./cpu/jmp_far.cc ./cpu/ctrl_xfer32.cc ./cpu/paging.cc ./cpu/shift32.cc ./cpu/access.cc\
./cpu/debugstuff.cc ./cpu/proc_ctrl.cc ./cpu/mult8.cc ./cpu/stack16.cc ./cpu/icache.cc ./cpu/arith16.cc\
./cpu/ctrl_xfer_pro.cc ./cpu/vmfunc.cc ./cpu/sse_string.cc ./cpu/load.cc ./cpu/sse.cc ./cpu/bmi32.cc ./cpu/mult16.cc\
./cpu/fpu/fpu.cc ./cpu/fpu/softfloat-specialize.cc ./cpu/fpu/softfloat.cc ./cpu/fpu/fsincos.cc ./cpu/fpu/fyl2x.cc\
./cpu/fpu/ferr.cc ./cpu/fpu/softfloat16.cc ./cpu/fpu/softfloat-muladd.cc ./cpu/fpu/fpu_load_store.cc ./cpu/fpu/fpu_misc.cc\
./cpu/fpu/fpu_cmov.cc ./cpu/fpu/softfloat-round-pack.cc ./cpu/fpu/fpu_arith.cc ./cpu/fpu/poly.cc ./cpu/fpu/fpu_const.cc\
./cpu/fpu/fpatan.cc ./cpu/fpu/fpu_compare.cc ./cpu/fpu/fprem.cc ./cpu/fpu/fpu_trans.cc ./cpu/fpu/softfloatx80.cc\
./cpu/fpu/f2xm1.cc ./cpu/iret.cc ./cpu/segment_ctrl.cc ./cpu/data_xfer64.cc ./cpu/soft_int.cc ./cpu/flag_ctrl_pro.cc\
./cpu/cpudb/p2_klamath.cc ./cpu/cpudb/corei5_lynnfield_750.cc ./cpu/cpudb/pentium_mmx.cc ./cpu/cpudb/corei5_arrandale_m520.cc\
./cpu/cpudb/p3_katmai.cc ./cpu/cpudb/corei7_haswell_4770.cc ./cpu/cpudb/corei7_ivy_bridge_3770K.cc ./cpu/cpudb/athlon64_venice.cc\
./cpu/cpudb/turion64_tyler.cc ./cpu/cpudb/broadwell_ult.cc ./cpu/cpudb/core2_penryn_t9600.cc ./cpu/cpudb/trinity_apu.cc\
./cpu/cpudb/pentium.cc ./cpu/cpudb/athlon64_clawhammer.cc ./cpu/cpudb/amd_k6_2_chomper.cc ./cpu/cpudb/phenomx3_8650_toliman.cc\
./cpu/cpudb/core_duo_t2400_yonah.cc ./cpu/cpudb/p4_prescott_celeron_336.cc ./cpu/cpudb/corei7_sandy_bridge_2600K.cc\
./cpu/cpudb/zambezi.cc ./cpu/cpudb/atom_n270.cc ./cpu/cpudb/p4_willamette.cc ./cpu/ret_far.cc ./cpu/vmx.cc ./cpu/svm.cc\
./cpu/mult64.cc 

x86_common = ./common/osdep.cc  ./common/crc.cc ./common/logio.cc ./common/config.cc\
./common/load32bitOShack.cc ./common/pc_system.cc ./common/plugin.cc ./common/x86_arch_interface.cc\
./common/gdbstub.cc

x86_memory = memory/memory.cc memory/misc_mem.cc

x86_iodev = ./iodev/parallel.cc ./iodev/dma.cc ./iodev/unmapped.cc ./iodev/keyboard.cc ./iodev/scancodes.cc\
./iodev/floppy.cc ./iodev/virt_timer.cc ./iodev/devices.cc ./iodev/acpi.cc ./iodev/busmouse.cc ./iodev/serial_raw.cc\
./iodev/iodebug.cc ./iodev/sound/opl.cc ./iodev/sound/soundosx.cc ./iodev/sound/soundsdl.cc ./iodev/sound/soundlow.cc\
./iodev/sound/soundalsa.cc ./iodev/sound/soundlnx.cc ./iodev/sound/soundwin.cc ./iodev/sound/es1370.cc ./iodev/sound/soundmod.cc\
./iodev/sound/sb16.cc ./iodev/sound/soundfile.cc ./iodev/usb/usb_uhci.cc ./iodev/usb/usb_common.cc ./iodev/usb/usb_hid.cc\
./iodev/usb/scsi_device.cc ./iodev/usb/usb_printer.cc ./iodev/usb/usb_ohci.cc ./iodev/usb/usb_xhci.cc ./iodev/usb/usb_msd.cc\
./iodev/usb/usb_hub.cc ./iodev/pic.cc ./iodev/pci_ide.cc ./iodev/biosdev.cc ./iodev/pci.cc ./iodev/speaker.cc\
./iodev/pcidev.cc ./iodev/extfpuirq.cc ./iodev/pci2isa.cc ./iodev/ioapic.cc ./iodev/serial.cc ./iodev/slowdown_timer.cc\
./iodev/pit.cc  ./iodev/harddrv.cc ./iodev/cmos.cc ./iodev/display/vgacore.cc ./iodev/display/vga.cc\
./iodev/display/voodoo.cc ./iodev/display/svga_cirrus.cc ./iodev/hdimage/vvfat.cc\
./iodev/hdimage/hdimage.cc ./iodev/hdimage/vmware4.cc ./iodev/hdimage/cdrom.cc ./iodev/hdimage/vpc-img.cc ./iodev/hdimage/cdrom_misc.cc\
./iodev/hdimage/vbox.cc ./iodev/hdimage/vmware3.cc ./iodev/hdimage/cdrom_osx.cc ./iodev/hdimage/cdrom_win32.cc\
./iodev/network/eth_vnet.cc ./iodev/network/eth_vde.cc ./iodev/network/eth_tap.cc \
./iodev/network/eth_fbsd.cc ./iodev/network/eth_slirp.cc ./iodev/network/ne2k.cc ./iodev/network/ne2k2.cc ./iodev/network/pcipnic.cc\
./iodev/network/eth_null.cc  ./iodev/network/e1000.cc ./iodev/network/eth_tuntap.cc\
./iodev/network/slirp/sbuf.cc ./iodev/network/slirp/tcp_timer.cc ./iodev/network/slirp/tcp_input.cc ./iodev/network/slirp/compat.cc\
./iodev/network/slirp/cksum.cc ./iodev/network/slirp/tcp_subr.cc ./iodev/network/slirp/ip_input.cc ./iodev/network/slirp/mbuf.cc\
./iodev/network/slirp/socket.cc ./iodev/network/slirp/dnssearch.cc ./iodev/network/slirp/udp.cc ./iodev/network/slirp/tftp.cc\
./iodev/network/slirp/misc.cc ./iodev/network/slirp/bootp.cc ./iodev/network/slirp/slirp.cc ./iodev/network/slirp/ip_output.cc\
./iodev/network/slirp/if.cc ./iodev/network/slirp/tcp_output.cc ./iodev/network/slirp/arp_table.cc ./iodev/network/slirp/ip_icmp.cc\
./iodev/network/netmod.cc ./iodev/pit82c54.cc iodev/check_device.cc\
./iodev/bu_61580.cc ./iodev/x86_fifo.cc ./iodev/x86_sja1000.cc ./iodev/x86_can_fifo.cc\
./iodev/x86_gpio.cc ./iodev/x86_ad.cc ./iodev/x86_rs422.cc ./iodev/x86_rs422_fifo.cc

x86_gui =  ./gui/gtk_enh_dbg_osdep.cc ./gui/carbon.cc ./gui/rfb.cc ./gui/wx.cc ./gui/textconfig.cc\
./gui/wxdialog.cc ./gui/term.cc ./gui/paramtree.cc ./gui/keymap.cc ./gui/sdl.cc ./gui/vncsrv.cc \
./gui/siminterface.cc ./gui/wxmain.cc ./gui/svga.cc  ./gui/gui.cc ./gui/amigaos.cc \
 ./gui/scrollwin.cc ./gui/nogui.cc ./gui/sdl2.cc 

x86_mach = mach/skyeye_mach_pc.c mach/pc_module.c
if WIN32
x86_iodev += ./iodev/network/eth_win32.cc
x86_gui +=  gui/win32dialog.cc gui/win32.cc   gui/enh_dbg.cc gui/win32_enh_dbg_osdep.cc gui/win32paramdlg.cc
bochs =-Xlinker -lmingw32  -no-undefined -shared -module  -Xlinker -L../../common/.libs/ -Xlinker -lcommon -Xlinker -lmsys-1.0 -Xlinker -lmsvcrt  -Xlinker -lkernel32 -Xlinker -lole32 -Xlinker -lkernel32 -Xlinker -lgdi32 -Xlinker -luuid -Xlinker -luser32  -Xlinker -lshell32 -Xlinker --export-all-symbol -Xlinker -lComctl32 -Xlinker -lcomdlg32
mach_la =

else
x86_iodev +=  ./iodev/network/eth_linux.cc
x86_gui += gui/x.cc
bochs = -lXpm -lstdc++  -lX11 -lXrandr
mach_la = -lstdc++
endif

x86_disasm = disasm/dis_decode.cc  disasm/resolve.cc \
disasm/dis_groups.cc  disasm/syntax.cc

x86_core = x86_core/x86_core.cc x86_core/x86_module.c

##
libx86_core_la_SOURCES =  $(x86_cpu) $(x86_common) $(x86_memory) $(x86_iodev) $(x86_gui) $(x86_disasm) $(x86_core)
libx86_core_la_LDFLAGS = $(bochs) -module

libpc_mach_la_SOURCES = $(x86_mach)
libpc_mach_la_LDFLAGS = $(mach_la) -module

if WIN32
WIN32_LD_DEVICE = -no-undefined -shared -module -Xlinker -L../../common/.libs/  -Xlinker -lcommon -L/mingw/lib  -Xlinker -L/mingw/ -Xlinker --export-all-symbol

libpc_mach_la_LDFLAGS +=  $(WIN32_LD_DEVICE)
endif


pkglib_LTLIBRARIES = libx86_core.la libpc_mach.la
DEFS = -g -fpermissive -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0 
INCLUDES = -I$(srcdir)/common -I$(srcdir)/cpu -I$(srcdir)/include -I$(srcdir)/ -I$(srcdir)/memory -I$(srcdir)/instrument/stubs -I$(srcdir)/iodev/  @COMMON_INCLUDES@
