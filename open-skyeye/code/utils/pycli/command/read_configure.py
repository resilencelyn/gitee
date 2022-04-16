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


def ReadConfigureRun(args):
	config = GetGlobalConfig()
	if config != None:
		errormessage.SetErrorInfo(0x40111101, False)
		raise
	t_cwd = os.getcwd()
	if len(args) < 1:
		errormessage.SetErrorInfo(0x40110001, False, 1, len(args))
		raise
	try:
		full_dir = tools.cmd_get_chp_dir(args[0])
	except IOError as e:
		errormessage.SetErrorInfo(0x40110000, False, e.message)
		raise
	os.chdir(full_dir)
	try:
		tools.start_ckpt_config(full_dir)
	except IOError as e:
		errormessage.SetErrorInfo(0x40110000, False, e.message)
		raise

	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x40111102, False)
		raise 
	mach_list = config.get_mach_list()
	for mach in mach_list:
		cpu = config.get_cpuname_by_mach(mach)
		cls = config.get_classname(cpu)
		if cls == "x86":
			SkyEyeX86LoadConfigure(cpu, full_dir)

	info_dic = tools.ckpt_get_image_info(full_dir)
	images = info_dic.keys()
	for image in images:
		for page_l in info_dic[image]:
			SkyEyeImageFileToPage(image, page_l[0], page_l[1])

	config = os.path.join(full_dir, "config")
	SkyEyeRecoverConfigure(config)
	os.chdir(t_cwd)


def ReadConfigureSynopses():
	return  "read-configure <filename>"
def ReadConfigureDoc():
	return  "read a checkpoint file"
def ReadConfigureSeeAlso():
	return  "write-configure <filename>"




def InitCommand():
	cli.NewCommand(cmd_name = "read-configure", cmd = ReadConfigureRun, synopses = ReadConfigureSynopses, see_also = ReadConfigureSeeAlso, doc = ReadConfigureDoc)
