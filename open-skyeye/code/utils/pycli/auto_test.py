#coding:utf-8
import os
import time
import sys
import se_path
import win_workspace as ws
from skyeye_autotest_command import *

class skyeye_auto_test():
	def __init__(self):
		del result_list[:]
		self.now_dir=os.getcwd()
		ws.load_module_from_ws()
	def test(self):
		for dirpath,dirnames,filenames in os.walk(self.now_dir):
			if self.now_dir==dirpath:
				if "skyeye_test.py" not in filenames and "skyeye_test.pyc" not in filenames:
					print ("There is no test scripts")
					skyeye_quit()
					sys.exit(1)
		if self.now_dir not in sys.path:
			sys.path.append(self.now_dir)
		mod=__import__('skyeye_test')

		ret=mod.test()
		ret.test_main()

		del sys.modules['skyeye_test']
		sys.path.remove(self.now_dir)
		if len(result_list)>0:
			test_log=open(os.path.join(se_path.LogPath,'test.log'),'a')
			for result in result_list:
				if result!=None:
					for count in range(0,len(result)):
						test_log.write(str(result[count]))
						if count < len(result)-1:
							test_log.write("->")
					test_log.write("\n")
			test_log.close()
		skyeye_quit()
		sys.exit(1)
