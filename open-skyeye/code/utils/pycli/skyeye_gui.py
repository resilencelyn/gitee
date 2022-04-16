# -*- coding: UTF-8 -*-

import wx
import os
import time
import _thread
import threading
import subprocess
####GUI######
import skyeye_gui_lic as lic_gui
import skyeye_gui_print_log as print_log_gui
import skyeye_gui_cpu_register as cpu_register_gui
import skyeye_gui_memory as memory_gui
import skyeye_gui_remote_gdb as remote_gdb_gui
import skyeye_gui_device_regs as device_regs_gui
import skyeye_gui_disassemble as dissemble_gui
import skyeye_gui_fj as fj_gui
import skyeye_gui_pc_recorder as pc_recorder_gui

import skyeye_gui_log as log_gui
import skyeye_gui_option as option_gui
import skyeye_gui_general_button as general_button_gui
import skyeye_gui_write_memory as write_memory_gui

import skyeye_gui_module as module_gui
import skyeye_gui_mem_map as mem_map_gui
import skyeye_gui_command as command_gui

import skyeye_gui_about as about_gui
###############
import se
import cli
import mips
import fsel
import se_path
import pytimer
import coverage
import errormessage
import conf as se_conf
import custom_dialog as CDialog
import win_workspace as WS
import skyeye_license as se_lic
import network_control as NetCtrl

from sky_log import *
from skyeye_mach import *
from tools import Profile
from common_dialog import *
from tools import SkyEyeConf
from skyeye_common_module import *


reverse_arch = ["sparc_v8", "c67x_core", "c64x_core", "c64xplus_core", "c66x_core", "c674x_core", "arm920t_core", "cortex_a8_core", "c5510_core", "arm926ejs_core", "c54x_core"]
checkpoint_arch = ["sparc_v8", "x86", "c67x_core", "c64x_core", "c64xplus_core", "c66x_core", "c674x_core"]

AppRestart = True

class BreakpointCheck(threading.Thread):
	def __init__(self, win):
		threading.Thread.__init__(self)
		self.thread_stop = False
		self.win = win
		self.isStoped = False

	def run(self):
		while not self.thread_stop:
			self.win.exec_thread_pool()
			if self.win.running == True or self.win.stepi_mode == True:
				machlist = self.win.config.get_mach_list()
				for mach in machlist:
					corename = self.win.config.get_cpuname_by_mach(mach)
					try:
						ret = SkyEyeCheckBreakpointHit(corename)
					except:
						self.win.ErrorReport()
					if ret == 1:
						self.win.on_skyeye_stop(0)
						try:
							hit_id = SkyEyeGetBreakpointHitId(corename)
						except:
							self.win.ErrorReport()
						try:
							addr = SkyEyeGetBreakpointAddrById(corename, hit_id)
						except:
							self.win.ErrorReport()
						try:
							ret = SkyEyeDeleteBreakpointByAddr(corename, addr)
						except:
							self.win.ErrorReport()
						self.win.dissemble_frame.delete_breakpoint(addr)
						if self.win.dissemble_frame != None:
							self.win.dissemble_frame.Refurbish()
						if self.win.stepi_mode == True:
							self.win.stepi_mode = False
					else:
						pass
			time.sleep(0.1)
		self.isStoped = True

	def stop(self):
		self.thread_stop = True
		while self.isStoped == False:
			time.sleep(0.01)

class MainFrame(wx.Frame):
	def __init__(self, parent = None, id = -1,title = "天目仿真"):
		wx.Frame.__init__(self, parent, id, title)
		self.workspace_dir_value = None
		self.cdialog_show = []

		self.set_configure = []
		self.se_coverage = []
		self.help_list = []
		self.open_conf_flage = 0
		self.running = False
		self.remote_gdb_status = False
		self.main_frame_enable = True
		self.configure_file_path = "null"
		self.start_path = ""
		self.skyeye_conf = None
		self.cpu_registers_frame = None
		self.memory_frame = None
		self.device_register_frame = None
		self.fault_inject_frame = None
		self.button_frame = None
		self.dissemble_frame = None
		self.log_frame = None
		self.module_frame = None
		self.mem_map_frame = None
		self.command_frame = None
		self.about_frame = None
		self.pc_recorder_frame = None
		self.write_memory_frame = None
		self.break_check = None
		self.profile = Profile(None)
		self.stepi_mode = False
		self.mach_count = 0
		self.tool_bar_run_status = None
		self.tool_bar_stop_status = None

		#set icon
		self.Bind(wx.EVT_CLOSE, self.quit_skyeye)
		ico_name = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(ico_name, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)

		self.menu_bar = wx.MenuBar()
		self.menu_bar.SetBackgroundColour("#f0f0f0")
		# Create Menu for File
		self.file_menu = wx.Menu()
		self.open_conf_menu = self.file_menu.Append(-1,"打开配置文件")
		self.open_new_session_menu = self.file_menu.Append(-1,"打开新会话")
		#############################
		#self.create_gp_config()
		#self.create_se_coverage()
		self.file_menu.AppendSeparator()
		self.open_checkpoint_menu = self.file_menu.Append(-1,"打开快照文件")
		self.save_checkpoint_menu = self.file_menu.Append(-1,"保存快照文件")
		self.file_menu.AppendSeparator()
		self.change_workspace_menu = self.file_menu.Append(-1,"切换工作空间")
		self.create_workspace_menu = self.file_menu.Append(-1,"新建工作空间")
		self.file_menu.AppendSeparator()
		quit_menu = self.file_menu.Append(-1,"退出")
		self.file_menu.AppendSeparator()

		self.Bind(wx.EVT_MENU, self.on_open_config, self.open_conf_menu)
		self.Bind(wx.EVT_MENU, self.on_open_new_sessio, self.open_new_session_menu)
		self.Bind(wx.EVT_MENU, self.on_open_checkpoint, self.open_checkpoint_menu)
		self.Bind(wx.EVT_MENU, self.on_save_checkpoint, self.save_checkpoint_menu)
		self.Bind(wx.EVT_MENU, self.on_change_workspac, self.change_workspace_menu)
		self.Bind(wx.EVT_MENU, self.on_create_workspace, self.create_workspace_menu)
		self.Bind(wx.EVT_MENU, self.quit_skyeye, quit_menu)

		# Create Menu for Debug
		self.debug_menu = wx.Menu()
		self.registers_menu = self.debug_menu.Append(-1,"处理器寄存器")
		self.memories_menu = self.debug_menu.Append(-1,"内存")
		self.remote_menu = self.debug_menu.Append(-1,"远程调试")
		self.device_reg_menu = self.debug_menu.Append(-1,"设备寄存器")
		self.disassembler_menu = self.debug_menu.Append(-1,"反汇编")
		self.faule_inject_menu = self.debug_menu.Append(-1,"故障注入")
		self.pc_recorder_menu = self.debug_menu.Append(-1,"指令流记录工具")

		self.Bind(wx.EVT_MENU, self.on_registers, self.registers_menu)
		self.Bind(wx.EVT_MENU, self.on_memories, self.memories_menu)
		self.Bind(wx.EVT_MENU, self.on_remote, self.remote_menu)
		self.Bind(wx.EVT_MENU, self.on_device_reg, self.device_reg_menu)
		self.Bind(wx.EVT_MENU, self.on_disassembler, self.disassembler_menu)
		self.Bind(wx.EVT_MENU, self.on_faule_inject, self.faule_inject_menu)
		self.Bind(wx.EVT_MENU, self.on_pc_recorder, self.pc_recorder_menu)

		#Create menu for tools
		self.tools_menu = wx.Menu()
		self.log_menu = self.tools_menu.Append(-1,"日志")
		self.option_menu = self.tools_menu.Append(-1,"选项")
		self.tools_menu.AppendSeparator()
		self.button_menu = self.tools_menu.Append(-1,"通用按钮")
		self.output_menu = self.tools_menu.Append(-1,"输出")
		self.write_mem_menu = self.tools_menu.Append(-1,"写入内存")
		self.tools_menu.AppendSeparator()
		self.create_custom_menu(self.tools_menu)

		self.Bind(wx.EVT_MENU, self.on_log, self.log_menu)
		self.Bind(wx.EVT_MENU, self.on_option, self.option_menu)
		self.Bind(wx.EVT_MENU, self.on_button, self.button_menu)
		self.Bind(wx.EVT_MENU, self.on_output, self.output_menu)
		self.Bind(wx.EVT_MENU, self.on_write_memory, self.write_mem_menu)

		#Create menu for browse
		self.browse_menu = wx.Menu()

		self.module_browse_menu = self.browse_menu.Append(-1,"模块")
		self.mem_map_browse_menu = self.browse_menu.Append(-1,"内存映射")
		self.command_browse_menu = self.browse_menu.Append(-1,"命令")

		self.Bind(wx.EVT_MENU, self.on_module_browse, self.module_browse_menu)
		self.Bind(wx.EVT_MENU, self.on_mem_map_browse, self.mem_map_browse_menu)
		self.Bind(wx.EVT_MENU, self.on_command_browse, self.command_browse_menu)

		# Create Menu for Help
		self.help_menu = wx.Menu()
		self.doc_menu = self.help_menu.Append(-1,"文档")
		self.lic_menu = self.help_menu.Append(-1,"注册")

		self.Bind(wx.EVT_MENU, self.on_doc, self.doc_menu)
		self.Bind(wx.EVT_MENU, self.on_lic, self.lic_menu)

		# Create Menu for about
		self.about_menu = wx.Menu()
		self.about_menu_item = self.about_menu.Append(-1,"关于")

		self.Bind(wx.EVT_MENU, self.on_about, self.about_menu_item)

		# Add the Memuitem into menu_bar
		self.menu_bar.Append(self.file_menu, "文件")
		self.menu_bar.Append(self.debug_menu, "调试")
		self.menu_bar.Append(self.tools_menu, "工具")
		self.menu_bar.Append(self.browse_menu, "浏览")
		self.menu_bar.Append(self.help_menu, "帮助")
		self.menu_bar.Append(self.about_menu, "关于")
		self.SetMenuBar(self.menu_bar)

		# Add the Tool Bar
		image_open =  wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN, wx.ART_TOOLBAR)
		image_new = wx.ArtProvider.GetBitmap(wx.ART_NEW, wx.ART_TOOLBAR)
		#image_config = wx.ArtProvider.GetBitmap(wx.ART_REMOVABLE, wx.ART_TOOLBAR)
		image_stop = GetImage(self, se_path.GetPicPath("stop.png"), 24, 24)
		image_run = GetImage(self, se_path.GetPicPath("run.png"), 24, 24)
		image_restart = GetImage(self, se_path.GetPicPath("back.png"), 24, 24)
		enable_exec = wx.Image(se_path.GetPicPath("enable_exec.png"), wx.BITMAP_TYPE_PNG).ConvertToBitmap()
		dyncom_exec = wx.Image(se_path.GetPicPath("dyncom.png"), wx.BITMAP_TYPE_PNG).ConvertToBitmap()

		self.ToolBar = self.CreateToolBar(wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT| wx.TB_TEXT)
		self.ToolBar.SetBackgroundColour("#f0f0f0")
		self.ToolBar.SetToolBitmapSize((32, 32))

		self.open_conf_bar = self.ToolBar.AddTool(-1, '', image_open, wx.NullBitmap, wx.ITEM_NORMAL, "打开")
		self.open_new_session_bar = self.ToolBar.AddTool(-1, '', image_new, wx.NullBitmap, wx.ITEM_NORMAL, "打开新会话")
		#self.conf_dev_system_bar = self.ToolBar.AddTool(-1, '', image_config, wx.NullBitmap, wx.ITEM_NORMAL, "配置硬件系统")
		self.ToolBar.AddSeparator()
		self.skyeye_stop_bar = self.ToolBar.AddCheckTool(-1, '', image_stop, wx.NullBitmap, "停止")
		self.skyeye_run_bar = self.ToolBar.AddCheckTool(-1, '', image_run, wx.NullBitmap, "运行")
		self.skyeye_restart_bar = self.ToolBar.AddCheckTool(-1, '', image_restart, wx.NullBitmap, "重启")
		self.ToolBar.AddSeparator()
		self.skyeye_reverse_bar = self.ToolBar.AddTool(-1, '', enable_exec, wx.NullBitmap, wx.ITEM_CHECK, "开启/关闭逆向执行")
		self.skyeye_up_bar = self.ToolBar.AddTool(-1, '', dyncom_exec, wx.NullBitmap, wx.ITEM_CHECK, "加速执行")
		self.ToolBar.Realize()

		self.Bind(wx.EVT_TOOL, self.on_open_config, self.open_conf_bar)
		self.Bind(wx.EVT_TOOL, self.on_open_new_sessio, self.open_new_session_bar)
		#self.Bind(wx.EVT_TOOL, self.on_conf_dev_system, self.conf_dev_system_bar)
		self.Bind(wx.EVT_TOOL, self.on_skyeye_stop, self.skyeye_stop_bar)
		self.Bind(wx.EVT_TOOL, self.on_skyeye_run, self.skyeye_run_bar)
		self.Bind(wx.EVT_TOOL, self.on_skyeye_restart, self.skyeye_restart_bar)
		self.Bind(wx.EVT_TOOL, self.on_skyeye_reverse, self.skyeye_reverse_bar)
		self.Bind(wx.EVT_TOOL, self.on_skyeye_up, self.skyeye_up_bar)

		self.statusbar = self.CreateStatusBar()
		self.statusbar.SetFieldsCount(3)
		self.statusbar.SetStatusWidths([-2, -2, -1])

		#Add histroy file open menu
		self.add_history_menu()


		#Save the working directory where start skyeye
		self.save_start_path()


		#Init the  home page button when start skyeye
		self.init_op_button()

		#Init workspace
		if se_path.os_info == "Linux":
			self.workspace_dir_value = se_path.default_workspace
		else:
			self.workspace_dir_value = self.get_last_workspace()
			ret = WS.check_workspace(self.workspace_dir_value)
			if  ret == False:
				self.first_create_workspace()

		#Load module from workspace
		WS.load_module_from_ws()

		#check if skyeye need to  automatically load conf
		wx.CallAfter(self.reload_config)

		self.scrolled = None
		self.Box = wx.BoxSizer(wx.HORIZONTAL)

		self.SetSizer(self.Box)

		self.update_mach_info(None)

		self.thread_pool = []
		self.thread_pool_mutex  = threading.Lock()

		self.start_handler()

		self.Center()

		pytimer.PyTmrInit()
		if se_path.os_info != "Linux":
			try:
				NetCtrl.server_init()
			except:
				print ("网络服务启动失败,检查GRPC服务是否配置正确!")


	def start_handler(self):
		self.break_check = BreakpointCheck(self)
		self.break_check.start()
		mips.mips_thread = mips.mips()
		mips.mips_thread.start()

	def end_handler(self):
		if self.break_check != None:
			self.break_check.stop()
		if mips.mips_thread != None:
			mips.mips_thread.stop()

	def exec_thread_pool(self):
		self.thread_pool_mutex.acquire()
		for fun_l in self.thread_pool:
			if fun_l[0] != None:
				fun_l[1]()
		self.thread_pool_mutex.release()

	#This function is used to disable or enable any button when start skyeye
	def init_op_button(self):
		self.save_checkpoint_menu.Enable(False)
		self.change_workspace_menu.Enable(True)
		self.create_workspace_menu.Enable(True)
		self.registers_menu.Enable(False)
		self.memories_menu.Enable(False)
		self.remote_menu.Enable(False)
		self.device_reg_menu.Enable(False)
		self.disassembler_menu.Enable(False)
		self.faule_inject_menu.Enable(False)
		self.pc_recorder_menu.Enable(False)
		self.mem_map_browse_menu.Enable(False)
		#self.conf_dev_system_bar.Enable(False)
		#self.skyeye_stop_bar.Enable(False)
		#self.skyeye_run_bar.Enable(False)
		#self.skyeye_restart_bar.Enable(False)

	#This function is used to enable or disable any button when open conf function was called
	def open_conf_op_button(self):
		self.save_checkpoint_menu.Enable(True)
		self.change_workspace_menu.Enable(True)
		self.create_workspace_menu.Enable(True)
		self.registers_menu.Enable(True)
		self.memories_menu.Enable(True)
		self.remote_menu.Enable(True)
		self.device_reg_menu.Enable(True)
		self.disassembler_menu.Enable(True)
		self.faule_inject_menu.Enable(True)
		self.pc_recorder_menu.Enable(True)
		self.mem_map_browse_menu.Enable(True)
		#self.conf_dev_system_bar.Enable(True)
		self.ToolBar.ToggleTool(self.skyeye_stop_bar.GetId(),True)
		#self.skyeye_run_bar.Enable(False)
		#self.skyeye_restart_bar.Enable(False)
		self.tool_bar_stop_status = self.ToolBar.GetToolState(self.skyeye_stop_bar.GetId())
		self.tool_bar_run_status = self.ToolBar.GetToolState(self.skyeye_run_bar.GetId())

	#This function is used to enable or disable any button when run skyeye  function was called
	def run_op_button(self):
		#self.save_checkpoint_menu.Enable(True)
		self.change_workspace_menu.Enable(False)
		self.create_workspace_menu.Enable(False)
		#self.registers_menu.Enable(True)
		#self.memories_menu.Enable(True)
		self.remote_menu.Enable(False)
		#self.device_reg_menu.Enable(True)
		#self.disassembler_menu.Enable(True)
		#self.faule_inject_menu.Enable(True)
		#self.pc_recorder_menu.Enable(True)
		#self.mem_map_browse_menu.Enable(False)
		#self.conf_dev_system_bar.Enable(False)
		self.ToolBar.ToggleTool(self.skyeye_stop_bar.GetId(),(not self.ToolBar.GetToolState(self.skyeye_stop_bar.GetId())))
		#self.skyeye_run_bar.Enable(False)
		#self.skyeye_restart_bar.Enable(False)
		self.tool_bar_stop_status = self.ToolBar.GetToolState(self.skyeye_stop_bar.GetId())
		self.tool_bar_run_status = self.ToolBar.GetToolState(self.skyeye_run_bar.GetId())

	#This function is used to enable or disable any button when stop running  function was called
	def stop_op_button(self):
		#self.save_checkpoint_menu.Enable(True)
		self.change_workspace_menu.Enable(True)
		self.create_workspace_menu.Enable(True)
		#self.registers_menu.Enable(True)
		#self.memories_menu.Enable(True)
		self.remote_menu.Enable(True)
		#self.device_reg_menu.Enable(True)
		#self.disassembler_menu.Enable(True)
		#self.faule_inject_menu.Enable(True)
		#self.pc_recorder_menu.Enable(True)
		#self.mem_map_browse_menu.Enable(False)
		#self.conf_dev_system_bar.Enable(False)
		self.ToolBar.ToggleTool(self.skyeye_run_bar.GetId(),(not self.ToolBar.GetToolState(self.skyeye_run_bar.GetId())))
		#self.skyeye_run_bar.Enable(False)
		#self.skyeye_restart_bar.Enable(False)
		self.tool_bar_stop_status = self.ToolBar.GetToolState(self.skyeye_stop_bar.GetId())
		self.tool_bar_run_status = self.ToolBar.GetToolState(self.skyeye_run_bar.GetId())

	#This function is used to enable or disable any button when remote gdb  function was called
	def remote_op_gdb(self):
		#self.save_checkpoint_menu.Enable(True)
		self.change_workspace_menu.Enable(False)
		self.create_workspace_menu.Enable(False)
		#self.registers_menu.Enable(True)
		#self.memories_menu.Enable(True)
		#self.remote_menu.Enable(True)
		#self.device_reg_menu.Enable(True)
		#self.disassembler_menu.Enable(True)
		#self.faule_inject_menu.Enable(True)
		#self.pc_recorder_menu.Enable(True)
		#self.mem_map_browse_menu.Enable(False)
		#self.conf_dev_system_bar.Enable(False)
		#self.skyeye_run_bar.Enable(False)
		#self.skyeye_restart_bar.Enable(False)

	#This function is used to enable or disable any button when stop remote gdb  function was called
	def stop_remote_op_button(self):
		#self.save_checkpoint_menu.Enable(True)
		self.change_workspace_menu.Enable(True)
		self.create_workspace_menu.Enable(True)
		if self.open_conf_flage == False:
			self.ToolBar.ToggleTool(self.skyeye_stop_bar.GetId(), False)
		#self.registers_menu.Enable(True)
		#self.memories_menu.Enable(True)
		#self.remote_menu.Enable(True)
		#self.device_reg_menu.Enable(True)
		#self.disassembler_menu.Enable(True)
		#self.faule_inject_menu.Enable(True)
		#self.pc_recorder_menu.Enable(True)
		#self.mem_map_browse_menu.Enable(False)
		#self.conf_dev_system_bar.Enable(False)
		#self.skyeye_run_bar.Enable(False)
		#self.skyeye_restart_bar.Enable(False)

	def add_history_menu(self):
		if(self.profile.has_section("history") == False):
			return
		keys = self.profile.keys("history")
		amount = len(keys)
		j = 1
		for i in range(amount, 0, -1):
			value = self.profile.get("history", "%d"%i)
			history_value = "%d"%j + " " + value
			self.history = wx.MenuItem(self.file_menu, wx.NewId(), history_value)
			self.file_menu.Append(self.history)
			self.Bind(wx.EVT_MENU, self.history_load_conf, id=self.history.GetId())
			j = j + 1


	def history_load_conf(self, event):
		item = self.GetMenuBar().FindItemById(event.GetId())
		text = item.GetItemLabelText()
		file = text[text.index('(') + 1:text.index(')')]
		path = os.path.dirname(file)
		if os.path.exists(file) == True:
			os.chdir(path)
			self.configure_file_path = file
			self.on_skyeye_restart("Reload")
		else:
			dlg = wx.MessageDialog(None, "非法路径或路径不存在!", "提示", wx.OK)
			dlg.ShowModal()
			return
	#This function is used to switch to the work dir where you start the skyeye
	def switch_to_start_path(self):
		os.chdir(self.start_path)

	#This function is used to save the work dir where you start the skyeye
	def save_start_path(self):
		self.start_path = os.getcwd()

	def on_open_config(self,evt):
		configure_file = ""
		Previous_OpenConfFlag = self.open_conf_flage
		if self.open_conf_flage == 0:
			self.open_conf_flage = 1
		else:
			retcode = GuiDialog(self, "镜像已加载， 是否重新加载？", "确认", 0)
			if(retcode != True):
				return

		if WS.ws_config.has_key("common", "lastworkspace") == True:
			defaultopendir =  WS.ws_config.get("common", "lastworkspace")
		else:
			defaultopendir =  se_path.default_workspace
		dialog = wx.FileDialog(None, "打开配置文件", defaultopendir, "", "SkyEye scripts(*.skyeye)|*.skyeye", wx.FD_OPEN)
		if dialog.ShowModal() == wx.ID_OK:
			configure_file =  dialog.GetPath()
			self.configure_file_path = configure_file
			try:
				os.chdir(dialog.GetDirectory())
			except:
				pass
			if configure_file == "":
				return
			ShowSkyConf(configure_file)
			self.on_skyeye_restart("Reload")
		else:
			self.open_conf_flage = Previous_OpenConfFlag

	def on_open_new_sessio(self,evt):
		self.on_skyeye_restart("NewSession")

	def on_open_checkpoint(self,evt):
		ws_dir = WS.get_workspace_dir()
		f = fsel.select_file(self, "选择快照文件", ws_dir, filters=[fsel.Filter_checkpoint(), fsel.Filter_all()])
		if f != None:
			self.configure_file_path = f
			self.on_skyeye_restart("ReadCheckpoint")

	def on_save_checkpoint(self,evt):
		config = se_conf.GetGlobalConfig()
		mach_list = config.get_mach_list()
		for mach in mach_list:
			cpu_cls = config.get_cpucls_list_by_mach(mach)
			for cpu in cpu_cls:
				if cpu not in checkpoint_arch:
					LogReport("%s处理器模块不支持快照保存!" % cpu)
					return
		ws_dir = WS.get_workspace_dir()
		f = fsel.select_file(self, "保存快照文件", ws_dir, save_name='skyeye.ckpt', allow_overwrite=False)
		if f == None:
			return
		cmd = cli.GetCommand("write-configure")
		if cmd == None:
			LogReport('write-configure: Can not find command "write-configure"')
			return
		try:
			cmd.run([f])
		except Exception as e:
			LogReportPack(errormessage.GetErrorInfo())

	def on_change_workspac(self,evt):
		args = [self.workspace_dir_value]
		ws = WS.change_workspace(self.workspace_dir_value)
		if ws:
			self.workspace_dir_value = ws
			WS.ws_config.set("common", "lastworkspace", self.workspace_dir_value)
			self.on_skyeye_restart("SwitchWorkSpace")

	def on_create_workspace(self,evt):
		ws = WS.create_workspace()
		if ws != None:
			self.workspace_dir_value = ws
			WS.ws_config.set("common", "lastworkspace", self.workspace_dir_value)
			self.on_skyeye_restart("SwitchWorkSpace")

	def on_registers(self,evt):
		if self.cpu_registers_frame == None:
			self.cpu_registers_frame = cpu_register_gui.CpuRegister(self)
			self.cpu_registers_frame.Show()

	def on_memories(self,evt):
		if self.memory_frame == None:
			self.memory_frame = memory_gui.MemFrame(self)
			self.memory_frame.Show()

	def on_remote(self,evt):
		if self.main_frame_enable == False:
			return
		self.remote_op_gdb()
		self.remote_gdb_status = True
		dlg = remote_gdb_gui.RemoteGdbDialog(parent = self)
		ret = dlg.ShowModal()
		try:
			SkyEyeStop()
		except:
			self.ErrorReport()
		self.stop_remote_op_button()
		self.refurbish_sub_gui()
		self.remote_gdb_status = False

	def on_device_reg(self,evt):
		if self.device_register_frame == None:
			self.device_register_frame = device_regs_gui.DevRegsFrame(parent = self)
			self.device_register_frame.Show()

	def update_dis_reverse(self, enable):
		if self.dissemble_frame != None:
			self.dissemble_frame.update_reverse(enable)

	def on_disassembler(self,evt):
		if self.dissemble_frame == None:
			self.dissemble_frame = dissemble_gui.DisFrame(parent = self)
			self.dissemble_frame.Show()
			enable = self.ToolBar.GetToolState(self.skyeye_reverse_bar.GetId())
			self.update_dis_reverse(enable)

	def on_faule_inject(self,evt):
		if self.fault_inject_frame == None:
			self.fault_inject_frame = fj_gui.FaultInjectFrame(parent = self)
			self.fault_inject_frame.Show()

	def on_pc_recorder(self,evt):
		if self.pc_recorder_frame == None:
			self.pc_recorder_frame = pc_recorder_gui.PcRecorder(parent = self)
			self.pc_recorder_frame.Show()

	def on_log(self,evt):
		if self.log_frame == None:
			self.log_frame = log_gui.LogDialog(parent = self)
			self.log_frame.Show()

	def on_option(self,evt):
		dialog = option_gui.OptionDialog(parent = self)
		ret = dialog.ShowModal()

	def on_button(self,evt):
		if self.button_frame == None:
			self.button_frame = general_button_gui.general_gpio_button_frame(parent = self)
			self.button_frame.Show()

	def on_output(self,evt):
		self.output_gui.Show()

	def on_write_memory(self,evt):
		if self.write_memory_frame == None:
			self.write_memory_frame = write_memory_gui.write_mem_to_x86(self)
			self.write_memory_frame.Show()

	def on_module_browse(self,evt):
		if self.module_frame == None:
			self.module_frame = module_gui.ModuleBrowseDialog(parent = self)
			self.module_frame.Show()

	def on_mem_map_browse(self,evt):
		if self.mem_map_frame == None:
			self.mem_map_frame = mem_map_gui.MemoryBrowserWindow(self)
			self.mem_map_frame.Show()

	def on_command_browse(self,evt):
		if self.command_frame == None:
			self.command_frame =  command_gui.CommandBrowserWindow(parent = self)
			self.command_frame.Show()

	def on_doc(self,evt):
		name = se_path.GetDocPath("help.chm")
		prc = subprocess.Popen(name, shell = True)
		self.help_list.append(prc)

	def on_lic(self,evt):
		lic = lic_gui.LicDialog()
		lic.ShowModal()

	def on_about(self,evt):
		if (self.about_frame == None):
			self.about_frame = about_gui.AboutDialog(parent = self)
			self.about_frame.Show()

	'''
	def on_conf_dev_system(self,evt):
		gp_dir = os.path.join(se_path.InstallDir, "uiConfig")
		if os.path.exists(gp_dir) == False:
			return
		import framework
		version  = framework.get_framework_version()
		ret  = framework.check_framework_version()
		if ret == False:
			ms = "要求.NET Framework 版本高于或等于4.0,本机的安装版本为 " + str(version) + ",请更新版本"
			GuiDialog(self, ms, "提示", 1)
		tmp_cwd = os.getcwd()
		os.chdir(os.path.dirname(se_path.ModelPath))
		if self.workspace_dir_value == None:
			ws = "C:\\"
		ws = self.workspace_dir_value
		cmd = "%s %s" % (se_path.ModelPath, ws)
		s = subprocess.Popen(cmd)
		self.set_configure.append(s)
		os.chdir(tmp_cwd)
	'''

	def on_skyeye_stop(self,evt):
		if self.main_frame_enable == False:
			return
		if self.open_conf_flage == 0:
			self.ToolBar.ToggleTool(self.skyeye_stop_bar.GetId(), False)
			return
		if self.tool_bar_stop_status == False:
			self.ToolBar.ToggleTool(self.skyeye_stop_bar.GetId(), True)
		else:
			self.ToolBar.ToggleTool(self.skyeye_stop_bar.GetId(), True)
			return
		try:
			SkyEyeStop()
		except:
			self.ErrorReport()
		self.stop_op_button()
		self.running = False
		self.refurbish_sub_gui()
		pytimer.set_running(False)

	def on_skyeye_run(self,evt):
		if self.main_frame_enable == False:
			return
		if self.open_conf_flage == 0:
			self.ToolBar.ToggleTool(self.skyeye_run_bar.GetId(),False)
			return
		if self.tool_bar_run_status == False:
			self.ToolBar.ToggleTool(self.skyeye_run_bar.GetId(),True)
		else:
			self.ToolBar.ToggleTool(self.skyeye_run_bar.GetId(),True)
			return
		self.clear_breakpoint_flag()
		try:
			SkyEyeRun()
		except:
			self.ErrorReport()
		self.run_op_button()
		self.stepi_mode = False
		self.running = True
		pytimer.set_running(True)

	def on_skyeye_restart(self,evt):
		restart_flag_list = ["Reload", "SwitchWorkSpace", "NewSession", "ReadCheckpoint"]
		cg=None
		cg = se_conf.GetGlobalConfig()
		if cg!=None:
			machlist = cg.get_mach_list()
			for mach in machlist:
				cpuclass=cg.get_cpuname_list_by_mach(mach)
				for cpu_name in cpuclass:
					cls = cg.get_classname(cpu_name)
					if cls != None and cls == "x86":
						SkyEyeX86Reset(cpu_name)
						return

		self.profile.set("common", "reset", "yes")
		if evt == restart_flag_list[2]:
			self.profile.set("openconf", "path", "null")
		else:
			self.profile.set("openconf", "path", self.configure_file_path)

		WS.ws_config.set("common", "lastworkspace", self.workspace_dir_value)
		se_conf.SetGlobalConfig(None)
		se_conf.SetReverseState(False)
		self.close_sub_gui()
		global AppRestart
		AppRestart = True
		self.end_handler()
		self.Destroy()
		wx.Exit()

	def enable_revexec(self, enable):
		if not enable:
			retcode = GuiDialog(self, "如果确定取消,你将无法基于之前保存的状态点做逆向执行, 保存点的状态会被清除！", "取消逆向执行?", 0)
			if retcode == False:
				self.ToolBar.ToggleTool(self.skyeye_reverse_bar.GetId(), True)
				return
			cmd = cli.GetCommand("disable-reverse")
			if cmd == None:
				LogReport('disable_reverse: Can not find command "disable-reverse"')
				return
			try:
				cmd.run([])
			except Exception as e:
				LogReportPack(errormessage.GetErrorInfo())
		else:
			cmd = cli.GetCommand("enable-reverse")
			if cmd == None:
				LogReport('enable_reverse: Can not find command "enable-reverse"')
				return
			try:
				cmd.run([])
			except Exception as e:
				LogReportPack(errormessage.GetErrorInfo())
		self.update_dis_reverse(enable)

	def on_skyeye_reverse(self,evt):
		config = se_conf.GetGlobalConfig()
		enable = self.ToolBar.GetToolState(self.skyeye_reverse_bar.GetId())
		if config == None:
			self.ToolBar.ToggleTool(self.skyeye_reverse_bar.GetId(), not enable)
			GuiDialog(self, "需要配置文件被加载!", "逆向执行", 1)
			return
		mach_list = config.get_mach_list()
		for mach in mach_list:
			cpu_cls = config.get_cpucls_list_by_mach(mach)
			for cpu in cpu_cls:
				if cpu not in reverse_arch:
					LogReport("%s 处理器模块不支持逆向执行!" %cpu )
					self.ToolBar.ToggleTool(self.skyeye_reverse_bar.GetId(), not enable)
					return
		self.enable_revexec(enable)

	def on_skyeye_up(self,evt):
		enable = self.ToolBar.GetToolState(self.skyeye_up_bar.GetId())
		config = se_conf.GetGlobalConfig()
		if not config:
			self.ToolBar.ToggleTool(self.skyeye_up_bar.GetId(), not enable)
			GuiDialog(self, "需要配置文件被加载!", "加速执行", 1)
			return
		if enable:
			se_conf.SetAllToDyncom(1)
			SkyEyeSetTimeNum(1)
			mips.time_num = 5
		else:
			se_conf.SetAllToDyncom(0)
			SkyEyeSetTimeNum(1)

	def stop_t(self):
		coverage.kill_objdump_thread()
		self.end_handler()
		self.Destroy()
		global AppRestart
		AppRestart = False
		wx.Exit()

	def quit_skyeye(self,evt):
		self.close_sub_gui()
		_thread.start_new_thread(self.stop_t,())

	def get_last_workspace(self):
		if WS.ws_config.has_section("common"):
			if WS.ws_config.has_key("common", "lastworkspace"):
				value = WS.ws_config.get("common", "lastworkspace")
				if value == None:
					return None
				else:
					return value
			else:
				return  None
		else:
			return None
	def clear_breakpoint_flag(self):
		machlist = self.config.get_mach_list()
		for mach in machlist:
			core_name_list = self.config.get_cpuname_list_by_mach(mach)
			for core_name in core_name_list:
				try:
					ret = SkyEyeClearBpHit(core_name)
				except:
					self.ErrorReport()


	def first_create_workspace(self):
		dlg = WS.select_workspace(self, self)
		ret = dlg.ShowModal()

	def set_last_workspace(self, ws):
		self.workspace_dir_value = ws

	def update_mach_info(self, info):
		if info == None:
			machine_info = MachInfo(self)
			info = machine_info.get_info()
			self.scrolled = MachineWindow(self, info)
			self.Box.Add(self.scrolled, 1, wx.EXPAND)
			self.SetSize((400, 410))
		else:
			self.scrolled.Clear()
			self.scrolled.Add(info)
			num = len(info)
			if num == 1:
				self.SetSize((400, 401))
			else:
				self.SetSize((400, 470))

	# Close sub gui if you want
	def close_sub_gui(self):
		if(self.cpu_registers_frame != None):
			self.cpu_registers_frame.Close()
		if(self.memory_frame != None):
			self.memory_frame.Close()
		if(self.device_register_frame != None):
			self.device_register_frame.Close()
		if(self.log_frame != None):
			self.log_frame.Close()
		if(self.fault_inject_frame != None):
			self.fault_inject_frame.Close()
		if(self.write_memory_frame != None):
			self.write_memory_frame.Close()
		NetCtrl.server_stop()
		self.close_gp_config()
		self.close_secoverage()
		if len(self.help_list) != 0:
			for sub in self.help_list:
				ret = sub.poll()
				if ret == None:
					cmd = "taskkill /f /t /pid %d" % sub.pid
					subprocess.call(cmd)
		self.set_configure = []
		self.se_coverage = []
		self.help_list = []
		self.close_custom_dlg()


	# Add the SubGui's refurbish in this function, call it when you stop skyeye
	def refurbish_sub_gui(self):
		if(self.cpu_registers_frame != None):
			self.cpu_registers_frame.RegsRefurbish()
		if(self.device_register_frame != None):
			self.device_register_frame.RegsRefurbish()
		if(self.dissemble_frame != None):
			self.dissemble_frame.Refurbish()
		if(self.fault_inject_frame != None):
			self.fault_inject_frame.Refurbish()

	def update_sub_gui(self, dg):
		if(self.cpu_registers_frame != None and self.cpu_registers_frame != dg):
			self.cpu_registers_frame.RegsRefurbish()
		if(self.device_register_frame != None and self.device_register_frame != dg):
			self.device_register_frame.RegsRefurbish()
		if(self.dissemble_frame != None and self.dissemble_frame != dg):
			self.dissemble_frame.Refurbish()
		if self.pc_recorder_frame != None:
			self.pc_recorder_frame.RecordRefurbish()

	# Add the SubGui's refurbish in this function, call it when you click the next step button in disassemble function
	def refurbish_sub_guiForStepi(self):
		if(self.cpu_registers_frame != None):
			self.cpu_registers_frame.RegsRefurbish()
		if(self.device_register_frame != None):
			self.device_register_frame.RegsRefurbish()

	def register_thread_func(self, obj, func):
		self.thread_pool_mutex.acquire()
		thread = [obj, func]
		self.thread_pool.append(thread)
		self.thread_pool_mutex.release()

	def remove_thread_func(self, obj):
		self.thread_pool_mutex.acquire()
		for i,e in enumerate(self.thread_pool):
			if e[0] is obj:
				#self.thread_pool[i][0] = None
				del self.thread_pool[i]
		self.thread_pool_mutex.release()

	'''
	def create_gp_config(self):
		gp_dir = os.path.join(se_path.InstallDir, "uiConfig")
		if os.path.exists(gp_dir) == False:
			return
		self.open_device_config_menu = self.file_menu.Append(-1,"配置硬件系统")
		self.Bind(wx.EVT_MENU, self.on_conf_dev_system, self.open_device_config_menu)

	def create_se_coverage(self):
		secoverage_dir = os.path.join(se_path.InstallDir, "SECoverage")
		if os.path.exists(secoverage_dir) == False:
			return
		self.open_coverage_menu = self.file_menu.Append(-1,"代码覆盖率测试")
		self.Bind(wx.EVT_MENU, self.open_coverage, self.open_coverage_menu)

	def open_coverage(self, event):
		if os.path.exists(os.path.dirname(se_path.SeCoveragePath)) == False:
			return
		tmp_cwd = os.getcwd()
		os.chdir(os.path.dirname(se_path.SeCoveragePath))
		cmd = "%s" % (se_path.SeCoveragePath)
		s = subprocess.Popen(cmd)
		self.se_coverage.append(s)
		os.chdir(tmp_cwd)
	'''

	def custom_func(self, event):
		item = self.GetMenuBar().FindItemById(event.GetId())
		text = item.GetItemLabelText()
		for cls in self.clses:
			if(operator.eq(text, cls[0]) == True):
				if self.check_dlg_exist(cls[1]) == True:
					continue
				dlg = cls[1](parent = self, args = None)
				iconame = se_path.GetPicPath("skyeye.ico")
				icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
				dlg.SetIcon(icon)

				dlg.Show()
				node = [cls[1], dlg]
				self.cdialog_show.append(node)

	def create_custom_menu(self, menu):
		self.clses = CDialog.get_custom_dialog()
		for cls  in self.clses:
			custommenu  = wx.MenuItem(menu, wx.NewId(), cls[0])
			menu.Append(custommenu)
			self.Bind(wx.EVT_MENU, self.custom_func, id= custommenu.GetId())
	def check_dlg_exist(self, cls):
		for dic in self.cdialog_show:
			if dic[0] == cls:
				try:
					dic[1].GetId()
				except:
					continue
				return True
		return False

	def close_custom_dlg(self):
		for dic in self.cdialog_show:
			try:
				dic[1].Close()
			except:
				pass
		self.cdialog_show = []

	def custom_close_self(self, custom):
		for dic in self.cdialog_show:
			if dic[1] == custom:
				self.cdialog_show.remove(dic)
	def close_secoverage(self):
		if len(self.se_coverage) != 0:
			for sub in self.se_coverage:
				try:
					subprocess.Popen.kill(sub)
				except:
					pass

	def close_gp_config(self):
		if len(self.set_configure) != 0:
			for sub in self.set_configure:
				try:
					subprocess.Popen.kill(sub)
				except:
					pass

	def reload_config(self):
		if self.profile.has_section("openconf") == False:
			return
		path = self.profile.get("openconf", "path")
		if operator.eq(path, "null") == True:
			return
		else:
			self.profile.set("openconf", "path", "null")
			if os.path.exists(path) == False:
				dlg = wx.MessageDialog(None, "非法路径或路径不存在!", "提示", wx.OK)
				dlg.ShowModal()
				return
			if path.endswith(".ckpt"):
				cmd = cli.GetCommand("read-configure")
				if cmd == None:
					LogReport('read-configure: Can not find command "read-configure"')
					return
				try:
					cmd.run([path])
				except Exception as e:
					LogReportPack(errormessage.GetErrorInfo())
					return
				self.configure_file_path = path
				self.record_history()
			else:
				e = os.path.dirname(path)
				os.chdir(e)
				ret = self.load(path)
				if ret == False:
					self.configure_file_path = "null"
					return  False
			self.open_conf_op_button()
			self.open_conf_flage = 1
			self.machine = MachInfo(self)
			self.config = se_conf.GetGlobalConfig()
			machlist = self.config.get_mach_list()
			self.mach_count = len(machlist)
			for mach in machlist:
				cpu = self.config.get_cpucls_list_by_mach(mach)
				self.machine.add_machine_info(mach, cpu, "1GB", "NULL", "NULL")
			info = self.machine.get_info()

			wx.CallAfter(self.update_mach_info, info)

			res = self.profile.get("common", "reset")
			if operator.eq(res, "no") == True:
				self.record_history()
			else:
				self.profile.set("common", "reset", "no")
			return True

	def load_conf(self, confpath):
		self.skyeye_conf = SkyEyeConf(confpath)
		try:
			cmd = cli.GetCommand("run-script")
			if cmd == None:
				print ("Can not find command: run-script ")
				return  False
			se_conf.SetGlobalScript(confpath)
			arglist = [confpath]
			try:
				ret = cmd.run(arglist)
			except:
				LogReportPack(errormessage.GetErrorInfo())
				return False
			if ret == False:
				return False
			self.config = se_conf.GetGlobalConfig()
			self.configure_file_path = confpath
			self.record_history()
		except :
			return  False
		return True

	def load(self, filename):
		if(self.open_conf_flage == 0):
			ret = self.load_conf(filename)
			return ret
		else:
			retcode = GuiDialog(self, "镜像已加载， 是否重新加载？", "确认", 0)
			if(retcode == True):
				self.on_skyeye_restart("Reload")
			else:
				return  False

	#This function is used to preserve historical records
	def record_history(self):
		max_num = 10
		if(self.profile.has_section("history") == False):
			keys = []
		else:
			keys = self.profile.keys("history")
		amount = len(keys)
		current_amount = amount
		histroy_list = []
		for i in range(1, amount+1):
			value = self.profile.get("history", "%d"%(i))
			histroy_list.append(value)
		new_value = "(" + self.configure_file_path + ")"
		if new_value in histroy_list:
			index = histroy_list.index(new_value)
			del histroy_list[index]
		elif len(histroy_list) >= max_num:
			del histroy_list[0]
		else:
			current_amount = amount + 1
		histroy_list.append(new_value)
		for i in range(0, current_amount):
			if i == max_num:
				break
			self.profile.set("history", "%d"%(i+1), histroy_list[i])

	def ErrorReport(self):
		mess = errormessage.GetErrorInfo()
		errmesg = ""
		detailmesg = mess[0]
		print (errmesg)
		print (detailmesg)
		dlg = ErrorReportDialog(errormesg = errmesg, detail = detailmesg)
		ret = dlg.ShowModal()
		if ret == wx.ID_CLOSE:
			self.quit_skyeye(0)
		elif ret == wx.ID_RESET:
			self.on_skyeye_restart("NewSession")
def GetImage(self, picture, l, h):
	image = wx.Image(picture)
	image.Rescale(l, h)
	imageBit = wx.Bitmap(image)
	return imageBit

def GuiDialog(self, message, title, flag):  #Return value: True/False, flag: 0 indicate YES and NO button, 1 indicate YES button only
	if flag == 0:
		dlg = wx.MessageDialog(None, message, title, wx.YES_NO)
		retcode = dlg.ShowModal()
		if(retcode == wx.ID_YES):
			dlg.Destroy()
			return True
		else:
			dlg.Destroy()
			return False
	elif flag == 1:
		dlg = wx.MessageDialog(None, message, title, wx.OK)
		retcode = dlg.ShowModal()
		if(retcode == wx.ID_OK):
			dlg.Destroy()
			return True
		else:
			dlg.Destroy()
			return False
	else:
		dlg = wx.MessageDialog(None, "error prompt", title, wx.OK)

def ShowSkyConf(filename):
	f = open(filename)
	while True:
		line = f.readline()
		if len(line) == 0:
			break
	f.close()


class SkyEyeGUI(wx.App):
	def __init__(self):
		wx.App.__init__(self,redirect=False)
	def OnInit(self):
		#self.locale = wx.Locale(wx.LANGUAGE_CHINESE_SIMPLIFIED)
		ret = CheckKEY()
		if ret == False:
			return False
		else:
			pass
		init_gui_log_view()
		self.frame = MainFrame(parent = None)
		self.frame.Show()
		self.SetTopWindow(self.frame)
		self.frame.output_gui = print_log_gui.Output(parent = self.frame)
		se.log=self.frame.output_gui
		return True
	def OnExit(self):
		try:
			self.Destroy()
		except:
			pass
		return True

def CheckKEY():
	ret = se_lic.license_verify()
	if ret[0] == False:
		lic = lic_gui.LicDialog()
		lic.ShowModal()
		ret = se_lic.license_verify()
		if ret[0] == False:
			return False
	return True

def Main():
	global AppRestart
	while(AppRestart):
		SkyEyeReset()
		app = SkyEyeGUI()
		app.locale = wx.Locale(wx.LANGUAGE_CHINESE_SIMPLIFIED)
		app.MainLoop()
	SkyEyeQuit()


def main_control():
	Main()


if __name__ == '__main__':
	main_control()
	#app = SkyEyeGUI()
	#app.MainLoop()
