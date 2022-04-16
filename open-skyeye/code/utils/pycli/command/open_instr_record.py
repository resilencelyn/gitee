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

def openInstrRecord(args):
	count = len(args)
	if count != 2:
		errormessage.SetErrorInfo(0x40210001, False, 2, len(args))
		raise
	cpu_name = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40210003, False)
		raise
	if cpu_name not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x40210004, False, cpu_name)
		raise
	filename = args[1]
	try:
		ret = SkyEyeOpenInstrRecord(cpu_name, filename)
	except Exception as e:
		errormessage.SetErrorInfo(0x40210000, False, e)
		raise
	if ret == 0:
		errormessage.SetErrorInfo(0x40212101, False)
		raise

def openInstrRecordSynopses():
	return  "open-instr-record <cpuname> <filename>"
def openInstrRecordDoc():
	return  "open cpu instruction recording"
def openInstrRecordSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "open-instr-record", cmd = openInstrRecord, synopses = openInstrRecordSynopses, see_also = openInstrRecordSeeAlso, doc = openInstrRecordDoc)
