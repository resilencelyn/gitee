# -*- coding: UTF-8 -*-
import os
import wx
import se_path
import skyeye_common_module as scm


gui_view = None
def init_gui_log_view():
	global gui_view
	if gui_view == None:
		gui_view = LogDlg(parent = None)

log_func_list = []

def RegisterLog(func):
	global log_func_list
	log_func_list.append(func)

def LogReportPack(log):
	LogReport(log[0], log[1])

def LogReport(log, model = False):
	global log_func_list
	for func in log_func_list:
		func(log)
	global gui_view
	if gui_view != None:
		gui_view.log_report(log,model)
		gui_view.Show()
	else:
		if model == True:
			scm.SkyEyeQuit()

def err(mesg):
	mesg_str = str(mesg)
	st = "Error:%s" % mesg_str
	return st

fname =  'Segoe UI'

class LogDlg(wx.Dialog):
	def __init__(self, parent):
		wx.Dialog.__init__(self, parent, wx.ID_ANY, "报告", style=wx.DEFAULT_FRAME_STYLE)
		self.parent = parent
		self.flag=False
		self.SetFont(wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName = fname))
		self.SetBackgroundColour(wx.WHITE)
		self.panel = wx.Panel(self)
		self.panel.SetBackgroundColour(wx.WHITE)
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.log_ctrl = wx.TextCtrl(self.panel, style = wx.TE_READONLY|wx.TE_RICH|wx.TE_RICH2|wx.BORDER_SUNKEN|wx.TE_MULTILINE )
		sizer.Add(self.log_ctrl, 1, wx.EXPAND)
		#det_button = wx.Button(self.panel, label = "详细日志")
		#self.Bind(wx.EVT_BUTTON, self.on_det_button, det_button)
		close_button = wx.Button(self.panel, label = "关闭")
		self.Bind(wx.EVT_BUTTON, self.on_close_button, close_button)
		pic = se_path.GetPicPath("log.ico")
		icon = wx.Icon(pic, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		h_sizer = wx.BoxSizer(wx.HORIZONTAL)
		h_sizer.Add((40, 40), 1, wx.EXPAND)
		#h_sizer.Add(det_button, 0, flag = wx.ALIGN_CENTER)
		#h_sizer.Add((20, 20), 0, wx.EXPAND)
		h_sizer.Add(close_button, 0, flag = wx.ALIGN_CENTER)
		h_sizer.Add((10, 10), 0, wx.EXPAND)
		sizer.Add(h_sizer, 0, wx.EXPAND)
		self.panel.SetSizer(sizer)
		self.Fit()
		self.SetSize((500, 200))
		self.Bind(wx.EVT_CLOSE, self.on_close_button, self)
		self.Center()

	def on_det_button(self, event):
		pass

	def on_close_button(self, event):
		self.Hide()
		if self.flag == True:
			scm.SkyEyeQuit()

	def log_report(self, log,flag):
		self.log_ctrl.SetValue(str(log))
		self.flag=flag

