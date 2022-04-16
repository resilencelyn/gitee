import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *
import mips



def LoadConfRun(args):
	if len(args) < 1:
		errormessage.SetErrorInfo(0x400d0001, False, 1, len(args))
		raise
	try:
		if not os.path.exists(args[0]):
			errormessage.SetErrorInfo(0x400d0002, False, args[0])
			raise
		Config = json_conf(parent = None, filename = args[0])
		if Config.get_init_result() == False:
			raise
	except:
		raise 
	try:
		Config.instance()
		SkyEyePrepareToRun()
	except:
		raise
	SetGlobalConfig(Config)
	return  True


def LoadConfSynopses():
	return  "load_conf  <filename>"
def LoadConfDoc():
	return  "Load a json script, that define the soc ,cpu, device and so on."
def LoadConfSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "load-conf", cmd = LoadConfRun, synopses = LoadConfSynopses, see_also = LoadConfSeeAlso, doc = LoadConfDoc)
