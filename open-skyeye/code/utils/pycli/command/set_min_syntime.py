import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *
import se_system as ss
import sky_log



def SetMinSynTimeRun(args):
	count = len(args)
	if count < 1:
		errormessage.SetErrorInfo(0x40191901, False, count)
		raise
	if count > 2:
		errormessage.SetErrorInfo(0x40191902, False, count)
		raise
	delay = 0.0
	if count == 2:
		try:
			delay = float(args[1])
		except:
			errormessage.SetErrorInfo(0x40191903, False, count)
			raise

	try:
		time = float(args[0])
	except:
		errormessage.SetErrorInfo(0x40191904, False, count)
		raise
	try:
		SkyEyeSetMinSynTime(time, delay)
	except Exception as e:
		errormessage.SetErrorInfo(0x40190000, False, e)


def SetMinSynTimeSynopses():
	return  "set-min-syntime  <time>"
def SetMinSynTimeDoc():
	return  "set minimum synchronization time between processor cores, parameter is second."
def SetMinSynTimeSeeAlso():
	return  ""


def InitCommand():
	cli.NewCommand(cmd_name = "set-min-syntime", cmd = SetMinSynTimeRun, synopses = SetMinSynTimeSynopses, see_also = SetMinSynTimeSeeAlso, doc = SetMinSynTimeDoc)
