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

def closeInstrRecord(args):
	count = len(args)
	if count != 1:
		errormessage.SetErrorInfo(0x401f0001, False, 1, len(args))
		raise
	cpu_name = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x401f0003, False)
		raise
	if cpu_name not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x401f0004, False, cpu_name)
		raise
	try:
		ret = SkyEyeCloseInstrRecord(cpu_name)
	except Exception as e:
		errormessage.SetErrorInfo(0x401f0000, False, e)
		raise
	if ret == 0:
		errormessage.SetErrorInfo(0x401f1f01, False)
		raise

def closeInstrRecordSynopses():
	return  "close-instr-record <cpuname>"
def closeInstrRecordDoc():
	return  "close cpu instruction recording"
def closeInstrRecordSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "close-instr-record", cmd = closeInstrRecord, synopses = closeInstrRecordSynopses, see_also = closeInstrRecordSeeAlso, doc = closeInstrRecordDoc)
