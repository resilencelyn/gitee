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

def str_to_int(stri):
	if stri[:2] == '0x':
		return int(stri, 16)
	else:
		return int(stri, 10)

def setPC(args):
	count = len(args)
	if count != 2:
		errormessage.SetErrorInfo(0x40220001, False, 2, len(args))
		raise
	cpu_name = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40220003, False)
		raise
	if cpu_name not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x40220004, False, cpu_name)
		raise
	try:
		pc_address_str = args[1]
		pc_address_uint = str_to_int(pc_address_str)
	except Exception as e:
		errormessage.SetErrorInfo(0x40220001, False, e)
		raise
	try:
		ret = SkyEyeSetPC(cpu_name, pc_address_uint)
	except Exception as e:
		errormessage.SetErrorInfo(0x40220000, False, e)
		raise
	if ret == 0:
		print ("SkyEyeSetPC Failed!")

def openSetPCSynopses():
	return  "set-pc <cpuname> <pc>"
def openSetPCDoc():
	return  "set cpu first pc"
def openSetPCSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "set-pc", cmd = setPC, synopses = openSetPCSynopses, see_also = openSetPCSeeAlso, doc = openSetPCDoc)
