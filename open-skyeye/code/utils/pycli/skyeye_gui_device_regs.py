import wx
import operator
from skyeye_common_module import *
from common_dialog import *
import conf
import se_path
import regs

def is_error(value_str):
	if value_str[:2] in ["0x", "0X"]:
		try: 
			va = int(value_str, 16)
		except:
			return True
	else:
		try:
			va = int(value_str, 10)
		except:
			return True
	if va < 0:
		return True
	return False

def str_to_int(value_str):
	try:
		value = int(value_str, 16)
		return value
	except:
		pass


class ModifyDialog(wx.Dialog):
	def __init__(self, parent, pos, reg_iter):
		wx.Dialog.__init__(self, parent, wx.ID_ANY, "修改", style=wx.DEFAULT_DIALOG_STYLE, pos = pos)
		self.par = parent
		self.reg_info = reg_iter
		self.before_text = wx.TextCtrl(self, -1, style = wx.BORDER_SUNKEN | wx.TE_READONLY)
		self.before_text.SetValue(self.reg_info[2])
		self.static_text = wx.StaticText(self, -1, "修改为")
		self.after_text = wx.TextCtrl(self, -1, style = wx.BORDER_SUNKEN)
		self.Bind(wx.EVT_TEXT, self.on_after_text , self.after_text)
		vsizer = wx.BoxSizer(wx.VERTICAL)
		psizer = wx.BoxSizer(wx.HORIZONTAL)
		psizer.Add(self.before_text, proportion=2, flag=wx.ALL |wx.EXPAND, border = 10)
		psizer.Add(self.static_text, proportion=1, flag=wx.ALL, border = 10)
		psizer.Add(self.after_text, proportion=2, flag=wx.ALL | wx.EXPAND, border = 10)
	
		p1sizer = wx.BoxSizer(wx.HORIZONTAL)
		v2sizer = wx.BoxSizer(wx.HORIZONTAL)
		self.okButton = wx.Button(self, -1, label = "确定")
		v3sizer = wx.BoxSizer(wx.HORIZONTAL)
		p1sizer.Add(v2sizer, proportion = 1)	
		p1sizer.Add(self.okButton, proportion=1)
		p1sizer.Add(v3sizer, proportion = 1)	
		vsizer.Add(psizer, proportion=1, flag=wx.ALL |wx.EXPAND, border = 0)
		vsizer.Add(p1sizer, proportion=1, flag=wx.ALL |wx.EXPAND, border = 0)
		self.SetSizerAndFit(vsizer)
		self.Bind(wx.EVT_BUTTON, self.OnOkbutton , self.okButton)
		self.Bind(wx.EVT_CLOSE, self.close, self)

	def close(self, event):
		self.par.device_register_frame = None
		self.Destroy()
	def OnOkbutton(self, event):
		new_value = self.after_text.GetLineText(0)	
		if is_error(new_value) == False:
			self.par.ModifyRegValue(self.reg_info[0], self.reg_info[1], new_value)
		self.Destroy()

	def on_after_text(self, event):
		value = self.after_text.GetValue()
		if (value.startswith("0x", 0 ,2) == True or value.startswith("0x", 0 ,2) == True):
			if(len(value) > 10):
				value = value[0:10]
				self.after_text.SetValue(value)
				pos = len(value)
				self.after_text.SetInsertionPoint(pos)
		else:
			if(len(value) > 8):
				value = value[0:8]
				self.after_text.SetValue(value)
				pos = len(value)
				self.after_text.SetInsertionPoint(pos)


class DevRegsFrame(wx.Frame):
	window_name = 'device_registers'

	def __init__(self, parent):
		wx.Frame.__init__(self, parent, wx.ID_ANY, "设备寄存器", style=wx.DEFAULT_FRAME_STYLE)
		self.par = parent
		self.initialize()
	def initialize(self):
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		self.device = ''
		self.device_regs = None
		psizer = wx.BoxSizer(wx.VERTICAL)
		self.panel = wx.Panel(self, wx.ID_ANY)
		psizer.Add(self.panel, proportion=1, flag=wx.EXPAND)
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.panel.SetSizer(sizer)
		self.choice = wx.Choice(self.panel, wx.ID_ANY, choices=[])
		#wx.EVT_CHOICE(self.choice, self.choice.GetId(), self.obj_choice)
		self.Bind(wx.EVT_CHOICE,self.obj_choice,self.choice)
		sizer.Add(self.choice, flag=wx.ALL | wx.EXPAND, border=2)
		self.cblist = wx.ListCtrl(self.panel, wx.ID_ANY, style=wx.LC_REPORT | wx.LC_SINGLE_SEL | wx.LC_VRULES | wx.LC_HRULES)
		self.cblist.SetFont(wx.Font(self.cblist.GetFont().GetPointSize(), wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL))
		self.cblist.InsertColumn(0, "偏移", format=wx.LIST_FORMAT_RIGHT)
		self.cblist.InsertColumn(1, "大小", format=wx.LIST_FORMAT_RIGHT)
		self.cblist.InsertColumn(2, "值(HEX)", format=wx.LIST_FORMAT_RIGHT)
		self.cblist.InsertColumn(3, "名称")
		self.cblist.InsertColumn(4, "描述")
		self.num_cols = 5
		widths = [12, 10, 19, 20, 25]
		for x in range(len(widths)):
			self.cblist.SetColumnWidth(x, widths[x] * self.GetCharWidth())

		self.cblist.SetSizeHints(sum([ self.cblist.GetColumnWidth(x) for x in range(self.num_cols) ]), self.GetCharHeight() * 35)
		sizer.Add(self.cblist, proportion=1, flag=wx.EXPAND | wx.ALL, border=2)
		self.SetSizerAndFit(psizer)
		self.xborder = self.GetSize()[0] - self.GetClientSize()[0]
		self.resizing = False
		#wx.EVT_SIZE(self.cblist, self.resize)
		self.Bind(wx.EVT_SIZE, self.resize,self.cblist)
		#wx.EVT_LIST_COL_END_DRAG(self.cblist, wx.ID_ANY, self.resize)
		self.Bind(wx.EVT_LIST_COL_END_DRAG,self.resize,self.cblist)
		self.Bind(wx.EVT_LIST_ITEM_RIGHT_CLICK, self.on_right, self.cblist)
		self.devname_list = []
		wx.CallAfter(self.fill)
		self.Bind(wx.EVT_CLOSE, self.close, self)

	def close(self, event):
		self.par.device_register_frame = None
		self.Destroy()


	def on_right(self, event):
		rel_pos = event.GetPoint()
		par_pos = self.GetPosition()
		abs_pos = (rel_pos[0] + par_pos[0], rel_pos[1] + par_pos[1])
		self.pos = abs_pos
		index = event.GetIndex()
		value_str = self.GetRegValueStr(index, 2)
		self.modify_item = (index, 2, value_str)

		self.popupID1 = wx.NewId()
		self.Bind(wx.EVT_MENU, self.OnPopupModify, id=self.popupID1)
		menu = wx.Menu()
		it = wx.MenuItem(menu, self.popupID1, "修改")
		menu.Append(it)
		self.PopupMenu(menu)
		menu.Destroy()

	def is_hex(self, value):
		if operator.eq(value[0:2],"0x") == True or operator.eq(value[0:2],"0X") == True:
			return True
		else:
			return False
	def GetRegValueStr(self, index, col):
		item = self.cblist.GetItem(index, col)
		value_str = item.GetText()
		return value_str
	def GetRegNameStr(self, index):
		item = self.cblist.GetItem(index, 3)
		value_str = item.GetText()
		return value_str

	def ModifyRegValue(self, index, col, value_str):
		value_str = value_str.lower()
		if self.is_hex(value_str) == True:
			value_str = value_str[2:]
		else:
			value_int = int(value_str,  10)
			value_str = "%x" % (value_int) 
		before_value_str = self.GetRegValueStr(index, col)
		before_value = str_to_int(before_value_str)
		value = str_to_int(value_str)
		if before_value != value:
			self.cblist.SetItem(index, col, value_str)
			index_c = self.choice.GetSelection()
			device = self.choice.GetString(index_c)
			devname = self.find_regname_in_list(self.devname_list, device)
			regs.set_register(devname, value, index)
			self.cblist.SetItemBackgroundColour(index, "#FFFF00")
		else:
			self.cblist.SetItemBackgroundColour(index, wx.WHITE)

	def OnPopupModify(self, event):
		modify = ModifyDialog(parent = self, pos = self.pos, reg_iter = self.modify_item)
		modify.ShowModal()

	def find_regname_in_list(self, device_list, allname):
		for dev in device_list:
			if operator.eq(allname, dev[1]) == True:
				return  dev[0]
		return None
	def update_device_regs(self, index):
		device = self.choice.GetString(index)
		devname = self.find_regname_in_list(self.devname_list, device)
		device = self.choice.GetString(index)
		if devname == None:
			return 
		regs_list = regs.get_registers(devname)
		if regs_list == None:
			return 
		num = 0
		for reg  in regs_list:
			value = "%x" % (reg["value"])
			before_value_str = self.GetRegValueStr(num, 2)
			before_value = str_to_int(before_value_str)
			if before_value != reg["value"]:
				self.cblist.SetItem(num, 2, value)
				self.cblist.SetItemBackgroundColour(num, "#FFFF00")
			else:
				self.cblist.SetItemBackgroundColour(num, wx.WHITE)
			num = num + 1

	def fill_device_regs(self, index):
		device = self.choice.GetString(index)
		devname = self.find_regname_in_list(self.devname_list, device)
		device = self.choice.GetString(index)
		if devname == None:
			return 
		regs_list = regs.get_registers(devname)
		if regs_list == None:
			return 
		num = 0
		for reg  in regs_list:
			offset = "%x" % reg["offset"]
			size = "%x" % reg["size"]
			value = "%x" % (reg["value"])
			index = self.cblist.InsertItem(num, reg["name"])
			self.cblist.SetItem(index, 0, offset)
			self.cblist.SetItem(index, 1, size)
			self.cblist.SetItem(index, 2, value)
			self.cblist.SetItem(index, 3, reg["name"])
			num = num + 1

	def fill_device_list(self, device_list):
		choice_list = [device_name[1] for device_name in device_list ]
		self.choice.SetItems(choice_list)
		if len(choice_list) < 0:
			return 
		self.choice.SetSelection(0)
		self.fill_device_regs(0)
	
	def fill(self):
		config = conf.GetGlobalConfig()	
		mach_list = config.get_mach_list()
		maped_devices = config.get_maped_devices()
		temp_list = [(dev_dic["name"], "%s.%s" % (dev_dic["mach"], dev_dic["name"])) for dev_dic in maped_devices]
		self.devname_list = self.devname_list + temp_list
		self.fill_device_list(self.devname_list)

	def obj_choice(self, event):
		self.cblist.DeleteAllItems()
		index = self.choice.GetSelection()
		self.fill_device_regs(index)

	def RegsRefurbish(self):
		index = self.choice.GetSelection()
		self.update_device_regs(index)


	def resize(self, event):
		event.Skip()
		if self.resizing:
			return
		self.resizing = True
		col_width = sum([ self.cblist.GetColumnWidth(x) for x in range(self.num_cols - 1) ])
		self.cblist.SetColumnWidth(self.num_cols - 1, self.cblist.GetClientSize()[0] - col_width - self.xborder * 2)
		self.resizing = False

if __name__ == '__main__':
	app = wx.PySimpleApp()
	Regsdlg = DevRegsFrame(parent = None)
	Regsdlg.Show()
	app.MainLoop()
