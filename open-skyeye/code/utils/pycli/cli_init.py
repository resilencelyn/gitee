import os
import sys
import se_path


def InitCommandList():
	ErrorFlag = True
	filepath = se_path.CommandPath
	if not  filepath in sys.path:
		sys.path.append(filepath)
	
	ModuleList = []
	for root, dirs, files in os.walk(filepath):
		for name in files:
			if ".cpython" not in name:
				if name.endswith(".py") or name.endswith(".pyc"):
					list = os.path.splitext(name)
					name = list[0]
					ModuleList.append(name)
	for module in ModuleList:
		if not module in sys.modules:
			mod = __import__(module)
			try:
				ret = mod.InitCommand()
				if ret == False:
					ErrorFlag = False
			except:
				pass
	return  ErrorFlag
	

