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

def enableSkyeyeDebug(args):
	count = len(args)
	if count != 1:
		errormessage.SetErrorInfo(0x40230001, False, 1, len(args))
		raise
	machine = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40230003, False)
		raise
	if machine not in config.get_mach_list():
		errormessage.SetErrorInfo(0x40230004, False, machine)
		raise
	try:
		ret = SkyEyeEnableDebug(machine)
	except Exception as e:
		errormessage.SetErrorInfo(0x40230000, False, e)
		raise
	if ret == 0:
		errormessage.SetErrorInfo(0x40232301, False)
		raise

def enableSkyeyeDebugSynopses():
	return  "enable-skyeye-debug <soc_name>"
def enableSkyeyeDebugRecordDoc():
	return  "enable skyeye to debug"
def enableSkyeyeDebugSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "enable-skyeye-debug", cmd = enableSkyeyeDebug, synopses = enableSkyeyeDebugSynopses, see_also = enableSkyeyeDebugSeeAlso, doc = enableSkyeyeDebugRecordDoc)
