#coding:utf-8
import wx,os
import time
import wx.lib.buttons as buttons
import conf, regs
import errormessage
import threading
from skyeye_common_module import *

def _(s):
	return unicode(s, 'gb2312')

class GetDeviceMsg(threading.Thread):
	def __init__(self,window):
		threading.Thread.__init__(self)
		self.setDaemon(True)
		self.ThreadStop=False
		self.win=window

	def run(self):
		while self.ThreadStop==False:
			#try:
			devname=self.win.CurrentDev.split(".")[1]
			ret=SkyEyeReadDataInjectDevice(devname)
			if ret!=None:
				self.updata(ret)
			#except:
			#	print "get error"
			time.sleep(2)
	
	def updata(self,msg):
		if self.win.StopReceive.IsChecked()==False and len(msg)>0:
			self.win.WriteValue("rec",msg)

	def stop(self):
		self.ThreadStop=True

class SendMsg(threading.Thread):
	def __init__(self,window):
		threading.Thread.__init__(self)
		self.setDaemon(True)
		self.ThreadStop=False
		self.win=window
		self.cycle=int(self.win.SendCycleC.GetLabel())

	def run(self):
		while self.ThreadStop==False:
			if self.win.SendThreadRun==True:
				for msg in self.win.SendMsgList:
					if self.win.SendThreadRun==False:
						break
					self.FileSendData(msg)
					if self.win.AutoSend.IsChecked()==True:
						time.sleep(0.001*self.cycle)
				self.win.SendMsgList=[]
				self.win.SendThreadRun=False
			time.sleep(0.1)

	def FileSendData(self,ID):
		ret=0
		FormatData=""
		for i in range(1,self.win.DataCol+1):
			row=self.win.DataText.GetItem(ID,i)
			data=row.GetText()
			FormatData=FormatData+DataCheck(data)
		if FormatData ==None:
			return
		devname=self.win.CurrentDev.split(".")[1]
		ret=SkyEyeWriteDataInjectDevice(devname,FormatData)
		if ret==1:
			self.win.WriteValue("send",FormatData)
			self.win.DataText.SetItemBackgroundColour(ID,"#FFA07A")

	def stop(self):
		self.ThreadStop=True

class inject(wx.Frame):
	def __init__(self,parent,args):
		wx.Frame.__init__(self,parent,-1,"数据注入")
		self.win=parent
		self.MainPanel=wx.Panel(self)
		self.RightPanel=wx.Panel(self.MainPanel)
		self.LeftPanel=wx.Panel(self.MainPanel)
		self.MainBoxSizer=wx.BoxSizer(wx.HORIZONTAL)
		self.LeftBoxSizer=wx.BoxSizer(wx.VERTICAL)
		self.RightBoxSizer=wx.BoxSizer(wx.VERTICAL)
		#页面左侧
		self.LConnBox=wx.StaticBox(self.LeftPanel,-1,"连接设置")
		self.LReceiveBox=wx.StaticBox(self.LeftPanel,-1,"接收区设置")
		self.LSendBox=wx.StaticBox(self.LeftPanel,-1,"发送区设置")
		self.LFileBox=wx.StaticBox(self.LeftPanel,-1,"文件区设置")
		self.LConnSz=wx.StaticBoxSizer(self.LConnBox,wx.VERTICAL)
		self.LReceiveSz=wx.StaticBoxSizer(self.LReceiveBox,wx.VERTICAL)
		self.LSendSz=wx.StaticBoxSizer(self.LSendBox,wx.VERTICAL)
		self.LFileSz=wx.StaticBoxSizer(self.LFileBox,wx.VERTICAL)
		#连接设置
		temp=wx.BoxSizer(wx.HORIZONTAL)
		DevText=wx.StaticText(self.LeftPanel,-1,"设备列表")
		self.cho=wx.Choice(self.LeftPanel,-1)
		self.Bind(wx.EVT_CHOICE,self.But_ChoiceDev,self.cho)
		temp.Add(DevText,0,wx.ALL|wx.EXPAND,5)
		temp.Add(self.cho,1,wx.ALL|wx.EXPAND,2)
		self.LConnSz.Add(temp,1,wx.ALL|wx.EXPAND,10)

		#接收设置
		self.RHexShow=wx.CheckBox(self.LeftPanel,-1,"格式化显示")
		self.StopReceive=wx.CheckBox(self.LeftPanel,-1,"暂停接收显示")
		self.SaveData=wx.Button(self.LeftPanel,-1,"保存数据")
		self.RCleanData=wx.Button(self.LeftPanel,-1,"清除显示")

		self.Bind(wx.EVT_CHECKBOX,self.Che_RFromatShow,self.RHexShow)
		self.Bind(wx.EVT_BUTTON,self.But_SaveData,self.SaveData)
		self.Bind(wx.EVT_BUTTON,self.But_RecCleanData,self.RCleanData)

		self.LReceiveSz.Add(self.RHexShow,1,wx.ALL|wx.EXPAND,5)
		self.LReceiveSz.Add(self.StopReceive,1,wx.ALL|wx.EXPAND,5)
		self.LReceiveSz.Add(self.SaveData,1,wx.ALL|wx.EXPAND,5)
		self.LReceiveSz.Add(self.RCleanData,1,wx.ALL|wx.EXPAND,5)

		#发送设置
		self.SHexShow=wx.CheckBox(self.LeftPanel,-1,"格式化显示")
		self.AutoSend=wx.CheckBox(self.LeftPanel,-1,"自动循环发送")
		self.Bind(wx.EVT_CHECKBOX,self.Che_SFromatShow,self.SHexShow)
		self.Bind(wx.EVT_CHECKBOX,self.Che_AutoSend,self.AutoSend)
		
		NumBox=wx.BoxSizer(wx.HORIZONTAL)
		SendNumT=wx.StaticText(self.LeftPanel,-1,"自动发送起始编号")
		self.SendNumC=wx.TextCtrl(self.LeftPanel,-1,value="1")
		NumBox.Add(SendNumT,0,wx.TOP,5)
		NumBox.Add(self.SendNumC,1,wx.ALL|wx.EXPAND,2)
		
		CycleBox=wx.BoxSizer(wx.HORIZONTAL)
		SendCycleT=wx.StaticText(self.LeftPanel,-1,"自动发送周期(ms)")
		self.SendCycleC=wx.TextCtrl(self.LeftPanel,-1,value="1000")
		CycleBox.Add(SendCycleT,0,wx.TOP,5)
		CycleBox.Add(self.SendCycleC,1,wx.ALL|wx.EXPAND,2)
		
		self.SCleanData=wx.Button(self.LeftPanel,-1,"清除显示")
		self.Bind(wx.EVT_BUTTON,self.But_SendCleanData,self.SCleanData)

		self.LSendSz.Add(self.SHexShow,1,wx.ALL|wx.EXPAND,5)
		self.LSendSz.Add(self.AutoSend,1,wx.ALL|wx.EXPAND,5)
		self.LSendSz.Add(NumBox,1,wx.ALL|wx.EXPAND,5)
		self.LSendSz.Add(CycleBox,1,wx.ALL|wx.EXPAND,5)
		self.LSendSz.Add(self.SCleanData,1,wx.ALL|wx.EXPAND,5)

		#文件设置
		self.FHexShow=wx.CheckBox(self.LeftPanel,-1,"格式化显示")
		self.LoadFile=wx.Button(self.LeftPanel,-1,"文件载入")
		self.FCleanData=wx.Button(self.LeftPanel,-1,"清除显示")

		self.Bind(wx.EVT_CHECKBOX,self.Che_FFromatShow,self.FHexShow)
		self.Bind(wx.EVT_BUTTON,self.But_FileCleanData,self.FCleanData)
		self.Bind(wx.EVT_BUTTON,self.But_LoadFile,self.LoadFile)

		self.LFileSz.Add(self.FHexShow,1,wx.ALL|wx.EXPAND,5)
		self.LFileSz.Add(self.LoadFile,1,wx.ALL|wx.EXPAND,5)
		self.LFileSz.Add(self.FCleanData,1,wx.ALL|wx.EXPAND,5)

		self.LeftBoxSizer.Add(self.LConnSz,0,wx.ALL|wx.EXPAND,10)
		self.LeftBoxSizer.Add(self.LReceiveSz,0,wx.ALL|wx.EXPAND,10)
		self.LeftBoxSizer.Add(self.LSendSz,0,wx.ALL|wx.EXPAND,10)
		self.LeftBoxSizer.Add(self.LFileSz,0,wx.ALL|wx.EXPAND,10)
		#页面右侧
		ShowBox=wx.BoxSizer(wx.HORIZONTAL)
		DataShowBox=wx.BoxSizer(wx.VERTICAL)
		self.RReceiveShowBox=wx.StaticBox(self.RightPanel,-1,"数据接收显示")
		self.RSendShowBox=wx.StaticBox(self.RightPanel,-1,"数据发送显示")
		self.RFileBox=wx.StaticBox(self.RightPanel,-1,"数据文件显示")
		self.RSendBox=wx.StaticBox(self.RightPanel,-1,"数据发送")
		
		self.RReceiveShowSz=wx.StaticBoxSizer(self.RReceiveShowBox,wx.VERTICAL)
		self.RSendShowSz=wx.StaticBoxSizer(self.RSendShowBox,wx.VERTICAL)
		self.RFileSz=wx.StaticBoxSizer(self.RFileBox,wx.VERTICAL)
		self.RSendSz=wx.StaticBoxSizer(self.RSendBox,wx.HORIZONTAL)
		
		#数据接收显示
		self.RecText=wx.ListCtrl(self.RightPanel,-1,style=wx.LC_REPORT|wx.LC_HRULES|wx.LC_VRULES)
		self.RReceiveShowSz.Add(self.RecText,1,wx.ALL|wx.EXPAND,5)
		
		#数据发送显示
		#self.SendText=wx.TextCtrl(self.RightPanel, -1,"sss\nsdf",style=wx.TE_READONLY|wx.TE_MULTILINE)
		self.SendText=wx.ListCtrl(self.RightPanel,-1,style=wx.LC_REPORT|wx.LC_HRULES|wx.LC_VRULES)
		self.RSendShowSz.Add(self.SendText,1,wx.ALL|wx.EXPAND,5)
		
		#数据文件显示
		#self.DataText=wx.TextCtrl(self.RightPanel, -1,"sss\nsdf",style=wx.TE_READONLY|wx.TE_MULTILINE)
		self.DataText=wx.ListCtrl(self.RightPanel,-1,style=wx.LC_REPORT|wx.LC_HRULES|wx.LC_VRULES)
		self.DataText.Bind(wx.EVT_LIST_ITEM_RIGHT_CLICK,self.OnItemActivated)
		self.RFileSz.Add(self.DataText,1,wx.ALL|wx.EXPAND,5)

		#数据发送
		DataT=wx.StaticText(self.RightPanel,-1,"数据")
		self.SendData=wx.TextCtrl(self.RightPanel, -1)
		self.SendBut=wx.Button(self.RightPanel, -1,"发送")
		self.Bind(wx.EVT_BUTTON,self.But_SendData,self.SendBut)
		self.RSendSz.Add(DataT,0,wx.TOP|wx.LEFT,14)
		self.RSendSz.Add(self.SendData,1,wx.ALL|wx.EXPAND,11)
		self.RSendSz.Add(self.SendBut,0,wx.ALL|wx.EXPAND,10)

		DataShowBox.Add(self.RReceiveShowSz,2,wx.ALL|wx.EXPAND,5)
		DataShowBox.Add(self.RSendShowSz,1,wx.ALL|wx.EXPAND,5)

		ShowBox.Add(DataShowBox,1,wx.ALL|wx.EXPAND,0)
		ShowBox.Add(self.RFileSz,1,wx.ALL|wx.EXPAND,5)
		
		self.RightBoxSizer.Add(ShowBox,1,wx.ALL|wx.EXPAND,0)
		self.RightBoxSizer.Add(self.RSendSz,0,wx.ALL|wx.EXPAND,10)
		#主
		self.MainBoxSizer.Add(self.LeftPanel,1,wx.ALL|wx.EXPAND,10)
		self.MainBoxSizer.Add(self.RightPanel,3,wx.ALL|wx.EXPAND,10)

		self.LeftPanel.SetSizer(self.LeftBoxSizer)
		self.LeftPanel.Fit()
		self.RightPanel.SetSizer(self.RightBoxSizer)
		self.RightPanel.Fit()
		self.MainPanel.SetSizer(self.MainBoxSizer)
		self.MainPanel.Fit()
		self.Fit()

		self.Bind(wx.EVT_CLOSE,self.OnCloseWindow)

		self.Value_Init()
		self.ControlInit()
		self.GetDevList()
		self.sm=SendMsg(self)
		self.sm.start()

	def OnCloseWindow(self, event):
		if self.GetThreadRun==True:
			self.gdm.stop()
		self.sm.stop()
		self.win.custom_close_self(self)
		self.Destroy()


	def Value_Init(self):
		self.SendMode1=False
		self.RecCol=0
		self.RecRow=0
		self.SendCol=0
		self.SendRow=0
		self.DataCol=0
		self.DataRow=0
		self.SendMsgList=[]
		self.CurrentDev=None
		self.GetThreadRun=False
		self.SendThreadRun=False

	def But_ChoiceDev(self,env):
		devname= self.cho.GetLabel()
		self.CurrentDev=devname
		self.DelWindowList("all")
		if self.SendMode1==True:
			self.RSendSz.Hide(self.CANSendSet)
			self.RSendSz.Remove(self.CANSendSet)
			self.RSendSz.Layout()
			self.SendMode1=False
		if self.AllBridgeDev[devname]=="can_bridge":
			self.CAN_DisplaySet()
			self.CanInit()
		elif self.AllBridgeDev[devname]=="rs422_bridge":
			self.Rs422_DisplaySet()
			self.Rs422Init()
		elif self.AllBridgeDev[devname]=="1553B_bridge":
			pass
		else:
			pass
		if self.GetThreadRun==False:
			self.gdm=GetDeviceMsg(self)
			self.gdm.start()
			self.GetThreadRun=True

	def CAN_DisplaySet(self):
		self.CAN_SendSet()
		titel=[("数据",500)]
		self.CreateListColumn("rec",titel)
		self.CreateListColumn("send",titel)
		self.CreateListColumn("file",titel)
		

	def Rs422_DisplaySet(self):
		titel=[("数据",500)]
		self.CreateListColumn("rec",titel)
		self.CreateListColumn("send",titel)
		self.CreateListColumn("file",titel)

	def But_SaveData(self,env):
		SaveFile=wx.FileDialog(self,"保存数据文件",os.getcwd(),'',"*.*", wx.SAVE|wx.OVERWRITE_PROMPT|wx.CHANGE_DIR)
		if SaveFile.ShowModal() == wx.ID_OK:
			temp=self.GetShowBoxData("rec")
			sf=open(SaveFile.GetPath(),"w")
			for i in temp:
				sf.write(DataToFormat(i))
				sf.write("\n")
			sf.close()
		SaveFile.Destroy()

	def But_RecCleanData(self,env):
		self.RecText.DeleteAllItems()
		self.RecRow=0
	
	def But_LoadFile(self,env):
		LoadFile=wx.FileDialog(self,"打开数据文件",os.getcwd(),'',"*.*", wx.OPEN|wx.CHANGE_DIR)
		if LoadFile.ShowModal() == wx.ID_OK:
			lf=open(LoadFile.GetPath(),"r")
			DataList = lf.readlines()
			lf.close()
			if len(DataList)!=0:
				self.DataText.DeleteAllItems()
				self.DataRow=0
				for data in DataList:
					#FormatData=data.strip().replace(" ","")
					FormatData=DataCheck(data)
					self.WriteValue("file",FormatData)

		LoadFile.Destroy()

	def But_SendCleanData(self,env):
		self.SendText.DeleteAllItems()
		self.SendRow=0

	def But_FileCleanData(self,env):
		self.DataText.DeleteAllItems()
		self.DataRow=0

	def But_SendData(self,env):
		data=self.SendData.GetLabel()
		if data!=None:
			FormatData=""
			if self.SendMode1==True:
				FormatData=self.InputCheck()
				if FormatData ==None:
					return
				ret=DataCheck(data,ord(FormatData[2:]))
				if ret==None:
					return
				FormatData=FormatData+ret
				#FormatData=FormatData
			else:
				FormatData=DataCheck(data)
				if FormatData ==None:
					return
			devname=self.CurrentDev.split(".")[1]
			ret=SkyEyeWriteDataInjectDevice(devname,FormatData)
			if ret==1:
				self.WriteValue("send",FormatData)
			#self.SendData.SetLabel("")

	def OnItemActivated(self,event):
		self.pop_index = event.GetIndex()
		menu = wx.Menu()
		mmi = wx.MenuItem(menu,-1,"发送")
		self.Bind(wx.EVT_MENU,self.RSendBut,mmi)
		menu.AppendItem(mmi)
		self.PopupMenu(menu)

	def RSendBut(self,event):
		self.SendMsgList.append(self.pop_index)
		self.SendThreadRun=True

	def Che_AutoSend(self,env):
		if env.IsChecked()==True:
			num=int(self.SendNumC.GetLabel())
			self.SendNumC.Enable(False)
			self.SendCycleC.Enable(False)
			for i in range(num-1,self.DataRow):
				self.SendMsgList.append(i)
			self.SendThreadRun=True
		else:
			self.SendNumC.Enable(True)
			self.SendCycleC.Enable(True)
			self.SendThreadRun=False
	
	def Che_RFromatShow(self,env):
		self.StopReceive.SetValue(True)
		temp=self.GetShowBoxData("rec")
		self.SetFormatShow("rec",env.IsChecked())
		for i in temp:
			self.WriteValue("rec",i)
		self.StopReceive.SetValue(False)

	def Che_SFromatShow(self,env):
		temp=self.GetShowBoxData("send")
		self.SetFormatShow("send",env.IsChecked())
		for i in temp:
			self.WriteValue("send",i)

	def Che_FFromatShow(self,env):
		temp=self.GetShowBoxData("file")
		self.SetFormatShow("file",env.IsChecked())
		for i in temp:
			self.WriteValue("file",i)
	
	def SetFormatShow(self,win,flag):
		self.DelWindowList(win)
		if flag==True:
			titel=[("设备ID",60),("帧类型",60),("长度",60),("数据1",60),("数据2",60),("数据3",60),("数据4",60),("数据5",60),("数据6",60),("数据7",60),("数据8",60)]
			self.CreateListColumn(win,titel)
		else:
			titel=[("数据",500)]
			self.CreateListColumn(win,titel)

	def CreateListColumn(self,ListName,ColList):
		if ListName == "rec":
			temp=self.RecText
			self.RecCol=len(ColList)
		elif ListName == "send":
			temp=self.SendText
			self.SendCol=len(ColList)

		elif ListName == "file":
			temp=self.DataText
			self.DataCol=len(ColList)
		else:
			return
		for num in range(0,len(ColList)):
			temp.InsertColumn(num,ColList[num][0])
			temp.SetColumnWidth(num,ColList[num][1])
		temp.InsertColumn(0,"序号")
		temp.SetColumnWidth(0,40)
		
	def WriteValue(self,ListName,DataList):
		if ListName == "rec":
			temp=self.RecText
			Ccount=self.RecCol
			Rcount=self.RecRow
			self.RecRow=self.RecRow+1
		elif ListName == "send":
			temp=self.SendText
			Ccount=self.SendCol
			Rcount=self.SendRow
			self.SendRow=self.SendRow+1

		elif ListName == "file":
			temp=self.DataText
			Ccount=self.DataCol
			Rcount=self.DataRow
			self.DataRow=self.DataRow+1
		else:
			return
		#temp.InsertStringItem(0,'tr')
		#temp.SetStringItem(0, 1,'Rcount')
		temp.InsertStringItem(Rcount,str(Rcount+1))
		if Ccount >1:
			for num in range(1,Ccount+1):
				if num >len(DataList):
					temp.SetStringItem(Rcount, num,"-")
				else:
					temp.SetStringItem(Rcount, num,str(hex(ord(DataList[num-1]))))
		else:
			datastr=""
			for data in DataList:
				datastr=datastr+"  "+str(hex(ord(data)))
			temp.SetStringItem(Rcount,1,datastr)
	def CAN_SendSet(self):
		self.CANSendSet=wx.BoxSizer(wx.HORIZONTAL)
		IdT=wx.StaticText(self.RightPanel,-1,"ID")
		self.CANId=wx.TextCtrl(self.RightPanel,-1,size=(30,-1))
		NormT=wx.StaticText(self.RightPanel,-1,"标准")
		self.CANNorm=wx.CheckBox(self.RightPanel,-1,"")
		LenT=wx.StaticText(self.RightPanel,-1,"长度(0~8)")
		self.CANLen=wx.TextCtrl(self.RightPanel,-1,size=(30,-1))
		
		self.CANSendSet.Add(IdT,0,wx.TOP,7)
		self.CANSendSet.Add(self.CANId,0,wx.TOP|wx.LEFT|wx.RIGHT|wx.EXPAND,4)
		self.CANSendSet.Add(NormT,0,wx.TOP|wx.RIGHT|wx.LEFT,7)
		self.CANSendSet.Add(self.CANNorm,0,wx.TOP|wx.RIGHT,9)
		self.CANSendSet.Add(LenT,0,wx.TOP|wx.LEFT,7)
		self.CANSendSet.Add(self.CANLen,0,wx.TOP|wx.LEFT,4)
		self.RSendSz.Insert(0,self.CANSendSet,0,wx.ALL|wx.EXPAND,9)
		self.RSendSz.Layout()
		self.SendMode1=True

	def DelWindowList(self,win):
		if win == "rec" or win == "all":
			self.RecText.DeleteAllItems()
			self.RecText.DeleteAllColumns()
			self.RecCol=0
			self.RecRow=0
		if win == "send" or win == "all":
			self.SendText.DeleteAllItems()
			self.SendText.DeleteAllColumns()
			self.SendCol=0
			self.SendRow=0
		if win == "file" or win == "all":
			self.DataText.DeleteAllItems()
			self.DataText.DeleteAllColumns()
			self.DataCol=0
			self.DataRow=0

	def GetDevList(self):
		self.AllBridgeDev={}
		Config=conf.GetGlobalConfig()
		if Config==None:
			return None
		MachList=Config.get_mach_list()
		for Mach in MachList:
			Cpu=Config.get_cpuname_by_mach(Mach)
			Memory_Space=Config.get_memoryspace_by_cpu(Cpu)
			DevList=Config.get_device_list_by_mach(Mach)
			for Dev in DevList:
				temp={}
				DevClsName=Config.get_device_classname(Mach,Dev)
				if "bridge" in DevClsName:
					DevName=Mach+"."+Dev
					self.AllBridgeDev[DevName]=DevClsName
					self.cho.Append(DevName)
	
	def GetShowBoxData(self,ListName):
		if ListName == "rec":
			temp=self.RecText
			Ccount=self.RecCol
			Rcount=self.RecRow
		elif ListName == "send":
			temp=self.SendText
			Ccount=self.SendCol
			Rcount=self.SendRow

		elif ListName == "file":
			temp=self.DataText
			Ccount=self.DataCol
			Rcount=self.DataRow
		else:
			return
		datalist=[]
		for i in range(0,Rcount):
			FormatData=""
			for j in range(1,Ccount+1):
				row=temp.GetItem(i,j)
				data=row.GetText()
				if data=="-":
					continue
				FormatData=FormatData+DataCheck(data)
			datalist.append(FormatData)
		return datalist
	
	def ControlInit(self):
		self.RHexShow.Enable(0)
		self.StopReceive.Enable(0)
		self.SaveData.Enable(0)
		self.RCleanData.Enable(0)
		self.SHexShow.Enable(0)
		self.AutoSend.Enable(0)
		self.SendNumC.Enable(0)
		self.SendCycleC.Enable(0)
		self.SCleanData.Enable(0)
		self.FHexShow.Enable(0)
		self.LoadFile.Enable(0)
		self.FCleanData.Enable(0)
		self.SendData.Enable(0)
		self.SendBut.Enable(0)
	
	def Rs422Init(self):
		self.RHexShow.Enable(0)
		self.StopReceive.Enable(1)
		self.SaveData.Enable(1)
		self.RCleanData.Enable(1)
		self.SHexShow.Enable(0)
		self.AutoSend.Enable(1)
		self.SendNumC.Enable(1)
		self.SendCycleC.Enable(1)
		self.SCleanData.Enable(1)
		self.FHexShow.Enable(0)
		self.LoadFile.Enable(1)
		self.FCleanData.Enable(1)
		self.SendData.Enable(1)
		self.SendBut.Enable(1)
	
	def CanInit(self):
		self.RHexShow.Enable(1)
		self.StopReceive.Enable(1)
		self.SaveData.Enable(1)
		self.RCleanData.Enable(1)
		self.SHexShow.Enable(1)
		self.AutoSend.Enable(1)
		self.SendNumC.Enable(1)
		self.SendCycleC.Enable(1)
		self.SCleanData.Enable(1)
		self.FHexShow.Enable(1)
		self.LoadFile.Enable(1)
		self.FCleanData.Enable(1)
		self.SendData.Enable(1)
		self.SendBut.Enable(1)
	
	def InputCheck(self):
		Id=self.CANId.GetLabel()
		Norm=0
		if self.CANNorm.IsChecked()==True:
			Norm=1
		Len=self.CANLen.GetLabel()
		if Id.isdigit()==False:
			retCode = wx.MessageBox('ID 输入为空或错误请重新输入！','提示',wx.OK|wx.ICON_EXCLAMATION)
			return None
		if Len.isdigit()==False:
			retCode = wx.MessageBox('长度输入为空或错误请重新输入！','提示',wx.OK|wx.ICON_EXCLAMATION)
			return None
		if int(Len)<0 or int(Len)>8:
			retCode = wx.MessageBox('长度输入超出范围请重新输入！','提示',wx.OK|wx.ICON_EXCLAMATION)
			return None

		return chr(int(Id))+chr(Norm)+chr(int(Len))

def DataToFormat(data):
	temp=""
	for i in data:
		temp=temp+str(hex(ord(i)))+' '
	return temp

def DataCheck(data,num=-1):
	datalist=data.strip().replace("  "," ").split(" ")
	if num!=-1:
		if num != len(datalist):
			retCode = wx.MessageBox('数据长度输入不在范围内请重新输入！','提示',wx.OK|wx.ICON_EXCLAMATION)
			return None
	FormatData=""
	for  i in datalist:
		if "0x" in i:
			FormatData=FormatData+chr(int(i,16))
		elif i>="a" and i<="z" or i>="A" and i<="Z":
			FormatData=FormatData+i
		elif i.isdigit() == True:
			FormatData=FormatData+chr(int(i,10))
		else :
			retCode = wx.MessageBox('数据格式错误请重新输入！','提示',wx.OK|wx.ICON_EXCLAMATION)
			"Format error"
			return None
	return FormatData



FileMenu = _("数据注入")
WinCls = inject


if "__main__" == __name__:
	app=wx.PySimpleApp()
	frame=inject()
	frame.Show(True)
	app.MainLoop()
