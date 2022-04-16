import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *
import se_system as ss



def Slow_x86OkRun(args):
	count = len(args)
	if count < 2:
		errormessage.SetErrorInfo(0x401a0001, False, 2, count)
		raise
	cpu = args[0]
	isLow = args[1]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x401a0003, False)
		raise
	if cpu not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x401a0004, False, cpu)
		raise
	try:
		SkyEyeSetX86Slow(cpu, int(isLow))
	except Exception as e:
		errormessage.SetErrorInfo(0x401a0000, False, e)
		raise


def Slow_x86OkSynopses():
	return  "init ok"
def Slow_x86OkDoc():
	return  "Slow_x86 the system, prepare to run"
def Slow_x86OkSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "slow-x86", cmd = Slow_x86OkRun, synopses = Slow_x86OkSynopses, see_also = Slow_x86OkSeeAlso, doc = Slow_x86OkDoc)
