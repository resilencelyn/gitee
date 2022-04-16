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
from se_path import InstallDir


def ReverseRun(args):
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40131102, False)
		raise
	if len(args) < 2:
		errormessage.SetErrorInfo(0x40130001, False, 2, len(args))
		raise

	ckpt = None
	if tools.is_windows():
		ckpt = os.path.join(InstallDir, ".reverse.ckpt")
	elif tools.is_linux():
		usr = os.path.expanduser('~')
		ckpt = os.path.join(usr, ".skyeye", "revere.ckpt")

	t_cwd = os.getcwd()
	os.chdir(ckpt)
	if GetReverseState() == False:
		errormessage.SetErrorInfo(0x40131301, False)
		raise

	core = args[0]
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40130003, False)
		raise
	if core not in config.get_cpu_list():
		errormessage.SetErrorInfo(0x40130004, False, core)
		raise
	reverse_steps = tools.str_to_uint(args[1])
	steps = tools.ckpt_get_steps(ckpt, core)
	current_steps = SkyEyeGetCpuSteps(core)
	run_steps = current_steps - steps - reverse_steps
	if current_steps == steps:
		return
	if reverse_steps > current_steps - steps:
		errormessage.SetErrorInfo(0x40131302, False, current_steps - steps)
		raise

	info_dic = tools.ckpt_get_image_info(ckpt)
	images = info_dic.keys()
	for image in images:
		SkyEyeImageClearAllPages(image)
		for page_l in info_dic[image]:
			SkyEyeImageFileToPage(image, page_l[0], page_l[1])
	config = os.path.join(ckpt, "config")
	SkyEyeRecoverConfigure(config)
	os.chdir(t_cwd)
	if run_steps != 0:
		SkyEyeStepi(core, "%d" % run_steps)


def ReverseSynopses():
	return  "reverse <cpuname> <steps>"
def ReverseDoc():
	return  "<cpuname> reverse execute <steps> instructon"
def ReverseSeeAlso():
	return  "disable-reverse/enable-reverse"




def InitCommand():
	cli.NewCommand(cmd_name = "reverse", cmd = ReverseRun, synopses = ReverseSynopses, see_also = ReverseSeeAlso, doc = ReverseDoc)
