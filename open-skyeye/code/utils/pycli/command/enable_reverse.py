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


def EnableReverseRun(args):
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40090003, False)
		raise
	ckpt = None
	if tools.is_windows():
		ckpt = os.path.join(InstallDir, ".reverse.ckpt")
	elif tools.is_linux():
		usr = os.path.expanduser('~')
		ckpt = os.path.join(usr, ".skyeye", "revere.ckpt")

	if GetReverseState() == True:
		errormessage.SetErrorInfo(0x40099001, False)
		raise
	SetReverseState(True)
	if os.path.exists(ckpt):
		shutil.rmtree(ckpt)
	full_ckpt_dir = tools.create_chp_dir(ckpt)

	mach_list = config.get_mach_list()
	for mach in mach_list:
		cpu = config.get_cpuname_by_mach(mach)
		device_list = config.get_device_list_by_mach(mach)
		for device in device_list:
			cls = config.get_classname(device)
			if cls == "image":
				count = SkyEyeImageGetPageCount(device)
				for i in range(0, count):
					index =  SkyEyeImageGetPageIndexById(device, i)
					image_file_name  = ''.join([device, "_%d" % index, ".image"])
					image_file_path = os.path.join(full_ckpt_dir, image_file_name)
					SkyEyeImagePageToFile(device, index, image_file_path)
					tools.ckpt_save_image_info(full_ckpt_dir, device, index, image_file_name)
			steps = SkyEyeGetCpuSteps(cpu)
		tools.ckpt_save_steps(full_ckpt_dir, cpu, steps)
	config = os.path.join(full_ckpt_dir, "config")
	SkyEyeStoreConfigure(config)


def EnableReverseSynopses():
	return  "enable-reverse"
def EnableReverseDoc():
	return  "Enable reverse execution, after enable, skyeye can run backwards from this point"
def EnableReverseSeeAlso():
	return  "enable-reverse"




def InitCommand():
	cli.NewCommand(cmd_name = "enable-reverse", cmd = EnableReverseRun, synopses = EnableReverseSynopses, see_also = EnableReverseSeeAlso, doc = EnableReverseDoc)
