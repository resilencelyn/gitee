#coding:utf-8
import wx
import time,copy
import operator
import fault_inject as fain
from threading import Thread
import wx.lib.buttons as buttons

MAX_FJ = 20

def vastr_to_int(va):
	try:
		if va[:2] == "0x":
			return int(va,16)
		else:
			return int(va,10)
	except:
		return None


class get_fault_list(Thread):
	def __init__(self,window):
		Thread.__init__(self)
		self.win=window
		self.ThreadStop=False
		self.fi_list=[]
	def stop(self):
		self.ThreadStop=True
	def run(self):
		while self.ThreadStop==False:
			temp=fain.skyeye_get_fj()
			if operator.eq(self.fi_list,temp) == False:
				self.fi_list=copy.copy(temp)
				wx.CallAfter(self.win.update_fault_list_ctrl,self.fi_list)
			time.sleep(0.1)

class FaultInjectFrame(wx.Frame):
	def __init__(self,parent):
		wx.Frame.__init__(self,parent,-1,"故障注入")
		self.parent=parent
		self.main_panel=wx.Panel(self)
		self.main_box=wx.BoxSizer(wx.HORIZONTAL)
		left_box_t=wx.StaticBox(self.main_panel,-1,"设备列表")
		left_box_sizer=wx.StaticBoxSizer(left_box_t,wx.HORIZONTAL)
		right_box_sizer=wx.BoxSizer(wx.VERTICAL)
		inject_box_t=wx.StaticBox(self.main_panel,-1,"故障注入")
		inject_box_sizer=wx.StaticBoxSizer(inject_box_t,wx.VERTICAL)
		show_box_t=wx.StaticBox(self.main_panel,-1,"故障列表")
		show_box_sizer=wx.StaticBoxSizer(show_box_t,wx.VERTICAL)
		###################left_box####################
		self.device_tree=wx.TreeCtrl(self.main_panel)
		self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnActivated, self.device_tree)
		left_box_sizer.Add(self.device_tree,1,wx.EXPAND|wx.ALL,10)
		###################right_box####################
		self.mode_name_list=['置0','置1','取反','无操作']
		self.mode_colour_list=['red','blue','yellow','white','red']
		in_mode_t=wx.StaticBox(self.main_panel,-1,"注入模式")
		in_mode_sizer=wx.StaticBoxSizer(in_mode_t,wx.HORIZONTAL)
		for i in range(0,len(self.mode_name_list)):
			colour=wx.StaticText(self.main_panel,-1)
			colour.SetBackgroundColour(self.mode_colour_list[i])
			colour.SetMinSize((15,15))
			name=wx.StaticText(self.main_panel,-1,self.mode_name_list[i])
			in_mode_sizer.Add(colour,0,wx.ALL,10)
			in_mode_sizer.Add(name,0,wx.TOP|wx.RIGHT,10)
		#**********************************************
		in_bit_box_t=wx.StaticBox(self.main_panel,-1,"注入位")
		in_bit_box_sizer=wx.StaticBoxSizer(in_bit_box_t,wx.VERTICAL)
		bit_box_t=wx.StaticBox(self.main_panel,-1,"")
		self.bit_box_sizer=wx.StaticBoxSizer(bit_box_t,wx.HORIZONTAL)
		self.bit_box_sizer.SetMinSize((450,220))
		value_box = wx.BoxSizer(wx.HORIZONTAL)
		value_t=wx.StaticText(self.main_panel,-1,"值：")
		self.bit_value = wx.TextCtrl(self.main_panel,-1)
		self.Bind(wx.EVT_TEXT, self.OnBitText,self.bit_value)
		value_box.Add(value_t,0,wx.EXPAND|wx.ALL,3)
		value_box.Add(self.bit_value,1,wx.EXPAND|wx.ALL,0)
		in_bit_box_sizer.Add(self.bit_box_sizer,1,wx.EXPAND|wx.ALL,5)
		in_bit_box_sizer.Add(value_box,0,wx.EXPAND|wx.ALL,5)
		#**********************************************
		inject_but=wx.Button(self.main_panel,-1,"注入故障")
		self.Bind(wx.EVT_BUTTON,self.inject_fault_but,inject_but)
		inject_box_sizer.Add(in_mode_sizer,0,wx.EXPAND|wx.ALL,10)
		inject_box_sizer.Add(in_bit_box_sizer,1,wx.EXPAND|wx.ALL,10)
		inject_box_sizer.Add(inject_but,0,wx.EXPAND|wx.ALL,10)
		#**********************************************
		list_tit=[("序号",40),('设备',150),('寄存器',91),('地址',91),('注入位',51),('模式',51)]
		self.fault_list=wx.ListCtrl(self.main_panel,-1,style=wx.LC_REPORT|wx.LC_HRULES|wx.LC_HRULES)
		for i in range(0,len(list_tit)):
			self.fault_list.InsertColumn(i,list_tit[i][0])
			self.fault_list.SetColumnWidth(i,list_tit[i][1])
		self.Bind(wx.EVT_LIST_ITEM_RIGHT_CLICK,self.OneActivated,self.fault_list)
		show_box_sizer.Add(self.fault_list,1,wx.EXPAND|wx.ALL,0)
		#**********************************************
		right_box_sizer.Add(inject_box_sizer,2,wx.EXPAND|wx.ALL,0)
		right_box_sizer.Add(show_box_sizer,1,wx.EXPAND|wx.ALL,0)
		#######################################
		self.main_box.Add(left_box_sizer,1,wx.EXPAND|wx.ALL,10)
		self.main_box.Add(right_box_sizer,2,wx.EXPAND|wx.ALL,10)
		#######################################
		self.main_panel.SetSizer(self.main_box)
		self.main_panel.Fit()
		self.Fit()
		#######################################
		self.all_device_info={}

		self.bit_show_mode=None
		self.bit_but_list=[]
		self.select_dev=None

		self.update_device_info()
		self.update_tree()
		self.gfl=get_fault_list(self)
		self.gfl.start()
		self.Bind(wx.EVT_CLOSE,self.OnCloseWindow)
	
	def update_device_info(self):
		self.all_device_info=fain.get_all_fault_inject_device_info()

	def update_tree(self):
		self.root=self.device_tree.AddRoot("System")
		for mach in sorted(self.all_device_info):
			m_id=self.device_tree.AppendItem(self.root,mach)
			for device in sorted(self.all_device_info[mach]):#[::-1]:
				d_id=self.device_tree.AppendItem(m_id,device)
				if self.all_device_info[mach][device]['type']=="IODev":
					for reg in sorted(self.all_device_info[mach][device]["register"]):
						self.device_tree.AppendItem(d_id,reg)
		cc_id=self.root
		while self.device_tree.ItemHasChildren(cc_id)==True:
			cc_id=self.device_tree.GetFirstChild(cc_id)[0]
		self.device_tree.SelectItem(cc_id,select=True)

	def get_on_mach_info(self,item_id,info=[]):
		name=self.device_tree.GetItemText(item_id)
		if name != "System":
			info.insert(0,name)
			f_id=self.device_tree.GetItemParent(item_id)
			return self.get_on_mach_info(f_id,info)
		else:
			return info

	def OnActivated(self,evt):
		if self.device_tree.ItemHasChildren(evt.GetItem())==False:
			self.select_dev= self.get_on_mach_info(evt.GetItem())
			self.update_bit_box()

	def create_btn(self,h,name="-"):
		bt=buttons.GenButton(self.main_panel,-1,name)
		bt.SetBackgroundColour("white")
		bt.Bind(wx.EVT_BUTTON,self.OnClick)
		bt.SetMinSize((10,h))
		bt.SetMaxSize((40,40))
		bt.Enable(0)
		self.bit_but_list.append(bt)
		return bt
	def OnClick(self,event):
		bt=event.GetEventObject()
		col=bt.GetBackgroundColour()
		l=self.mode_colour_list.index(col)
		bt.SetBackgroundColour(self.mode_colour_list[l+1])
		self.refresh_bit_value()

	def io_dev_show(self,count):
		rows=count//8+1+(lambda :1 if count%8>0 else 0)()
		but_sizer=wx.FlexGridSizer(rows,cols=9,hgap=5,vgap=5)
		for row in range(0,rows):
			for col in range(0,9):
				if row*9+col!=0 and (row*8+col-8)<=count:
					if row*9+col <9 or (row*9+9+col)%9==0:
						temp=str(row*8+col-1)
						if (row*9+col)%9==0:
							temp=str(row*8+col-8)+"-"+temp
						st=wx.StaticText(self.main_panel,-1,temp,style=wx.ALIGN_CENTER)
					else:
						st=self.create_btn(10)
				else:
					st=(0,0)
				but_sizer.Add(st,0,wx.EXPAND|wx.ALIGN_CENTER_VERTICAL)
		for i in range(1,9):
			but_sizer.AddGrowableCol(i)
		for i in range(1,rows):
			but_sizer.AddGrowableRow(i)
		return but_sizer
	
	def mem_dev_show(self):
		mem_box=wx.BoxSizer(wx.VERTICAL)
		addr_box=wx.BoxSizer(wx.HORIZONTAL)
		in_addr_tit=wx.StaticText(self.main_panel,-1,"注入地址：0x")
		self.inject_addr=wx.TextCtrl(self.main_panel,-1)
		self.Bind(wx.EVT_TEXT, self.OnText,self.inject_addr)
		self.inject_state=wx.StaticText(self.main_panel,-1)
		self.inject_state.SetBackgroundColour("red")
		self.inject_state.SetMinSize((15,15))
		addr_tit=wx.StaticText(self.main_panel,-1,"注入地址范围:")
		self.addr_ctrl=wx.StaticText(self.main_panel,-1)
		addr_box.Add(in_addr_tit,0,wx.LEFT|wx.TOP,2)
		addr_box.Add(self.inject_addr,0,wx.EXPAND|wx.ALL,0)
		addr_box.Add(self.inject_state,0,wx.EXPAND|wx.ALL,2)
		addr_box.Add(addr_tit,0,wx.LEFT|wx.TOP,2)
		addr_box.Add(self.addr_ctrl,0,wx.LEFT|wx.TOP,3)
		bit_sizer=wx.FlexGridSizer(rows=2,cols=8,hgap=5,vgap=5)
		for row in range(0,2):
			for col in range(0,8):
				if row==0:
					st=wx.StaticText(self.main_panel,-1,str(col),style=wx.ALIGN_CENTER)
				else:
					st=self.create_btn(30)
				bit_sizer.Add(st,1,wx.EXPAND)
		for i in range(0,8):
			bit_sizer.AddGrowableCol(i)
		mem_box.Add(addr_box,0,wx.ALL,10)
		mem_box.Add(bit_sizer,1,wx.EXPAND|wx.ALL,10)
		return mem_box

	def refresh_but_value(self,va,bit_count):
		if bit_count!=0:
			self.bit_value.Enable(True)
			self.bit_value.ChangeValue(hex(va).strip("L"))
			va=bin(int(va))[2:][::-1]
		else:
			self.bit_value.Enable(False)
		while len(va)<bit_count:
			va=va+"0"
		for i in range(0,len(self.bit_but_list)):
			if i>len(va) or len(va)==0:
				ff=["-",0]
			else:
				ff=[va[i],1]
			self.bit_but_list[i].SetLabel(ff[0])
			self.bit_but_list[i].Refresh()
			self.bit_but_list[i].Enable(ff[1])
			self.bit_but_list[i].SetBackgroundColour("white")

	def refresh_bit_value(self):
		va = ""
		for i in range(0,len(self.bit_but_list)):
			bit_va = self.bit_but_list[i].GetLabel()
			col=self.bit_but_list[i].GetBackgroundColour()
			if col == "red":
				bit_va = '0'
			elif col == "blue":
				bit_va = '1'
			elif col == "yellow":
				bit_va = str(~(int(bit_va))+2)
			va = bit_va + va
		self.bit_value.ChangeValue(hex(int(va,2)).strip("L"))

	def OnBitText(self,evt):
		bit_str_va = self.bit_value.GetValue()
		bit_va = vastr_to_int(bit_str_va)
		bit_count = len(self.bit_but_list)
		if 0 > bit_va or bit_va > 2**bit_count-1:
			for but in self.bit_but_list:
				but.SetBackgroundColour("white")
			if bit_str_va != "0x" and bit_str_va != "":
				wx.MessageBox('请输出正确的数值','提示',wx.OK|wx.ICON_EXCLAMATION)
		else:
			bit_va = bin(bit_va)[2:][::-1]
			while len(bit_va) < bit_count:
				bit_va = bit_va + "0"
			for i in range(0,bit_count):
				but_va = self.bit_but_list[i].GetLabel()
				if but_va != bit_va[i]:
					col = self.mode_colour_list[int(bit_va[i])]
					self.bit_but_list[i].SetBackgroundColour(col)
				else:
					self.bit_but_list[i].SetBackgroundColour("white")
		self.bit_box_sizer.Layout()

	def update_bit_box(self):
		dev=self.all_device_info[self.select_dev[0]][self.select_dev[1]]
		if self.bit_show_mode != None and dev["type"]!=self.bit_show_mode:
			self.bit_box_sizer.Hide(0)
			self.bit_box_sizer.Remove(0)
			self.bit_show_mode = None
			del self.bit_but_list[:]
		if dev["type"]=="IODev":
			reg=dev["register"][self.select_dev[2]]
			bit_count=reg["bits"]
			va=reg["value"]
			if self.bit_show_mode ==None:
				self.bit_box_sizer.Add(self.io_dev_show(bit_count),1,wx.EXPAND|wx.ALL,0)
			self.refresh_but_value(va,bit_count)
		elif dev["type"]=="MemDev":
			baseaddr=dev["baseaddr"]
			length=dev["length"]
			if self.bit_show_mode ==None:
				self.bit_box_sizer.Add(self.mem_dev_show(),1,wx.EXPAND|wx.ALL,0)
			self.addr_ctrl.SetLabel("0x%x-0x%x"%(baseaddr,baseaddr+length))
			self.inject_addr.ChangeValue("%x"%baseaddr)
			self.OnText(1)
		else:
			pass
		self.bit_box_sizer.Layout()
		self.bit_show_mode=dev["type"]
	def OnText(self,evt):
		try:
			addr=int(self.inject_addr.GetValue(),16)
		except:
			addr=None
			temp=["",0,"red"]
		if addr!=None:
			ret=fain.get_mem_device_addr_info(self.select_dev[0],addr)
			if ret !=False:
				dev=self.all_device_info[self.select_dev[0]][self.select_dev[1]]
				if addr>=dev["baseaddr"] and addr<=(dev["baseaddr"]+dev["length"]) :
					temp=[ret['value'],ret["bits"],"green"]
				else:
					temp=["",0,"red"]
		self.refresh_but_value(temp[0],temp[1])
		self.inject_state.SetBackgroundColour(temp[2])
		self.inject_state.Refresh()

	def inject_fault_but(self,evt):
		if self.parent.running==True:
			wx.MessageBox('请暂停天目','提示',wx.OK|wx.ICON_EXCLAMATION)
			return False
		dev=self.all_device_info[self.select_dev[0]][self.select_dev[1]]
		if dev["type"]=="IODev":
			addr=dev['register'][self.select_dev[2]]['addr']
		else:
			try:
				addr=int(self.inject_addr.GetValue(),16)
			except:
				return False
		for bit in range(0,len(self.bit_but_list)):
			col=self.bit_but_list[bit].GetBackgroundColour()
			if col !="white":
				mode=self.mode_colour_list.index(col)
				ret=fain.skyeye_set_fj(self.select_dev[0],self.select_dev[1],addr,bit,mode)
				if ret ==False:
					wx.MessageBox('注入故障失败!!\n可能失败的原因：\n  1.超出注入故障上限，最多注入%d个故障!!'%MAX_FJ,'提示',wx.OK|wx.ICON_EXCLAMATION)
					self.update_device_info()
					self.update_bit_box()
					return
		self.update_device_info()
		self.update_bit_box()

	def update_fault_list_ctrl(self,fi_list):
		self.fault_list.DeleteAllItems()
		for i in range(0,len(fi_list)):
			self.fault_list.InsertItem(i,str(i+1))
			temp=[]
			temp.append("%s.%s"%(fi_list[i][1],fi_list[i][0]))
			if fi_list[i][2]==None:
				temp.append("-")
			else:
				temp.append(fi_list[i][2])
			temp.append(hex(fi_list[i][3]).strip("L"))
			temp.append(str(fi_list[i][4]))
			temp.append(self.mode_name_list[fi_list[i][5]])
			for j in range(0,len(temp)):
				self.fault_list.SetItem(i,j+1,temp[j])

	def OneActivated(self,evt):
		pop_index = evt.GetIndex()
		menu = wx.Menu()
		mmi = wx.MenuItem(menu,-1,"删除")
		self.Bind(wx.EVT_MENU,lambda evt,index=pop_index : self.del_fault(evt,index),mmi)
		menu.Append(mmi)
		self.PopupMenu(menu)
	def del_fault(self,evt,index):
		del_item=self.gfl.fi_list[index]
		fain.skyeye_clear_fj(del_item[1],del_item[0],del_item[3],del_item[4],del_item[5])
		self.update_device_info()
		self.update_bit_box()
	def OnCloseWindow(self, event):
		self.gfl.stop()
		self.parent.fault_inject_frame=None
		self.Destroy()
	def Refurbish(self):
		self.update_device_info()
		self.update_bit_box()


if __name__=="__main__":
	app=wx.PySimpleApp()
	FaultInjectFrame(None).Show(True)
	app.MainLoop()
