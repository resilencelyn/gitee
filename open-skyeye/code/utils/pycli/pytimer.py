from skyeye_common_module import *
import _thread
import time
from conf import *


RUNNING = False

def set_running(mode):
	global RUNNING
	RUNNING = mode

__SLEEP_TIME 	= 0.0001

TMR_ONE_SHOT 	= 0
TMR_PERIOD	= 1

ThreadHandle = None
class Tmr():
	def __init__(self, cpu, ms, mode, callback, data):
		self.cpu 	      = cpu
		self.ms 	      = ms
		self.mode 	      = mode
		self.callback	  = callback
		self.data 	      = data
		self.tmr_handler  = 0
		self.__TmrCreate()

	def __TmrCheckHit(self):
		return PythonTmrCheckHit(self.tmr_handler)
	def __TmrCreate(self):
		if(not self.tmr_handler):
			self.tmr_handler = PythonTmrCreate(self.cpu, self.ms, self.mode)
			if(not self.tmr_handler):
				raise Exception("TMR class create fail.")
	def __TmrProcessOk(self):
		PythonTmrProcessOk(self.tmr_handler)
	def TmrDelete(self):
		return PythonTmrDelete(self.tmr_handler)
	def CheckAndProcess(self):
		if(not self.tmr_handler):
			return
		if(self.__TmrCheckHit()):
			#try:
			self.callback(self.data)
			#except Exception as e:
				#LogReport('In %s:\n\r\r%s'%(self.callback.__name__, e))
			self.__TmrProcessOk()

def __PyTmrCheck():
	while True:
		config = GetGlobalConfig()
		if(config and RUNNING == True):
			for tmr in config.TmrList:
				tmr.CheckAndProcess()
		time.sleep(__SLEEP_TIME)

def PyTmrInit():
	global ThreadHandle
	if(ThreadHandle == None):
		ThreadHandle= _thread.start_new_thread(__PyTmrCheck, ())

def PyTmrCreate(cpu, ms, mode, callback, data):
	config = GetGlobalConfig()
	if(config):
		tmr = Tmr(cpu, ms, mode, callback, data)
		config.TmrList.append(tmr)
		return tmr

def PyTmrDelete(tmr):
	config = GetGlobalConfig()
	if(config):
		ret = tmr.TmrDelete()
		config.TmrList.remove(tmr)
		del tmr
		return ret
	return False

################################################
class WatchOnPc():
	def __init__(self, cpu, addr, callback, data):
		self.cpu	       = cpu
		self.addr	       = addr
		self.callback	   = callback
		self.data 	       = data
		self.watch_handler = 0
		self.__SetWatchOnPc()

	def __WatchCheckHit(self):
		return PythonTmrCheckHit(self.watch_handler)
	def __SetWatchOnPc(self):
		if(not self.watch_handler):
			self.watch_handler = SkyEyeSetWatchOnPc(self.cpu,self.addr)
			if(not self.watch_handler):
				raise Exception("WatchOnPc class create fail.")
	def __WatchProcessOk(self):
		PythonTmrProcessOk(self.watch_handler)
	def WatchDelete(self):
		SkyEyeUnWatchOnPc(self.cpu,self.addr)
	def CheckAndProcess(self):
		if(not self.watch_handler):
			return
		if(self.__WatchCheckHit()):
			#try:
			self.callback(self.data)
			#except Exception as e:
				#LogReport('In %s:\n\r\r%s'%(self.callback.__name__, e))
			self.__WatchProcessOk()

def PySetWatchOnPc(cpu, addr, callback, data):
	config = GetGlobalConfig()
	if(config):
		watch = WatchOnPc(cpu, addr, callback, data)
		config.TmrList.append(watch)
		return watch

def PyUnWatchOnPc(watch):
	config = GetGlobalConfig()
	if(config):
		watch.WatchDelete()
		config.TmrList.remove(watch)
		del watch
		return True
	return False

##################################################
class WatchOnMem():
	def __init__(self, ms_name, m_type, addr, data_type, length, callback, data):
		self.ms_name	   = ms_name
		self.m_type        = m_type
		self.addr	       = addr
		self.length        = length
		self.data_type     = data_type
		self.callback	   = callback
		self.data 	       = data
		self.watch_handler = 0
		self.__SetWatchOnPc()

	def __WatchCheckHit(self):
		return PythonTmrCheckHit(self.watch_handler)
	def __SetWatchOnPc(self):
		if(not self.watch_handler):
			self.watch_handler = SkyEyeSetWatchOnMem(self.ms_name, self.m_type, self.addr, self.data_type, self.length)
			if(not self.watch_handler):
				raise Exception("WatchOnMem class create fail.")
	def __WatchProcessOk(self):
		PythonTmrProcessOk(self.watch_handler)
	def WatchDelete(self):
		SkyEyeUnWatchOnMem(self.ms_name, self.m_type, self.addr, self.length)
	def CheckAndProcess(self):
		if(not self.watch_handler):
			return
		if(self.__WatchCheckHit()):
			#try:
			self.callback(self.data)
			#except Exception as e:
				#LogReport('In %s:\n\r\r%s'%(self.callback.__name__, e))
			self.__WatchProcessOk()

def PySetWatchOnMem(ms_name, m_type, addr, data_type, length, callback, data):
	config = GetGlobalConfig()
	if(config):
		watch = WatchOnMem(ms_name, m_type, addr, data_type, length, callback, data)
		config.TmrList.append(watch)
		return watch

def PyUnWatchOnMem(watch):
	config = GetGlobalConfig()
	if(config):
		watch.WatchDelete()
		config.TmrList.remove(watch)
		del watch
		return True
	return False
