# Embedded file name: C:\Users\zym\Desktop\SkyEye\opt/skyeye/bin/skyeye_mach.py
import wx
import os
import se_path
import operator
MACHINE_BORDER = 2


def add_machine_desc(win, tsizer, desc, value):
	tsizer.SetRows(tsizer.GetRows() + 1)
	txt = wx.StaticText(win, wx.ID_ANY, desc + ':')
	font = txt.GetFont()
	font.SetWeight(wx.FONTWEIGHT_BOLD)
	txt.SetFont(font)
	tsizer.Add(txt, flag=wx.ALL, border=2)
	txt = wx.StaticText(win, wx.ID_ANY, value)
	tsizer.Add(txt, flag=wx.ALL, border=2)


def get_bitmap(name, l, h):
	image = wx.Image(name)
	image.Rescale(l, h)
	return wx.Bitmap(image)


class BackgroundPanel(wx.Panel):

	def __init__(self, parent, id, background_color, style):
		wx.Panel.__init__(self, parent, id, style=style)
		self.bg_color = background_color
		self.bg_pen = wx.Pen(self.bg_color)
		self.bg_brush = wx.Brush(self.bg_color)
		self.SetBackgroundColour(self.bg_color)
		self.Bind(wx.EVT_PAINT, self.OnPaint)

	def OnPaint(self, evt):
		dc = wx.BufferedPaintDC(self)
		sx, sy = dc.GetSize()
		dc.SetPen(self.bg_pen)
		dc.SetBrush(self.bg_brush)
		dc.DrawRectangle(0, 0, sx, sy)


class MachInfo():
	iconame = se_path.GetPicPath('logo.png')
	machname = se_path.GetPicPath('soc.png')
	def __init__(self, parent):
		self.info_list = []
		self.default_info = [u'\u672a\u52a0\u8f7d\u6838\u5fc3\u677f', self.iconame, [[u'\u6838\u5fc3\u677f', ''],
		  [u'\u5904\u7406\u5668', ''],
		  [u'\u5185\u5b58', ''],
		  [u'\u7f51\u7edc', ''],
		  [u'\u50a8\u5b58', '']]]

	def add_machine_info(self, sys, pro, mem, eth, sto):
		info_head = ['', self.machname]
		info_body = [[[u'\u6838\u5fc3\u677f', sys]] + [[u'\u5904\u7406\u5668', cpu] for cpu in pro] +\
			[[u'\u5185\u5b58', mem],[u'\u7f51\u7edc', eth],[u'\u50a8\u5b58', sto]]]
		self.info_list.append(info_head + info_body)

	def get_info(self):
		if len(self.info_list) == 0:
			self.info_list.append(self.default_info)
			return self.info_list
		else:
			return self.info_list

	def OnPaint(self, evt):
		dc = wx.BufferedPaintDC(self)
		sx, sy = dc.GetSize()
		dc.SetPen(self.bg_pen)
		dc.SetBrush(self.bg_brush)
		dc.DrawRectangle(0, 0, sx, sy)


def create_machine_view(win, info):
	desc, icon, props = info
	sizer = wx.BoxSizer(wx.VERTICAL)
	panel = BackgroundPanel(win, wx.ID_ANY, wx.TheColourDatabase.FindColour('WHITE'), wx.SIMPLE_BORDER)
	psizer = wx.BoxSizer(wx.VERTICAL)
	machname = wx.StaticText(panel, wx.ID_ANY, desc)
	psizer.Add(machname, flag=wx.LEFT, border=12)
	msizer = wx.BoxSizer(wx.HORIZONTAL)
	picname = os.path.basename(icon)
	if operator.eq(picname, 'logo.png') == True:
		bm = wx.StaticBitmap(panel, wx.ID_ANY, get_bitmap(icon, 150, 100))
	else:
		bm = wx.StaticBitmap(panel, wx.ID_ANY, get_bitmap(icon, 200, 150))
	msizer.Add(bm, flag=wx.ALIGN_CENTER)
	tsizer = wx.FlexGridSizer(0, 2,0,0)
	for key, value in props:
		add_machine_desc(panel, tsizer, key, value)
	msizer.Add(tsizer, proportion=1, flag=wx.EXPAND)
	dummysizer = wx.BoxSizer(wx.HORIZONTAL)
	psizer.Add(msizer)
	psizer.Add(dummysizer, proportion=1, flag=wx.EXPAND)
	panel.SetSizerAndFit(psizer)
	sizer.Add(panel, proportion=1, flag=wx.EXPAND)
	return sizer


class MachineWindow(wx.ScrolledWindow):

	def __init__(self, parent, extractor):
		wx.ScrolledWindow.__init__(self, parent, wx.ID_ANY, style=wx.SUNKEN_BORDER)
		self.SetBackgroundColour('WHITE')
		self.SetScrollRate(0, 20)
		self.SetScrollbars(1, 1, 400, 300)
		self.msizer = wx.BoxSizer(wx.VERTICAL)
		for j in extractor:
			self.msizer.Add(create_machine_view(self, j), proportion=1, flag=wx.EXPAND | wx.ALL, border=MACHINE_BORDER)
		self.SetSizer(self.msizer)

	def Clear(self):
		self.msizer.Hide(0)
		self.msizer.Remove(0)

	def Add(self, extract):
		count = 0
		for j in extract:
			self.msizer.Add(create_machine_view(self, j), proportion=1, flag=wx.EXPAND | wx.ALL, border=MACHINE_BORDER)
			self.msizer.Show(0)
			count = count + 1
'''

class ScFrame(wx.Frame):
	scrolled = None

	def __init__(self):
		wx.Frame.__init__(self, None, -1, 'Scrollbar Example', size=(450, 300))
		machine_info = machinfo(self)
		machine_info.add_machine_info('AM335X', ['cortex'], '', '', '')
		machine_info.add_machine_info('AM335X', ['cortex'], '', '', '')
		machine_info.add_machine_info('AM335X', ['cortex'], '', '', '')
		info = machine_info.get_info()
		self.scrolled = machine_window(self, info)
		return


if __name__ == '__main__':
	app = wx.PySimpleApp()
	frame = ScFrame()
	frame.Show()
	app.MainLoop()
'''