#coding:utf-8
import os
import re
import time
import cli_init
import cli
import platform
import mips
from conf import *
import threading
import psutil
from fuzzywuzzy import fuzz
from pre_conf_obj import *
import skyeye_common_module as scm
import se_path
import operator


cli_init.InitCommandList()
result_list=[]
ac=None

class autotest_command():
	def __init__(self,testname,enable = True,run_system = "all",test_branch = "new_framework"):
		global ac
		ac=self
		self.error_flag=True
		self.enable_flag=True
		self.autotest_run=True
		self.error_test=[]
		self.testname=testname
		self.dirname=os.getcwd()
		self.speed=0
		self.run_mode='default'
		self.start_mem=0.0
		self.end_mem=0.0
		self.count=0
		self.test_info=[self.dirname,self.testname]
		ret=test_enable(self.test_info,test_branch,enable,run_system)
		if ret!=True:
			self.error_test.append([self.error_reset(self.test_info),ret])
			self.error_flag=False
			self.enable_flag=False

	def init(self):
		if self.enable_flag!=True:
			return
		self.error_flag=True
		#self.speed=0
		self.run_mode='default'

	def run_script(self):
		if self.error_flag==False:
			return
		self.get_skyeye_running_mem("start")
		print ("\nLoad test cases: "+self.testname)
		try:
			cmd = cli.GetCommand("run-script")
			if cmd == None:
				print ("\nCan not find command: run-script\n")
				self.error_test.append([self.error_reset(self.test_info),'Error loading test cases'])
				return
			list = ParseParameter(self.testname)
			if cmd.run(list)==False:
				self.error_test.append([self.error_reset(self.test_info),'Error loading test cases'])
				return
		except:
			self.error_test.append([self.error_reset(self.test_info),'Error loading test cases'])
			return
	def set_cpu_mode(self,mode):
		if self.error_flag==False:
			return
		print ("\nSet CPU model: "+str(mode))
		try:
			self.run_mode=mode
			config = GetGlobalConfig()
			if config == None:
				return
			mach_list = config.get_mach_list()
			for mach in mach_list:
				cpu = config.get_cpuname_by_mach(mach)
				scm.SkyEyeCpuSetMode(cpu, mode)
		except:
			self.error_test.append([self.error_reset(self.test_info),'Set CPU mode error'])
			return
	def add_file_out_dev(self,con_objname,file_name):
		if self.error_flag==False:
			return
		print ("\nAdd file_output module: "+con_objname+"")
		try:
			obj1=pre_conf_obj("uart_file_autoadd%d"%self.count,'uart_file')
			obj1.set_attr('filename',file_name)
			obj1.connect(con_objname,'uart_file_intf')
			self.count += 1
		except:
			self.error_test.append([self.error_reset(self.test_info),'Add file_output module error'])
	def run(self,sleep_time=None):
		if self.error_flag==False:
			return
		print ("\nRunning tstcases")
		try:
			mipsn = mips.mips()
			mipsn.start()
			scm.SkyEyeRun()
			sleep_count = 0
			while self.autotest_run == True:
				if sleep_time != None:
					sleep_count = sleep_count + 0.1
					if sleep_count >= float(sleep_time):
						break
				if self.error_flag==False:
					break
				mips_dic = mips.get()
				for item in mips_dic.keys():
					if self.speed < mips_dic[item]:
						self.speed=mips_dic[item]
				time.sleep(0.1)
			mipsn.stop()
			print ('\nMax ips: %d'%(self.speed))
		except:
			self.error_test.append([self.error_reset(self.test_info),'Running tstcases error'])
	def stop(self):
		if self.error_flag==False:
			return
		print ("\nStop the testcase")
		try:
			scm.SkyEyeStop()
		except:
			self.error_test.append([self.error_reset(self.test_info),'Stop the test case error'])

	def compare(self,actual_out,expect_out,accuracy):
		if self.error_flag==False:
			return
		print ("\nResults the proofreading: "+actual_out)
		sj_output=actual_out
		try:
			fp1=open(sj_output,'r')
		except:
			self.error_test.append([self.error_reset(self.test_info),'The actual output file was not found'])
			return
		actual=fp1.read()
		fp1.close()
		actual=del_sign(actual)
		expect_out=del_sign(expect_out)
		if len(actual)==0:
			self.error_test.append([self.error_reset(self.test_info),'The actual output file is empty'])
			return
		if len(expect_out)==0:
			self.error_test.append([self.error_reset(self.test_info),'The desired output is empty'])
			return
		accuracy_list=[]
		accuracy_list.append(fuzz.ratio(actual,expect_out))
		accuracy_list.append(fuzz.partial_ratio(actual,expect_out))
		if accuracy!=100:
			accuracy_list.append(fuzz.token_set_ratio(actual,expect_out))
		max_accuracy = max(accuracy_list)
		if max_accuracy >= accuracy:
			print ("\nProofreading success")
			return True
		else:
			print ('\nCheck failure.',)
			print ("expect accuracy: %d%%  max actual accuracy: %d%%\n"%(accuracy,max_accuracy))
			self.error_test.append([self.error_reset(self.test_info),'Check failure'])
			return False
		
	def reset(self):
		if self.error_flag==False:
			self.error_flag=True
			return
		print ("\nReset")
		try:
			scm.SkyEyeReset()
			time.sleep(2)
			self.init()
		except:
			self.error_test.append([self.error_reset(self.test_info),'Reset error'])
		self.get_skyeye_running_mem("end")
	def output_result(self):
		temp=[]
		if len(self.error_test)>0:
			print ("Test failed: ",)
			print (self.error_test[-1][1])
			if self.error_test[-1] not in result_list:
				temp.append(self.error_test[-1][0])
				temp.append(self.error_test[-1][1])
		else:
			print ("\nThe test pass")
			temp.append(os.path.join(self.test_info[0],self.test_info[1]))
			temp.append("The test pass")
		temp.append(self.speed)
		temp.append(self.run_mode)
		if self.start_mem !=0.0 and self.end_mem !=0.0:
			leak_mem=self.end_mem-self.start_mem
			temp.append("%.2f"%leak_mem)
			print ("leak_mem: %.2f MB"%leak_mem)
		else:
			temp.append("None")
		result_list.append(temp)

	def error_reset(self,test_info):
		self.error_flag=False
		print ("\nTest aborted\n\n")
		scm.SkyEyeReset()
		time.sleep(2)
		return os.path.join(test_info[0],test_info[1])

	def quit(self):
		scm.SkyEyeQuit()

	def get_skyeye_running_mem(self,flag):
		p=psutil.Process(os.getpid())
		temp=p.memory_info().rss/1024.0/1024.0
		if flag == "start":
			self.start_mem=temp
		if flag == "end":
			self.end_mem=temp

def test_enable(test_info,branch,enable,run_system):
	os_info = platform.system()
	if operator.eq(os_info, "Linux"):
		if run_system == 'windows':
			return "This is a Windows use cases"
	else:
		if run_system == 'linux':
			return "This is a Linux use cases"
	if run_system not in ["windows","linux","all"]:
		return "run_system parameter error"
	if os.path.isfile(os.path.join(se_path.LogPath,"branch_info.txt")):
		bi=open(os.path.join(se_path.LogPath,"branch_info.txt"),'r')
		branch_info=bi.read()
		bi.close()
		if branch_info!=branch:
			return "Different branches to skip"
	if enable!=True:
		return "The test is not enabled"
	return True
		
def skyeye_quit():
	scm.SkyEyeQuit()

def del_sign(st):
	rmlist=['\t','\r',' ','\n']
	for i in rmlist:
		st=st.replace(i,'')
	return st
	
def ParseParameter(arg):
	arglist = []
	if arg == "":
		return arglist
	arglist = arg.split(' ')
	while 1:
		try:
			arglist.remove('')
		except:
			break
	return arglist

