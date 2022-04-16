import os
import sys
import cli
from skyeye_common_module import *
import tools
from conf import *
from exception import *
import traceback
from sky_log import *
import coverage
import errormessage

def EnableCodecov(args):
	count = len(args)
	if count != 2:
		errormessage.SetErrorInfo(0x40070001, False, 2, count)
		raise
	machine = args[0]
	fusion_mode = args[1]
	if fusion_mode not in ['true','false']:
		errormessage.SetErrorInfo(0x40077001, False)
		raise
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40070003, False)
		raise
	if machine not in config.get_mach_list():
		errormessage.SetErrorInfo(0x40070004, False, machine)
		raise
	try:
		SkyEyeEnableCpuCodecov(machine)
		coverage.set_fusion_mode(fusion_mode)
		#coverage.write_mach_all_object_file(machine)
	except Exception as e:
		errormessage.SetErrorInfo(0x40070000, False, e)
		raise

def EnableCodecovSynopses():
	return  "enable-codecov  <machinename>"
def EnableCodecovDoc():
	return  "Enable machine all cpus codecov function"
def EnableCodecovSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "enable-codecov", cmd = EnableCodecov, synopses = EnableCodecovSynopses, see_also = EnableCodecovSeeAlso, doc = EnableCodecovDoc)
