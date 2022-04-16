import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *
import se_system as ss



def InitOkRun(args):
	try:
		SkyEyePrepareToRun()
	except:
		errormessage.SetErrorInfo(0x400aa001, False)
		raise
	if ss.system==None:
		ss.CreateClass()
	return


def InitOkSynopses():
	return  "init ok"
def InitOkDoc():
	return  "Init the system, prepare to run"
def InitOkSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "init-ok", cmd = InitOkRun, synopses = InitOkSynopses, see_also = InitOkSeeAlso, doc = InitOkDoc)
