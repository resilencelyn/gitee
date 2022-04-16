# -*- coding: UTF-8 -*-
import os
import sys
import cli
from skyeye_common_module import *
from tools import *
import errormessage
from conf import *
from exception import *
from sky_log import *
import project_config as pc


def RunScriptRun(args):
	if len(args) < 1:
		errormessage.SetErrorInfo(0x40160001, False, 1, len(args))
		raise
	fullfilename = os.path.realpath(args[0])
	if not os.path.exists(fullfilename):
		errormessage.SetErrorInfo(0x40160002, False, fullfilename)
		raise
	cmdfile = GetCommandFile(fullfilename)
	SkyEyeSetScriptPath(fullfilename)
	pc.set_script_path(fullfilename)
	ret = cmdfile.CheckCommandExists()
	if ret == False:
		errormessage.SetErrorInfo(0x40160000, False, cmdfile.GetErrorString())
		raise
	ret=False
	try:
		ret = cmdfile.ExectueCmdFile()
	except:
		raise
	if ret == False:
		return False
	return True

def RunScriptSynopses():
	return  "run-script  <filename>"
def RunScriptDoc():
	return  "Excute commands in the <filename>"
def RunScriptSeeAlso():
	return  ""



def InitCommand():
	cli.NewCommand(cmd_name = "run-script", cmd = RunScriptRun, synopses = RunScriptSynopses, see_also = RunScriptSeeAlso, doc = RunScriptDoc)
