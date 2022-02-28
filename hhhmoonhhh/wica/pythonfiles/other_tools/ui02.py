# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version Jun 17 2015)
## http://www.wxformbuilder.org/
##
## PLEASE DO "NOT" EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc
import wx.adv

###########################################################################
## Class MyFrame1
###########################################################################

class MyFrame000001 ( wx.Frame ):
	
	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"其他帮助", pos = wx.DefaultPosition, size = wx.Size( 310,354 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
		self.SetSizeHintsSz( wx.DefaultSize, wx.DefaultSize )
		
		bSizer1 = wx.BoxSizer( wx.VERTICAL )
		
		sbSizer1 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"GitHub" ), wx.VERTICAL )
		
		self.m_hyperlink1 = wx.adv.HyperlinkCtrl( sbSizer1.GetStaticBox(), wx.ID_ANY, u"https://github.com/Huyuxuan-github", u"https://github.com/Huyuxuan-github", wx.DefaultPosition, wx.DefaultSize)#, wx.HL_DEFAULT_STYLE )
		sbSizer1.Add( self.m_hyperlink1, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer1, 1, wx.EXPAND, 5 )
		
		sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"Gitee" ), wx.VERTICAL )
		
		self.m_hyperlink2 = wx.adv.HyperlinkCtrl( sbSizer2.GetStaticBox(), wx.ID_ANY, u"https://gitee.com/hhhmoonhhh", u"https://gitee.com/hhhmoonhhh", wx.DefaultPosition, wx.DefaultSize)#, wx.HL_DEFAULT_STYLE )
		sbSizer2.Add( self.m_hyperlink2, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer2, 1, wx.EXPAND, 5 )
		
		sbSizer3 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"CSDN" ), wx.VERTICAL )
		
		self.m_hyperlink3 = wx.adv.HyperlinkCtrl( sbSizer3.GetStaticBox(), wx.ID_ANY, u"https://hyxmoon.blog.csdn.net/", u"https://hyxmoon.blog.csdn.net/", wx.DefaultPosition, wx.DefaultSize)#, wx.HL_DEFAULT_STYLE )
		sbSizer3.Add( self.m_hyperlink3, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer3, 1, wx.EXPAND, 5 )
		
		sbSizer4 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"JueJin" ), wx.VERTICAL )
		
		self.m_hyperlink4 = wx.adv.HyperlinkCtrl( sbSizer4.GetStaticBox(), wx.ID_ANY, u"https://juejin.cn/user/3510117404522061", u"https://juejin.cn/user/3510117404522061", wx.DefaultPosition, wx.DefaultSize)#, wx.HL_DEFAULT_STYLE )
		sbSizer4.Add( self.m_hyperlink4, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer4, 1, wx.EXPAND, 5 )
		
		sbSizer5 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"Twitter" ), wx.VERTICAL )
		
		self.m_hyperlink5 = wx.adv.HyperlinkCtrl( sbSizer5.GetStaticBox(), wx.ID_ANY, u"https://twitter.com/5kmU0XqYVqLXwru", u"https://twitter.com/5kmU0XqYVqLXwru", wx.DefaultPosition, wx.DefaultSize)#, wx.HL_DEFAULT_STYLE )
		sbSizer5.Add( self.m_hyperlink5, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer5, 1, wx.EXPAND, 5 )
		
		
		self.SetSizer( bSizer1 )
		self.Layout()
		
		self.Centre( wx.BOTH )
	
	def __del__( self ):
		pass
	

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
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"其他帮助", pos = wx.DefaultPosition, size = wx.Size( 455,450 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
		self.SetSizeHintsSz( wx.DefaultSize, wx.DefaultSize )
		
		bSizer1 = wx.BoxSizer( wx.VERTICAL )
		
		sbSizer1 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"GitHub" ), wx.VERTICAL )
		
		self.m_hyperlink1 = wx.HyperlinkCtrl( sbSizer1.GetStaticBox(), wx.ID_ANY, u"https://github.com/Huyuxuan-github", u"https://github.com/Huyuxuan-github", wx.DefaultPosition, wx.DefaultSize, wx.HL_DEFAULT_STYLE )
		sbSizer1.Add( self.m_hyperlink1, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer1, 1, wx.EXPAND, 5 )
		
		sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"Gitee" ), wx.VERTICAL )
		
		self.m_hyperlink2 = wx.HyperlinkCtrl( sbSizer2.GetStaticBox(), wx.ID_ANY, u"https://gitee.com/hhhmoonhhh", u"https://gitee.com/hhhmoonhhh", wx.DefaultPosition, wx.DefaultSize, wx.HL_DEFAULT_STYLE )
		sbSizer2.Add( self.m_hyperlink2, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer2, 1, wx.EXPAND, 5 )
		
		sbSizer3 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"CSDN" ), wx.VERTICAL )
		
		self.m_hyperlink3 = wx.HyperlinkCtrl( sbSizer3.GetStaticBox(), wx.ID_ANY, u"https://hyxmoon.blog.csdn.net/", u"https://hyxmoon.blog.csdn.net/", wx.DefaultPosition, wx.DefaultSize, wx.HL_DEFAULT_STYLE )
		sbSizer3.Add( self.m_hyperlink3, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer3, 1, wx.EXPAND, 5 )
		
		sbSizer4 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"JueJin" ), wx.VERTICAL )
		
		self.m_hyperlink4 = wx.HyperlinkCtrl( sbSizer4.GetStaticBox(), wx.ID_ANY, u"https://juejin.cn/user/3510117404522061", u"https://juejin.cn/user/3510117404522061", wx.DefaultPosition, wx.DefaultSize, wx.HL_DEFAULT_STYLE )
		sbSizer4.Add( self.m_hyperlink4, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer4, 1, wx.EXPAND, 5 )
		
		sbSizer5 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"Twitter" ), wx.VERTICAL )
		
		self.m_hyperlink5 = wx.HyperlinkCtrl( sbSizer5.GetStaticBox(), wx.ID_ANY, u"https://twitter.com/5kmU0XqYVqLXwru", u"https://twitter.com/5kmU0XqYVqLXwru", wx.DefaultPosition, wx.DefaultSize, wx.HL_DEFAULT_STYLE )
		sbSizer5.Add( self.m_hyperlink5, 0, wx.ALL, 5 )
		
		
		bSizer1.Add( sbSizer5, 1, wx.EXPAND, 5 )
		
		
		self.SetSizer( bSizer1 )
		self.Layout()
		
		self.Centre( wx.BOTH )
	
	def __del__( self ):
		pass
	

# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version Jun 17 2015)
## http://www.wxformbuilder.org/
##
## PLEASE DO "NOT" EDIT THIS FILE!
###########################################################################
