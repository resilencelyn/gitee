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


def EnableDeviceWork(args):
	count = len(args)
	if count < 1:
		errormessage.SetErrorInfo(0x40080001, False, 1, count)
		raise
	device = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40080003, False)
		raise
	if device not in config.get_device_list():
		errormessage.SetErrorInfo(0x40080004, False, device)
		raise
	try:
		SkyEyeEnableDeviceWork(device)
	except Exception as e:
		errormessage.SetErrorInfo(0x40080000, False, e)
		raise

def EnableDeviceWorkSynopses():
	return  "enable-device-work  <deviceName>"
def EnableDeviceWorkDoc():
	return  "Enable device work"
def EnableDeviceWorkSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "enable-device-work", cmd = EnableDeviceWork, synopses = EnableDeviceWorkSynopses, see_also = EnableDeviceWorkDoc, doc = EnableDeviceWorkSeeAlso)

