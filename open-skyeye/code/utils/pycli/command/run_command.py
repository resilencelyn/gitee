import os
import sys
import cli
from skyeye_common_module import *
from tools import *
import errormessage
from conf import *
from exception import *

def RunCommandRun(args):
	if len(args) < 1:
		errormessage.SetErrorInfo(0x40140001, False, 1, len(args))
		raise
	args = args.replace('"', ' ')
	list = ParseParameter(args)
	cmdname = list[0]
	argslist = list[1:]
	ret = cli.IsCommandExists(cmdname)
	if ret == False:
		errormessage.SetErrorInfo(0x40141401, False, cmdname)
		raise
	cmd = cli.GetCommand(cmdname)
	try:
		cmd.run(argslist)
	except Exception:
		raise

def RunCommandSynopses():
	return  "run-command  <command string>"
def RunCommandDoc():
	return  "Excute commands one command line"
def RunCommandSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "run-command", cmd = RunCommandRun, synopses = RunCommandSynopses, see_also = RunCommandSeeAlso, doc = RunCommandDoc)
