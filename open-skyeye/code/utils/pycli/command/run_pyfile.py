# -*- coding: UTF-8 -*-
import os
import sys
import cli
from tools import *
import errormessage
from exception import *
from sky_log import *
import se
import se_system as ss
import threading


def RunPyfileRun(args):
	if len(args) < 1:
		errormessage.SetErrorInfo(0x40150001, False, 1, len(args))
		raise
	if not os.path.exists(args[0]):
		errormessage.SetErrorInfo(0x40150002, False, args[0])
		raise
	try:
		sys.path.append(".")
		global_dict = {}
		exec(open(args[0],"r").read(),global_dict)
		sys.stdout.flush()
	except Exception as e:
		errormessage.SetErrorInfo(0x40151501, False, args[0], e)
		raise
	return True

def RunPyfileSynopses():
	return  "run-pyfile  <filename>"
def RunPyfileDoc():
	return  "Excute commands in the <filename>"
def RunPyfileSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "run-pyfile", cmd = RunPyfileRun, synopses = RunPyfileSynopses, see_also = RunPyfileSeeAlso, doc = RunPyfileDoc)
