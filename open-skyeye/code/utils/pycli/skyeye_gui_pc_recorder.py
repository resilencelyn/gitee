#coding:utf-8
import wx
import se_path
import conf
from skyeye_common_module import *


class PcRecorder(wx.Frame):
	window_name = 'pc_recorder'
	def __init__(self, parent):
		wx.Frame.__init__(self, parent, wx.ID_ANY, "指令流记录工具", style=wx.DEFAULT_FRAME_STYLE)
		self.par = parent;
		self.initialize()
		self.Bind(wx.EVT_CLOSE, self.CloseSelf, self)

	def initialize(self):
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)

		panel = wx.Panel(self)
		vbox = wx.BoxSizer(wx.VERTICAL)
		hbox1 = wx.BoxSizer(wx.HORIZONTAL)
		st1 = wx.StaticText(panel,label='设置缓存大小:')
        
		hbox1.Add(st1,flag=wx.RIGHT,border=8)
		self.tc = wx.TextCtrl(panel, -1, "最大缓存 2000")
		hbox1.Add(self.tc,proportion=1)
		vbox.Add(hbox1,flag=wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP,border=10)

		hbox1.Add((10, -1))
		self.btn1 = wx.Button(panel, label='设置')
		self.Bind(wx.EVT_BUTTON, self.CacheSet, self.btn1)
		hbox1.Add(self.btn1)
		vbox.Add((-1,10))

		hbox3 = wx.BoxSizer(wx.HORIZONTAL)
		self.list_ctrl = wx.ListCtrl(panel, size=(-1, 500), style=wx.LC_REPORT | wx.BORDER_SUNKEN)
		self.list_ctrl.InsertColumn(0, 'index')
		self.list_ctrl.InsertColumn(1, 'pc')
		hbox3.Add(self.list_ctrl, proportion=1, flag=wx.EXPAND)
		vbox.Add(hbox3, proportion=1, flag=wx.LEFT|wx.RIGHT|wx.EXPAND, border=10)

		vbox.Add((-1,10))

		hbox4 = wx.BoxSizer(wx.HORIZONTAL)
		self.btn2 = wx.Button(panel, label="上一页")
		self.Bind(wx.EVT_BUTTON, self.PageUp, self.btn2)
		self.btn2.Disable()
		hbox4.Add(self.btn2)
		self.btn3 = wx.Button(panel, label='下一页')
		self.Bind(wx.EVT_BUTTON, self.PageDown, self.btn3)
		hbox4.Add(self.btn3)
		vbox.Add(hbox4, flag=wx.ALIGN_CENTER|wx.RIGHT, border=10)

		vbox.Add((-1,20))

		panel.SetSizer(vbox)

		self.DoAction()
	def CloseSelf(self,evt):
		self.par.pc_recorder_frame = None
		self.Destroy()

	def DoAction(self):
		self.ls = []
		self.current_page_num = 0
		self.current_index = 0
		self.end_index = 0
		self.total_page_num = 0
		self.remain_item = 0
		self.total_len = 0

		if self.test_instr_mod() is True:
			self.RecordRefurbish()
		else:
			self.tc.Disable()
			self.btn1.Disable()
			self.btn3.Disable()
	
	def RecordRefurbish(self):
		self.get_pc_records(self.dev)
		self.total_len = len(self.ls)
		self.total_page_num = len(self.ls) // 100
		self.remain_item = len(self.ls) % 100
		if self.remain_item != 0:
			self.total_page_num += 1
		self.PageDown(None)

	def PageUp(self, event):
		self.list_ctrl.DeleteAllItems()
		if self.end_index == self.total_len:
			self.end_index = self.end_index - self.remain_item
			if self.remain_item == 0:
				self.end_index = self.end_index - 100
			self.current_index = self.end_index - 100
		else:
			self.end_index -= 100
			self.current_index = self.end_index - 100

		for n in range(self.current_index, self.end_index):
			self.list_ctrl.InsertItem(n - self.current_index, str(n))
			self.list_ctrl.SetItem(n - self.current_index, 1, "0x%08x" % self.ls[n])
		self.current_page_num -= 1 
		if self.current_page_num == 1:
			self.btn2.Disable()
		if self.current_page_num < self.total_page_num:
			self.btn3.Enable()
			return

	def test_instr_mod(self):
		try:
			config = conf.GetGlobalConfig()
			for mach in config.get_mach_list():
				for cpu in config.get_cpuname_list_by_mach(mach):
					ret = SkyEyeGetInstrProcessDevice(mach, cpu)
					if ret != None:
						self.dev = ret
						return True
		except:
			return False
		return False

	def PageDown(self, event):
		if self.total_len == 0:
			self.btn3.Disable()
			return
		self.list_ctrl.DeleteAllItems()
		self.end_index = self.end_index + 100
		if self.end_index >= self.total_len:
			self.end_index = self.total_len
			self.current_index = self.end_index - self.remain_item
			if self.remain_item == 0:
				self.current_index = self.end_index - 100
		else:
			self.current_index = self.end_index - 100
		for n in range(self.current_index, self.end_index):
			self.list_ctrl.InsertItem(n - self.current_index, str(n))
			self.list_ctrl.SetItem(n - self.current_index, 1, '0x%08x' % self.ls[n])
		self.current_page_num = (self.end_index + 100 - 1) // 100
		if self.current_page_num > 1 and self.end_index > 100:
			self.btn2.Enable()
		if self.current_page_num == self.total_page_num:
			self.btn3.Disable()
	
	def get_pc_records(self, instr_process):
		self.ls = []
		try:
			size = SkyEyeGetPcRecordSize(instr_process)
			index = SkyEyeGetPcRecordIndex(instr_process)
			overflow = SkyEyeGetPcRecordOverflow(instr_process)
			for i in range(index - 1, -1, -1):
				val = SkyEyeGetPcByIndex(instr_process, i)
				self.ls.append(val)

			if overflow == 1:
				for i in range(size - 1, index - 1, -1):
					val = SkyEyeGetPcByIndex(instr_process, i)
					self.ls.append(val)
		except:
			pass
	
	def CacheSet(self, event):
		val = self.tc.GetValue()
		try:
			v = int(val)
			if v > 2000:
				v = 2000
			if v <= 0:
				self.tc.SetValue("Input Error")
				return
			self.tc.SetValue("Success reopen this window")
			SkyEyeSetPcRecordSize(self.dev, v)
		except:
			self.tc.SetValue("Input Error")
