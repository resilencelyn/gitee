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


def DelayLoadRun(args):
	count = len(args)
	if count < 2:
		errormessage.SetErrorInfo(0x40040001, False, 2, count)
		raise
	soc = args[0]
	ms = args[1]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40040003, False)
		raise
	if soc not in config.get_mach_list():
		errormessage.SetErrorInfo(0x40040004, False, soc)
		raise
	try:
		ms = int(ms)
	except Exception as e:
		errormessage.SetErrorInfo(0x40040000, False, e)
		raise
	SkyEyeSetSocDelayLoad(soc, ms)

def DelayLoadSynopses():
	return  "delay-load  <socname> <ms>"
def DelayLoadDoc():
	return  "after some ms time, the cpu on <soc> begins to execute"
def DelayLoadSeeAlso():
	return  ""


def InitCommand():
	cli.NewCommand(cmd_name = "delay-load", cmd = DelayLoadRun, synopses = DelayLoadSynopses, see_also = DelayLoadSeeAlso, doc = DelayLoadDoc)
