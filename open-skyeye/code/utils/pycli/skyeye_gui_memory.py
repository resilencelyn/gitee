import wx
import re
import inspect
import operator
import platform
import se_path, conf
from skyeye_common_module import *

def is_windows():
	os_info = platform.system()
	if operator.eq(os_info, "Linux") == True:
		return False
	else:
		return True
################################--win_utils--##########################
def get_default_tt_font(win):
	if is_windows():
		face = 'Courier New'
	else:
		face = 'Monospace'
	return wx.Font(win.GetFont().GetPointSize(), wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=face)

################################--memory_modols--##########################
class SpecialByteValue(object):

	def __init__(self, hex_str, char_str, description):
		self.hex_str = hex_str
		self.char_str = char_str
		self.description = description

	def asHex(self):
		return self.hex_str

	def asChar(self):
		return self.char_str

	def isSpecial(self):
		return True

	def getDescription(self):
		return self.description


class memory(object):

	def __init__(self):
		self.cpu = None

	def get_bytes(self,addr_array):
		return [self.memChar(addr) for addr in addr_array]
		
	def memChar(self, addr): 
		ch =  SkyEyeReadMemory8(self.cpu, addr)
		return ch

	def set_cpu(self, cpuname):
		self.cpu = cpuname

noTranslationValue = SpecialByteValue('--', '.', "无法翻译")
outsideMemoryValue = SpecialByteValue('**', '.', "溢出内存")
noInquiryValue = SpecialByteValue('??', '.', "未知")
specialByteValues = [noTranslationValue, outsideMemoryValue, noInquiryValue]

printable = range(32, 127)
def char(byte):
	if byte in printable:
		return chr(byte)
	return '.'

class ByteValue(object):

	def __init__(self, value):
		self.value = value

	def asHex(self):
		return '%02x' % self.value

	def asChar(self):
		return char(self.value)

	def getValue(self):
		return self.value

	def isSpecial(self):
		return False

###################################################################

def number_str(val, radix = None):
	return "0x%x" % (val)


def str_number(s):
	try:
		return cli.str_number(s)
	except cli_impl.CliSyntaxError as e:
		raise ValueError(s)


def my_hex(width, val):
	return '%%0%dx' % width % val


def convert_to_teletype_font(ctrl):
	ctrl.SetFont(get_default_tt_font(ctrl))


def sign(v):
	return int(v < 0) * -1 + int(v > 0) * 1


def any_special(bytes):
	for b in bytes:
		if b.isSpecial():
			return True

	return False


class HexField(wx.TextCtrl):
	"""A text control which selects the entire control content on double
	click. Use this for fields containing hex values since it is very
	annoying that double clicking doesn't select past the x in hex values.
	"""

	def __init__(self, *a, **kw):
		wx.TextCtrl.__init__(self, *a, **kw)
		self.Bind(wx.EVT_LEFT_DCLICK, self.onDoubleClick)

	def onDoubleClick(self, event):
		self.SetSelection(-1, -1)


class HiResScrollBar(wx.ScrollBar):
	"""Scrollbar capable of keeping track of more than 2**31-1 elements.
	"""
	LOW_RES_PAGE_SIZE = 2
	SIZE_MULTIPLIER = 10

	def __init__(self, parent, scrolledView, *a, **kw):
		wx.ScrollBar.__init__(self, parent, *a, **kw)
		self.scrolledView = scrolledView
		self.position = self.GetThumbPosition()
		self.range = max(1, self.GetRange())
		self.thumbSize = self.GetThumbSize()
		self.pageSize = self.GetPageSize()
		self.Bind(wx.EVT_SCROLL, self.onScroll)
		self.inProgramaticUpdate = False

	def setScrolledView(self, scrolledView):
		self.scrolledView = scrolledView

	def lowResToHiRes(self, v):
		return v * self.range // self.GetRange()

	def hiResToLowRes(self, v):
		return v * self.GetRange() // self.range

	def updateState(self):
		try:
			self.inProgramaticUpdate = True
			self.SetScrollbar(self.getLowResPosition(), self.getLowResThumbSize(), self.GetRange(), self.LOW_RES_PAGE_SIZE, True)
			self.oldLowResPosition = self.GetThumbPosition()
			if self.scrolledView:
				self.scrolledView.doScroll()
		finally:
			self.inProgramaticUpdate = False

	def getLowResPosition(self):
		if self.range <= self.thumbSize:
			return 0
		return min(self.GetRange() - self.SIZE_MULTIPLIER - self.getLowResThumbSize(), max(self.SIZE_MULTIPLIER, self.position * (self.GetRange() - self.getLowResThumbSize()) // (self.range - self.thumbSize)))

	def getLowResThumbSize(self):
		return self.hiResToLowRes(self.getThumbSize())

	def constrainPosition(self):
		self.position = max(0, min(self.position, self.range - self.thumbSize))

	def getPageSize(self):
		return self.pageSize

	def setPageSize(self, ps):
		self.pageSize = ps

	def getRange(self):
		return self.range

	def setRange(self, r):
		self.range = max(1, r)
		self.constrainPosition()
		self.updateState()

	def getPosition(self):
		return self.position

	def setPosition(self, p):
		self.position = p
		self.constrainPosition()
		self.updateState()

	def getThumbSize(self):
		return self.thumbSize

	def setThumbSize(self, t):
		self.thumbSize = t
		self.constrainPosition()
		self.updateState()

	def scrollBy(self, delta):
		self.setPosition(self.getPosition() + delta)

	def scrollTo(self, pos):
		self.setPosition(pos)

	def lineUp(self):
		self.scrollBy(-1)

	def lineDown(self):
		self.scrollBy(1)

	def pageUp(self):
		self.scrollBy(-self.getPageSize())

	def pageDown(self):
		self.scrollBy(self.getPageSize())

	def onScroll(self, event):
		if self.inProgramaticUpdate:
			return
		change = event.GetPosition() - self.oldLowResPosition
		if change == 0:
			return
		if abs(change) == 1:
			self.scrollBy(change)
		elif abs(change) == self.LOW_RES_PAGE_SIZE:
			self.scrollBy(sign(change) * self.getPageSize())
		else:
			self.setPosition(self.lowResToHiRes(event.GetPosition()))

	def setScrollSize(self, newSize):
		"""Update the scrollbars knowledge about its own size.
		You must call this before the user can generate scrollbar events and
		after every change in size.
		"""
		self.SetScrollbar(0, 0, newSize * self.SIZE_MULTIPLIER, self.LOW_RES_PAGE_SIZE, False)
		self.updateState()


class ColumnView(wx.Window):

	def __init__(self, parent, dataSource, *a, **kw):
		wx.Window.__init__(self, parent, *a, **kw)
		self.setDataSource(dataSource)
		convert_to_teletype_font(self)
		self.SetMinSize((self.getWidth(), self.getLineHeight() * 16))
		self.SetMaxSize((self.getWidth(), 100000))
		self.Bind(wx.EVT_PAINT, self.OnPaint)
		self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)

	def setDataSource(self, dataSource):
		self.dataSource = dataSource

	def getDataSource(self):
		return self.dataSource

	def getStartLine(self):
		return self.GetParent().getStartLine()

	def getLineHeight(self):
		return self.GetParent().getLineHeight()

	def getVisibleRange(self):
		w, h = self.GetClientSize()
		start = self.getStartLine()
		stop = min(start + h // self.getLineHeight() + 1, self.getDataSource().getNumberOfLines())
		return (start, stop)

	def getVisibleLines(self):
		start, stop = self.getVisibleRange()
		return stop - start

	def OnPaint(self, evt):
		dc = wx.BufferedPaintDC(self)
		rect = self.GetUpdateRegion().GetBox()
		#dc.SetClippingRect(rect)
		dc.SetClippingRegion(rect)
		dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
		dc.Clear()
		#self.DoPrepareDC(dc)
		self.Draw(dc)

	def OnEraseBackground(self, evt):
		pass

	def GetCharWidth(self):
		"""GetCharWidth in wx.Window is wrong, this one actually returns the
		width of the characters.
		"""
		dc = wx.ClientDC(self)
		dc.SetFont(self.GetFont())
		w, h = self.GetTextExtent('X')
		return w


class AddressView(ColumnView):

	def getNumberOfChars(self):
		return 16

	def getWidth(self):
		return self.getNumberOfChars() * self.GetCharWidth()

	def Draw(self, dc):
		fg = wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOWTEXT)
		dc.SetTextForeground(fg)
		dc.SetFont(self.GetFont())
		start, stop = self.getVisibleRange()
		addressChars = len(my_hex(0, self.getDataSource().getNumberOfBytes() - 1))
		x = self.getWidth() - self.GetCharWidth() * addressChars
		for y_idx, line in enumerate(range(start, stop)):
			y = y_idx * self.getLineHeight()
			address = my_hex(addressChars, self.getDataSource().getLineAddress(line))
			dc.DrawText(address, x, y)


class MyTimer(wx.Timer):

	def __init__(self, callback):
		wx.Timer.__init__(self)
		self.callback = callback

	def Notify(self):
		self.callback()


class DataView(ColumnView):
	OUTSIDE_SCROLL_INTERVAL = 100

	def __init__(self, *a, **kw):
		ColumnView.__init__(self, *a, **kw)
		self.selectionStart = None
		self.selectionEnd = None
		self.outsideScrollTimer = MyTimer(self.outsideScroll)
		self.SetBackgroundColour(wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOW))
		self.Bind(wx.EVT_LEFT_DOWN, self.onLeftDown)
		self.Bind(wx.EVT_LEFT_UP, self.onLeftUp)
		self.Bind(wx.EVT_MOTION, self.onMotion)
		return

	def getWidth(self):
		return self.getCellWidth() * self.getDataSource().getLineSize()

	def onLeftDown(self, event):
		self.CaptureMouse()
		if event.ShiftDown():
			self.selectionStart = self.getDataSource().getSelectedRange()[0]
			self.selectionEnd = self.addressAtPoint(event.GetPosition())
			self.updateSelection(self.selectionStart, self.selectionEnd)
		else:
			self.selectionStart = self.addressAtPoint(event.GetPosition())
		self.SetFocus()

	def onMotion(self, event):
		if self.selectionStart is None:
			event.Skip()
			return
		else:
			self.updateSelectionEnd(self.addressAtPoint(event.GetPosition()))
			self.lastMousePosition = event.GetPosition()
			self.scrollIfOutside(event)
			return

	def scrollIfOutside(self, event):
		x, y = event.GetPosition()
		w, h = self.GetSize()
		if y >= 0 and y < h:
			self.stopOutsideScrolling()
		elif y < 0:
			self.startOutsideScrolling(y)
		elif y >= h:
			self.startOutsideScrolling(y - h + 1)

	def startOutsideScrolling(self, speed):
		self.outsideScrollSpeed = speed
		if self.outsideScrollTimer.IsRunning():
			return
		self.outsideScrollTimer.Start(self.OUTSIDE_SCROLL_INTERVAL)

	def stopOutsideScrolling(self):
		self.outsideScrollSpeed = 0
		self.outsideScrollTimer.Stop()

	def outsideScroll(self):
		self.GetParent().scrollBy(self.outsideScrollSpeed)
		self.updateSelectionEnd(self.addressAtPoint(self.lastMousePosition))

	def updateSelection(self, start, end):
		self.getDataSource().setSelectedRange((start, end))

	def updateSelectionEnd(self, end):
		self.selectionEnd = end
		self.updateSelection(self.selectionStart, self.selectionEnd)

	def onLeftUp(self, event):
		if not self.HasCapture():
			return
		else:
			self.ReleaseMouse()
			if self.selectionStart is not None and self.selectionEnd is None:
				self.updateSelection(self.selectionStart, self.selectionStart)
			self.selectionStart = None
			self.selectionEnd = None
			self.stopOutsideScrolling()
			return

	def isCellSelected(self, row, col):
		return self.getDataSource().isAddressSelected(row * self.getDataSource().getLineSize() + col)

	def drawLineText(self, dc, y, bytes):
		s = self.getLineString(bytes)
		dc.DrawText(s, self.getLeftPadding(), y)

	def DrawLine(self, dc, y, line, bytes):
		lineStart, lineEnd = self.getDataSource().getLineRange(line)
		selStart, selEnd = self.getDataSource().getNormalizedSelectedRange()
		selFg = wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT)
		fg = wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOWTEXT)
		if selEnd <= lineStart or selStart >= lineEnd:
			dc.SetTextForeground(fg)
			self.drawLineText(dc, y, bytes)
		elif selStart <= lineStart and selEnd >= lineEnd:
			dc.DrawRectangle(0, y, self.getWidth(), self.getLineHeight())
			dc.SetTextForeground(selFg)
			self.drawLineText(dc, y, bytes)
		else:
			for col, b in enumerate(bytes):
				x = col * self.getCellWidth()
				if self.isCellSelected(line, col):
					dc.SetTextForeground(selFg)
					dc.DrawRectangle(x, y, self.getCellWidth(), self.getLineHeight())
				else:
					dc.SetTextForeground(fg)
				self.DrawByte(dc, x, y, b)

	def Draw(self, dc):
		dc.SetFont(self.GetFont())
		selBrush = wx.Brush(wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHT))
		dc.SetBrush(selBrush)
		dc.SetPen(wx.TRANSPARENT_PEN)
		start, stop = self.getVisibleRange()
		lines = self.getDataSource().getLines(start, stop)
		for y_idx, (line, bytes) in enumerate(zip(range(start, stop), lines)):
			y = y_idx * self.getLineHeight()
			self.DrawLine(dc, y, line, bytes)

	def addressAtPoint(self, pos):
		"""Find the address at pos. Addresses lies between bytes. Address 0 is
		before the first byte and address n is between bytes n-1 and n. There
		is also an address just beyond the last byte.
		"""
		x, y = pos
		ch = self.getLineHeight()
		cw = self.getCellWidth()
		ox, oy = self.GetPosition()
		row = max(0, min(y // ch, self.getVisibleLines() - 1))
		col = max(0, min((x + cw // 2) // cw, self.getDataSource().getLineSize()))
		addr = self.getDataSource().getLineAddress(self.getStartLine() + row) + col
		return min(addr, self.getDataSource().getNumberOfBytes())


class HexView(DataView):

	def getCellWidth(self):
		return 3 * self.GetCharWidth()

	def getByteString(self, byte):
		return byte.asHex()

	def getLineString(self, bytes):
		return ' '.join((self.getByteString(b) for b in bytes))

	def getLeftPadding(self):
		return self.GetCharWidth() // 2

	def DrawByte(self, dc, x, y, byte):
		dc.DrawText(self.getByteString(byte), x + self.GetCharWidth() // 2, y)


class AsciiView(DataView):

	def getCellWidth(self):
		return self.GetCharWidth()

	def getByteString(self, byte):
		return byte.asChar()

	def getLineString(self, bytes):
		return ''.join((self.getByteString(b) for b in bytes))

	def getLeftPadding(self):
		return 0

	def DrawByte(self, dc, x, y, byte):
		dc.DrawText(self.getByteString(byte), x, y)


class ListView(wx.Window):
	VIEW_CLASSES = [AddressView, HexView, AsciiView]

	def __init__(self, parent, dataSource, *a, **kw):
		wx.Window.__init__(self, parent, *a, **kw)
		convert_to_teletype_font(self)
		dc = wx.ClientDC(self)
		dc.SetFont(self.GetFont())
		w, h = dc.GetTextExtent('Wy')
		self.lineHeight = h
		self.dataSource = dataSource
		self.wheelAcc = 0
		sizer = wx.BoxSizer(wx.HORIZONTAL)
		self.views = map(lambda c: c(self, dataSource=dataSource), self.VIEW_CLASSES)
		for v in self.views:
			sizer.Add(v, flag=wx.EXPAND, proportion=0)

		self.scrollbar = HiResScrollBar(self, self, style=wx.SB_VERTICAL)
		sizer.Add(self.scrollbar, flag=wx.EXPAND)
		self.scrollbar.setThumbSize(self.getVisibleLines())
		self.scrollbar.setScrolledView(self)
		w, h = self.scrollbar.GetSize()
		self.scrollbar.setScrollSize(h)
		self.SetSizerAndFit(sizer)
		self.Layout()
		self.Bind(wx.EVT_SIZE, self.onSize)
		self.scrollbar.Bind(wx.EVT_SIZE, self.onScrollBarSize)
		self.scrollbar.Bind(wx.EVT_MOUSEWHEEL, self.onScrollWheel)
		self.Bind(wx.EVT_MOUSEWHEEL, self.onScrollWheel)
		for v in self.views:
			v.Bind(wx.EVT_KEY_DOWN, self.onKeyDown)

		self.Bind(wx.EVT_KEY_DOWN, self.onKeyDown)

	def updateView(self):
		self.scrollbar.setRange(self.dataSource.getNumberOfLines())
		self.Refresh()

	def getVisibleLines(self):
		w, h = self.GetSize()
		return h // self.getLineHeight()

	def setDataSource(self, dataSource):
		for v in self.views:
			v.setDataSource(dataSource)
		self.updateView()
		self.Refresh()

	def setStartLine(self, l):
		self.startLine = l

	def getStartLine(self):
		return self.startLine

	def getLineHeight(self):
		return self.lineHeight

	def onKeyDown(self, event):
		c = event.GetKeyCode()
		if c in [wx.WXK_DOWN, wx.WXK_NUMPAD_DOWN]:
			self.scrollLineDown()
		elif c in [wx.WXK_UP, wx.WXK_NUMPAD_UP]:
			self.scrollLineUp()
		elif c in [wx.WXK_PAGEDOWN,
			wx.WXK_NUMPAD_PAGEDOWN]:
			self.scrollPageDown()
		elif c in [wx.WXK_PAGEUP,
			wx.WXK_NUMPAD_PAGEUP]:
			self.scrollPageUp()
		elif c in [wx.WXK_HOME, wx.WXK_NUMPAD_HOME]:
			self.scrollTo(0)
		elif c in [wx.WXK_END, wx.WXK_NUMPAD_END]:
			self.scrollToEnd()
		else:
			event.Skip()

	def onSize(self, event):
		self.Layout()
		self.scrollbar.setThumbSize(self.getVisibleLines())
		self.scrollbar.setPageSize(self.getVisibleLines())

	def onScrollBarSize(self, event):
		w, h = event.GetSize()
		self.scrollbar.setScrollSize(h)

	def onScrollWheel(self, event):
		self.wheelAcc += event.GetWheelRotation()
		steps = self.wheelAcc // event.GetWheelDelta()
		self.wheelAcc = self.wheelAcc % event.GetWheelDelta()
		self.scrollBy(-steps)

	def scrollBy(self, delta):
		self.scrollbar.scrollBy(delta)

	def scrollTo(self, pos):
		self.scrollbar.scrollTo(pos)

	def scrollToEnd(self):
		self.scrollbar.scrollTo(self.scrollbar.getRange())

	def scrollLineUp(self):
		self.scrollbar.lineUp()

	def scrollLineDown(self):
		self.scrollbar.lineDown()

	def scrollPageUp(self):
		self.scrollbar.pageUp()

	def scrollPageDown(self):
		self.scrollbar.pageDown()

	def doScroll(self):
		self.setStartLine(self.scrollbar.getPosition())
		self.Refresh() 

class Inspector(wx.Window):
	SPECIAL_VALUE = ('<unknown>')
	TOO_LONG_VALUE = ('<too long selection>')
	TOO_LONG_THRESHOLD = 256

	def __init__(self, parent, dataSource, *a, **kw):
		wx.Window.__init__(self, parent, *a, **kw)
		self.dataSource = dataSource
		radioLabel = wx.StaticText(self, label=(''))
		beButton = wx.RadioButton(self, label="大端", style=wx.RB_GROUP)
		leButton = wx.RadioButton(self, label="小端")
		self.beSelected = True
		beButton.SetValue(self.beSelected)
		countLabel = wx.StaticText(self, label=self.getSelectionCountString())
		self.countLabel = countLabel
		valueLabel = wx.StaticText(self, label="被选中的值:")
		valueField = HexField(self, style=wx.TE_READONLY | wx.TE_RIGHT)
		self.valueField = valueField
		radioSizer = wx.BoxSizer(wx.VERTICAL)
		radioSizer.Add(radioLabel)
		radioSizer.Add(beButton)
		radioSizer.Add(leButton)
		sizer = wx.BoxSizer(wx.VERTICAL)
		sizer.Add(radioSizer)
		sizer.Add(valueLabel)
		sizer.Add(valueField, flag=wx.EXPAND)
		sizer.Add(countLabel, flag=wx.EXPAND)
		self.SetSizerAndFit(sizer)
		self.Layout()
		beButton.Bind(wx.EVT_RADIOBUTTON, self.onBeSelected)
		leButton.Bind(wx.EVT_RADIOBUTTON, self.onLeSelected)

	def getSelectionCountString(self):
		start, end = self.dataSource.getNormalizedSelectedRange()
		count = end - start
		if count == 0:
			return ("没有字节被选中")
		elif count == 1:
			return ("1 %s" % "个字节被选中")
		elif count >= 1000000000:
			return (">1000000000 %s" % "个字节被选中")
		else:
			return ("%d %s" % (count, "个字节被选中"))

	def onBeSelected(self, event):
		self.beSelected = True
		self.updateView()

	def onLeSelected(self, event):
		self.beSelected = False
		self.updateView()

	def updateView(self):
		self.updateValue()
		self.updateCountLabel()

	def updateValue(self):
		start, end = self.dataSource.getNormalizedSelectedRange()
		if end - start > self.TOO_LONG_THRESHOLD:
			self.valueField.SetValue(self.TOO_LONG_VALUE)
			return
		bytes = self.dataSource.getBytesForRange(start, end)
		if any_special(bytes):
			self.valueField.SetValue(self.SPECIAL_VALUE)
			return
		conv = [self.leStr, self.beStr][self.beSelected]
		self.valueField.SetValue(conv(bytes))

	def beStr(self, bytes):
		v = 0
		for b in bytes:
			v <<= 8
			v |= b.getValue()

		return number_str(v)

	def leStr(self, bytes):
		return self.beStr(list(reversed(bytes)))

	def updateCountLabel(self):
		self.countLabel.SetLabel(self.getSelectionCountString())


def fixedText(parent, text):
	r = wx.StaticText(parent, label=text)
	convert_to_teletype_font(r)
	return r


def proportionalText(parent, text):
	return wx.StaticText(parent, label=text)


class Controller(object):
	LINE_SIZE = 16
	ADDRESS_SPLITTER = re.compile(u'-+|\u2013')

	def __init__(self, window):
		self.window = window
		self.selectedRange = (0, 0)
		self.inAddressFieldChange = False
		self.memory = memory()
		self.buildGui()
		self.bindToViews()
		self.updateSpaceChoice()
		self.view.SetFocus()
		self.window.SetTitle("内存")
		iconame = se_path.GetPicPath("skyeye.ico")
		icon = wx.Icon(iconame, wx.BITMAP_TYPE_ICO)
		self.window.SetIcon(icon)
		self.mach_cpu = []

	def buildGui(self):
		panel = wx.Panel(self.window)
		self.spaceChoice = wx.Choice(panel)
		self.addressField = HexField(panel, wx.ID_ANY, style=wx.TE_PROCESS_ENTER)
		self.view = ListView(panel, dataSource=self)
		self.inspector = Inspector(panel, dataSource=self)
		legendSizer = wx.FlexGridSizer(3, 3, 0)
		for s in specialByteValues:
			t = fixedText(panel, s.asHex())
			legendSizer.Add(t)
			legendSizer.Add((t.GetCharWidth(), 0))
			legendSizer.Add(proportionalText(panel, s.getDescription()))

		rightSizer = wx.BoxSizer(wx.VERTICAL)
		rightSizer.Add(self.inspector)
		rightSizer.Add((0, 0), flag=wx.EXPAND, proportion=1)
		rightSizer.Add(legendSizer, flag=wx.EXPAND)
		topSizer = wx.BoxSizer(wx.HORIZONTAL)
		topSizer.Add(wx.StaticText(panel, label="处理器:"), flag=wx.ALIGN_CENTER_VERTICAL)
		topSizer.Add((4, 0))
		topSizer.Add(self.spaceChoice,proportion=1, flag=wx.ALIGN_CENTER_VERTICAL)
		topSizer.Add((8, 0))
		topSizer.Add(wx.StaticText(panel, label=("地址:")), flag=wx.ALIGN_CENTER_VERTICAL)
		topSizer.Add((4, 0))
		topSizer.Add(self.addressField, proportion=2, flag=wx.ALIGN_CENTER_VERTICAL)
		gridSizer = wx.FlexGridSizer(2, 2, 0)
		gridSizer.AddGrowableRow(1, 1)
		gridSizer.Add(topSizer, flag=wx.EXPAND | wx.ALL, border=5)
		gridSizer.Add((0, 0))
		gridSizer.Add(self.view, flag=wx.EXPAND)
		gridSizer.Add(rightSizer, flag=wx.EXPAND | wx.LEFT | wx.RIGHT, border=5)
		panel.SetSizerAndFit(gridSizer)
		winSizer = wx.BoxSizer(wx.HORIZONTAL)
		winSizer.Add(panel, flag=wx.EXPAND, proportion=1)
		winSizer.SetSizeHints(self.window)
		self.window.SetSizerAndFit(winSizer)
		w, h = self.window.GetSize()
		mw, mh = self.window.GetMaxSize()
		self.window.SetMaxSize((w, mh))

	def bindToViews(self):
		self.view.setDataSource(self)
		self.updateViews()
		self.addressField.Bind(wx.EVT_TEXT_ENTER, self.onAddressFieldChanged)
		self.spaceChoice.Bind(wx.EVT_CHOICE, self.onMemoryChoice)
		self.window.Bind(wx.EVT_KEY_DOWN, self.onKeyDown)

	def setMemoryList(self):
		self.updateSpaceChoice()

	def setMemory(self):
		self.scrollToSelection()
		self.updateViews()

	def updateViews(self):
		self.view.updateView()
		self.inspector.updateView()
		if not self.inAddressFieldChange:
			self.addressField.SetValue(self.getSelectedRangeAsText())

	def update_memory(self):
		index = self.spaceChoice.GetSelection()
		if (index != -1):
			mach_cpu = self.spaceChoice.GetString(index)
			array = mach_cpu.split('.')
			cpu = array[1]
			self.memory.set_cpu(cpu)

	
	def updateSpaceChoice(self):
		self.spaceChoice.Clear()
		config = conf.GetGlobalConfig()
		machlist = config.get_mach_list()
		list = []
		for mach in machlist:
			temp_list = []
			cpulist = config.get_cpulist_by_mach(mach)
			temp_list = [("%s.%s"%(mach, cpu), cpu) for cpu in cpulist]
			for item in temp_list:
				list.append(item)
		show_list = [cpu[0] for cpu in list]
		self.spaceChoice.AppendItems(show_list)
		self.spaceChoice.SetSelection(0)
		self.window.Layout()

	def getLineRange(self, l):
		low = l * self.LINE_SIZE
		return (low, low + self.LINE_SIZE)

	def scrollToSelection(self):
		l, h = self.getNormalizedSelectedRange()
		self.view.scrollTo(self.getLineFromAddress(l))

	def getBytesForRange(self, start, end):
		addr_array = [addr for addr in range(start, end)]
		self.update_memory()
		val_array = self.memory.get_bytes(addr_array)
		byts = [ByteValue(val) for val in val_array]
		return byts

	def setSelectedRange(self, r):
		if self.selectedRange == r:
			return
		self.selectedRange = r
		self.updateViews()

	def getSelectedRange(self):
		return self.selectedRange

	def getNormalizedSelectedRange(self):
		"""Return the current selection with the lowest address first.
		"""
		l, h = self.selectedRange
		if h < l:
			l, h = h, l
		return (l, h)

	def isAddressSelected(self, idx):
		l, h = self.getNormalizedSelectedRange()
		return idx >= l and idx < h

	def getNumberOfBytes(self):
		#return self.memory.getNumberOfBytes()
		return 0xffffffff

	def getLine(self, l):
		addr_array = [addr for addr in range(l * self.LINE_SIZE, self.LINE_SIZE + l * self.LINE_SIZE)]
		self.update_memory()
		val_array = self.memory.get_bytes(addr_array)
		line = [ByteValue(val) for val in val_array]
		return line 

	def getLines(self, start, stop):
		return [self.getLine(l) for l in range(start, stop) ]

	def getNumberOfLines(self):
		return (self.getNumberOfBytes() + self.LINE_SIZE - 1) // self.LINE_SIZE

	def getLineAddress(self, l):
		return l * self.LINE_SIZE

	def getLineSize(self):
		return self.LINE_SIZE

	def getLineFromAddress(self, address):
		return address // self.LINE_SIZE

	def getSelectedRangeAsText(self):
		l, h = self.getNormalizedSelectedRange()
		if l == h:
			return number_str(l, 16)
		else:
			return u'%s \u2013 %s' % tuple((number_str(v, 16) for v in (l, h)))

	def getAddressValueFromString(self, s):
		addr_str = str(s)
		if operator.eq(addr_str[:2], "0x") == True:
			addr = int(addr_str, 16)
		else:
			addr = int(addr_str)
		return addr

	def clampRange(self, r):
		return [ min(self.getNumberOfBytes(), e) for e in r ]

	def setAddressFromAddressField(self):
		txt = self.addressField.GetValue()
		parts = self.ADDRESS_SPLITTER.split(txt)
		try:
			if len(parts) == 1:
				r = self.getAddressValueFromString(parts[0])
				r = [r] * 2
			elif len(parts) == 2:
				r = [ self.getAddressValueFromString(p) for p in parts ]
			else:
				raise ValueError
			clamped = self.clampRange(r)
			self.setSelectedRange(clamped)
			self.scrollToSelection()
			if clamped != r:
				self.addressField.SetValue(self.getSelectedRangeAsText())
		except ValueError as e:
			self.addressField.SetValue(self.getSelectedRangeAsText())

	def onAddressFieldChanged(self, event):
		self.inAddressFieldChange = True
		try:
			self.setAddressFromAddressField()
		finally:
			self.inAddressFieldChange = False

		self.view.SetFocus()

	def onMemoryChoice(self, event):
		self.setMemory()

	def onKeyDown(self, event):
		return self.view.onKeyDown(event)
	

class MemFrame(wx.Frame):
	window_name = 'memory_view'
	def __init__(self, parent):
		wx.Frame.__init__(self, parent, wx.ID_ANY, '', style=wx.DEFAULT_FRAME_STYLE)
		self.par = parent
		self.controller = Controller(self)
		self.Layout()
		self.Bind(wx.EVT_CLOSE, self.on_close, self)

	def on_close(self, event):
		self.par.memory_frame = None
		self.Destroy()


if __name__ == '__main__':
	app = wx.PySimpleApp()
	Memdlg = MemFrame()
	Memdlg.Show()
	app.MainLoop()
