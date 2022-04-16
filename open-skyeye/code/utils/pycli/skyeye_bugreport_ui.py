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
## Class BugReportFrame
###########################################################################

class BugReportFrame ( wx.Frame ):
	
	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"错误报告", pos = wx.DefaultPosition, size = wx.Size( 600,500 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
		#self.SetSizeHintsSz( wx.DefaultSize, wx.Size( -1,-1 ) )
		self.SetForegroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_BTNTEXT ) )
		self.SetBackgroundColour( wx.Colour( 236, 236, 236 ) )
		
		bSizer1 = wx.BoxSizer( wx.VERTICAL )
		
		bSizer2 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_bitmap1 = wx.StaticBitmap( self, wx.ID_ANY, wx.ArtProvider.GetBitmap( wx.ART_INFORMATION, wx.ART_MESSAGE_BOX ), wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer2.Add( self.m_bitmap1, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		self.m_staticText1 = wx.StaticText( self, wx.ID_ANY, u"SkyEye(天目)遇到错误，给您带来不便，我们深表歉意。", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText1.Wrap( -1 )
		bSizer2.Add( self.m_staticText1, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		
		bSizer1.Add( bSizer2, 0, wx.EXPAND, 5 )
		
		bSizer3 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_staticText2 = wx.StaticText( self, wx.ID_ANY, u"我们已经产生了相关的错误报告(报告中不包含您的任何隐私信息)，希望您发送此报告给我们以帮助改善SkyEye的质量。", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText2.Wrap( -1 )
		self.m_staticText2.SetMinSize( wx.Size( -1,40 ) )
		
		bSizer3.Add( self.m_staticText2, 1, wx.EXPAND|wx.TOP|wx.RIGHT|wx.LEFT, 5 )
		
		
		bSizer1.Add( bSizer3, 0, wx.EXPAND, 5 )
		
		bSizer4 = wx.BoxSizer( wx.VERTICAL )
		
		bSizer5 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_staticText3 = wx.StaticText( self, wx.ID_ANY, u"报告路径:", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText3.Wrap( -1 )
		bSizer5.Add( self.m_staticText3, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		self.m_staticText_path = wx.StaticText( self, wx.ID_ANY, u"C:/SkyEye/temp", wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_LEFT )
		self.m_staticText_path.Wrap( -1 )
		bSizer5.Add( self.m_staticText_path, 1, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5 )
		
		self.m_button_open = wx.Button( self, wx.ID_ANY, u"打开目录", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer5.Add( self.m_button_open, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		
		bSizer4.Add( bSizer5, 0, wx.EXPAND, 5 )
		
		self.m_textCtrl_Log = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_MULTILINE|wx.TE_READONLY )
		bSizer4.Add( self.m_textCtrl_Log, 5, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( bSizer4, 1, wx.EXPAND, 5 )
		
		bSizer6 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_checkBox_restart = wx.CheckBox( self, wx.ID_ANY, u"重启SkyEye", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer6.Add( self.m_checkBox_restart, 1, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		self.m_button_ok = wx.Button( self, wx.ID_ANY, u"确定", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer6.Add( self.m_button_ok, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		
		bSizer1.Add( bSizer6, 0, wx.EXPAND, 5 )
		
		
		self.SetSizer( bSizer1 )
		self.Layout()
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.m_button_open.Bind( wx.EVT_BUTTON, self.OpenLogFileDir )
		self.m_button_ok.Bind( wx.EVT_BUTTON, self.ButtonOK )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def OpenLogFileDir( self, event ):
		event.Skip()
	
	def ButtonOK( self, event ):
		event.Skip()
	

