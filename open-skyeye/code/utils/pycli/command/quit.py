import os
import sys
import cli
from skyeye_common_module import *
from tools import *
import errormessage
from conf import *
from exception import *

def RunQuit(args):
	SkyEyeQuit()

def QuitSynopses():
	return  "quit"
def QuitDoc():
	return  "Let the platform quit."
def QuitSeeAlso():
	return  ""

def InitCommand():
	cli.NewCommand(cmd_name = "quit", cmd = RunQuit, synopses = QuitSynopses, see_also = QuitSeeAlso, doc = QuitDoc)
