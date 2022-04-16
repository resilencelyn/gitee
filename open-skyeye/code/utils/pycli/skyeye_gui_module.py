import wx, os, sys
from threading import Thread
from skyeye_common_module import *
from common_dialog import *

class ModuleWindow(wx.Panel):

	def __init__(self, parent, extractor):
		wx.Panel.__init__(self, parent, wx.ID_ANY, style=wx.BORDER_DOUBLE, pos=(10, 10), size=(400, 390))
		font = wx.Font(9, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName='Segoe UI')
		self.SetFont(font)
		StaticBox1 = wx.StaticBox(self, -1, "作者", name='grou1', size=(180, 60), pos=(10, 10))
		StaticBox2 = wx.StaticBox(self, -1, "版本", name='grou3', size=(180, 60), pos=(200, 10))
		StaticBox2 = wx.StaticBox(self, -1, "路径", name='grou2', size=(370, 60), pos=(10, 80))
		StaticBox3 = wx.StaticBox(self, -1, "描述", name='grou3', size=(370, 230), pos=(10, 150))
		self.autor = wx.TextCtrl(self, -1, pos=(20, 35), size=(160, 22), style=wx.TE_RICH | wx.BORDER_SUNKEN)
		self.version = wx.TextCtrl(self, -1, pos=(210, 35), size=(160, 22), style=wx.TE_RICH | wx.BORDER_SUNKEN)
		self.path = wx.TextCtrl(self, -1, pos=(20, 100), size=(350, 22), style=wx.TE_RICH | wx.BORDER_SUNKEN)
		self.des = wx.TextCtrl(self, -1, pos=(20, 170), size=(350, 200), style=wx.TE_RICH | wx.BORDER_SUNKEN)
		self.autor.SetEditable(False)
		self.version.SetEditable(False)
		self.path.SetEditable(False)
		self.des.SetEditable(False)

	def SetAuthor(self, value):
		self.autor.SetValue(value)

	def SetVersion(self, value):
		self.version.SetValue(value)

	def SetDes(self, value):
		self.des.SetValue(value)

	def SetPath(self, value):
		path = os.path.abspath(value)
		self.path.SetValue(path)

	def Clear(self):
		self.autor.Clear()
		self.version.Clear()
		self.des.Clear()
		self.path.Clear()


class ListPanel(wx.Panel):

	def __init__(self, parent):
		wx.Panel.__init__(self, parent)
		font = wx.Font(9, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName='Segoe UI')
		self.SetFont(font)
		self.par = parent
		self.SetBackgroundColour('#f0f0f0')
		self.ListBox = wx.ListBox(self, -1, pos=(10, 10), size=(155, 390), style=wx.LB_SINGLE | wx.LB_HSCROLL | wx.LB_NEEDED_SB )
		self.Bind(wx.EVT_LISTBOX, self.OnSelect, self.ListBox)
		self.Thread = Thread(group=None, target=self.Load)
		self.Thread.start()
		return

	def SetItem(self, strlist):
		listtemp = [(x.lower(),x) for x in strlist]
		listtemp.sort()  
		sort_list = [x[1] for x in listtemp]
		self.ListBox.InsertItems(sort_list, 0)
		num = self.ListBox.GetCount()
		if num > 0:
			self.ListBox.Select(0)
			self.SetContentPageByIndex(0)

	def Load(self):
		try:
			name_str = SkyEyeGetModules()
		except:
			self.par.par.par.ErrorReport()
		list = name_str.split(';')
		list.remove('|')
		wx.CallAfter(self.SetItem, list)

	def SetContentPageByIndex(self, index):
		li = []
		item = self.ListBox.GetString(index)
		try:
			author = SkyEyeGetModuleValueByName(item, 'ModuleAuthor')
		except:
			self.par.par.par.ErrorReport()
		try:
			version = SkyEyeGetModuleValueByName(item, 'ModuleVersion')
		except:
			self.par.par.par.ErrorReport()
		try:
			des = SkyEyeGetModuleValueByName(item, 'ModuleDescription')
		except:
			self.par.par.par.ErrorReport()
		try:
			path = SkyEyeGetModulePathByName(item)
		except:
			self.par.par.par.ErrorReport()
		li.append(author)
		li.append(version)
		li.append(des)
		li.append(path)
		self.par.SetValue(li)

	def OnSelect(self, event):
		self.par.Clear()
		index = self.ListBox.GetSelection()
		self.SetContentPageByIndex(index)


class ContentPanel(wx.Panel):

	def __init__(self, parent):
		wx.Panel.__init__(self, parent=parent)
		self.ModuleWindow = ModuleWindow(self, None)
		return

	def SetValue(self, ValueList):
		if ValueList[0] != None:
			self.ModuleWindow.SetAuthor(ValueList[0])
		if ValueList[1] != None:
			self.ModuleWindow.SetVersion(ValueList[1])
		if ValueList[2] != None:
			self.ModuleWindow.SetDes(ValueList[2])
		if ValueList[3] != None:
			self.ModuleWindow.SetPath(ValueList[3])
		return

	def Clear(self):
		self.ModuleWindow.Clear()


class TopPanel(wx.Panel):

	def __init__(self, parent):
		wx.Panel.__init__(self, parent=parent)
		self.par = parent
		self.ContentWindow = ContentPanel(self)
		self.ListWindow = ListPanel(self)
		Box = wx.BoxSizer(wx.HORIZONTAL)
		Box.Add(self.ListWindow, 2, wx.EXPAND)
		Box.Add(self.ContentWindow, 5, wx.EXPAND)
		self.SetSizer(Box)

	def SetValue(self, ValueList):
		self.ContentWindow.SetValue(ValueList)

	def Clear(self):
		self.ContentWindow.Clear()


class ModuleBrowseDialog(wx.Dialog):

	def __init__(self, parent):
		wx.Dialog.__init__(self, parent, title="模块", size=(600, 475))
		self.par = parent
		self.toppanel = TopPanel(self)
		self.bottonpanel = wx.Panel(self)
		self.bottonpanel.SetBackgroundColour('#f0f0f0')
		OkButton = wx.Button(self.bottonpanel, label="确定", pos=(500, 5), size=(80, 28))
		Box = wx.BoxSizer(wx.VERTICAL)
		Box.Add(self.toppanel, 10, flag=wx.EXPAND)
		Box.Add(self.bottonpanel, 1, flag=wx.EXPAND)
		self.SetSizer(Box)
		self.Bind(wx.EVT_BUTTON, self.CloseSelf, OkButton)
		self.Bind(wx.EVT_CLOSE, self.CloseSelf)

	def CloseSelf(self, event):
		self.par.module_frame = None
		self.Destroy()


if __name__ == '__main__':
	app = wx.PySimpleApp()
	dialog = ModuleBrowseDialog(parent=None)
	ret = dialog.ShowModal()
	app.MainLoop()

