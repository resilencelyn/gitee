import wx, os, sys
import se_path
from tools import Profile


DefaultPath = se_path.DefaultModulePath


class AddPathDialog(wx.Dialog):

	def __init__(self, parent, info):
		wx.Dialog.__init__(self, parent=parent, title="增加路径", size=(400, 180))
		self.par = parent
		wx.StaticBox(self, -1, '', name='grou1', size=(375, 90), pos=(10, 10))
		font = wx.Font(11, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName='Segoe UI')
		self.SetFont(font)
		self.Path = wx.TextCtrl(self, -1, pos=(20, 40), size=(270, 25), style=wx.TE_RICH)
		self.DirButton = wx.Button(self, wx.ID_ANY, label="浏览", pos=(300, 40), size=(80, 26))
		self.Bind(wx.EVT_BUTTON, self.DirBrowseFunc, self.DirButton)
		self.OkButton = wx.Button(self, wx.ID_ANY, label="确定", pos=(300, 110), size=(80, 26))
		self.Bind(wx.EVT_BUTTON, self.OnOkButton, self.OkButton)

	def OnOkButton(self, event):
		path = self.Path.GetValue()
		if path != '':
			self.par.AddPathFunc(path)
		self.Destroy()

	def DirBrowseFunc(self, event):
		dialog = wx.DirDialog(None, message="工作目录选择", style=wx.DD_DEFAULT_STYLE | wx.DD_NEW_DIR_BUTTON, size=wx.DefaultSize, pos=wx.DefaultPosition)
		if dialog.ShowModal() == wx.ID_OK:
			path = dialog.GetPath()
			self.Path.SetValue(path)
		dialog.Destroy()
		return


class RemoteGdbWindow(wx.ScrolledWindow):

	def __init__(self, parent, info):
		self.profile = Profile(None)
		wx.ScrolledWindow.__init__(self, parent, wx.ID_ANY, style=wx.BORDER_DOUBLE, size=(540, 390), pos=(0, 10))
		self.SetBackgroundColour('#f0f0f0')
		font = wx.Font(11, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT)
		self.SetFont(font)
		wx.StaticBox(self, -1, "监听IP地址", name='grou1', size=(480, 80), pos=(30, 30))
		self.IpAddr = wx.TextCtrl(self, -1, pos=(60, 60), size=(270, 25), style=wx.TE_RICH)
		self.DefaultAddrButton = wx.Button(self, wx.ID_ANY, label="使用默认", pos=(350, 60), size=(80, 26))
		wx.StaticBox(self, -1, "监听端口号", name='grou2', size=(480, 80), pos=(30, 120))
		self.Port = wx.TextCtrl(self, -1, pos=(60, 150), size=(270, 25), style=wx.TE_RICH)
		self.DefaultPortButton = wx.Button(self, wx.ID_ANY, label="使用默认", pos=(350, 150), size=(80, 26))
		self.InitData()
		self.Bind(wx.EVT_BUTTON, self.OnAddrButton, self.DefaultAddrButton)
		self.Bind(wx.EVT_BUTTON, self.OnPortButton, self.DefaultPortButton)
		return

	def InitData(self):
		if self.profile.has_key('RemoteGdb', 'ipaddr'):
			addr = self.profile.get('RemoteGdb', 'ipaddr')
			self.IpAddr.SetValue(addr)
		else:
			self.IpAddr.SetValue('INADDR_ANY')
		if self.profile.has_key('RemoteGdb', 'port'):
			port = self.profile.get('RemoteGdb', 'port')
			self.Port.SetValue(port)
		else:
			self.Port.SetValue('10000')

	def OnAddrButton(self, event):
		self.IpAddr.SetValue('INADDR_ANY')

	def OnPortButton(self, event):
		self.Port.SetValue('10000')

	def Save(self):
		self.profile.set('RemoteGdb', 'ipaddr', self.IpAddr.GetValue())
		self.profile.set('RemoteGdb', 'port', self.Port.GetValue())

	def Check(self):
		pass 


class ModuleWindow(wx.ScrolledWindow):

	def __init__(self, parent, extractor):
		wx.ScrolledWindow.__init__(self, parent, wx.ID_ANY, style=wx.BORDER_NONE, size=(540, 390), pos=(0, 10))
		self.SetBackgroundColour('#f0f0f0')
		self.SetScrollRate(0, 20)
		self.SetScrollbars(1, 1, 500, 380)
		self.profile = Profile(None)
		wx.StaticBox(self, -1, "模块加载路径配置", name='grou1', size=(515, 370), pos=(10, 8))
		self.AddButton = wx.Button(self, label="增加路径", pos=(340, 40), size=(80, 24))
		self.DeleteButton = wx.Button(self, label="删除路径", pos=(430, 40), size=(80, 24))
		self.DeleteButton.Disable()
		self.PathList = wx.ListCtrl(self, -1, style=wx.LC_REPORT | wx.LC_HRULES | wx.BORDER_SUNKEN, pos=(20, 80), size=(495, 290))
		self.PathList.InsertColumn(0, "路径", format=wx.LIST_FORMAT_LEFT, width=480)
		self.PathList.InsertItem(0, DefaultPath)
		self.InsertExtModulePath()
		self.Bind(wx.EVT_BUTTON, self.OnAddButton, self.AddButton)
		self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnSelect, self.PathList)
		self.Bind(wx.EVT_LIST_ITEM_DESELECTED, self.OnDisselect, self.PathList)
		self.Bind(wx.EVT_BUTTON, self.OnDeleteButton, self.DeleteButton)
		return

	def InsertExtModulePath(self):
		if self.profile.has_key('ExtModulePath', 'count') == False:
			return
		num_str = self.profile.get('ExtModulePath', 'count')
		num = int(num_str)
		for i in range(1, num + 1):
			path = self.profile.get('ExtModulePath', str(i))
			#path = unicode(path, 'UTF-8')
			self.PathList.InsertItem(i, path)

	def OnDeleteButton(self, event):
		item = self.PathList.GetFocusedItem()
		self.PathList.DeleteItem(item)

	def OnDisselect(self, event):
		self.DeleteButton.Disable()

	def OnSelect(self, event):
		item = self.PathList.GetFocusedItem()
		if item != self.PathList.GetTopItem():
			self.DeleteButton.Enable()
		else:
			self.DeleteButton.Disable()

	def OnAddButton(self, event):
		dlg = AddPathDialog(self, info=None)
		ret = dlg.ShowModal()
		return

	def AddPathFunc(self, path):
		num = self.PathList.GetItemCount()
		if self.PathList.FindItem(0, path) == -1:
			self.PathList.InsertItem(num, path)

	def Save(self):
		num = self.PathList.GetItemCount()
		self.profile.set('ExtModulePath', 'count', str(num - 1))
		for i in range(1, num):
			path = self.PathList.GetItemText(i)
			self.profile.set('ExtModulePath', str(i), path)

	def Check(self):
		pass


class ListPanel(wx.Panel):

	def __init__(self, parent):
		wx.Panel.__init__(self, parent=parent)
		self.par = parent
		self.SetBackgroundColour('#f0f0f0')
		self.tree = wx.TreeCtrl(self, pos=(10, 10), size=(160, 390))
		root = self.tree.AddRoot("选项")
		self.PathItem = self.tree.AppendItem(root, "模块加载")
		self.RemoteGdbItem = self.tree.AppendItem(root, "远程调试")
		self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelectItem, self.tree)
		self.tree.ExpandAll()
		self.par.ContentSetWindow(0)

	def OnSelectItem(self, event):
		item = self.tree.GetSelection()
		if item == self.PathItem:
			self.par.ContentSetWindow(1)
		elif item == self.RemoteGdbItem:
			self.par.ContentSetWindow(2)


class ContentPanel(wx.Panel):

	def __init__(self, parent):
		wx.Panel.__init__(self, parent=parent)
		self.Box = wx.BoxSizer(wx.HORIZONTAL)
		self.SetSizer(self.Box)
		self.WindowList = []

	def SetWindow(self, id):
		if id == 0:
			self.scrolled = ModuleWindow(self, None)
			self.WindowList.append(self.scrolled)
			self.Box.Add(self.scrolled, 1, wx.EXPAND)
			self.Box.Hide(0)
			self.Box.Remove(0)
			self.RemoteGdb = RemoteGdbWindow(self, None)
			self.WindowList.append(self.RemoteGdb)
			self.Box.Add(self.RemoteGdb, 1, wx.EXPAND)
			self.Box.Hide(0)
			self.Box.Remove(0)
			self.Box.Add(self.scrolled, 1, wx.EXPAND)
			self.Box.Show(0)
			return
		else:
			if id == 1:
				self.Box.Hide(0)
				self.Box.Remove(0)
				self.Box.Add(self.scrolled, 1, wx.EXPAND)
				self.Box.Show(0)
			if id == 2:
				self.Box.Hide(0)
				self.Box.Remove(0)
				self.Box.Add(self.RemoteGdb, 1, wx.EXPAND)
				self.Box.Show(0)
			return

	def Save(self):
		for window in self.WindowList:
			window.Save()

	def Check(self):
		for window in self.WindowList:
			window.Check()


class TopPanel(wx.Panel):
	def __init__(self, parent):
		wx.Panel.__init__(self, parent=parent)
		self.SetBackgroundColour('#f0f0f0')
		self.ContentWindow = ContentPanel(self)
		self.ListWindow = ListPanel(self)
		Box = wx.BoxSizer(wx.HORIZONTAL)
		Box.Add(self.ListWindow, 1, wx.EXPAND)
		Box.Add(self.ContentWindow, 3, wx.EXPAND)
		self.SetSizer(Box)

	def ContentSetWindow(self, id):
		self.ContentWindow.SetWindow(id)

	def Save(self):
		self.ContentWindow.Save()


class OptionDialog(wx.Dialog):

	def __init__(self, parent):
		wx.Dialog.__init__(self, parent=None, title="选项", size=(740, 475))
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		self.toppanel = TopPanel(self)
		self.bottonpanel = wx.Panel(self)
		self.bottonpanel.SetBackgroundColour('#f0f0f0')
		OkButton = wx.Button(self.bottonpanel, label="确定", pos=(645, 5), size=(80, 28))
		CancelButton = wx.Button(self.bottonpanel, label="取消", pos=(550, 5), size=(80, 28))
		Box = wx.BoxSizer(wx.VERTICAL)
		Box.Add(self.toppanel, 10, flag=wx.EXPAND)
		Box.Add(self.bottonpanel, 1, flag=wx.EXPAND)
		self.SetSizer(Box)
		self.Bind(wx.EVT_BUTTON, self.OnOkButton, OkButton)
		self.Bind(wx.EVT_BUTTON, self.CancelOkButton, CancelButton)

	def OnOkButton(self, event):
		self.toppanel.Save()
		self.Destroy()

	def CancelOkButton(self, event):
		self.Destroy()

	def CloseSelf(self, event):
		self.Destroy()


if __name__ == '__main__':
	app = wx.PySimpleApp()
	dialog = OptionDialog(parent=None)
	ret = dialog.ShowModal()
	app.MainLoop()
