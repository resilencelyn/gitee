# -*- coding: UTF-8 -*-

import wx
import os
import _thread
import conf
import regs
import mips
import skyeye_common_module as scm


class CpuRegister(wx.Frame):
	def __init__(self, parent):
		wx.Frame.__init__(self, parent, wx.ID_ANY, title="处理器")
		self.par = parent
		self.SetBackgroundColour(wx.BLACK)
		self.panel = wx.Panel(self)
		self.panel.SetBackgroundColour(wx.WHITE)
		self.sizer1 = wx.BoxSizer(wx.VERTICAL)

		top_sizer = wx.BoxSizer(wx.HORIZONTAL)

		cpu_box = wx.StaticBox(self.panel, -1, "处理器")
		cpu_sizer = wx.StaticBoxSizer(cpu_box, wx.VERTICAL)
		self.speed = wx.TextCtrl(self.panel, -1, style=wx.TE_READONLY | wx.BORDER_SUNKEN | wx.TE_RICH | wx.TE_RIGHT)

		speed_box = wx.StaticBox(self.panel, -1, "指令速度(条/每秒)")
		speed_sizer = wx.StaticBoxSizer(speed_box, wx.VERTICAL)
		speed_sizer.Add(self.speed, 1, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, 5)

		config = conf.GetGlobalConfig()
		cpu_list = config.get_cpu_list()
		self.cpu_choice = wx.Choice(self.panel, wx.ID_ANY, choices=cpu_list)
		self.cpu_choice.SetSelection(0)
		self.Bind(wx.EVT_CHOICE, self.on_cpu_choice, self.cpu_choice)
		self.sw = wx.ScrolledWindow(self.panel, wx.ID_ANY, style=wx.BORDER_DOUBLE | wx.HSCROLL)
		self.sw.EnableScrolling(False, True)
		self.sw.SetScrollRate(0, 20)
		self.sw.SetSizeHints(-1, self.sw.GetCharHeight() * 10)
		self.sw.SetBackgroundColour(wx.WHITE)

		self.sw_sizer = wx.GridBagSizer(hgap=5, vgap=5)
		top_sizer.Add(cpu_sizer, 1, wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, 5)
		top_sizer.Add(speed_sizer, 1, wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, 5)

		cpu_sizer.Add(self.cpu_choice, 1, wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, 5)
		self.sizer1.Add(top_sizer, 1, wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, 10)
		self.sizer1.Add(self.sw, 5, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, 5)

		self.controls = {}
		self.names = {}
		self.last_cpu = None
		self.create_cpu_view()
		self.panel.SetSizerAndFit(self.sizer1)
		self.Bind(wx.EVT_CLOSE, self.CloseSelf, self)
		self.Fit()
		self.SetSize((700, 500))
		_thread.start_new_thread(self.add_thread, ())

	def add_thread(self):
		self.par.register_thread_func(self, self.update_speed)

	def update_speed(self):
		dic = mips.get()
		old_speed = self.speed.GetValue()

		cpu = self.get_cur_cpu()
		if cpu not in dic:
			return
		if dic[cpu] < 0:
			return
		new_speed = dic[cpu]
		if old_speed == new_speed:
			return
		self.speed.SetValue("{:,}".format(new_speed))

	def remove(self):
		self.par.remove_thread_func(self)
		self.par.cpu_registers_frame = None
		self.Destroy()

	def CloseSelf(self, event):
		_thread.start_new_thread(self.remove, ())

	def event_on_cpu_choice(self):
		for key in self.controls.keys():
			self.controls[key].Destroy()
		for key in self.names.keys():
			self.names[key].Destroy()
		self.sw_sizer.Clear()
		self.controls = {}
		self.names = {}
		self.create_cpu_view()
		self.sw_sizer.Layout()
		self.sw.SetSizerAndFit(self.sw_sizer)
		self.panel.SetSizerAndFit(self.sizer1)
		self.Fit()
		self.SetSize((700, 500))

	def on_cpu_choice(self, event):
		cpu = self.get_cur_cpu()
		if cpu == self.last_cpu:
			return
		wx.CallAfter(self.event_on_cpu_choice)

	def RegsRefurbish(self):
		cpu = self.get_cur_cpu()
		register_l = regs.get_registers(cpu)
		for reg in register_l:
			control = self.controls[reg["name"]]
			value = control.GetValue()
			new_value = "%x" % reg["value"]
			if new_value != value:
				control.SetValue(new_value)
				control.SetBackgroundColour("#ffff00")
			else:
				control.SetBackgroundColour(wx.WHITE)

	def get_cur_cpu(self):
		index = self.cpu_choice.GetSelection()
		return self.cpu_choice.GetString(index)

	def create_cpu_view(self):
		cpu = self.get_cur_cpu()
		self.last_cpu = cpu

		register_l = regs.get_registers(cpu)
		register_list = []
		h_list = []
		col_num = 4
		for reg in register_l:
			dic = {}
			dic["name"] = reg["name"]
			dic["value"] = "%x" % (reg["value"])
			dic["id"] = reg["id"]
			register_list.append(dic)
		h_num = len(register_list) // col_num
		l_num = len(register_list) % col_num
		for i in range(0, h_num):
			tmp_list = register_list[i * col_num:i * col_num + col_num]
			h_list.append(tmp_list)
		h_list.append(register_list[h_num * col_num:])
		for i, e in enumerate(h_list):
			for j, e in enumerate(h_list[i]):
				regname = h_list[i][j]["name"]
				regvalue = h_list[i][j]["value"]
				name_ctrl = wx.StaticText(self.sw, -1, regname)
				value_ctrl = wx.TextCtrl(self.sw, -1, style=wx.TE_PROCESS_ENTER | wx.BORDER_SUNKEN | wx.TE_RICH, name=str(h_list[i][j]["id"]))
				self.Bind(wx.EVT_TEXT_ENTER, self.value_change, value_ctrl)
				value_ctrl.SetValue(regvalue)
				self.controls[regname] = value_ctrl
				self.names[regname] = name_ctrl
				self.sw_sizer.Add(name_ctrl, pos=(i, j * 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.ALIGN_RIGHT, border=5)
				self.sw_sizer.Add(value_ctrl, pos=(i, j * 2 + 1), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.ALIGN_RIGHT, border=5)
		self.sw.SetSizerAndFit(self.sw_sizer)

	def value_change(self, evt):
		rid = int(evt.EventObject.GetName())
		config = conf.GetGlobalConfig()
		cpuname = self.get_cur_cpu()
		machname = config.get_cpu_mach(cpuname)
		last_value = scm.SkyEyeGetDevRegValueById(machname, cpuname, rid)
		try:
			value = int(evt.EventObject.GetValue(), 16)
			if value < 0:
				raise
		except:
			evt.EventObject.SetValue("%x" % last_value)
			self.error_report("修改失败！\n请输入正确的十六进制数！")
			return
		ret = scm.SkyEyeSetDevRegValueById(machname, cpuname, value, rid)
		if ret != 1:
			self.error_report("没有找到处理器寄存器接口!")
			evt.EventObject.SetValue("%x" % last_value)
			return
		else:
			pass
		evt.EventObject.SetValue("%x" % value)
		if last_value != value:
			evt.EventObject.SetBackgroundColour("#ffff00")

	def error_report(self, mes):
		wx.MessageBox(mes, "提示", wx.OK | wx.ICON_EXCLAMATION)
