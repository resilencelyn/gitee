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

def getPC(args):
	count = len(args)
	if count != 1:
		errormessage.SetErrorInfo(0x40200001, False, 1, len(args))
		raise
	cpu_name = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40200003, False)
		raise
	if cpu_name not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x40200004, False, cpu_name)
		raise
	try:
		pc = SkyEyeGetPC(cpu_name)
		print ("Current PC:0x%x"%pc)
	except Exception as e:
		errormessage.SetErrorInfo(0x40200000, False, e)
		raise

def openGetPCSynopses():
	return  "get-pc <cpuname>"
def openGetPCDoc():
	return  "get cpu current pc"
def openGetPCSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "get-pc", cmd = getPC, synopses = openGetPCSynopses, see_also = openGetPCSeeAlso, doc = openGetPCDoc)
