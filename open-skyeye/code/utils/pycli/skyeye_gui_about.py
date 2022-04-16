# -*- coding: utf-8 -*-
import wx, sys, os
from skyeye_common_module import *
import se_path
import tools
import skyeye_license as se_lic

fname =  'Segoe UI'


class main_panel(wx.Panel):
	def __init__(self, parent):
		wx.Panel.__init__(self, parent, style = wx.BORDER_SUNKEN)
		self.SetFont(wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName = fname))

		self.SetBackgroundColour(wx.WHITE)

		logo = wx.StaticText(self, -1,  "DIGIPROTO", style = wx.ALIGN_CENTRE_HORIZONTAL)
		logo.SetFont(wx.Font(30,wx.FONTFAMILY_DEFAULT, wx.ITALIC, wx.FONTWEIGHT_BOLD, faceName=fname))
		logo.SetForegroundColour(wx.RED)
		sizer_a = wx.BoxSizer(wx.HORIZONTAL)
		sizer_a.Add((0, 0), 0, wx.EXPAND)
		sizer_a.Add(logo, 1, wx.EXPAND|wx.ALIGN_CENTER)
		sizer_a.Add((0, 0), 0, wx.EXPAND)

		sizer = wx.BoxSizer(wx.VERTICAL)
		self.vera = wx.StaticText(self, -1,  " ", style = wx.ALIGN_CENTRE_HORIZONTAL)
		self.vera.SetFont(wx.Font(15, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName = fname))
		verb = wx.StaticText(self, -1, "版权所有 2013-2022 北京迪捷数原科技有限公司 保留所有权利", style = wx.ALIGN_CENTRE_HORIZONTAL)
		
		lic_info= wx.StaticBox(self, -1, "产品信息")  
		static_sizer2 = wx.StaticBoxSizer(lic_info, wx.VERTICAL)  
		self.sw = wx.ScrolledWindow(self, wx.ID_ANY)
		self.sw.SetFont(wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName = fname))
		self.sw.EnableScrolling(False, True)
		self.sw.SetScrollRate(0, 20)
		self.sw.SetSizeHints(-1, self.sw.GetCharHeight() * 20)
		self.sw.SetBackgroundColour(wx.WHITE)

		lic_sizer = wx.GridBagSizer(hgap=5, vgap=5)

		sizer_sw = wx.BoxSizer(wx.VERTICAL)
		pn = wx.StaticText(self.sw, -1,  "产品名称:", style = wx.ALIGN_RIGHT)
		pns = wx.StaticText(self.sw, -1,  "SkyEye硬件仿真系统", style = wx.ALIGN_CENTRE_HORIZONTAL)
		version = wx.StaticText(self.sw, -1,  "版本:", style = wx.ALIGN_RIGHT)
		self.version_id = wx.StaticText(self.sw, -1,  "")
		commit_id = wx.StaticText(self.sw, -1,  "版本提交:", style = wx.ALIGN_RIGHT)

		self.commits = wx.TextCtrl(self.sw, -1, style =wx.TE_READONLY | wx.TE_RICH2 |wx.TE_CENTER)
		
		########################################
		lic_type_key = wx.StaticText(self.sw, -1,  "许可类型:", style = wx.ALIGN_CENTRE_HORIZONTAL)
		lic_cpu_key = wx.StaticText(self.sw, -1,  "许可核心:", style = wx.ALIGN_RIGHT)
		lic_time_key = wx.StaticText(self.sw, -1,  "许可时限:", style = wx.ALIGN_CENTRE_HORIZONTAL)
		self.lic_time_value = wx.StaticText(self.sw, -1,  "剩余15天", style = wx.ALIGN_CENTRE_HORIZONTAL)
		#self.lic_cpu_value = wx.StaticText(self.sw, -1,  lic_cpu_text, style = wx.ALIGN_LEFT)
		self.lic_cpu_value = wx.ListBox(self.sw, -1,size=(80,80))
		self.lic_type_value = wx.StaticText(self.sw, -1,  "本地许可", style = wx.ALIGN_CENTRE_HORIZONTAL)
		########################################

		lic_sizer.Add((20, 20), pos = (0, 0))
		lic_sizer.Add(pn, pos = (0, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		lic_sizer.Add(pns, pos = (0, 2),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 5)
		lic_sizer.Add((20, 20), pos = (1, 0))
		lic_sizer.Add(version, pos = (1, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		lic_sizer.Add(self.version_id, pos = (1, 2),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 5)
		lic_sizer.Add((20, 20), pos = (2, 0))
		lic_sizer.Add(commit_id, pos = (2, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		lic_sizer.Add(self.commits, pos = (2, 2),flag = wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 5)
		lic_sizer.Add((20, 20), pos = (3, 0))
		########################################
		lic_sizer.Add(lic_type_key, pos = (3, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border=5)
		lic_sizer.Add(self.lic_type_value, pos = (3, 2),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 5)

		lic_sizer.Add((20, 20), pos = (4, 0))
		lic_sizer.Add(lic_time_key, pos = (4, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		lic_sizer.Add(self.lic_time_value, pos = (4, 2),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 5)
		lic_sizer.Add((20, 20), pos = (5, 0))
		lic_sizer.Add(lic_cpu_key, pos = (5, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		lic_sizer.Add(self.lic_cpu_value, pos = (5, 2),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER|wx.EXPAND, border= 5)
		########################################
		lic_sizer.AddGrowableCol(2)
		sizer_sw.Add(lic_sizer, 0, wx.EXPAND)
		sizer_sw.Add((20, 20), 0, wx.EXPAND)

		self.package_sizer = wx.GridBagSizer(hgap=5, vgap=5)
		pkg_name = wx.StaticText(self.sw, -1,  "组件名称", style = wx.ALIGN_RIGHT)
		self.set_bold(pkg_name)
		pkg_path = wx.StaticText(self.sw, -1,  "安装路径", style = wx.ALIGN_RIGHT)
		self.set_bold(pkg_path)
		self.package_sizer.Add((5, 5), pos = (0, 0),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 10)
		self.package_sizer.Add(pkg_name, pos = (0, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 10)
		self.package_sizer.Add(pkg_path, pos = (0, 2),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 10)
		static_line = wx.StaticLine(self.sw)
		self.package_sizer.Add(static_line, pos = (1, 1), span = (1, 2),flag = wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 1)
		self.package_num = 1
		
		self.package_sizer.AddGrowableCol(2)
		sizer_sw.Add(self.package_sizer, 1, wx.EXPAND)
		self.sw.SetSizer(sizer_sw)

		static_sizer2.Add(self.sw,1, flag = wx.EXPAND)
		
		sizer.Add(sizer_a, 0, wx.EXPAND)
		sizer.Add((15, 15), 0, wx.EXPAND)
		sizer.Add(self.vera, 0, wx.EXPAND)
		sizer.Add((5, 5), 0, wx.EXPAND)
		sizer.Add(verb, 0, wx.EXPAND)
		sizer.Add((15, 15), 0, wx.EXPAND)
		#sizer.Add(static_sizer1, 0, wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, 10)
		sizer.Add(static_sizer2, 1, wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, 10)
		self.SetSizer(sizer)
		self.Fit()
		########################################
		lic_data=se_lic.gui_get_license_info()
		LicMode=lic_data.get_license_mode()
		LicTime=lic_data.get_license_time()
		LicCpus=lic_data.get_license_cpus()

		########################################

		package_config = tools.GetPackageConfig()
		builddate = tools.GetBuildDate(package_config)
		self.buildversion = tools.GetBuildVersion(package_config)
		versionStr = self.buildversion + " Build-" + builddate
		commit_str = tools.GetCommitId(package_config)
		########################################
		self.SetLicMode(LicMode)
		self.SetLicTime(LicTime)
		self.SetLicCpus(LicCpus)
		########################################
		self.SetVerisonID(versionStr)
		self.SetCommitID(commit_str)
		package_dic = tools.GetPackageDic()
		for p in package_dic.keys():
			self.add_package(p, package_dic[p])


	def SetLicCpus(self, info):
		#self.lic_cpu_value.SetLabel(info)
		self.lic_cpu_value.Set(info)

	def SetLicMode(self, info):
		self.lic_type_value.SetLabel(info)

	def SetLicTime(self, info):
		self.lic_time_value.SetLabel(info)

	def SetVerisonID(self, info):
		self.version_id.SetLabel(info)
		self.vera.SetLabel("SkyEye  %s" % self.buildversion)

	def SetCommitID(self, info):
		self.commits.SetValue(info)

	def add_package(self, name, path):
		self.package_num = self.package_num + 1
		pkg_name = wx.StaticText(self.sw, -1,  name, style = wx.ALIGN_RIGHT)
		pkg_path = wx.StaticText(self.sw, -1,  path, style = wx.ALIGN_CENTER)
		self.package_sizer.Add((5, 5), pos = (self.package_num, 0),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 10)
		self.package_sizer.Add(pkg_name, pos = (self.package_num, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 10)
		self.package_sizer.Add(pkg_path, pos = (self.package_num, 2),flag = wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER, border= 10)
		
	def set_bold(self, ctr):
		font = ctr.GetFont()
		font.SetWeight(wx.BOLD)
		ctr.SetFont(font)

class AboutDialog(wx.Frame):
	def __init__(self, parent):
		wx.Frame.__init__(self, parent, -1, "关于SkyEye")
		self.win = parent
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.SetBackgroundColour("#D8D8BF")
		panel = main_panel(self)
		sizer.Add(panel, 1, wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, 5)
		sizer.Add((0,0), 0, wx.EXPAND)
		self.SetSizer(sizer)
		self.Fit()
		self.SetSize((500 , 500))
		self.SetMaxSize((500, 500))
		self.Bind(wx.EVT_CLOSE, self.CloseSelf, self)
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)

	
	def CloseSelf(self, event):
		self.win.about_frame = None
		self.Destroy()
	


