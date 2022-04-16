# -*- coding: UTF-8 -*-
import wx, os
from threading import Thread
import time
import wx.lib.buttons as buttons  
from tools import Profile
from skyeye_common_module import *


class TopPanel(wx.Panel):
	def __init__(self, parent):
		wx.Panel.__init__(self, parent = parent)
		self.par = parent
		self.SetBackgroundColour("#f0f0f0")
		font = wx.Font(9, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName='Segoe UI')
		self.SetFont(font)
		sizer = wx.BoxSizer(wx.VERTICAL)

		StaticBox0 = wx.StaticBox(self, -1, "调试对象", name='grou0')
		Box0 = wx.StaticBoxSizer(StaticBox0, wx.HORIZONTAL)
		targetpanel = wx.Panel(self)
		TargetBox = wx.BoxSizer(wx.HORIZONTAL)
		TargetTite = wx.StaticText(targetpanel, -1, "调试对象")
		TargetList = self.par.par.config.get_cpu_list()
		self.TargetComboBox = wx.ComboBox(targetpanel, -1, choices = TargetList)
		self.TargetComboBox.SetEditable(False)
		self.TargetComboBox.SetSelection(0)
		TargetBox.Add(TargetTite, 1, flag = wx.ALL, border = 5)
		TargetBox.Add(self.TargetComboBox, 3, flag = wx.ALL)
		targetpanel.SetSizer(TargetBox)

		Box0.Add(targetpanel, 1, flag = wx.EXPAND | wx.ALL)
		StaticBox1 = wx.StaticBox(self, -1, "状态:", name='grou1')
		Box = wx.StaticBoxSizer(StaticBox1, wx.HORIZONTAL)
		titepanel = wx.Panel(self)
		contextpanel = wx.Panel(self)
		Box.Add(titepanel, 2, flag = wx.EXPAND | wx.ALL, border = 10)
		Box.Add(contextpanel, 4, flag = wx.EXPAND | wx.ALL, border = 10)

		sizer.Add(Box0, 2, flag = wx.EXPAND | wx.ALL, border = 10)
		sizer.Add(Box, 5, flag = wx.EXPAND | wx.ALL, border = 10)
		self.SetSizer(sizer)
			
		ConnectTite = wx.StaticText(titepanel, -1, "连接状态:")
		IpTite = wx.StaticText(titepanel, -1, "监听IP:")
		PortTite = wx.StaticText(titepanel, -1, "监听端口:")
		ClientIpTite = wx.StaticText(titepanel, -1, "远程连接IP:")
		titebox = wx.BoxSizer(wx.VERTICAL) 
		titebox.Add(ConnectTite, flag = wx.ALL, border = 7)
		titebox.Add(IpTite, flag = wx.ALL, border = 8)
		titebox.Add(PortTite, flag = wx.ALL, border = 8)
		titebox.Add(ClientIpTite, flag = wx.ALL, border = 8)
		titepanel.SetSizer(titebox)


		self.ConnectStatus = wx.ListBox(contextpanel, size = (100, 20))
		self.ConnectStatus.SetBackgroundColour(wx.RED)
		self.IpStatus = wx.ListBox(contextpanel, size = (200, 20))
		self.IpStatus.SetBackgroundColour("#f0f0f0")
		self.PortStatus = wx.ListBox(contextpanel, size = (100, 20))
		self.PortStatus.SetBackgroundColour("#f0f0f0")
		self.ClientIpStatus = wx.ListBox(contextpanel, size = (200, 20))
		self.ClientIpStatus.SetBackgroundColour("#f0f0f0")
		self.Bind(wx.EVT_LISTBOX, self.on_single_select, self.ConnectStatus)
		self.Bind(wx.EVT_LISTBOX, self.on_single_select, self.IpStatus)
		self.Bind(wx.EVT_LISTBOX, self.on_single_select, self.PortStatus)
		self.Bind(wx.EVT_LISTBOX, self.on_single_select, self.ClientIpStatus)
		self.ListBoxList = []
		self.ListBoxList.append(self.ConnectStatus)
		self.ListBoxList.append(self.IpStatus)
		self.ListBoxList.append(self.PortStatus)
		self.ListBoxList.append(self.ClientIpStatus)

		contextbox = wx.BoxSizer(wx.VERTICAL) 
		contextbox.Add(self.ConnectStatus, flag = wx.ALL, border = 5)
		contextbox.Add(self.IpStatus, flag = wx.ALL , border = 5)
		contextbox.Add(self.PortStatus, flag = wx.ALL , border = 5)
		contextbox.Add(self.ClientIpStatus, flag = wx.ALL, border = 5)
		contextpanel.SetSizer(contextbox)

		self.Thread = Thread(group=None, target=self.init)
		self.Thread.start()

	def on_single_select(self, event):
		for listbox in self.ListBoxList:
			n = listbox.GetSelection()
			if n != wx.NOT_FOUND:
				listbox.Deselect(n)
	def init(self):
		profile = Profile(None)
		if profile.has_section("RemoteGdb") == True:
			ip = profile.get("RemoteGdb", "ipaddr")
			port = profile.get("RemoteGdb", "port")
		else:
			ip = "INADDR_ANY"
			port = "10000"

		list = [] 
		list.append(ip)
		wx.CallAfter(self.IpStatus.InsertItems,list, 0)
		list = []
		list.append(port)
		wx.CallAfter(self.PortStatus.InsertItems,list, 0)
		self.set_connect_status(0)
	def set_client_ip(self, ip):
		list = [] 
		list.append(ip)
		self.ClientIpStatus.InsertItems(list, 0)
		
	def set_connect_status(self, status):
		status_list = ["    未开始", "    等待连接", "    已连接"]
		colour_list = ["#FF0000", "#FFFF00", "#00FF00"]
		wx.CallAfter(self.ConnectStatus.Clear)
		list = []
		list.append(status_list[status])
		wx.CallAfter(self.ConnectStatus.InsertItems,list, 0)
		wx.CallAfter(self.ConnectStatus.SetBackgroundColour,colour_list[status])


	def enable_target_chose(self, status):
		if status == True:
			self.TargetComboBox.Enable()
		else:
			self.TargetComboBox.Disable()
	def get_remote_target(self):
		return self.TargetComboBox.GetStringSelection()

	def get_remote_ip(self):
		ip = self.IpStatus.GetString(0)
		if ip == "INADDR_ANY":
			ip = "0.0.0.0"
		return ip

	def get_remote_port(self):
		return self.PortStatus.GetString(0)


class MidPanel(wx.Panel):
	def __init__(self, parent):
		wx.Panel.__init__(self, parent = parent)
		self.SetBackgroundColour("#f0f0f0")
		font = wx.Font(9, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName='Segoe UI')
		logfont = wx.Font(7, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName='Segoe UI')
		self.SetFont(font)
		sizer = wx.BoxSizer(wx.HORIZONTAL)
		StaticBox1 = wx.StaticBox(self, -1, "日志:", name='grou1')

		Box = wx.StaticBoxSizer(StaticBox1, wx.HORIZONTAL)
		logpanel = wx.Panel(self);
		logpanel.SetFont(logfont)
		Box.Add(logpanel, 1, flag = wx.EXPAND | wx.ALL, border = 2)
		self.Log = wx.ListBox(logpanel, -1, size=(360, 75), style=wx.LB_SINGLE | wx.LB_HSCROLL | wx.LB_NEEDED_SB )
		sizer.Add(Box, 1, flag = wx.EXPAND | wx.ALL, border = 10)
		self.SetSizerAndFit(sizer)
		self.LogList = []
		self.ItemCount = 0
	def update(self, logstr):
		wx.CallAfter(self.Log.Clear)
		logstr = "-->" + logstr
		self.LogList.append(logstr)
		wx.CallAfter(self.Log.InsertItems,self.LogList, 0)
		#count = self.Log.GetCount()
		#self.Log.SetFirstItem(count - 1)

	def update_list(self, list):
		count = len(list)
		for i in range(0, count):
			list[i] = "-->" + list[i]
		self.Log.Clear()
		self.LogList.extend(list)
		self.Log.InsertItems(self.LogList, 0)
		count = self.Log.GetCount()
		self.Log.SetFirstItem(count - 1)





class RemoteGdbDialog(wx.Dialog):
	def __init__(self, parent):
		wx.Dialog.__init__(self, parent = parent, title = "远程调试", size = (400, 480), style = wx.DEFAULT_DIALOG_STYLE | wx.MINIMIZE_BOX)
		self.par =parent
		self.toppanel = TopPanel(self)
		self.midpanel = MidPanel(self)
		self.bottonpanel = wx.Panel(self)
		#self.bottonpanel.SetBackgroundColour("#f0f0f0")
		font = wx.Font(9, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName='Segoe UI')
		self.bottonpanel.SetFont(font)
		self.StartButton = buttons.GenToggleButton(self.bottonpanel, label = "开始", pos = (100, 4), size = (80, 28))
		self.StopButton = buttons.GenToggleButton(self.bottonpanel, label = "停止", pos = (200, 4), size = (80, 28))
		self.StopButton.SetToggle(True)
		CloseButton = buttons.GenButton(self.bottonpanel, label = "关闭", pos = (305, 4), size = (80, 28))
		Box = wx.BoxSizer(wx.VERTICAL)
		Box.Add(self.toppanel, 6, flag = wx.EXPAND)
		Box.Add(self.midpanel, 3, flag = wx.EXPAND)
		Box.Add(self.bottonpanel, 1, flag = wx.EXPAND)
		self.SetSizer(Box)
		self.Bind(wx.EVT_BUTTON, self.on_close_button, CloseButton)
		self.Bind(wx.EVT_BUTTON, self.on_start_button, self.StartButton)
		self.Bind(wx.EVT_BUTTON, self.on_stop_button, self.StopButton)
		self.Bind(wx.EVT_CLOSE, self.on_close_button, self)
		self.CheckLinkThread = Thread(group=None, target=self.check_link)
		self.CheckLinkThread.start()
		self.target_cpu = None
		self.ClientIp = None
		self.CloseFlag = False
	def connect_func(self):
		self.toppanel.set_connect_status(2)
		try:
			ip = SkyEyeRemoteGdbGetClientIp(self.target_cpu)
		except:
			ip = ""
			self.ErrorReport()
		self.ClientIp = ip
		self.toppanel.set_client_ip(ip)
		ipstr = "已连接  来自: %s" % (ip) 
		self.midpanel.update(ipstr)

	def check_link(self):
		logstr = "初始化完成"
		wx.CallAfter(self.midpanel.update,logstr)
		while 1:
			if self.target_cpu != None and self.ClientIp == None:
				try:
					ret = SkyEyeRemoteGdbCheckLink(self.target_cpu)
				except:
					ret = False
					self.ErrorReport()
				if ret == True:
					wx.CallAfter(self.connect_func)
				
			if self.CloseFlag == True:
				return 
			time.sleep(0.01)



	def on_close_button(self, event):
		dlg = wx.MessageDialog(None, "是否确定退出远程调试? ", "提示", wx.YES_NO)
		retcode = dlg.ShowModal()
		if(retcode == wx.ID_YES):
			target = self.toppanel.get_remote_target()
			self.delete_gdb_server(target)
			self.CloseFlag = True
			self.CheckLinkThread.join()
			self.Destroy()
		else:
			pass
	def on_start_button(self, event):
		if self.StopButton.GetValue() == False:
			self.StartButton.SetToggle(True)
			return 
		self.StopButton.SetToggle(False)
		self.toppanel.enable_target_chose(False)
		target = self.toppanel.get_remote_target()
		ip = self.toppanel.get_remote_ip()
		port = self.toppanel.get_remote_port()
		self.create_gdb_server(target, port, ip)

	def create_gdb_server(self, target, port, ip):
		list = []
		logstr0 = "正在创建调试器..."
		logstr1 = "CPU:%s" % (target)
		logstr2 = "IP:%s, PORT:%s" % (ip, port) 
		list.append(logstr0)
		list.append(logstr1)
		list.append(logstr2)
		self.midpanel.update_list(list)
		try:
			ret = SkyEyeCreateRemoteGdb(target, port , ip)
		except:
			ret = False
			self.ErrorReport()
		if ret == True:
			list = ["创建成功", "等待连接..."]
			self.midpanel.update_list(list)
			self.toppanel.set_connect_status(1)
			self.target_cpu = target
		elif ret == False:
			self.midpanel.update("创建失败")


	def delete_gdb_server(self, target):
		self.target_cpu = None
		self.ClientIp = None
		try:
			ret = SkyEyeDeleteRemoteGdb(target)
		except:
			ret = False
			self.ErrorReport()
		if ret == True:
			self.midpanel.update("连接已关闭")
			self.toppanel.set_connect_status(0)
			self.toppanel.set_client_ip("")
		elif ret == False:
			self.midpanel.update("连接关闭失败")
		
		


	def on_stop_button(self, event):
		if self.StartButton.GetValue() == False:
			self.StopButton.SetToggle(True)
			return 

		target = self.toppanel.get_remote_target()

		self.StartButton.SetToggle(False)
		self.toppanel.enable_target_chose(True)
		self.delete_gdb_server(target)


	def ErrorReport(self):
		if self.par != None:
			self.par.ErrorReport()

		

	
if __name__ == '__main__':
	app = wx.PySimpleApp()
	dialog = RemoteGdbDialog(parent = None)
	ret = dialog.ShowModal()
	app.MainLoop()
