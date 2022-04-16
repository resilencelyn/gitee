#coding:utf-8
import wx
import os
import re
import time
import binascii
import skyeye_common_module as scm
import conf
import tools

def str_to_int(data):
	va = None
	try:
		if data[:2] == "0x":
			va = int(data,16)
		else:
			va = int(data,10)
	except:
		pass
	return va

class write_mem_to_x86(wx.Frame):
	def __init__(self,parent):
		wx.Frame.__init__(self,parent,-1,"加载二进制文件")
		self.par = parent
		self.main_panel = wx.Panel(self)
		self.main_box = wx.BoxSizer(wx.VERTICAL)
		file_box = wx.BoxSizer(wx.HORIZONTAL)
		cpu_tit = wx.StaticText(self.main_panel,-1,"处理器")
		self.cpu_choice = wx.Choice(self.main_panel,-1,choices=["rr","we"])
		self.cpu_choice.SetMinSize((100,25))
		file_tit = wx.StaticText(self.main_panel,-1,"文件")
		self.file_path = wx.TextCtrl(self.main_panel,-1)
		open_file_but = wx.Button(self.main_panel,-1,"打开")
		open_file_but.SetMaxSize((40,25))
		addr_tit = wx.StaticText(self.main_panel,-1,"加载地址")
		self.addr_ctrl = wx.TextCtrl(self.main_panel,-1,"0x")
		self.addr_ctrl.SetMaxSize((80,25))
		self.addr_ctrl.Enable(False)
		add_but = wx.Button(self.main_panel,-1,"添加")
		add_but.SetMaxSize((50,25))
		self.Bind(wx.EVT_BUTTON,self.open_file,open_file_but)
		self.Bind(wx.EVT_BUTTON,self.add_file,add_but)
		
		file_box.Add(cpu_tit,0,wx.TOP|wx.LEFT,8)
		file_box.Add(self.cpu_choice,0,wx.ALL,5)
		file_box.Add(file_tit,0,wx.TOP|wx.LEFT,8)
		file_box.Add(self.file_path,1,wx.ALL,5)
		file_box.Add(open_file_but,0,wx.ALL,5)
		file_box.Add(addr_tit,0,wx.TOP|wx.LEFT,8)
		file_box.Add(self.addr_ctrl,0,wx.ALL,5)
		file_box.Add(add_but,0,wx.ALL,5)
		
		file_list_box_tit = wx.StaticBox(self.main_panel,-1,"文件列表")
		file_list_box = wx.StaticBoxSizer(file_list_box_tit,wx.HORIZONTAL)
		self.file_list = wx.ListCtrl(self.main_panel,-1,style=wx.LC_REPORT|wx.LC_HRULES|wx.LC_VRULES)
		self.Bind(wx.EVT_LIST_ITEM_RIGHT_CLICK,self.OneActivated,self.file_list)
		self.file_list .InsertColumn(0,"加载处理器")
		self.file_list .SetColumnWidth(0,100)
		self.file_list .InsertColumn(1,"二进制文件")
		self.file_list .SetColumnWidth(1,400)
		self.file_list .InsertColumn(2,"加载地址")
		self.file_list .SetColumnWidth(2,100)
		file_list_box.Add(self.file_list,1,wx.EXPAND|wx.ALL,5)
		
		load_but_box = wx.BoxSizer(wx.HORIZONTAL)
		
		load_binary_but = wx.Button(self.main_panel,-1,"加载")
		load_binary_but.SetMaxSize((60,25))
		load_but_box.Add((0,0),1,wx.EXPAND|wx.ALL,5)
		
		load_but_box.Add(load_binary_but,0,wx.ALL,5)
		self.Bind(wx.EVT_BUTTON,self.load_binary,load_binary_but)

		self.main_box.Add(file_box,0,wx.EXPAND|wx.ALL,0)
		self.main_box.Add(file_list_box,1,wx.EXPAND|wx.ALL,5)
		self.main_box.Add(load_but_box,0,wx.EXPAND|wx.ALL,0)
		self.main_panel.SetSizer(self.main_box)
		self.main_panel.Fit()
		self.Fit()
		self.Center()
		self.Refurbish()
		self.Bind(wx.EVT_CLOSE,self.OnCloseWindow)
	
	def Refurbish(self):
		cpu_list = []
		cg = conf.GetGlobalConfig()
		if cg!=None:
			machlist = cg.get_mach_list()
			for mach in machlist:
				cpu_list = cpu_list + cg.get_cpuname_list_by_mach(mach)
		self.cpu_choice.SetItems(cpu_list)
		self.cpu_choice.SetSelection(0)
	
	def OneActivated(self,evt):
		pop_index = evt.GetIndex()
		menu = wx.Menu()
		mmi = wx.MenuItem(menu,-1,"删除")
		self.Bind(wx.EVT_MENU,lambda evt,index=pop_index : self.del_file(evt,index),mmi)
		menu.Append(mmi)
		self.PopupMenu(menu)

	def del_file(self,evt,index):
		self.file_list.DeleteItem(index)
				
	
	def open_file(self,evt):
		LoadFile=wx.FileDialog(self,"打开二进制文件",os.getcwd(),'',"*.*", wx.FD_OPEN|wx.FD_CHANGE_DIR)
		if LoadFile.ShowModal() == wx.ID_OK:
			file_name = LoadFile.GetPath()
			self.file_path.SetValue(file_name)
			if self.get_file_type(file_name) == "other":
				self.addr_ctrl.Enable(True)
			else:
				self.addr_ctrl.Enable(False)
		LoadFile.Destroy()
	
	def add_file(self,evt):
		cpuname = self.cpu_choice.GetString(self.cpu_choice.GetSelection())
		file_path = self.file_path.GetValue()
		file_addr = self.addr_ctrl.GetValue()
		if cpuname == "":
			self.log_out("未加载测试用例，请加载后重试!")
			return
		if os.path.exists(file_path) == False or not os.path.isfile(file_path):
			self.log_out("二进制文件未选择或不存在请重新选择!")
			return
		if self.get_file_type(file_path) == "other" and str_to_int(file_addr) == None:
			self.log_out("加载地址输入错误，请输入正确的十进制或十六进制数!")
			return
		if self.get_file_type(file_path) != "other":
			file_addr = "-"
		row = self.file_list.GetItemCount()
		self.file_list.InsertItem(row,cpuname)
		self.file_list.SetItem(row,1,file_path)
		self.file_list.SetItem(row,2,file_addr)

	def is_x86(self,cpuname):
		cg = conf.GetGlobalConfig()
		if cg!=None:
				cpu_class = cg.get_classname(cpuname)
				if cpu_class == "x86":
					return True
		return False
	
	def get_file_type(self,file_name):
		return tools.get_binary_file_type(file_name)
		
	def load_binary(self,evt):
		rows = self.file_list.GetItemCount()
		for row in range(0,rows):
			cpu_name = self.file_list.GetItemText(row,0)
			file_name = self.file_list.GetItemText(row,1)
			load_addr = str_to_int(self.file_list.GetItemText(row,2))
			f_type = self.get_file_type(file_name)
			fp = open(file_name,"rb")
			data = fp.read().decode()
			fp.close()
			if f_type == "elf":
				self.log_out("暂不支持该格式!")
				return
			elif f_type == "coff":
				self.log_out("暂不支持该格式!")
				return
			elif f_type == "hex":
				self.load_hex_file(cpu_name, data)
			else:
				self.load_bin_file(cpu_name, load_addr, data)
			self.log_out("文件:\n%s \n写入成功"%file_name,wx.ICON_INFORMATION)

	def write_memory_value(self,cpu_name,load_addr,data,count):
		try:
			scm.SkyEyeWriteMemory(cpu_name, load_addr,data.decode('latin1'),count)
		except:
			print ("write filed")
	
	def load_bin_file(self,cpu_name, load_addr, data):
		if self.is_x86(cpu_name) and load_addr % 0x1000 == 0:
			count = 0x1000
		else:
			count = 1
		for index in range(0,len(data),count):
			self.write_memory_value(cpu_name, load_addr + index, data[index : index + count],count)

	def load_hex_file(self, cpu_name, file_data):
		file_data_list = file_data.split("\n")
		d_base_addr = 0
		is_x86_flag = self.is_x86(cpu_name)
		for index in range(0,len(file_data_list)):
			info = file_data_list[index].strip(":\r ")
			length = int(info[0:2],16)
			addr = int(info[2:6],16)
			type = int(info[6:8],16)
			data_i = info[8:8+length*2]
			data_s = binascii.a2b_hex(data_i)
			if type == 4:
				d_base_addr = int(data_i,16) << 16
				continue
			elif type == 1:
				break
			elif type == 0:
				if is_x86_flag:
					self.write_memory_value(cpu_name,d_base_addr+addr,data_s,length)
				else:
					count = 1
					for i in range(0,length,count):
						self.write_memory_value(cpu_name,d_base_addr+addr+i,data_s[i:i+count],count)
			else:
				pass

	
	def OnCloseWindow(self, event):
		self.par.write_memory_frame=None
		self.Destroy()
	
	def log_out(self,message,icon = wx.ICON_EXCLAMATION):
		md = wx.MessageDialog(None, message, '提示', wx.OK|icon)
		md.ShowModal()
		md.Destroy()

if __name__ == "__main__":
	app = wx.App()
	wmtx = write_mem_to_x86()
	wmtx.Show()
	app.MainLoop()
