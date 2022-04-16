import os
import sys
import cli
from skyeye_common_module import *
from tools import *
import errormessage
from conf import *
from exception import *

def RunRun(args):
	SkyEyeRun()

def RunSynopses():
	return  "run"
def RunDoc():
	return  "Let the platform start running"
def RunSeeAlso():
	return  ""



def InitCommand():
	cli.NewCommand(cmd_name = "run", cmd = RunRun, synopses = RunSynopses, see_also = RunSeeAlso, doc = RunDoc)
