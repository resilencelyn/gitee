# -*- coding: utf-8 -*-
		

import wx
import se_path
import operator
import conf
#from threading import Thread
import _thread

def getSplitterSashSize(splitter):
	"""Get the size of the sash for a vertical splitter window.
	"""
	sw, sh = splitter.GetSize()
	lw, lh = splitter.GetWindow1().GetSize()
	rw, rh = splitter.GetWindow2().GetSize()
	return sw - lw - rw

class Column(object):

	def __init__(self, name, width, key, format):
		self.name = name
		self.width = width
		self.key = key
		self.format = format

class Controller(object):
	def __init__(self, window):
		self.window = window
		#self.model = model
		splitter = wx.SplitterWindow(window)
		cw = self.window.GetCharWidth()
		self.spaceTree = wx.TreeCtrl(splitter, style=wx.TR_DEFAULT_STYLE)
		self.rootID = self.spaceTree.AddRoot("设备列表")
		self.spaceTree.Expand(self.rootID)
		w = cw * 20
		dummy, h = self.spaceTree.GetSize()
		self.spaceTree.SetSize((w, h))
		rightPanel = wx.Panel(splitter)
		self.mapList = wx.ListCtrl(rightPanel, style=wx.LC_REPORT)
		self.columns = [Column("基地址", 15 * cw, 'base', wx.LIST_FORMAT_RIGHT),
		Column("设备名称", 25 * cw, 'device', wx.LIST_FORMAT_LEFT),
		Column("偏移", 15 * cw, 'offset', wx.LIST_FORMAT_RIGHT),
		Column("内存长度", 15 * cw, 'length', wx.LIST_FORMAT_RIGHT),
		Column("所属对象", 15 * cw, 'target', wx.LIST_FORMAT_LEFT)]
		for i, c in enumerate(self.columns):
			self.mapList.InsertColumn(i, c.name, format=c.format)
			self.mapList.SetColumnWidth(i, c.width)

		w = sum((c.width for c in self.columns))
		dummy, h = self.mapList.GetSize()
		self.mapList.SetSize((w, h))
		self.inspectButton = wx.Button(rightPanel, label='确定')
		self.inspectButton.Disable()
		rightSizer = wx.BoxSizer(wx.VERTICAL)
		rightSizer.Add(self.mapList, flag=wx.EXPAND, proportion=1)
		rightSizer.Add(self.inspectButton, flag=wx.ALIGN_RIGHT | wx.ALL, border=8)
		rightPanel.SetSizer(rightSizer)
		lw, lh = self.spaceTree.GetSize()
		rw, rh = self.mapList.GetSize()
		splitter.SplitVertically(self.spaceTree, rightPanel)
		sashWidth = getSplitterSashSize(splitter)
		w = lw + rw + sashWidth
		h = w * 0.4
		splitterSize = (w, h)
		splitter.SetSize(splitterSize)
		splitter.SetSashPosition(lw)
		bw, bh = self.inspectButton.GetSize()
		splitter.SetMinimumPaneSize(bw + 16)
		mw = splitter.GetMinimumPaneSize() * 2 + sashWidth
		mh = mw * 0.4
		self.window.SetClientSize((mw, mh))
		self.window.SetMinSize(self.window.GetSize())
		outerSizer = wx.BoxSizer(wx.HORIZONTAL)
		outerSizer.Add(splitter, flag=wx.EXPAND, proportion=1)
		self.window.SetSizer(outerSizer)
		self.window.SetClientSize(splitterSize)
		self.spaceTree.Bind(wx.EVT_TREE_SEL_CHANGED, self.onNodeSelected)
		self.config = None
		load_thread = _thread.start_new_thread (self.fillTree,())
		self.memmap = []
		self.inspectButton.Bind(wx.EVT_BUTTON, self.onInspectMemory)

	def UpdateTree(self, map_list):
		for item in map_list:
			self.spaceTree.AppendItem(self.rootID, item["fulldes"])
		self.spaceTree.Expand(self.rootID)
	def fillTree(self):
		self.memmap = self.GetMemMap()
		wx.CallAfter(self.UpdateTree, self.memmap)
		
	def GetMemSpace(self, mach, device):
		map_attr = []
		attr_list = self.config.get_device_attr_list(mach, device)
		if "length" in attr_list and "address" in attr_list:
			length = self.config.get_device_attr_value_list(mach, device, "length")
			base = self.config.get_device_attr_value_list(mach, device, "address")
			map_attr.append(base)
			map_attr.append(length)
		return map_attr
		
	def GetTarget(self, cpulist):
		target_str = ""
		if len(cpulist) == 1:
			target_str = cpulist[0]
		else:
			for cpuname in cpulist:
				cpustr = "[%d]" % (cpuname)
				target_str = target_str + cpustr
		return target_str

	def GetMemMap(self):
		maplist = []
		self.config = conf.GetGlobalConfig()
		if self.config == None:
			return maplist
		maped_devices = self.config.get_maped_devices()
		for dev_dic in maped_devices:
			target_list = self.config.get_cpulist_by_mach(dev_dic["mach"])
			if len(target_list) == 0:
				target_str  = ""
			else:
				target_str = self.GetTarget(target_list)
			dic = {}
			dic["mach"] = dev_dic["mach"]
			dic["device"] = dev_dic["name"]
			dic["base"] = "%x" % dev_dic["base"]
			dic["length"] = "%x" % dev_dic["length"]
			dic["offset"] = "%x" % dev_dic["offset"]
			dic["target"] = target_str
			dic["fulldes"] = "%s.%s" %(dev_dic["mach"], dev_dic["name"])
			maplist.append(dic)
		return maplist

	def onNodeSelected(self, evt):
		current = self.getCurrentNode()
		if current is None:
			self.inspectButton.Disable()
		else:
			self.inspectButton.Enable()
			self.fillInNodeInfo(current)
		return

	def fillInNodeInfo(self, node):
		self.mapList.DeleteAllItems()
		index = self.mapList.InsertItem(0, node["base"])
		self.mapList.SetItem(index, 1, node["fulldes"])
		self.mapList.SetItem(index, 2, node["offset"])
		self.mapList.SetItem(index, 3, node["length"])
		self.mapList.SetItem(index, 4, node["target"])

	def getCurrentNode(self):
		i = self.spaceTree.GetSelection()
		if not i.IsOk() or i == self.rootID:
			return None
		fulldes = self.spaceTree.GetItemText(i)
		for node in self.memmap:
			if operator.eq(node["fulldes"], fulldes) == True:
				return node
		return None
			
			
	def onInspectMemory(self, evt):
		self.window.on_close(0)

class MemoryBrowserWindow(wx.Frame):
	def __init__(self, parent):
		wx.Frame.__init__(self, parent, title = "内存映射", style=wx.DEFAULT_FRAME_STYLE)
		self.par = parent
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		self.controller = Controller(self)
		self.Bind(wx.EVT_CLOSE, self.on_close)

	def on_close(self, event):
		self.par.mem_map_frame = None
		self.Destroy()


if __name__ == '__main__':
	app = wx.PySimpleApp()
	Regsdlg = MemoryBrowserWindow(parent = None)
	Regsdlg.Show()
	app.MainLoop()
