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


def LoadBinaryX86Run(args):
	count = len(args)
	if count < 2:
		errormessage.SetErrorInfo(0x40240001, False, 2, len(args))
		raise
	cpu = args[0]
	filename = args[1]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40240003, False)
		raise
	if cpu not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x40240004, False, cpu)
		raise
	if not os.path.exists(filename):
		errormessage.SetErrorInfo(0x40240002, False, filename)
		raise
	binary_l[cpu] = os.path.join(os.getcwd(), filename)

def LoadBinaryX86Synopses():
	return  "load_binary_x86  <cpuname> <filename>"
def LoadBinaryX86Doc():
	return  "load a binary file to memory space for x86 cpu"
def LoadBinaryX86SeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "load-binary-x86", cmd = LoadBinaryX86Run, synopses = LoadBinaryX86Synopses, see_also = LoadBinaryX86SeeAlso, doc = LoadBinaryX86Doc)
