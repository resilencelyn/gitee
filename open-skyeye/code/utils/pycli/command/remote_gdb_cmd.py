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

def RemoteGdbRun(args):
	if len(args) < 2:
		errormessage.SetErrorInfo(0x40120001, False, 2, len(args))
		raise
	if len(args) == 2:
		args.append("0.0.0.0")
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40120003, False)
		raise
	if args[0] not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x40120004, False, args[0])
		raise
	try:
		args[1] = int(args[1])
	except Exception as e:
		errormessage.SetErrorInfo(0x40120000, False, e)
		raise
	try:
		ret = SkyEyeCreateRemoteGdb(args[0],args[1],args[2])
		if ret != 1:
			errormessage.SetErrorInfo(0x40121201, False, args[0], args[1], args[2])
			raise
	except:
		errormessage.SetErrorInfo(0x40121202, False, args[0], args[1], args[2])
		raise

def RemoteGdbSynopses():
	return  "remote-gdb <cpuname> <port> <ip>"
def RemoteGdbDoc():
	return  "remote-gdb for one cpu"
def RemoteGdbSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "remote-gdb", cmd = RemoteGdbRun, synopses = RemoteGdbSynopses, see_also = RemoteGdbSeeAlso, doc = RemoteGdbDoc)
