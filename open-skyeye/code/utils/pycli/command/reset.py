import os
import sys
import cli
from skyeye_common_module import *
from tools import *
import errormessage
from conf import *
from exception import *

def RunReset(args):
	SkyEyeReset()

def ResetSynopses():
	return  "reset"
def ResetDoc():
	return  "Let the platform reset"
def ResetSeeAlso():
	return  ""


def InitCommand():
	cli.NewCommand(cmd_name = "reset", cmd = RunReset, synopses = ResetSynopses, see_also = ResetSeeAlso, doc = ResetDoc)
