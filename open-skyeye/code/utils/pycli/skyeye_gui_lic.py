#coding:utf-8
import wx
import os,re,time
import skyeye_license as se_lic
import se_path
import threading
import _thread

class local_lic(wx.Panel):
	def __init__(self,parent):
		wx.Panel.__init__(self,parent.note)
		self.parent=parent
		self.show_flag = False
		self.local_box=wx.BoxSizer(wx.VERTICAL)
		mac_temp = wx.BoxSizer(wx.HORIZONTAL)
		mac_tit = wx.StaticText(self,-1,"网卡")
		self.mac_choice = wx.Choice(self,-1)
		self.mac_choice.Enable(False)
		self.mac_choice.SetMinSize((100,25))
		self.Bind(wx.EVT_CHOICE,self.select_mac,self.mac_choice)
		mac_temp.Add(mac_tit,0,wx.ALL|wx.EXPAND,3)
		mac_temp.Add(self.mac_choice,1,wx.ALL|wx.EXPAND,0)

		self.file_tc=wx.TextCtrl(self,-1,size=(420,25),style=wx.TE_RICH2 | wx.TE_MULTILINE | wx.TE_READONLY)
		self.img_box = wx.BoxSizer(wx.HORIZONTAL)
		self.show_image()
		but_box=wx.BoxSizer(wx.HORIZONTAL)
		imp_but=wx.Button(self,-1,"导入License文件")
		self.reg_but=wx.Button(self,-1,"注册")
		cancel_but=wx.Button(self,-1,"取消")
		self.reg_but.Enable(0)
		self.Bind(wx.EVT_BUTTON,self.import_lic_file,imp_but)
		self.Bind(wx.EVT_BUTTON,self.register_lic,self.reg_but)
		self.Bind(wx.EVT_BUTTON,parent.OnCloseWindow,cancel_but)
		but_box.Add((0,0),1,wx.ALL|wx.EXPAND,5)
		but_box.Add(imp_but,0,wx.ALL|wx.EXPAND,5)
		but_box.Add(self.reg_but,0,wx.ALL|wx.EXPAND,5)
		but_box.Add(cancel_but,0,wx.ALL|wx.EXPAND,5)

		self.local_box.Add(mac_temp,1,wx.ALL|wx.EXPAND,5)
		self.local_box.Add(self.file_tc,1,wx.ALL|wx.EXPAND,5)
		self.local_box.Add(self.img_box,0,wx.ALL|wx.EXPAND,0)
		self.local_box.Add(but_box,0,wx.ALL|wx.EXPAND,5)
		self.SetSizer(self.local_box)
		self.Fit()
		self.mac_list = []
		self.licfile=None

	def select_mac(self,evt):
		index = self.mac_choice.GetCurrentSelection()
		hardware_info = se_lic.get_hardware_info(self.mac_list[index][1])
		if hardware_info[0] == False:
			message_dialog(hardware_info[1])
			return
		self.refresh_image(hardware_info[1],hardware_info[2])

	def refresh_image(self,cipher,image):
		if self.show_flag == False or \
			self.parent.close == True:
			return
		self.file_tc.SetValue(cipher)
		img=wx.Image(image,wx.BITMAP_TYPE_ANY)
		img=img.Scale(150,150).ConvertToBitmap()
		self.lic_img.SetBitmap(img)
		self.img_box.Layout()

	def get_system_info(self):
		self.mac_list = se_lic.get_mac_info()
		self.mac_choice.SetItems([mac[0] for mac in self.mac_list])
		self.mac_choice.SetSelection(0)
		self.mac_choice.Enable(True)
		hardware_info = se_lic.get_hardware_info()
		if hardware_info[0] == False:
			message_dialog(hardware_info[1])
			return
		self.refresh_image(hardware_info[1],hardware_info[2])

	def show_image(self):
		if self.show_flag == True:
			return
		self.file_tc.SetValue("获取中请稍后...")
		img_path = se_path.GetPicPath("getting.png")
		img=wx.Image(img_path,wx.BITMAP_TYPE_ANY)
		img=img.Scale(150,150).ConvertToBitmap()
		self.lic_img=wx.StaticBitmap(self,-1,img)
		self.img_box.Add(self.lic_img,1,wx.ALL|wx.EXPAND,0)
		self.img_box.Layout()
		self.local_box.Layout()
		_thread.start_new_thread(self.get_system_info, () )
		self.show_flag = True

	def hide_image(self):
		if self.show_flag == False:
			return
		self.img_box.Hide(0)
		self.img_box.Remove(0)
		self.img_box.Layout()
		self.local_box.Layout()
		self.show_flag = False

	def display_license(self,filename):
		try:
			ret=se_lic.read_license_info(filename)
		except:
			ret=None
		if ret==None:
			message_dialog("无效的License文件")
			self.reg_but.Enable(0)
			return False
		else:
			data=""
			for i in ["start","end","cpu_list"]:
				data=data+("%s:\n\t%s\n\n"%(i.upper(),ret[i].replace(":","\n\t")))
			self.hide_image()
			self.file_tc.SetValue(data)
			self.reg_but.Enable(1)

	def import_lic_file(self,evt):
		dialog = wx.FileDialog(None, "License文件导入", "c:\\","", "license file(*.lic)|*.lic", wx.FD_OPEN)
		if dialog.ShowModal() == wx.ID_OK:
			self.licfile = dialog.GetPath()
			self.display_license(self.licfile)

	def register_lic(self,evt):
		se_lic.set_local_license(self.licfile)
		try:
			ret=se_lic.license_verify()
		except:
			ret=(False,"license verify call error")
		if ret[0]==False:
			message_dialog(ret[1])
			se_lic.clear_license_config()
			self.show_image()
			self.reg_but.Enable(0)
			return False
		else:
			message_dialog(ret[1])
			self.parent.OnCloseWindow(1)

class network_lic(wx.Panel):
	def __init__(self,parent):
		wx.Panel.__init__(self,parent.note)
		self.parent=parent
		net_box=wx.BoxSizer(wx.VERTICAL)
		ip_box=wx.BoxSizer(wx.HORIZONTAL)
		ip_t=wx.StaticText(self,-1,"IP   ")
		self.ip_tc=wx.TextCtrl(self,-1)
		ip_box.Add(ip_t,0,wx.ALL|wx.EXPAND,8)
		ip_box.Add(self.ip_tc,1,wx.ALL|wx.EXPAND,5)

		port_box=wx.BoxSizer(wx.HORIZONTAL)
		port_t=wx.StaticText(self,-1,"Port")
		self.port_tc=wx.TextCtrl(self,-1)
		port_box.Add(port_t,0,wx.ALL|wx.EXPAND,8)
		port_box.Add(self.port_tc,1,wx.ALL|wx.EXPAND,5)

		ctrl_box=wx.BoxSizer(wx.HORIZONTAL)
		link_but=wx.Button(self,-1,"连接")
		cancel_but=wx.Button(self,-1,"取消")
		self.Bind(wx.EVT_BUTTON,self.link_server,link_but)
		self.Bind(wx.EVT_BUTTON,parent.OnCloseWindow,cancel_but)
		ctrl_box.Add((0,0),1,wx.ALL|wx.EXPAND,5)
		ctrl_box.Add(link_but,0,wx.ALL|wx.EXPAND,5)
		ctrl_box.Add(cancel_but,0,wx.ALL|wx.EXPAND,5)

		net_box.Add(ip_box,0,wx.ALL|wx.EXPAND,5)
		net_box.Add(port_box,0,wx.ALL|wx.EXPAND,5)
		net_box.Add((0,0),1,wx.ALL|wx.EXPAND,5)
		net_box.Add(ctrl_box,0,wx.ALL|wx.EXPAND,5)
		self.SetSizer(net_box)
		self.Fit()

	def link_server(self,evt):
		ip=self.ip_tc.GetValue()
		port=self.port_tc.GetValue()
		if self.check_data(ip,port):
			se_lic.set_network_license(ip,port)
			try:
				ret=se_lic.license_verify()
			except:
				ret=(False,"license verify call error")
			if ret[0]==False:
				message_dialog(ret[1])
				se_lic.clear_license_config()
				return False
			else:
				message_dialog(ret[1])
				self.parent.OnCloseWindow(1)
			
		else:
			pass

	def check_data(self,ip,port):
		flag=True
		if not re.match(r"^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$", ip):
			self.ip_tc.SetBackgroundColour("#FFC0CB")
			flag=False
		else:
			self.ip_tc.SetBackgroundColour("#FFFFFF")
		if port=="" or not port.isdigit() or int(port)>65535 or int(port)<0:
			self.port_tc.SetBackgroundColour("#FFC0CB")
			flag=False
		else:
			self.port_tc.SetBackgroundColour("#FFFFFF")
		self.Refresh()
		return flag

class usb_check(threading.Thread):
	def __init__(self, win):
		threading.Thread.__init__(self)
		self.setDaemon(True)
		self.thread_stop = False
		self.win = win
	def run(self):
		while self.thread_stop == False:
			ret = se_lic.check_usb_connect()
			wx.CallAfter(self.win.set_connect_status,ret)
			time.sleep(0.5)
	def stop(self):
		self.thread_stop = True

class dog_lic(wx.Panel):
	def __init__(self,parent):
		wx.Panel.__init__(self,parent.note)
		self.parent = parent
		dog_box = wx.BoxSizer(wx.VERTICAL)
		insert_box = wx.BoxSizer(wx.HORIZONTAL)
		inser_t = wx.StaticText(self,-1,"USB ID")
		self.insert_tc = wx.TextCtrl(self,-1,"未连接",style = wx.TE_READONLY)
		insert_box.Add(inser_t,0,wx.ALL|wx.EXPAND,8)
		insert_box.Add(self.insert_tc,1,wx.ALL,5)

		ctrl_box = wx.BoxSizer(wx.HORIZONTAL)
		verify_but = wx.Button(self,-1,"验证")
		cancel_but = wx.Button(self,-1,"取消")
		self.Bind(wx.EVT_BUTTON,self.verify_dog,verify_but)
		self.Bind(wx.EVT_BUTTON,parent.OnCloseWindow,cancel_but)
		ctrl_box.Add((0,0),1,wx.ALL|wx.EXPAND,5)
		ctrl_box.Add(verify_but,0,wx.ALL|wx.EXPAND,5)
		ctrl_box.Add(cancel_but,0,wx.ALL|wx.EXPAND,5)

		dog_box.Add(insert_box,1,wx.ALL|wx.EXPAND,5)
		dog_box.Add(ctrl_box,0,wx.ALL|wx.EXPAND,5)
		self.SetSizer(dog_box)
		self.Fit()

	def set_connect_status(self,status):
		if self.parent.close == True:
			return
		text = "未连接"
		if status == True:
			text = "已连接"
		self.insert_tc.SetValue(text)
	def verify_dog(self,evt):
		se_lic.set_usb_license()
		try:
			ret = se_lic.license_verify()
		except:
			ret = (False,"license verify call error")
		if ret[0] == False:
			message_dialog(ret[1])
			return False
		else:
			message_dialog(ret[1])
			self.parent.OnCloseWindow(1)

class LicDialog(wx.Dialog):
	def __init__(self):
		wx.Dialog.__init__(self,None,-1,"产品注册")
		icon = wx.Icon(se_path.GetPicPath("lic.ico"), wx.BITMAP_TYPE_ICO)
		self.SetIcon(icon)
		self.main_panel=wx.Panel(self)
		self.main_box=wx.BoxSizer(wx.HORIZONTAL)
		self.note=wx.Notebook(self.main_panel)
		self.local_lic_page=local_lic(self)
		self.net_lic_page=network_lic(self)
		self.dog_lic_page=dog_lic(self)
		self.note.AddPage(self.local_lic_page,"本地License验证")
		self.note.AddPage(self.net_lic_page,"网络License验证")
		self.note.AddPage(self.dog_lic_page,"USBLicense验证")

		self.main_box.Add(self.note,1,wx.ALL|wx.EXPAND,10)
		self.main_panel.SetSizer(self.main_box)
		self.main_panel.Fit()
		self.Fit()

		self.Bind(wx.EVT_CLOSE,self.OnCloseWindow)
		self.uc = usb_check(self.dog_lic_page)
		self.uc.start()
		self.close = False

	def OnCloseWindow(self,evt):
		self.close = True
		self.uc.stop()
		self.Destroy()

def message_dialog(info):
	dlg = wx.MessageDialog(None,info,"提示", wx.OK)
	dlg.ShowModal()
	dlg.Destroy()

if __name__ == '__main__':
	app = wx.PySimpleApp()
	Regsdlg = LicDialog()
	Regsdlg.Show()
	app.MainLoop()
