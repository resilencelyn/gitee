# -*- coding: UTF-8 -*-
import wx
import conf
import thread
from skyeye_common_module import *
def _(st):
	return unicode(st, "gb2312")
OBJ = ["gpio_light"]

class Light(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent = parent, style =  wx.RAISED_BORDER)
	self.SetBackgroundColour("#2f2f2f")
	self.open_flag = 0
    def ifOpen(self):
	    return self.open_flag
    def setValue(self, value):
	    if value == 0 and self.open_flag != 0:
		    self.Close()
		    self.Refresh()
	    elif value != 0 and self.open_flag == 0:
		    self.Open()
		    self.Refresh()
			    
    def Open(self):
	    self.open_flag = 1
	    self.SetBackgroundColour(wx.RED)

    def Close(self):
	    self.open_flag = 0
	    self.SetBackgroundColour("#2f2f2f")
class Frame(wx.Frame):
    def __init__(self, parent, args):
        wx.Frame.__init__(self, parent, wx.ID_ANY, title=_("GPIO显示器"), size = (500, 200), style=wx.DEFAULT_FRAME_STYLE)
	self.win = parent
	self.SetBackgroundColour(wx.WHITE)

	gpio_name = wx.StaticText(self, -1,  _("GPIO值"), style = wx.ALIGN_CENTER)
	self.gpio_value = wx.TextCtrl(self,style =wx.TE_RICH2 |wx.TE_CENTER|wx.TE_READONLY)
	
	sizer = wx.BoxSizer(wx.VERTICAL)
	box_sizer0 = wx.BoxSizer(wx.HORIZONTAL)  
	box_sizer0.Add(gpio_name, 1, flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 5)
	box_sizer0.Add(self.gpio_value, 2, flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 5)
	box_sizer0.Add((0, 0), 3, flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 5)
	static_box1= wx.StaticBox(self, -1, _("显示")) 
	box_sizer1 = wx.StaticBoxSizer(static_box1, wx.HORIZONTAL)  
	sizer.Add(box_sizer0, 1, flag = wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 5)
	sizer.Add(box_sizer1, 4, flag = wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 5)
	self.light_group = self.create_light_group(box_sizer1)
	self.SetSizer(sizer)
	self.Fit()
	self.SetSize((450, 220))
	self.win.register_thread_func(self, self.update)
	self.Bind(wx.EVT_CLOSE, self.Close, self)

    def remove(self):
        self.win.remove_thread_func(self)
        self.win.custom_close_self(self)
	self.Destroy()

    def Close(self, event):
	thread.start_new_thread(self.remove,())

    def update_light(self, value):
	    for i in range(0, 5):
		    v = (value >> i) & 1
		    self.light_group[i].setValue(v)
		    
    	
    def update(self):
	    config = conf.GetGlobalConfig()
	    if not config:
		    return 
	    value = SkyEyeReadDevice4(OBJ[0], 0)
	    if value == errormessage.DLLCALLERRORINFO:
		    value = 0
	    value = value & 0xffff
	    value_str = "0x%x" % value
	    gpio_value = self.gpio_value.GetValue()
	    if gpio_value != value_str:
	    	self.gpio_value.SetValue(value_str)
	    self.update_light(value)
	    

    def create_light_group(self, sizer):
	    name = [_("供电1"),_("供电2"),_("供电3"), _("供电4"), _("热电池")]
	    light_group = []
	    for i in range(0, 5):
		    gpio_sizer = wx.BoxSizer(wx.VERTICAL)
		    light = Light(self)
		    gpio_name = wx.StaticText(self, -1,  name[i], style = wx.ALIGN_CENTER)
		    gpio_sizer.Add(light, 2, flag = wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 5)
		    gpio_sizer.Add(gpio_name, 1, flag = wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 5)
		    sizer.Add(gpio_sizer, 1, flag = wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 10)
		    light_group.append(light)
	    return light_group
		    
		    
	    
FileMenu = _("GPIO显示器")
WinCls = Frame

if __name__ == '__main__':
	app = wx.PySimpleApp()
	Regsdlg = Frame()
	Regsdlg.Show()
	app.MainLoop()
