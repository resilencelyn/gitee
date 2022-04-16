import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *
import operator



def LoadFileRun(args):
	if len(args) < 3:
		errormessage.SetErrorInfo(0x400e0001, False, 3, len(args))
		raise
	py_address_str = args[2]
	try:
		if operator.eq(py_address_str[0:2], "0x") == True:
			py_address_uint = int(py_address_str, 16)
		else:
			py_address_uint = int(py_address_str, 10)
	except Exception as e:
		errormessage.SetErrorInfo(0x400e0000, False, e)
		raise
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x400e0003, False)
		raise
	if args[0] not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x400e0004, False, args[0])
		raise
	if not os.path.exists(args[1]):
		errormessage.SetErrorInfo(0x400e0002, False, args[1])
		raise
	try:
		ret = SkyEyeLoadFile(args[0], args[1], py_address_uint)
	except Exception as e:
		raise
	if ret == False:
		errormessage.SetErrorInfo(0x400e0000, False, "Failed to load file!")
		raise

def LoadFileSynopses():
	return  "load_file  <cpuname>  <filename>  <offset>"
def LoadFileDoc():
	return  "Loads the contents of the file named <filename> into memory(<cpuname> processor's memory space), start at physical address offset. <filename> must be the absolute path"
def LoadFileSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "load-file", cmd = LoadFileRun, synopses = LoadFileSynopses, see_also = LoadFileSeeAlso, doc = LoadFileDoc)
