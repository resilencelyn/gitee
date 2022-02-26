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
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"WICA", pos = wx.DefaultPosition, size = wx.Size( 762,614 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
		self.SetSizeHintsSz( wx.DefaultSize, wx.DefaultSize )
		
		fgSizer1 = wx.FlexGridSizer( 0, 4, 0, 0 )
		fgSizer1.SetFlexibleDirection( wx.BOTH )
		fgSizer1.SetNonFlexibleGrowMode( wx.FLEX_GROWMODE_SPECIFIED )
		
		bSizer1 = wx.BoxSizer( wx.VERTICAL )
		
		sbSizer1 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"手机与电脑连接" ), wx.VERTICAL )
		
		sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( sbSizer1.GetStaticBox(), wx.ID_ANY, u"手机的局域网IP地址" ), wx.VERTICAL )
		
		self.m_textCtrl1 = wx.TextCtrl( sbSizer2.GetStaticBox(), wx.ID_ANY, u"192.168.0.104", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer2.Add( self.m_textCtrl1, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		
		sbSizer1.Add( sbSizer2, 1, wx.EXPAND, 5 )
		
		sbSizer3 = wx.StaticBoxSizer( wx.StaticBox( sbSizer1.GetStaticBox(), wx.ID_ANY, u"手机的adb服务端口" ), wx.VERTICAL )
		
		self.m_spinCtrl1 = wx.SpinCtrl( sbSizer3.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.SP_ARROW_KEYS, 5555, 5585, 5555 )
		sbSizer3.Add( self.m_spinCtrl1, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		
		sbSizer1.Add( sbSizer3, 1, wx.EXPAND, 5 )
		
		self.m_button1 = wx.Button( sbSizer1.GetStaticBox(), wx.ID_ANY, u"确认连接", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer1.Add( self.m_button1, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer1, 0, 0, 5 )
		
		sbSizer4 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"手机配置ATX服务" ), wx.VERTICAL )
		
		self.m_button2 = wx.Button( sbSizer4.GetStaticBox(), wx.ID_ANY, u"检查ATX服务", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer4.Add( self.m_button2, 1, wx.ALL|wx.EXPAND, 5 )
		
		self.m_button3 = wx.Button( sbSizer4.GetStaticBox(), wx.ID_ANY, u"开启ATX服务", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer4.Add( self.m_button3, 1, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer4, 0, wx.EXPAND, 5 )
		
		sbSizer5 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"电脑操控手机" ), wx.VERTICAL )
		
		self.m_button4 = wx.Button( sbSizer5.GetStaticBox(), wx.ID_ANY, u"开始操控", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer5.Add( self.m_button4, 1, wx.ALL|wx.EXPAND, 5 )
		
		self.m_button6 = wx.Button( sbSizer5.GetStaticBox(), wx.ID_ANY, u"结束操控", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer5.Add( self.m_button6, 1, wx.ALL|wx.EXPAND, 5 )
		
		
		bSizer1.Add( sbSizer5, 1, wx.EXPAND, 5 )
		
		
		fgSizer1.Add( bSizer1, 1, wx.EXPAND, 5 )
		
		bSizer2 = wx.BoxSizer( wx.VERTICAL )
		
		sbSizer6 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"一些常用的手机操作" ), wx.VERTICAL )
		
		fgSizer2 = wx.FlexGridSizer( 0, 2, 0, 0 )
		fgSizer2.SetFlexibleDirection( wx.BOTH )
		fgSizer2.SetNonFlexibleGrowMode( wx.FLEX_GROWMODE_SPECIFIED )
		
		bSizer3 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_button5 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"亮屏", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button5, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button9 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"返回", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button9, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button10 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"通知栏", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button10, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button11 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"向左翻页", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button11, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button12 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"向前滑动", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button12, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button13 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"音量调高", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button13, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button14 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"设置静音", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button14, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button15 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"最近运行软件", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button15, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button16 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"正在运行进程", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button16, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button17 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"停止所有应用", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer3.Add( self.m_button17, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		
		fgSizer2.Add( bSizer3, 1, wx.EXPAND, 5 )
		
		bSizer4 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_button7 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"熄屏", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button7, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button18 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"HOME", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button18, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button19 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"快速设置", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button19, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button20 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"向右翻页", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button20, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button21 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"向后滑动", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button21, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button22 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"音量调低", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button22, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button23 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"点击电源键", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button23, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button24 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"检查进程守护", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button24, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button25 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"查看设置信息", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button25, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button8 = wx.Button( sbSizer6.GetStaticBox(), wx.ID_ANY, u"获取手机信息", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer4.Add( self.m_button8, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		
		fgSizer2.Add( bSizer4, 1, wx.EXPAND, 5 )
		
		
		sbSizer6.Add( fgSizer2, 1, wx.EXPAND, 5 )
		
		
		bSizer2.Add( sbSizer6, 0, wx.ALIGN_RIGHT, 5 )
		
		
		fgSizer1.Add( bSizer2, 1, wx.EXPAND, 5 )
		
		bSizer5 = wx.BoxSizer( wx.VERTICAL )
		
		sbSizer8 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"其他辅助功能" ), wx.VERTICAL )
		
		sbSizer7 = wx.StaticBoxSizer( wx.StaticBox( sbSizer8.GetStaticBox(), wx.ID_ANY, u"向手机文本框发送文本内容" ), wx.VERTICAL )
		
		self.m_textCtrl2 = wx.TextCtrl( sbSizer7.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer7.Add( self.m_textCtrl2, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		self.m_button26 = wx.Button( sbSizer7.GetStaticBox(), wx.ID_ANY, u"确认发送", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer7.Add( self.m_button26, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 5 )
		
		
		sbSizer8.Add( sbSizer7, 1, wx.EXPAND, 5 )
		
		sbSizer9 = wx.StaticBoxSizer( wx.StaticBox( sbSizer8.GetStaticBox(), wx.ID_ANY, u"手机功能扩展" ), wx.VERTICAL )
		
		sbSizer10 = wx.StaticBoxSizer( wx.StaticBox( sbSizer9.GetStaticBox(), wx.ID_ANY, u"查看手机文件" ), wx.VERTICAL )
		
		self.m_toggleBtn1 = wx.ToggleButton( sbSizer10.GetStaticBox(), wx.ID_ANY, u"点击开始（结束）", wx.DefaultPosition, wx.DefaultSize, 0 )
		sbSizer10.Add( self.m_toggleBtn1, 1, wx.ALL|wx.EXPAND|wx.ALIGN_CENTER_HORIZONTAL, 5 )
		
		
		sbSizer9.Add( sbSizer10, 1, wx.EXPAND, 5 )
		
		sbSizer11 = wx.StaticBoxSizer( wx.StaticBox( sbSizer9.GetStaticBox(), wx.ID_ANY, u"使用YOLO功能" ), wx.VERTICAL )
		
		self.m_toggleBtn2 = wx.ToggleButton( sbSizer11.GetStaticBox(), wx.ID_ANY, u"点击开始（结束）", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_toggleBtn2.SetValue( True ) 
		sbSizer11.Add( self.m_toggleBtn2, 1, wx.ALL|wx.EXPAND|wx.ALIGN_CENTER_HORIZONTAL, 5 )
		
		
		sbSizer9.Add( sbSizer11, 1, wx.EXPAND, 5 )
		
		
		sbSizer8.Add( sbSizer9, 1, wx.EXPAND, 5 )
		
		
		bSizer5.Add( sbSizer8, 1, wx.EXPAND, 5 )
		
		
		fgSizer1.Add( bSizer5, 1, wx.EXPAND, 5 )
		
		
		self.SetSizer( fgSizer1 )
		self.Layout()
		self.m_menubar1 = wx.MenuBar( 0 )
		self.m_menu1 = wx.Menu()
		self.m_menuItem2 = wx.MenuItem( self.m_menu1, wx.ID_ANY, u"软件说明", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu1.AppendItem( self.m_menuItem2 )
		
		self.m_menuItem7 = wx.MenuItem( self.m_menu1, wx.ID_ANY, u"使用说明", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu1.AppendItem( self.m_menuItem7 )
		
		self.m_menu11 = wx.Menu()
		self.m_menuItem1 = wx.MenuItem( self.m_menu11, wx.ID_ANY, u"获取其他帮助", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu11.AppendItem( self.m_menuItem1 )
		
		self.m_menu1.AppendSubMenu( self.m_menu11, u"帮助" )
		
		self.m_menubar1.Append( self.m_menu1, u"帮助" ) 
		
		self.m_menu2 = wx.Menu()
		self.m_menuItem3 = wx.MenuItem( self.m_menu2, wx.ID_ANY, u"传输文件", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu2.AppendItem( self.m_menuItem3 )
		
		self.m_menuItem6 = wx.MenuItem( self.m_menu2, wx.ID_ANY, u"手机APP管理", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu2.AppendItem( self.m_menuItem6 )
		
		self.m_menuItem91 = wx.MenuItem( self.m_menu2, wx.ID_ANY, u"查看手机进程", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu2.AppendItem( self.m_menuItem91 )
		
		self.m_menuItem9 = wx.MenuItem( self.m_menu2, wx.ID_ANY, u"自制音乐播放器", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu2.AppendItem( self.m_menuItem9 )
		
		self.m_menubar1.Append( self.m_menu2, u"工具" ) 
		
		self.m_menu3 = wx.Menu()
		self.m_menuItem4 = wx.MenuItem( self.m_menu3, wx.ID_ANY, u"作者信息", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu3.AppendItem( self.m_menuItem4 )
		
		self.m_menuItem5 = wx.MenuItem( self.m_menu3, wx.ID_ANY, u"赞助项目", wx.EmptyString, wx.ITEM_NORMAL )
		self.m_menu3.AppendItem( self.m_menuItem5 )
		
		self.m_menubar1.Append( self.m_menu3, u"联系作者" ) 
		
		self.SetMenuBar( self.m_menubar1 )
		
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.m_button1.Bind( wx.EVT_BUTTON, self.m01to_connectthe_cap )
		self.m_button2.Bind( wx.EVT_BUTTON, self.m02checkatx )
		self.m_button3.Bind( wx.EVT_BUTTON, self.m03startatx )
		self.m_button4.Bind( wx.EVT_BUTTON, self.m04startcontrol )
		self.m_button6.Bind( wx.EVT_BUTTON, self.m05endcontrol )
		self.m_button5.Bind( wx.EVT_BUTTON, self.m06openscreen )
		self.m_button9.Bind( wx.EVT_BUTTON, self.m07clickback )
		self.m_button10.Bind( wx.EVT_BUTTON, self.m08shownotifications )
		self.m_button11.Bind( wx.EVT_BUTTON, self.m09turnleft )
		self.m_button12.Bind( wx.EVT_BUTTON, self.m10turnforward )
		self.m_button13.Bind( wx.EVT_BUTTON, self.m11voiceup )
		self.m_button14.Bind( wx.EVT_BUTTON, self.m12novoice )
		self.m_button15.Bind( wx.EVT_BUTTON, self.m13recentapps )
		self.m_button16.Bind( wx.EVT_BUTTON, self.m14runprocess )
		self.m_button17.Bind( wx.EVT_BUTTON, self.m15stopallapps )
		self.m_button7.Bind( wx.EVT_BUTTON, self.m16closescreen )
		self.m_button18.Bind( wx.EVT_BUTTON, self.m17clickhome )
		self.m_button19.Bind( wx.EVT_BUTTON, self.m18quicksettings )
		self.m_button20.Bind( wx.EVT_BUTTON, self.m19turnright )
		self.m_button21.Bind( wx.EVT_BUTTON, self.m20turnback )
		self.m_button22.Bind( wx.EVT_BUTTON, self.m21voicedown )
		self.m_button23.Bind( wx.EVT_BUTTON, self.m22clickpower )
		self.m_button24.Bind( wx.EVT_BUTTON, self.m23healthprocessing )
		self.m_button25.Bind( wx.EVT_BUTTON, self.m24settingsmsg )
		self.m_button8.Bind( wx.EVT_BUTTON, self.m25phonemsg )
		self.m_button26.Bind( wx.EVT_BUTTON, self.m26sending_texts )
		self.m_toggleBtn1.Bind( wx.EVT_TOGGLEBUTTON, self.m27phonefileslooking )
		self.m_toggleBtn2.Bind( wx.EVT_TOGGLEBUTTON, self.m28usephoneyolov5 )
		self.Bind( wx.EVT_MENU, self.m29appintroductions, id = self.m_menuItem2.GetId() )
		self.Bind( wx.EVT_MENU, self.m30usingintroductions, id = self.m_menuItem7.GetId() )
		self.Bind( wx.EVT_MENU, self.m31helpings, id = self.m_menuItem1.GetId() )
		self.Bind( wx.EVT_MENU, self.m32sendingfiles, id = self.m_menuItem3.GetId() )
		self.Bind( wx.EVT_MENU, self.m33appsmanager, id = self.m_menuItem6.GetId() )
		self.Bind( wx.EVT_MENU, self.m34processinglooking, id = self.m_menuItem91.GetId() )
		self.Bind( wx.EVT_MENU, self.m35musicvideo, id = self.m_menuItem9.GetId() )
		self.Bind( wx.EVT_MENU, self.m36authormessage, id = self.m_menuItem4.GetId() )
		self.Bind( wx.EVT_MENU, self.m37moneytheproject, id = self.m_menuItem5.GetId() )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def m01to_connectthe_cap( self, event ):
		event.Skip()
	
	def m02checkatx( self, event ):
		event.Skip()
	
	def m03startatx( self, event ):
		event.Skip()
	
	def m04startcontrol( self, event ):
		event.Skip()
	
	def m05endcontrol( self, event ):
		event.Skip()
	
	def m06openscreen( self, event ):
		event.Skip()
	
	def m07clickback( self, event ):
		event.Skip()
	
	def m08shownotifications( self, event ):
		event.Skip()
	
	def m09turnleft( self, event ):
		event.Skip()
	
	def m10turnforward( self, event ):
		event.Skip()
	
	def m11voiceup( self, event ):
		event.Skip()
	
	def m12novoice( self, event ):
		event.Skip()
	
	def m13recentapps( self, event ):
		event.Skip()
	
	def m14runprocess( self, event ):
		event.Skip()
	
	def m15stopallapps( self, event ):
		event.Skip()
	
	def m16closescreen( self, event ):
		event.Skip()
	
	def m17clickhome( self, event ):
		event.Skip()
	
	def m18quicksettings( self, event ):
		event.Skip()
	
	def m19turnright( self, event ):
		event.Skip()
	
	def m20turnback( self, event ):
		event.Skip()
	
	def m21voicedown( self, event ):
		event.Skip()
	
	def m22clickpower( self, event ):
		event.Skip()
	
	def m23healthprocessing( self, event ):
		event.Skip()
	
	def m24settingsmsg( self, event ):
		event.Skip()
	
	def m25phonemsg( self, event ):
		event.Skip()
	
	def m26sending_texts( self, event ):
		event.Skip()
	
	def m27phonefileslooking( self, event ):
		event.Skip()
	
	def m28usephoneyolov5( self, event ):
		event.Skip()
	
	def m29appintroductions( self, event ):
		event.Skip()
	
	def m30usingintroductions( self, event ):
		event.Skip()
	
	def m31helpings( self, event ):
		event.Skip()
	
	def m32sendingfiles( self, event ):
		event.Skip()
	
	def m33appsmanager( self, event ):
		event.Skip()
	
	def m34processinglooking( self, event ):
		event.Skip()
	
	def m35musicvideo( self, event ):
		event.Skip()
	
	def m36authormessage( self, event ):
		event.Skip()
	
	def m37moneytheproject( self, event ):
		event.Skip()
	

# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version Jun 17 2015)
## http://www.wxformbuilder.org/
##
## PLEASE DO "NOT" EDIT THIS FILE!
###########################################################################
