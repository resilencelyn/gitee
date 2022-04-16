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
import shutil


def DisableReverseRun(args):
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40060003, False)
		raise
	if GetReverseState() == False:
		errormessage.SetErrorInfo(0x40066001, False)
		raise
	SetReverseState(False)

def DisableReverseSynopses():
	return  "disable-reverse"
def DisableReverseDoc():
	return  "disable reverse execution, after disable, skyeye can not run backwards from enable point"
def DisableReverseSeeAlso():
	return  "disable-reverse"




def InitCommand():
	cli.NewCommand(cmd_name = "disable-reverse", cmd = DisableReverseRun, synopses = DisableReverseSynopses, see_also = DisableReverseSeeAlso, doc = DisableReverseDoc)
