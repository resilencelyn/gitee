#!/usr/bin/env python
#-*-coding:utf-8-*-

import os
import sys
import time
import subprocess
import platform
import operator

import wx
from skyeye_bugreport_ui import BugReportFrame

g_outfile = None
g_time    = None

def is_windows():
	os_info = platform.system()
	if operator.eq(os_info, "Linux"):
		return False
	return True

def is_linux():
	os_info = platform.system()
	if operator.eq(os_info, "Linux"):
		return True
	return False

def PathProcess(path_str):
	if is_linux():
		return path_str.replace('\\', '/')
	else:
		return path_str.replace('/', '\\')

class MainApp(BugReportFrame):
	def __init__(self, parent):
		BugReportFrame.__init__(self, parent)
		self.dirname = None

		pngname = os.path.dirname(os.path.abspath(__file__)) + "/picture/bug.png"
		pngname = PathProcess(pngname)
		icon = wx.Icon(pngname, wx.BITMAP_TYPE_PNG)
		self.SetIcon(icon)
		#self.m_checkBox_restart.Hide()
		if is_linux():
			self.m_checkBox_restart.Enable(False)

	def SystemInfo(self, Logfile):
		global g_time
		return
		try:
			with open(Logfile, 'r+') as f:
				old = f.read()
				f.seek(0)
				#f.write('SkyEye 问题报告:\n出错时间: ')
				#f.write(g_time.decode('gbk').encode('utf-8') + '\n')
				#f.write('操作系统: \n')
				f.write(old)
		except IOError:
			pass

	def LoadLogFile(self, Logfile):
		if Logfile is None:
			return
		self.SystemInfo(Logfile)
		try:
			with open(Logfile, 'r') as f:
				logMsg = f.read()
				self.m_textCtrl_Log.Clear()
				self.m_textCtrl_Log.AppendText(logMsg)
				self.dirname = os.path.dirname(Logfile)
				self.m_staticText_path.SetLabel(self.dirname)
		except IOError:
			pass

	def OpenLogFileDir(self, event):
		if is_windows():
			FileManagerName = 'explorer '
		else:
			FileManagerName = 'xdg-open '
		if (self.dirname == None):
			return
		os.system(FileManagerName + PathProcess(self.dirname))

	def ButtonOK(self, event):
		if self.m_checkBox_restart.GetValue() == True:
			if is_windows():
				SkyeyeName = 'skyeye_ui.exe'
			else:
				SkyeyeName = 'skyeye'
			SkyeyeExec = os.path.dirname(os.path.abspath(__file__)) + '/' + SkyeyeName
			SkyeyeExec = PathProcess(SkyeyeExec)
			subprocess.Popen([SkyeyeExec]);
		self.Close()

def bugreport_main(outfile, timeStr):
	global g_outfile
	global g_time
	g_outfile = outfile
	g_time    = timeStr
	app   = wx.App(False)

	s_frame = MainApp(None)
	s_frame.Show(True)
	s_frame.LoadLogFile(g_outfile)

	#start the applications
	app.MainLoop()

if __name__=="__main__":
	bugreport_main(None, None)

