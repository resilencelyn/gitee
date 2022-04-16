# -*- coding: utf-8 -*-
		

import wx
import wx.lib.buttons as buttons  
from skyeye_common_module import *
import time
import tools
import copy
import conf
import regs
import json
import se_path
import operator


def get_dev_list():
	config = conf.GetGlobalConfig()	
	if config:
		maped_devices = config.get_maped_devices()
		names =  [dev['name'] for dev in maped_devices]
		return names
	else:
		return []
def get_reg_list(dev):
	reglist = regs.get_registers(dev)
	if reglist == None:
		return []
	reg_names = [reg["name"] for reg in reglist]
	return reg_names

def get_reg(dev, regname):
	reglist = regs.get_registers(dev)
	if reglist == None:
		return None
	for reg in reglist:
		if reg["name"] == regname:
			return reg
	return None

def get_reg_info(dev, reg):
	#"reg1":{"length":"16", "offset": "0", "value":"0x0"},
	if dev == "" and reg == "":
		return None
	reglist = regs.get_registers(dev)
	if reglist == None:
		return None
	reg_dic = None
	for reg_info in reglist:
		if reg_info["name"] == reg:
			reg_dic = reg_info
			break
	return reg_dic


class button_conf():
	def __init__(self, conf):
		self.conf = conf
		self.button = None
		self.value = 0x0
		self.name = self.conf[0]["value"]
		self.dev = self.conf[3]["value"]
		self.reg = self.conf[4]["value"]
		self.offset = self.conf[5]["value"]
		self.length = self.conf[6]["value"]
		self.reg_id = self.conf[9]["value"]
		self.update_value = None

	def get_reg_value(self):
		return regs.get_register(self.dev, int(self.reg_id,10))

	def set_reg_value(self, value):
		offset = int(self.offset, 10)
		length = int(self.length, 10)
		SkyEyeWriteDevice4(self.dev, offset, value)

	def callback(self, d_p):
		bit = int(self.conf[9]["value"], 10)
		offset = int(self.conf[5]["value"], 10)
		value = self.get_reg_value()
		if(d_p):
			value |= (1 << bit)
		else:
			value &= ~(1 << bit)

		self.set_reg_value(value)
		if self.update_value:
			self.update_value(self.dev, self.reg, value)

class checkbox(wx.CheckBox):
	def __init__(self, parent, conf):
		wx.CheckBox.__init__(self, parent, wx.ID_ANY, size = (20, 20))
		self.conf = conf 
		self.col = self.conf.button.GetBackgroundColour()
		self.del_col = "#95955f"
	def chosed(self):
		self.conf.button.SetBackgroundColour(self.del_col)
		self.conf.button.Refresh()
	def unchosed(self):
		self.conf.button.SetBackgroundColour(self.col)
		self.conf.button.Refresh()
			

class layout_dialog(wx.Dialog):
	def __init__(self, parent, layout):
		wx.Dialog.__init__(self, parent, wx.ID_ANY, '布局', style=wx.DEFAULT_DIALOG_STYLE)
		self.layout = layout
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.sw = wx.ScrolledWindow(self, -1, style = wx.BORDER_SUNKEN)
		self.sw.EnableScrolling(False, True)
		self.sw.SetScrollRate(0, 20)
		self.sw.SetBackgroundColour("#ece9d8")
		self.sizer1 = wx.GridBagSizer(hgap=5, vgap=5)

		column =  wx.StaticText(self.sw, -1, label = "列数:")
		cols = ["%d" % col for col in range(1, 100)]
		self.column_choice =  wx.Choice(self.sw, -1, choices = cols)
		self.column_choice.SetSelection(self.layout["column"] - 1)
	

		inters = ["%d" % inter for inter in range(1, 100)]
		interval =  wx.StaticText(self.sw, -1, label = "按钮间隔:")
		self.interval_choice =  wx.Choice(self.sw, -1, choices = inters)
		self.interval_choice.SetSelection(self.layout["interval"] - 1)
		ctrls = [
			[column, self.column_choice],
			[interval, self.interval_choice]
		]
		self.sizer1.Add((10,10), pos = (0, 0),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		for index , ctrl in enumerate(ctrls):
			r = index + 1
			self.sizer1.Add((0,0), pos = (r, 0),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			self.sizer1.Add(ctrl[0], pos = (r, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			self.sizer1.Add(ctrl[1], pos = (r, 2),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			self.sizer1.Add((20, 20), pos = (r, 3),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)

		self.sw.SetSizer(self.sizer1)

		self.del_button = wx.Button(self, label = '确定')
		self.Bind(wx.EVT_BUTTON,self.ok_button_event, self.del_button)

		sizer.Add(self.sw, 6, flag = wx.EXPAND| wx.RIGHT|wx.LIGHT|wx.TOP, border = 20)
		sizer.Add(self.del_button, 1, flag = wx.RIGHT|wx.LIGHT|wx.TOP|wx.BOTTOM|wx.ALIGN_RIGHT, border = 10)
		self.SetSizer(sizer)
		self.Fit()

	def ok_button_event(self, event):
		col = self.column_choice.GetString(self.column_choice.GetSelection())
		inter = self.interval_choice.GetString(self.interval_choice.GetSelection())
		cur_col = int(col, 10)
		cur_inter = int(inter, 10)
		if(cur_col == self.layout["column"] and cur_inter == self.layout["interval"]):
			self.EndModal(0)
		else:
			self.layout["column"] = cur_col
			self.layout["interval"] = cur_inter
			self.EndModal(1)

class del_button_dialog(wx.Dialog):
	def __init__(self, parent, confs):
		wx.Dialog.__init__(self, parent, wx.ID_ANY, '删除', style=wx.DEFAULT_DIALOG_STYLE)
		self.confs = confs
		self.ctrl_list = []
		self.del_list = []
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.sw = wx.ScrolledWindow(self, -1, style = wx.BORDER_SUNKEN)
		self.sw.EnableScrolling(False, True)
		self.sw.SetScrollRate(0, 20)
		#self.sw.SetSizeHints(-1, self.sw.GetCharHeight() * 20)
		self.sw.SetBackgroundColour("#ece9d8")
		self.sizer1 = wx.GridBagSizer(hgap=5, vgap=5)
		self.create_bt_items(self.confs)
		self.sw.SetSizer(self.sizer1)

		self.del_button = wx.Button(self, label = '删除')
		self.Bind(wx.EVT_BUTTON,self.del_button_event, self.del_button)
		sizer.Add(self.sw, 6, flag = wx.EXPAND| wx.RIGHT|wx.LIGHT|wx.TOP, border = 20)
		sizer.Add(self.del_button, 1, flag = wx.RIGHT|wx.LIGHT|wx.TOP|wx.BOTTOM|wx.ALIGN_RIGHT, border = 10)
		sizer.Fit(self)

		self.SetSizer(sizer)


	def get_del_list(self):
		return [ check.conf for check in self.del_list]
			

	def del_button_event(self,event):
		self.EndModal(1)
	def checkbox_event(self,event):
		obj = event.GetEventObject()
		if obj.IsChecked():
			self.del_list.append(obj)
			obj.chosed()
		else:
			self.del_list.remove(obj)
			obj.unchosed()
		if len(self.del_list) > 0:
			self.del_button.Enable()
		else:
			self.del_button.Disable()


	def create_bt_items(self,confs):
		self.sizer1.Add((10,10), pos = (0, 0),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		for itr,conf in enumerate(confs):
			index = itr + 1
			check = checkbox(self.sw, conf)
			self.Bind(wx.EVT_CHECKBOX, self.checkbox_event, check)
			bt_ctrl = wx.TextCtrl(self.sw, -1, style = wx.BORDER_SUNKEN |wx.TE_RICH)
			bt_ctrl.SetBackgroundColour("#ece9d8")
			bt_ctrl.SetValue(conf.conf[0]["value"])
			self.sizer1.Add((0,0), pos = (index, 0),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			self.sizer1.Add(check, pos = (index, 1),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			self.sizer1.Add(bt_ctrl, pos = (index, 2),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			self.sizer1.Add((20, 20), pos = (index, 3),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			self.ctrl_list.append(check)

class add_button_dialog(wx.Dialog):
	button_index = 1
	def __init__(self, parent):
		wx.Dialog.__init__(self, parent, wx.ID_ANY, '添加', style=wx.DEFAULT_DIALOG_STYLE)
		self.par = parent
		self.SetBackgroundColour("#ece9d8")

		sizer = wx.GridBagSizer(hgap=5, vgap=5)
		name_s = 'Button%d' % self.__class__.button_index
		self.__class__.button_index +=1
	

		name = wx.StaticText(self, -1, '名称')
		self.name_ctrl = wx.TextCtrl(self, -1, style = wx.BORDER_SUNKEN |wx.TE_RICH)
		self.name_ctrl.SetValue(name_s)

		label = wx.StaticText(self, -1, '标签')
		self.label_ctrl = wx.TextCtrl(self, -1, style = wx.BORDER_SUNKEN |wx.TE_RICH)

		button_type = wx.StaticText(self, -1, '按钮类型')
		self.type_choice = wx.Choice(self, wx.ID_ANY, choices=["Normal", "Switch"])
		self.type_choice.SetSelection(0)

		dev = wx.StaticText(self, -1, '设备')
		self.dev_choice = wx.Choice(self, wx.ID_ANY, choices=get_dev_list())
		self.Bind(wx.EVT_CHOICE, self.dev_choice_event, self.dev_choice)

		reg = wx.StaticText(self, -1, '寄存器')
		self.reg_choice = wx.Choice(self, wx.ID_ANY, choices=[])
		self.Bind(wx.EVT_CHOICE, self.reg_choice_event, self.reg_choice)

		offset = wx.StaticText(self, -1, '偏移(Byte)')
		self.offset_ctrl = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.offset_ctrl.SetBackgroundColour("#ece9d8")

		self.offset_ctrl.SetBackgroundColour("#ece9d8")

		length = wx.StaticText(self, -1, '长度(Bit)')
		self.length_ctrl = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.length_ctrl.SetBackgroundColour("#ece9d8")
		val = wx.StaticText(self, -1, '值')
		self.val_ctrl = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.val_ctrl.SetBackgroundColour("#ece9d8")

		method = wx.StaticText(self, -1, '方式')
		self.method_choice = wx.Choice(self, wx.ID_ANY, choices=["Bit", "Register"])
		self.Bind(wx.EVT_CHOICE, self.method_choice_event, self.method_choice)

		self.bit_des =  '将寄存器的某一位连接到将要添加的按钮。'
		self.register_des =  '将寄存器的每一位分别连接到一个按钮。'
		self.method_choice.SetSelection(0)
		self.method_des = wx.StaticText(self, -1, "")
		self.method_des.SetLabel(self.bit_des)

		bit = wx.StaticText(self, -1, '位')
		self.bit_choice = wx.Choice(self, wx.ID_ANY, choices=[])
		self.Bind(wx.EVT_CHOICE, self.bit_choice_event, self.bit_choice)
		self.ok_button = wx.Button(self, label = '添加')
		self.ok_button.Disable()
	
		self.Bind(wx.EVT_BUTTON, self.ok_button_event, self.ok_button)
		self.kv_ctrls = [
			[(10, 10), (10, 10), wx.EXPAND,0],
			[name, self.name_ctrl, wx.EXPAND,1],
			[label, self.label_ctrl, wx.EXPAND,1],
			[button_type, self.type_choice, wx.EXPAND,2],
			[dev, self.dev_choice, wx.EXPAND,2],
			[reg, self.reg_choice, wx.EXPAND,2] ,
			[offset, self.offset_ctrl, wx.EXPAND,1],
			[length, self.length_ctrl, wx.EXPAND,1],
			[val, self.val_ctrl, wx.EXPAND,1],
			[method, self.method_choice, wx.EXPAND,2],
			[(10, 10), self.method_des, wx.EXPAND,0],
			[bit, self.bit_choice, wx.EXPAND,2],
			[(10, 10), self.ok_button, 0,0],
			[(10, 10), (10, 10), 0,0]
			]

		
		for index, kv in enumerate(self.kv_ctrls):
			sizer.Add(kv[0], pos = (index, 0),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			sizer.Add(kv[1], pos = (index, 1),flag = kv[2]|wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			sizer.Add((10, 10), pos = (index, 2),flag = kv[2]|wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		self.SetSizer(sizer)
		self.Fit()
	def bit_choice_event(self, event):
		self.button_op()

	def method_choice_event(self, event):
		method = self.method_choice.GetString(self.method_choice.GetSelection())
		if(method == 'Bit'):
			self.method_des.SetLabel(self.bit_des)
			self.Refresh()
			self.label_ctrl.Enable()
			self.bit_choice.Enable()
		else:
			self.method_des.SetLabel(self.register_des)
			self.Refresh()
			self.label_ctrl.SetValue("")
			self.label_ctrl.Disable()
			self.bit_choice.Disable()
			self.bit_choice.SetItems([])
			reg = self.reg_choice.GetString(self.reg_choice.GetSelection())
			dev = self.dev_choice.GetString(self.dev_choice.GetSelection())
			ret =  get_reg_info(dev, reg)
			if ret==None:
				return 
			byte=ret["size"]
			if(byte):
				bits = ["%d" % bit for bit in range(0, byte*8)]
				self.bit_choice.SetItems(bits)
		self.button_op()
	def button_op(self):
		if self.is_valid():
			self.ok_button.Enable()
		else:
			self.ok_button.Disable()

	def is_valid(self):
		if self.dev_choice.GetSelection() < 0:
			return False
		if self.reg_choice.GetSelection() < 0:
			return False
		method = self.method_choice.GetString(self.method_choice.GetSelection())
		if method == 'Bit':
			if self.bit_choice.GetSelection() < 0:
				return False
		return True

	def dev_choice_event(self, event):
		dev = self.dev_choice.GetString(self.dev_choice.GetSelection())
		reg_list = get_reg_list(dev)
		if len(reg_list) == 0:
			self.reg_choice.SetItems([])
			self.length_ctrl.SetValue('')
			self.val_ctrl.SetValue('')
			self.offset_ctrl.SetValue('')
			self.bit_choice.SetItems([])
			self.button_op()
			return 
		self.reg_choice.SetItems(get_reg_list(dev))
		self.reg_choice.SetSelection(0)
		self.reg_choice_event(0)
		reg = self.reg_choice.GetString(self.reg_choice.GetSelection())
		byte =  get_reg_info(dev, reg)["size"]
		self.length_ctrl.SetValue("%d" %  (byte* 8))
		self.offset_ctrl.SetValue("%d" % get_reg_info(dev, reg)["offset"])
		self.val_ctrl.SetValue("%d" % get_reg_info(dev, reg)["value"])
		self.button_op()

	def reg_choice_event(self, event):
		dev = self.dev_choice.GetString(self.dev_choice.GetSelection())
		reg = self.reg_choice.GetString(self.reg_choice.GetSelection())
		info =  get_reg_info(dev, reg)
		if info:
			byte =  get_reg_info(dev, reg)["size"]
			self.length_ctrl.SetValue("%d" % ( byte* 8))
			self.offset_ctrl.SetValue("%d" % (get_reg_info(dev, reg)["offset"]))
			self.val_ctrl.SetValue("%d" % (get_reg_info(dev, reg)["value"]))
			byte =  get_reg_info(dev, reg)[ "size"]
			bits = ["%d" % bit for bit in range(0, byte * 8)]
			self.bit_choice.SetItems(bits)
			self.button_op()

	def get_confs(self):
		confs = []
		conf = []
		for index, kv in enumerate(self.kv_ctrls):
			if isinstance(kv[0], tuple):
				continue
			ctr_dic = {}
			ctr_dic["name"] = self.kv_ctrls[index][0].GetLabel()
			if self.kv_ctrls[index][3]==1:
				ctr_dic["value"] = self.kv_ctrls[index][1].GetValue()
			else:
				ctr_dic["value"] = self.kv_ctrls[index][1].GetStringSelection()
			conf.append(ctr_dic)
		reg = get_reg(conf[3]["value"], conf[4]["value"])
		if reg:
			id_dic = {}
			id_dic["name"] = 'id'
			id_dic["value"] = reg["id"]
			conf.append(id_dic)

		if conf[8]["value"] == 'Register':
			reg_len = int(conf[6]["value"], 10)
			self.__class__.button_index -= 1
			for bit in range(0, reg_len):
				conf_new = copy.deepcopy(conf)
				conf_new[0]["value"] = "Button%d" % (self.__class__.button_index)
				conf_new[9]["value"] = "%d" % bit
				self.__class__.button_index += 1
				confs.append(conf_new)
		else:
			confs.append(conf)
		return confs

	def ok_button_event(self, event):
		self.EndModal(1)

class gpiobutton(buttons.GenButton):
	def __init__(self, parent, conf):
		buttons.GenButton.__init__(self, parent, wx.ID_ANY, size = (60, 60))
		self.conf = conf 

class toggle_gpiobutton(buttons.GenToggleButton):
	def __init__(self, parent, conf):
		buttons.GenToggleButton.__init__(self, parent, wx.ID_ANY, size = (60, 60))
		self.conf = conf 


class gpioPanel(wx.ScrolledWindow):
	def __init__(self, parent, layout):
		self.par = parent
		self.layout = layout
		wx.ScrolledWindow.__init__(self, parent, style = wx.BORDER_SUNKEN)
		self.SetBackgroundColour("#545454")
		self.EnableScrolling(True, True)
		self.SetScrollRate(20, 20)
		self.button_list = []
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.sizer_b = wx.GridBagSizer(hgap=5, vgap=5)
		sizer.Add(self.sizer_b, 1, flag = wx.EXPAND | wx.RIGHT | wx.LEFT|wx.TOP|wx.BOTTOM, border = 20)
		self.SetSizer(sizer)

	def add_buttons(self, confs):
		for bt_conf in confs:
			if operator.eq(bt_conf.conf[2]["value"], 'Normal') == True:
				b = gpiobutton(self, bt_conf)
				self.Bind(wx.EVT_BUTTON, self.button_event_up, b)
				b.Bind(wx.EVT_LEFT_DOWN, self.button_event_down)
			else:
				b = toggle_gpiobutton(self, bt_conf)
				self.Bind(wx.EVT_BUTTON, self.button_toggle_event, b)
			b.SetBezelWidth(3)
			b.SetLabel(bt_conf.conf[1]["value"])
			self.button_list.append(b)
			bt_conf.button = b
		self.update_buttons_view()

	def del_buttons(self, confs):
		for conf in confs:
			self.button_list.remove(conf.button)
			conf.button.Destroy()
		self.update_buttons_view()

	def update_buttons_view(self):
		self.sizer_b.Clear()
		for index, button in enumerate(self.button_list):
			r = index / self.layout["column"]
			l = index % self.layout["column"]
			self.sizer_b.Add(button, pos = (r, l),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_RIGHT, border= self.layout["interval"]/2)
		self.sizer_b.Layout()
		self.Refresh()


	def button_toggle_event(self, event):
		obj = event.GetEventObject()
		if obj.GetToggle():
			obj.conf.callback(1)
		else:
			obj.conf.callback(0)


	def button_event_up(self, event):
		obj = event.GetEventObject()
		obj.conf.callback(0)
		event.Skip()
	def button_event_down(self, event):
		obj = event.GetEventObject()
		obj.conf.callback(1)
		event.Skip()

class attrPanel(wx.Panel):
	def __init__(self, parent, button_list):
		wx.Panel.__init__(self, parent, style = wx.BORDER_SUNKEN)
		self.par = parent
		self.button_list = button_list

		self.SetBackgroundColour("#ece9d8")

		sizer = wx.GridBagSizer(hgap=5, vgap=5)
		name_s = ""

		name = wx.StaticText(self, -1, '名称')
		self.name_ctrl = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.name_ctrl.SetValue(name_s)
		self.name_ctrl.SetBackgroundColour("#ece9d8")


		label = wx.StaticText(self, -1, '标签')
		self.label_ctrl = wx.TextCtrl(self, -1, style = wx.BORDER_SUNKEN |wx.TE_RICH)
		self.Bind(wx.EVT_TEXT_ENTER, self.label_ctrl_event, self.label_ctrl)

		button_type = wx.StaticText(self, -1, '按钮类型')
		self.type_choice = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.type_choice.SetBackgroundColour("#ece9d8")

		dev = wx.StaticText(self, -1, '设备')
		self.dev_choice = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.dev_choice.SetBackgroundColour("#ece9d8")

		reg = wx.StaticText(self, -1, '寄存器')
		self.reg_choice = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.reg_choice.SetBackgroundColour("#ece9d8")

		offset = wx.StaticText(self, -1, '偏移')
		self.offset_ctrl = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.offset_ctrl.SetBackgroundColour("#ece9d8")

		length = wx.StaticText(self, -1, '长度')
		self.length_ctrl = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.length_ctrl.SetBackgroundColour("#ece9d8")

		val = wx.StaticText(self, -1, '值')
		self.val_ctrl = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.val_ctrl.SetBackgroundColour("#ece9d8")

		method = wx.StaticText(self, -1, '方式')
		self.method_choice = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.method_choice.SetBackgroundColour("#ece9d8")

		bit = wx.StaticText(self, -1, '位')
		self.bit_choice = wx.TextCtrl(self, -1, style = wx.TE_READONLY|wx.BORDER_SUNKEN |wx.TE_RICH)
		self.bit_choice.SetBackgroundColour("#ece9d8")

		self.kv_ctrls = [
			[(10, 10), (10, 10), wx.EXPAND],
			[name, self.name_ctrl, wx.EXPAND],
			[label, self.label_ctrl, wx.EXPAND],
			[button_type, self.type_choice, wx.EXPAND],
			[dev, self.dev_choice, wx.EXPAND],
			[reg, self.reg_choice, wx.EXPAND] ,
			[offset, self.offset_ctrl, wx.EXPAND],
			[length, self.length_ctrl, wx.EXPAND],
			[val, self.val_ctrl, wx.EXPAND],
			[method, self.method_choice, wx.EXPAND],
			[bit, self.bit_choice, wx.EXPAND],
			[(10, 10), (10, 10), 0]
			]
		self.ctrls = [
			[name, self.name_ctrl, wx.EXPAND],
			[label, self.label_ctrl, wx.EXPAND],
			[button_type, self.type_choice, wx.EXPAND],
			[dev, self.dev_choice, wx.EXPAND],
			[reg, self.reg_choice, wx.EXPAND] ,
			[offset, self.offset_ctrl, wx.EXPAND],
			[length, self.length_ctrl, wx.EXPAND],
			[val, self.val_ctrl, wx.EXPAND],
			[method, self.method_choice, wx.EXPAND],
			[bit, self.bit_choice, wx.EXPAND],
			]


			
		for index, kv in enumerate(self.kv_ctrls):
			sizer.Add(kv[0], pos = (index, 0),flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			sizer.Add(kv[1], pos = (index, 1),flag = kv[2]|wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
			sizer.Add((10, 10), pos = (index, 2),flag = kv[2]|wx.TOP|wx.LEFT|wx.RIGHT|wx.ALIGN_RIGHT, border= 5)
		self.SetSizer(sizer)
		self.Fit()
		self.cur_button_conf = None


	def label_ctrl_event(self, event):
		obj = event.GetEventObject()
		label = obj.GetValue()
		name = self.name_ctrl.GetValue()
		cur_bt_ctrl = None
		for bt_conf in self.button_list:
			if bt_conf.name == name:
				cur_bt_ctrl = bt_conf
		if bt_conf != None:
			bt_conf.conf[1]["value"] = label
			cur_bt_ctrl.button.SetLabel(label)
			cur_bt_ctrl.button.Refresh()
		else:
			obj.SetValue("")

		

	def clear(self):
		for index, ctrl in enumerate(self.ctrls):
			if isinstance(ctrl[0], tuple):
				continue
			ctrl[1].SetValue('')
	
	def update_value(self, dev, reg, value): 
		cur_dev = self.dev_choice.GetValue()
		cur_reg = self.reg_choice.GetValue()
		if dev == cur_dev and reg == cur_reg:
			self.val_ctrl.SetValue("%x" % value)

	def update(self, button_conf):
		for index, conf_dic in enumerate(button_conf.conf):
			if index <= 9:
				self.ctrls[index][1].SetValue(conf_dic["value"])



class general_gpio_button_frame(wx.Frame):
	def __init__(self, parent):
		wx.Frame.__init__(self, parent,title = 'GPIO BUTTON', style=wx.DEFAULT_FRAME_STYLE, size = (600, 520))
		self.par = parent
		self.button_list = []
		self.layout = {"column":4, "interval":10}
	
		font = wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName = 'Segoe UI')
		self.SetFont(font)
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		self.SetBackgroundColour("#ece9d8")
	

		image_del =  wx.ArtProvider.GetBitmap(wx.ART_DELETE, wx.ART_TOOLBAR)
		image_add =  wx.ArtProvider.GetBitmap(wx.ART_NEW, wx.ART_TOOLBAR)
		image_lay =  wx.ArtProvider.GetBitmap(wx.ART_HELP_SETTINGS, wx.ART_TOOLBAR)
		image_save =  wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE, wx.ART_TOOLBAR)
		self.ToolBar = self.CreateToolBar(wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT | wx.TB_TEXT | wx.TB_3DBUTTONS)
		self.ToolBar.SetBackgroundColour("#ece9d8")
		self.ToolBar.SetToolBitmapSize((30, 30)) 
		id_add =  wx.NewId()
		id_del =  wx.NewId()
		id_layout =  wx.NewId()
		id_save =  wx.NewId()

		self.ToolBar.AddTool(id_add, '添加', image_add, wx.NullBitmap, wx.ITEM_NORMAL, '添加')
		self.ToolBar.AddTool(id_del, '删除', image_del, wx.NullBitmap, wx.ITEM_NORMAL, '删除')
		self.ToolBar.AddTool(id_save, '保存', image_save, wx.NullBitmap, wx.ITEM_NORMAL, '保存')
		self.ToolBar.AddTool(id_layout, '布局', image_lay, wx.NullBitmap, wx.ITEM_NORMAL, '布局')

		self.ToolBar.Realize()
		self.Bind(wx.EVT_TOOL, self.add_button, id = id_add)
		self.Bind(wx.EVT_TOOL, self.del_button, id = id_del)
		self.Bind(wx.EVT_TOOL, self.layout_button, id = id_layout)
		self.Bind(wx.EVT_TOOL, self.save_button, id = id_save)

		self.sizer = wx.BoxSizer(wx.HORIZONTAL)

		self.gpio_panel = gpioPanel(self, self.layout)

		self.attr_panel = wx.Panel(self, style = wx.SUNKEN_BORDER)
		self.attr_panel.SetBackgroundColour("#ece9d8")

		attr_sizer = wx.BoxSizer(wx.VERTICAL) 
		self.ap = attrPanel(self.attr_panel, self.button_list)
		self.button_choice = wx.Choice(self.attr_panel, wx.ID_ANY, choices=[], style = wx.SUNKEN_BORDER)
		self.Bind(wx.EVT_CHOICE, self.button_choice_event, self.button_choice)


		attr_sizer.Add((0, 0), 1, flag = wx.LEFT | wx.RIGHT | wx.TOP | wx.BOTTOM, border = 1) 
		attr_sizer.Add(self.button_choice, 1, flag = wx.EXPAND|wx.LEFT | wx.RIGHT | wx.TOP | wx.BOTTOM, border = 10) 
		attr_sizer.Add(self.ap, 14, flag = wx.EXPAND|wx.LEFT | wx.RIGHT | wx.TOP | wx.BOTTOM, border = 10) 
	
		self.attr_panel.SetSizer(attr_sizer)
		self.attr_panel.Fit()

		#self.light_panel = wx.Panel(self, style = wx.SUNKEN_BORDER)
		sizer_l = wx.BoxSizer(wx.VERTICAL)
		sizer_l.Add(self.attr_panel, 1, flag = wx.EXPAND|wx.LEFT | wx.RIGHT | wx.TOP | wx.BOTTOM, border = 0) 
		#sizer_l.Add(self.light_panel, 1, wx.EXPAND)

		self.Bind(wx.EVT_CLOSE, self.CloseSelf, self)

		self.sizer.Add(sizer_l, 0, wx.EXPAND)
		self.sizer.Add(self.gpio_panel, 3, wx.EXPAND)
		wx.CallAfter(self.init_from_conf_file)
		self.SetSizer(self.sizer)
		self.ap.Fit()

	def CloseSelf(self, event):
		self.par.button_frame = None
		self.button_confs_to_confs()
		self.Destroy()


	def init_from_conf_file(self):
		config = conf.GetGlobalConfig()	
		if not config:
			return 
		confs = self.confs_to_button_confs()
		if confs :
			self.gpio_panel_add_buttons(confs)
		
	def button_choice_event(self, event):
		index = self.button_choice.GetSelection()
		self.update_attr_ctrl(self.button_list[index])

	def update_attr_ctrl(self, button_conf):
		self.ap.clear()
		self.ap.update(button_conf)

	def button_confs_to_confs(self):
		confs = [bt_conf.conf for bt_conf in self.button_list]
		json_dic = {}
		json_dic["confs"] = confs
		json_str = json.dumps(json_dic)
		f = open(se_path.ButtonConfPath, 'w')
		f.write(json_str)
		f.close()

	def confs_to_button_confs(self):
		if os.path.exists(se_path.ButtonConfPath) == False:
			return None
		f = open(se_path.ButtonConfPath, 'r')
		json_str = f.read()
		f.close()
		try:
			json_dic = json.loads(json_str)
		except:
			return  None
		confs = json_dic["confs"]
		return confs



	def gpio_panel_add_buttons(self, confs):
		bt_confs = [button_conf(conf) for conf in confs]
		for conf in bt_confs:
			conf.update_value = self.ap.update_value
		self.gpio_panel.add_buttons(bt_confs)
		self.button_list.extend(bt_confs)
		names = [bt.conf[0]["value"] for bt in self.button_list]
		self.button_choice.SetItems(names)
		self.button_choice.SetSelection(0)
		self.update_attr_ctrl(self.button_list[0])

	def gpio_panel_del_buttons(self, confs):
		self.gpio_panel.del_buttons(confs)
		for conf in confs:
			self.button_list.remove(conf)
		names = [bt.conf[0]["value"] for bt in self.button_list]
		self.button_choice.SetItems(names)
		if len(names) > 0:
			self.button_choice.SetSelection(0)
			self.update_attr_ctrl(self.button_list[0])
		else:
			self.ap.clear()
	def gpio_panel_update(self):
		self.gpio_panel.update_buttons_view()
		self.Refresh()

	def save_button(self, event):
		self.button_confs_to_confs()
	def layout_button(self, event):
		dialog = layout_dialog(self, self.layout)
		ret = dialog.ShowModal()
		if ret == 1:
			wx.CallAfter(self.gpio_panel_update)
			

	def add_button(self, event):
		gpio_dialog = add_button_dialog(self)
		ret = gpio_dialog.ShowModal()
		if ret == 1:
			confs = gpio_dialog.get_confs()
			gpio_dialog.Destroy()
			wx.CallAfter(self.gpio_panel_add_buttons, confs)

	def del_button(self, event):
		dialog = del_button_dialog(self, self.button_list)
		ret = dialog.ShowModal()
		if ret == 1:
			del_list = dialog.get_del_list()
			wx.CallAfter(self.gpio_panel_del_buttons, del_list)
			




if __name__ == '__main__':
	app = wx.PySimpleApp()
	frame = general_gpio_button_frame(parent = None)
	frame.Show()
	app.MainLoop()
