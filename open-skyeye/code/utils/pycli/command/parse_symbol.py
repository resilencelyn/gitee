import os
import sys
import cli
from skyeye_common_module import *
import tools
import se_path
import uuid
import tempfile
import errormessage
from conf import *
from exception import *
import traceback
from sky_log import *
import subprocess

def ParseSymbolRun(args):
	count = len(args)
	if count < 2:
		errormessage.SetErrorInfo(0x400f0001, False, 2, count)
		raise
	cpu = args[0]
	filename = args[1]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x400f0003, False)
		raise
	if cpu not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x400f0004, False, cpu)
		raise
	if not os.path.exists(filename):
		errormessage.SetErrorInfo(0x400f0002, False, filename)
		raise
	try:
		archname = SkyEyeGetArchitecture(cpu)
	except Exception as e:
		errormessage.SetErrorInfo(0x400f0000, False, e)
		raise
	if archname == "tic6x":
		archname = archname + "_" + tools.get_binary_file_type(filename)
	arch_dir = os.path.join(se_path.ArchToolsDir, archname)
	if os.path.exists(arch_dir):
		ofd6x_cmd = os.path.join(arch_dir, 'ofd6x.exe')
		if not os.path.exists(ofd6x_cmd):
			errormessage.SetErrorInfo(0x400f0002, False, ofd6x_cmd)
			raise
		usr_temp_path = tempfile.gettempdir() + '\\skyeye_temp'
		if not os.path.exists(usr_temp_path):
			os.makedirs(usr_temp_path)
		temp_sym_file = usr_temp_path + '\\skyeye-symbol-' + str(uuid.uuid4())
		cmd = ''.join([ofd6x_cmd,' -g -x -o ', temp_sym_file, ' ', filename])
		subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
		SkyEyeParseSymbol(cpu, temp_sym_file)
	else:
		errormessage.SetErrorInfo(0x400f0006, False, arch_dir)
		raise

def ParseSymbolSynopses():
	return  "parse-symbol  <cpuname> <filename>"

def ParseSymbolDoc():
	return  "load a binary file to memory space for one cpu"

def ParseSymbolSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "parse-symbol", cmd = ParseSymbolRun, synopses = ParseSymbolSynopses, see_also = ParseSymbolSeeAlso, doc = ParseSymbolDoc)
