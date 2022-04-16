import os
import sys
import cli
from skyeye_common_module import *
from tools import *
import errormessage
from conf import *
from exception import *

def RunStop(args):
	SkyEyeStop()

def StopSynopses():
	return  "stop"
def StopDoc():
	return  "Let the platform start running"
def StopSeeAlso():
	return  ""




def InitCommand():
	cli.NewCommand(cmd_name = "stop", cmd = RunStop, synopses = StopSynopses, see_also = StopSeeAlso, doc = StopDoc)
