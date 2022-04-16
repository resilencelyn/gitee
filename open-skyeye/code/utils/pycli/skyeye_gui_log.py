# -*- coding: UTF-8 -*-
import wx
import threading
from stat import *
import se_path
import re


LogFilePath = se_path.RunLogPath

fname =  'Segoe UI'
MaxShowNum = 1000

def parse_line(line):
	dic = {}
	match = re.findall("\[(.*?)\]", line)
	if len(match) == 0:
		return dic
	dic["type"] = match[0]
	dic["time"] = match[1]
	dic["func"] = (re.findall("FUNCTION:(.*$)",match[2]))[0]
	dic["log"] = (re.findall("LOG:(.*$)", line))[0]
	return dic

def get_log_from_file():
	with open(LogFilePath, 'r') as fp:
		lines = fp.readlines()
	return lines

class GetLog(threading.Thread):
	def __init__(self, win):
		threading.Thread.__init__(self)
		self.thread_stop = False
		self.win = win
		self.log_list = []
		self.log_num = 0
		self.setDaemon(True)
	def __update_view__(self):
		self.win.log_to_view(self.log_list)
	def run(self):
		self.log_list = get_log_from_file()
		num = len(self.log_list)
		if num >= MaxShowNum:
			self.log_list = self.log_list[MaxShowNum:]
		self.__update_view__()

class LogDialog(wx.Frame):
	def __init__(self, parent):
		wx.Frame.__init__(self, parent,  wx.ID_ANY, title = "系统日志", size = (780, 600))
		self.SetBackgroundColour("#D8D8BF")
		self.par = parent
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)

		self.log_ctrl = wx.ListCtrl(self, wx.ID_ANY, style=wx.LC_REPORT | wx.SIMPLE_BORDER | wx.LC_HRULES | wx.LC_VRULES)	
		self.sizer = wx.BoxSizer(wx.VERTICAL)
		self.log_ctrl.InsertColumn(0, "类型")
		self.log_ctrl.InsertColumn(1, "时间")
		self.log_ctrl.InsertColumn(2, "输出")
		self.log_ctrl.SetColumnWidth(0, 80)
		self.log_ctrl.SetColumnWidth(1, 180)
		self.log_ctrl.SetColumnWidth(2, 500)
		self.log_ctrl.SetFont(wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName = fname))

		self.update_button = wx.Button(self, label = "刷新")
		self.Bind(wx.EVT_BUTTON, self.on_update, self.update_button)
		self.up_button = wx.Button(self, label = "<")
		self.up_button.Disable()
		self.Bind(wx.EVT_BUTTON, self.on_up, self.up_button)
		self.down_button = wx.Button(self, label = ">")
		self.Bind(wx.EVT_BUTTON, self.on_down, self.down_button)
		self.page_index= wx.StaticText(self, wx.ID_ANY, "0/0页", style = wx.ALIGN_RIGHT )
		self.page_index.SetFont(wx.Font(11, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName = fname))
		top_sizer = wx.BoxSizer(wx.HORIZONTAL)
		top_sizer.Add(self.update_button, 0 , flag = wx.EXPAND|wx.RIGHT, border = 10)
		top_sizer.Add(self.up_button, 0 ,flag = wx.EXPAND|wx.RIGHT, border = 10)
		top_sizer.Add(self.down_button, 0 , flag = wx.EXPAND|wx.RIGHT, border = 10)
		top_sizer.Add(self.page_index, 1 , flag = wx.EXPAND|wx.ALIGN_RIGHT, border = 10)
		top_sizer.Add((20, 0), 0 , flag = wx.EXPAND|wx.RIGHT, border = 10)

		self.sizer.Add(top_sizer, 0, wx.EXPAND | wx.ALL, border = 10)
		self.sizer.Add(self.log_ctrl, 1, flag = wx.EXPAND|wx.LEFT|wx.RIGHT|wx.BOTTOM, border = 10)
		self.SetSizer(self.sizer)

		self.line_height, self.head_height = self.find_height()
		self.lines = self.log_ctrl.GetClientSize()[1] // self.line_height
		get_log = GetLog(self)
		get_log.start()
		self.Bind(wx.EVT_SIZE, self.resize)
		self.Bind(wx.EVT_CLOSE, self.CloseSelf, self)
		self.logs = []
		self.pages = []
		self.now_page = 0


	def on_up(self, event):
		self.now_page += 1
		self.fill_one_page(self.now_page)
		if self.now_page >= len(self.pages) -1:
			self.up_button.Disable()
		if self.now_page  != 0:
			self.down_button.Enable()
	def on_down(self, event):
		self.now_page -= 1
		self.fill_one_page(self.now_page)
		if self.now_page  == 0:
			self.down_button.Disable()
		if self.now_page < len(self.pages) - 1:
			self.up_button.Enable()
	def on_update(self, event):
		get_log = GetLog(self)
		get_log.start()
	def resize(self, event):
		self.Layout()
		self.line_height, self.head_height = self.find_height()
		self.lines = self.log_ctrl.GetClientSize()[1] // self.line_height
		width = self.log_ctrl.GetClientSize()[0] - 80 - 180
		self.log_ctrl.SetColumnWidth(2, width)
		self.update_view()
	def fill_one_page(self, page_num):
		self.log_ctrl.DeleteAllItems()
		for index, log in enumerate(self.pages[page_num]):
			info = parse_line(log)
			if "type" in info and "time" in info and "log" in info:
				self.log_ctrl.InsertItem(index, info["type"])
				self.log_ctrl.SetItem(index, 1, info["time"])
				self.log_ctrl.SetItem(index, 2, info["log"])
				if info['type'] == 'ERROR':
					self.log_ctrl.SetItemBackgroundColour(index, (172, 86, 0))
		self.page_index.SetLabel("%d/%d页" % (page_num+1, len(self.pages)))
		self.Layout()

	def logs_to_pages(self, logs):
		self.pages = []
		page_head = 0
		page_num = len(logs) // self.lines
		for i in range(0, page_num):
			page_tail = page_head + self.lines
			self.pages.append(logs[page_head:page_tail])
			page_head = page_tail
		self.pages.append(logs[page_head:])
		self.pages.reverse()
		self.now_page = 0
		le = len(logs) % self.lines
		if le > 0:
			page_num += 1
		if page_num <= 1:
			self.up_button.Disable()
		else:
			self.up_button.Enable()
		self.down_button.Disable()
		
		
	def update_view(self):
		self.logs_to_pages(self.logs)
		self.fill_one_page(self.now_page)

	def log_to_view(self, log_list):
		self.logs = log_list
		self.pages = []
		self.update_view()

	def find_height(self):
		lst = wx.ListCtrl(self, wx.ID_ANY, style=wx.LC_REPORT | wx.SIMPLE_BORDER | wx.LC_HRULES | wx.LC_VRULES)
		lst.SetFont(wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName = fname))
		lst.InsertColumn(0, ('address'))
		lst.InsertItem(0, ('text'))
		x, head, x, height = lst.GetItemRect(0, wx.LIST_RECT_LABEL)
		lst.Destroy()
		return (height, head)
	def CloseSelf(self, event):
		self.par.log_frame = None
		self.Destroy()


	
if __name__ == '__main__':
	app = wx.PySimpleApp()
	dialog = LogDialog(parent = None)
	ret = dialog.Show()
	app.MainLoop()
