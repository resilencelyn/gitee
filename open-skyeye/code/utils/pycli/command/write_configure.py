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
import win_workspace as ws
import shutil


def WriteConfigureRun(args):
	config = GetGlobalConfig()
	if config == None:
		errormessage.SetErrorInfo(0x401b0003, False)
		raise
	mach_list = config.get_mach_list()
	cmd_dir = None
	if len(args) >= 1:
		cmd_dir = args[0]

	json_file = config.get_current_fp()
	skyeye_script  = GetGlobalScript()
	files = [json_file, skyeye_script]
	if cmd_dir == None:
		cmd_dir = 'skyeye.ckpt'
	else:
		if cmd_dir.endswith(".ckpt") == False:
			cmd_dir = ''.join([cmd_dir, ".ckpt"])
	if tools.is_linux():
		ckpt_dir = os.path.join(os.getcwd(), cmd_dir)
	else:
		ws_dir = ws.get_workspace_dir()
		if not os.path.exists(ws_dir):
			errormessage.SetErrorInfo(0x401b1b01, False)
			raise
		ckpt_dir = os.path.join(ws_dir, cmd_dir)
	try:
		full_dir = tools.create_chp_dir(ckpt_dir)
		tools.ckpt_save_files(full_dir, files)
	except IOError as e:
		errormessage.SetErrorInfo(0x401b0000, False, e)
		raise
	for mach in mach_list:
		cpu = config.get_cpuname_by_mach(mach)
		cls = config.get_classname(cpu)
		if cls == "x86":
			SkyEyeX86SaveConfigure(cpu, full_dir)
		device_list = config.get_device_list_by_mach(mach)
		for device in device_list:
			cls = config.get_classname(device)
			if cls == "image":
				count = SkyEyeImageGetPageCount(device)
				for i in range(0, count):
					index =  SkyEyeImageGetPageIndexById(device, i)
					image_file_name  = ''.join([device, "_%d" % index, ".image"])
					image_file_path = os.path.join(full_dir, image_file_name)
					SkyEyeImagePageToFile(device, index, image_file_path)
					tools.ckpt_save_image_info(full_dir, device, index, image_file_name)
		steps = SkyEyeGetCpuSteps(cpu)
		tools.ckpt_save_steps(full_dir, cpu, steps)
	config = os.path.join(full_dir, "config")
	SkyEyeStoreConfigure(config)


def WriteConfigureSynopses():
	return  "write-configure <filename>"
def WriteConfigureDoc():
	return  "save a checkpoint"
def WriteConfigureSeeAlso():
	return  "read-configure <filename>"




def InitCommand():
	cli.NewCommand(cmd_name = "write-configure", cmd = WriteConfigureRun, synopses = WriteConfigureSynopses, see_also = WriteConfigureSeeAlso, doc = WriteConfigureDoc)
