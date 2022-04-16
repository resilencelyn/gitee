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


def PreTranslateRun(args):
	count = len(args)
	if count < 1:
		errormessage.SetErrorInfo(0x40100001, False, 1, count)
		raise
	cpu = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40100003, False)
		raise
	if cpu not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x40100004, False, cpu)
		raise
	SkyPreTranslate(cpu)

def PreTranslateSynopses():
	return  "pretranslate  <cpuname>"
def PreTranslateDoc():
	return  "pretranslate for one cpu"
def PreTranslateSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "pre_translate", cmd = PreTranslateRun, synopses = PreTranslateSynopses, see_also = PreTranslateSeeAlso, doc = PreTranslateDoc)
