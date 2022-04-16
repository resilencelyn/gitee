# -*- coding: UTF-8 -*-
import os, platform, sys
from ctypes import *
import operator
#from exception import *
#from common_dialog import *
import errormessage
import traceback
#from sky_log import LogReport, err
import se_path

os_info = platform.system()
if operator.eq(os_info, 'Linux') == True:
	libcommon_path = os.getenv('SKYEYEBIN') + '/../lib/skyeye/libcommon.so'
	libcommon = CDLL(libcommon_path, RTLD_GLOBAL)
	libdisasm_path = os.getenv('SKYEYEBIN') + '/../lib/skyeye/libdisasm.so'
	libdisasm = CDLL(libdisasm_path, RTLD_LOCAL)
else:
	skyeye_bin = se_path.SkyEyeBin
	libcommon_path = os.path.abspath(os.path.join(skyeye_bin, "libcommon-0.dll"))
	libcommon = CDLL(libcommon_path, RTLD_GLOBAL)
	libdisasm_path = os.path.join(skyeye_bin, r"..\\lib\skyeye\\libdisasm-0.dll")
	libdisasm = CDLL(libdisasm_path, RTLD_LOCAL)
	libpytimer_path = os.path.join(skyeye_bin, r"libpytimer-0.dll")
	libpytimer = CDLL(libpytimer_path, RTLD_LOCAL)

MaxChar = 1024

swtich="try_open"
#swtich="try_close"


def convert(data):
	if isinstance(data, bytes):
		return data.decode()
	if isinstance(data, dict):
		return dict(map(convert, data.items()))
	if isinstance(data, tuple):
		return tuple(map(convert, data))
	if isinstance(data, list):
		return list(map(convert, data))
	return data


def swtich_try(func):
	global swich
	global fun_list
	def try_open(*args,**argv):
		try:
			args = convert(args)
			argv = convert(argv)
			ret = func(*args,**argv)
		except:
			errormessage.SetErrorInfo(0x401e1e01, False, func.__name__)
			raise
		if ret[0] == True:
			return convert(ret[1])
		else:
			if bool(ret[2]) == False:
				return convert(ret[1])
			error_info = []
			for info in ret[2]:
				if info == None:
					break
				error_info.append(info.decode())
			errormessage.C_SetErrorInfo(convert(error_info), False)
			raise

	def try_close(*args,**argv):
		args = convert(args)
		argv = convert(argv)
		ret = func(*args,**argv)
		if ret[0] == True:
			return convert(ret[1])
		else:
			if bool(ret[2]) == False:
				return convert(ret[1])
			error_info = []
			for info in ret[2]:
				if info == None:
					break
				error_info.append(info.decode())
			errormessage.C_SetErrorInfo(convert(error_info), False)
			raise
	func_dict = {"try_open":try_open,"try_close":try_close}
	return func_dict[swtich]

class log_object(Structure):
	_fields_ = []

class conf_object(Structure):
	_fields_ = [
		("objname",c_char_p),
		("obj", c_void_p),
		("class_name", c_char_p),
		("c_class",c_void_p),
		("object_type_t", c_int),
		("log_object_t", POINTER(log_object)),
	]
conf_object_t = POINTER(conf_object)

int_list = c_uint *40
class fi_struct(Structure):
	_fields_ = [
		("bit",c_uint),
		("addr", c_uint),
		("mode", c_uint)
	]

class fj_struct(Structure):
	_fields_ = [
		("count",c_int), 
		("fi", int_list)
	]
fi_type = POINTER(fi_struct)
fj_type = POINTER(fj_struct)

data = c_char * 0x2000
class skyeye_api_result(Structure):
	_fields_ = [
		("result",c_int),
		("retStrValue", data),
		("retIntValue",c_uint32),
		("retFloatValue", c_float),
		("retDoubleValue", c_double),
		("retLongValue", c_uint64),
		("conf_obj", conf_object_t),
		("list", POINTER(c_char_p)),
		("errMsg", POINTER(c_char_p)),
		("fault_inject", fj_type),
	]


@swtich_try
def SkyEyeRun():
	libcommon.skyeye_run.restype = skyeye_api_result
	ret = libcommon.skyeye_run()
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeStop():
	libcommon.skyeye_stop.restype = skyeye_api_result
	ret = libcommon.skyeye_stop()
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeRunningStatus():
	libcommon.skyeye_running_status.restype = skyeye_api_result
	ret = libcommon.skyeye_running_status()
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeQuit():
	libcommon.skyeye_quit.restype = skyeye_api_result
	ret = libcommon.skyeye_quit()
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeReset():
	libcommon.skyeye_reset.restype = skyeye_api_result
	ret = libcommon.skyeye_reset()
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyePrepareToRun():
	libcommon.skyeye_prepare_running.restype = skyeye_api_result
	ret = libcommon.skyeye_prepare_running()
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeClearBpHit(corename):
	libcommon.skyeye_clear_bp_hit.restype = skyeye_api_result
	libcommon.skyeye_clear_bp_hit.argtypes = [c_char_p]
	ret = libcommon.skyeye_clear_bp_hit(corename.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetBreakpointHitId(corename):
	libcommon.skyeye_get_bp_hit_id.restype = skyeye_api_result
	libcommon.skyeye_get_bp_hit_id.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_bp_hit_id(corename.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetBreakpointAddrById(corename, id):
	libcommon.skyeye_get_breakpoint_address_by_id.restype = skyeye_api_result
	libcommon.skyeye_get_breakpoint_address_by_id.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_get_breakpoint_address_by_id(corename.encode("utf-8"), id)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeDeleteBreakpointById(corename, id):
	libcommon.skyeye_delete_breakpoint_by_id.restype = skyeye_api_result
	libcommon.skyeye_delete_breakpoint_by_id.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_delete_breakpoint_by_id(corename.encode("utf-8"), id)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeDeleteBreakpointByAddr(corename, addr):
	libcommon.skyeye_delete_breakpoint_by_addr.restype = skyeye_api_result
	libcommon.skyeye_delete_breakpoint_by_addr.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_delete_breakpoint_by_addr(corename.encode("utf-8"), addr)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetBpNumbers(corename):
	libcommon.skyeye_get_bp_numbers.restype = skyeye_api_result
	libcommon.skyeye_get_bp_numbers.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_bp_numbers(corename.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeCheckBreakpointHit(corename):
	libcommon.skyeye_check_bp_hit.restype = skyeye_api_result
	libcommon.skyeye_check_bp_hit.argtypes = [c_char_p]
	ret = libcommon.skyeye_check_bp_hit(corename.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeCreateBreakpoint(corename, addr):
	libcommon.skyeye_create_breakpoint.restype = skyeye_api_result
	libcommon.skyeye_create_breakpoint.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_create_breakpoint(corename.encode("utf-8"), addr)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetSocNum():
	libcommon.skyeye_get_soc_num.restype = skyeye_api_result
	ret = libcommon.skyeye_get_soc_num()
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyePreConfObj(objname,clsname):
	libcommon.skyeye_add_pre_obj.restype = skyeye_api_result
	libcommon.skyeye_add_pre_obj.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_add_pre_obj(objname.encode("utf-8"), clsname.encode("utf-8"))
	return [ret.result, ret.conf_obj.contents, ret.errMsg]

@swtich_try
def SkyEyeCreateMach(machname, machcls):
	libcommon.skyeye_create_mach.restype = skyeye_api_result
	libcommon.skyeye_create_mach.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_create_mach(machname.encode("utf-8"), machcls.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeCreateLinker(linkername, linkercls):
	libcommon.skyeye_create_linker.restype = skyeye_api_result
	libcommon.skyeye_create_linker.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_create_linker(linkername.encode("utf-8"), linkercls.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeCreateCpu(machname, cpuname, cpucls):
	libcommon.skyeye_create_cpu.restype = skyeye_api_result
	libcommon.skyeye_create_cpu.argtypes = [c_char_p, c_char_p, c_char_p]
	ret = libcommon.skyeye_create_cpu(machname.encode("utf-8"), cpuname.encode("utf-8"), cpucls.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeCreateDevice(machname, devicename, devcls):
	libcommon.skyeye_create_device.restype = skyeye_api_result
	libcommon.skyeye_create_device.argtypes = [c_char_p, c_char_p, c_char_p]
	ret = libcommon.skyeye_create_device(machname.encode("utf-8"), devicename.encode("utf-8"), devcls.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]


@swtich_try
def SkyEyeConfigConfObj():
	libcommon.skyeye_config_obj.restype = skyeye_api_result
	ret = libcommon.skyeye_config_obj()
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeSetMinSynTime(s, delay):
	libcommon.skyeye_set_min_syn_time.restype = skyeye_api_result
	libcommon.skyeye_set_min_syn_time.argtypes = [c_double, c_double]
	ret = libcommon.skyeye_set_min_syn_time(s, delay)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeSetAttr(objname, key, attr_type, value):
	libcommon.skyeye_set_attr.restype = skyeye_api_result
	libcommon.skyeye_set_attr.argtypes = [c_char_p, c_char_p, c_char_p, c_char_p]
	ret = libcommon.skyeye_set_attr(objname.encode("utf-8"), key.encode("utf-8"), attr_type.encode("utf-8"), value.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeConnect(connect_obj, iface_obj, name, index):
	libcommon.skyeye_connect_device.restype = skyeye_api_result
	libcommon.skyeye_connect_device.argtypes = [c_char_p, c_char_p, c_char_p, c_uint]
	ret = libcommon.skyeye_connect_device(connect_obj.encode("utf-8"), iface_obj.encode("utf-8"), name.encode("utf-8"), index)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeMpAddMap(memory_space, device, addr, length):
	libcommon.skyeye_add_map.restype = skyeye_api_result
	libcommon.skyeye_add_map.argtypes = [c_char_p, c_char_p, c_uint64, c_uint]
	ret = libcommon.skyeye_add_map(memory_space.encode("utf-8"), device.encode("utf-8"), addr, length)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeMpAddMapGroup(memory_space, device, addr, length, index):
	libcommon.skyeye_add_map_group.restype = skyeye_api_result
	libcommon.skyeye_add_map_group.argtypes = [c_char_p, c_char_p, c_uint64, c_uint, c_uint]
	ret = libcommon.skyeye_add_map_group(memory_space.encode("utf-8"), device.encode("utf-8"), addr, length, index)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeDevGetRegNameById(corename, id):
	libcommon.skyeye_get_regname_by_id.restype = skyeye_api_result
	libcommon.skyeye_get_regname_by_id.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_get_regname_by_id(corename.encode("utf-8"), id)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeDevGetRegValueById(corename, id):
	libcommon.skyeye_get_regvalue_by_id.restype = skyeye_api_result
	libcommon.skyeye_get_regvalue_by_id.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_get_regvalue_by_id(corename.encode("utf-8"), id)
	return [ret.result, ret.retLongValue, ret.errMsg]

@swtich_try
def SkyEyeGetPcByCoreName(corename):
	libcommon.skyeye_get_pc_by_cpuname.restype = skyeye_api_result
	libcommon.skyeye_get_pc_by_cpuname.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_pc_by_cpuname(corename.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeStepi(corename, step):
	libcommon.skyeye_step_run.restype = skyeye_api_result
	libcommon.skyeye_step_run.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_step_run(corename.encode("utf-8"), step.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetModules():
	libcommon.skyeye_get_module_names.restype = skyeye_api_result
	ret = libcommon.skyeye_get_module_names()
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeGetModuleValueByName(ModuleName, key):
	libcommon.skyeye_module_get_value_by_name.restype = skyeye_api_result
	libcommon.skyeye_module_get_value_by_name.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_module_get_value_by_name(ModuleName.encode("utf-8"), key.encode("utf-8"))
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeGetModulePathByName(ModuleName):
	libcommon.skyeye_module_get_path_by_name.restype = skyeye_api_result
	libcommon.skyeye_module_get_path_by_name.argtypes = [c_char_p]
	ret = libcommon.skyeye_module_get_path_by_name(ModuleName.encode("utf-8"))
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeLoadModuleFromDir(DirName):
	libcommon.skyeye_load_module_fromdir.restype = skyeye_api_result
	libcommon.skyeye_load_module_fromdir.argtypes = [c_char_p]
	ret = libcommon.skyeye_load_module_fromdir(DirName.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeCreateRemoteGdb(Target, Port, Ip):
	libcommon.skyeye_create_remote_gdb.restype = skyeye_api_result
	libcommon.skyeye_create_remote_gdb.argtypes = [c_char_p, c_uint, c_char_p]
	port_int = int(Port)
	ret = libcommon.skyeye_create_remote_gdb(Target.encode("utf-8"), port_int, Ip.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeDeleteRemoteGdb(Target):
	libcommon.skyeye_delete_remote_gdb.restype = skyeye_api_result
	libcommon.skyeye_delete_remote_gdb.argtypes = [c_char_p]
	ret = libcommon.skyeye_delete_remote_gdb(Target.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeRemoteGdbCheckLink(Target):
	libcommon.skyeye_remote_gdb_check_link.restype = skyeye_api_result
	libcommon.skyeye_remote_gdb_check_link.argtypes = [c_char_p]
	ret = libcommon.skyeye_remote_gdb_check_link(Target.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeRemoteGdbGetClientIp(Target):
	libcommon.skyeye_remote_gdb_get_client_ip.restype = skyeye_api_result
	libcommon.skyeye_remote_gdb_get_client_ip.argtypes = [c_char_p]
	ret = libcommon.skyeye_remote_gdb_get_client_ip(Target.encode("utf-8"))
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeLoadFile(cpuname, filename, pc_addr):
	libcommon.skyeye_load_file.restype = skyeye_api_result
	libcommon.skyeye_load_file.argtypes = [c_char_p, c_char_p, c_uint]
	ret = libcommon.skyeye_load_file(cpuname.encode("utf-8"), filename.encode("utf-8"), pc_addr)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetDevRegNum(machname, devicename):
	libcommon.skyeye_get_register_num.restype = skyeye_api_result
	libcommon.skyeye_get_register_num.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_register_num(devicename.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetDevRegNameById(machname, devicename, regid):
	libcommon.skyeye_get_regname_by_id.restype = skyeye_api_result
	libcommon.skyeye_get_regname_by_id.argtypes = [c_char_p, c_uint]
	ret = libcommon.skyeye_get_regname_by_id(devicename.encode("utf-8"), regid)
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeGetDevRegValueById(machname, devicename, regid):
	libcommon.skyeye_get_regvalue_by_id.restype = skyeye_api_result
	libcommon.skyeye_get_regvalue_by_id.argtypes = [c_char_p, c_uint]
	ret = libcommon.skyeye_get_regvalue_by_id(devicename.encode("utf-8"), regid)
	return [ret.result, ret.retLongValue, ret.errMsg]

@swtich_try
def SkyEyeGetDevRegOffsetById(machname, devicename, regid):
	libcommon.skyeye_get_regoffset_by_id.restype = skyeye_api_result
	libcommon.skyeye_get_regoffset_by_id.argtypes = [c_char_p, c_uint]
	ret = libcommon.skyeye_get_regoffset_by_id(devicename.encode("utf-8"), regid)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeSetDevRegValueById(machname, devicename, value, regid):
	libcommon.skyeye_set_regvalue_by_id.restype = skyeye_api_result
	libcommon.skyeye_set_regvalue_by_id.argtypes = [c_char_p, c_uint64, c_uint]
	ret = libcommon.skyeye_set_regvalue_by_id(devicename.encode("utf-8"), value, regid)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetDevRegIdByName(machname, devicename, regname):
	libcommon.skyeye_get_regid_by_name.restype = skyeye_api_result
	libcommon.skyeye_get_regid_by_name.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_get_regid_by_name(devicename.encode("utf-8"), regname.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeDisassemble(cpuname, addr):
	libcommon.skyeye_disassemble_by_addr.restype = skyeye_api_result
	libcommon.skyeye_disassemble_by_addr.argtypes = [c_char_p, c_uint]
	ret = libcommon.skyeye_disassemble_by_addr(cpuname.encode("utf-8"), addr)
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeReadMemory8(cpuname, addr):
	libcommon.skyeye_memory_read.restype = skyeye_api_result
	libcommon.skyeye_memory_read.argtypes = [c_char_p, c_uint, c_uint]
	ret = libcommon.skyeye_memory_read(cpuname.encode("utf-8"), addr, 1)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeWriteMemory(cpuname,addr,value,length):
	libcommon.skyeye_memory_write.restype = skyeye_api_result
	libcommon.skyeye_memory_write.argtypes = [c_char_p, c_uint, c_char_p, c_uint]
	ret = libcommon.skyeye_memory_write(cpuname.encode("utf-8"), addr, value.encode('latin1'), length)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetAddressWidth(cpuname):
	libcommon.skyeye_get_cpu_address_width.restype = skyeye_api_result
	libcommon.skyeye_get_cpu_address_width.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_cpu_address_width(cpuname.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeWriteDevice4(device, offset, data):
	libcommon.skyeye_device_write.restype = skyeye_api_result
	libcommon.skyeye_device_write.argtypes = [c_char_p, c_uint, c_char_p, c_uint]
	ret = libcommon.skyeye_device_write(device.encode("utf-8"), offset, data.encode("utf-8"), 4)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeWriteDevice(device, offset, data, size):
	libcommon.skyeye_device_write.restype = skyeye_api_result
	libcommon.skyeye_device_write.argtypes = [c_char_p, c_uint, c_char_p, c_uint]
	ret = libcommon.skyeye_device_write(device.encode("utf-8"), offset, data.encode("utf-8"), size)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeReadDevice4(device, offset):
	libcommon.skyeye_device_read.restype = skyeye_api_result
	libcommon.skyeye_device_read.argtypes = [c_char_p, c_uint, c_uint]
	ret = libcommon.skyeye_device_read(device.encode("utf-8"), offset, 4)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeReadDataInjectDevice(device):
	data = create_string_buffer('\0'*1000)
	libcommon.skyeye_inject_device_read.restype = skyeye_api_result
	libcommon.skyeye_inject_device_read.argtypes = [c_char_p]
	ret = libcommon.skyeye_inject_device_read(device.encode("utf-8"), byref(data))
	return [ret.result, data[:ret.retIntValue], ret.errMsg]

@swtich_try
def SkyEyeWriteDataInjectDevice(device,value,lens=-1):
	if lens==-1:
		lens=len(value)
	libcommon.skyeye_inject_device_write.restype = skyeye_api_result
	libcommon.skyeye_inject_device_write.argtypes = [c_char_p, c_char_p, c_int]
	ret = libcommon.skyeye_inject_device_write(device.encode("utf-8"), value.encode("utf-8"), lens)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetClassList():
	libcommon.skyeye_get_class_list.restype = skyeye_api_result
	ret = libcommon.skyeye_get_class_list()
	cls_l = []
	for cls in ret.list:
		if cls == None:
			break
		cls_l.append(cls)
	return [ret.result, cls_l, ret.errMsg]

@swtich_try
def SkyEyeGetClassType(name):
	libcommon.skyeye_get_class_type.restype = skyeye_api_result
	libcommon.skyeye_get_class_type.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_class_type(name.encode("utf-8"))
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeGetClassIfaceList(cls):
	libcommon.skyeye_get_interface_list.restype = skyeye_api_result
	libcommon.skyeye_get_interface_list.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_interface_list(cls.encode("utf-8"))
	iface_l = []
	for iface in ret.list:
		if iface == None:
			break
		iface_l.append(iface)
	return [ret.result, iface_l, ret.errMsg]

@swtich_try
def SkyEyeGetClassConnectList(cls):
	libcommon.skyeye_get_connect_list.restype = skyeye_api_result
	libcommon.skyeye_get_connect_list.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_connect_list(cls.encode("utf-8"))
	connect_l = []
	for connect in ret.list:
		if connect == None:
			break
		connect_l.append(connect)
	return [ret.result, connect_l, ret.errMsg]

@swtich_try
def SkyEyeGetClassAttrList(cls):
	libcommon.skyeye_get_attr_list.restype = skyeye_api_result
	libcommon.skyeye_get_attr_list.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_attr_list(cls.encode("utf-8"))
	attr_l = []
	for attr in ret.list:
		if attr == None:
			break
		attr_l.append(attr)
	return [ret.result, attr_l, ret.errMsg]

@swtich_try
def SkyEyeGetClassAttrInfo(cls, attr):
	libcommon.skyeye_get_attr_info.restype = skyeye_api_result
	libcommon.skyeye_get_attr_info.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_get_attr_info(cls.encode("utf-8"), attr.encode("utf-8"))
	info_l = []
	for info in ret.list:
		if info == None:
			break
		info_l.append(info)
	return [ret.result, info_l, ret.errMsg]

@swtich_try
def SkyEyeSetFj(mp, addr, bit, mode, count):
	libcommon.skyeye_set_fault.restype = skyeye_api_result
	libcommon.skyeye_set_fault.argtypes = [c_char_p, c_uint, c_uint, c_uint, c_uint]
	ret = libcommon.skyeye_set_fault(mp.encode("utf-8"), addr, bit, mode, count)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetFj(mp, addr):
	libcommon.skyeye_get_fault.restype = skyeye_api_result
	libcommon.skyeye_get_fault.argtypes = [c_char_p, c_uint]
	ret = libcommon.skyeye_get_fault(mp.encode("utf-8"), addr)
	fj_l = []
	stru = ret.fault_inject.contents
	cnt = stru.count
	for i in range(0, cnt):
		dic = {}
		fi = cast(stru.fi[i*2 + 1], fi_type).contents
		dic["bit"] = fi.bit
		dic["addr"] = fi.addr
		dic["mode"] = fi.mode
		fj_l.append(dic)
	return [ret.result, fj_l, ret.errMsg]

@swtich_try
def SkyEyeClearFj(mp, addr, bit, mode):
	libcommon.skyeye_clear_fault.restype = skyeye_api_result
	libcommon.skyeye_clear_fault.argtypes = [c_char_p, c_uint, c_uint, c_uint]
	ret = libcommon.skyeye_clear_fault(mp.encode("utf-8"), addr, bit, mode)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeX86GetDeviceNum(cpu):
	libcommon.x86_get_device_num.restype = c_uint
	libcommon.x86_get_device_num.argtypes = [c_char_p]
	num = libcommon.x86_get_device_num(cpu.encode("utf-8"))
	return [True, num, None]

@swtich_try
def SkyEyeX86GetDeviceName(cpu, did):
	libcommon.x86_get_device_name.restype = c_char_p
	libcommon.x86_get_device_name.argtypes = [c_char_p, c_uint]
	name = libcommon.x86_get_device_name(cpu.encode("utf-8"), did)
	return [True, name, None]

@swtich_try
def SkyEyeX86GetDeviceRegNum(cpu, did):
	libcommon.x86_get_device_reg_num.restype = c_uint
	libcommon.x86_get_device_reg_num.argtypes = [c_char_p, c_uint]
	num  = libcommon.x86_get_device_reg_num(cpu.encode("utf-8"), did)
	return [True, num, None]

@swtich_try
def SkyEyeX86GetRegName(cpu, did, rid):
	libcommon.x86_get_device_reg_name.restype = c_char_p
	libcommon.x86_get_device_reg_name.argtypes = [c_char_p, c_uint, c_uint]
	name = libcommon.x86_get_device_reg_name(cpu.encode("utf-8"), did, rid)
	return [True, name, None]

@swtich_try
def SkyEyeX86GetRegValue(cpu, did, rid):
	libcommon.x86_get_device_reg_value.restype = c_uint
	libcommon.x86_get_device_reg_value.argtypes = [c_char_p, c_uint, c_uint]
	val = libcommon.x86_get_device_reg_value(cpu.encode("utf-8"), did, rid)
	return [True, val, None]

@swtich_try
def SkyEyeX86GetLargeRegValue(cpu, did, rid,offset):
	libcommon.x86_get_device_reg_large_value.restype = c_uint
	libcommon.x86_get_device_reg_large_value.argtypes = [c_char_p, c_uint, c_uint, c_uint]
	val = libcommon.x86_get_device_reg_large_value(cpu.encode("utf-8"), did, rid, offset)
	return [True, val, None]

@swtich_try
def SkyEyeX86GetDeviceRegWidth(cpu, did):
	libcommon.x86_get_device_reg_width.restype = c_uint
	libcommon.x86_get_device_reg_width.argtypes = [c_char_p, c_uint]
	width = libcommon.x86_get_device_reg_width(cpu.encode("utf-8"), did)
	return [True, width, None]

@swtich_try
def SkyEyeX86GetRegAddr(cpu, did, rid):
	libcommon.x86_get_device_reg_addr.restype = c_uint
	libcommon.x86_get_device_reg_addr.argtypes = [c_char_p, c_uint, c_uint]
	addr = libcommon.x86_get_device_reg_addr(cpu.encode("utf-8"), did, rid)
	return [True, addr, None]

@swtich_try
def SkyEyeX86Reset(cpuname):
	libcommon.x86_reset.argtypes = [c_char_p]
	libcommon.x86_reset(cpuname.encode("utf-8"))
	return [True, True, None]

@swtich_try
def SkyEyeX86SaveFin(cpu, did, addr, bit, mode):
	libcommon.x86_device_save_fin.argtypes = [c_char_p, c_uint, c_uint, c_uint, c_uint]
	ret = libcommon.x86_device_save_fin(cpu.encode("utf-8"), did, addr, bit, mode)
	return [True, ret, None]

x86_int_list = c_uint *40
class x86_fi_struct(Structure):
	_fields_ = [
		("addr", c_uint),
		("bit",c_uint),
		("mode", c_uint)
	]

class x86_fj_struct(Structure):
	_fields_ = [
		("count",c_int),
		("fi", int_list)
	]
x86_fi_type = POINTER(x86_fi_struct)
x86_fj_type = POINTER(x86_fj_struct)

@swtich_try
def SkyEyeX86GetFin(cpu, did):
	fj_l = []
	libcommon.x86_device_get_fin.restype = x86_fj_type
	libcommon.x86_device_get_fin.argtypes = [c_char_p, c_uint]
	stru = libcommon.x86_device_get_fin(cpu.encode("utf-8"), did)
	try:
		stru = stru.contents
	except:
		return [True, fj_l, None]
	cnt = stru.count
	for i in range(0, cnt):
		dic = {}
		fi = cast(stru.fi[i*2 + 1], x86_fi_type).contents
		dic["bit"] = fi.bit
		dic["addr"] = fi.addr
		dic["mode"] = fi.mode
		fj_l.append(dic)
	return [True, fj_l, None]

@swtich_try
def SkyEyeX86ClearFin(cpu, did, addr, bit, mode):
	libcommon.x86_device_clear_fin.argtypes = [c_char_p, c_uint, c_uint, c_uint, c_uint]
	ret = libcommon.x86_device_clear_fin(cpu.encode("utf-8"), did, addr, bit, mode)
	return [True, ret, None]

@swtich_try
def SkyEyeX86SaveConfigure(cpu, filename):
	libcommon.x86_save_configure.argtypes = [c_char_p, c_char_p]
	libcommon.x86_save_configure(cpu.encode("utf-8"), filename.encode("utf-8"))
	return [True, True, None]

@swtich_try
def SkyEyeX86LoadConfigure(cpu, filename):
	libcommon.x86_load_configure.argtypes = [c_char_p, c_char_p]
	libcommon.x86_load_configure(cpu.encode("utf-8"), filename.encode("utf-8"))
	return [True, True, None]

@swtich_try
def SkyEyeStoreConfigure(filename):
	libcommon.WIN_store_configure.argtypes = [c_char_p]
	libcommon.WIN_store_configure(filename.encode("utf-8"))
	return [True, True, None]

@swtich_try
def SkyEyeRecoverConfigure(filename):
	libcommon.WIN_recover_configure.argtypes = [c_char_p]
	libcommon.WIN_recover_configure(filename.encode("utf-8"))
	return [True, True, None]

@swtich_try
def SkyEyeImageGetPageCount(imagename):
	libcommon.WIN_image_get_page_count.restype = c_uint
	libcommon.WIN_image_get_page_count.argtypes = [c_char_p]
	ret = libcommon.WIN_image_get_page_count(imagename.encode("utf-8"))
	return [True, ret, None]

@swtich_try
def SkyEyeImageGetPageSize(imagename):
	libcommon.WIN_image_get_page_size.restype = c_uint
	libcommon.WIN_image_get_page_size.argtypes = [c_char_p]
	ret = libcommon.WIN_image_get_page_size(imagename.encode("utf-8"))
	return [True, ret, None]

@swtich_try
def SkyEyeImageGetPageIndexById(imagename, ID):
	libcommon.WIN_image_get_page_index_by_id.restype = c_uint
	libcommon.WIN_image_get_page_index_by_id.argtypes = [c_char_p, c_uint]
	ret = libcommon.WIN_image_get_page_index_by_id(imagename.encode("utf-8"), ID)
	return [True, ret, None]

@swtich_try
def SkyEyeImageClearAllPages(imagename):
	libcommon.WIN_image_get_page_index_by_id.restype = c_uint
	libcommon.WIN_image_get_page_index_by_id.argtypes = [c_char_p]
	ret = libcommon.WIN_image_clear_all_pages(imagename.encode("utf-8"))
	return [True, ret, None]

@swtich_try
def SkyEyeImagePageToFile(imagename, index, filename):
	libcommon.WIN_image_page_to_file.argtypes = [c_char_p, c_uint, c_char_p]
	libcommon.WIN_image_page_to_file(imagename.encode("utf-8"), index, filename.encode("utf-8"))
	return [True, True, None]

@swtich_try
def SkyEyeImageFileToPage(imagename, index, filename):
	libcommon.WIN_image_file_to_page.argtypes = [c_char_p, c_uint, c_char_p]
	libcommon.WIN_image_file_to_page(imagename.encode("utf-8"), index, filename.encode("utf-8"))
	return [True, True, None]

@swtich_try
def SkyEyeLoadBinary(cpuname, filename):
	libcommon.skyeye_load_binary.restype = skyeye_api_result
	libcommon.skyeye_load_binary.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_load_binary(cpuname.encode("utf-8"), filename.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeParseSymbol(cpuname, filename):
	libcommon.skyeye_parse_symbol.restype = skyeye_api_result
	libcommon.skyeye_parse_symbol.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_parse_symbol(cpuname.encode("utf-8"), filename.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetCpuSteps(cpuname):
	libcommon.skyeye_get_cpu_steps.restype = skyeye_api_result
	libcommon.skyeye_get_cpu_steps.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_cpu_steps(cpuname.encode("utf-8"))
	return [ret.result, ret.retLongValue, ret.errMsg]

@swtich_try
def SkyEyeCpuSetMode(cpuname, mode):
	libcommon.skyeye_set_cpu_run_mode.restype = skyeye_api_result
	libcommon.skyeye_set_cpu_run_mode.argtypes = [c_char_p, c_uint]
	ret = libcommon.skyeye_set_cpu_run_mode(cpuname.encode("utf-8"), mode)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeSetTimeNum(num):
	libcommon.set_time_num.argtypes = [c_uint]
	libcommon.set_time_num(num)
	return [True, True, None]

@swtich_try
def SkyEyeGetExecutedPcFile(instr_process, fname):
	libcommon.skyeye_get_executed_pc_file.restype = skyeye_api_result
	libcommon.skyeye_get_executed_pc_file.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_get_executed_pc_file(instr_process.encode("utf-8"), fname.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetPcRecordSize(instr_process):
	libcommon.skyeye_get_pc_record_size.restype = skyeye_api_result
	libcommon.skyeye_get_pc_record_size.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_pc_record_size(instr_process.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeSetPcRecordSize(instr_process, size):
	libcommon.skyeye_set_pc_record_size.restype = skyeye_api_result
	libcommon.skyeye_set_pc_record_size.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_set_pc_record_size(instr_process.encode("utf-8"), size)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetPcRecordNums(instr_process):
	libcommon.skyeye_get_pc_record_nums.restype = skyeye_api_result
	libcommon.skyeye_get_pc_record_nums.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_pc_record_nums(instr_process.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetPcRecordIndex(instr_process):
	libcommon.skyeye_get_pc_record_index.restype = skyeye_api_result
	libcommon.skyeye_get_pc_record_index.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_pc_record_index(instr_process.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetPcRecordOverflow(instr_process):
	libcommon.skyeye_get_pc_record_overflow.restype = skyeye_api_result
	libcommon.skyeye_get_pc_record_overflow.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_pc_record_overflow(instr_process.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetPcByIndex(instr_process, n):
	libcommon.skyeye_get_pc_by_index.restype = skyeye_api_result
	libcommon.skyeye_get_pc_by_index.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_get_pc_by_index(instr_process.encode("utf-8"), n)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetArchitecture(cpuname):
	libcommon.skyeye_get_cpu_architecture.restype = skyeye_api_result
	libcommon.skyeye_get_cpu_architecture.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_cpu_architecture(cpuname.encode("utf-8"))
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyPreTranslate(cpuname):
	libcommon.do_pre_translate.argtypes = [c_char_p]
	libcommon.do_pre_translate(cpuname)
	return [True, True, None]

@swtich_try
def SkyEyeGetPinNum(device):
	libcommon.get_pin_num.restype = c_int
	libcommon.get_pin_num.argtypes = [c_char_p]
	pinnum = libcommon.get_pin_num(device.encode("utf-8"))
	return [True, pinnum, None]

@swtich_try
def SkyEyeGetPinType(device, index):
	libcommon.get_pin_type.restype = c_int
	libcommon.get_pin_type.argtypes = [c_char_p, c_uint]
	pinType = libcommon.get_pin_type(device.encode("utf-8"), int(index))
	return [True, pinType, None]

@swtich_try
def SkyEyeReadPinState(device, index):
	libcommon.read_pin_state.restype = c_float
	libcommon.read_pin_state.argtypes = [c_char_p, c_uint]
	pinState = libcommon.read_pin_state(device.encode("utf-8"), int(index))
	return [True, pinState, None]

@swtich_try
def SkyEyeWritePinState(device, index, state):
	libcommon.write_pin_state.argtypes = [c_char_p, c_uint, c_float]
	libcommon.write_pin_state(device.encode("utf-8"), int(index), float(state))
	return [True, True, None]

@swtich_try
def PythonTmrCreate(cpu, ms, mode):
	libpytimer.PythonTmrCreate.restype = c_uint
	libpytimer.PythonTmrCreate.argtypes = [c_char_p, c_uint, c_uint]
	ret = libpytimer.PythonTmrCreate(cpu.encode("utf-8"), ms, mode)
	return [True, ret, None]

@swtich_try
def PythonTmrCheckHit(pytmr):
	libpytimer.PythonTmrCheckHit.restype = c_uint
	libpytimer.PythonTmrCheckHit.argtypes = [c_uint]
	ret = libpytimer.PythonTmrCheckHit(pytmr)
	return [True, ret, None]

@swtich_try
def PythonTmrProcessOk(pytmr):
	libpytimer.PythonTmrProcessOk.argtypes = [c_uint]
	ret = libpytimer.PythonTmrProcessOk(pytmr)
	return [True, ret, None]

@swtich_try
def PythonTmrDelete(pytmr):
	libpytimer.PythonTmrDelete.restype = c_bool
	libpytimer.PythonTmrDelete.argtypes = [c_uint]
	ret = libpytimer.PythonTmrDelete(pytmr)
	return [True, ret, None]

@swtich_try
def SkyEyeSetWatchOnPc(cpu,addr):
	libpytimer.PythonSetWatchOnPc.restype = c_uint
	libpytimer.PythonSetWatchOnPc.argtypes = [c_char_p, c_uint32]
	ret = libpytimer.PythonSetWatchOnPc(cpu.encode("utf-8"),addr)
	return [True, ret, None]

@swtich_try
def SkyEyeUnWatchOnPc(cpu,addr):
	libpytimer.PythonUnWatchOnPc.argtypes = [c_char_p, c_uint32]
	libpytimer.PythonUnWatchOnPc(cpu.encode("utf-8"), addr)
	return [True, True, None]

@swtich_try
def SkyEyeSetWatchOnMem(memory_space, mm_type, m_addr, data_type, m_length):
	libpytimer.PythonSetWatchonMemory.restpye = c_uint
	libpytimer.PythonSetWatchonMemory.argtypes = [c_char_p, c_int, c_uint32, c_uint32, c_uint32]
	ret = libpytimer.PythonSetWatchonMemory(memory_space.encode("utf-8"),mm_type,m_addr,data_type,m_length)
	return [True, ret, None]

@swtich_try
def SkyEyeUnWatchOnMem(memory_space, mm_type, m_addr,m_length):
	libpytimer.PythonUnWatchOnMemory.argtypes = [c_char_p, c_int, c_uint32, c_uint32]
	libpytimer.PythonUnWatchOnMemory(memory_space.encode("utf-8"),mm_type, m_addr,m_length)
	return [True, True, None]

@swtich_try
def SkyEyeGetNextLog():
	libcommon.skyeye_get_next_logMsg.restype = skyeye_api_result
	ret = libcommon.skyeye_get_next_logMsg()
	return [ret.result, ret.retStrValue, ret.errMsg]

class vm(Structure):
	_fields_ = [
		("id",c_uint32),
		("type",c_uint32),
		("name",c_char_p)
	]

class vm_py(Structure):
	_fields_ = [
		("count",c_int),
		("vm_list",POINTER(vm))
	]

@swtich_try
def SkyEyeGetVms(limit=256):
	vm_date=vm_py()
	libcommon.WIN_get_vms(byref(vm_date),c_int(int(limit)))
	vm_list=[]
	for i in range(0,vm_date.count):
		temp={}
		temp["id"] = vm_date.vm_list[i].id
		temp["type"] = vm_date.vm_list[i].type
		temp["name"] = vm_date.vm_list[i].name
		vm_list.append(temp)
	return [True, vm_list, None]

class var(Structure):
	_fields_ = [
		("type",c_uint32),
		("t",c_uint64),
		("uint_v",c_uint64),
		("int_v",c_int64),
		("double_v",c_double)
	]

class var_py(Structure):
	_fields_ = [
		("count",c_int),
		("var_list",POINTER(var))
	]

@swtich_try
def SkyEyeGetVars(Id,limit=1000):
	var_date=var_py()
	libcommon.WIN_get_vars(byref(var_date),c_uint(Id),c_uint(limit))
	var_list=[]
	for i in range(0,var_date.count):
		temp={}
		temp["type"] = var_date.var_list[i].type
		temp["t"] = var_date.var_list[i].t
		temp["uint_v"] = var_date.var_list[i].uint_v
		temp["int_v"] = var_date.var_list[i].int_v
		temp["double_v"] = var_date.var_list[i].double_v
		var_list.append(temp)
	return [True, var_list, None]

@swtich_try
def SkyEyeSetVarConditions(Id, condition):
	libcommon.WIN_insert_exp.argtypes = [c_uint, c_char_p]
	libcommon.WIN_insert_exp(Id, condition.encode("utf-8"))
	return [True, True, None]

@swtich_try
def SkyEyeClearVarConditions(Id):
	libcommon.WIN_remove_exp.argtypes = [c_char_p]
	libcommon.WIN_remove_exp(Id)
	return [True, True, None]

@swtich_try
def SkyEyeListDir(path):
	libcommon.skyeye_list_dir.restype = skyeye_api_result
	libcommon.skyeye_list_dir.argtypes = [c_char_p]
	ret = libcommon.skyeye_list_dir(path.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeListModules():
	libcommon.skyeye_list_modules.restype = skyeye_api_result
	ret = libcommon.skyeye_list_modules()
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeMemoryInfo(args):
	libcommon.skyeye_get_mm_info.restype = skyeye_api_result
	libcommon.skyeye_get_mm_info.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_mm_info(args.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeSetSocDelayLoad(soc, ms):
	libcommon.set_soc_delay_load.restype = c_uint
	libcommon.set_soc_delay_load.argtypes = [c_char_p, c_uint]
	libcommon.set_soc_delay_load(soc.encode("utf-8"), ms)
	return [True, True, None]

@swtich_try
def SkyEyeSetX86Slow(cpu, isLow):
	libcommon.slow_x86.argtypes = [c_char_p, c_uint]
	libcommon.slow_x86(cpu.encode("utf-8"), isLow)
	return [True, True, None]

@swtich_try
def SkyEyeLicenseVerify():
	libcommon.skyeye_license_verify.restype = skyeye_api_result
	ret = libcommon.skyeye_license_verify()
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeReadLicenseInfo(filename):
	libcommon.skyeye_read_license_info.restype = skyeye_api_result
	libcommon.skyeye_read_license_info.argtypes = [c_char_p]
	ret = libcommon.skyeye_read_license_info(filename.encode("utf-8"))
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeGetCpuId():
	libcommon.skyeye_get_cpuid.restype = skyeye_api_result
	ret = libcommon.skyeye_get_cpuid()
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeCheckUsbLicConnect():
	libcommon.skyeye_check_usbKey_connect.restype = skyeye_api_result
	ret = libcommon.skyeye_check_usbKey_connect()
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetSimulationRunTime(cpuname):
	libcommon.skyeye_get_simulation_time.restype = skyeye_api_result
	libcommon.skyeye_get_simulation_time.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_simulation_time(cpuname.encode("utf-8"))
	return [ret.result, ret.retDoubleValue, ret.errMsg]

@swtich_try
def SkyEyeGetSimulationInsnNum(cpuname):
	libcommon.skyeye_get_simulation_insn_num.restype = skyeye_api_result
	libcommon.skyeye_get_simulation_insn_num.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_simulation_insn_num(cpuname.encode("utf-8"))
	return [ret.result, ret.retLongValue, ret.errMsg]

@swtich_try
def SkyEyeGetDeviceModuleType(devicename):
	libcommon.skyeye_get_device_module_type.restype = skyeye_api_result
	libcommon.skyeye_get_device_module_type.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_device_module_type(devicename.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetInstrProcessDevice(machname, cpuname):
	libcommon.skyeye_get_instr_process_device.restype = skyeye_api_result
	libcommon.skyeye_get_instr_process_device.argtypes = [c_char_p, c_char_p, c_char_p]
	ret = libcommon.skyeye_get_instr_process_device(machname.encode("utf-8"), cpuname.encode("utf-8"), "instr_process".encode("utf-8"))
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeSetScriptPath(script_file_name):
	libcommon.skyeye_set_script_path.restype = skyeye_api_result
	libcommon.skyeye_set_script_path.argtypes = [c_char_p]
	ret = libcommon.skyeye_set_script_path(script_file_name.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetWorkFullPath():
	libcommon.skyeye_get_work_full_path.restype = skyeye_api_result
	ret = libcommon.skyeye_get_work_full_path()
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeGetWorkPath():
	libcommon.skyeye_get_work_path.restype = skyeye_api_result
	ret = libcommon.skyeye_get_work_path()
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeGetSymbolAddr(cpuname,varname):
	libcommon.skyeye_get_symbol_addr.restype = skyeye_api_result
	libcommon.skyeye_get_symbol_addr.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_get_symbol_addr(cpuname.encode("utf-8"),varname.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetSymbolValue(cpuname,varname,value_bytes_number):
	libcommon.skyeye_get_symbol_value.restype = skyeye_api_result
	libcommon.skyeye_get_symbol_value.argtypes = [c_char_p, c_char_p, c_int]
	ret = libcommon.skyeye_get_symbol_value(cpuname.encode("utf-8"), varname.encode("utf-8"), value_bytes_number)
	return [ret.result, ret.retLongValue, ret.errMsg]

@swtich_try
def SkyEyeGetFloatSymbolValue(cpuname,varname,value_bytes_number):
	libcommon.skyeye_get_float_symbol_value.restype = skyeye_api_result
	libcommon.skyeye_get_float_symbol_value.argtypes = [c_char_p, c_char_p, c_int]
	ret = libcommon.skyeye_get_float_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"), value_bytes_number)
	return [ret.result, ret.retFloatValue, ret.errMsg]
	
@swtich_try
def SkyEyeGetDoubleSymbolValue(cpuname,varname,value_bytes_number):
	libcommon.skyeye_get_double_symbol_value.restype = skyeye_api_result
	libcommon.skyeye_get_double_symbol_value.argtypes = [c_char_p, c_char_p, c_int]
	ret = libcommon.skyeye_get_double_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"), value_bytes_number)
	return [ret.result, ret.retDoubleValue, ret.errMsg]

@swtich_try
def SkyEyeSetSymbolValue(cpuname,varname,value,value_bytes_number):
	libcommon.skyeye_set_symbol_value.restype = skyeye_api_result
	libcommon.skyeye_set_symbol_value.argtypes = [c_char_p, c_char_p, c_uint64, c_int]
	ret = libcommon.skyeye_set_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"), value,value_bytes_number)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeSetFloatSymbolValue(cpuname,varname,value,value_bytes_number):
	libcommon.skyeye_set_float_symbol_value.restype = skyeye_api_result
	libcommon.skyeye_set_float_symbol_value.argtypes = [c_char_p, c_char_p, c_float, c_int]
	ret = libcommon.skyeye_set_float_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"),value,value_bytes_number)
	return [ret.result, ret.retIntValue, ret.errMsg]
	
@swtich_try
def SkyEyeSetDoubleSymbolValue(cpuname,varname,value,value_bytes_number):
	libcommon.skyeye_set_double_symbol_value.restype = skyeye_api_result
	libcommon.skyeye_set_double_symbol_value.argtypes = [c_char_p, c_char_p, c_double, c_int]
	ret = libcommon.skyeye_set_double_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"),value,value_bytes_number)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeLoadBinBinary(cpuname, filename, addr, length, start_pc):
	libcommon.skyeye_load_bin_binary.restype = skyeye_api_result
	libcommon.skyeye_load_bin_binary.argtypes = [c_char_p, c_char_p, c_uint, c_uint, c_uint]
	ret = libcommon.skyeye_load_bin_binary(cpuname.encode("utf-8"), filename.encode("utf-8"), addr, length, start_pc)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetCpuFreq(cpuname):
	libcommon.skyeye_get_cpu_freq.restype = skyeye_api_result
	libcommon.skyeye_get_cpu_freq.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_cpu_freq(cpuname.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeReadByte(cpuname,addr):
	libcommon.skyeye_memory_read.restype = skyeye_api_result
	libcommon.skyeye_memory_read.argtypes = [c_char_p, c_int, c_uint]
	ret = libcommon.skyeye_memory_read(cpuname.encode("utf-8"), addr, 1)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeReadDWord(cpuname,addr):
	libcommon.skyeye_memory_read.restype = skyeye_api_result
	libcommon.skyeye_memory_read.argtypes = [c_char_p, c_int, c_uint]
	ret = libcommon.skyeye_memory_read(cpuname.encode("utf-8"),addr, 4)
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeWriteByte(cpuname,addr,value):
	libcommon.skyeye_memory_write.restype = skyeye_api_result
	libcommon.skyeye_memory_write.argtypes = [c_char_p, c_int, c_char_p, c_uint]
	ret = libcommon.skyeye_memory_write(cpuname.encode("utf-8"),addr,cast(pointer(c_int(value)),c_char_p), 1)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeWriteDWord(cpuname,addr,value):
	libcommon.skyeye_memory_write.restype = skyeye_api_result
	libcommon.skyeye_memory_write.argtypes = [c_char_p, c_int, c_char_p, c_uint]
	ret = libcommon.skyeye_memory_write(cpuname.encode("utf-8"),addr,cast(pointer(c_int(value)),c_char_p), 4)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetFuncAddr(cpuname,funcname):
	libcommon.skyeye_get_func_addr.restype = skyeye_api_result
	libcommon.skyeye_get_func_addr.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_get_func_addr(cpuname.encode("utf-8"),funcname.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeGetFuncLength(cpuname,funcname):
	libcommon.skyeye_get_func_length.restype = skyeye_api_result
	libcommon.skyeye_get_func_length.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_get_func_length(cpuname.encode("utf-8"),funcname.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeEnableCpuCodecov(machname):
	libcommon.skyeye_enable_cpu_codecov.restype = skyeye_api_result
	libcommon.skyeye_enable_cpu_codecov.argtypes = [c_char_p]
	ret = libcommon.skyeye_enable_cpu_codecov(machname.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetCpuExceptionNum(cpuname):
	libcommon.skyeye_get_cpu_exception_num.restype = skyeye_api_result
	libcommon.skyeye_get_cpu_exception_num.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_cpu_exception_num(cpuname.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeSetCpuException(cpuname, exception_id):
	libcommon.skyeye_set_cpu_exception.restype = skyeye_api_result
	libcommon.skyeye_set_cpu_exception.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_set_cpu_exception(cpuname.encode("utf-8"), exception_id)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeSetExtInterrupt(intc_name, interrupt_num):
	libcommon.skyeye_set_ext_interrupt.restype = skyeye_api_result
	libcommon.skyeye_set_ext_interrupt.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_set_ext_interrupt(intc_name.encode("utf-8"),interrupt_num)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeDisableDeviceWork(device):
	libcommon.skyeye_disable_device_work.restype = skyeye_api_result
	libcommon.skyeye_disable_device_work.argtypes = [c_char_p]
	ret = libcommon.skyeye_disable_device_work(device.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeEnableDeviceWork(device):
	libcommon.skyeye_enable_device_work.restype = skyeye_api_result
	libcommon.skyeye_enable_device_work.argtypes = [c_char_p]
	ret = libcommon.skyeye_enable_device_work(device.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeSaveImage(device):
	libcommon.com_save_image.argtypes = [c_char_p]
	ret = libcommon.com_save_image(device.encode("utf-8"))
	return [True, ret, None]

@swtich_try
def SkyEyeGetCpuExceptionNameById(cpuname, exception_id):
	libcommon.skyeye_get_cpu_exception_name_by_id.restype = skyeye_api_result
	libcommon.skyeye_get_cpu_exception_name_by_id.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_get_cpu_exception_name_by_id(cpuname.encode("utf-8"),exception_id)
	return [ret.result, ret.retStrValue, ret.errMsg]

@swtich_try
def SkyEyeSetPC(cpuname, pc):
	libcommon.skyeye_set_pc.restype = skyeye_api_result
	libcommon.skyeye_set_pc.argtypes = [c_char_p, c_int]
	ret = libcommon.skyeye_set_pc(cpuname.encode("utf-8"),pc)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeGetPC(cpuname):
	libcommon.skyeye_get_pc.restype = skyeye_api_result
	libcommon.skyeye_get_pc.argtypes = [c_char_p]
	ret = libcommon.skyeye_get_pc(cpuname.encode("utf-8"))
	return [ret.result, ret.retIntValue, ret.errMsg]

@swtich_try
def SkyEyeCloseInstrRecord(cpuname):
	libcommon.skyeye_close_instr_record.restype = skyeye_api_result
	libcommon.skyeye_close_instr_record.argtypes = [c_char_p]
	ret = libcommon.skyeye_close_instr_record(cpuname.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeOpenInstrRecord(cpuname, filename):
	libcommon.skyeye_open_instr_record.restype = skyeye_api_result
	libcommon.skyeye_open_instr_record.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_open_instr_record(cpuname.encode("utf-8"),filename.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeEnableDebug(socname):
	libcommon.skyeye_enable_debug.restype = skyeye_api_result
	libcommon.skyeye_enable_debug.argtypes = [c_char_p]
	ret = libcommon.skyeye_enable_debug(socname.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeNewLoadFile(memoryname, filename, pc_addr):
	libcommon.skyeye_new_load_file.restype = skyeye_api_result
	libcommon.skyeye_new_load_file.argtypes = [c_char_p, c_char_p, c_uint]
	ret = libcommon.skyeye_new_load_file(memoryname.encode("utf-8"), filename.encode("utf-8"), pc_addr)
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeTermWaitForString(termname, string):
	libcommon.skyeye_term_wait_for_string.restype = skyeye_api_result
	libcommon.skyeye_term_wait_for_string.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_term_wait_for_string(termname.encode("utf-8"), string.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeTermWrite(termname, string):
	libcommon.skyeye_term_write.restype = skyeye_api_result
	libcommon.skyeye_term_write.argtypes = [c_char_p, c_char_p]
	ret = libcommon.skyeye_term_write(termname.encode("utf-8"), string.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]

@swtich_try
def SkyEyeTermWaitThenWrite(termname, wait_string, write_string):
	libcommon.skyeye_term_wait_then_write.restype = skyeye_api_result
	libcommon.skyeye_term_wait_then_write.argtypes = [c_char_p, c_char_p, c_char_p]
	ret = libcommon.skyeye_term_wait_then_write(termname.encode("utf-8"), wait_string.encode("utf-8"), write_string.encode("utf-8"))
	return [ret.result, ret.result, ret.errMsg]
