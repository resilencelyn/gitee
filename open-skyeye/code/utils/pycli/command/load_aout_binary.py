import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *

def str_to_int(stri):
	if stri[:2] == '0x':
		return int(stri, 16)
	else:
		return int(stri, 10)

def LoadAOutBinaryRun(args):
	if len(args) != 4:
		errormessage.SetErrorInfo(0x400b0001, False, 4, len(args))
		raise
	
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x400b0003, False)
		raise
	if args[0] not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x400b0004, False, args[0])
		raise
	if not os.path.exists(args[1]):
		errormessage.SetErrorInfo(0x400b0002, False, args[1])
		raise
	try:
		py_address_str = args[2]
		py_address_uint = str_to_int(py_address_str)

		py_length_uint = os.path.getsize(args[1])

		py_pc_str = args[3]
		py_pc_uint = str_to_int(py_pc_str)
	except Exception as e:
		errormessage.SetErrorInfo(0x400b0000, False, e)
		raise
	bin_file = os.path.abspath(args[1]) + ".bin"
	tools.get_bin_file(args[0], args[1], bin_file)
	#coverage_replace_file = os.path.abspath(args[1]) + ".replace"
	#tools.get_objcopy_file(args[0], args[1], coverage_replace_file, py_address_str)
	
	if not os.path.exists(bin_file):
		errormessage.SetErrorInfo(0x400b0002, False, bin_file)
		raise
	#if not os.path.exists(coverage_replace_file):
	#	errormessage.SetErrorInfo(0x400b0002, False, coverage_replace_file)
	#	raise
	try:
		ret = SkyEyeLoadBinBinary(args[0], bin_file, py_address_uint, py_length_uint, py_pc_uint)
	except Exception as e:
		errormessage.SetErrorInfo(0x400b0000, False, e)
		raise
	if ret == False:
		errormessage.SetErrorInfo(0x400b0005, False)
		raise
	#binary_l[args[0]] = coverage_replace_file
	#coverage.replace_binary = coverage_replace_file
	#coverage.binary_offset = py_address_uint

def LoadAOutBinarySynopses():
	return  "load-aout-binary  <cpuname>  <filename>  <offset>  <start_pc>"
def LoadAOutBinaryDoc():
	return  "Loads the contents of the a.out file named <filename> into memory(<cpuname> processor's memory space), start at physical address offset. <filename> must be the absolute path"
def LoadAOutBinarySeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "load-aout-binary", cmd = LoadAOutBinaryRun, synopses = LoadAOutBinarySynopses, see_also = LoadAOutBinarySeeAlso, doc = LoadAOutBinaryDoc)
