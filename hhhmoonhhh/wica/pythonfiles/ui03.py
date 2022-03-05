# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version Jun 17 2015)
## http://www.wxformbuilder.org/
##
## PLEASE DO "NOT" EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc

###########################################################################
## Class MyFrame1
###########################################################################

class MyFrame0000000001 ( wx.Frame ):
	
	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"APPManaging", pos = wx.DefaultPosition, size = wx.Size( 330,440 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )
		
		bSizer1 = wx.BoxSizer( wx.VERTICAL )
		
		sbSizer1 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"查看所有手机APP" ), wx.VERTICAL )
		
		self.m_button1 = wx.Button( sbSizer1.GetStaticBox(), wx.ID_ANY, u"查看所有手机APP", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer1.Add( self.m_button1, 0, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer1, 1, wx.EXPAND, 5 )
		
		sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"查看正在运行的APP" ), wx.VERTICAL )
		
		self.m_button2 = wx.Button( sbSizer2.GetStaticBox(), wx.ID_ANY, u"查看正在运行的APP", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer2.Add( self.m_button2, 0, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer2, 1, wx.EXPAND, 5 )
		
		sbSizer3 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"关闭所有APP" ), wx.VERTICAL )
		
		self.m_button3 = wx.Button( sbSizer3.GetStaticBox(), wx.ID_ANY, u"关闭所有APP", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer3.Add( self.m_button3, 0, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer3, 1, wx.EXPAND, 5 )
		
		sbSizer4 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"安装手机软件(APP)" ), wx.VERTICAL )
		
		self.m_filePicker1 = wx.FilePickerCtrl( sbSizer4.GetStaticBox(), wx.ID_ANY, wx.EmptyString, u"选择需要安装的APP", u"*.*", wx.DefaultPosition, wx.DefaultSize, wx.FLP_DEFAULT_STYLE )
		sbSizer4.Add( self.m_filePicker1, 0, wx.ALL|wx.EXPAND, 5 )
		
		self.m_button4 = wx.Button( sbSizer4.GetStaticBox(), wx.ID_ANY, u"安装手机APP", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer4.Add( self.m_button4, 0, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer4, 1, wx.EXPAND, 5 )
		
		sbSizer5 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"卸载手机软件(APP)" ), wx.VERTICAL )
		
		self.m_textCtrl1 = wx.TextCtrl( sbSizer5.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer5.Add( self.m_textCtrl1, 0, wx.ALL|wx.EXPAND, 5 )
		
		self.m_button5 = wx.Button( sbSizer5.GetStaticBox(), wx.ID_ANY, u"卸载手机APP", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer5.Add( self.m_button5, 0, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer5, 1, wx.EXPAND, 5 )
		
		
		self.SetSizer( bSizer1 )
		self.Layout()
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.m_button1.Bind( wx.EVT_BUTTON, self.lookallthe_apps0 )
		self.m_button2.Bind( wx.EVT_BUTTON, self.lookrunning_apps0 )
		self.m_button3.Bind( wx.EVT_BUTTON, self.close_apps0 )
		self.m_button4.Bind( wx.EVT_BUTTON, self.install_apps0 )
		self.m_button5.Bind( wx.EVT_BUTTON, self.delete_apps0 )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def lookallthe_apps0( self, event ):
		event.Skip()
	
	def lookrunning_apps0( self, event ):
		event.Skip()
	
	def close_apps0( self, event ):
		event.Skip()
	
	def install_apps0( self, event ):
		event.Skip()
	
	def delete_apps0( self, event ):
		event.Skip()
	

