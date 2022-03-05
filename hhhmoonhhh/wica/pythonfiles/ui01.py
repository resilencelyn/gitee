

"""
use the baked ui01.py.............
"""




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

class MyFrame1 ( wx.Frame ):

    
	
	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"向手机发送文件", pos = wx.DefaultPosition, size = wx.Size( 400,450 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
            # icon = wx.Icon('lihf.ico', wx.BITMAP_TYPE_ICO)
            # self.SetIcon(icon)
        # self.icon = wx.Icon('lihf.ico', wx.BITMAP_TYPE_ICO)   
        # self.SetIcon(self.icon)  
        # self.icon = 

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )  

        
        # self.SetIcon
        # self.icon = wx.Icon('lihf.ico', wx.BITMAP_TYPE_ICO)
        # self.SetIcon(self.icon)       
        # self
        # icon =  
          
		
		bSizer1 = wx.BoxSizer( wx.VERTICAL )
		
		sbSizer3 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"向手机发送电脑中的文件" ), wx.VERTICAL )
		
		sbSizer1 = wx.StaticBoxSizer( wx.StaticBox( sbSizer3.GetStaticBox(), wx.ID_ANY, u"查看电脑中的文件" ), wx.VERTICAL )
		
		self.m_genericDirCtrl2 = wx.GenericDirCtrl( sbSizer1.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.DIRCTRL_3D_INTERNAL|wx.SUNKEN_BORDER, wx.EmptyString, 0 )
		
		self.m_genericDirCtrl2.ShowHidden( False )
		sbSizer1.Add( self.m_genericDirCtrl2, 1, wx.EXPAND |wx.ALL, 5 )
		
		
		sbSizer3.Add( sbSizer1, 1, wx.EXPAND, 5 )
		
		sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( sbSizer3.GetStaticBox(), wx.ID_ANY, u"选择电脑中的文件" ), wx.VERTICAL )
		
        # m_filePicker3 - > file to send.....
		self.m_filePicker3 = wx.FilePickerCtrl( sbSizer2.GetStaticBox(), wx.ID_ANY, wx.EmptyString, u"Select A File To Send.", u"*.*", wx.DefaultPosition, wx.Size( 750,86 ), wx.FLP_DEFAULT_STYLE )
		sbSizer2.Add( self.m_filePicker3, 0, wx.ALL|wx.EXPAND, 5 )
		
		
		# sbSizer2.AddSpacer( ( 0, 0), 1, wx.EXPAND, 5 )
		
		self.m_button1 = wx.Button( sbSizer2.GetStaticBox(), wx.ID_ANY, u"Send The File To The Phone", wx.DefaultPosition, wx.Size( 400,75 ), 0 )
		sbSizer2.Add( self.m_button1, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL, 5 )
		
		
		# sbSizer2.AddSpacer( ( 0, 0), 1, wx.EXPAND, 5 )
		
		
		sbSizer3.Add( sbSizer2, 1, wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer3, 1, wx.EXPAND, 5 )
		
		
		self.SetSizer( bSizer1 )
		self.Layout()
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.m_button1.Bind( wx.EVT_BUTTON, self.sendingthepcfiletophone )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def sendingthepcfiletophone( self, event ):
		event.Skip()
	








