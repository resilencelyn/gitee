import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *
import traceback
from sky_log import *


def LoadBinaryRun(args):
	count = len(args)
	if count < 2:
		errormessage.SetErrorInfo(0x40030001, False, 2, len(args))
		raise
	cpu = args[0]
	filename = args[1]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40030003, False)
		raise
	if cpu not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x40030004, False, cpu)
		raise
	if not os.path.exists(filename):
		errormessage.SetErrorInfo(0x40030002, False, filename)
		raise
	try:
		ret = SkyEyeLoadBinary(cpu, filename)
	except Exception as e:
		raise
	binary_l[cpu] = os.path.join(os.getcwd(), filename)
	if ret == 10:
		errormessage.SetErrorInfo(0x40030005, False)
		raise

def LoadBinarySynopses():
	return  "load_binary  <cpuname> <filename>"
def LoadBinaryDoc():
	return  "load a binary file to memory space for one cpu"
def LoadBinarySeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "load-binary", cmd = LoadBinaryRun, synopses = LoadBinarySynopses, see_also = LoadBinarySeeAlso, doc = LoadBinaryDoc)
