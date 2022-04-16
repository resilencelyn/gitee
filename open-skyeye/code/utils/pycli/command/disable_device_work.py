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

def DisableDeciceWork(args):
	count = len(args)
	if count < 1:
		errormessage.SetErrorInfo(0x40050001, False, 1, count)
		raise
	device = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40050003, False)
		raise
	if device not in config.get_device_list():
		errormessage.SetErrorInfo(0x40050004, False, device)
		raise
	try:
		SkyEyeDisableDeviceWork(device)
	except Exception as e:
		errormessage.SetErrorInfo(0x40050000, False, e)
		raise

def DisableDeviceWorkSynopses():
	return  "disable-device-work  <deviceName>"
def DisableDeviceWorkDoc():
	return  "disable device work"
def DisableDeviceWorkSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "disable-device-work", cmd = DisableDeciceWork, synopses = DisableDeviceWorkSynopses, see_also = DisableDeviceWorkDoc, doc = DisableDeviceWorkSeeAlso)

