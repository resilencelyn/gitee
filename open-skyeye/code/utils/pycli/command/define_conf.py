import os
import sys
import cli
from skyeye_common_module import *
import tools
import errormessage
from conf import *
from exception import *
import traceback



def DefineConfRun(args):
	if len(args) < 1:
		errormessage.SetErrorInfo(0x401c0001, False, 1, len(args))
		raise
	try:
		if not os.path.exists(args[0]):
			errormessage.SetErrorInfo(0x401c0002, False, args[0])
			raise
		Config = json_conf(parent = None, filename = args[0])
		define_json_file[0] = os.path.join(os.getcwd(), args[0])
		Config.conf_check()
	except:
		raise
	try:
		if Config.instance()==False:
			return False
	except:
		raise
	SetGlobalConfig(Config)
	return  True


def DefineConfSynopses():
	return  "define_conf  <filename>"
def DefineConfDoc():
	return  "define a json script, that define the soc ,cpu, device and so on."
def DefineConfSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "define-conf", cmd = DefineConfRun, synopses = DefineConfSynopses, see_also = DefineConfSeeAlso, doc = DefineConfDoc)
