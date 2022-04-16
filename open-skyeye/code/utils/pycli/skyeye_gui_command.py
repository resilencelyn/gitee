# -*- coding: utf-8 -*-
		

import wx
import cli
import se_path

class CommandBrowserWindow(wx.Frame):
	def __init__(self, parent):
		wx.Frame.__init__(self, parent,title = "命令", style=wx.DEFAULT_FRAME_STYLE)
		self.par = parent
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		self.current_cmd = ''
		self.cmd_names = []
		self.last_search = ''
		sizer = wx.BoxSizer(wx.HORIZONTAL)
		panel = wx.Panel(self, wx.ID_ANY)
		sizer.Add(panel, flag=wx.EXPAND, proportion=1)
		psizer = wx.BoxSizer(wx.HORIZONTAL)
		lsizer = wx.BoxSizer(wx.VERTICAL)
		rsizer = wx.BoxSizer(wx.VERTICAL)
		psizer.Add(lsizer, flag=wx.EXPAND, proportion=2)
		psizer.Add(rsizer, flag=wx.EXPAND, proportion=3)
		ssizer = wx.BoxSizer(wx.HORIZONTAL)
		lsizer.Add(ssizer, flag=wx.EXPAND | wx.ALL, border=4)
		self.search = wx.TextCtrl(panel, wx.ID_ANY, style=wx.TE_PROCESS_ENTER)
		self.Bind(wx.EVT_TEXT_ENTER, self.do_search, self.search)
		self.sbutton = wx.Button(panel, wx.ID_FIND, style=wx.BU_EXACTFIT, label = "查找")
		self.Bind(wx.EVT_BUTTON, self.do_search,self.sbutton)
		ssizer.Add(self.search, proportion=1, flag=wx.EXPAND | wx.ALL, border=2)
		ssizer.Add(self.sbutton, flag=wx.ALL, border=2)
		cbox = wx.StaticBox(panel, label="可用命令")
		csizer = wx.StaticBoxSizer(cbox, wx.VERTICAL)
		lsizer.Add(csizer, flag=wx.EXPAND | wx.ALL, proportion=1, border=2)
		self.command_list = wx.ListBox(panel, wx.ID_ANY, style=wx.LB_SINGLE, choices=[])
		self.Bind(wx.EVT_LISTBOX, self.command_list_select,self.command_list)
		self.command_list.SetSizeHints(self.GetCharWidth() * 30, -1)
		csizer.Add(self.command_list, proportion=1, flag=wx.EXPAND | wx.ALL, border=2)
		ybox = wx.StaticBox(panel, label="用法")
		ysizer = wx.StaticBoxSizer(ybox, wx.VERTICAL)
		rsizer.Add(ysizer, proportion=2, flag=wx.EXPAND | wx.ALL, border=2)
		self.synopsis = wx.TextCtrl(panel, wx.ID_ANY, style=wx.TE_READONLY | wx.TE_MULTILINE | wx.TE_RICH)
		self.synopsis.SetFont(wx.Font(self.synopsis.GetFont().GetPointSize(), wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL))
		ysizer.Add(self.synopsis, proportion=1, flag=wx.EXPAND | wx.ALL, border=2)
		dbox = wx.StaticBox(panel, label="说明")
		dsizer = wx.StaticBoxSizer(dbox, wx.VERTICAL)
		rsizer.Add(dsizer, proportion=6, flag=wx.EXPAND | wx.ALL, border=2)
		self.docs = wx.TextCtrl(panel, wx.ID_ANY, style=wx.TE_READONLY | wx.TE_MULTILINE | wx.TE_RICH)
		dsizer.Add(self.docs, proportion=1, flag=wx.EXPAND | wx.ALL, border=2)
		abox = wx.StaticBox(panel, label="另见")
		asizer = wx.StaticBoxSizer(abox, wx.VERTICAL)
		rsizer.Add(asizer, proportion=3, flag=wx.EXPAND | wx.ALL, border=2)
		self.see_also = wx.ListBox(panel, wx.ID_ANY, style=wx.LB_SINGLE, choices=[])
		self.Bind(wx.EVT_LISTBOX, self.see_also_select,self.see_also)
		asizer.Add(self.see_also, proportion=1, flag=wx.EXPAND | wx.ALL, border=2)
		#self.update_command_list()
		panel.SetSizer(psizer)
		self.SetSizerAndFit(sizer)
		self.SetAutoLayout(True)
		self.all_command_list = []
		self.all_command_list = cli.Commands()
		self.update_command_list(self.all_command_list)
		self.Bind(wx.EVT_CLOSE, self.on_close)

	def on_close(self, event):
		self.par.command_frame = None
		self.Destroy()

	def search_commands(self, cmdname):
		search_list = []
		for cmd in self.all_command_list:
			if cmdname in cmd:
				search_list.append(cmd)

		return search_list

	def update_command_list(self, cmd_list):
		listtemp = [(x.lower(),x) for x in cmd_list]
		listtemp.sort()  
		command_sort_list = [x[1] for x in listtemp]
		self.command_list.Clear()
		self.command_list.InsertItems(command_sort_list, 0)

	def delayed_update_after_search(self, cmdname):
		if cmdname == "":
			self.update_command_list(self.all_command_list)
		else:
			cmd_list = self.search_commands(cmdname)
			self.update_command_list(cmd_list)
		self.sbutton.Enable(True)

	def update_after_search(self, text):
		self.last_search = text
		self.sbutton.Enable(False)
		wx.CallAfter(self.delayed_update_after_search, text)


	def do_search(self, event):
		text = self.search.GetLineText(0)
		self.update_after_search(text)

	def command_list_select(self, event):
			self.display_command(event.GetString())

	def see_also_select(self, event):
			self.display_command(event.GetString())
		
	def display_command(self, cmd_name):
		cmd = cli.GetCommand(cmd_name)
		if cmd == None:
			self.docs.SetValue('')
			self.synopsis.SetValue('')
			self.see_also.Clear()
			return
		else:
			doc_str = cmd.doc() 
			see_also = cmd.see_also()
			syn_list = cmd.synopses()
			self.docs.SetValue(doc_str)
			self.synopsis.SetValue(syn_list)
			self.docs.ShowPosition(0)
			self.see_also.Clear()
			self.see_also.InsertItems([see_also], 0)





if __name__ == '__main__':
	app = wx.PySimpleApp()
	Regsdlg = CommandBrowserWindow(parent = None)
	Regsdlg.Show()
	app.MainLoop()
