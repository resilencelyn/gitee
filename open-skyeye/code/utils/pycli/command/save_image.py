import os
import sys
import cli
from skyeye_common_module import *
from tools import *
import errormessage
from conf import *
from exception import *

def SaveImageCommand(args):
	if len(args) < 1:
		errormessage.SetErrorInfo(0x40170001, False, 1, len(args))
		raise
	device = args[0]
	if device not in image_devices:
		print ("Device: %s not init." % device)
	try:
		SkyEyeSaveImage(device)
	except Exception as e:
		errormessage.SetErrorInfo(0x40170000, False, e)
		raise

def SaveImageCommandSynopses():
	return  "save-image-command  <command string> <command string>"
def RunCommandDoc():
	return  "Excute commands one command line"
def RunCommandSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "save-image", cmd = SaveImageCommand, synopses = SaveImageCommandSynopses, see_also = RunCommandSeeAlso, doc = RunCommandDoc)
