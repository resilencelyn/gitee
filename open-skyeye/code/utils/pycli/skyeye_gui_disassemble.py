import string
import wx
import se_path
import conf
from conf import GetGlobalConfig
from skyeye_common_module import *
from sky_log import *
import cli
from common_dialog import *


class cpu():
	def __init__(self):
		self.name = "cpuname"

class disassembly_popup_menu(wx.Menu):

	def __init__(self, running, run_until):
		wx.Menu.__init__(self)
		item0 = wx.MenuItem(self, wx.NewId(), "插入断点并执行")
		self.Append(item0)
		item0.Enable(not running)
		self.Bind(wx.EVT_MENU, lambda x: run_until(True, x), item0)

class cpu_context_selector(object):

	def __init__(self, parent, sizer, cpu_change_func, cpu_list):
		self.cpus = set()
		self.cpu_name = None
		self.cpu_change_func = cpu_change_func
		self.cpu_list = cpu_list
		cbox = wx.StaticBox(parent, label=("处理器"))
		csizer = wx.StaticBoxSizer(cbox, wx.HORIZONTAL)
		sizer.Add(csizer, flag=wx.EXPAND | wx.ALL, border=2)
		self.choice = wx.Choice(parent, wx.ID_ANY, choices=[])
		self.choice.Bind(wx.EVT_CHOICE, self.cpu_choice)
		csizer.Add(self.choice, flag=wx.ALL, border=2)
		self.ctxt = wx.TextCtrl(parent, wx.ID_ANY, style=wx.TE_READONLY)
		csizer.Add(self.ctxt, flag=wx.ALIGN_CENTER_VERTICAL | wx.EXPAND | wx.ALL, border=2)
		self.ctxt.SetBackgroundColour(parent.GetBackgroundColour())
		self.ctxt.SetSizeHints(35 * parent.GetCharWidth(), -1)
		self.choice.SetSizeHints(30 * parent.GetCharWidth(), -1)
		wx.CallAfter(self.update_cpus)
		return

	def update(self, cpu):
		""" called by user when changing processor"""
		if cpu:
			self.cpu_name = cpu.name
			self.choice.SetStringSelection(cpu.name)
		self.set_context(cpu)

	def set_context(self, cpu):
		if cpu:
			print ("set_context")
		else:
			print ("set_context")
		return

	def set_ctx(self, cpu):
		self.ctxt.SetValue('')
		config = GetGlobalConfig()
		mach_name = config.get_cpu_mach(cpu)
		self.ctxt.SetValue("%s.%s" % (mach_name, cpu))

	def cpu_choice(self, event):
		cpu = event.GetString()
		self.cpu_change_func(cpu)
		self.set_ctx(cpu)

	def all_processors(self):
		#return simics.VT_get_all_objects_impl([simics.PROCESSOR_INFO_INTERFACE])
		return [cpu(), cpu(), cpu()]

	def update_cpus(self):
		self.choice.Clear()
		self.choice.SetItems(self.cpu_list)
		self.choice.SetStringSelection(self.cpu_list[0])
		self.cpu_change_func(self.cpu_list[0])
		self.set_ctx(self.cpu_list[0])

	def context_change(self, obj, cpu):
		if self.ctx_change_func:
			self.ctx_change_func(cpu)
		self.set_context(cpu)

	def objects_added(self, obj):
		self.update_cpus()

	def objects_deleted(self, obj):
		self.update_cpus()

	def object_renamed(self, obj, old_name):
		self.update_cpus()

class no_dclick_button(wx.Button):

	def __init__(self, parent, label, style, handler):
		wx.Button.__init__(self, parent, wx.ID_ANY, label, style=style)
		self.Bind(wx.EVT_BUTTON, handler)

def bp_handle_to_flags(handle):
	for bp in conf.sim.breakpoints:
		if handle in bp[12]:
			return bp[6]

	raise KeyError('No such breakpoint')


class addr_validator(wx.Validator):

	def __init__(self, parent):
		wx.Validator.__init__(self)
		self.Bind(wx.EVT_CHAR, self.char_input)
		self.parent = parent

	def Clone(self):
		return addr_validator(self.parent)

	def Validate(self, win):
		val = self.GetWindow().GetValue()
		for x in val:
			if x not in string.digits:
				return False

		return True

	def char_input(self, event):
		key = event.GetKeyCode()
		if key < wx.WXK_SPACE or key == wx.WXK_DELETE or key > 255:
			event.Skip()
			return

		val = self.GetWindow().GetValue()
		input_str = str(val)
		leng = len(input_str)
		if leng >= 10:
			return 
		if input_str == '' and chr(key) not in string.digits:
			return 
		if leng == 1 and  (chr(key) != 'x' and chr(key)  in string.ascii_letters):
			return 
		if leng >= 2 and  (val.startswith('0x') and chr(key)  not in string.hexdigits):
			return 
		event.Skip()

class disassembly_list(wx.ListCtrl):

	def __init__(self, parent, win):
		wx.ListCtrl.__init__(self, parent, wx.ID_ANY, style=wx.LC_REPORT | wx.SIMPLE_BORDER | wx.LC_SINGLE_SEL | wx.LC_HRULES | wx.LC_VRULES)
		font = self.GetFont()
		self.resizing = False
		self.own_select = False
		self.SetFont(wx.Font(font.GetPointSize(), wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL))
		self.win = win
		##################################
		self.break_list = []
		self.pc = 0x0
		self.cpuname = None
		##################################
		self.first = 0
		self.sel = 0
		self.pc_item = None
		self.bpid = -1
		self.Bind(wx.EVT_KEY_DOWN, self.key_down)
		self.Bind(wx.EVT_MOUSEWHEEL, self.mouse_wheel)
		self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.item_select)
		self.Bind(wx.EVT_CONTEXT_MENU, self.context_menu)
		self.line_height, self.head_height = self.find_height()
		
		self.mark_col = 0
		self.func_col = 1
		self.addr_col = 2
		self.inst_col = 3
		self.InsertColumn(self.mark_col, '')
		self.InsertColumn(self.func_col, "函数")
		self.InsertColumn(self.addr_col, "地址", format=wx.LIST_FORMAT_RIGHT)
		self.InsertColumn(self.inst_col, "指令")
		white = (255, 255, 255)
		black = (0, 0, 0)
		blackimage = wx.Image(20, 13)
		self.il = wx.ImageList(20, 13)
		self.il.Add(blackimage.ConvertToBitmap(), black)
		for img_file in [se_path.GetPicPath('arrow-blue.png'),
			se_path.GetPicPath('arrow-red.png'), 
			se_path.GetPicPath('arrow-br.png'), 
			se_path.GetPicPath('arrow-bw.png'),
			se_path.GetPicPath('arrow-bwr.png')]:
			im = wx.Image(img_file, wx.BITMAP_TYPE_PNG)
			self.il.Add(im.ConvertToBitmap(), white)

		self.SetImageList(self.il, wx.IMAGE_LIST_SMALL)
		self.no_image = 0
		self.blue_image = 1
		self.red_image = 2
		self.br_image = 3
		self.bw_image = 4
		self.bwr_image = 5
		self.lines = self.GetClientSize()[1] // self.line_height
		self.head_addr = 0x0
		self.instr_len = 4
		self.tail_addr = self.head_addr  + self.instr_len * self.lines
		self.last_scroll_pass = 0
		self.select_item = 0
		mark_width = 25
		func_width = 25 * self.GetCharWidth()
		addr_width = 20 * self.GetCharWidth()
		inst_width = 35 * self.GetCharWidth()
		self.xborder = self.GetSize()[0] - self.GetClientSize()[0]
		self.yborder = self.GetSize()[1] - self.GetClientSize()[1]
		self.SetSizeHints(mark_width + func_width + addr_width + inst_width + self.xborder * 2, self.head_height + self.line_height * 22 + self.yborder)
		self.SetColumnWidth(self.mark_col, mark_width)
		self.SetColumnWidth(self.func_col, func_width)
		self.SetColumnWidth(self.addr_col, addr_width)
		self.SetColumnWidth(self.inst_col, inst_width)
		self.create()
		self.Bind(wx.EVT_SIZE, self.resize)
		self.Bind(wx.EVT_LIST_COL_END_DRAG, self.resize)
		return
	def stop_update(self):
		pass
	def update_pc(self):
		try:
			pc = SkyEyeGetPcByCoreName(self.cpuname)
		except:
			self.ErrorReport()
		self.pc = pc

	def set_cpu(self, cpuname):
		self.cpuname = cpuname
		try:
			width = SkyEyeGetAddressWidth(cpuname)
			self.instr_len = width
		except:
			pass
		self.update_pc()


	def get_line_addr(self, line):
		addr = self.head_addr + self.instr_len*line
		return addr

	def get_min_size(self):
		return (40 * self.GetCharWidth(), self.head_height + self.line_height * 4 + self.yborder)

	def find_height(self):
		lst = wx.ListCtrl(self, wx.ID_ANY, style=wx.LC_REPORT | wx.SIMPLE_BORDER | wx.LC_HRULES | wx.LC_VRULES)
		lst.InsertColumn(0, ('address'))
		lst.InsertItem(0, ('text'))
		x, head, x, height = lst.GetItemRect(0, wx.LIST_RECT_LABEL)
		lst.Destroy()
		return (height, head)

	def instruction_length(self, address):
		pass

	def instruction_text(self, address):
		pass

	def create(self):
		self.update_view()
	def update_image(self, item, addr):
		if (addr == self.pc):
			self.SetItemImage(item, self.no_image)
			self.SetItemImage(item, self.blue_image)
		if (addr in self.break_list):
			self.SetItemImage(item, self.red_image)
		if addr == self.pc and addr in self.break_list:
			self.SetItemImage(item, self.blue_image)

	def update_view(self):
		self.DeleteAllItems()
		for row in range(self.lines):
			self.InsertItem(row, '')
			addr = self.get_line_addr(row)
			self.update_image(row, addr)
			if addr > 0xffffffff:
				self.SetItem(row, 2, '')
			else:
				self.SetItem(row, 2, "0x%08x"%(addr))
			self.SetItem(row, 1, '?')
			if (self.cpuname != None):
				try:
					instr = SkyEyeDisassemble(self.cpuname, addr)
					self.SetItem(row, 3, instr)
				except:
					self.SetItem(row, 3, 'unknown')
			else:
				self.SetItem(row, 3, 'unknown')


	def get_selected_address(self):
		return self.get_item_address(self.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED))

	def item_select(self, event):
		if self.own_select:
			return
		self.set_selection_address(self.get_selected_address(), True)

	def resize(self, event):
		event.Skip()
		if self.resizing:
			return
		else:
			self.resizing = True
			self.SetColumnWidth(self.inst_col, self.GetClientSize()[0] - self.GetColumnWidth(self.mark_col) - self.GetColumnWidth(self.func_col) - self.GetColumnWidth(self.addr_col) - self.xborder * 2)
			self.lines = self.GetCountPerPage()
			self.scroll.set_lines(self.lines)
			self.create()
			self.redraw()
			self.resizing = False
			self.set_head_addr(self.head_addr)
			return

	def redraw(self):
		addr = self.first
		pc_hidden = False
		return pc_hidden

	def count_instructions(self, start_addr, end_addr):
		count = 0
		while start_addr < end_addr:
			start_addr += self.instruction_length(start_addr)
			count += 1

		return (count, start_addr == end_addr)

	def set_new_first_address(self, item, address):
		if address & self.win.min_instr_len() - 1 != 0:
			return
		first_addr = address
		items = 0
		while items <= item:
			items, match = self.count_instructions(first_addr, address)
			if items == item and match:
				break
			elif first_addr == 0:
				break
			first_addr -= self.win.min_instr_len()

		self.first = first_addr
		self.scroll.new_first_visible(self.first)

	def get_address_item(self, address):
		item_addr = self.first
		for item in range(self.lines):
			item_len = self.instruction_length(item_addr)
			if address >= item_addr and address < item_addr + item_len:
				return item
			item_addr += item_len

		return -1

	def get_item_address(self, item):
		if item >= self.lines:
			raise (Exception, 'Illegal item %d' % item)
		addr = self.first
		return addr

	def goto_address(self, address, top = False, send_event = True):
		address &= ~(self.win.min_instr_len() - 1)
		if top:
			self.set_new_first_address(0, address)
		elif not self.address_in_view(address):
			if self.win.min_instr_len() == 1:
				self.set_new_first_address(0, address)
			else:
				self.set_new_first_address(self.lines // 3, address)
		self.change_selected(address, send_event)
		self.redraw()

	def address_in_view(self, address):
		return self.get_address_item(address) >= 0

	def set_selection_address(self, address, send_event):
		self.sel = address
		if send_event:
			#win_main.change_inspect_pc(self.win.cpu, address, self.win.GetId())
			pass

	def change_selected(self, address, send_event = True, current = False):
		sel_item = self.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)
		if sel_item >= 0:
			self.SetItemState(sel_item, 0, wx.LIST_STATE_SELECTED)
		item = self.get_address_item(address)
		if item < 0:
			return
		self.set_selection_address(address, send_event)
		self.own_select = True
		state = wx.LIST_STATE_FOCUSED
		if not current:
			state = wx.LIST_STATE_SELECTED
		self.SetItemState(item, state, wx.LIST_STATE_SELECTED | wx.LIST_STATE_FOCUSED)
		self.own_select = False

	def update_selected(self):
		self.change_selected(self.sel)
	def set_head_addr(self, addr):
		self.head_addr = addr
		self.tail_addr = self.head_addr  + self.instr_len * (self.lines - 1)

	def get_pc_item(self):
		if self.pc_is_in_view():
			for item in range(0, self.lines):
				pc_addr = self.GetItem(item, 2).GetText()
				if self.pc == int(pc_addr, 16):
					return item
			return  None
		return None

	def pc_is_in_view(self):
		if self.pc >= self.head_addr and self.pc <= self.tail_addr:
			return 1
		else:
			return 0

	def view_current_pc(self, send_event):
		self.SetItemState(self.select_item, 0, wx.LIST_STATE_SELECTED)
		if self.pc <= self.lines *self.instr_len:
			self.set_head_addr(0x0)
			self.tail_addr = self.head_addr  + self.instr_len * self.lines
			self.select_item = self.pc // self.instr_len 
		else:
			self.set_head_addr(self.pc - 10 * self.instr_len)
			self.select_item = 10
		self.update_view()
		self.SetItemState(self.select_item, wx.LIST_STATE_SELECTED, wx.LIST_STATE_SELECTED)

	def get_item_addr(self, item):
		return self.head_addr + self.instr_len*item
	def context_menu(self, event):
		running = self.win.skyeye_is_running()
		self.PopupMenu(disassembly_popup_menu(running, self.run_until), self.ScreenToClient(event.GetPosition()))

	def line_up(self):
		self.set_new_first_address(1, self.first)
		self.change_selected(self.sel)
		self.redraw()

	def page_up(self):
		self.set_new_first_address(self.lines - 1, self.first)
		self.change_selected(self.sel)
		self.redraw()

	def line_down(self):
		if self.first + self.instruction_length(self.first) >= self.win.max_address:
			return
		self.first += self.instruction_length(self.first)
		self.change_selected(self.sel)
		self.redraw()
		self.scroll.new_first_visible(self.first)

	def page_down(self):
		for row in range(self.lines - 1):
			if self.first + self.instruction_length(self.first) >= self.win.max_address:
				return
			self.first += self.instruction_length(self.first)

		self.change_selected(self.sel)
		self.redraw()
		self.scroll.new_first_visible(self.first)

	def key_down(self, event):
		key = event.GetKeyCode()
		if key == wx.WXK_DOWN or key == wx.WXK_NUMPAD_DOWN:
			self.line_down()
		elif key == wx.WXK_UP or key == wx.WXK_NUMPAD_UP:
			self.line_up()
		elif key == wx.WXK_PAGEDOWN or key == wx.WXK_NUMPAD_PAGEDOWN or key == wx.WXK_NEXT:
			self.page_down()
		elif key == wx.WXK_PAGEUP or key == wx.WXK_NUMPAD_PAGEUP or key == wx.WXK_PRIOR:
			self.page_up()
		else:
			event.Skip()

	def mouse_wheel(self, event):
		steps = event.GetWheelRotation() // event.GetWheelDelta() * event.GetLinesPerAction()
		head_addr = self.head_addr + -1 * steps *self.instr_len
		if head_addr < 0:
			self.set_head_addr(0x0)
		else:
			self.set_head_addr(head_addr)
		self.update_view()
	
	def create_breakpoint(self, addr):
		if self.cpuname == None:
			return 
		try:
			ret = SkyEyeCreateBreakpoint(self.cpuname, addr)
		except:
			self.ErrorReport() 
	def run_until(self, forward, event):
		item = self.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)
		addr = self.get_item_addr(item)
		if addr == self.pc:
			return 
		self.create_breakpoint(addr)
		self.break_list.append(addr)
		self.update_view()
		self.win.skyeye_run()

	def delete_breakpoint(self, addr):
		self.break_list.remove(addr)

	def ErrorReport(self):
		mess = errormessage.GetErrorInfo()
		errmesg = mess[1]
		detail = mess[2]
		dlg = ErrorReportDialog(errmesg, detail)
		ret = dlg.ShowModal()
		if ret == wx.ID_CLOSE:
			sys.exit(0)
		elif ret == wx.ID_RESET:
			sys.exit(100)


class address_scroll(wx.ScrollBar):

	def __init__(self, parent, win, dis):
		wx.ScrollBar.__init__(self, parent, wx.ID_ANY, style=wx.SB_VERTICAL)
		self.win = win
		self.dis = dis
		self.last_pos = 0
		self.first_address = 0
		self.no_first_change = False
		self.vlines = 1000
		self.set_lines(20)
		self.Bind(wx.EVT_SCROLL, self.scroll)
		self.Bind(wx.EVT_SCROLL_CHANGED, self.scroll_ended)

	def set_lines(self, lines, thumb = None):
		if thumb == None:
			thumb = self.last_pos
		self.lines = lines
		#self.SetScrollbar(thumb, lines, self.vlines, lines - 1)
		self.SetScrollbar(thumb, lines, self.vlines, 0)
		return

	def new_first_visible(self, address):
		return 
		if self.no_first_change:
			return
		new_first = max(0, address - self.vlines // 2) & -1024
		instr_bytes_left = self.vlines * self.win.min_instr_len()
		if new_first > self.win.max_address - instr_bytes_left:
			new_first = self.win.max_address - instr_bytes_left
		new_pos = (address - new_first) // self.win.avg_instr_len()
		self.set_lines(self.lines, new_pos)
		self.first_address = new_first
		self.last_pos = new_pos

	def scroll_ended(self, event):
		self.SetThumbPosition(0)

	def scroll(self, event):
		scroll_pass = self.GetThumbPosition()
		if self.dis.last_scroll_pass == 0:
			scroll_pass = -1*self.dis.lines
			self.dis.last_scroll_pass = scroll_pass
		else:
			self.dis.last_scroll_pass = 0
		head_addr = self.dis.head_addr  + scroll_pass*self.dis.instr_len
		if head_addr < 0:
			self.dis.set_head_addr(0x0)
		else:
			self.dis.set_head_addr(head_addr)
		self.dis.update_view()
		event.Skip()

class DisFrame(wx.Frame):
	window_name = 'disassembly'
	def __init__(self, parent):
		wx.Frame.__init__(self, parent, wx.ID_ANY, '', style=wx.DEFAULT_FRAME_STYLE)
		self.parent = parent
		self.config = GetGlobalConfig()
		self.cpu_list = self.config.get_cpu_list()
		self.cpu = None
		self.context = None
		self.min_instruction_length = None
		self.max_instruction_length = None
		self.avg_instruction_length = None
		self.addrbox_list = []
		self.bits = 32
		self.max_address = 1 << self.bits
		self.restore_focus = None
		self.Bind(wx.EVT_KEY_DOWN, self.key_down)
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.panel = wx.Panel(self, wx.ID_ANY)
		sizer.Add(self.panel, proportion=1, flag=wx.EXPAND)
		psizer = wx.BoxSizer(wx.VERTICAL)
		ssizer = wx.BoxSizer(wx.HORIZONTAL)
		self.cpu_context = cpu_context_selector(self.panel, ssizer, self.cpu_change, self.cpu_list)
		psizer.Add(ssizer, flag=wx.EXPAND | wx.ALL, border=2)
		ssizer.Add((0, 0), proportion=1, flag=wx.EXPAND)
		
		h2sizer = wx.BoxSizer(wx.HORIZONTAL)
		psizer.Add(h2sizer, flag=wx.EXPAND | wx.ALL, border=2)
		sbox = wx.StaticBox(self.panel, label=("查找"))
		h2bsizer = wx.StaticBoxSizer(sbox, wx.HORIZONTAL)
		h2sizer.Add(h2bsizer, flag=wx.EXPAND | wx.ALL, border=2)
		h2bsizer.Add((0, 0), proportion=1, flag=wx.EXPAND)
		self.pc_button = wx.Button(self.panel, wx.ID_ANY, style=wx.BU_EXACTFIT, label=("当前"))
		h2bsizer.Add(self.pc_button, flag=wx.ALIGN_CENTER_VERTICAL | wx.ALL, border=2)
		self.Bind(wx.EVT_BUTTON,self.goto_pc,self.pc_button)
		h2bsizer.Add((0, 0), proportion=1, flag=wx.EXPAND)
		txt = wx.StaticText(self.panel, wx.ID_ANY, "输入地址")
		self.addrbox = wx.ComboBox(self.panel, wx.ID_ANY, style=wx.CB_DROPDOWN | wx.TE_PROCESS_ENTER, validator=addr_validator(self))
		self.addrbox.SetSizeHints(24 * self.addrbox.GetTextExtent('8')[0], -1)
		h2bsizer.Add(txt, flag=wx.ALIGN_CENTER_VERTICAL | wx.ALL, border=2)
		h2bsizer.Add(self.addrbox, flag=wx.ALIGN_CENTER_VERTICAL | wx.ALL, border=2)
		#self.left = wx.Button(self.panel, wx.ID_ANY, label='<', style=wx.BU_EXACTFIT)
		#self.right = wx.Button(self.panel, wx.ID_ANY, label='>', style=wx.BU_EXACTFIT)
		#h2bsizer.Add(self.left, flag=wx.ALIGN_CENTER_VERTICAL | wx.TOP | wx.BOTTOM | wx.LEFT, border=2)
		#h2bsizer.Add(self.right, flag=wx.ALIGN_CENTER_VERTICAL | wx.TOP | wx.BOTTOM | wx.RIGHT, border=2)
		#wx.EVT_BUTTON(self.left, self.left.GetId(), self.adjust_left)
		#wx.EVT_BUTTON(self.right, self.right.GetId(), self.adjust_right)
		self.left_tt = wx.ToolTip('')
		self.right_tt = wx.ToolTip('')
		#self.left.SetToolTip(self.left_tt)
		#self.right.SetToolTip(self.right_tt)
		h2sizer.Add((0, 0), proportion=1, flag=wx.EXPAND)
		sbox = wx.StaticBox(self.panel, label=("系统单步"))
		s1sizer = wx.StaticBoxSizer(sbox, wx.HORIZONTAL)
		h2sizer.Add(s1sizer, flag=wx.EXPAND | wx.ALIGN_RIGHT | wx.ALL, border=2)

		self.sys_unstep = no_dclick_button(self.panel, "反向单步", wx.BU_EXACTFIT, self.sys_step_reverse)
		self.sys_unstep.Disable()
		self.sys_step = no_dclick_button(self.panel, "单步", wx.BU_EXACTFIT, self.sys_step_forward)
	
		s1sizer.Add(self.sys_unstep, flag=wx.ALIGN_CENTER_VERTICAL | wx.ALL, border=2)
		s1sizer.Add(self.sys_step, flag=wx.ALIGN_CENTER_VERTICAL | wx.ALL, border=2)
		s1sizer.Add((20, 20), flag=wx.ALIGN_CENTER_VERTICAL | wx.ALL, border=2)
		h2bsizer.Add((0, 0), proportion=1, flag=wx.EXPAND)
		self.Bind(wx.EVT_COMBOBOX,self.addr_select,self.addrbox)
		self.Bind(wx.EVT_TEXT_ENTER, self.addr_entry, self.addrbox)
		self.dis = disassembly_list(self.panel, self)
		self.dis.SetFocus()
		dsizer = wx.BoxSizer(wx.HORIZONTAL)
		dsizer.Add(self.dis, proportion=1, flag=wx.EXPAND | wx.ALL, border=1)
		self.scroll = address_scroll(self.panel, self, self.dis)
		self.dis.scroll = self.scroll
		dsizer.Add(self.scroll, flag=wx.EXPAND | wx.ALL, border=1)
		psizer.Add(dsizer, proportion=1, flag=wx.EXPAND | wx.ALL, border=2)
		h4sizer = wx.BoxSizer(wx.HORIZONTAL)
		psizer.Add(h4sizer, flag=wx.EXPAND)
		h4sizer.Add((0, 0), proportion=1, flag=wx.EXPAND)
		self.panel.SetSizer(psizer)
		self.SetSizer(sizer)
		s1 = self.GetEffectiveMinSize()
		self.dis.SetSizeHints(*self.dis.get_min_size())
		self.Fit()
		s2 = self.GetEffectiveMinSize()
		self.SetSize(s1)
		self.SetSizeHints(*s2)
		self.Bind(wx.EVT_CLOSE, self.quit, self)

	def quit(self, event):
		self.parent.dissemble_frame = None
		self.Destroy()
	def skyeye_is_running(self):
		return self.parent.running

	def skyeye_run(self):
		self.parent.on_skyeye_run(0)
	def update_all(self):
		self.update_page(self.cpu)
		self.update_buttons()

	def context_updated(self, context):
		if context == self.context:
			self.update_all()

	def symtable_updated(self, obj):
		self.update_all()

	def breakpoint_change(self, obj):
		self.update_page(self.cpu)

	def pc_changed(self, obj, address, win_id):
		address &= 18446744073709551615
		if win_id == self.GetId() or obj != self.cpu:
			return
		if address < 0:
			address = 0
		self.update_page_if_cpu_mode_changed()
		self.dis.goto_address(address, send_event=False)

	def adjust_left(self, event):
		pass

	def adjust_right(self, event):
		pass

	def goto_pc(self, event):
		self.dis.view_current_pc(send_event=True)

	def goto_sym(self, event):
		pass

	def addr_select(self, event):
		val_str = self.addrbox.GetValue().strip()
		value = int(val_str, 16)
		self.addrbox.SetValue(val_str)

	def error_addr_to_right(self, addr):
		while(addr % self.dis.instr_len != 0):
			addr += 1
		return addr
	def addr_entry(self, event):
		self.dis.SetItemState(self.dis.select_item, 0, wx.LIST_STATE_SELECTED)
		val_str = self.addrbox.GetValue().strip()
		input_addr = 0
		if val_str.startswith('0x'):
			addr = int(val_str, 16) 
		else: 
			addr = int(val_str, 10)
		input_addr = addr
		addr = self.error_addr_to_right(addr)
		if addr <= self.dis.lines *self.dis.instr_len:
			self.dis.set_head_addr(0x0)
			self.dis.select_item = addr // self.dis.instr_len 
		else:
			self.dis.set_head_addr(addr - 10 * self.dis.instr_len )
			self.dis.select_item = 10
		self.dis.update_view()
		self.dis.SetItemState(self.dis.select_item, wx.LIST_STATE_SELECTED, wx.LIST_STATE_SELECTED)
		addr_s = "0x%x" % input_addr
		if addr_s not in self.addrbox_list:
			self.addrbox_list.append(addr_s)
		self.addrbox.SetItems(self.addrbox_list)
		self.addrbox.SetValue("")

		
	def min_instr_len(self):
		return self.min_instruction_length

	def avg_instr_len(self):
		return self.avg_instruction_length

	def update_title(self):
		self.SetTitle("反汇编" + "-" + self.cpu)

	def get_opcode_info(self, cpu):
		#if hasattr(cpu.iface, 'opcode_info'):
		#	length_info = cpu.iface.opcode_info.get_opcode_length_info()
		#	return (length_info.min_alignment, length_info.max_length, length_info.avg_length)
		#else:
		return (4, 4, 4)

	def update_instruction_mode(self, cpu):
		min, max, avg = self.get_opcode_info(cpu)
		return min != self.min_instruction_length or max != self.max_instruction_length or avg != self.avg_instruction_length

	def update_page(self, cpu):
		self.cpu = cpu
		self.context = None 
		self.update_title()
		self.dis.set_cpu(cpu)
		self.dis.view_current_pc(send_event=None)

	def update_page_if_cpu_mode_changed(self):
		if self.update_instruction_mode(self.cpu):
			self.update_page(self.cpu)

	def step_cmd(self, cpu, cmd):
		old_cpu = cli.current_processor()
		cli.set_current_processor(cpu)
		try:
			cli.run_command(cmd)
		except Exception as msg:
			pass

		cli.set_current_processor(old_cpu)

	def sys_step_reverse(self, event):
		cmd = cli.GetCommand("reverse")
		if cmd == None:
			LogReport('reverse: Can not find command "reverse"')
			return 
		try:
			cmd.run([self.cpu, "1"])
		except Exception as e:
			LogReport(e)
			return
		self.dis.update_pc()
		if self.dis.pc_is_in_view() == 0:
			self.dis.set_head_addr(self.dis.pc)
		self.dis.update_view()
		self.parent.update_sub_gui(self)


	def sys_step_forward(self, event):
		if self.dis.pc_is_in_view():
			pass
		else:
			self.dis.view_current_pc(send_event = None)
		old_pc_item = self.dis.get_pc_item()
		try:
			ret = SkyEyeStepi(self.dis.cpuname, "1")
		except:
			self.dis.ErrorReport()
		self.dis.update_pc()
	
		if self.dis.pc_is_in_view() == 0:
			self.dis.set_head_addr(self.dis.pc)
		self.dis.update_view()
		self.parent.update_sub_gui(self)
	

	def ctx_next_reverse(self, event):
		pass

	def ctx_step_reverse(self, event):
		pass

	def ctx_step_forward(self, event):
		pass

	def ctx_next_forward(self, event):
		pass

	def update_buttons(self):
		if self.context:
			have_symtable = self.context.symtable != None
			#may_run = simics.SIM_simics_is_running() and False
			may_rev = False
		else:
			may_run = 1
			may_rev = may_run 
		self.sys_step.Enable(may_run)
		#self.ctx_step.Enable(may_run)
		#self.ctx_next.Enable(may_run )
		#self.sys_unstep.Enable(may_rev)
		#self.ctx_unstep.Enable(may_rev)
		#self.ctx_prev.Enable(may_rev)
		return
	def update_reverse(self, enable):
		if enable:
			self.sys_unstep.Enable()
		else:
			self.sys_unstep.Disable()

	def new_arguments(self, obj, args):
		if self.cpu != obj:
			self.cpu = obj
			self.update_all()

	def cpu_change(self, cpu):
		self.cpu = cpu
		self.update_all()

	def key_down(self, event):
		self.dis.key_down(event)

	def sim_started(self, obj):
		foc = self.FindFocus()
		if foc in [self.sys_step,
			self.sys_unstep,
			self.ctx_step,
			self.ctx_unstep,
			self.ctx_prev,
			self.ctx_next]:
			self.restore_focus = foc
		else:
			self.restore_focus = None
		self.update_buttons()
		return

	def Refurbish(self):
		self.update_all()
	def delete_breakpoint(self, addr):
		self.dis.delete_breakpoint(addr)

	def sim_rexec_activation(self, obj, active):
		self.update_buttons()


if __name__ == '__main__':
	app = wx.PySimpleApp()
	Memdlg = DisFrame(parent = None)
	Memdlg.Show()
	app.MainLoop()
