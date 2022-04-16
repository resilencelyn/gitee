import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *
import operator

def str_to_int(stri):
	if stri[:2] == '0x':
		return int(stri, 16)
	else:
		return int(stri, 10)

def LoadBinBinaryRun(args):
	if len(args) < 5:
		errormessage.SetErrorInfo(0x400c0001, False, 5, len(args))
		raise
	try:
		py_address_str = args[2]
		py_address_uint = str_to_int(py_address_str)
		py_length_str = args[3]
		py_length_uint = str_to_int(py_length_str)
		py_pc_str = args[4]
		py_pc_uint = str_to_int(py_pc_str)
	except Exception as e:
		errormessage.SetErrorInfo(0x400c0000, False, e)
		raise
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x400c0003, False)
		raise
	if args[0] not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x400c0004, False, args[0])
		raise
	if not os.path.exists(args[1]):
		errormessage.SetErrorInfo(0x400c0002, False, args[1])
		raise
	try:
		ret = SkyEyeLoadBinBinary(args[0], args[1], py_address_uint, py_length_uint, py_pc_uint)
	except Exception as e:
		errormessage.SetErrorInfo(0x400c0000, False, e)
		raise
	if ret == False:
		errormessage.SetErrorInfo(0x400c0005, False)
		raise

def LoadBinBinarySynopses():
	return  "load-bin-binary  <cpuname>  <filename>  <offset>  <length>  <start_pc>"
def LoadBinBinaryDoc():
	return  "Loads the contents of the bin file named <filename> into memory(<cpuname> processor's memory space), start at physical address offset. <filename> must be the absolute path"
def LoadBinBinarySeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "load-bin-binary", cmd = LoadBinBinaryRun, synopses = LoadBinBinarySynopses, see_also = LoadBinBinarySeeAlso, doc = LoadBinBinaryDoc)
