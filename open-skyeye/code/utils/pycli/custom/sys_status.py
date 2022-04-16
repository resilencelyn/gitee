#-*- coding:UTF-8 -*-
import random
import wx
import wx.lib.buttons as buttons
import wx.lib.popupctl as pop
from skyeye_common_module import *
import conf
import threading
import thread
import time
import errormessage as EM
fname = 'Segoe UI'
int_col = '#ADEAEA'
int_addr_col = wx.BLACK
bc_col = wx.BLUE
right_popup_col = '#DBDB70'
ALOW = 8
INSITE = 16
LOST = 64
STATE = 128
ABFLAG = 32768
TELEMETRY = 128
ObjCls = ['sparc_systate', 'sparc_remote_ctrl']

def _(s):
	return unicode(s, 'gb2312')

def write_status(bit):
    global ObjCls
    config = conf.GetGlobalConfig()
    if config == None:
        return
    obj = config.get_obj_by_class(ObjCls[0])
    if obj == None:
        return
    mach = config.get_mach_for_device(obj)
    regvalue = SkyEyeGetDevRegValueById(mach, obj, 4)
    if regvalue == EM.DLLCALLERRORINFO:
        return
    regvalue = regvalue ^ bit
    ret = SkyEyeSetDevRegValueById(mach, obj, regvalue, 4)
    if ret == EM.DLLCALLERRORINFO:
        return
    else:
        return


channel= [0x80, 0x84]
def send_interrupt(ch, irq_data):
    config = conf.GetGlobalConfig()
    if config == None:
        return False
    else:
        obj = config.get_obj_by_class(ObjCls[1])
        SkyEyeWriteDevice4(obj, channel[ch], irq_data)
        return True

def send_stop_data(stop_data):
    config = conf.GetGlobalConfig()
    if config == None:
        return False
    else:
        obj = config.get_obj_by_class(ObjCls[1])
        SkyEyeWriteDevice4(obj, 0x8c, stop_data)
        return True


def tcua_send(val):
    config = conf.GetGlobalConfig()
    if config == None:
        return False
    else:
        obj = config.get_obj_by_class(ObjCls[1])
        SkyEyeWriteDevice4(obj, 192, val)
        return True


def tcub_send(val):
    config = conf.GetGlobalConfig()
    if config == None:
        return False
    else:
        obj = config.get_obj_by_class(ObjCls[1])
        SkyEyeWriteDevice4(obj, 196, val)
        return True


def comb_str_1(obj_list):
    sys_mesg = _("未发现@CLS@设备!")
    cls_str = ''
    num = len(obj_list)
    for i in range(0, num):
        if obj_list[i] != None:
            continue
        cls_str = cls_str + '[' + ObjCls[i] + ']'

    sys_mesg = str(sys_mesg)
    sys_mesg = sys_mesg.replace('@CLS@', cls_str)
    return sys_mesg


class gp_doc(wx.Dialog):
    def __init__(self, parent, win):
        wx.Dialog.__init__(self, parent)
	self.win = win
	self.SetTitle(_("说明"))
        text = wx.TextCtrl(self, -1, size = (500, 500), style=wx.TE_RICH2 | wx.TE_MULTILINE | wx.TE_READONLY)
	text.SetBackgroundColour('#ADEAEA')
	text.SetValue(self.doc())
	sizer = wx.BoxSizer(wx.VERTICAL)
	sizer.Add(text, 1, wx.EXPAND)
        self.Bind(wx.EVT_CLOSE, self.close, self)
	self.SetSizer(sizer)
    def close(self, event):
	    self.win.doc = None
	    self.Destroy()
    def doc(self):
	    return '''\
	                系统状态
系统状态寄存器地址：0x{sys_status:x}
位        名称               定义
-------------------------------------------------
[15]      A/B机标示          1:A机, 0:B机   
[14:13]   型号模式           0x10:XX-5
[8]       权状态             1:有权, 0:无权
[7]       箭星分离           1:分离前, 0:分离后
[4]       准禁自主切权       1:准, 0:禁


	            遥控中断
遥控中断注入地址：
-------------------------------------------------
通道1：0x{interrupt_addr1:x}
通道2：0x{interrupt_addr2:x}


	            数字量遥测
-------------------------------------------------
地址       位          功能
0x{tcua:x} [15:0]      TCU A遥测数据存放
0x{tcub:x} [15:0]      TCU B遥测数据存放
0x{sat:x}  [15:0]      TCU B遥测数据存放


	            DPSR数据缓冲区
页面寄存器:0x{page_index:x}
-------------------------------------------------
位        功能
[7:0]     0x00 ~ 0xFF 共256页,
          0x00用作发送数据存放,
          0x01 ~ 0xFF用作延摇缓存

发送缓冲地址:0x{dpsr_start:x}
-------------------------------------------------
4k字      第0页
4k字      第1页
4k字      第2页
4k字      第3页
...



	            间接指令
-------------------------------------------------
地址          位           功能
0x{ic_l:x}    [7:0]        间接指令数据存储低8位
0x{ic_h:x}    [23:8]       间接指令数据存储高16位
'''.format(sys_status = 0x11010030,\
	   interrupt_addr1 = 0x10000080,\
	   interrupt_addr2 = 0x10000084,\
	   tcua = 0x100000C0,\
	   tcub = 0x100000C4,\
	   sat = 0x10000000,\
	   page_index = 0x10000040,\
	   dpsr_start = 0x10004000,\
	   ic_l = 0x100000D4,\
	   ic_h = 0x100000D0
	   ) 

class indirect_code(wx.Dialog):
    def __init__(self, parent, win):
        wx.Dialog.__init__(self, parent)
	self.SetTitle(_("间接指令"))
	self.win = win
	self.SetBackgroundColour('#ADEAEA')
	h = wx.StaticText(self, -1, _('高位:'))
	l = wx.StaticText(self, -1, _('低位:'))
        self.h_text = wx.TextCtrl(self, -1, style=wx.TE_RICH2 |wx.TE_READONLY)
        self.l_text = wx.TextCtrl(self, -1, style=wx.TE_RICH2 |wx.TE_READONLY)
	sizer = wx.GridBagSizer(hgap=5, vgap=5)
        sizer.Add(h, pos=(0, 0), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL, border=8)
        sizer.Add(self.h_text, pos=(0, 1), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer.Add(l, pos=(1, 0), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=8)
        sizer.Add(self.l_text, pos=(1, 1), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        self.Bind(wx.EVT_CLOSE, self.close, self)
	self.SetSizer(sizer)
	self.Fit()

    def close(self, event):
        self.win.indirect = None
	self.Destroy()
    def update_value(self, h, l):
	    h_s = "0x%x" % h
	    l_s = "0x%x" % l
	    h_value = self.h_text.GetValue()
	    l_value = self.l_text.GetValue()
	    if h_value != h_s:
	    	self.h_text.SetValue(h_s)
	    if l_value != l_s:
	        self.l_text.SetValue(l_s)

class Update():
    def __init__(self, win):
	self.win = win
        self.connect = False
        self.win.connect = False
	
    def connect_check(self, obj_list):
        status = _("设备就绪!")
        self.connect = True
        self.win.connect = True
        for obj in obj_list:
            if obj == None:
                status = _("设备没有连接!")
                self.connect = False
                self.win.connect = False
                break

        self.win.set_connect_status(status)
        return
    def check_objcls(self):
        return_list = []
        vf = True
        config = conf.GetGlobalConfig()
        if config == None:
            return_list = [ None for cls in ObjCls ]
            vf = False
        else:
            for cls in ObjCls:
                obj = config.get_obj_by_class(cls)
                if obj == None:
                    vf = False
                    return_list.append(None)
                else:
                    return_list.append(obj)

        if vf == False:
            st = comb_str_1(return_list)
            self.win.set_sys_info(st)
        return return_list

    def fit_status(self, sys_device):
        if self.connect == False:
            return
        else:
            config = conf.GetGlobalConfig()
            if config == None:
                return
            mach = config.get_mach_for_device(sys_device)
            regvalue = SkyEyeGetDevRegValueById(mach, sys_device, 4)
            if regvalue == EM.DLLCALLERRORINFO:
                return
            self.win.set_status(regvalue)
            return

    
    def fit_buff(self, telemetry_device):
        self.win.buff.set_item_string()

    def update_indirect(self):
	    global ObjCls
	    config = conf.GetGlobalConfig()
	    if config == None:
		return
	    obj = config.get_obj_by_class(ObjCls[1])
	    if obj == None:
		return
	    mach = config.get_mach_for_device(obj)
	    l_data = SkyEyeGetDevRegValueById(mach, obj, 9)
	    h_data = SkyEyeGetDevRegValueById(mach, obj, 10)
	    if l_data == EM.DLLCALLERRORINFO:
		return
	    if h_data == EM.DLLCALLERRORINFO:
		return
	    l = l_data & (0xff)
	    h = h_data & (0xffff << 8)
	    if self.win.indirect != None:
	        self.win.indirect.update_value(h, l)

    def update(self):
	obj_list = self.check_objcls()
        self.connect_check(obj_list)
        self.fit_status(obj_list[0])
        self.fit_buff(obj_list[1])
	self.update_indirect()


	    
class page_frush(threading.Thread):

    def __init__(self, win):
        threading.Thread.__init__(self)
        self.thread_stop = False
        self.win = win
        self.connect = False
        self.win.connect = False
        self.win_close = False

    def connect_check(self, obj_list):
        status = _("设备就绪!")
        self.connect = True
        self.win.connect = True
        for obj in obj_list:
            if obj == None:
                status = _("设备没有连接!")
                self.connect = False
                self.win.connect = False
                break

        self.win.set_connect_status(status)
        return

    def check_objcls(self):
        return_list = []
        vf = True
        config = conf.GetGlobalConfig()
        if config == None:
            return_list = [ None for cls in ObjCls ]
            vf = False
        else:
            for cls in ObjCls:
                obj = config.get_obj_by_class(cls)
                if obj == None:
                    vf = False
                    return_list.append(None)
                else:
                    return_list.append(obj)

        if vf == False:
            st = comb_str_1(return_list)
            self.win.set_sys_info(st)
        return return_list

    def fit_status(self, sys_device):
        if self.connect == False:
            return
        else:
            config = conf.GetGlobalConfig()
            if config == None:
                return
            mach = config.get_mach_for_device(sys_device)
            regvalue = SkyEyeGetDevRegValueById(mach, sys_device, 4)
            if regvalue == EM.DLLCALLERRORINFO:
                return
            self.win.set_status(regvalue)
            return


    def fit_buff(self, telemetry_device):
        self.win.buff.set_item_string()

    def update_indirect(self):
	    global ObjCls
	    config = conf.GetGlobalConfig()
	    if config == None:
		return
	    obj = config.get_obj_by_class(ObjCls[1])
	    if obj == None:
		return
	    mach = config.get_mach_for_device(obj)
	    l_data = SkyEyeGetDevRegValueById(mach, obj, 9)
	    h_data = SkyEyeGetDevRegValueById(mach, obj, 10)
	    if l_data == EM.DLLCALLERRORINFO:
		return
	    if h_data == EM.DLLCALLERRORINFO:
		return
	    l = l_data & (0xff)
	    h = h_data & (0xffff << 8)
	    if self.win.indirect != None:
	        self.win.indirect.update_value(h, l)

    def frush(self):
        obj_list = self.check_objcls()
        self.connect_check(obj_list)
        self.fit_status(obj_list[0])
        self.fit_buff(obj_list[1])
	self.update_indirect()

    def run(self):
        while self.thread_stop == False:
            self.frush()
            time.sleep(0.1)

        if self.win_close == True:
            self.win.destroy()


    def stop(self, win_close):
        self.thread_stop = True
	time.sleep(1)
    	self.win.remove_self_from_main()
        self.win_close = win_close


class buffTextCtrl(wx.ListCtrl):

    def __init__(self, parent, win):
        wx.ListCtrl.__init__(self, parent, wx.ID_ANY, style=wx.LC_REPORT | wx.LC_SINGLE_SEL)
        self.win = win
        self.addr_len = 100
        self.char_len = 40
        self.cols = 16
        self.lines = 0
        self.addr_base = 268451840
        self.addr_base_i = 268451840
        self.offset_base = 0
        self.page_size = 1024
        self.resizing = False
        self.updateing = False
        self.SetBackgroundColour(wx.BLACK)
        wx.EVT_MOUSEWHEEL(self, self.mouse_wheel)
        font = self.GetFont()
        self.resizing = False
        self.SetFont(wx.Font(font.GetPointSize(), wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL))
        wx.EVT_SIZE(self, self.resize)
        self.SetMaxSize((200, 500))

    def resize(self, event):
        if self.resizing:
            return
        self.resizing = True
        self.ClearAll()
        self.cols = (self.GetClientSize()[0] - self.addr_len) / self.char_len
        self.lines = 8
        self.insert_item(self.lines)
        self.insert_cloumn(self.cols)
        self.win.v_scroll.set_cols(self.cols)
        self.resizing = False

    def update_addr(self, page):
        page_num = int(page)
        self.addr_base = self.addr_base_i + self.page_size * (page_num - 1) * 8
        self.set_item()

    def update_right(self, offset):
        if self.updateing:
            return
        self.updateing = True
        self.lines = self.GetCountPerPage()
        addr_base = self.addr_base + offset * self.page_size
        if addr_base < self.addr_base_i:
            self.addr_base = self.addr_base_i
        self.addr_base = addr_base
        self.set_item()
        self.updateing = False

    def update_bottom(self, offset):
        self.offset_base = self.offset_base + offset
        if self.updateing:
            return
        self.updateing = True
        self.set_cloumun()
        self.updateing = False

    def get_value(self, addr):
        if self.win.connect == False:
            return ''
        else:
            config = conf.GetGlobalConfig()
            if config == None:
                return ''
            obj = config.get_obj_by_class(ObjCls[1])
            value = SkyEyeReadDevice4(obj, addr - 0x10000000)
	    if value == errormessage.DLLCALLERRORINFO:
		    value = 0
            va = value & 255
            st = ('%02x' % va).upper()
            return st

    def find_height(self):
        lst = wx.ListCtrl(self, wx.ID_ANY, style=wx.LC_REPORT | wx.SIMPLE_BORDER | wx.LC_HRULES | wx.LC_VRULES)
        lst.InsertColumn(0, 'address')
        lst.InsertStringItem(0, 'text')
        x, head, x, height = lst.GetItemRect(0, wx.LIST_RECT_LABEL)
        lst.Destroy()
        return (height, head)

    def set_cloumun(self):
        for i in range(1, self.cols + 1):
            x = wx.ListItem()
            index = '%x' % (i + self.offset_base)
            x.SetText(index.upper())
            self.SetColumn(i, x)

    def set_item(self):
        for i in range(0, self.lines):
            item = '0x%x' % (self.addr_base + self.page_size * i)
            self.SetItemText(i, item)

    def set_item_string(self):
        for i in range(0, self.lines):
            for j in range(1, self.cols + 1):
                addr_base = self.get_addr_by_line(i)
                addr = addr_base + self.offset_base + j - 1
                item = self.GetItem(i, j)
                txt = item.GetText()
                val = self.get_value(addr)
                if cmp(txt, val) == 0:
                    continue
                self.SetStringItem(i, j, val)

    def insert_cloumn(self, num):
        self.InsertColumn(0, '')
        for i in range(1, num + 1):
            self.InsertColumn(i, '%3x' % i, format=wx.LIST_FORMAT_RIGHT)

        self.SetColumnWidth(0, self.addr_len)
        for i in range(1, num + 1):
            self.SetColumnWidth(i, self.char_len)

    def insert_item(self, num):
        for i in range(0, num):
            item = '0x%x' % (self.addr_base + self.page_size * i)
            self.InsertStringItem(i, item)
            self.SetItemTextColour(i, wx.GREEN)

    def insert_str(self):
        for i in range(0, self.lines):
            for j in range(1, self.cols + 1):
                addr = self.get_addr_by_line(i)
                self.SetStringItem(i, j, self.get_value(addr))

    def get_addr_by_line(self, line):
        return self.addr_base + line * self.page_size

    def mouse_wheel(self, event):
        steps = event.GetWheelRotation() / event.GetWheelDelta() * event.GetLinesPerAction()
        page_base = self.win.page_base + steps * -1
        if page_base < 1:
            page_base = 1
        if page_base > 237:
            page_base = 237
        self.win.page_base = page_base
        self.win.set_page_index()


class v_scroll(wx.ScrollBar):

    def __init__(self, parent, win, ctr):
        wx.ScrollBar.__init__(self, parent, wx.ID_ANY, style=wx.SB_HORIZONTAL)
        self.win = win
        self.ctr = ctr
        self.vlines = 1023
        self.lines = 0
        self.old_pos = 0
        self.set_lines(20)
        wx.EVT_SCROLL(self, self.scroll)
        wx.EVT_SCROLL_CHANGED(self, self.scroll_ended)

    def set_cols(self, col):
        self.set_lines(col)

    def set_lines(self, lines):
        self.lines = lines
        self.SetScrollbar(0, lines, self.vlines, 0, refresh=True)

    def scroll_ended(self, event):
        pass

    def set_s(self, pos):
        self.SetScrollbar(pos, self.lines, self.vlines, 0, refresh=True)
        offset = pos - self.old_pos
        self.old_pos = pos
        self.ctr.update_bottom(offset)

    def scroll(self, event):
        scroll_pass = self.GetThumbPosition()
        wx.CallAfter(self.set_s, scroll_pass)


class TransientPopup(wx.PopupTransientWindow):
    """Adds a bit of text and mouse movement to the wx.PopupWindow"""

    def __init__(self, parent, style):
        wx.PopupTransientWindow.__init__(self, parent, style)
        self.SetBackgroundColour('#FFB6C1')
        st = wx.StaticText(self, -1, '', pos=(10, 10))
        sz = st.GetBestSize()
        self.SetSize((sz.width + 20, sz.height + 20))

    def ProcessLeftDown(self, evt):
        return False

    def OnDismiss(self):
        pass


class RightPopup(wx.PopupTransientWindow):
    """Adds a bit of text and mouse movement to the wx.PopupWindow"""

    def __init__(self, parent, style):
        wx.PopupTransientWindow.__init__(self, parent, style)
        self.SetBackgroundColour(right_popup_col)
        text = ''
        st = wx.StaticText(self, -1, text, pos=(10, 10))
        sz = st.GetBestSize()
        self.SetSize((sz.width + 20, sz.height + 20))

    def ProcessLeftDown(self, evt):
        return False

    def OnDismiss(self):
        pass


class MyFrame(wx.Frame):

    def __init__(self, parent, args):
        wx.Frame.__init__(self, parent, -1)
        self.SetTitle(_("遥测系统状态"))
        self.win = parent
        self.connect = False
        self.statusbar = self.CreateStatusBar()
        self.statusbar.SetFieldsCount(3)
        self.statusbar.SetStatusWidths([-3, -2, -2])
	self.statusbar.SetStatusText(_("数字量遥测消息:"), 2)
	self.statusbar.SetStatusText(_("硬件链接状态:"), 1)
	self.statusbar.SetStatusText(_("系统消息:"), 0)
        font_size1 = 8
	self.doc = None
	self.indirect = None
        self.col = {1: wx.RED,
         0: '#32CC32'}
        self.remote = {0: _("空闲"), 1: _("繁忙")}
        self.remote_col = {0: '#32CC32', 1: wx.RED}
	self.a_b_s = {1:_("A机"), 0: _("B机")}
        self.a_b_status = 1
	self.right_s = {1: _("有权"), 0: _("无权")}
        self.right_status = 1
        self.star_s = {1: _("分离前"), 0: _("分离后")}
        self.star_status = 1
        self.auto_s = {1: _("准"), 0: _("禁")}
        self.auto_status = 1
        self.page_base = 1
        self.SetBackgroundColour(bc_col)
        panel_left = wx.Panel(self, style=wx.BORDER_DOUBLE)
        panel_left.SetBackgroundColour(bc_col)
        panel_right = wx.Panel(self, style=wx.BORDER_DOUBLE)
        panel_right.SetBackgroundColour(bc_col)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(panel_left, 1, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, 10)
        sizer.Add(panel_right, 2, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, 10)
        sizer1 = wx.BoxSizer(wx.VERTICAL)
        panel_system = wx.Panel(panel_left, style=wx.BORDER_RAISED)
        panel_system.SetBackgroundColour('#ADEAEA')
        sysLbl = wx.StaticText(panel_system, -1, _("系统状态"))
        sysLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD, faceName=fname))
        static_line = wx.StaticLine(panel_system)
	a_bLbl = wx.StaticText(panel_system, -1, _("A/B机标示:"))
        a_bLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        self.a_bShow = buttons.GenButton(panel_system, -1, size=(20, 20))
        self.a_bShow.SetBackgroundColour(self.col[self.a_b_status])
        self.Bind(wx.EVT_BUTTON, self.on_a_bShow, self.a_bShow)
        self.a_bFlag = wx.StaticText(panel_system, -1, self.a_b_s[self.a_b_status])
        self.a_bFlag.SetFont(wx.Font(font_size1, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
	modeLbl = wx.StaticText(panel_system, -1, _("模式型号:"))
        modeLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        modeShow = buttons.GenButton(panel_system, -1, size=(20, 20))
        modeShow.SetBackgroundColour(wx.RED)
        modeName = wx.StaticText(panel_system, -1, _('XX-5'))
        modeName.SetFont(wx.Font(font_size1, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName=fname))
	rightLbl = wx.StaticText(panel_system, -1, _("权状态:"))
        rightLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        self.rightShow = buttons.GenButton(panel_system, -1, size=(20, 20))
        self.rightShow.SetBackgroundColour(self.col[self.right_status])
        self.Bind(wx.EVT_BUTTON, self.on_rightShow, self.rightShow)
        self.rightName = wx.StaticText(panel_system, -1, self.right_s[self.right_status])
        self.rightName.SetFont(wx.Font(font_size1, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName=fname))
	starLbl = wx.StaticText(panel_system, -1, _("箭星分离:"))
        starLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        self.starShow = buttons.GenButton(panel_system, -1, size=(20, 20))
        self.starShow.SetBackgroundColour(self.col[self.star_status])
        self.Bind(wx.EVT_BUTTON, self.on_starShow, self.starShow)
        self.starName = wx.StaticText(panel_system, -1, self.star_s[self.star_status])
        self.starName.SetFont(wx.Font(font_size1, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName=fname))
        internalLbl = wx.StaticText(panel_system, -1, _("内部赋权"))
        internalLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        internalShow = buttons.GenButton(panel_system, -1, size=(20, 20))
        internalShow.SetBackgroundColour(wx.RED)
	autoLbl = wx.StaticText(panel_system, -1, _("准禁自主切权:"))
        autoLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        self.autoShow = buttons.GenButton(panel_system, -1, size=(20, 20))
        self.autoShow.SetBackgroundColour(self.col[self.auto_status])
        self.Bind(wx.EVT_BUTTON, self.on_autoShow, self.autoShow)
        self.autoName = wx.StaticText(panel_system, -1, self.auto_s[self.auto_status])
        self.autoName.SetFont(wx.Font(font_size1, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT, faceName=fname))
        sizer_sys = wx.GridBagSizer(hgap=5, vgap=5)
        sizer_sys.Add(sysLbl, pos=(0, 0), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_sys.Add(static_line, pos=(1, 0), span=(1, 3), flag=wx.EXPAND)
        sizer_sys.AddGrowableCol(2)
        sizer_sys.Add(a_bLbl, pos=(2, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_RIGHT, border=5)
        sizer_sys.Add(self.a_bShow, pos=(2, 1), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_sys.Add(self.a_bFlag, pos=(2, 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        sizer_sys.Add(modeLbl, pos=(3, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_RIGHT, border=5)
        sizer_sys.Add(modeShow, pos=(3, 1), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_sys.Add(modeName, pos=(3, 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        sizer_sys.Add(rightLbl, pos=(4, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_RIGHT, border=5)
        sizer_sys.Add(self.rightShow, pos=(4, 1), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_sys.Add(self.rightName, pos=(4, 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        sizer_sys.Add(starLbl, pos=(5, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_RIGHT, border=5)
        sizer_sys.Add(self.starShow, pos=(5, 1), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_sys.Add(self.starName, pos=(5, 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        sizer_sys.Add(internalLbl, pos=(6, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_RIGHT, border=5)
        sizer_sys.Add(internalShow, pos=(6, 1), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_sys.Add((0, 0), pos=(6, 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        sizer_sys.Add(autoLbl, pos=(7, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_RIGHT, border=5)
        sizer_sys.Add(self.autoShow, pos=(7, 1), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_sys.Add(self.autoName, pos=(7, 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        panel_system.SetSizer(sizer_sys)


	tcu_panel = wx.Panel(panel_left, style=wx.BORDER_RAISED)
        tcu_panel.SetBackgroundColour('#ADEAEA')
        digLbl = wx.StaticText(tcu_panel, -1, _("数字量遥测"))
        digLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD, faceName=fname))
        static_line = wx.StaticLine(tcu_panel)
        tcuaLbl = wx.StaticText(tcu_panel, -1, _('TCU A'))
        tcuaLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        self.tcuaTextctr = wx.TextCtrl(tcu_panel, -1, style=wx.TE_RICH2 | wx.BORDER_SUNKEN)
        self.tcuaTextctr.SetBackgroundColour(int_addr_col)
        self.Bind(wx.EVT_TEXT, self.on_tcua_input, self.tcuaTextctr)
        self.Bind(wx.EVT_TEXT_ENTER, self.on_tcua_Button, self.tcuaTextctr)
	self.tcuaButton = buttons.GenButton(tcu_panel, -1, label=_("发送"), size=(40, 25))
        self.tcuaButton.SetBackgroundColour(int_col)
        self.Bind(wx.EVT_BUTTON, self.on_tcua_Button, self.tcuaButton)

        tcubLbl = wx.StaticText(tcu_panel, -1, _('TCU B'))
        tcubLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        self.tcubTextctr = wx.TextCtrl(tcu_panel, -1, style=wx.TE_RICH2 | wx.BORDER_SUNKEN)
        self.tcubTextctr.SetBackgroundColour(int_addr_col)
        self.Bind(wx.EVT_TEXT, self.on_tcub_input, self.tcubTextctr)
        self.Bind(wx.EVT_TEXT_ENTER, self.on_tcub_Button, self.tcubTextctr)
        
        self.tcubButton = buttons.GenButton(tcu_panel, -1, label=_("发送"), size=(40, 25))
        self.tcubButton.SetBackgroundColour(int_col)
        self.Bind(wx.EVT_BUTTON, self.on_tcub_Button, self.tcubButton)


        sizer_panel = wx.GridBagSizer(hgap=5, vgap=5)
        sizer_panel.Add(digLbl, pos=(0, 0), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_panel.Add(static_line, pos=(1, 0), span=(1, 7), flag=wx.EXPAND)
        sizer_panel.Add(tcuaLbl, pos=(2, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        sizer_panel.Add(self.tcuaTextctr, pos=(3, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        sizer_panel.Add(self.tcuaButton, pos=(4, 0), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_RIGHT, border=5)
        sizer_panel.Add(tcubLbl, pos=(2, 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_CENTER, border=5)
        sizer_panel.Add(self.tcubTextctr, pos=(3, 2), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM | wx.ALIGN_RIGHT, border=5)
        sizer_panel.Add(self.tcubButton, pos=(4, 2), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_panel.AddGrowableCol(6)
        tcu_panel.SetSizer(sizer_panel)
	tcu_panel.Layout()



        panel_int = wx.Panel(panel_right, style=wx.BORDER_RAISED) # intterupt
        panel_int.SetBackgroundColour(int_col)
	intLbl = wx.StaticText(panel_int, -1, _("遥控中断"))
        intLbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD, faceName=fname))
        static_line = wx.StaticLine(panel_int)
        sizer_int = wx.GridBagSizer(hgap=5, vgap=5)
	create_data_button = buttons.GenButton(panel_int, -1, label = _("生成数据"))
	send_data_button = buttons.GenButton(panel_int, -1, label = _("发送"))
        self.data_ctrl = wx.TextCtrl(panel_int, -1, style=wx.TE_RICH2 | wx.TE_MULTILINE)
	self.data_ctrl.SetBackgroundColour("#C0C0C0")
	
        sizer_int.Add(intLbl, pos=(0, 0), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_int.Add(static_line, pos=(1, 0), span=(1, 2), flag=wx.EXPAND)
        sizer_int.Add(create_data_button, pos=(2, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_int.Add(send_data_button, pos=(3, 0), flag=wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        sizer_int.Add(self.data_ctrl, pos=(2, 1), span=(6,1), flag=wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=5)
        self.Bind(wx.EVT_BUTTON, self.on_create_data_button, create_data_button)
        self.Bind(wx.EVT_BUTTON, self.on_send_data_button, send_data_button)
	
        sizer_int.AddGrowableCol(1)
        panel_int.SetSizer(sizer_int)
	panel_int.Layout()
        sizer1.Add(panel_system, 10, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=10)
        sizer1.Add(tcu_panel, 7, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=10)
        panel_left.SetSizer(sizer1)
        page1Lbl = wx.StaticText(panel_right, -1, _("页索引"))
        page1Lbl.SetFont(wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName=fname))
        page1Lbl.SetForegroundColour(wx.WHITE)
        self.lo = wx.ComboBox(panel_right, style=wx.CB_DROPDOWN | wx.TE_PROCESS_ENTER | wx.CB_READONLY)
        for i in range(0, 256):
            self.lo.Append('%d' % (i + 1))

        self.lo.SetSelection(0)
        self.lo.SetBackgroundColour(int_addr_col)
        self.lo.SetForegroundColour('#32CC32')
        self.Bind(wx.EVT_COMBOBOX, self.on_lo, self.lo)
        self.set_page_index()
        right_defButton = buttons.GenButton(panel_right, -1, label=_("说明"), size=(80, 22))
        self.Bind(wx.EVT_BUTTON, self.on_right_defButton, right_defButton)

        code_button = buttons.GenButton(panel_right, -1, label=_("间接指令"), size=(80, 22))
        self.Bind(wx.EVT_BUTTON, self.on_code_button, code_button)
        self.buff = buffTextCtrl(panel_right, self)
        self.v_scroll = v_scroll(panel_right, win=self, ctr=self.buff)
        wx.CallAfter(self.buff.resize, 0)
        sizer2 = wx.GridBagSizer()
        sizer2.Add(self.buff, pos=(0, 0), flag=wx.EXPAND)
        sizer2.Add(self.v_scroll, pos=(1, 0), span=(1, 2), flag=wx.EXPAND)
        sizer2.AddGrowableCol(0)
        sizer2.AddGrowableRow(0)
        

        r_sizer = wx.BoxSizer(wx.VERTICAL)
        s_sizer = wx.BoxSizer(wx.VERTICAL)
        t_sizer = wx.BoxSizer(wx.HORIZONTAL)
        t_sizer.Add(page1Lbl, 0, wx.RIGHT, border=10)
        t_sizer.Add(self.lo, 0, wx.RIGHT, border=10)
        t_sizer.Add(right_defButton, 0, wx.LEFT, border=30)
        t_sizer.Add(code_button, 0, wx.LEFT, border=100)
        s_sizer.Add(panel_int, 0, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=10)
        s_sizer.Add((0, 0), 0, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=10)
        s_sizer.Add(t_sizer, 0, wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=10)
        s_sizer.Add(sizer2, 1, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=10)
        r_sizer.Add(s_sizer, 1, wx.EXPAND | wx.TOP | wx.LEFT | wx.RIGHT | wx.BOTTOM, border=20)
        panel_right.SetSizer(r_sizer)
        self.SetSizer(sizer)
        self.Fit()
        self.SetSize((1000, 600))
        self.Bind(wx.EVT_CLOSE, self.close, self)
        #self.frush = page_frush(self)
        #self.frush.start()
	self.update_obj = Update(self)
	thread.start_new_thread(self.add_thread,())
    def add_thread(self):
	self.win.register_thread_func(self, self.update)

    def on_send_data_button(self, event):
	    value = self.data_ctrl.GetValue()
	    data_str_list = value.split(' ')
	    data_list = [int(data_str, 16) for data_str in data_str_list]
	    data_str = [chr(data) for data in data_list]

	    config = conf.GetGlobalConfig()
            if config == None:
        	return False
    	    else:
        	obj = config.get_obj_by_class(ObjCls[1])
		data_str = ''.join(data_str)
		try:
        		SkyEyeWriteDevice(obj, channel[0], data_str, len(data_str))
		except:
			pass
		self.data_ctrl.Clear()



	    
    def on_create_data_button(self, event):
	    self.data_ctrl.Clear()
	    data_list= [0xdc, 0x32, 0x3c, 0x3c]
	    d = [random.randint(0, 0xff) for i in range(0, 18)]
	    data_list.extend(d)
	    data_str = ''
	    for data in data_list:
		    data_str = ''.join([data_str, ' ', "%02x" % data])
	    data_str = data_str.upper()
	    self.data_ctrl.AppendText(data_str[1:])
	    
	    
    def update(self):
	    self.update_obj.update()

    
    def on_code_button(self, event):
	    if self.indirect == None:
	        self.indirect = indirect_code(self, self)
	        self.indirect.Show()

    def close_sub(self):
	if self.doc != None:
		self.doc.close(0)
	if self.indirect != None:
		self.indirect.close(0)
    def on_lo(self, event):
        page = self.lo.GetValue()
        self.buff.update_addr(page)


    def set_status(self, regvalue):
        bit = (regvalue & ALOW) >> 3
        self.autoShow.SetBackgroundColour(self.col[bit])
        self.autoName.SetLabel(self.auto_s[bit])
        bit = (regvalue & LOST) >> 6
        self.starShow.SetBackgroundColour(self.col[bit])
        self.starName.SetLabel(self.star_s[bit])
        bit = (regvalue & STATE) >> 7
        self.rightShow.SetBackgroundColour(self.col[bit])
        self.rightName.SetLabel(self.right_s[bit])
        bit = (regvalue & ABFLAG) >> 15
        self.a_bShow.SetBackgroundColour(self.col[bit])
        self.a_bFlag.SetLabel(self.a_b_s[bit])
        self.autoShow.Refresh()
        self.starShow.Refresh()
        self.rightShow.Refresh()
        self.a_bShow.Refresh()

    def destroy(self):
        self.win.custom_close_self(self)
        self.Destroy()

    def remove(self):
        self.win.remove_thread_func(self)
        self.win.custom_close_self(self)
	self.Destroy()
	    
    def close(self, event):
	thread.start_new_thread(self.remove,())

    def Close(self):
	thread.start_new_thread(self.remove,())

    def set_sys_info(self, info):
	s = _("系统消息:") + '  ' + info
        self.statusbar.SetStatusText(s, 0)

    def set_dig_mesg(self, mesg):
        s = _("数字量遥测:") + '      ' + mesg
        self.statusbar.SetStatusText(s, 2)

    def set_connect_status(self, status):
        s = _("硬件链接状态:") + '  ' + status
        self.statusbar.SetStatusText(s, 1)

    def on_tcua_Button(self, event):
        value_str = self.tcuaTextctr.GetValue()
        self.tcuaTextctr.Clear()
        try:
            value = int(value_str, 16)
        except:
            return

        context = _("TCU A 发送成功 %x 成功!" % (value))
        context_err =  _("TCU A 发送成功 %x 失败!" % (value))
        if self.connect == True:
            ret = tcua_send(value)
            if ret == True:
                self.set_dig_mesg(context)
            else:
                self.set_dig_mesg(context_err)
        else:
            self.set_dig_mesg(context_err)

    def on_tcua_input(self, event):
        value = self.tcuaTextctr.GetValue()
        new_font = wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        self.tcuaTextctr.SetStyle(0, len(value), wx.TextAttr(wx.GREEN, wx.NullColor, new_font))

    def on_tcub_Button(self, event):
        value_str = self.tcubTextctr.GetValue()
        self.tcubTextctr.Clear()
        try:
            value = int(value_str, 16)
        except:
            return

 	context = _("TCU A 发送成功 %x 成功!" % (value))
 	context_err = _("TCU A 发送成功 %x 失败!" % (value))
        if self.connect == True:
            ret = tcub_send(value)
            if ret == True:
                self.set_dig_mesg(context)
            else:
                self.set_dig_mesg(context_err)
        else:
            self.set_dig_mesg(context_err)

    def on_tcub_input(self, event):
        value = self.tcubTextctr.GetValue()
        new_font = wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        self.tcubTextctr.SetStyle(0, len(value), wx.TextAttr(wx.GREEN, wx.NullColor, new_font))

    def set_page_index(self):
        start = self.page_base
        st = '%d' % start
        st = '%d' % (start + 19)

    def init_addr_index(self):
        pass

    def on_right_defButton(self, event):
	    if self.doc == None:
	    	self.doc = gp_doc(self, self) 
		self.doc.Show()
        
    def on_int_inButton(self, event):
        number_str = self.cb.GetValue()
        self.cb.Clear()
        try:
            irq_data = int(number_str, 16)
        except:
            st = _("输入有误!")
            self.log.AppendText('%s\n' % st)
            new_font = wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
            value = self.log.GetValue()
            self.log.SetStyle(0, len(value), wx.TextAttr(wx.GREEN, wx.NullColor, new_font))
            event.Skip()
            return

        if self.connect == True:
            send_interrupt(0, irq_data)
            st = _("通道1 注数　0x%x 发送成功!" % irq_data)
        else:
            st = _("通道1 注数　0x%x 发送失败!" % irq_data)
        self.log.AppendText('%s\n' % st)
        new_font = wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        value = self.log.GetValue()
        self.log.SetStyle(0, len(value), wx.TextAttr(wx.GREEN, wx.NullColor, new_font))
        event.Skip()

    def on_int_inButton2(self, event):
        number_str = self.cb2.GetValue()
        self.cb2.Clear()
        try:
            irq_data = int(number_str, 16)
        except:
            st = _("输入有误!")
            self.log.AppendText('%s\n' % st)
            new_font = wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
            value = self.log.GetValue()
            self.log.SetStyle(0, len(value), wx.TextAttr(wx.GREEN, wx.NullColor, new_font))
            event.Skip()
            return

        if self.connect == True:
            send_interrupt(1, irq_data)
            st = _("通道2 注数　0x%x 发送成功!" % irq_data)
        else:
            st = _("通道2 注数　0x%x 发送失败!" % irq_data)
        self.log.AppendText('%s\n' % st)
        new_font = wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        value = self.log.GetValue()
        self.log.SetStyle(0, len(value), wx.TextAttr(wx.GREEN, wx.NullColor, new_font))
        event.Skip()

    def on_input_int(self, event):
        self.cb.SetStyle(0, 1, wx.TextAttr(wx.GREEN, wx.NullColor, wx.NullFont))
        event.Skip()

    def on_input_int2(self, event):
        self.cb2.SetStyle(0, 1, wx.TextAttr(wx.GREEN, wx.NullColor, wx.NullFont))
        event.Skip()


    def on_stop_Button(self, event):
        number_str = self.stop.GetValue()
        self.stop.Clear()
        try:
           stop_data = int(number_str, 16)
        except:
            st = _("输入有误!")
            self.log.AppendText('%s\n' % st)
            new_font = wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
            value = self.log.GetValue()
            self.log.SetStyle(0, len(value), wx.TextAttr(wx.GREEN, wx.NullColor, new_font))
            event.Skip()
            return

        if self.connect == True:
	    send_stop_data(stop_data)
            st = _("停止　0x%x 发送成功!" % stop_data)
        else:
            st = _("停止　0x%x 发送失败!" % stop_data)
        self.log.AppendText('%s\n' % st)
        new_font = wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        value = self.log.GetValue()
        self.log.SetStyle(0, len(value), wx.TextAttr(wx.GREEN, wx.NullColor, new_font))
        event.Skip()

    def on_input_stop(self, event):
        self.stop.SetStyle(0, 1, wx.TextAttr(wx.GREEN, wx.NullColor, wx.NullFont))
        event.Skip()


    def OnShowPopupTransient(self, event):
        win = TransientPopup(self, wx.SIMPLE_BORDER)
        btn = event.GetEventObject()
        pos = btn.ClientToScreen((0, 0))
        sz = btn.GetSize()
        win.Position(pos, (0, sz[1]))
        win.Popup()

    def bit_change(self, bit):
        if bit == 0:
            bit = 1
        else:
            bit = 0
        return bit

    def on_a_bShow(self, event):
        write_status(ABFLAG)

    def on_rightShow(self, event):
        write_status(STATE)

    def on_starShow(self, event):
        write_status(LOST)

    def on_autoShow(self, event):
        write_status(ALOW)


FileMenu = _("遥测控制")
WinCls = MyFrame
if __name__ == '__main__':
    app = wx.PySimpleApp()
    Memdlg = MyFrame()
    Memdlg.Show()
    app.MainLoop()
