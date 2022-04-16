DEFS = -DMODET -g -O2 -D_FILE_OFFSET_BITS=64 -DSTANDALONE -DDEFAULT_INLINE=0  @COMMON_CFLAGS@

skyeye_bin_dir = \"$(prefix)/bin/\"
INCLUDES = -I./ @COMMON_INCLUDES@ -DSKYEYE_BIN=$(skyeye_bin_dir)
if WIN32
INCLUDES += -I$(top_srcdir)/third-party/sdl2.0/lib_win/SDL2
INCLUDES += -I$(top_srcdir)/third-party/libusb1.0/lib_win
INCLUDES += -I$(top_srcdir)/third-party/SESync/lib_win
else

if LINUX_64
INCLUDES += -I$(top_srcdir)/third-party/sdl2.0/lib_linux/SDL2
INCLUDES += -I$(top_srcdir)/third-party/libusb1.0/lib_linux
else
INCLUDES += -I$(top_srcdir)/third-party/sdl2.0/lib_linux32/SDL2
INCLUDES += -I$(top_srcdir)/third-party/libusb1.0/lib_linux32
endif

endif


INCLUDES += @LCD_CFLAGS@
if WIN32
INCLUDES +=-I/mingw/gtk-2.0/include/gtk-2.0/
INCLUDES +=-I/mingw/gtk-2.0/include/glib-2.0/
INCLUDES +=-I/mingw/gtk-2.0/include/cairo/
INCLUDES +=-I/mingw/gtk-2.0/include/pango-1.0/
INCLUDES +=-I/mingw/gtk-2.0/include/atk-1.0/
INCLUDES +=-I/mingw/gtk-2.0/lib/gtk-2.0/include/
INCLUDES +=-I/mingw/gtk-2.0/lib/glib-2.0/include/

else
INCLUDES += -I/usr/include/gtk-2.0/ -I/usr/include/glib-2.0/
endif

INCLUDES += -I$(top_srcdir)/android/
INCLUDES += -I$(top_srcdir)
INCLUDES += -I$(top_srcdir)/android/skyeye/

if WIN32
WIN32_LD_DEVICE = -no-undefined -shared -module -Xlinker -L../common/.libs/  -Xlinker -lcommon -L/mingw/lib -Xlinker -L$(top_builddir) -Xlinker -L/mingw/ -Xlinker --export-all-symbol
endif

#Board c6678 simulate device
C6678SimulateDevice = boardsSimulateDevice/c6678SimulateDevice

dev_device_state_control_c6678 = $(C6678SimulateDevice)/device_state_control_c6678/device_state_control_c6678.c $(C6678SimulateDevice)/device_state_control_c6678/device_state_control_c6678_module.c

libdevice_state_control_c6678_la_SOURCES = $(dev_device_state_control_c6678)
libdevice_state_control_c6678_la_LDFLAGS = -module
libdevice_state_control_c6678_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES = libdevice_state_control_c6678.la

dev_dsc_c6678 = $(C6678SimulateDevice)/dsc_c6678/dsc_c6678.c $(C6678SimulateDevice)/dsc_c6678/dsc_c6678_module.c
libdsc_c6678_la_SOURCES = $(dev_dsc_c6678)
libdsc_c6678_la_LDFLAGS = -module
libdsc_c6678_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libdsc_c6678.la

dev_msmc_c6678 = $(C6678SimulateDevice)/msmc_c6678/msmc_c6678.c $(C6678SimulateDevice)/msmc_c6678/msmc_c6678_module.c
libmsmc_c6678_la_SOURCES = $(dev_msmc_c6678)
libmsmc_c6678_la_LDFLAGS = -module
libmsmc_c6678_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmsmc_c6678.la

dev_ddr3_c6678 = $(C6678SimulateDevice)/ddr3_c6678/ddr3_c6678.c $(C6678SimulateDevice)/ddr3_c6678/ddr3_c6678_module.c
libddr3_c6678_la_SOURCES = $(dev_ddr3_c6678)
libddr3_c6678_la_LDFLAGS = -module
libddr3_c6678_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libddr3_c6678.la

dev_srio_c6678 = $(C6678SimulateDevice)/srio_c6678/srio_c6678.c $(C6678SimulateDevice)/srio_c6678/srio_c6678_module.c
libsrio_c6678_la_SOURCES = $(dev_srio_c6678)
libsrio_c6678_la_LDFLAGS = -module
libsrio_c6678_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsrio_c6678.la

#Board c6713 simulate device
C6713SimulateDevice = boardsSimulateDevice/c6713SimulateDevice

dev_atm_zn122 = $(C6713SimulateDevice)/atm_zn122/atm_zn122.c $(C6713SimulateDevice)/atm_zn122/atm_zn122_module.c

libatm_zn122_la_SOURCES = $(dev_atm_zn122)
libatm_zn122_la_LDFLAGS = -module
libatm_zn122_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libatm_zn122.la

dev_kj_16c_rs422_6713 = $(C6713SimulateDevice)/kj_16c_rs422_6713/kj_16c_rs422_6713.c $(C6713SimulateDevice)/kj_16c_rs422_6713/kj_16c_rs422_6713_module.c

libkj_16c_rs422_6713_la_SOURCES = $(dev_kj_16c_rs422_6713)
libkj_16c_rs422_6713_la_LDFLAGS = -module
libkj_16c_rs422_6713_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libkj_16c_rs422_6713.la

dev_L2cache_c6713 = $(C6713SimulateDevice)/L2cache_c6713/L2cache_c6713.c $(C6713SimulateDevice)/L2cache_c6713/L2cache_c6713_module.c

libL2cache_c6713_la_SOURCES = $(dev_L2cache_c6713)
libL2cache_c6713_la_LDFLAGS = -module
libL2cache_c6713_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libL2cache_c6713.la

dev_Lvds_c6k = $(C6713SimulateDevice)/Lvds_c6k/Lvds_c6k.c $(C6713SimulateDevice)/Lvds_c6k/Lvds_c6k_module.c

libLvds_c6k_la_SOURCES = $(dev_Lvds_c6k)
libLvds_c6k_la_LDFLAGS = -module
libLvds_c6k_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libLvds_c6k.la

dev_nmi_trigger = $(C6713SimulateDevice)/nmi_trigger/nmi_trigger.c $(C6713SimulateDevice)/nmi_trigger/nmi_trigger_module.c

libnmi_trigger_la_SOURCES = $(dev_nmi_trigger)
libnmi_trigger_la_LDFLAGS = -module
libnmi_trigger_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libnmi_trigger.la

dev_semaphore_c6713 = $(C6713SimulateDevice)/semaphore_c6713/semaphore_c6713.c $(C6713SimulateDevice)/semaphore_c6713/semaphore_c6713_module.c

libsemaphore_c6713_la_SOURCES = $(dev_semaphore_c6713)
libsemaphore_c6713_la_LDFLAGS = -module
libsemaphore_c6713_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsemaphore_c6713.la

#Board c6748 simulate device
C6748SimulateDevice = boardsSimulateDevice/c6748SimulateDevice

dev_ecap_c6748 = $(C6748SimulateDevice)/ecap_c6748/ecap_c6748.c $(C6748SimulateDevice)/ecap_c6748/ecap_c6748_module.c

libecap_c6748_la_SOURCES = $(dev_ecap_c6748)
libecap_c6748_la_LDFLAGS = -module
libecap_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libecap_c6748.la

dev_sata_c6748 = $(C6748SimulateDevice)/sata_c6748/sata_c6748.c $(C6748SimulateDevice)/sata_c6748/sata_c6748_module.c

libsata_c6748_la_SOURCES = $(dev_sata_c6748)
libsata_c6748_la_LDFLAGS = -module
libsata_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsata_c6748.la

dev_upp_c6748 = $(C6748SimulateDevice)/upp_c6748/upp_c6748.c $(C6748SimulateDevice)/upp_c6748/upp_fifo.c $(C6748SimulateDevice)/upp_c6748/upp_c6748_module.c

libupp_c6748_la_SOURCES = $(dev_upp_c6748)
libupp_c6748_la_LDFLAGS = -module
libupp_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libupp_c6748.la

#Board ft2000a simulate device
Ft2000ASimulateDevice = boardsSimulateDevice/ft2000aSimulateDevice

dev_ft2000a_cru = $(Ft2000ASimulateDevice)/ft2000a_cru/ft2000a_cru.c $(Ft2000ASimulateDevice)/ft2000a_cru/ft2000a_cru_module.c

libft2000a_cru_la_SOURCES = $(dev_ft2000a_cru)
libft2000a_cru_la_LDFLAGS = -module
libft2000a_cru_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_cru.la

dev_ft2000a_lpc = $(Ft2000ASimulateDevice)/ft2000a_lpc/ft2000a_lpc.c $(Ft2000ASimulateDevice)/ft2000a_lpc/ft2000a_lpc_module.c

libft2000a_lpc_la_SOURCES = $(dev_ft2000a_lpc)
libft2000a_lpc_la_LDFLAGS = -module
libft2000a_lpc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_lpc.la

dev_ft2000a_mcu = $(Ft2000ASimulateDevice)/ft2000a_mcu/ft2000a_mcu.c $(Ft2000ASimulateDevice)/ft2000a_mcu/ft2000a_mcu_module.c

libft2000a_mcu_la_SOURCES = $(dev_ft2000a_mcu)
libft2000a_mcu_la_LDFLAGS = -module
libft2000a_mcu_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_mcu.la


#Board htey simulate device
HteySimulateDevice = boardsSimulateDevice/hteySimulateDevice

if WIN32
dev_htey_client = $(HteySimulateDevice)/htey_client/htey_client.c $(HteySimulateDevice)/htey_client/htey_client_module.c

libhtey_client_la_SOURCES = $(dev_htey_client)
libhtey_client_la_LDFLAGS = -module
libhtey_client_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_client.la
endif

dev_htey_cmd_file = $(HteySimulateDevice)/htey_cmd_file/htey_cmd_file.c $(HteySimulateDevice)/htey_cmd_file/htey_cmd_file_module.c

libhtey_cmd_file_la_SOURCES = $(dev_htey_cmd_file)
libhtey_cmd_file_la_LDFLAGS = -module
libhtey_cmd_file_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_cmd_file.la

dev_htey_GPS = $(HteySimulateDevice)/htey_GPS/htey_GPS.c $(HteySimulateDevice)/htey_GPS/htey_GPS_module.c

libhtey_GPS_la_SOURCES = $(dev_htey_GPS)
libhtey_GPS_la_LDFLAGS = -module
libhtey_GPS_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_GPS.la

dev_htey_hdlc = $(HteySimulateDevice)/htey_hdlc/htey_hdlc.c $(HteySimulateDevice)/htey_hdlc/htey_hdlc_module.c

libhtey_hdlc_la_SOURCES = $(dev_htey_hdlc)
libhtey_hdlc_la_LDFLAGS = -module
libhtey_hdlc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_hdlc.la

dev_htey_hdlc_cmd_file = $(HteySimulateDevice)/htey_hdlc_cmd_file/htey_hdlc_cmd_file.c $(HteySimulateDevice)/htey_hdlc_cmd_file/htey_hdlc_cmd_file_module.c

libhtey_hdlc_cmd_file_la_SOURCES = $(dev_htey_hdlc_cmd_file)
libhtey_hdlc_cmd_file_la_LDFLAGS = -module
libhtey_hdlc_cmd_file_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_hdlc_cmd_file.la

dev_htey_kgl = $(HteySimulateDevice)/htey_kgl/htey_kgl.c $(HteySimulateDevice)/htey_kgl/htey_kgl_module.c

libhtey_kgl_la_SOURCES = $(dev_htey_kgl)
libhtey_kgl_la_LDFLAGS = -module
libhtey_kgl_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_kgl.la

dev_htey_uart = $(HteySimulateDevice)/htey_uart/htey_uart.c $(HteySimulateDevice)/htey_uart/htey_fifo.c $(HteySimulateDevice)/htey_uart/htey_uart_module.c

libhtey_uart_la_SOURCES = $(dev_htey_uart)
libhtey_uart_la_LDFLAGS = -module
libhtey_uart_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_uart.la

#Board imx6q simulate device
Imx6qSimulateDevice = boardsSimulateDevice/imx6qSimulateDevice

dev_ccm_imx6q = $(Imx6qSimulateDevice)/ccm_imx6q/ccm_imx6q.c $(Imx6qSimulateDevice)/ccm_imx6q/ccm_imx6q_module.c

libccm_imx6q_la_SOURCES = $(dev_ccm_imx6q)
libccm_imx6q_la_LDFLAGS = -module
libccm_imx6q_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libccm_imx6q.la

dev_gpt_imx6q = $(Imx6qSimulateDevice)/gpt_imx6q/gpt_imx6q.c $(Imx6qSimulateDevice)/gpt_imx6q/gpt_imx6q_module.c

libgpt_imx6q_la_SOURCES = $(dev_gpt_imx6q)
libgpt_imx6q_la_LDFLAGS = -module
libgpt_imx6q_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpt_imx6q.la

#Board mpc750 simulate device
Mpc750SimulateDevice = boardsSimulateDevice/mpc750SimulateDevice

dev_i8254_timer = $(Mpc750SimulateDevice)/i8254_timer/i8254_timer.c $(Mpc750SimulateDevice)/i8254_timer/i8254_timer_module.c

libi8254_timer_la_SOURCES = $(dev_i8254_timer)
libi8254_timer_la_LDFLAGS = -module
libi8254_timer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libi8254_timer.la

dev_i8259a_pic = $(Mpc750SimulateDevice)/i8259a_pic/i8259a_pic.c $(Mpc750SimulateDevice)/i8259a_pic/i8259a_pic_module.c

libi8259a_pic_la_SOURCES = $(dev_i8259a_pic)
libi8259a_pic_la_LDFLAGS = -module
libi8259a_pic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libi8259a_pic.la

dev_mc146818_rtc = $(Mpc750SimulateDevice)/mc146818_rtc/mc146818_rtc.c $(Mpc750SimulateDevice)/mc146818_rtc/mc146818_rtc_module.c

libmc146818_rtc_la_SOURCES = $(dev_mc146818_rtc)
libmc146818_rtc_la_LDFLAGS = -module
libmc146818_rtc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmc146818_rtc.la

dev_sc16550_uart = $(Mpc750SimulateDevice)/sc16550_uart/sc16550_uart.c $(Mpc750SimulateDevice)/sc16550_uart/sc16550_uart_module.c

libsc16550_uart_la_SOURCES = $(dev_sc16550_uart)
libsc16550_uart_la_LDFLAGS = -module
libsc16550_uart_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsc16550_uart.la

#Board mpc5121 simulate device
Mpc5121SimulateDevice = boardsSimulateDevice/mpc5121SimulateDevice

dev_sfifoc_mpc5121 = $(Mpc5121SimulateDevice)/sfifoc_mpc5121/sfifoc_mpc5121.c $(Mpc5121SimulateDevice)/sfifoc_mpc5121/sfifoc_mpc5121_module.c

libsfifoc_mpc5121_la_SOURCES = $(dev_sfifoc_mpc5121)
libsfifoc_mpc5121_la_LDFLAGS = -module
libsfifoc_mpc5121_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsfifoc_mpc5121.la

#Board mpc8378 simulate device
Mpc8378SimulateDevice = boardsSimulateDevice/mpc8378SimulateDevice

dev_mpc8378_ddr = $(Mpc8378SimulateDevice)/mpc8378_ddr/mpc8378_ddr.c $(Mpc8378SimulateDevice)/mpc8378_ddr/mpc8378_ddr_module.c

libmpc8378_ddr_la_SOURCES = $(dev_mpc8378_ddr)
libmpc8378_ddr_la_LDFLAGS = -module
libmpc8378_ddr_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8378_ddr.la

dev_mpc8378_pit = $(Mpc8378SimulateDevice)/mpc8378_pit/mpc8378_pit.c $(Mpc8378SimulateDevice)/mpc8378_pit/mpc8378_pit_module.c

libmpc8378_pit_la_SOURCES = $(dev_mpc8378_pit)
libmpc8378_pit_la_LDFLAGS = -module
libmpc8378_pit_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8378_pit.la

#Board mpc8560 simulate device
Mpc8560SimulateDevice = boardsSimulateDevice/mpc8560SimulateDevice

dev_cpm_scc = $(Mpc8560SimulateDevice)/cpm_scc/cpm_scc.c $(Mpc8560SimulateDevice)/cpm_scc/cpm_scc_module.c

libcpm_scc_la_SOURCES = $(dev_cpm_scc)
libcpm_scc_la_LDFLAGS = -module
libcpm_scc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libcpm_scc.la

#Board mpc8641 simulate device
Mpc8641SimulateDevice = boardsSimulateDevice/mpc8641SimulateDevice

dev_rapidio_mpc8641 = $(Mpc8641SimulateDevice)/rapidio_mpc8641/rapidio_mpc8641.c $(Mpc8641SimulateDevice)/rapidio_mpc8641/rapidio_mpc8641_module.c

librapidio_mpc8641_la_SOURCES = $(dev_rapidio_mpc8641)
librapidio_mpc8641_la_LDFLAGS = -module
librapidio_mpc8641_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librapidio_mpc8641.la

#Board omapl138 simulate device
Omapl138SimulateDevice = boardsSimulateDevice/omapl138SimulateDevice

dev_mcasp_omapl138 = $(Omapl138SimulateDevice)/mcasp_omapl138/mcasp_omapl138.c $(Omapl138SimulateDevice)/mcasp_omapl138/mcasp_omapl138_module.c

libmcasp_omapl138_la_SOURCES = $(dev_mcasp_omapl138)
libmcasp_omapl138_la_LDFLAGS = -module
libmcasp_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmcasp_omapl138.la

dev_psc_omapl138 = $(Omapl138SimulateDevice)/psc_omapl138/psc_omapl138.c $(Omapl138SimulateDevice)/psc_omapl138/psc_omapl138_module.c

libpsc_omapl138_la_SOURCES = $(dev_psc_omapl138)
libpsc_omapl138_la_LDFLAGS = -module
libpsc_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpsc_omapl138.la

dev_syscfg_omapl138 = $(Omapl138SimulateDevice)/syscfg_omapl138/syscfg_omapl138.c $(Omapl138SimulateDevice)/syscfg_omapl138/syscfg_omapl138_module.c

libsyscfg_omapl138_la_SOURCES = $(dev_syscfg_omapl138)
libsyscfg_omapl138_la_LDFLAGS = -module
libsyscfg_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsyscfg_omapl138.la

#Board p2020 simulate device
P2020SimulateDevice = boardsSimulateDevice/p2020SimulateDevice

dev_p2020_ecm = $(P2020SimulateDevice)/p2020_ecm/p2020_ecm.c $(P2020SimulateDevice)/p2020_ecm/p2020_ecm_module.c

libp2020_ecm_la_SOURCES = $(dev_p2020_ecm)
libp2020_ecm_la_LDFLAGS = -module
libp2020_ecm_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libp2020_ecm.la

dev_p2020_elbc = $(P2020SimulateDevice)/p2020_elbc/p2020_elbc.c $(P2020SimulateDevice)/p2020_elbc/p2020_elbc_module.c

libp2020_elbc_la_SOURCES = $(dev_p2020_elbc)
libp2020_elbc_la_LDFLAGS = -module
libp2020_elbc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libp2020_elbc.la

dev_p2020_guts = $(P2020SimulateDevice)/p2020_guts/p2020_guts.c $(P2020SimulateDevice)/p2020_guts/p2020_guts_module.c

libp2020_guts_la_SOURCES = $(dev_p2020_guts)
libp2020_guts_la_LDFLAGS = -module
libp2020_guts_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libp2020_guts.la

dev_p2020_L2_cache = $(P2020SimulateDevice)/p2020_L2_cache/p2020_L2_cache.c $(P2020SimulateDevice)/p2020_L2_cache/p2020_L2_cache_module.c

libp2020_L2_cache_la_SOURCES = $(dev_p2020_L2_cache)
libp2020_L2_cache_la_LDFLAGS = -module
libp2020_L2_cache_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libp2020_L2_cache.la

#Board rm48x simulate device
Rm48xSimulateDevice = boardsSimulateDevice/rm48xSimulateDevice

dev_rm48x_esm = $(Rm48xSimulateDevice)/rm48x_esm/rm48x_esm.c $(Rm48xSimulateDevice)/rm48x_esm/rm48x_esm_module.c

librm48x_esm_la_SOURCES = $(dev_rm48x_esm)
librm48x_esm_la_LDFLAGS = -module
librm48x_esm_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_esm.la

dev_rm48x_pbist = $(Rm48xSimulateDevice)/rm48x_pbist/rm48x_pbist.c $(Rm48xSimulateDevice)/rm48x_pbist/rm48x_pbist_module.c

librm48x_pbist_la_SOURCES = $(dev_rm48x_pbist)
librm48x_pbist_la_LDFLAGS = -module
librm48x_pbist_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_pbist.la

dev_rm48x_pcr = $(Rm48xSimulateDevice)/rm48x_pcr/rm48x_pcr.c $(Rm48xSimulateDevice)/rm48x_pcr/rm48x_pcr_module.c

librm48x_pcr_la_SOURCES = $(dev_rm48x_pcr)
librm48x_pcr_la_LDFLAGS = -module
librm48x_pcr_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_pcr.la

dev_rm48x_pmm = $(Rm48xSimulateDevice)/rm48x_pmm/rm48x_pmm.c $(Rm48xSimulateDevice)/rm48x_pmm/rm48x_pmm_module.c

librm48x_pmm_la_SOURCES = $(dev_rm48x_pmm)
librm48x_pmm_la_LDFLAGS = -module
librm48x_pmm_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_pmm.la

#Board s3c6410 simulate device
S3c6410SimulateDevice = boardsSimulateDevice/s3c6410SimulateDevice

dev_mfc_s3c6410 = $(S3c6410SimulateDevice)/mfc_s3c6410/mfc_s3c6410.c $(S3c6410SimulateDevice)/mfc_s3c6410/mfc_s3c6410_module.c

libmfc_s3c6410_la_SOURCES = $(dev_mfc_s3c6410)
libmfc_s3c6410_la_LDFLAGS = -module
libmfc_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmfc_s3c6410.la

dev_s3c6410_keypad = $(S3c6410SimulateDevice)/s3c6410_keypad/s3c6410_keypad.c $(S3c6410SimulateDevice)/s3c6410_keypad/s3c6410_keypad_module.c

libs3c6410_keypad_la_SOURCES = $(dev_s3c6410_keypad)
libs3c6410_keypad_la_LDFLAGS = -module
libs3c6410_keypad_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libs3c6410_keypad.la

dev_sdhci_s3c6410 = $(S3c6410SimulateDevice)/sdhci_s3c6410/sdhci_s3c6410.c $(S3c6410SimulateDevice)/sdhci_s3c6410/sdhci_s3c6410_module.c

libsdhci_s3c6410_la_SOURCES = $(dev_sdhci_s3c6410)
libsdhci_s3c6410_la_LDFLAGS = -module
libsdhci_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsdhci_s3c6410.la


#Board stm32f0x simulate device
Stm32f0xSimulateDevice = boardsSimulateDevice/stm32f0xSimulateDevice

dev_nvic_stm32f0xx = $(Stm32f0xSimulateDevice)/nvic_stm32f0xx/nvic_stm32f0xx.c $(Stm32f0xSimulateDevice)/nvic_stm32f0xx/nvic_stm32f0xx_module.c

libnvic_stm32f0xx_la_SOURCES = $(dev_nvic_stm32f0xx)
libnvic_stm32f0xx_la_LDFLAGS = -module
libnvic_stm32f0xx_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libnvic_stm32f0xx.la

dev_rcc_stm32f0x = $(Stm32f0xSimulateDevice)/rcc_stm32f0x/rcc_stm32f0x.c $(Stm32f0xSimulateDevice)/rcc_stm32f0x/rcc_stm32f0x_module.c

librcc_stm32f0x_la_SOURCES = $(dev_rcc_stm32f0x)
librcc_stm32f0x_la_LDFLAGS = -module
librcc_stm32f0x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librcc_stm32f0x.la

dev_scb_stm32f0xx = $(Stm32f0xSimulateDevice)/scb_stm32f0xx/scb_stm32f0xx.c $(Stm32f0xSimulateDevice)/scb_stm32f0xx/scb_stm32f0xx_module.c

libscb_stm32f0xx_la_SOURCES = $(dev_scb_stm32f0xx)
libscb_stm32f0xx_la_LDFLAGS = -module
libscb_stm32f0xx_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libscb_stm32f0xx.la

#Board t2080 simulate device
T2080SimulateDevice = boardsSimulateDevice/t2080SimulateDevice

dev_dcfg_t2080 = $(T2080SimulateDevice)/dcfg_t2080/dcfg_t2080.c $(T2080SimulateDevice)/dcfg_t2080/dcfg_t2080_module.c

libdcfg_t2080_la_SOURCES = $(dev_dcfg_t2080)
libdcfg_t2080_la_LDFLAGS = -module
libdcfg_t2080_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libdcfg_t2080.la

#Board tsc695f simulate device
Tsc695fSimulateDevice = boardsSimulateDevice/tsc695fSimulateDevice

dev_dmc_sparc = $(Tsc695fSimulateDevice)/dmc_sparc/dmc_sparc.c $(Tsc695fSimulateDevice)/dmc_sparc/dmc_fifo.c $(Tsc695fSimulateDevice)/dmc_sparc/dmc_sparc_module.c

libdmc_sparc_la_SOURCES = $(dev_dmc_sparc)
libdmc_sparc_la_LDFLAGS = -module
libdmc_sparc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libdmc_sparc.la

dev_fight_control_sparc = $(Tsc695fSimulateDevice)/fight_control_sparc/fight_control_sparc.c $(Tsc695fSimulateDevice)/fight_control_sparc/fight_control_sparc_module.c

libfight_control_sparc_la_SOURCES = $(dev_fight_control_sparc)
libfight_control_sparc_la_LDFLAGS = -module
libfight_control_sparc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libfight_control_sparc.la

dev_remote_ctrl_sparc = $(Tsc695fSimulateDevice)/remote_ctrl_sparc/remote_ctrl_sparc.c $(Tsc695fSimulateDevice)/remote_ctrl_sparc/remote_ctrl_sparc_module.c

libremote_ctrl_sparc_la_SOURCES = $(dev_remote_ctrl_sparc)
libremote_ctrl_sparc_la_LDFLAGS = -module
libremote_ctrl_sparc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libremote_ctrl_sparc.la

dev_systate_sparc = $(Tsc695fSimulateDevice)/systate_sparc/systate_sparc.c $(Tsc695fSimulateDevice)/systate_sparc/systate_sparc_module.c

libsystate_sparc_la_SOURCES = $(dev_systate_sparc)
libsystate_sparc_la_LDFLAGS = -module
libsystate_sparc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsystate_sparc.la

#Board tsc695f simulate device
LoongsonSimulateDevice = boardsSimulateDevice/loongsonSimulateDevice

dev_loongson_adc = $(LoongsonSimulateDevice)/loongson_adc/loongson_adc.c $(LoongsonSimulateDevice)/loongson_adc/loongson_adc_module.c

libloongson_adc_la_SOURCES = $(dev_loongson_adc)
libloongson_adc_la_LDFLAGS = -module
libloongson_adc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_adc.la

dev_loongson_collect = $(LoongsonSimulateDevice)/loongson_collect/loongson_collect.c $(LoongsonSimulateDevice)/loongson_collect/loongson_collect_module.c

libloongson_collect_la_SOURCES = $(dev_loongson_collect)
libloongson_collect_la_LDFLAGS = -module
libloongson_collect_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_collect.la

dev_loongson_config = $(LoongsonSimulateDevice)/loongson_config/loongson_config.c $(LoongsonSimulateDevice)/loongson_config/loongson_config_module.c

libloongson_config_la_SOURCES = $(dev_loongson_config)
libloongson_config_la_LDFLAGS = -module
libloongson_config_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_config.la

dev_loongson_flash = $(LoongsonSimulateDevice)/loongson_flash/loongson_flash.c $(LoongsonSimulateDevice)/loongson_flash/loongson_flash_module.c

libloongson_flash_la_SOURCES = $(dev_loongson_flash)
libloongson_flash_la_LDFLAGS = -module
libloongson_flash_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_flash.la

dev_loongson_hpet = $(LoongsonSimulateDevice)/loongson_hpet/loongson_hpet.c $(LoongsonSimulateDevice)/loongson_hpet/loongson_hpet_module.c

libloongson_hpet_la_SOURCES = $(dev_loongson_hpet)
libloongson_hpet_la_LDFLAGS = -module
libloongson_hpet_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_hpet.la

dev_loongson_ipmb = $(LoongsonSimulateDevice)/loongson_ipmb/loongson_ipmb.c $(LoongsonSimulateDevice)/loongson_ipmb/loongson_ipmb_module.c

libloongson_ipmb_la_SOURCES = $(dev_loongson_ipmb)
libloongson_ipmb_la_LDFLAGS = -module
libloongson_ipmb_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_ipmb.la

dev_loongson_pwm = $(LoongsonSimulateDevice)/loongson_pwm/loongson_pwm.c $(LoongsonSimulateDevice)/loongson_pwm/loongson_pwm_module.c

libloongson_pwm_la_SOURCES = $(dev_loongson_pwm)
libloongson_pwm_la_LDFLAGS = -module
libloongson_pwm_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_pwm.la

dev_loongson_sec = $(LoongsonSimulateDevice)/loongson_sec/loongson_sec.c $(LoongsonSimulateDevice)/loongson_sec/loongson_sec_module.c

libloongson_sec_la_SOURCES = $(dev_loongson_sec)
libloongson_sec_la_LDFLAGS = -module
libloongson_sec_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_sec.la

dev_loongson_spi = $(LoongsonSimulateDevice)/loongson_spi/loongson_spi.c $(LoongsonSimulateDevice)/loongson_spi/loongson_spi_fifo.c $(LoongsonSimulateDevice)/loongson_spi/loongson_spi_module.c

libloongson_spi_la_SOURCES = $(dev_loongson_spi)
libloongson_spi_la_LDFLAGS = -module
libloongson_spi_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_spi.la

dev_loongson_spi_slave = $(LoongsonSimulateDevice)/loongson_spi_slave/loongson_spi_slave.c $(LoongsonSimulateDevice)/loongson_spi_slave/loongson_spi_slave_module.c

libloongson_spi_slave_la_SOURCES = $(dev_loongson_spi_slave)
libloongson_spi_slave_la_LDFLAGS = -module
libloongson_spi_slave_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_spi_slave.la

dev_loongson_uart = $(LoongsonSimulateDevice)/loongson_uart/loongson_uart.c $(LoongsonSimulateDevice)/loongson_uart/loongson_uart_module.c

libloongson_uart_la_SOURCES = $(dev_loongson_uart)
libloongson_uart_la_LDFLAGS = -module
libloongson_uart_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libloongson_uart.la

#Board at697f simulate device
At697fSimulateDevice = boardsSimulateDevice/at697fSimulateDevice

dev_at697f_flash = $(At697fSimulateDevice)/at697f_flash/at697f_flash.c $(At697fSimulateDevice)/at697f_flash/at697f_flash_module.c

libat697f_flash_la_SOURCES = $(dev_at697f_flash)
libat697f_flash_la_LDFLAGS = -module
libat697f_flash_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libat697f_flash.la

dev_at697f_iic = $(At697fSimulateDevice)/at697f_iic/at697f_iic.c $(At697fSimulateDevice)/at697f_iic/at697f_iic_module.c

libat697f_iic_la_SOURCES = $(dev_at697f_iic)
libat697f_iic_la_LDFLAGS = -module
libat697f_iic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libat697f_iic.la

dev_at697f_spi = $(At697fSimulateDevice)/at697f_spi/at697f_spi.c $(At697fSimulateDevice)/at697f_spi/at697f_spi_fifo.c $(At697fSimulateDevice)/at697f_spi/at697f_spi_module.c

libat697f_spi_la_SOURCES = $(dev_at697f_spi)
libat697f_spi_la_LDFLAGS = -module
libat697f_spi_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libat697f_spi.la

dev_at697f_spi_device = $(At697fSimulateDevice)/at697f_spi_device/at697f_spi_device.c $(At697fSimulateDevice)/at697f_spi_device/at697f_spi_device_module.c

libat697f_spi_device_la_SOURCES = $(dev_at697f_spi_device)
libat697f_spi_device_la_LDFLAGS = -module
libat697f_spi_device_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libat697f_spi_device.la

dev_bm3803_system_control = $(At697fSimulateDevice)/bm3803_system_control/bm3803_system_control.c $(At697fSimulateDevice)/bm3803_system_control/bm3803_intc.c $(At697fSimulateDevice)/bm3803_system_control/bm3803_gpio.c $(At697fSimulateDevice)/bm3803_system_control/bm3803_timer.c $(At697fSimulateDevice)/bm3803_system_control/bm3803_uart.c $(At697fSimulateDevice)/bm3803_system_control/intc_fifo.c $(At697fSimulateDevice)/bm3803_system_control/bm3803_system_control_module.c

libbm3803_system_control_la_SOURCES = $(dev_bm3803_system_control)
libbm3803_system_control_la_LDFLAGS = -module
libbm3803_system_control_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libbm3803_system_control.la

dev_bm3823_system_control = $(At697fSimulateDevice)/bm3823_system_control/bm3823_system_control.c $(At697fSimulateDevice)/bm3823_system_control/bm3823_timer.c $(At697fSimulateDevice)/bm3823_system_control/bm3823_uart.c $(At697fSimulateDevice)/bm3823_system_control/bm3823_system_control_module.c

libbm3823_system_control_la_SOURCES = $(dev_bm3823_system_control)
libbm3823_system_control_la_LDFLAGS = -module
libbm3823_system_control_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libbm3823_system_control.la

#Board mpc5554 simulate device
Mpc5554SimulateDevice = boardsSimulateDevice/mpc5554SimulateDevice

dev_mpc5554_ebi = $(Mpc5554SimulateDevice)/mpc5554_ebi/mpc5554_ebi.c $(Mpc5554SimulateDevice)/mpc5554_ebi/mpc5554_ebi_module.c

libmpc5554_ebi_la_SOURCES = $(dev_mpc5554_ebi)
libmpc5554_ebi_la_LDFLAGS = -module
libmpc5554_ebi_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_ebi.la

dev_mpc5554_edma = $(Mpc5554SimulateDevice)/mpc5554_edma/mpc5554_edma.c $(Mpc5554SimulateDevice)/mpc5554_edma/mpc5554_edma_module.c

libmpc5554_edma_la_SOURCES = $(dev_mpc5554_edma)
libmpc5554_edma_la_LDFLAGS = -module
libmpc5554_edma_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_edma.la

dev_mpc5554_esci = $(Mpc5554SimulateDevice)/mpc5554_esci/mpc5554_esci.c $(Mpc5554SimulateDevice)/mpc5554_esci/mpc5554_esci_fifo.c $(Mpc5554SimulateDevice)/mpc5554_esci/mpc5554_esci_module.c

libmpc5554_esci_la_SOURCES = $(dev_mpc5554_esci)
libmpc5554_esci_la_LDFLAGS = -module
libmpc5554_esci_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_esci.la

dev_mpc5554_flash = $(Mpc5554SimulateDevice)/mpc5554_flash/mpc5554_flash.c $(Mpc5554SimulateDevice)/mpc5554_flash/mpc5554_flash_module.c

libmpc5554_flash_la_SOURCES = $(dev_mpc5554_flash)
libmpc5554_flash_la_LDFLAGS = -module
libmpc5554_flash_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_flash.la

dev_mpc5554_fmpll = $(Mpc5554SimulateDevice)/mpc5554_fmpll/mpc5554_fmpll.c $(Mpc5554SimulateDevice)/mpc5554_fmpll/mpc5554_fmpll_module.c

libmpc5554_fmpll_la_SOURCES = $(dev_mpc5554_fmpll)
libmpc5554_fmpll_la_LDFLAGS = -module
libmpc5554_fmpll_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_fmpll.la

dev_mpc5554_intc = $(Mpc5554SimulateDevice)/mpc5554_intc/mpc5554_intc.c $(Mpc5554SimulateDevice)/mpc5554_intc/mpc5554_intc_module.c

libmpc5554_intc_la_SOURCES = $(dev_mpc5554_intc)
libmpc5554_intc_la_LDFLAGS = -module
libmpc5554_intc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_intc.la

dev_mpc5554_siu = $(Mpc5554SimulateDevice)/mpc5554_siu/mpc5554_siu.c $(Mpc5554SimulateDevice)/mpc5554_siu/mpc5554_siu_module.c

libmpc5554_siu_la_SOURCES = $(dev_mpc5554_siu)
libmpc5554_siu_la_LDFLAGS = -module
libmpc5554_siu_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_siu.la

dev_mpc5554_sm29lv160 = $(Mpc5554SimulateDevice)/mpc5554_sm29lv160/mpc5554_sm29lv160.c $(Mpc5554SimulateDevice)/mpc5554_sm29lv160/mpc5554_sm29lv160_module.c

libmpc5554_sm29lv160_la_SOURCES = $(dev_mpc5554_sm29lv160)
libmpc5554_sm29lv160_la_LDFLAGS = -module
libmpc5554_sm29lv160_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_sm29lv160.la

dev_mpc5554_pit = $(Mpc5554SimulateDevice)/mpc5554_pit/mpc5554_pit.c $(Mpc5554SimulateDevice)/mpc5554_pit/mpc5554_pit_module.c

libmpc5554_pit_la_SOURCES = $(dev_mpc5554_pit)
libmpc5554_pit_la_LDFLAGS = -module
libmpc5554_pit_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_pit.la

dev_mpc5554_stm = $(Mpc5554SimulateDevice)/mpc5554_stm/mpc5554_stm.c $(Mpc5554SimulateDevice)/mpc5554_stm/mpc5554_stm_module.c

libmpc5554_stm_la_SOURCES = $(dev_mpc5554_stm)
libmpc5554_stm_la_LDFLAGS = -module
libmpc5554_stm_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc5554_stm.la

EepromSimulateDevice = eepromSimulateDevice

dev_eeprom_cat24c64 = $(EepromSimulateDevice)/eeprom_cat24c64/cat24c64.c $(EepromSimulateDevice)/eeprom_cat24c64/cat24c64_module.c

libeeprom_cat24c64_la_SOURCES = $(dev_eeprom_cat24c64)
libeeprom_cat24c64_la_LDFLAGS = -module
libeeprom_cat24c64_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libeeprom_cat24c64.la

dev_eeprom_cat24c256 = $(EepromSimulateDevice)/eeprom_cat24c256/cat24c256.c $(EepromSimulateDevice)/eeprom_cat24c256/cat24c256_module.c

libeeprom_cat24c256_la_SOURCES = $(dev_eeprom_cat24c256)
libeeprom_cat24c256_la_LDFLAGS = -module
libeeprom_cat24c256_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libeeprom_cat24c256.la

dev_eeprom_sparc = $(EepromSimulateDevice)/eeprom_sparc/eeprom.c $(EepromSimulateDevice)/eeprom_sparc/eeprom_module.c

libeeprom_sparc_la_SOURCES = $(dev_eeprom_sparc)
libeeprom_sparc_la_LDFLAGS = -module
libeeprom_sparc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libeeprom_sparc.la

FlashSimulateDevice = flashSimulateDevice

dev_amd_flash = $(FlashSimulateDevice)/amd_flash/amd_flash.c $(FlashSimulateDevice)/amd_flash/am29_flash.c $(FlashSimulateDevice)/amd_flash/amd_flash_module.c

libamd_flash_la_SOURCES = $(dev_amd_flash)
libamd_flash_la_LDFLAGS = -module
libamd_flash_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libamd_flash.la

dev_c6713_flash = $(FlashSimulateDevice)/c6713_flash/c6713_flash.c $(FlashSimulateDevice)/c6713_flash/c6713_flash_module.c

libc6713_flash_la_SOURCES = $(dev_c6713_flash)
libc6713_flash_la_LDFLAGS = -module
libc6713_flash_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libc6713_flash.la

dev_flash = $(FlashSimulateDevice)/flash/dev_flash_sst39lvf160.c $(FlashSimulateDevice)/flash/flash_module.c $(FlashSimulateDevice)/flash/dev_flash_am29.c  $(FlashSimulateDevice)/flash/dev_flash_intel.c  $(FlashSimulateDevice)/flash/skyeye_flash.c

libflash_la_SOURCES = $(dev_flash)
libflash_la_LDFLAGS = -module
libflash_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libflash.la

dev_flash_28335 = $(FlashSimulateDevice)/flash_28335/flash_28335.c $(FlashSimulateDevice)/flash_28335/flash_28335_module.c

libflash_28335_la_SOURCES = $(dev_flash_28335)
libflash_28335_la_LDFLAGS = -module
libflash_28335_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libflash_28335.la

dev_flash_am29lv160B = $(FlashSimulateDevice)/flash_am29lv160B/flash_am29lv160B.c $(FlashSimulateDevice)/flash_am29lv160B/flash_am29lv160B_module.c

libflash_am29lv160B_la_SOURCES = $(dev_flash_am29lv160B)
libflash_am29lv160B_la_LDFLAGS = -module
libflash_am29lv160B_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libflash_am29lv160B.la

dev_flash_interface_stm32f0x = $(FlashSimulateDevice)/flash_interface_stm32f0x/flash_interface_stm32f0x.c $(FlashSimulateDevice)/flash_interface_stm32f0x/flash_interface_stm32f0x_module.c

libflash_interface_stm32f0x_la_SOURCES = $(dev_flash_interface_stm32f0x)
libflash_interface_stm32f0x_la_LDFLAGS = -module
libflash_interface_stm32f0x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libflash_interface_stm32f0x.la

dev_flash_s29al016d = $(FlashSimulateDevice)/flash_s29al016d/flash_s29al016d.c $(FlashSimulateDevice)/flash_s29al016d/flash_s29al016d_module.c

libflash_s29al016d_la_SOURCES = $(dev_flash_s29al016d)
libflash_s29al016d_la_LDFLAGS = -module
libflash_s29al016d_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libflash_s29al016d.la

dev_flash_s29gl01gs = $(FlashSimulateDevice)/flash_s29gl01gs/flash_s29gl01gs.c $(FlashSimulateDevice)/flash_s29gl01gs/flash_s29gl01gs_module.c

libflash_s29gl01gs_la_SOURCES = $(dev_flash_s29gl01gs)
libflash_s29gl01gs_la_LDFLAGS = -module
libflash_s29gl01gs_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libflash_s29gl01gs.la

dev_nand_c6748 = $(FlashSimulateDevice)/nand_c6748/nand_c6748.c $(FlashSimulateDevice)/nand_c6748/nand_fifo.c $(FlashSimulateDevice)/nand_c6748/nand_c6748_module.c

libnand_c6748_la_SOURCES = $(dev_nand_c6748)
libnand_c6748_la_LDFLAGS = -module
libnand_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libnand_c6748.la

dev_nandflash = $(FlashSimulateDevice)/nandflash/dev_nandflash_s3c2410.c $(FlashSimulateDevice)/nandflash/nandflash_smallblock.c $(FlashSimulateDevice)/nandflash/skyeye_nandflash.c $(FlashSimulateDevice)/nandflash/nandflash_module.c $(FlashSimulateDevice)/nandflash/dev_nandflash_s3c6410.c $(FlashSimulateDevice)/nandflash/K9G8G08.c $(FlashSimulateDevice)/nandflash/nandflash_K9G8G08.c

libnandflash_la_SOURCES = $(dev_nandflash)
libnandflash_la_LDFLAGS = -module
libnandflash_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libnandflash.la

dev_c6678_n25q128 =  $(FlashSimulateDevice)/c6678_n25q128/c6678_n25q128.c  $(FlashSimulateDevice)/c6678_n25q128/c6678_n25q128_module.c

libc6678_n25q128_la_SOURCES = $(dev_c6678_n25q128)
libc6678_n25q128_la_LDFLAGS = -module
libc6678_n25q128_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libc6678_n25q128.la

dev_c6678_mt29f1g =  $(FlashSimulateDevice)/c6678_mt29f1g/c6678_mt29f1g.c  $(FlashSimulateDevice)/c6678_mt29f1g/c6678_mt29f1g_module.c

libc6678_mt29f1g_la_SOURCES = $(dev_c6678_mt29f1g)
libc6678_mt29f1g_la_LDFLAGS = -module
libc6678_mt29f1g_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libc6678_mt29f1g.la

GpioSimulateDevice = gpioSimulateDevice

dev_adk_kio = $(GpioSimulateDevice)/adk_kio/adk_kio.c $(GpioSimulateDevice)/adk_kio/adk_kio_module.c

libadk_kio_la_SOURCES = $(dev_adk_kio)
libadk_kio_la_LDFLAGS = -module
libadk_kio_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libadk_kio.la

dev_ft2000a_gpio = $(GpioSimulateDevice)/ft2000a_gpio/ft2000a_gpio.c $(GpioSimulateDevice)/ft2000a_gpio/ft2000a_gpio_module.c

libft2000a_gpio_la_SOURCES = $(dev_ft2000a_gpio)
libft2000a_gpio_la_LDFLAGS = -module
libft2000a_gpio_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_gpio.la

dev_gpio = $(GpioSimulateDevice)/gpio/gpio.c $(GpioSimulateDevice)/gpio/gpio_module.c

libgpio_la_SOURCES = $(dev_gpio)
libgpio_la_LDFLAGS = -module
libgpio_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio.la

dev_gpio_am335x = $(GpioSimulateDevice)/gpio_am335x/gpio_am335x.c $(GpioSimulateDevice)/gpio_am335x/gpio_am335x_module.c

libgpio_am335x_la_SOURCES = $(dev_gpio_am335x)
libgpio_am335x_la_LDFLAGS = -module
libgpio_am335x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_am335x.la

dev_gpio_am3359 = $(GpioSimulateDevice)/gpio_am3359/gpio_am3359.c $(GpioSimulateDevice)/gpio_am3359/gpio_am3359_module.c

libgpio_am3359_la_SOURCES = $(dev_gpio_am3359)
libgpio_am3359_la_LDFLAGS = -module
libgpio_am3359_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_am3359.la

dev_gpio_at91rm9200 = $(GpioSimulateDevice)/gpio_at91rm9200/gpio_at91rm9200.c $(GpioSimulateDevice)/gpio_at91rm9200/gpio_at91rm9200_module.c

libgpio_at91rm9200_la_SOURCES = $(dev_gpio_at91rm9200)
libgpio_at91rm9200_la_LDFLAGS = -module
libgpio_at91rm9200_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_at91rm9200.la

dev_gpio_c64x = $(GpioSimulateDevice)/gpio_c64x/gpio_c64x.c $(GpioSimulateDevice)/gpio_c64x/gpio_c64x_module.c

libgpio_c64x_la_SOURCES = $(dev_gpio_c64x)
libgpio_c64x_la_LDFLAGS = -module
libgpio_c64x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_c64x.la

dev_gpio_c5510 = $(GpioSimulateDevice)/gpio_c5510/gpio_c5510.c $(GpioSimulateDevice)/gpio_c5510/gpio_c5510_module.c

libgpio_c5510_la_SOURCES = $(dev_gpio_c5510)
libgpio_c5510_la_LDFLAGS = -module
libgpio_c5510_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_c5510.la

dev_gpio_c6713 = $(GpioSimulateDevice)/gpio_c6713/gpio_c6713.c $(GpioSimulateDevice)/gpio_c6713/gpio_c6713_module.c

libgpio_c6713_la_SOURCES = $(dev_gpio_c6713)
libgpio_c6713_la_LDFLAGS = -module
libgpio_c6713_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_c6713.la

dev_gpio_c6748 = $(GpioSimulateDevice)/gpio_c6748/gpio_c6748.c $(GpioSimulateDevice)/gpio_c6748/gpio_c6748_module.c

libgpio_c6748_la_SOURCES = $(dev_gpio_c6748)
libgpio_c6748_la_LDFLAGS = -module
libgpio_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_c6748.la

dev_gpio_f28335 = $(GpioSimulateDevice)/gpio_f28335/gpio_f28335.c $(GpioSimulateDevice)/gpio_f28335/gpio_f28335_module.c

libgpio_f28335_la_SOURCES = $(dev_gpio_f28335)
libgpio_f28335_la_LDFLAGS = -module
libgpio_f28335_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_f28335.la

dev_gpio_omapl138 = $(GpioSimulateDevice)/gpio_omapl138/gpio_omapl138.c $(GpioSimulateDevice)/gpio_omapl138/gpio_omapl138_module.c

libgpio_omapl138_la_SOURCES = $(dev_gpio_omapl138)
libgpio_omapl138_la_LDFLAGS = -module
libgpio_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_omapl138.la

dev_gpio_s3c2410x = $(GpioSimulateDevice)/gpio_s3c2410x/gpio_s3c2410x.c $(GpioSimulateDevice)/gpio_s3c2410x/gpio_s3c2410x_module.c

libgpio_s3c2410x_la_SOURCES = $(dev_gpio_s3c2410x)
libgpio_s3c2410x_la_LDFLAGS = -module
libgpio_s3c2410x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_s3c2410x.la

dev_gpio_s3c6410 = $(GpioSimulateDevice)/gpio_s3c6410/gpio_s3c6410.c $(GpioSimulateDevice)/gpio_s3c6410/gpio_s3c6410_module.c

libgpio_s3c6410_la_SOURCES = $(dev_gpio_s3c6410)
libgpio_s3c6410_la_LDFLAGS = -module
libgpio_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_s3c6410.la

dev_gpio_x86 = $(GpioSimulateDevice)/gpio_x86/gpio_x86.c $(GpioSimulateDevice)/gpio_x86/gpio_x86_module.c

libgpio_x86_la_SOURCES = $(dev_gpio_x86)
libgpio_x86_la_LDFLAGS = -module
libgpio_x86_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_x86.la

dev_io_leon2 = $(GpioSimulateDevice)/io_leon2/io_leon2.c $(GpioSimulateDevice)/io_leon2/io_leon2_module.c

libio_leon2_la_SOURCES = $(dev_io_leon2)
libio_leon2_la_LDFLAGS = -module
libio_leon2_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libio_leon2.la

dev_rm48x_gio = $(GpioSimulateDevice)/rm48x_gio/rm48x_gio.c $(GpioSimulateDevice)/rm48x_gio/rm48x_gio_module.c

librm48x_gio_la_SOURCES = $(dev_rm48x_gio)
librm48x_gio_la_LDFLAGS = -module
librm48x_gio_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_gio.la

dev_rm48x_io = $(GpioSimulateDevice)/rm48x_io/rm48x_io.c $(GpioSimulateDevice)/rm48x_io/rm48x_io_module.c

librm48x_io_la_SOURCES = $(dev_rm48x_io)
librm48x_io_la_LDFLAGS = -module
librm48x_io_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_io.la

dev_sopc_gpio = $(GpioSimulateDevice)/sopc_gpio/sopc_gpio.c $(GpioSimulateDevice)/sopc_gpio/sopc_gpio_module.c

libsopc_gpio_la_SOURCES = $(dev_sopc_gpio)
libsopc_gpio_la_LDFLAGS = -module
libsopc_gpio_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsopc_gpio.la

dev_swt_sparc = $(GpioSimulateDevice)/swt_sparc/swt_sparc.c $(GpioSimulateDevice)/swt_sparc/swt_sparc_module.c

libswt_sparc_la_SOURCES = $(dev_swt_sparc)
libswt_sparc_la_LDFLAGS = -module
libswt_sparc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libswt_sparc.la

dev_gpio_c6678 = $(GpioSimulateDevice)/gpio_c6678/gpio_c6678.c $(GpioSimulateDevice)/gpio_c6678/gpio_c6678_module.c

libgpio_c6678_la_SOURCES = $(dev_gpio_c6678)
libgpio_c6678_la_LDFLAGS = -module
libgpio_c6678_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_c6678.la

I2cSimulateDevice = i2cSimulateDevice

dev_ft2000a_iic = $(I2cSimulateDevice)/ft2000a_iic/ft2000a_iic.c $(I2cSimulateDevice)/ft2000a_iic/iic_fifo.c $(I2cSimulateDevice)/ft2000a_iic/ft2000a_iic_module.c

libft2000a_iic_la_SOURCES = $(dev_ft2000a_iic)
libft2000a_iic_la_LDFLAGS = -module
libft2000a_iic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_iic.la

dev_iic_am335x = $(I2cSimulateDevice)/iic_am335x/iic_am335x.c $(I2cSimulateDevice)/iic_am335x/iic_am335x_module.c

libiic_am335x_la_SOURCES = $(dev_iic_am335x)
libiic_am335x_la_LDFLAGS = -module
libiic_am335x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libiic_am335x.la

dev_iic_c6713 = $(I2cSimulateDevice)/iic_c6713/iic_c6713.c $(I2cSimulateDevice)/iic_c6713/iic_c6713_module.c

libiic_c6713_la_SOURCES = $(dev_iic_c6713)
libiic_c6713_la_LDFLAGS = -module
libiic_c6713_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libiic_c6713.la

dev_iic_f28335 = $(I2cSimulateDevice)/iic_f28335/iic_f28335.c $(I2cSimulateDevice)/iic_f28335/iic_f28335_module.c

libiic_f28335_la_SOURCES = $(dev_iic_f28335)
libiic_f28335_la_LDFLAGS = -module
libiic_f28335_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libiic_f28335.la

dev_iic_omapl138 = $(I2cSimulateDevice)/iic_omapl138/iic_omapl138.c $(I2cSimulateDevice)/iic_omapl138/iic_omapl138_module.c

libiic_omapl138_la_SOURCES = $(dev_iic_omapl138)
libiic_omapl138_la_LDFLAGS = -module
libiic_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libiic_omapl138.la

dev_iic_xdht = $(I2cSimulateDevice)/iic_xdht/iic_xdht.c $(I2cSimulateDevice)/iic_xdht/iic_xdht_module.c

libiic_xdht_la_SOURCES = $(dev_iic_xdht)
libiic_xdht_la_LDFLAGS = -module
libiic_xdht_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libiic_xdht.la

dev_mpc8378_i2c = $(I2cSimulateDevice)/mpc8378_i2c/mpc8378_i2c.c $(I2cSimulateDevice)/mpc8378_i2c/mpc8378_i2c_module.c

libmpc8378_i2c_la_SOURCES = $(dev_mpc8378_i2c)
libmpc8378_i2c_la_LDFLAGS = -module
libmpc8378_i2c_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8378_i2c.la

dev_sopc_i2c = $(I2cSimulateDevice)/sopc_i2c/sopc_i2c.c $(I2cSimulateDevice)/sopc_i2c/sopc_i2c_module.c

libsopc_i2c_la_SOURCES = $(dev_sopc_i2c)
libsopc_i2c_la_LDFLAGS = -module
libsopc_i2c_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsopc_i2c.la

dev_rm48x_iic = $(I2cSimulateDevice)/rm48x_iic/rm48x_iic.c $(I2cSimulateDevice)/rm48x_iic/rm48x_iic_module.c

librm48x_iic_la_SOURCES = $(dev_rm48x_iic)
librm48x_iic_la_LDFLAGS = -module
librm48x_iic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_iic.la

dev_tca6416 = $(I2cSimulateDevice)/tca6416/tca6416.c $(I2cSimulateDevice)/tca6416/tca6416_module.c

libtca6416_la_SOURCES = $(dev_tca6416)
libtca6416_la_LDFLAGS = -module
libtca6416_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtca6416.la

dev_i2c_c6678 = $(I2cSimulateDevice)/i2c_c6678/i2c_c6678.c $(I2cSimulateDevice)/i2c_c6678/i2c_c6678_module.c

libi2c_c6678_la_SOURCES = $(dev_i2c_c6678)
libi2c_c6678_la_LDFLAGS = -module
libi2c_c6678_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libi2c_c6678.la

IntcSimulateDevice = intcSimulateDevice

dev_aintc_c674x = $(IntcSimulateDevice)/aintc_c674x/aintc_c674x.c $(IntcSimulateDevice)/aintc_c674x/aintc_c674x_module.c

libaintc_c674x_la_SOURCES = $(dev_aintc_c674x)
libaintc_c674x_la_LDFLAGS = -module
libaintc_c674x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libaintc_c674x.la

dev_aintc_omapl138 = $(IntcSimulateDevice)/aintc_omapl138/aintc_omapl138.c $(IntcSimulateDevice)/aintc_omapl138/aintc_omapl138_module.c

libaintc_omapl138_la_SOURCES = $(dev_aintc_omapl138)
libaintc_omapl138_la_LDFLAGS = -module
libaintc_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libaintc_omapl138.la

dev_c687_epic = $(IntcSimulateDevice)/c687_epic/c687_epic.c $(IntcSimulateDevice)/c687_epic/c687_epic_module.c

libc687_epic_la_SOURCES = $(dev_c687_epic)
libc687_epic_la_LDFLAGS = -module
libc687_epic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libc687_epic.la

dev_ft2000a_gic = $(IntcSimulateDevice)/ft2000a_gic/ft2000a_gic.c $(IntcSimulateDevice)/ft2000a_gic/ft2000a_gic_cpu_interface.c $(IntcSimulateDevice)/ft2000a_gic/ft2000a_gic_module.c

libft2000a_gic_la_SOURCES = $(dev_ft2000a_gic)
libft2000a_gic_la_LDFLAGS = -module
libft2000a_gic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_gic.la

dev_GIC_PL390_imx6 = $(IntcSimulateDevice)/GIC_PL390_imx6/GIC_PL390_imx6.c $(IntcSimulateDevice)/GIC_PL390_imx6/GIC_PL390_CPU_Interface.c $(IntcSimulateDevice)/GIC_PL390_imx6/GIC_PL390_Distributor.c $(IntcSimulateDevice)/GIC_PL390_imx6/GIC_PL390_imx6_module.c

libGIC_PL390_imx6_la_SOURCES = $(dev_GIC_PL390_imx6)
libGIC_PL390_imx6_la_LDFLAGS = -module
libGIC_PL390_imx6_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libGIC_PL390_imx6.la

dev_intc_6713 = $(IntcSimulateDevice)/intc_6713/intc_6713.c $(IntcSimulateDevice)/intc_6713/intc_6713_module.c

libintc_6713_la_SOURCES = $(dev_intc_6713)
libintc_6713_la_LDFLAGS = -module
libintc_6713_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libintc_6713.la

dev_intc_am335x = $(IntcSimulateDevice)/intc_am335x/intc_am335x.c $(IntcSimulateDevice)/intc_am335x/intc_am335x_module.c

libintc_am335x_la_SOURCES = $(dev_intc_am335x)
libintc_am335x_la_LDFLAGS = -module
libintc_am335x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libintc_am335x.la

dev_intc_am3359 = $(IntcSimulateDevice)/intc_am3359/intc_am3359.c $(IntcSimulateDevice)/intc_am3359/intc_am3359_module.c

libintc_am3359_la_SOURCES = $(dev_intc_am3359)
libintc_am3359_la_LDFLAGS = -module
libintc_am3359_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libintc_am3359.la

dev_intc_c6678 = $(IntcSimulateDevice)/intc_c6678/intc_c6678.c $(IntcSimulateDevice)/intc_c6678/intc_c6678_module.c

libintc_c6678_la_SOURCES = $(dev_intc_c6678)
libintc_c6678_la_LDFLAGS = -module
libintc_c6678_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libintc_c6678.la

dev_intc_xdht = $(IntcSimulateDevice)/intc_xdht/intc_xdht.c $(IntcSimulateDevice)/intc_xdht/intc_xdht_module.c

libintc_xdht_la_SOURCES = $(dev_intc_xdht)
libintc_xdht_la_LDFLAGS = -module
libintc_xdht_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libintc_xdht.la

dev_ipic_mpc5121 = $(IntcSimulateDevice)/ipic_mpc5121/ipic_mpc5121.c $(IntcSimulateDevice)/ipic_mpc5121/ipic_mpc5121_module.c

libipic_mpc5121_la_SOURCES = $(dev_ipic_mpc5121)
libipic_mpc5121_la_LDFLAGS = -module
libipic_mpc5121_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libipic_mpc5121.la

dev_irq_leon2 = $(IntcSimulateDevice)/irq_leon2/irq_leon2.c $(IntcSimulateDevice)/irq_leon2/irq_leon2_module.c

libirq_leon2_la_SOURCES = $(dev_irq_leon2)
libirq_leon2_la_LDFLAGS = -module
libirq_leon2_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libirq_leon2.la

dev_p2020_pic = $(IntcSimulateDevice)/p2020_pic/p2020_pic.c $(IntcSimulateDevice)/p2020_pic/p2020_pic_module.c

libp2020_pic_la_SOURCES = $(dev_p2020_pic)
libp2020_pic_la_LDFLAGS = -module
libp2020_pic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libp2020_pic.la

dev_pic_mpc837x = $(IntcSimulateDevice)/pic_mpc837x/pic_mpc837x.c $(IntcSimulateDevice)/pic_mpc837x/pic_mpc837x_fifo.c $(IntcSimulateDevice)/pic_mpc837x/pic_mpc837x_module.c

libpic_mpc837x_la_SOURCES = $(dev_pic_mpc837x)
libpic_mpc837x_la_LDFLAGS = -module
libpic_mpc837x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpic_mpc837x.la

dev_pic_mpc8245 = $(IntcSimulateDevice)/pic_mpc8245/pic_mpc8245.c $(IntcSimulateDevice)/pic_mpc8245/pic_mpc8245_module.c

libpic_mpc8245_la_SOURCES = $(dev_pic_mpc8245)
libpic_mpc8245_la_LDFLAGS = -module
libpic_mpc8245_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpic_mpc8245.la

dev_pic_mpc8641d = $(IntcSimulateDevice)/pic_mpc8641d/pic_mpc8641d.c $(IntcSimulateDevice)/pic_mpc8641d/pic_mpc8641d_module.c

libpic_mpc8641d_la_SOURCES = $(dev_pic_mpc8641d)
libpic_mpc8641d_la_LDFLAGS = -module
libpic_mpc8641d_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpic_mpc8641d.la

dev_pic_p1010 = $(IntcSimulateDevice)/pic_p1010/pic_p1010.c $(IntcSimulateDevice)/pic_p1010/pic_p1010_module.c

libpic_p1010_la_SOURCES = $(dev_pic_p1010)
libpic_p1010_la_LDFLAGS = -module
libpic_p1010_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpic_p1010.la

dev_pic_T2080 = $(IntcSimulateDevice)/pic_T2080/pic_T2080.c $(IntcSimulateDevice)/pic_T2080/pic_fifo.c $(IntcSimulateDevice)/pic_T2080/pic_T2080_module.c

libpic_T2080_la_SOURCES = $(dev_pic_T2080)
libpic_T2080_la_LDFLAGS = -module
libpic_T2080_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpic_T2080.la

dev_Pie_28335 = $(IntcSimulateDevice)/Pie_28335/Pie_28335.c $(IntcSimulateDevice)/Pie_28335/pie_fifo.c $(IntcSimulateDevice)/Pie_28335/Pie_28335_module.c

libPie_28335_la_SOURCES = $(dev_Pie_28335)
libPie_28335_la_LDFLAGS = -module
libPie_28335_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libPie_28335.la

dev_Pie_c2812 = $(IntcSimulateDevice)/Pie_c2812/Pie_c2812.c $(IntcSimulateDevice)/Pie_c2812/pie_c2812_fifo.c $(IntcSimulateDevice)/Pie_c2812/Pie_c2812_module.c

libPie_c2812_la_SOURCES = $(dev_Pie_c2812)
libPie_c2812_la_LDFLAGS = -module
libPie_c2812_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libPie_c2812.la

dev_sopc_ictl = $(IntcSimulateDevice)/sopc_ictl/sopc_ictl.c $(IntcSimulateDevice)/sopc_ictl/ictl_fifo.c $(IntcSimulateDevice)/sopc_ictl/sopc_ictl_module.c

libsopc_ictl_la_SOURCES = $(dev_sopc_ictl)
libsopc_ictl_la_LDFLAGS = -module
libsopc_ictl_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsopc_ictl.la

dev_bm3823_intc = $(IntcSimulateDevice)/bm3823_intc/bm3823_intc.c $(IntcSimulateDevice)/bm3823_intc/bm3823_intc_fifo.c $(IntcSimulateDevice)/bm3823_intc/bm3823_intc_module.c

libbm3823_intc_la_SOURCES = $(dev_bm3823_intc)
libbm3823_intc_la_LDFLAGS = -module
libbm3823_intc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libbm3823_intc.la

dev_cic_c6678 = $(IntcSimulateDevice)/cic_c6678/cic_c6678.c $(IntcSimulateDevice)/cic_c6678/cic_c6678_module.c

libcic_c6678_la_SOURCES = $(dev_cic_c6678)
libcic_c6678_la_LDFLAGS = -module
libcic_c6678_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libcic_c6678.la

dev_zynq7000_gic = $(IntcSimulateDevice)/zynq7000_gic/zynq7000_gic.c $(IntcSimulateDevice)/zynq7000_gic/zynq7000_gic_module.c

libzynq7000_gic_la_SOURCES = $(dev_zynq7000_gic)
libzynq7000_gic_la_LDFLAGS = -module
libzynq7000_gic_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libzynq7000_gic.la

LcdSimulateDevice = lcdSimulateDevice

dev_lcd = $(LcdSimulateDevice)/lcd/dev_lcd_au1100.c $(LcdSimulateDevice)/lcd/dev_lcd_s3c2410.c $(LcdSimulateDevice)/lcd/skyeye_lcd.c \
$(LcdSimulateDevice)/lcd/dev_lcd_ep7312.c $(LcdSimulateDevice)/lcd/dev_lcd_s3c44b0x.c $(LcdSimulateDevice)/lcd/dev_lcd_pxa.c $(LcdSimulateDevice)/lcd/lcd_module.c

if GTK_LCD
liblcd_la_SOURCES = $(dev_lcd)
liblcd_la_LDFLAGS = -module $(LCD_LIBS)
liblcd_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += liblcd.la
endif

dev_lcd_gtk = $(LcdSimulateDevice)/lcd_gtk/lcd_gtk.c $(LcdSimulateDevice)/lcd_gtk/lcd_gtk_module.c

if GTK_LCD
liblcd_gtk_la_SOURCES = $(dev_lcd_gtk)
liblcd_gtk_la_LDFLAGS = -module
liblcd_gtk_la_LIBADD = $(LCD_LIBS)
liblcd_gtk_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += liblcd_gtk.la
endif


dev_lcd_omapl138 = $(LcdSimulateDevice)/lcd_omapl138/lcd_omapl138.c $(LcdSimulateDevice)/lcd_omapl138/sdl_pixel.c $(LcdSimulateDevice)/lcd_omapl138/lcd_omapl138_module.c

liblcd_omapl138_la_SOURCES = $(dev_lcd_omapl138)
if WIN32
liblcd_omapl138_la_LDFLAGS = -module -L$(top_srcdir)/third-party/sdl2.0/lib_win -lSDL2
else
if LINUX_64
liblcd_omapl138_la_LDFLAGS = -module -L$(top_srcdir)/third-party/sdl2.0/lib_linux -lSDL2 -Wl,-rpath=$(prefix)/lib
else
liblcd_omapl138_la_LDFLAGS = -module -L$(top_srcdir)/third-party/sdl2.0/lib_linux32 -lSDL2 -Wl,-rpath=$(prefix)/lib
endif

endif
liblcd_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += liblcd_omapl138.la

dev_lcd_s3c6410 = $(LcdSimulateDevice)/lcd_s3c6410/lcd_s3c6410.c $(LcdSimulateDevice)/lcd_s3c6410/lcd_s3c6410_module.c

liblcd_s3c6410_la_SOURCES = $(dev_lcd_s3c6410)
liblcd_s3c6410_la_LDFLAGS = -module
liblcd_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += liblcd_s3c6410.la

dev_lcd_sdl = $(LcdSimulateDevice)/lcd_sdl/lcd_sdl.c $(LcdSimulateDevice)/lcd_sdl/lcd_sdl_module.c

liblcd_sdl_la_SOURCES = $(dev_lcd_sdl)

if LINUX_64
liblcd_sdl_la_LDFLAGS = -module -L$(top_srcdir)/third-party/sdl2.0/lib_linux -lSDL2 -Wl,-rpath=$(prefix)/lib
else
liblcd_sdl_la_LDFLAGS = -module -L$(top_srcdir)/third-party/sdl2.0/lib_linux32 -lSDL2 -Wl,-rpath=$(prefix)/lib
endif

liblcd_sdl_la_LDFLAGS += $(WIN32_LD_DEVICE)

pkglib_LTLIBRARIES += liblcd_sdl.la

dev_uc1608_lcd = $(LcdSimulateDevice)/uc1608_lcd/uc1608_lcd.c $(LcdSimulateDevice)/uc1608_lcd/uc1608_sdl.c $(LcdSimulateDevice)/uc1608_lcd/uc1608_lcd_module.c

libuc1608_lcd_la_SOURCES = $(dev_uc1608_lcd)
if WIN32
libuc1608_lcd_la_LDFLAGS = -module -L$(top_srcdir)/third-party/sdl2.0/lib_win -lSDL2
else

if LINUX_64
libuc1608_lcd_la_LDFLAGS = -module -L$(top_srcdir)/third-party/sdl2.0/lib_linux -lSDL2 -Wl,-rpath=$(prefix)/lib
else
libuc1608_lcd_la_LDFLAGS = -module -L$(top_srcdir)/third-party/sdl2.0/lib_linux32 -lSDL2 -Wl,-rpath=$(prefix)/lib
endif

endif
libuc1608_lcd_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuc1608_lcd.la

LinkerSimulateDevice = linkerSimulateDevice

dev_ad_linker = $(LinkerSimulateDevice)/ad_linker/ad_linker.c $(LinkerSimulateDevice)/ad_linker/ad_linker_module.c

libad_linker_la_SOURCES = $(dev_ad_linker)
libad_linker_la_LDFLAGS = -module
libad_linker_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libad_linker.la

dev_atm_link = $(LinkerSimulateDevice)/atm_link/atm_link.c $(LinkerSimulateDevice)/atm_link/atm_link_module.c

libatm_link_la_SOURCES = $(dev_atm_link)
libatm_link_la_LDFLAGS = -module
libatm_link_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libatm_link.la

dev_bu61580_bridge = $(LinkerSimulateDevice)/bu61580_bridge/bu61580_bridge.c $(LinkerSimulateDevice)/bu61580_bridge/bu61580_bridge_fifo.c $(LinkerSimulateDevice)/bu61580_bridge/bu61580_bridge_module.c

libbu61580_bridge_la_SOURCES = $(dev_bu61580_bridge)
libbu61580_bridge_la_LDFLAGS = -module
libbu61580_bridge_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libbu61580_bridge.la

dev_can_bridge = $(LinkerSimulateDevice)/can_bridge/can_bridge.c $(LinkerSimulateDevice)/can_bridge/can_bridge_fifo.c $(LinkerSimulateDevice)/can_bridge/can_bridge_module.c

libcan_bridge_la_SOURCES = $(dev_can_bridge)
libcan_bridge_la_LDFLAGS = -module
libcan_bridge_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libcan_bridge.la

dev_can_exc_bridge = $(LinkerSimulateDevice)/can_exc_bridge/can_exc_bridge.c $(LinkerSimulateDevice)/can_exc_bridge/can_exc_bridge_module.c

libcan_exc_bridge_la_SOURCES = $(dev_can_exc_bridge)
libcan_exc_bridge_la_LDFLAGS = -module
libcan_exc_bridge_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libcan_exc_bridge.la

dev_can_link = $(LinkerSimulateDevice)/can_link/can_link.c $(LinkerSimulateDevice)/can_link/can_link_module.c

libcan_link_la_SOURCES = $(dev_can_link)
libcan_link_la_LDFLAGS = -module
libcan_link_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libcan_link.la

dev_dmc_link = $(LinkerSimulateDevice)/dmc_link/dmc_link.c $(LinkerSimulateDevice)/dmc_link/dmc_link_module.c

libdmc_link_la_SOURCES = $(dev_dmc_link)
libdmc_link_la_LDFLAGS = -module
libdmc_link_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libdmc_link.la

dev_gpio_connector = $(LinkerSimulateDevice)/gpio_connector/gpio_connector.c $(LinkerSimulateDevice)/gpio_connector/gpio_connector_module.c

libgpio_connector_la_SOURCES = $(dev_gpio_connector)
libgpio_connector_la_LDFLAGS = -module
libgpio_connector_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_connector.la

dev_gpio_linker = $(LinkerSimulateDevice)/gpio_linker/gpio_linker.c $(LinkerSimulateDevice)/gpio_linker/gpio_linker_module.c

libgpio_linker_la_SOURCES = $(dev_gpio_linker)
libgpio_linker_la_LDFLAGS = -module
libgpio_linker_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libgpio_linker.la

dev_io_linker = $(LinkerSimulateDevice)/io_linker/io_linker.c $(LinkerSimulateDevice)/io_linker/io_linker_module.c

libio_linker_la_SOURCES = $(dev_io_linker)
libio_linker_la_LDFLAGS = -module
libio_linker_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libio_linker.la

dev_Lvds_bus = $(LinkerSimulateDevice)/Lvds_bus/Lvds_bus.c $(LinkerSimulateDevice)/Lvds_bus/Lvds_bus_module.c

libLvds_bus_la_SOURCES = $(dev_Lvds_bus)
libLvds_bus_la_LDFLAGS = -module
libLvds_bus_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libLvds_bus.la

dev_McBSP_linker = $(LinkerSimulateDevice)/McBSP_linker/McBSP_linker.c $(LinkerSimulateDevice)/McBSP_linker/McBSP_linker_module.c

libMcBSP_linker_la_SOURCES = $(dev_McBSP_linker)
libMcBSP_linker_la_LDFLAGS = -module
libMcBSP_linker_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libMcBSP_linker.la

dev_rs422_bridge = $(LinkerSimulateDevice)/rs422_bridge/rs422_bridge.c $(LinkerSimulateDevice)/rs422_bridge/rs422_bridge_fifo.c $(LinkerSimulateDevice)/rs422_bridge/rs422_bridge_module.c

librs422_bridge_la_SOURCES = $(dev_rs422_bridge)
librs422_bridge_la_LDFLAGS = -module
librs422_bridge_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librs422_bridge.la

dev_rs422_linker = $(LinkerSimulateDevice)/rs422_linker/rs422_linker.c $(LinkerSimulateDevice)/rs422_linker/rs422_linker_module.c

librs422_linker_la_SOURCES = $(dev_rs422_linker)
librs422_linker_la_LDFLAGS = -module
librs422_linker_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librs422_linker.la

dev_Rs485_bus = $(LinkerSimulateDevice)/Rs485_bus/Rs485_bus.c $(LinkerSimulateDevice)/Rs485_bus/Rs485_bus_module.c

libRs485_bus_la_SOURCES = $(dev_Rs485_bus)
libRs485_bus_la_LDFLAGS = -module
libRs485_bus_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libRs485_bus.la

dev_tsi578_link = $(LinkerSimulateDevice)/tsi578_link/tsi578_link.c $(LinkerSimulateDevice)/tsi578_link/tsi578_link_module.c

libtsi578_link_la_SOURCES = $(dev_tsi578_link)
libtsi578_link_la_LDFLAGS = -module
libtsi578_link_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtsi578_link.la

dev_upp_linker = $(LinkerSimulateDevice)/upp_linker/upp_linker.c $(LinkerSimulateDevice)/upp_linker/upp_linker_module.c

libupp_linker_la_SOURCES = $(dev_upp_linker)
libupp_linker_la_LDFLAGS = -module
libupp_linker_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libupp_linker.la

dev_Rs232_bus = $(LinkerSimulateDevice)/Rs232_bus/Rs232_bus.c $(LinkerSimulateDevice)/Rs232_bus/Rs232_bus_module.c

libRs232_bus_la_SOURCES = $(dev_Rs232_bus)
libRs232_bus_la_LDFLAGS = -module
libRs232_bus_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libRs232_bus.la

dev_Rs422_bus = $(LinkerSimulateDevice)/Rs422_bus/Rs422_bus.c $(LinkerSimulateDevice)/Rs422_bus/Rs422_bus_module.c

libRs422_bus_la_SOURCES = $(dev_Rs422_bus)
libRs422_bus_la_LDFLAGS = -module
libRs422_bus_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libRs422_bus.la

dev_i2c_bus = $(LinkerSimulateDevice)/i2c/i2c_bus.c $(LinkerSimulateDevice)/i2c/i2c_bus_module.c

libi2c_bus_la_SOURCES = $(dev_i2c_bus)
libi2c_bus_la_LDFLAGS = -module
libi2c_bus_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libi2c_bus.la

dev_spi_bus = $(LinkerSimulateDevice)/spi/spi_bus.c $(LinkerSimulateDevice)/spi/spi_bus_module.c

libspi_bus_la_SOURCES = $(dev_spi_bus)
libspi_bus_la_LDFLAGS = -module
libspi_bus_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libspi_bus.la

dev_a429_bus = $(LinkerSimulateDevice)/arinc_429_bus/arinc_429_bus.c $(LinkerSimulateDevice)/arinc_429_bus/arinc_429_bus_module.c

liba429_bus_la_SOURCES = $(dev_a429_bus)
liba429_bus_la_LDFLAGS = -module
liba429_bus_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += liba429_bus.la

dev_pulse_linker = $(LinkerSimulateDevice)/pulse_linker/pulse_linker.c $(LinkerSimulateDevice)/pulse_linker/pulse_linker_module.c

libpulse_linker_la_SOURCES = $(dev_pulse_linker)
libpulse_linker_la_LDFLAGS = -module
libpulse_linker_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpulse_linker.la

dev_sparc_ctrl_bridge = $(LinkerSimulateDevice)/sparc_ctrl_bridge/sparc_ctrl_bridge.c $(LinkerSimulateDevice)/sparc_ctrl_bridge/sparc_ctrl_bridge_module.c

libsparc_ctrl_bridge_la_SOURCES = $(dev_sparc_ctrl_bridge)
libsparc_ctrl_bridge_la_LDFLAGS = -module
libsparc_ctrl_bridge_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsparc_ctrl_bridge.la

dev_pcie_linker = $(LinkerSimulateDevice)/pcie_linker/pcie_linker.c $(LinkerSimulateDevice)/pcie_linker/pcie_linker_module.c

libpcie_linker_la_SOURCES = $(dev_pcie_linker)
libpcie_linker_la_LDFLAGS = -module
libpcie_linker_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpcie_linker.la

dev_power_link = $(LinkerSimulateDevice)/power_link/power_link.c $(LinkerSimulateDevice)/power_link/power_link_module.c

libpower_link_la_SOURCES = $(dev_power_link)
libpower_link_la_LDFLAGS = -module
libpower_link_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libpower_link.la

dev_load_file_bridge = $(LinkerSimulateDevice)/load_file_bridge/load_file_bridge.c $(LinkerSimulateDevice)/load_file_bridge/load_file_fifo.c $(LinkerSimulateDevice)/load_file_bridge/load_file_bridge_module.c

libload_file_bridge_la_SOURCES = $(dev_load_file_bridge)
libload_file_bridge_la_LDFLAGS = -module
libload_file_bridge_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libload_file_bridge.la

McbspSimulateDevice = mcbspSimulateDevice

dev_fifo_McBSP_c6748 = $(McbspSimulateDevice)/fifo_McBSP_c6748/fifo_McBSP_c6748.c $(McbspSimulateDevice)/McBSP_c6748/McBSP_fifo.c $(McbspSimulateDevice)/fifo_McBSP_c6748/fifo_McBSP_c6748_module.c

libfifo_McBSP_c6748_la_SOURCES = $(dev_fifo_McBSP_c6748)
libfifo_McBSP_c6748_la_LDFLAGS = -module
libfifo_McBSP_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libfifo_McBSP_c6748.la

dev_mcbsp_c5510 = $(McbspSimulateDevice)/mcbsp_c5510/mcbsp_c5510.c $(McbspSimulateDevice)/mcbsp_c5510/mcbsp_c5510_module.c

libmcbsp_c5510_la_SOURCES = $(dev_mcbsp_c5510)
libmcbsp_c5510_la_LDFLAGS = -module
libmcbsp_c5510_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmcbsp_c5510.la

dev_McBSP_c6748 = $(McbspSimulateDevice)/McBSP_c6748/McBSP_c6748.c $(McbspSimulateDevice)/McBSP_c6748/McBSP_fifo.c $(McbspSimulateDevice)/McBSP_c6748/McBSP_c6748_module.c

libMcBSP_c6748_la_SOURCES = $(dev_McBSP_c6748)
libMcBSP_c6748_la_LDFLAGS = -module
libMcBSP_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libMcBSP_c6748.la

dev_mcbsp_f28335 = $(McbspSimulateDevice)/mcbsp_f28335/mcbsp_f28335.c $(McbspSimulateDevice)/mcbsp_f28335/mcbsp_f28335_module.c

libmcbsp_f28335_la_SOURCES = $(dev_mcbsp_f28335)
libmcbsp_f28335_la_LDFLAGS = -module
libmcbsp_f28335_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmcbsp_f28335.la

NetSimulateDevice = netSimulateDevice

dev_BMan_T2080 = $(NetSimulateDevice)/BMan_T2080/BMan_T2080.c $(NetSimulateDevice)/BMan_T2080/BMan_T2080_module.c

libBMan_T2080_la_SOURCES = $(dev_BMan_T2080)
libBMan_T2080_la_LDFLAGS = -module
libBMan_T2080_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libBMan_T2080.la

dev_FMan_T2080 = $(NetSimulateDevice)/FMan_T2080/FMan_T2080.c $(NetSimulateDevice)/FMan_T2080/FMan_T2080_module.c

libFMan_T2080_la_SOURCES = $(dev_FMan_T2080)
libFMan_T2080_la_LDFLAGS = -module
libFMan_T2080_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libFMan_T2080.la

dev_QMan_T2080 = $(NetSimulateDevice)/QMan_T2080/QMan_T2080.c $(NetSimulateDevice)/QMan_T2080/QMan_T2080_module.c

libQMan_T2080_la_SOURCES = $(dev_QMan_T2080)
libQMan_T2080_la_LDFLAGS = -module
libQMan_T2080_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libQMan_T2080.la

dev_emac_ctl_omapl138 = $(NetSimulateDevice)/emac_ctl_omapl138/emac_ctl_omapl138.c $(NetSimulateDevice)/emac_ctl_omapl138/emac_ctl_omapl138_module.c

libemac_ctl_omapl138_la_SOURCES = $(dev_emac_ctl_omapl138)
libemac_ctl_omapl138_la_LDFLAGS = -module
libemac_ctl_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libemac_ctl_omapl138.la

dev_emac_mdio_omapl138 = $(NetSimulateDevice)/emac_mdio_omapl138/emac_mdio_omapl138.c $(NetSimulateDevice)/emac_mdio_omapl138/emac_mdio_omapl138_module.c

libemac_mdio_omapl138_la_SOURCES = $(dev_emac_mdio_omapl138)
libemac_mdio_omapl138_la_LDFLAGS = -module
libemac_mdio_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libemac_mdio_omapl138.la

if WIN32
dev_emac_module_omapl138 = $(NetSimulateDevice)/emac_module_omapl138/emac_module_omapl138.c $(NetSimulateDevice)/emac_module_omapl138/emac_module_omapl138_module.c
dev_emac_module_c6748 = $(NetSimulateDevice)/emac_module_c6748/emac_module_c6748.c $(NetSimulateDevice)/emac_module_c6748/emac_module_c6748_module.c
else
dev_emac_module_omapl138 = $(NetSimulateDevice)/emac_module_omapl138/emac_module_omapl138.c $(NetSimulateDevice)/emac_module_omapl138/emac_module_omapl138_module.c $(NetSimulateDevice)/emac_module_omapl138/sniffer.c
dev_emac_module_c6748 = $(NetSimulateDevice)/emac_module_c6748/emac_module_c6748.c $(NetSimulateDevice)/emac_module_c6748/emac_module_c6748_module.c $(NetSimulateDevice)/emac_module_c6748/c6748_sniffer.c
endif

libemac_module_c6748_la_SOURCES = $(dev_emac_module_c6748)
libemac_module_c6748_la_LDFLAGS = -module
libemac_module_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libemac_module_c6748.la

libemac_module_omapl138_la_SOURCES = $(dev_emac_module_omapl138)
libemac_module_omapl138_la_LDFLAGS = -module
libemac_module_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libemac_module_omapl138.la

dev_eTSEC_8641d = $(NetSimulateDevice)/eTSEC_8641d/eTSEC_8641d.c $(NetSimulateDevice)/eTSEC_8641d/eTSEC_8641d_module.c

libeTSEC_8641d_la_SOURCES = $(dev_eTSEC_8641d)
libeTSEC_8641d_la_LDFLAGS = -module
libeTSEC_8641d_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libeTSEC_8641d.la

dev_ft2000a_gmac = $(NetSimulateDevice)/ft2000a_gmac/ft2000a_gmac.c $(NetSimulateDevice)/ft2000a_gmac/ft2000a_gmac_config.c $(NetSimulateDevice)/ft2000a_gmac/ft2000a_gmac_module.c

libft2000a_gmac_la_SOURCES = $(dev_ft2000a_gmac)
libft2000a_gmac_la_LDFLAGS = -module
libft2000a_gmac_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_gmac.la

dev_linux_netsim = $(NetSimulateDevice)/linux_netsim/linux_netsim.c $(NetSimulateDevice)/linux_netsim/linux_netsim_module.c

liblinux_netsim_la_SOURCES = $(dev_linux_netsim)
liblinux_netsim_la_LDFLAGS = -module

dev_win_netsim = $(NetSimulateDevice)/win_netsim/win_netsim.cpp $(NetSimulateDevice)/win_netsim/win_netsim_module.c

libwin_netsim_la_SOURCES = $(dev_win_netsim)
libwin_netsim_la_LDFLAGS = -module
libwin_netsim_la_LDFLAGS += $(WIN32_LD_DEVICE)

if WIN32
pkglib_LTLIBRARIES += libwin_netsim.la
else
pkglib_LTLIBRARIES += liblinux_netsim.la
endif

dev_net = $(NetSimulateDevice)/net/dev_net_cs8900a.c  $(NetSimulateDevice)/net/skyeye_net_tuntap.c \
$(NetSimulateDevice)/net/dev_net_rtl8019.c $(NetSimulateDevice)/net/skyeye_net_vhub.c $(NetSimulateDevice)/net/skyeye_net.c $(NetSimulateDevice)/net/skyeye_net_vnet.c $(NetSimulateDevice)/net/net_module.c

libnet_la_SOURCES = $(dev_net)
libnet_la_LDFLAGS = -module
libnet_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libnet.la

dev_w5100_end = $(NetSimulateDevice)/w5100_end/w5100_end.c $(NetSimulateDevice)/w5100_end/socket_operation.c $(NetSimulateDevice)/w5100_end/w5100_end_module.c

libw5100_end_la_SOURCES = $(dev_w5100_end)
libw5100_end_la_LDFLAGS = -module
libw5100_end_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libw5100_end.la

RamSimulateDevice = ramSimulateDevice

dev_image = $(RamSimulateDevice)/image/image.c $(RamSimulateDevice)/image/image_module.c

libimage_la_SOURCES = $(dev_image)
libimage_la_LDFLAGS = -module
libimage_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libimage.la

dev_ram = $(RamSimulateDevice)/ram/ram.c $(RamSimulateDevice)/ram/ram_module.c

libram_la_SOURCES = $(dev_ram)
libram_la_LDFLAGS = -module
libram_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libram.la

dev_ram16 = $(RamSimulateDevice)/ram16/ram16.c $(RamSimulateDevice)/ram16/ram16_module.c

libram16_la_SOURCES = $(dev_ram16)
libram16_la_LDFLAGS = -module
libram16_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libram16.la

Rs422SimulateDevice = rs422SimulateDevice

dev_adk_rs422 = $(Rs422SimulateDevice)/adk_rs422/adk_rs422.c $(Rs422SimulateDevice)/adk_rs422/adk_rs422_module.c

libadk_rs422_la_SOURCES = $(dev_adk_rs422)
libadk_rs422_la_LDFLAGS = -module
libadk_rs422_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libadk_rs422.la

dev_rs232_serial = $(Rs422SimulateDevice)/rs232_serial/rs232_serial.c $(Rs422SimulateDevice)/rs232_serial/rs232_serial_module.c

librs232_serial_la_SOURCES = $(dev_rs232_serial)
librs232_serial_la_LDFLAGS = -module
librs232_serial_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librs232_serial.la

dev_Rs422_serial = $(Rs422SimulateDevice)/Rs422_serial/Rs422_serial.c $(Rs422SimulateDevice)/Rs422_serial/Rs422_serial_module.c

libRs422_serial_la_SOURCES = $(dev_Rs422_serial)
libRs422_serial_la_LDFLAGS = -module
libRs422_serial_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libRs422_serial.la

dev_rs422_serial_port = $(Rs422SimulateDevice)/rs422_serial_port/rs422_serial_port.c $(Rs422SimulateDevice)/rs422_serial_port/rs422_serial_port_module.c

librs422_serial_port_la_SOURCES = $(dev_rs422_serial_port)
librs422_serial_port_la_LDFLAGS = -module
librs422_serial_port_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librs422_serial_port.la

dev_rs422_uart = $(Rs422SimulateDevice)/rs422_uart/rs422_uart.c $(Rs422SimulateDevice)/rs422_uart/rs422_uart_module.c

librs422_uart_la_SOURCES = $(dev_rs422_uart)
librs422_uart_la_LDFLAGS = -module
librs422_uart_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librs422_uart.la

Rs485SimulateDevice = rs485SimulateDevice

dev_HDLC_Rs485 = $(Rs485SimulateDevice)/HDLC_Rs485/HDLC_Rs485.c $(Rs485SimulateDevice)/HDLC_Rs485/HDLC_Rs485_module.c

libHDLC_Rs485_la_SOURCES = $(dev_HDLC_Rs485)
libHDLC_Rs485_la_LDFLAGS = -module
libHDLC_Rs485_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libHDLC_Rs485.la

dev_Rs485_serial = $(Rs485SimulateDevice)/Rs485_serial/Rs485_serial.c $(Rs485SimulateDevice)/Rs485_serial/Rs485_serial_module.c

libRs485_serial_la_SOURCES = $(dev_Rs485_serial)
libRs485_serial_la_LDFLAGS = -module
libRs485_serial_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libRs485_serial.la

RtcSimulateDevice = rtcSimulateDevice

dev_rtc_am335x = $(RtcSimulateDevice)/rtc_am335x/rtc_am335x.c $(RtcSimulateDevice)/rtc_am335x/rtc_am335x_module.c

librtc_am335x_la_SOURCES = $(dev_rtc_am335x)
librtc_am335x_la_LDFLAGS = -module
librtc_am335x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librtc_am335x.la

dev_rtc_c6748 = $(RtcSimulateDevice)/rtc_c6748/rtc_c6748.c $(RtcSimulateDevice)/rtc_c6748/rtc_c6748_module.c

librtc_c6748_la_SOURCES = $(dev_rtc_c6748)
librtc_c6748_la_LDFLAGS = -module
librtc_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librtc_c6748.la

dev_rtc_s3c6410 = $(RtcSimulateDevice)/rtc_s3c6410/rtc_s3c6410.c $(RtcSimulateDevice)/rtc_s3c6410/rtc_s3c6410_module.c

librtc_s3c6410_la_SOURCES = $(dev_rtc_s3c6410)
librtc_s3c6410_la_LDFLAGS = -module
librtc_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librtc_s3c6410.la

SciSimulateDevice = sciSimulateDevice

dev_rm48x_sci = $(SciSimulateDevice)/rm48x_sci/rm48x_sci.c $(SciSimulateDevice)/rm48x_sci/rm48x_sci_fifo.c $(SciSimulateDevice)/rm48x_sci/rm48x_sci_module.c

librm48x_sci_la_SOURCES = $(dev_rm48x_sci)
librm48x_sci_la_LDFLAGS = -module
librm48x_sci_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_sci.la

dev_Sci_28335 = $(SciSimulateDevice)/Sci_28335/sci_28335.c $(SciSimulateDevice)/Sci_28335/sci_fifo.c $(SciSimulateDevice)/Sci_28335/sci_28335_module.c

libSci_28335_la_SOURCES = $(dev_Sci_28335)
libSci_28335_la_LDFLAGS = -module
libSci_28335_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libSci_28335.la

SoundSimulateDevice = soundSimulateDevice

dev_ac97_s3c6410 = $(SoundSimulateDevice)/ac97_s3c6410/ac97_s3c6410.c $(SoundSimulateDevice)/ac97_s3c6410/ac97_s3c6410_module.c

libac97_s3c6410_la_SOURCES = $(dev_ac97_s3c6410)
libac97_s3c6410_la_LDFLAGS = -module
libac97_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libac97_s3c6410.la

dev_aic31 = $(SoundSimulateDevice)/aic31/aic31.c $(SoundSimulateDevice)/aic31/sound_pcm.c $(SoundSimulateDevice)/aic31/aic31_module.c

libaic31_la_SOURCES = $(dev_aic31)
libaic31_la_LDFLAGS = -module
libaic31_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libaic31.la

dev_sound = $(SoundSimulateDevice)/sound/skyeye_sound.c  $(SoundSimulateDevice)/sound/skyeye_sound_pcm.c $(SoundSimulateDevice)/sound/sound_module.c

libsound_la_SOURCES = $(dev_sound)
libsound_la_LDFLAGS = -module
libsound_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsound.la

dev_vic_at91rm9200 = $(SoundSimulateDevice)/vic_at91rm9200/vic_at91rm9200.c $(SoundSimulateDevice)/vic_at91rm9200/vic_at91rm9200_module.c

libvic_at91rm9200_la_SOURCES = $(dev_vic_at91rm9200)
libvic_at91rm9200_la_LDFLAGS = -module
libvic_at91rm9200_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libvic_at91rm9200.la

dev_vic_s3c2410x = $(SoundSimulateDevice)/vic_s3c2410x/vic_s3c2410x.c $(SoundSimulateDevice)/vic_s3c2410x/vic_s3c2410x_module.c

libvic_s3c2410x_la_SOURCES = $(dev_vic_s3c2410x)
libvic_s3c2410x_la_LDFLAGS = -module
libvic_s3c2410x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libvic_s3c2410x.la

dev_vic_s3c6410 = $(SoundSimulateDevice)/vic_s3c6410/vic_s3c6410.c $(SoundSimulateDevice)/vic_s3c6410/vic_s3c6410_module.c

libvic_s3c6410_la_SOURCES = $(dev_vic_s3c6410)
libvic_s3c6410_la_LDFLAGS = -module
libvic_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libvic_s3c6410.la

SpiSimulateDevice = spiSimulateDevice

dev_ft2000a_spi = $(SpiSimulateDevice)/ft2000a_spi/ft2000a_spi.c $(SpiSimulateDevice)/ft2000a_spi/ft2000a_spi_module.c

libft2000a_spi_la_SOURCES = $(dev_ft2000a_spi)
libft2000a_spi_la_LDFLAGS = -module
libft2000a_spi_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_spi.la

dev_max1253 = $(SpiSimulateDevice)/max1253/max1253.c $(SpiSimulateDevice)/max1253/max1253_module.c

libmax1253_la_SOURCES = $(dev_max1253)
libmax1253_la_LDFLAGS = -module
libmax1253_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmax1253.la

dev_rm48x_spi = $(SpiSimulateDevice)/rm48x_spi/rm48x_spi.c $(SpiSimulateDevice)/rm48x_spi/rm48x_spi_fifo.c $(SpiSimulateDevice)/rm48x_spi/rm48x_spi_module.c

librm48x_spi_la_SOURCES = $(dev_rm48x_spi)
librm48x_spi_la_LDFLAGS = -module
librm48x_spi_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_spi.la

dev_sopc_spi = $(SpiSimulateDevice)/sopc_spi/sopc_spi.c $(SpiSimulateDevice)/sopc_spi/sopc_spi_fifo.c $(SpiSimulateDevice)/sopc_spi/sopc_spi_module.c

libsopc_spi_la_SOURCES = $(dev_sopc_spi)
libsopc_spi_la_LDFLAGS = -module
libsopc_spi_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsopc_spi.la

dev_Spi_28335 = $(SpiSimulateDevice)/Spi_28335/spi_28335.c $(SpiSimulateDevice)/Spi_28335/spi_28335_module.c

libSpi_28335_la_SOURCES = $(dev_Spi_28335)
libSpi_28335_la_LDFLAGS = -module
libSpi_28335_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libSpi_28335.la

dev_spi_am335x = $(SpiSimulateDevice)/spi_am335x/spi_am335x.c $(SpiSimulateDevice)/spi_am335x/spi_am335x_module.c

libspi_am335x_la_SOURCES = $(dev_spi_am335x)
libspi_am335x_la_LDFLAGS = -module
libspi_am335x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libspi_am335x.la

dev_spi_m25p128 = $(SpiSimulateDevice)/spi_m25p128/spi_m25p128.c $(SpiSimulateDevice)/spi_m25p128/m25p128.c $(SpiSimulateDevice)/spi_m25p128/spi_m25p128_module.c

libspi_m25p128_la_SOURCES = $(dev_spi_m25p128)
libspi_m25p128_la_LDFLAGS = -module
libspi_m25p128_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libspi_m25p128.la

dev_spi_mx25l128 = $(SpiSimulateDevice)/spi_mx25l128/spi_mx25l128.c $(SpiSimulateDevice)/spi_mx25l128/mx25l128.c $(SpiSimulateDevice)/spi_mx25l128/spi_mx25l128_module.c

libspi_mx25l128_la_SOURCES = $(dev_spi_mx25l128)
libspi_mx25l128_la_LDFLAGS = -module
libspi_mx25l128_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libspi_mx25l128.la

dev_spi_omapl138 = $(SpiSimulateDevice)/spi_omapl138/spi_omapl138.c $(SpiSimulateDevice)/spi_omapl138/omapl138_spi_fifo.c $(SpiSimulateDevice)/spi_omapl138/spi_omapl138_module.c

libspi_omapl138_la_SOURCES = $(dev_spi_omapl138)
libspi_omapl138_la_LDFLAGS = -module
libspi_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libspi_omapl138.la

dev_spi_s3c6410 = $(SpiSimulateDevice)/spi_s3c6410/spi_s3c6410.c $(SpiSimulateDevice)/spi_s3c6410/spi_s3c6410_module.c

libspi_s3c6410_la_SOURCES = $(dev_spi_s3c6410)
libspi_s3c6410_la_LDFLAGS = -module
libspi_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libspi_s3c6410.la

dev_spi_xdht = $(SpiSimulateDevice)/spi_xdht/spi_xdht.c $(SpiSimulateDevice)/spi_xdht/spi_xdht_module.c

libspi_xdht_la_SOURCES = $(dev_spi_xdht)
libspi_xdht_la_LDFLAGS = -module
libspi_xdht_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libspi_xdht.la

dev_spi_c6678 = $(SpiSimulateDevice)/spi_c6678/spi_c6678.c $(SpiSimulateDevice)/spi_c6678/spi_c6678_module.c

libspi_c6678_la_SOURCES = $(dev_spi_c6678)
libspi_c6678_la_LDFLAGS = -module
libspi_c6678_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libspi_c6678.la

SysctrlSimulateDevice = sysctrlSimulateDevice

dev_ctrl_am3359 = $(SysctrlSimulateDevice)/ctrl_am3359/ctrl_am3359.c $(SysctrlSimulateDevice)/ctrl_am3359/ctrl_am3359_module.c

libctrl_am3359_la_SOURCES = $(dev_ctrl_am3359)
libctrl_am3359_la_LDFLAGS = -module
libctrl_am3359_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libctrl_am3359.la

dev_rm48x_sys = $(SysctrlSimulateDevice)/rm48x_sys/rm48x_sys.c $(SysctrlSimulateDevice)/rm48x_sys/rm48x_sys_module.c

librm48x_sys_la_SOURCES = $(dev_rm48x_sys)
librm48x_sys_la_LDFLAGS = -module
librm48x_sys_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_sys.la

dev_sopc_sysctrl = $(SysctrlSimulateDevice)/sopc_sysctrl/sopc_sysctrl.c $(SysctrlSimulateDevice)/sopc_sysctrl/sopc_sysctrl_module.c

libsopc_sysctrl_la_SOURCES = $(dev_sopc_sysctrl)
libsopc_sysctrl_la_LDFLAGS = -module
libsopc_sysctrl_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsopc_sysctrl.la

dev_Sysctrl_28335 = $(SysctrlSimulateDevice)/Sysctrl_28335/sysctrl_28335.c $(SysctrlSimulateDevice)/Sysctrl_28335/sysctrl_28335_module.c

libSysctrl_28335_la_SOURCES = $(dev_Sysctrl_28335)
libSysctrl_28335_la_LDFLAGS = -module
libSysctrl_28335_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libSysctrl_28335.la

dev_sysctrl_at91rm9200 = $(SysctrlSimulateDevice)/sysctrl_at91rm9200/sysctrl_at91rm9200.c $(SysctrlSimulateDevice)/sysctrl_at91rm9200/sysctrl_at91rm9200_module.c

libsysctrl_at91rm9200_la_SOURCES = $(dev_sysctrl_at91rm9200)
libsysctrl_at91rm9200_la_LDFLAGS = -module
libsysctrl_at91rm9200_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsysctrl_at91rm9200.la

dev_sysctrl_c2812 = $(SysctrlSimulateDevice)/sysctrl_c2812/sysctrl_c2812.c $(SysctrlSimulateDevice)/sysctrl_c2812/sysctrl_c2812_module.c

libsysctrl_c2812_la_SOURCES = $(dev_sysctrl_c2812)
libsysctrl_c2812_la_LDFLAGS = -module
libsysctrl_c2812_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsysctrl_c2812.la

dev_sysctrl_c5510 = $(SysctrlSimulateDevice)/sysctrl_c5510/sysctrl_c5510.c $(SysctrlSimulateDevice)/sysctrl_c5510/sysctrl_c5510_module.c

libsysctrl_c5510_la_SOURCES = $(dev_sysctrl_c5510)
libsysctrl_c5510_la_LDFLAGS = -module
libsysctrl_c5510_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsysctrl_c5510.la

dev_sysctrl_s3c2410x = $(SysctrlSimulateDevice)/sysctrl_s3c2410x/sysctrl_s3c2410x.c $(SysctrlSimulateDevice)/sysctrl_s3c2410x/sysctrl_s3c2410x_module.c

libsysctrl_s3c2410x_la_SOURCES = $(dev_sysctrl_s3c2410x)
libsysctrl_s3c2410x_la_LDFLAGS = -module
libsysctrl_s3c2410x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsysctrl_s3c2410x.la

dev_sysctrl_s3c6410 = $(SysctrlSimulateDevice)/sysctrl_s3c6410/sysctrl_s3c6410.c $(SysctrlSimulateDevice)/sysctrl_s3c6410/sysctrl_s3c6410_module.c

libsysctrl_s3c6410_la_SOURCES = $(dev_sysctrl_s3c6410)
libsysctrl_s3c6410_la_LDFLAGS = -module
libsysctrl_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsysctrl_s3c6410.la

dev_sysctrl_sparc = $(SysctrlSimulateDevice)/sysctrl_sparc/sysctrl_sparc.c $(SysctrlSimulateDevice)/sysctrl_sparc/sysctrl_fifo.c $(SysctrlSimulateDevice)/sysctrl_sparc/sysctrl_sparc_module.c

libsysctrl_sparc_la_SOURCES = $(dev_sysctrl_sparc)
libsysctrl_sparc_la_LDFLAGS = -module
libsysctrl_sparc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsysctrl_sparc.la

SystickSimulateDevice = systickSimulateDevice

dev_ds3231 = $(SystickSimulateDevice)/ds3231/ds3231.c $(SystickSimulateDevice)/ds3231/ds3231_module.c

libds3231_la_SOURCES = $(dev_ds3231)
libds3231_la_LDFLAGS = -module
libds3231_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libds3231.la

dev_systick_stm32f0xx = $(SystickSimulateDevice)/systick_stm32f0xx/systick_stm32f0xx.c $(SystickSimulateDevice)/systick_stm32f0xx/systick_stm32f0xx_module.c

libsystick_stm32f0xx_la_SOURCES = $(dev_systick_stm32f0xx)
libsystick_stm32f0xx_la_LDFLAGS = -module
libsystick_stm32f0xx_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsystick_stm32f0xx.la

TestSimulateDevice = testSimulateDevice

dev_api_test_dev = $(TestSimulateDevice)/api_test_dev/api_test_dev.c $(TestSimulateDevice)/api_test_dev/api_test_dev_module.c

libapi_test_dev_la_SOURCES = $(dev_api_test_dev)
libapi_test_dev_la_LDFLAGS = -module
libapi_test_dev_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libapi_test_dev.la

dev_armr4_mpu_test = $(TestSimulateDevice)/armr4_mpu_test/armr4_mpu_test.c $(TestSimulateDevice)/armr4_mpu_test/armr4_mpu_test_module.c

libarmr4_mpu_test_la_SOURCES = $(dev_armr4_mpu_test)
libarmr4_mpu_test_la_LDFLAGS = -module
libarmr4_mpu_test_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libarmr4_mpu_test.la

dev_bugreport_test = $(TestSimulateDevice)/bugreport_test/bugreport_test.c $(TestSimulateDevice)/bugreport_test/bugreport_test_module.c

libbugreport_test_la_SOURCES = $(dev_bugreport_test)
libbugreport_test_la_LDFLAGS = -module
libbugreport_test_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libbugreport_test.la

dev_global_symbol = $(TestSimulateDevice)/global_symbol/global_symbol.c $(TestSimulateDevice)/global_symbol/global_symbol_module.c

libglobal_symbol_la_SOURCES = $(dev_global_symbol)
libglobal_symbol_la_LDFLAGS = -module
libglobal_symbol_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libglobal_symbol.la

dev_interface_test = $(TestSimulateDevice)/interface_test/interface_test.c $(TestSimulateDevice)/interface_test/interface_test_module.c

libinterface_test_la_SOURCES = $(dev_interface_test)
libinterface_test_la_LDFLAGS = -module
libinterface_test_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libinterface_test.la

dev_mem_monitor_demo = $(TestSimulateDevice)/mem_monitor_demo/mem_monitor_demo.c $(TestSimulateDevice)/mem_monitor_demo/mem_monitor_demo_module.c

libmem_monitor_demo_la_SOURCES = $(dev_mem_monitor_demo)
libmem_monitor_demo_la_LDFLAGS = -module
libmem_monitor_demo_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmem_monitor_demo.la

TimerSimulateDevice = timerSimulateDevice

dev_c687_timer = $(TimerSimulateDevice)/c687_timer/c687_timer.c $(TimerSimulateDevice)/c687_timer/c687_timer_module.c

libc687_timer_la_SOURCES = $(dev_c687_timer)
libc687_timer_la_LDFLAGS = -module
libc687_timer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libc687_timer.la

dev_htey_chk_timer = $(TimerSimulateDevice)/htey_chk_timer/htey_chk_timer.c $(TimerSimulateDevice)/htey_chk_timer/htey_chk_timer_module.c

libhtey_chk_timer_la_SOURCES = $(dev_htey_chk_timer)
libhtey_chk_timer_la_LDFLAGS = -module
libhtey_chk_timer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_chk_timer.la

dev_htey_dsp_timer = $(TimerSimulateDevice)/htey_dsp_timer/htey_dsp_timer.c $(TimerSimulateDevice)/htey_dsp_timer/htey_dsp_timer_module.c

libhtey_dsp_timer_la_SOURCES = $(dev_htey_dsp_timer)
libhtey_dsp_timer_la_LDFLAGS = -module
libhtey_dsp_timer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_dsp_timer.la

dev_htey_hard_timer = $(TimerSimulateDevice)/htey_hard_timer/htey_hard_timer.c $(TimerSimulateDevice)/htey_hard_timer/htey_hard_timer_module.c

libhtey_hard_timer_la_SOURCES = $(dev_htey_hard_timer)
libhtey_hard_timer_la_LDFLAGS = -module
libhtey_hard_timer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libhtey_hard_timer.la

dev_rm48x_timer = $(TimerSimulateDevice)/rm48x_timer/rm48x_timer.c $(TimerSimulateDevice)/rm48x_timer/rm48x_timer_module.c

librm48x_timer_la_SOURCES = $(dev_rm48x_timer)
librm48x_timer_la_LDFLAGS = -module
librm48x_timer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += librm48x_timer.la

dev_sopc_timer = $(TimerSimulateDevice)/sopc_timer/sopc_timer.c $(TimerSimulateDevice)/sopc_timer/sopc_timer_module.c

libsopc_timer_la_SOURCES = $(dev_sopc_timer)
libsopc_timer_la_LDFLAGS = -module
libsopc_timer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsopc_timer.la

dev_systimer_at91rm9200 = $(TimerSimulateDevice)/systimer_at91rm9200/systimer_at91rm9200.c $(TimerSimulateDevice)/systimer_at91rm9200/systimer_at91rm9200_module.c

libsystimer_at91rm9200_la_SOURCES = $(dev_systimer_at91rm9200)
libsystimer_at91rm9200_la_LDFLAGS = -module
libsystimer_at91rm9200_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsystimer_at91rm9200.la

dev_t104x_timer = $(TimerSimulateDevice)/t104x_timer/t104x_timer.c $(TimerSimulateDevice)/t104x_timer/t104x_timer_module.c

libt104x_timer_la_SOURCES = $(dev_t104x_timer)
libt104x_timer_la_LDFLAGS = -module
libt104x_timer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libt104x_timer.la

dev_timer_6713 = $(TimerSimulateDevice)/timer_6713/timer_6713.c $(TimerSimulateDevice)/timer_6713/timer_6713_module.c

libtimer_6713_la_SOURCES = $(dev_timer_6713)
libtimer_6713_la_LDFLAGS = -module
libtimer_6713_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_6713.la

dev_timer_am335x = $(TimerSimulateDevice)/timer_am335x/timer_am335x.c $(TimerSimulateDevice)/timer_am335x/timer_am335x_module.c

libtimer_am335x_la_SOURCES = $(dev_timer_am335x)
libtimer_am335x_la_LDFLAGS = -module
libtimer_am335x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_am335x.la

dev_timer_1ms_am335x = $(TimerSimulateDevice)/timer_am335x/timer_1ms_am335x.c $(TimerSimulateDevice)/timer_am335x/timer_1ms_am335x_module.c

libtimer_1ms_am335x_la_SOURCES = $(dev_timer_1ms_am335x)
libtimer_1ms_am335x_la_LDFLAGS = -module
libtimer_1ms_am335x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_1ms_am335x.la

dev_timer_am3359 = $(TimerSimulateDevice)/timer_am3359/timer_am3359.c $(TimerSimulateDevice)/timer_am3359/timer_1ms_am3359.c $(TimerSimulateDevice)/timer_am3359/timer_am3359_module.c

libtimer_am3359_la_SOURCES = $(dev_timer_am3359)
libtimer_am3359_la_LDFLAGS = -module
libtimer_am3359_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_am3359.la

dev_timer_c28 = $(TimerSimulateDevice)/timer_c28/timer_c28.c $(TimerSimulateDevice)/timer_c28/timer_c28_module.c

libtimer_c28_la_SOURCES = $(dev_timer_c28)
libtimer_c28_la_LDFLAGS = -module
libtimer_c28_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_c28.la

dev_timer_c64x = $(TimerSimulateDevice)/timer_c64x/timer_c64x.c $(TimerSimulateDevice)/timer_c64x/timer_c64x_module.c

libtimer_c64x_la_SOURCES = $(dev_timer_c64x)
libtimer_c64x_la_LDFLAGS = -module
libtimer_c64x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_c64x.la

dev_timer_c541x = $(TimerSimulateDevice)/timer_c541x/timer_c541x.c $(TimerSimulateDevice)/timer_c541x/timer_c541x_module.c

libtimer_c541x_la_SOURCES = $(dev_timer_c541x)
libtimer_c541x_la_LDFLAGS = -module
libtimer_c541x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_c541x.la

dev_timer_c645x = $(TimerSimulateDevice)/timer_c645x/timer_c645x.c $(TimerSimulateDevice)/timer_c645x/timer_c645x_module.c

libtimer_c645x_la_SOURCES = $(dev_timer_c645x)
libtimer_c645x_la_LDFLAGS = -module
libtimer_c645x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_c645x.la

dev_timer_c5510 = $(TimerSimulateDevice)/timer_c5510/timer_c5510.c $(TimerSimulateDevice)/timer_c5510/timer_c5510_module.c

libtimer_c5510_la_SOURCES = $(dev_timer_c5510)
libtimer_c5510_la_LDFLAGS = -module
libtimer_c5510_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_c5510.la

dev_timer_c6474 = $(TimerSimulateDevice)/timer_c6474/timer_c6474.c $(TimerSimulateDevice)/timer_c6474/timer_c6474_module.c

libtimer_c6474_la_SOURCES = $(dev_timer_c6474)
libtimer_c6474_la_LDFLAGS = -module
libtimer_c6474_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_c6474.la

dev_timer_c6678 = $(TimerSimulateDevice)/timer_c6678/timer_c6678.c $(TimerSimulateDevice)/timer_c6678/timer_c6678_module.c

libtimer_c6678_la_SOURCES = $(dev_timer_c6678)
libtimer_c6678_la_LDFLAGS = -module
libtimer_c6678_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_c6678.la

dev_timer_c6748 = $(TimerSimulateDevice)/timer_c6748/timer_c6748.c $(TimerSimulateDevice)/timer_c6748/timer_c6748_module.c

libtimer_c6748_la_SOURCES = $(dev_timer_c6748)
libtimer_c6748_la_LDFLAGS = -module
libtimer_c6748_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_c6748.la

dev_timer_leon2 = $(TimerSimulateDevice)/timer_leon2/timer_leon2.c $(TimerSimulateDevice)/timer_leon2/timer_leon2_module.c

libtimer_leon2_la_SOURCES = $(dev_timer_leon2)
libtimer_leon2_la_LDFLAGS = -module
libtimer_leon2_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_leon2.la

dev_timer_omapl138 = $(TimerSimulateDevice)/timer_omapl138/timer_omapl138.c $(TimerSimulateDevice)/timer_omapl138/timer_omapl138_module.c

libtimer_omapl138_la_SOURCES = $(dev_timer_omapl138)
libtimer_omapl138_la_LDFLAGS = -module
libtimer_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_omapl138.la

dev_timer_s3c2410x = $(TimerSimulateDevice)/timer_s3c2410x/timer_s3c2410x.c $(TimerSimulateDevice)/timer_s3c2410x/timer_s3c2410x_module.c

libtimer_s3c2410x_la_SOURCES = $(dev_timer_s3c2410x)
libtimer_s3c2410x_la_LDFLAGS = -module
libtimer_s3c2410x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_s3c2410x.la

dev_timer_s3c6410 = $(TimerSimulateDevice)/timer_s3c6410/timer_s3c6410.c $(TimerSimulateDevice)/timer_s3c6410/timer_s3c6410_module.c

libtimer_s3c6410_la_SOURCES = $(dev_timer_s3c6410)
libtimer_s3c6410_la_LDFLAGS = -module
libtimer_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer_s3c6410.la

dev_timer1_stm32f0x = $(TimerSimulateDevice)/timer1_stm32f0x/timer1_stm32f0x.c $(TimerSimulateDevice)/timer1_stm32f0x/timer1_stm32f0x_module.c

libtimer1_stm32f0x_la_SOURCES = $(dev_timer1_stm32f0x)
libtimer1_stm32f0x_la_LDFLAGS = -module
libtimer1_stm32f0x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtimer1_stm32f0x.la

dev_mpc8378_timer_intc = $(TimerSimulateDevice)/mpc8378_timer_intc/mpc8378_timer_intc.c $(TimerSimulateDevice)/mpc8378_timer_intc/mpc8378_timer_intc_module.c

libmpc8378_timer_intc_la_SOURCES = $(dev_mpc8378_timer_intc)
libmpc8378_timer_intc_la_LDFLAGS = -module
libmpc8378_timer_intc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8378_timer_intc.la

dev_zynq7000_cptimer = $(TimerSimulateDevice)/zynq7000_cptimer/zynq7000_cptimer.c $(TimerSimulateDevice)/zynq7000_cptimer/zynq7000_cptimer_module.c

libzynq7000_cptimer_la_SOURCES = $(dev_zynq7000_cptimer)
libzynq7000_cptimer_la_LDFLAGS = -module
libzynq7000_cptimer_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libzynq7000_cptimer.la

TouchSimulateDevice = touchSimulateDevice

dev_touchscreen = $(TouchSimulateDevice)/touchscreen/skyeye_touchscreen.c $(TouchSimulateDevice)/touchscreen/dev_touchscreen_skyeye.c $(TouchSimulateDevice)/touchscreen/ts_module.c

libtouchscreen_la_SOURCES = $(dev_touchscreen)
libtouchscreen_la_LDFLAGS = -module
libtouchscreen_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtouchscreen.la

dev_touchscreen_s3c6410 = $(TouchSimulateDevice)/touchscreen_s3c6410/touchscreen_s3c6410.c $(TouchSimulateDevice)/touchscreen_s3c6410/touchscreen_s3c6410_module.c

libtouchscreen_s3c6410_la_SOURCES = $(dev_touchscreen_s3c6410)
libtouchscreen_s3c6410_la_LDFLAGS = -module
libtouchscreen_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libtouchscreen_s3c6410.la

UartSimulateDevice = uartSimulateDevice

dev_c687_uart = $(UartSimulateDevice)/c687_uart/c687_uart.c $(UartSimulateDevice)/c687_uart/c687_uart_module.c

libc687_uart_la_SOURCES = $(dev_c687_uart)
libc687_uart_la_LDFLAGS = -module
libc687_uart_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libc687_uart.la

dev_dbguart_at91rm9200 = $(UartSimulateDevice)/dbguart_at91rm9200/dbguart_at91rm9200.c $(UartSimulateDevice)/dbguart_at91rm9200/at91rm9200_dbgfifo.c $(UartSimulateDevice)/dbguart_at91rm9200/dbguart_at91rm9200_module.c

libdbguart_at91rm9200_la_SOURCES = $(dev_dbguart_at91rm9200)
libdbguart_at91rm9200_la_LDFLAGS = -module
libdbguart_at91rm9200_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libdbguart_at91rm9200.la

dev_duart_p1010 = $(UartSimulateDevice)/duart_p1010/duart_p1010.c $(UartSimulateDevice)/duart_p1010/duart_p1010_module.c

libduart_p1010_la_SOURCES = $(dev_duart_p1010)
libduart_p1010_la_LDFLAGS = -module
libduart_p1010_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libduart_p1010.la

dev_ft2000a_uart = $(UartSimulateDevice)/ft2000a_uart/ft2000a_uart.c $(UartSimulateDevice)/ft2000a_uart/ft2000a_uart_module.c

libft2000a_uart_la_SOURCES = $(dev_ft2000a_uart)
libft2000a_uart_la_LDFLAGS = -module
libft2000a_uart_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_uart.la

dev_sopc_uart = $(UartSimulateDevice)/sopc_uart/sopc_uart.c $(UartSimulateDevice)/sopc_uart/sopc_fifo.c $(UartSimulateDevice)/sopc_uart/sopc_uart_module.c

libsopc_uart_la_SOURCES = $(dev_sopc_uart)
libsopc_uart_la_LDFLAGS = -module
libsopc_uart_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsopc_uart.la

dev_uart = $(UartSimulateDevice)/uart/skyeye_uart.c $(UartSimulateDevice)/uart/skyeye_uart_net.c $(UartSimulateDevice)/uart/skyeye_uart_pipe.c $(UartSimulateDevice)/uart/skyeye_uart_stdio.c $(UartSimulateDevice)/uart/skyeye_uart_term.c $(UartSimulateDevice)/uart/uart_module.c

libuart_la_SOURCES = $(dev_uart)
libuart_la_LDFLAGS = -module
libuart_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart.la

dev_uart_16550 = $(UartSimulateDevice)/uart_16550/uart_16550.c $(UartSimulateDevice)/uart_16550/uart_16550_module.c

libuart_16550_la_SOURCES = $(dev_uart_16550)
libuart_16550_la_LDFLAGS = -module
libuart_16550_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_16550.la

dev_uart_am335x = $(UartSimulateDevice)/uart_am335x/uart_am335x.c $(UartSimulateDevice)/uart_am335x/uart_am335x_module.c

libuart_am335x_la_SOURCES = $(dev_uart_am335x)
libuart_am335x_la_LDFLAGS = -module
libuart_am335x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_am335x.la

dev_uart_am3359 = $(UartSimulateDevice)/uart_am3359/uart_am3359.c $(UartSimulateDevice)/uart_am3359/am3359_fifo.c $(UartSimulateDevice)/uart_am3359/uart_am3359_module.c

libuart_am3359_la_SOURCES = $(dev_uart_am3359)
libuart_am3359_la_LDFLAGS = -module
libuart_am3359_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_am3359.la

dev_uart_at91rm9200 = $(UartSimulateDevice)/uart_at91rm9200/uart_at91rm9200.c $(UartSimulateDevice)/uart_at91rm9200/at91rm9200_fifo.c $(UartSimulateDevice)/uart_at91rm9200/uart_at91rm9200_module.c

libuart_at91rm9200_la_SOURCES = $(dev_uart_at91rm9200)
libuart_at91rm9200_la_LDFLAGS = -module
libuart_at91rm9200_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_at91rm9200.la

dev_uart_c2812 = $(UartSimulateDevice)/uart_c2812/uart_c2812.c $(UartSimulateDevice)/uart_c2812/uart_c2812_module.c

libuart_c2812_la_SOURCES = $(dev_uart_c2812)
libuart_c2812_la_LDFLAGS = -module
libuart_c2812_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_c2812.la

dev_uart_c5510 = $(UartSimulateDevice)/uart_c5510/uart_c5510.c $(UartSimulateDevice)/uart_c5510/c5510_fifo.c $(UartSimulateDevice)/uart_c5510/uart_c5510_module.c

libuart_c5510_la_SOURCES = $(dev_uart_c5510)
libuart_c5510_la_LDFLAGS = -module
libuart_c5510_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_c5510.la

dev_uart_com = $(UartSimulateDevice)/uart_com/uart_com.c $(UartSimulateDevice)/uart_com/uart_com_serial_win.c $(UartSimulateDevice)/uart_com/uart_com_serial_linux.c $(UartSimulateDevice)/uart_com/uart_com_module.c

libuart_com_la_SOURCES = $(dev_uart_com)
libuart_com_la_LDFLAGS = -module
libuart_com_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_com.la

dev_uart_file = $(UartSimulateDevice)/uart_file/uart_file.c $(UartSimulateDevice)/uart_file/uart_file_module.c

libuart_file_la_SOURCES = $(dev_uart_file)
libuart_file_la_LDFLAGS = -module
libuart_file_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_file.la

dev_uart_imx6 = $(UartSimulateDevice)/uart_imx6/uart_imx6.c $(UartSimulateDevice)/uart_imx6/uart_imx6_fifo.c $(UartSimulateDevice)/uart_imx6/uart_imx6_module.c

libuart_imx6_la_SOURCES = $(dev_uart_imx6)
libuart_imx6_la_LDFLAGS = -module
libuart_imx6_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_imx6.la

dev_uart_leon2 = $(UartSimulateDevice)/uart_leon2/uart_leon2.c $(UartSimulateDevice)/uart_leon2/uart_leon2_module.c

libuart_leon2_la_SOURCES = $(dev_uart_leon2)
libuart_leon2_la_LDFLAGS = -module
libuart_leon2_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_leon2.la

dev_uart_ls3a = $(UartSimulateDevice)/uart_ls3a/uart_ls3a.c $(UartSimulateDevice)/uart_ls3a/uart_ls3a_module.c

libuart_ls3a_la_SOURCES = $(dev_uart_ls3a)
libuart_ls3a_la_LDFLAGS = -module
libuart_ls3a_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_ls3a.la

dev_uart_mpc5121 = $(UartSimulateDevice)/uart_mpc5121/uart_mpc5121.c $(UartSimulateDevice)/uart_mpc5121/uart_mpc5121_module.c

libuart_mpc5121_la_SOURCES = $(dev_uart_mpc5121)
libuart_mpc5121_la_LDFLAGS = -module
libuart_mpc5121_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_mpc5121.la

dev_uart_mpc8245 = $(UartSimulateDevice)/uart_mpc8245/uart_mpc8245.c $(UartSimulateDevice)/uart_mpc8245/uart_mpc8245_module.c

libuart_mpc8245_la_SOURCES = $(dev_uart_mpc8245)
libuart_mpc8245_la_LDFLAGS = -module
libuart_mpc8245_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_mpc8245.la

dev_uart_omapl138 = $(UartSimulateDevice)/uart_omapl138/uart_omapl138.c $(UartSimulateDevice)/uart_omapl138/omapl138_fifo.c $(UartSimulateDevice)/uart_omapl138/uart_omapl138_module.c

libuart_omapl138_la_SOURCES = $(dev_uart_omapl138)
libuart_omapl138_la_LDFLAGS = -module
libuart_omapl138_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_omapl138.la

dev_uart_pc = $(UartSimulateDevice)/uart_pc/uart_pc.c $(UartSimulateDevice)/uart_pc/serial.c $(UartSimulateDevice)/uart_pc/uart_pc_module.c

libuart_pc_la_SOURCES = $(dev_uart_pc)
libuart_pc_la_LDFLAGS = -module
libuart_pc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_pc.la

dev_uart_s3c2410x = $(UartSimulateDevice)/uart_s3c2410x/uart_s3c2410x.c $(UartSimulateDevice)/uart_s3c2410x/s3c2410x_fifo.c $(UartSimulateDevice)/uart_s3c2410x/uart_s3c2410x_module.c

libuart_s3c2410x_la_SOURCES = $(dev_uart_s3c2410x)
libuart_s3c2410x_la_LDFLAGS = -module
libuart_s3c2410x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_s3c2410x.la

dev_uart_s3c6410 = $(UartSimulateDevice)/uart_s3c6410/uart_s3c6410.c $(UartSimulateDevice)/uart_s3c6410/fifo.c $(UartSimulateDevice)/uart_s3c6410/uart_s3c6410_module.c

libuart_s3c6410_la_SOURCES = $(dev_uart_s3c6410)
libuart_s3c6410_la_LDFLAGS = -module
libuart_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_s3c6410.la

dev_uart_term = $(UartSimulateDevice)/uart_term/uart_term.c $(UartSimulateDevice)/uart_term/telnet.c $(UartSimulateDevice)/uart_term/uart_term_module.c

libuart_term_la_SOURCES = $(dev_uart_term)
libuart_term_la_LDFLAGS = -module
libuart_term_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_term.la

dev_uart_tsc695f = $(UartSimulateDevice)/uart_tsc695f/uart_tsc695f.c $(UartSimulateDevice)/uart_tsc695f/uart_tsc695f_module.c

libuart_tsc695f_la_SOURCES = $(dev_uart_tsc695f)
libuart_tsc695f_la_LDFLAGS = -module
libuart_tsc695f_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_tsc695f.la

dev_uart_xdht = $(UartSimulateDevice)/uart_xdht/uart_xdht.c $(UartSimulateDevice)/uart_xdht/uart_xdht_module.c

libuart_xdht_la_SOURCES = $(dev_uart_xdht)
libuart_xdht_la_LDFLAGS = -module
libuart_xdht_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libuart_xdht.la

dev_usart_stm32f0x = $(UartSimulateDevice)/usart_stm32f0x/usart_stm32f0x.c $(UartSimulateDevice)/usart_stm32f0x/usart_stm32f0x_module.c

libusart_stm32f0x_la_SOURCES = $(dev_usart_stm32f0x)
libusart_stm32f0x_la_LDFLAGS = -module
libusart_stm32f0x_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libusart_stm32f0x.la

dev_c6678_uart = $(UartSimulateDevice)/c6678_uart/c6678_uart.c $(UartSimulateDevice)/c6678_uart/c6678_uart_module.c

libc6678_uart_la_SOURCES = $(dev_c6678_uart)
libc6678_uart_la_LDFLAGS = -module
libc6678_uart_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libc6678_uart.la

dev_zynq7000_uart = $(UartSimulateDevice)/zynq7000_uart/zynq7000_uart.c $(UartSimulateDevice)/zynq7000_uart/zynq7000_uart_module.c

libzynq7000_uart_la_SOURCES = $(dev_zynq7000_uart)
libzynq7000_uart_la_LDFLAGS = -module
libzynq7000_uart_la_LDFLAGS +=$(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libzynq7000_uart.la

UsbSimulateDevice = usbSimulateDevice

dev_usbhost_s3c6410 = $(UsbSimulateDevice)/usbhost_s3c6410/usbhost_s3c6410.c $(UsbSimulateDevice)/usbhost_s3c6410/usbhost_s3c6410_module.c

libusbhost_s3c6410_la_SOURCES = $(dev_usbhost_s3c6410)
libusbhost_s3c6410_la_LDFLAGS = -module
libusbhost_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libusbhost_s3c6410.la

VideoSimulateDevice = videoSimulateDevice

dev_camif_s3c6410 = $(VideoSimulateDevice)/camif_s3c6410/camif_s3c6410.c $(VideoSimulateDevice)/camif_s3c6410/camif_s3c6410_module.c

libcamif_s3c6410_la_SOURCES = $(dev_camif_s3c6410)
libcamif_s3c6410_la_LDFLAGS = -module
libcamif_s3c6410_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libcamif_s3c6410.la

WdtSimulateDevice = wdtSimulateDevice

dev_ft2000a_wdt = $(WdtSimulateDevice)/ft2000a_wdt/ft2000a_wdt.c $(WdtSimulateDevice)/ft2000a_wdt/ft2000a_wdt_module.c

libft2000a_wdt_la_SOURCES = $(dev_ft2000a_wdt)
libft2000a_wdt_la_LDFLAGS = -module
libft2000a_wdt_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libft2000a_wdt.la

dev_sopc_wdt = $(WdtSimulateDevice)/sopc_wdt/sopc_wdt.c $(WdtSimulateDevice)/sopc_wdt/sopc_wdt_module.c

libsopc_wdt_la_SOURCES = $(dev_sopc_wdt)
libsopc_wdt_la_LDFLAGS = -module
libsopc_wdt_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libsopc_wdt.la

Mpc8247SimulateDevice = boardsSimulateDevice/mpc8247SimulateDevice

dev_mpc8247_brg = $(Mpc8247SimulateDevice)/mpc8247_brg/mpc8247_brg.c $(Mpc8247SimulateDevice)/mpc8247_brg/mpc8247_brg_module.c

libmpc8247_brg_la_SOURCES = $(dev_mpc8247_brg)
libmpc8247_brg_la_LDFLAGS = -module
libmpc8247_brg_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_brg.la

dev_mpc8247_clockandreset = $(Mpc8247SimulateDevice)/mpc8247_clockandreset/mpc8247_clockandreset.c $(Mpc8247SimulateDevice)/mpc8247_clockandreset/mpc8247_clockandreset_module.c

libmpc8247_clockandreset_la_SOURCES = $(dev_mpc8247_clockandreset)
libmpc8247_clockandreset_la_LDFLAGS = -module
libmpc8247_clockandreset_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_clockandreset.la

dev_mpc8247_dpram = $(Mpc8247SimulateDevice)/mpc8247_dpram/mpc8247_dpram.c $(Mpc8247SimulateDevice)/mpc8247_dpram/mpc8247_dpram_module.c

libmpc8247_dpram_la_SOURCES = $(dev_mpc8247_dpram)
libmpc8247_dpram_la_LDFLAGS = -module
libmpc8247_dpram_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_dpram.la

dev_mpc8247_fcc1 = $(Mpc8247SimulateDevice)/mpc8247_fcc1/mpc8247_fcc1.c $(Mpc8247SimulateDevice)/mpc8247_fcc1/mpc8247_fcc1_module.c

libmpc8247_fcc1_la_SOURCES = $(dev_mpc8247_fcc1)
libmpc8247_fcc1_la_LDFLAGS = -module
libmpc8247_fcc1_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_fcc1.la

dev_mpc8247_iic = $(Mpc8247SimulateDevice)/mpc8247_iic/mpc8247_iic.c $(Mpc8247SimulateDevice)/mpc8247_iic/mpc8247_iic_module.c

libmpc8247_iic_la_SOURCES = $(dev_mpc8247_iic)
libmpc8247_iic_la_LDFLAGS = -module
libmpc8247_iic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_iic.la

dev_mpc8247_io = $(Mpc8247SimulateDevice)/mpc8247_io/mpc8247_io.c $(Mpc8247SimulateDevice)/mpc8247_io/mpc8247_io_module.c

libmpc8247_io_la_SOURCES = $(dev_mpc8247_io)
libmpc8247_io_la_LDFLAGS = -module
libmpc8247_io_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_io.la

dev_mpc8247_memorycontroller = $(Mpc8247SimulateDevice)/mpc8247_memorycontroller/mpc8247_memorycontroller.c $(Mpc8247SimulateDevice)/mpc8247_memorycontroller/mpc8247_memorycontroller_module.c

libmpc8247_memorycontroller_la_SOURCES = $(dev_mpc8247_memorycontroller)
libmpc8247_memorycontroller_la_LDFLAGS = -module
libmpc8247_memorycontroller_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_memorycontroller.la

dev_mpc8247_pci = $(Mpc8247SimulateDevice)/mpc8247_pci/mpc8247_pci.c $(Mpc8247SimulateDevice)/mpc8247_pci/mpc8247_pci_module.c

libmpc8247_pci_la_SOURCES = $(dev_mpc8247_pci)
libmpc8247_pci_la_LDFLAGS = -module
libmpc8247_pci_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_pci.la

dev_mpc8247_scc = $(Mpc8247SimulateDevice)/mpc8247_scc/mpc8247_scc.c $(Mpc8247SimulateDevice)/mpc8247_scc/mpc8247_scc_module.c

libmpc8247_scc_la_SOURCES = $(dev_mpc8247_scc)
libmpc8247_scc_la_LDFLAGS = -module
libmpc8247_scc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_scc.la

dev_mpc8247_siu_ic = $(Mpc8247SimulateDevice)/mpc8247_siu_ic/mpc8247_siu_ic.c $(Mpc8247SimulateDevice)/mpc8247_siu_ic/mpc8247_siu_ic_module.c

libmpc8247_siu_ic_la_SOURCES = $(dev_mpc8247_siu_ic)
libmpc8247_siu_ic_la_LDFLAGS = -module
libmpc8247_siu_ic_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_siu_ic.la

dev_mpc8247_smc = $(Mpc8247SimulateDevice)/mpc8247_smc/mpc8247_smc.c $(Mpc8247SimulateDevice)/mpc8247_smc/mpc8247_smc_module.c

libmpc8247_smc_la_SOURCES = $(dev_mpc8247_smc)
libmpc8247_smc_la_LDFLAGS = -module
libmpc8247_smc_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8247_smc.la

dev_mpc8272_cpm_mux = $(Mpc8247SimulateDevice)/mpc8272_cpm_mux/mpc8272_cpm_mux.c $(Mpc8247SimulateDevice)/mpc8272_cpm_mux/mpc8272_cpm_mux_module.c

libmpc8272_cpm_mux_la_SOURCES = $(dev_mpc8272_cpm_mux)
libmpc8272_cpm_mux_la_LDFLAGS = -module
libmpc8272_cpm_mux_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8272_cpm_mux.la

dev_mpc8272_processor_comm = $(Mpc8247SimulateDevice)/mpc8272_processor_comm/mpc8272_processor_comm.c $(Mpc8247SimulateDevice)/mpc8272_processor_comm/mpc8272_processor_comm_module.c

libmpc8272_processor_comm_la_SOURCES = $(dev_mpc8272_processor_comm)
libmpc8272_processor_comm_la_LDFLAGS = -module
libmpc8272_processor_comm_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8272_processor_comm.la

dev_mpc8272_siu = $(Mpc8247SimulateDevice)/mpc8272_siu/mpc8272_siu.c $(Mpc8247SimulateDevice)/mpc8272_siu/mpc8272_siu_module.c

libmpc8272_siu_la_SOURCES = $(dev_mpc8272_siu)
libmpc8272_siu_la_LDFLAGS = -module
libmpc8272_siu_la_LDFLAGS += $(WIN32_LD_DEVICE)
pkglib_LTLIBRARIES += libmpc8272_siu.la