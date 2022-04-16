#coding:utf-8
import wx
import os,sys,time
import se_path


class Output(wx.Frame):
	def __init__(self,parent):
		wx.Frame.__init__(self,parent,-1,"输出")
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		panel=wx.Panel(self)
		MainBox=wx.BoxSizer(wx.VERTICAL)
		ShowBox=wx.BoxSizer(wx.VERTICAL)
		CtrlBox=wx.BoxSizer(wx.HORIZONTAL)

		self.ShowText=wx.TextCtrl(panel,-1,style=wx.TE_READONLY|wx.TE_MULTILINE|wx.HSCROLL)
		self.ShowText.SetMinSize((400,400))
		ShowBox.Add(self.ShowText,1,wx.ALL|wx.EXPAND,10)
		

		SaveBtu=wx.Button(panel,-1,"保存")
		self.Bind(wx.EVT_BUTTON,self.Btu_SaveBtu,SaveBtu)
		CtrlBox.Add(SaveBtu,0,wx.ALL|wx.EXPAND,10)


		MainBox.Add(ShowBox,1,wx.ALL|wx.EXPAND,10)
		MainBox.Add(CtrlBox,0,wx.ALL|wx.EXPAND,10)
		panel.SetSizer(MainBox)
		panel.Fit()
		self.Fit()
		self.Bind(wx.EVT_CLOSE,self.Close,self)
	
	def Close(self,env):
		self.Hide()

	def WriteText(self,data):
		self.ShowText.AppendText(data)

	def Btu_SaveBtu(self,env):
		SaveFile=wx.FileDialog(self,"保存数据文件",os.getcwd(),'',"*.*", wx.FD_SAVE|wx.FD_OVERWRITE_PROMPT|wx.FD_CHANGE_DIR)
		if SaveFile.ShowModal() == wx.ID_OK:
			info=self.ShowText.GetValue()
			sf=open(SaveFile.GetPath(),"w")
			sf.write(info)
			sf.close()
		SaveFile.Destroy()

