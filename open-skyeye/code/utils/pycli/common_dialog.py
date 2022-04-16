# -*- coding: UTF-8 -*-
import wx, os
import se_path

class ErrorReportDialog(wx.Dialog):
	def __init__(self, errormesg, detail):
		wx.Dialog.__init__(self, parent = None, title = "错误报告", size = (400, 400))

		font = wx.Font(8, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
		self.SetFont(font)
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		self.SetBackgroundColour("#f0f0f0")
		TopPanel = wx.Panel(self)
		TopPanel.SetBackgroundColour(wx.WHITE)
		text = wx.StaticText(TopPanel, -1, errormesg, (50, 15))
		font = wx.Font(8, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD)
		text.SetFont(font)

		MidPanel = wx.Panel(self)
		MidPanel.SetBackgroundColour("#f0f0f0")
		text = wx.StaticText(MidPanel, -1, "详细信息:", (20, 20))
		font = wx.Font(10, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD)
		text.SetFont(font)

		DetailInfo = wx.TextCtrl(MidPanel, -1, pos = (20, 40), size = (355, 125), style = wx.TE_READONLY | wx.TE_MULTILINE | wx.TE_RICH)
		DetailInfo.SetEditable(False)
		DetailInfo.SetValue(detail)


		BottonPanel = wx.Panel(self)
		text = wx.StaticText(BottonPanel, -1, "软件问题导致 天目 意外停止。", (20, 20))
		font = wx.Font(8, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD)
		text.SetFont(font)

		wx.StaticText(BottonPanel, -1, "由此带来的不便, 我们深表歉意。 您可与我们取得联系并可以将上面\n的错误详细信息提供给我们寻求解决方案。", (20, 60))
	#	text = wx.StaticText(BottonPanel, -1, "点击 重置 按钮可以重新启动天目。", (20, 100))

		self.CloseButton = wx.Button(BottonPanel, wx.ID_ANY, label = "确定", pos = (300, 130))
		self.Bind(wx.EVT_BUTTON, self.CloseSelf, self.CloseButton)

		self.ResetButton = wx.Button(BottonPanel, wx.ID_ANY, label = "重启天目", pos = (215, 130))
		self.Bind(wx.EVT_BUTTON, self.ResetSkyEye, self.ResetButton)

		sizer = wx.BoxSizer(wx.VERTICAL)
		sizer.Add(TopPanel, 1, flag = wx.EXPAND)
		sizer.Add(MidPanel, 4, flag = wx.EXPAND)
		sizer.Add(BottonPanel, 4, flag = wx.EXPAND)

		self.SetSizer(sizer)
		self.Center()
		self.Bind(wx.EVT_CLOSE, self.CloseSelf, self)

	def CloseSelf(self, event):
		self.EndModal(wx.ID_CLOSE)

	def ResetSkyEye(self, event):
		self.EndModal(wx.ID_RESET)




class WorkSpaceSet(wx.Dialog):
	def __init__(self, parent, args):
		wx.Dialog.__init__(self, parent = parent, size = (500, 220), style = wx.CLIP_CHILDREN)
		self.par = parent
		self.DefaultWorkspaceDir = args[0]

		font = wx.Font(11, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT)
		self.SetFont(font)
		text = wx.StaticText(self, -1, "工作目录:", (20, 65))
		font = wx.Font(10, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD)
		text.SetFont(font)
		self.WorkspaceDir = wx.TextCtrl(self, -1, pos = (100, 60), size =(290, 25)  , style =wx.TE_RICH)
		self.WorkspaceDir.SetValue(self.DefaultWorkspaceDir)

		self.DirButton = wx.Button(self, wx.ID_ANY, label = "浏览", pos = (400, 59), size = (80, 26))
		self.Bind(wx.EVT_BUTTON, self.DirBrowseFunc, self.DirButton)

		self.OkButton = wx.Button(self, wx.ID_ANY, label = "确定", pos = (300, 170), size = (80, 26))
		self.Bind(wx.EVT_BUTTON, self.OnOkButton, self.OkButton)
		self.CancelButton = wx.Button(self, wx.ID_ANY, label = "取消", pos = (400, 170), size = (80, 26))
		self.Bind(wx.EVT_BUTTON, self.OnCancelButton, self.CancelButton)

		self.NoAsk = wx.CheckBox(self, wx.ID_ANY, pos = (20, 140), label = "设置为默认值且不再询问")

	def OnOkButton(self, event):
		WorkspaceDirValue = self.WorkspaceDir.GetValue()
		self.par.WorkspaceDirValue = WorkspaceDirValue
		if self.NoAsk.GetValue() == True:
			self.EndModal(wx.ID_DEFAULT)
		else:
			self.EndModal(wx.ID_OK)

	def OnCancelButton(self, event):
		self.EndModal(wx.ID_CANCEL)


	def DirBrowseFunc(self, event):
		dialog = wx.DirDialog(None, message = "工作目录选择", style = wx.DD_DEFAULT_STYLE | wx.DD_NEW_DIR_BUTTON, size = wx.DefaultSize, pos = wx.DefaultPosition)
		if dialog.ShowModal() == wx.ID_OK:
			path = dialog.GetPath()
			self.WorkspaceDir.SetValue(path)
		dialog.Destroy()






if __name__ == '__main__':
	app = wx.PySimpleApp()
	dialog = WorkSpaceSet(parent = None)
	ret = dialog.ShowModal()
	if ret == wx.ID_CANCEL:
		pass
	app.MainLoop()
