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
import functools

###################################################################
#--------------------   Load libs   ------------------------------#
###################################################################
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

###################################################################
#--------------------   Define Stuct of C API   ------------------#
###################################################################
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

class SkyeyeAPIResult(skyeye_api_result):
    @property
    def list(self):
        res = getattr(self, '_list', None)
        if res is None:
            res = []
            list_ = super().list
            if list_:
                for s in list_:
                    if s is None:
                        break
                    res.append(s.decode('utf-8').strip())
            self._list = res
        return res

    @property
    def errMsg(self):
        res = getattr(self, '_errMsg', None)
        if res is None:
            res = []
            errMsg = super().errMsg
            if errMsg:
                for s in errMsg:
                    if s is None:
                        break
                    res.append(s.decode('utf-8').strip())
            self._errMsg = res
        return res

###################################################################
#------------------ Define exception decorator -------------------#
###################################################################
ERROR_ALL_F = '0xffffffff'

class SkyeyeAPIException(Exception):
    Language = 'ch'
    err_json = None

    def __init__(self, errMsg):
        self._errMsg = errMsg
        self.err_json = None

    def __str__(self):
        return self.msg()

    def msg(self):
        if self._errMsg[0] != ERROR_ALL_F:
            if SkyeyeAPIException.err_json is None:
                import json
                try:
                    with open(se_path.ErrorDataPath, encoding='utf-8') as f:
                        SkyeyeAPIException.err_json = json.load(f)
                except Exception as e:
                    raise Exception('读取错误数据文件出错')

            id = int(self._errMsg[0], 0)
            l_id = hex(id & 0xffff)
            h_id = hex(id - (id & 0xffff))
            try:
                msg1 = SkyeyeAPIException.err_json[h_id][SkyeyeAPIException.Language]
                msg2 = SkyeyeAPIException.err_json[l_id][SkyeyeAPIException.Language]
            except Exception as e:
                raise Exception("未找到该ID信息, ID: %x" % id)
            self._errMsg.append(msg1 + ' : ' + msg2)
        return self._errMsg[-1]

def exception_decorator(api_func):
    @functools.wraps(api_func)
    def wrapper(*args):
        c_ret = api_func(*args)
        if not c_ret.result:
            raise SkyeyeAPIException(c_ret.errMsg)
        return c_ret
    return wrapper

###################################################################
#----------------------  Skyeye Python API -----------------------#
###################################################################
def SkyEyeRun():
    res = SkyEyeRunningStatus()
    if res:
        raise SkyeyeAPIException([ERROR_ALL_F, 'Machine has already been running, needn\'t run again.'])

    c_ret = _SkyEyeRun()
    return None

@exception_decorator
def _SkyEyeRun():
    c_ret = libcommon.skyeye_run()
    return c_ret
libcommon.skyeye_run.restype = SkyeyeAPIResult

def SkyEyeStop():
    res = SkyEyeRunningStatus()
    if not res: 
        raise SkyeyeAPIException([ERROR_ALL_F, 'Machine has already stopped, needn\'t stop again.'])
    c_ret = _SkyEyeStop()
    return None

@exception_decorator
def _SkyEyeStop():
    c_ret = libcommon.skyeye_stop()
    return c_ret
libcommon.skyeye_stop.restype = SkyeyeAPIResult

def SkyEyeRunningStatus():
    c_ret = _SkyEyeRunningStatus()
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeRunningStatus():
    c_ret = libcommon.skyeye_running_status()
    return c_ret
libcommon.skyeye_running_status.restype = SkyeyeAPIResult

def SkyEyeQuit():
    # FIXME: 直接退出？
    c_ret = _SkyEyeQuit()
    return None

@exception_decorator
def _SkyEyeQuit():
    c_ret = libcommon.skyeye_quit()
    return c_ret
libcommon.skyeye_quit.restype = SkyeyeAPIResult

def SkyEyeReset():
    # BUG: reset when machine is runnging
    c_ret = _SkyEyeReset()
    return None

@exception_decorator
def _SkyEyeReset():
    c_ret = libcommon.skyeye_reset()
    return c_ret
libcommon.skyeye_reset.restype = SkyeyeAPIResult

def SkyEyePrepareToRun():
    c_ret = _SkyEyePrepareToRun()
    return None

@exception_decorator
def _SkyEyePrepareToRun():
    c_ret = libcommon.skyeye_prepare_running()
    return c_ret
libcommon.skyeye_prepare_running.restype = SkyeyeAPIResult

def SkyEyeClearBpHit(corename):
    # hasn't tested
    c_ret = _SkyEyeClearBpHit(corename)
    return None 

@exception_decorator
def _SkyEyeClearBpHit(corename):
    c_ret = libcommon.skyeye_clear_bp_hit(corename.encode("utf-8"))
    return c_ret
libcommon.skyeye_clear_bp_hit.restype = SkyeyeAPIResult
libcommon.skyeye_clear_bp_hit.argtypes = [c_char_p]

def SkyEyeGetBreakpointHitId(corename):
    c_ret = _SkyEyeGetBreakpointHitId(corename)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetBreakpointHitId(corename):
    c_ret = libcommon.skyeye_get_bp_hit_id(corename.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_bp_hit_id.restype = SkyeyeAPIResult
libcommon.skyeye_get_bp_hit_id.argtypes = [c_char_p]

def SkyEyeGetBreakpointAddrById(corename, id):
    #BUG: id越界, 不报错
    c_ret = _SkyEyeGetBreakpointAddrById(corename, id)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetBreakpointAddrById(corename, id):
    c_ret = libcommon.skyeye_get_breakpoint_address_by_id(corename.encode("utf-8"), id)
    return c_ret
libcommon.skyeye_get_breakpoint_address_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_get_breakpoint_address_by_id.argtypes = [c_char_p, c_int]

def SkyEyeDeleteBreakpointById(corename, id):
    c_ret = _SkyEyeDeleteBreakpointById(corename, id)
    return None

@exception_decorator
def _SkyEyeDeleteBreakpointById(corename, id):
    c_ret = libcommon.skyeye_delete_breakpoint_by_id(corename.encode("utf-8"), id)
    return c_ret
libcommon.skyeye_delete_breakpoint_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_delete_breakpoint_by_id.argtypes = [c_char_p, c_int]

def SkyEyeDeleteBreakpointByAddr(corename, addr):
    c_ret = _SkyEyeDeleteBreakpointByAddr(corename, addr)
    return None

@exception_decorator
def _SkyEyeDeleteBreakpointByAddr(corename, addr):
    return libcommon.skyeye_delete_breakpoint_by_addr(corename.encode("utf-8"), addr)
libcommon.skyeye_delete_breakpoint_by_addr.restype = SkyeyeAPIResult
libcommon.skyeye_delete_breakpoint_by_addr.argtypes = [c_char_p, c_uint64]

def SkyEyeGetBpNumbers(corename):
    c_ret = _SkyEyeGetBpNumbers(corename)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetBpNumbers(corename):
    c_ret = libcommon.skyeye_get_bp_numbers(corename.encode("utf-8"))
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_get_bp_numbers.restype = SkyeyeAPIResult
libcommon.skyeye_get_bp_numbers.argtypes = [c_char_p]

def SkyEyeCheckBreakpointHit(corename):
    c_ret = _SkyEyeCheckBreakpointHit(corename)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeCheckBreakpointHit(corename):
    c_ret = libcommon.skyeye_check_bp_hit(corename.encode("utf-8"))
    return c_ret
libcommon.skyeye_check_bp_hit.restype = SkyeyeAPIResult
libcommon.skyeye_check_bp_hit.argtypes = [c_char_p]

def SkyEyeCreateBreakpoint(corename, addr):
    # BUG: 相同bp地址， 未报错。
    c_ret = _SkyEyeCreateBreakpoint(corename, addr)
    return None

@exception_decorator
def _SkyEyeCreateBreakpoint(corename, addr):
    c_ret = libcommon.skyeye_create_breakpoint(corename.encode("utf-8"), addr)
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_create_breakpoint.restype = SkyeyeAPIResult
libcommon.skyeye_create_breakpoint.argtypes = [c_char_p, c_uint64]

def SkyEyeGetSocNum():
    c_ret = _SkyEyeGetSocNum()
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetSocNum():
    c_ret = libcommon.skyeye_get_soc_num()
    return c_ret
libcommon.skyeye_get_soc_num.restype = SkyeyeAPIResult

def SkyEyePreConfObj(objname, clsname):
    c_ret = _SkyEyePreConfObj(objname, clsname)
    return c_ret.conf_obj.contents

@exception_decorator
def _SkyEyePreConfObj(objname, clsname):
    c_ret = libcommon.skyeye_add_pre_obj(objname.encode("utf-8"), clsname.encode("utf-8"))
    return c_ret
libcommon.skyeye_add_pre_obj.restype = SkyeyeAPIResult
libcommon.skyeye_add_pre_obj.argtypes = [c_char_p, c_char_p]

def SkyEyeCreateMach(machname, machcls):
    c_ret = _SkyEyeCreateMach(machname, machcls)
    return None

@exception_decorator
def _SkyEyeCreateMach(machname, machcls):
    c_ret = libcommon.skyeye_create_mach(machname.encode("utf-8"), machcls.encode("utf-8"))
    return c_ret
libcommon.skyeye_create_mach.restype = SkyeyeAPIResult
libcommon.skyeye_create_mach.argtypes = [c_char_p, c_char_p]

def SkyEyeCreateLinker(linkername, linkercls):
    c_ret = _SkyEyeCreateLinker(linkername, linkercls)
    return None

@exception_decorator
def _SkyEyeCreateLinker(linkername, linkercls):
    c_ret = libcommon.skyeye_create_linker(linkername.encode("utf-8"), linkercls.encode("utf-8"))
    return c_ret
libcommon.skyeye_create_linker.restype = SkyeyeAPIResult
libcommon.skyeye_create_linker.argtypes = [c_char_p, c_char_p]

def SkyEyeCreateCpu(machname, cpuname, cpucls):
    c_ret = _SkyEyeCreateCpu(machname, cpuname, cpucls)
    return None

@exception_decorator
def _SkyEyeCreateCpu(machname, cpuname, cpucls):
    c_ret = libcommon.skyeye_create_cpu(machname.encode("utf-8"), cpuname.encode("utf-8"), cpucls.encode("utf-8"))
    return c_ret
libcommon.skyeye_create_cpu.restype = SkyeyeAPIResult
libcommon.skyeye_create_cpu.argtypes = [c_char_p, c_char_p, c_char_p]

def SkyEyeCreateDevice(machname, devicename, devcls):
    c_ret = _SkyEyeCreateDevice(machname, devicename, devcls)
    return None

@exception_decorator
def _SkyEyeCreateDevice(machname, devicename, devcls):
    c_ret = libcommon.skyeye_create_device(machname.encode("utf-8"), devicename.encode("utf-8"), devcls.encode("utf-8"))
    return c_ret
libcommon.skyeye_create_device.restype = SkyeyeAPIResult
libcommon.skyeye_create_device.argtypes = [c_char_p, c_char_p, c_char_p]

def SkyEyeConfigConfObj():
    c_ret = _SkyEyeConfigConfObj()
    return None

@exception_decorator
def _SkyEyeConfigConfObj():
    c_ret = libcommon.skyeye_config_obj()
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_config_obj.restype = SkyeyeAPIResult

def SkyEyeSetMinSynTime(s, delay):
    c_ret = _SkyEyeSetMinSynTime(s, delay)
    return None

@exception_decorator
def _SkyEyeSetMinSynTime(s, delay):
    c_ret = libcommon.skyeye_set_min_syn_time(s, delay)
    return c_ret
libcommon.skyeye_set_min_syn_time.restype = SkyeyeAPIResult
libcommon.skyeye_set_min_syn_time.argtypes = [c_double, c_double]

def SkyEyeSetAttr(objname, key, attr_type, value):
    c_ret = _SkyEyeSetAttr(objname, key, attr_type, value)
    return None

@exception_decorator
def _SkyEyeSetAttr(objname, key, attr_type, value):
    c_ret = libcommon.skyeye_set_attr(objname.encode("utf-8"), key.encode("utf-8"), attr_type.encode("utf-8"), value.encode("utf-8"))
    return c_ret
libcommon.skyeye_set_attr.restype = SkyeyeAPIResult
libcommon.skyeye_set_attr.argtypes = [c_char_p, c_char_p, c_char_p, c_char_p]

def SkyEyeConnect(connect_obj, iface_obj, name, index):
    c_ret = _SkyEyeConnect(connect_obj, iface_obj, name, index)
    return None

@exception_decorator
def _SkyEyeConnect(connect_obj, iface_obj, name, index):
    c_ret = libcommon.skyeye_connect_device(connect_obj.encode("utf-8"), iface_obj.encode("utf-8"), name.encode("utf-8"), index)
    return c_ret
libcommon.skyeye_connect_device.restype = SkyeyeAPIResult
libcommon.skyeye_connect_device.argtypes = [c_char_p, c_char_p, c_char_p, c_uint]

def SkyEyeMpAddMap(memory_space, device, addr, length):
    c_ret = _SkyEyeMpAddMap(memory_space, device, addr, length)
    return None

@exception_decorator
def _SkyEyeMpAddMap(memory_space, device, addr, length):
    c_ret = libcommon.skyeye_add_map(memory_space.encode("utf-8"), device.encode("utf-8"), addr, length)
    return c_ret
libcommon.skyeye_add_map.restype = SkyeyeAPIResult
libcommon.skyeye_add_map.argtypes = [c_char_p, c_char_p, c_uint64, c_uint]

def SkyEyeMpAddMapGroup(memory_space, device, addr, length, index):
    c_ret = _SkyEyeMpAddMapGroup(memory_space, device, addr, length, index)
    return None

@exception_decorator
def _SkyEyeMpAddMapGroup(memory_space, device, addr, length, index):
    c_ret = libcommon.skyeye_add_map_group(memory_space.encode("utf-8"), device.encode("utf-8"), addr, length, index)
    return c_ret
libcommon.skyeye_add_map_group.restype = SkyeyeAPIResult
libcommon.skyeye_add_map_group.argtypes = [c_char_p, c_char_p, c_uint64, c_uint, c_uint]

def SkyEyeDevGetRegNameById(corename, id):
    c_ret = _SkyEyeDevGetRegNameById(corename, id)
    return None

@exception_decorator
def _SkyEyeDevGetRegNameById(corename, id):
    c_ret = libcommon.skyeye_get_regname_by_id(corename.encode("utf-8"), id)
    return c_ret
libcommon.skyeye_get_regname_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_get_regname_by_id.argtypes = [c_char_p, c_int]

def SkyEyeDevGetRegValueById(corename, id):
    c_ret = _SkyEyeDevGetRegValueById(corename, id)
    return c_ret.retLongValue

@exception_decorator
def _SkyEyeDevGetRegValueById(corename, id):
    c_ret = libcommon.skyeye_get_regvalue_by_id(corename.encode("utf-8"), id)
    return c_ret
libcommon.skyeye_get_regvalue_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_get_regvalue_by_id.argtypes = [c_char_p, c_int]

def SkyEyeGetPcByCoreName(corename):
    c_ret = _SkyEyeGetPcByCoreName(corename)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetPcByCoreName(corename):
    c_ret = libcommon.skyeye_get_pc_by_cpuname(corename.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_pc_by_cpuname.restype = SkyeyeAPIResult
libcommon.skyeye_get_pc_by_cpuname.argtypes = [c_char_p]

def SkyEyeStepi(corename, step):
    c_ret = _SkyEyeStepi(corename, step)
    return None

@exception_decorator
def _SkyEyeStepi(corename, step):
    c_ret = libcommon.skyeye_step_run(corename.encode("utf-8"), step.encode("utf-8"))
    return c_ret
libcommon.skyeye_step_run.restype = SkyeyeAPIResult
libcommon.skyeye_step_run.argtypes = [c_char_p, c_char_p]

def SkyEyeGetModules():
    c_ret = _SkyEyeGetModules()
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetModules():
    c_ret = libcommon.skyeye_get_module_names()
    return c_ret
libcommon.skyeye_get_module_names.restype = SkyeyeAPIResult

def SkyEyeGetModuleValueByName(ModuleName, key):
    c_ret = _SkyEyeGetModuleValueByName(ModuleName, key)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetModuleValueByName(ModuleName, key):
    c_ret = libcommon.skyeye_module_get_value_by_name(ModuleName.encode("utf-8"), key.encode("utf-8"))
    return c_ret
libcommon.skyeye_module_get_value_by_name.restype = SkyeyeAPIResult
libcommon.skyeye_module_get_value_by_name.argtypes = [c_char_p, c_char_p]

def SkyEyeGetModulePathByName(ModuleName):
    c_ret = _SkyEyeGetModulePathByName(ModuleName)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetModulePathByName(ModuleName):
    c_ret = libcommon.skyeye_module_get_path_by_name(ModuleName.encode("utf-8"))
    return c_ret
libcommon.skyeye_module_get_path_by_name.restype = SkyeyeAPIResult
libcommon.skyeye_module_get_path_by_name.argtypes = [c_char_p]

def SkyEyeLoadModuleFromDir(DirName):
    c_ret = _SkyEyeLoadModuleFromDir(DirName)
    return None

@exception_decorator
def _SkyEyeLoadModuleFromDir(DirName):
    c_ret = libcommon.skyeye_load_module_fromdir(DirName.encode("utf-8"))
    return c_ret
libcommon.skyeye_load_module_fromdir.restype = SkyeyeAPIResult
libcommon.skyeye_load_module_fromdir.argtypes = [c_char_p]

def SkyEyeCreateRemoteGdb(Target, Port, Ip):
    c_ret = _SkyEyeCreateRemoteGdb(Target, Port, Ip)
    return None

@exception_decorator
def _SkyEyeCreateRemoteGdb(Target, Port, Ip):
    port_int = int(Port)
    c_ret = libcommon.skyeye_create_remote_gdb(Target.encode("utf-8"), port_int, Ip.encode("utf-8"))
    return c_ret
libcommon.skyeye_create_remote_gdb.restype = SkyeyeAPIResult
libcommon.skyeye_create_remote_gdb.argtypes = [c_char_p, c_uint, c_char_p]

def SkyEyeDeleteRemoteGdb(Target):
    c_ret = _SkyEyeDeleteRemoteGdb(Target)
    return None

@exception_decorator
def _SkyEyeDeleteRemoteGdb(Target):
    c_ret = libcommon.skyeye_delete_remote_gdb(Target.encode("utf-8"))
    return c_ret
libcommon.skyeye_delete_remote_gdb.restype = SkyeyeAPIResult
libcommon.skyeye_delete_remote_gdb.argtypes = [c_char_p]

def SkyEyeRemoteGdbCheckLink(Target):
    c_ret = _SkyEyeRemoteGdbCheckLink(Target)
    return None

@exception_decorator
def _SkyEyeRemoteGdbCheckLink(Target):
    c_ret = libcommon.skyeye_remote_gdb_check_link(Target.encode("utf-8"))
    return c_ret
libcommon.skyeye_remote_gdb_check_link.restype = SkyeyeAPIResult
libcommon.skyeye_remote_gdb_check_link.argtypes = [c_char_p]

def SkyEyeRemoteGdbGetClientIp(Target):
    c_ret =  _SkyEyeRemoteGdbGetClientIp(Target)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeRemoteGdbGetClientIp(Target):
    c_ret = libcommon.skyeye_remote_gdb_get_client_ip(Target.encode("utf-8"))
    return c_ret
libcommon.skyeye_remote_gdb_get_client_ip.restype = SkyeyeAPIResult
libcommon.skyeye_remote_gdb_get_client_ip.argtypes = [c_char_p]

def SkyEyeLoadFile(cpuname, filename, pc_addr):
    c_ret = _SkyEyeLoadFile(cpuname, filename, pc_addr)
    return None

@exception_decorator
def _SkyEyeLoadFile(cpuname, filename, pc_addr):
    c_ret = libcommon.skyeye_load_file(cpuname.encode("utf-8"), filename.encode("utf-8"), pc_addr)
    return c_ret
libcommon.skyeye_load_file.restype = SkyeyeAPIResult
libcommon.skyeye_load_file.argtypes = [c_char_p, c_char_p, c_uint64]

def SkyEyeGetDevRegNum(machname, devicename):
    c_ret = _SkyEyeGetDevRegNum(machname, devicename)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetDevRegNum(machname, devicename):
    c_ret = libcommon.skyeye_get_register_num(devicename.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_register_num.restype = SkyeyeAPIResult
libcommon.skyeye_get_register_num.argtypes = [c_char_p]

def SkyEyeGetDevRegNameById(machname, devicename, regid):
    # hasn't been tested
    c_ret = _SkyEyeGetDevRegNameById(machname, devicename, regid)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetDevRegNameById(machname, devicename, regid):
    c_ret = libcommon.skyeye_get_regname_by_id(devicename.encode("utf-8"), regid)
    return c_ret
libcommon.skyeye_get_regname_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_get_regname_by_id.argtypes = [c_char_p, c_uint]

def SkyEyeGetDevRegValueById(machname, devicename, regid):
    c_ret = _SkyEyeGetDevRegValueById(machname, devicename, regid)
    return c_ret.retLongValue

@exception_decorator
def _SkyEyeGetDevRegValueById(machname, devicename, regid):
    c_ret = libcommon.skyeye_get_regvalue_by_id(devicename.encode("utf-8"), regid)
    return c_ret
libcommon.skyeye_get_regvalue_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_get_regvalue_by_id.argtypes = [c_char_p, c_uint]

def SkyEyeGetDevRegOffsetById(machname, devicename, regid):
    c_ret = _SkyEyeGetDevRegOffsetById(machname, devicename, regid)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetDevRegOffsetById(machname, devicename, regid):
    c_ret = libcommon.skyeye_get_regoffset_by_id(devicename.encode("utf-8"), regid)
    return c_ret
libcommon.skyeye_get_regoffset_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_get_regoffset_by_id.argtypes = [c_char_p, c_uint]

def SkyEyeSetDevRegValueById(machname, devicename, value, regid):
    c_ret = _SkyEyeSetDevRegValueById(machname, devicename, value, regid)
    return None

@exception_decorator
def _SkyEyeSetDevRegValueById(machname, devicename, value, regid):
    c_ret = libcommon.skyeye_set_regvalue_by_id(devicename.encode("utf-8"), value, regid)
    return c_ret
libcommon.skyeye_set_regvalue_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_set_regvalue_by_id.argtypes = [c_char_p, c_uint64, c_uint]

def SkyEyeGetDevRegIdByName(machname, devicename, regname):
    c_ret = _SkyEyeGetDevRegIdByName(machname, devicename, regname)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetDevRegIdByName(machname, devicename, regname):
    c_ret = libcommon.skyeye_get_regid_by_name(devicename.encode("utf-8"), regname.encode("utf-8"))
    debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_get_regid_by_name.restype = SkyeyeAPIResult
libcommon.skyeye_get_regid_by_name.argtypes = [c_char_p, c_char_p]

def SkyEyeDisassemble(cpuname, addr):
    # BUG: 可用？
    c_ret = _SkyEyeDisassemble(cpuname, addr)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeDisassemble(cpuname, addr):
    c_ret = libcommon.skyeye_disassemble_by_addr(cpuname.encode("utf-8"), addr)
    return c_ret
libcommon.skyeye_disassemble_by_addr.restype = SkyeyeAPIResult
libcommon.skyeye_disassemble_by_addr.argtypes = [c_char_p, c_uint64]

def SkyEyeReadMemory8(cpuname, addr):
    # FIXME: 地址越界
    c_ret = _SkyEyeReadMemory8(cpuname, addr)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeReadMemory8(cpuname, addr):
    c_ret = libcommon.skyeye_memory_read(cpuname.encode("utf-8"), addr, 1)
    return c_ret
libcommon.skyeye_memory_read.restype = SkyeyeAPIResult
libcommon.skyeye_memory_read.argtypes = [c_char_p, c_uint64, c_uint]

def SkyEyeWriteMemory(cpuname,addr,value,length):
    c_ret =_SkyEyeWriteMemory(cpuname,addr,value,length)
    return None

@exception_decorator
def _SkyEyeWriteMemory(cpuname,addr,value,length):
    c_ret = libcommon.skyeye_memory_write(cpuname.encode("utf-8"), addr, value.encode('latin1'), length)
    return c_ret
libcommon.skyeye_memory_write.restype = SkyeyeAPIResult
libcommon.skyeye_memory_write.argtypes = [c_char_p, c_uint64, c_char_p, c_uint]

def SkyEyeGetAddressWidth(cpuname):
    c_ret = _SkyEyeGetAddressWidth(cpuname)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetAddressWidth(cpuname):
    c_ret = libcommon.skyeye_get_cpu_address_width(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_cpu_address_width.restype = SkyeyeAPIResult
libcommon.skyeye_get_cpu_address_width.argtypes = [c_char_p]

def SkyEyeWriteDevice4(device, offset, data):
    c_ret = _SkyEyeWriteDevice4(device, offset, data)
    return None

@exception_decorator
def _SkyEyeWriteDevice4(device, offset, data):
    c_ret = libcommon.skyeye_device_write(device.encode("utf-8"), offset, data.encode("utf-8"), 4)
    return c_ret
libcommon.skyeye_device_write.restype = SkyeyeAPIResult
libcommon.skyeye_device_write.argtypes = [c_char_p, c_uint, c_char_p, c_uint]

def SkyEyeWriteDevice(device, offset, data, size):
    c_ret = _SkyEyeWriteDevice(device, offset, data, size)
    return None

@exception_decorator
def _SkyEyeWriteDevice(device, offset, data, size):
    c_ret = libcommon.skyeye_device_write(device.encode("utf-8"), offset, data.encode("utf-8"), size)
    return c_ret
libcommon.skyeye_device_write.restype = SkyeyeAPIResult
libcommon.skyeye_device_write.argtypes = [c_char_p, c_uint, c_char_p, c_uint]

def SkyEyeReadDevice4(device, offset):
    c_ret = _SkyEyeReadDevice4(device, offset)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeReadDevice4(device, offset):
    c_ret = libcommon.skyeye_device_read(device.encode("utf-8"), offset, 4)
    return c_ret
libcommon.skyeye_device_read.restype = SkyeyeAPIResult
libcommon.skyeye_device_read.argtypes = [c_char_p, c_uint, c_uint]

def SkyEyeReadDataInjectDevice(device):
    c_ret = _SkyEyeReadDataInjectDevice(device)
    data = _SkyEyeReadDataInjectDevice.data
    return data[:c_ret.retIntValue]

@exception_decorator
def _SkyEyeReadDataInjectDevice(device):
    data = _SkyEyeReadDataInjectDevice.data = create_string_buffer('\0'*1000)
    c_ret = libcommon.skyeye_inject_device_read(device.encode("utf-8"), byref(data))
    return c_ret
libcommon.skyeye_inject_device_read.restype = SkyeyeAPIResult
libcommon.skyeye_inject_device_read.argtypes = [c_char_p]

def SkyEyeWriteDataInjectDevice(device, value, lens):
    c_ret =_SkyEyeWriteDataInjectDevice(device, value, lens)
    return None

@exception_decorator
def _SkyEyeWriteDataInjectDevice(device, value, lens=-1):
    if lens == -1:
        lens = len(value)
    c_ret = libcommon.skyeye_inject_device_write(device.encode("utf-8"), value.encode("utf-8"), lens)
    return c_ret
libcommon.skyeye_inject_device_write.restype = SkyeyeAPIResult
libcommon.skyeye_inject_device_write.argtypes = [c_char_p, c_char_p, c_int]

def SkyEyeGetClassList():
    c_ret = _SkyEyeGetClassList()
    return c_ret.list

@exception_decorator
def _SkyEyeGetClassList():
    c_ret = libcommon.skyeye_get_class_list()
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_get_class_list.restype = SkyeyeAPIResult

def SkyEyeGetClassType(name):
    c_ret = _SkyEyeGetClassType(name)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetClassType(name):
    c_ret = libcommon.skyeye_get_class_type(name.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_class_type.restype = SkyeyeAPIResult
libcommon.skyeye_get_class_type.argtypes = [c_char_p]

def SkyEyeGetClassIfaceList(cls):
    c_ret = _SkyEyeGetClassIfaceList(cls)
    return c_ret.list

@exception_decorator
def _SkyEyeGetClassIfaceList(cls):
    c_ret = libcommon.skyeye_get_interface_list(cls.encode("utf-8"))
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_get_interface_list.restype = SkyeyeAPIResult
libcommon.skyeye_get_interface_list.argtypes = [c_char_p]

def SkyEyeGetClassConnectList(cls):
    c_ret = _SkyEyeGetClassConnectList(cls)
    return c_ret.list

@exception_decorator
def _SkyEyeGetClassConnectList(cls):
    c_ret = libcommon.skyeye_get_connect_list(cls.encode("utf-8"))
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_get_connect_list.restype = SkyeyeAPIResult
libcommon.skyeye_get_connect_list.argtypes = [c_char_p]

def SkyEyeGetClassAttrList(cls):
    c_ret = _SkyEyeGetClassAttrList(cls)
    return c_ret.list

@exception_decorator
def _SkyEyeGetClassAttrList(cls):
    c_ret = libcommon.skyeye_get_attr_list(cls.encode("utf-8"))
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_get_attr_list.restype = SkyeyeAPIResult
libcommon.skyeye_get_attr_list.argtypes = [c_char_p]

def SkyEyeGetClassAttrInfo(cls, attr):
    c_ret = _SkyEyeGetClassAttrInfo(cls, attr)
    #debug_clib_result(c_ret)
    return c_ret.list

@exception_decorator
def _SkyEyeGetClassAttrInfo(cls, attr):
    c_ret = libcommon.skyeye_get_attr_info(cls.encode("utf-8"), attr.encode("utf-8"))
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_get_attr_info.restype = SkyeyeAPIResult
libcommon.skyeye_get_attr_info.argtypes = [c_char_p, c_char_p]

def SkyEyeSetFj(mp, addr, bit, mode, count):
    c_ret = _SkyEyeSetFj(mp, addr, bit, mode, count)
    return None

@exception_decorator
def _SkyEyeSetFj(mp, addr, bit, mode, count):
    c_ret = libcommon.skyeye_set_fault(mp.encode("utf-8"), addr, bit, mode, count)
    return c_ret
libcommon.skyeye_set_fault.restype = SkyeyeAPIResult
libcommon.skyeye_set_fault.argtypes = [c_char_p, c_uint64, c_uint, c_uint, c_uint]

def SkyEyeGetFj(mp, addr):
    c_ret = _SkyEyeGetFj(mp, addr)
    fj_l = []
    stru = c_ret.fault_inject.contents
    cnt = stru.count
    for i in range(0, cnt):
        dic = {}
        fi = cast(stru.fi[i*2 + 1], fi_type).contents
        dic["bit"] = fi.bit
        dic["addr"] = fi.addr
        dic["mode"] = fi.mode
        fj_l.append(dic)
    return fj_l

@exception_decorator
def _SkyEyeGetFj(mp, addr):
    c_ret = libcommon.skyeye_get_fault(mp.encode("utf-8"), addr)
    return c_ret
libcommon.skyeye_get_fault.restype = SkyeyeAPIResult
libcommon.skyeye_get_fault.argtypes = [c_char_p, c_uint64]

def SkyEyeClearFj(mp, addr, bit, mode):
    c_ret = _SkyEyeClearFj(mp, addr, bit, mode)
    return None 

@exception_decorator
def _SkyEyeClearFj(mp, addr, bit, mode):
    c_ret = libcommon.skyeye_clear_fault(mp.encode("utf-8"), addr, bit, mode)
    return c_ret
libcommon.skyeye_clear_fault.restype = SkyeyeAPIResult
libcommon.skyeye_clear_fault.argtypes = [c_char_p, c_uint64, c_uint, c_uint]

def SkyEyeLoadBinary(cpuname, filename):
    c_ret = _SkyEyeLoadBinary(cpuname, filename)
    return None

@exception_decorator
def _SkyEyeLoadBinary(cpuname, filename):
    c_ret = libcommon.skyeye_load_binary(cpuname.encode("utf-8"), filename.encode("utf-8"))
    return c_ret
libcommon.skyeye_load_binary.restype = SkyeyeAPIResult
libcommon.skyeye_load_binary.argtypes = [c_char_p, c_char_p]

def SkyEyeParseSymbol(cpuname, filename):
    c_ret = _SkyEyeParseSymbol(cpuname, filename)
    return None

@exception_decorator
def _SkyEyeParseSymbol(cpuname, filename):
    c_ret = libcommon.skyeye_parse_symbol(cpuname.encode("utf-8"), filename.encode("utf-8"))
    return c_ret
libcommon.skyeye_parse_symbol.restype = SkyeyeAPIResult
libcommon.skyeye_parse_symbol.argtypes = [c_char_p, c_char_p]

def SkyEyeGetCpuSteps(cpuname):
    c_ret = _SkyEyeGetCpuSteps(cpuname)
    return c_ret.retLongValue

@exception_decorator
def _SkyEyeGetCpuSteps(cpuname):
    c_ret = libcommon.skyeye_get_cpu_steps(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_cpu_steps.restype = SkyeyeAPIResult
libcommon.skyeye_get_cpu_steps.argtypes = [c_char_p]

def SkyEyeCpuSetMode(cpuname, mode):
    c_ret = _SkyEyeCpuSetMode(cpuname, mode)
    return None

@exception_decorator
def _SkyEyeCpuSetMode(cpuname, mode):
    c_ret = libcommon.skyeye_set_cpu_run_mode(cpuname.encode("utf-8"), mode)
    return c_ret
libcommon.skyeye_set_cpu_run_mode.restype = SkyeyeAPIResult
libcommon.skyeye_set_cpu_run_mode.argtypes = [c_char_p, c_uint]

def SkyEyeGetExecutedPcFile(instr_process, fname):
    c_ret = _SkyEyeGetExecutedPcFile(instr_process, fname)
    return None

@exception_decorator
def _SkyEyeGetExecutedPcFile(instr_process, fname):
    c_ret = libcommon.skyeye_get_executed_pc_file(instr_process.encode("utf-8"), fname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_executed_pc_file.restype = SkyeyeAPIResult
libcommon.skyeye_get_executed_pc_file.argtypes = [c_char_p, c_char_p]

def SkyEyeGetPcRecordSize(instr_process):
    c_ret = _SkyEyeGetPcRecordSize(instr_process)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetPcRecordSize(instr_process):
    c_ret = libcommon.skyeye_get_pc_record_size(instr_process.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_pc_record_size.restype = SkyeyeAPIResult
libcommon.skyeye_get_pc_record_size.argtypes = [c_char_p]

def SkyEyeSetPcRecordSize(instr_process, size):
    c_ret = _SkyEyeSetPcRecordSize(instr_process, size)
    return None

@exception_decorator
def _SkyEyeSetPcRecordSize(instr_process, size):
    c_ret = libcommon.skyeye_set_pc_record_size(instr_process.encode("utf-8"), size)
    return c_ret
libcommon.skyeye_set_pc_record_size.restype = SkyeyeAPIResult
libcommon.skyeye_set_pc_record_size.argtypes = [c_char_p, c_int]

def SkyEyeGetPcRecordNums(instr_process):
    c_ret = _SkyEyeGetPcRecordNums(instr_process)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetPcRecordNums(instr_process):
    c_ret = libcommon.skyeye_get_pc_record_nums(instr_process.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_pc_record_nums.restype = SkyeyeAPIResult
libcommon.skyeye_get_pc_record_nums.argtypes = [c_char_p]

def SkyEyeGetPcRecordIndex(instr_process):
    c_ret = _SkyEyeGetPcRecordIndex(instr_process)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetPcRecordIndex(instr_process):
    c_ret = libcommon.skyeye_get_pc_record_index(instr_process.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_pc_record_index.restype = SkyeyeAPIResult
libcommon.skyeye_get_pc_record_index.argtypes = [c_char_p]

def SkyEyeGetPcRecordOverflow(instr_process):
    c_ret = _SkyEyeGetPcRecordOverflow(instr_process)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetPcRecordOverflow(instr_process):
    c_ret = libcommon.skyeye_get_pc_record_overflow(instr_process.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_pc_record_overflow.restype = SkyeyeAPIResult
libcommon.skyeye_get_pc_record_overflow.argtypes = [c_char_p]

def SkyEyeGetPcByIndex(instr_process, n):
    c_ret = _SkyEyeGetPcByIndex(instr_process, n)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetPcByIndex(instr_process, n):
    c_ret = libcommon.skyeye_get_pc_by_index(instr_process.encode("utf-8"), n)
    return c_ret
libcommon.skyeye_get_pc_by_index.restype = SkyeyeAPIResult
libcommon.skyeye_get_pc_by_index.argtypes = [c_char_p, c_int]

def SkyEyeGetArchitecture(cpuname):
    c_ret = _SkyEyeGetArchitecture(cpuname)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetArchitecture(cpuname):
    c_ret = libcommon.skyeye_get_cpu_architecture(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_cpu_architecture.restype = SkyeyeAPIResult
libcommon.skyeye_get_cpu_architecture.argtypes = [c_char_p]

def SkyEyeListDir(path):
    c_ret = _SkyEyeListDir(path)
    return None

@exception_decorator
def _SkyEyeListDir(path):
    c_ret = libcommon.skyeye_list_dir(path.encode("utf-8"))
    return c_ret
libcommon.skyeye_list_dir.restype = SkyeyeAPIResult
libcommon.skyeye_list_dir.argtypes = [c_char_p]

def SkyEyeListModules():
    # 为什么返回retIntValue?
    c_ret = _SkyEyeListModules()
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeListModules():
    c_ret = libcommon.skyeye_list_modules()
    #debug_clib_result(c_ret)
    return c_ret
libcommon.skyeye_list_modules.restype = SkyeyeAPIResult

def SkyEyeMemoryInfo(args):
    c_ret = _SkyEyeMemoryInfo(args)
    return None

@exception_decorator
def _SkyEyeMemoryInfo(args):
    c_ret = libcommon.skyeye_get_mm_info(args.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_mm_info.restype = SkyeyeAPIResult
libcommon.skyeye_get_mm_info.argtypes = [c_char_p]

# def SkyEyeGetCpuId():
#     c_ret = _SkyEyeGetCpuId()
#     return c_ret.retStrValue
#
# @exception_decorator
# def _SkyEyeGetCpuId():
#     c_ret = libcommon.skyeye_get_cpuid()
#     return c_ret
# libcommon.skyeye_get_cpuid.restype = SkyeyeAPIResult

def SkyEyeGetSimulationRunTime(cpuname):
    c_ret = _SkyEyeGetSimulationRunTime(cpuname)
    return c_ret.retDoubleValue

@exception_decorator
def _SkyEyeGetSimulationRunTime(cpuname):
    c_ret = libcommon.skyeye_get_simulation_time(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_simulation_time.restype = SkyeyeAPIResult
libcommon.skyeye_get_simulation_time.argtypes = [c_char_p]

def SkyEyeGetSimulationInsnNum(cpuname):
    c_ret = _SkyEyeGetSimulationInsnNum(cpuname)
    return c_ret.retLongValue

@exception_decorator
def _SkyEyeGetSimulationInsnNum(cpuname):
    c_ret = libcommon.skyeye_get_simulation_insn_num(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_simulation_insn_num.restype = SkyeyeAPIResult
libcommon.skyeye_get_simulation_insn_num.argtypes = [c_char_p]

def SkyEyeGetDeviceModuleType(devicename):
    c_ret = _SkyEyeGetDeviceModuleType(devicename)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetDeviceModuleType(devicename):
    c_ret = libcommon.skyeye_get_device_module_type(devicename.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_device_module_type.restype = SkyeyeAPIResult
libcommon.skyeye_get_device_module_type.argtypes = [c_char_p]

def SkyEyeGetInstrProcessDevice(machname, cpuname):
    c_ret = _SkyEyeGetInstrProcessDevice(machname, cpuname)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetInstrProcessDevice(machname, cpuname):
    c_ret = libcommon.skyeye_get_instr_process_device(machname.encode("utf-8"), cpuname.encode("utf-8"), "instr_process".encode("utf-8"))
    return c_ret
libcommon.skyeye_get_instr_process_device.restype = SkyeyeAPIResult
libcommon.skyeye_get_instr_process_device.argtypes = [c_char_p, c_char_p, c_char_p]

def SkyEyeSetScriptPath(script_file_name):
    c_ret = _SkyEyeSetScriptPath(script_file_name)
    return None

@exception_decorator
def _SkyEyeSetScriptPath(script_file_name):
    c_ret = libcommon.skyeye_set_script_path(script_file_name.encode("utf-8"))
    return c_ret
libcommon.skyeye_set_script_path.restype = SkyeyeAPIResult
libcommon.skyeye_set_script_path.argtypes = [c_char_p]

def SkyEyeGetWorkFullPath():
    c_ret = _SkyEyeGetWorkFullPath()
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetWorkFullPath():
    c_ret = libcommon.skyeye_get_work_full_path()
    return c_ret
libcommon.skyeye_get_work_full_path.restype = SkyeyeAPIResult

def SkyEyeGetWorkPath():
    c_ret = _SkyEyeGetWorkPath()
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetWorkPath():
    c_ret = libcommon.skyeye_get_work_path()
    return c_ret
libcommon.skyeye_get_work_path.restype = SkyeyeAPIResult

def SkyEyeGetSymbolAddr(cpuname,varname):
    c_ret = _SkyEyeGetSymbolAddr(cpuname,varname)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetSymbolAddr(cpuname,varname):
    c_ret = libcommon.skyeye_get_symbol_addr(cpuname.encode("utf-8"),varname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_symbol_addr.restype = SkyeyeAPIResult
libcommon.skyeye_get_symbol_addr.argtypes = [c_char_p, c_char_p]

def SkyEyeGetSymbolValue(cpuname,varname,value_bytes_number):
    c_ret = _SkyEyeGetSymbolValue(cpuname,varname,value_bytes_number)
    return c_ret.retLongValue

@exception_decorator
def _SkyEyeGetSymbolValue(cpuname,varname,value_bytes_number):
    c_ret = libcommon.skyeye_get_symbol_value(cpuname.encode("utf-8"), varname.encode("utf-8"), value_bytes_number)
    return c_ret
libcommon.skyeye_get_symbol_value.restype = SkyeyeAPIResult
libcommon.skyeye_get_symbol_value.argtypes = [c_char_p, c_char_p, c_int]

def SkyEyeGetFloatSymbolValue(cpuname,varname,value_bytes_number):
    c_ret =  _SkyEyeGetFloatSymbolValue(cpuname,varname,value_bytes_number)
    return c_ret.retFloatValue

@exception_decorator
def _SkyEyeGetFloatSymbolValue(cpuname,varname,value_bytes_number):
    c_ret = libcommon.skyeye_get_float_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"), value_bytes_number)
    return c_ret
libcommon.skyeye_get_float_symbol_value.restype = SkyeyeAPIResult
libcommon.skyeye_get_float_symbol_value.argtypes = [c_char_p, c_char_p, c_int]
    
def SkyEyeGetDoubleSymbolValue(cpuname,varname,value_bytes_number):
    c_ret = _SkyEyeGetDoubleSymbolValue(cpuname,varname,value_bytes_number)
    return c_ret.retDoubleValue

@exception_decorator
def _SkyEyeGetDoubleSymbolValue(cpuname,varname,value_bytes_number):
    c_ret = libcommon.skyeye_get_double_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"), value_bytes_number)
    return c_ret
libcommon.skyeye_get_double_symbol_value.restype = SkyeyeAPIResult
libcommon.skyeye_get_double_symbol_value.argtypes = [c_char_p, c_char_p, c_int]

def SkyEyeSetSymbolValue(cpuname,varname,value,value_bytes_number):
    c_ret = _SkyEyeSetSymbolValue(cpuname,varname,value,value_bytes_number)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeSetSymbolValue(cpuname,varname,value,value_bytes_number):
    c_ret = libcommon.skyeye_set_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"), value,value_bytes_number)
    return c_ret
libcommon.skyeye_set_symbol_value.restype = SkyeyeAPIResult
libcommon.skyeye_set_symbol_value.argtypes = [c_char_p, c_char_p, c_uint64, c_int]

def SkyEyeSetFloatSymbolValue(cpuname,varname,value,value_bytes_number):
    c_ret = _SkyEyeSetFloatSymbolValue(cpuname,varname,value,value_bytes_number)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeSetFloatSymbolValue(cpuname,varname,value,value_bytes_number):
    c_ret = libcommon.skyeye_set_float_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"),value,value_bytes_number)
    return c_ret
libcommon.skyeye_set_float_symbol_value.restype = SkyeyeAPIResult
libcommon.skyeye_set_float_symbol_value.argtypes = [c_char_p, c_char_p, c_float, c_int]
    
def SkyEyeSetDoubleSymbolValue(cpuname,varname,value,value_bytes_number):
    c_ret = _SkyEyeSetDoubleSymbolValue(cpuname,varname,value,value_bytes_number)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeSetDoubleSymbolValue(cpuname,varname,value,value_bytes_number):
    c_ret = libcommon.skyeye_set_double_symbol_value(cpuname.encode("utf-8"),varname.encode("utf-8"),value,value_bytes_number)
    return c_ret
libcommon.skyeye_set_double_symbol_value.restype = SkyeyeAPIResult
libcommon.skyeye_set_double_symbol_value.argtypes = [c_char_p, c_char_p, c_double, c_int]

def SkyEyeLoadBinBinary(cpuname, filename, addr, length, start_pc):
    c_ret = _SkyEyeLoadBinBinary(cpuname, filename, addr, length, start_pc)
    return None

@exception_decorator
def _SkyEyeLoadBinBinary(cpuname, filename, addr, length, start_pc):
    c_ret = libcommon.skyeye_load_bin_binary(cpuname.encode("utf-8"), filename.encode("utf-8"), addr, length, start_pc)
    return c_ret
libcommon.skyeye_load_bin_binary.restype = SkyeyeAPIResult
libcommon.skyeye_load_bin_binary.argtypes = [c_char_p, c_char_p, c_uint64, c_uint, c_uint]

def SkyEyeGetCpuFreq(cpuname):
    c_ret = _SkyEyeGetCpuFreq(cpuname)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetCpuFreq(cpuname):
    c_ret = libcommon.skyeye_get_cpu_freq(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_cpu_freq.restype = SkyeyeAPIResult
libcommon.skyeye_get_cpu_freq.argtypes = [c_char_p]

def SkyEyeReadByte(cpuname,addr):
    c_ret = _SkyEyeReadByte(cpuname,addr)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeReadByte(cpuname,addr):
    c_ret = libcommon.skyeye_memory_read(cpuname.encode("utf-8"), addr, 1)
    return c_ret
libcommon.skyeye_memory_read.restype = SkyeyeAPIResult
libcommon.skyeye_memory_read.argtypes = [c_char_p, c_uint64, c_uint]

def SkyEyeReadDWord(cpuname,addr):
    c_ret = _SkyEyeReadDWord(cpuname,addr)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeReadDWord(cpuname,addr):
    c_ret = libcommon.skyeye_memory_read(cpuname.encode("utf-8"),addr, 4)
    return c_ret
libcommon.skyeye_memory_read.restype = SkyeyeAPIResult
libcommon.skyeye_memory_read.argtypes = [c_char_p, c_uint64, c_uint]

def SkyEyeWriteByte(cpuname,addr,value):
    c_ret = _SkyEyeWriteByte(cpuname,addr,value)
    return None

@exception_decorator
def _SkyEyeWriteByte(cpuname,addr,value):
    c_ret = libcommon.skyeye_memory_write(cpuname.encode("utf-8"),addr,cast(pointer(c_int(value)),c_char_p), 1)
    return c_ret
libcommon.skyeye_memory_write.restype = SkyeyeAPIResult
libcommon.skyeye_memory_write.argtypes = [c_char_p, c_uint64, c_char_p, c_uint]

def SkyEyeWriteDWord(cpuname,addr,value):
    c_ret = _SkyEyeWriteDWord(cpuname,addr,value)
    return None

@exception_decorator
def _SkyEyeWriteDWord(cpuname,addr,value):
    c_ret = libcommon.skyeye_memory_write(cpuname.encode("utf-8"),addr,cast(pointer(c_int(value)),c_char_p), 4)
    return c_ret
libcommon.skyeye_memory_write.restype = SkyeyeAPIResult
libcommon.skyeye_memory_write.argtypes = [c_char_p, c_uint64, c_char_p, c_uint]

def SkyEyeGetFuncAddr(cpuname,funcname):
    c_ret = _SkyEyeGetFuncAddr(cpuname,funcname)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetFuncAddr(cpuname,funcname):
    c_ret = libcommon.skyeye_get_func_addr(cpuname.encode("utf-8"),funcname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_func_addr.restype = SkyeyeAPIResult
libcommon.skyeye_get_func_addr.argtypes = [c_char_p, c_char_p]

def SkyEyeGetFuncLength(cpuname,funcname):
    c_ret = _SkyEyeGetFuncLength(cpuname,funcname)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetFuncLength(cpuname,funcname):
    c_ret = libcommon.skyeye_get_func_length(cpuname.encode("utf-8"),funcname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_func_length.restype = SkyeyeAPIResult
libcommon.skyeye_get_func_length.argtypes = [c_char_p, c_char_p]

def SkyEyeEnableCpuCodecov(machname):
    c_ret = _SkyEyeEnableCpuCodecov(machname)
    return None

@exception_decorator
def _SkyEyeEnableCpuCodecov(machname):
    c_ret = libcommon.skyeye_enable_cpu_codecov(machname.encode("utf-8"))
    return c_ret
libcommon.skyeye_enable_cpu_codecov.restype = SkyeyeAPIResult
libcommon.skyeye_enable_cpu_codecov.argtypes = [c_char_p]

def SkyEyeGetCpuExceptionNum(cpuname):
    c_ret = _SkyEyeGetCpuExceptionNum(cpuname)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetCpuExceptionNum(cpuname):
    c_ret = libcommon.skyeye_get_cpu_exception_num(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_cpu_exception_num.restype = SkyeyeAPIResult
libcommon.skyeye_get_cpu_exception_num.argtypes = [c_char_p]

def SkyEyeSetCpuException(cpuname, exception_id):
    c_ret = _SkyEyeSetCpuException(cpuname, exception_id)
    return None

@exception_decorator
def _SkyEyeSetCpuException(cpuname, exception_id):
    c_ret = libcommon.skyeye_set_cpu_exception(cpuname.encode("utf-8"), exception_id)
    return c_ret
libcommon.skyeye_set_cpu_exception.restype = SkyeyeAPIResult
libcommon.skyeye_set_cpu_exception.argtypes = [c_char_p, c_int]

def SkyEyeSetExtInterrupt(intc_name, interrupt_num):
    c_ret = _SkyEyeSetExtInterrupt(intc_name, interrupt_num)
    return None

@exception_decorator
def _SkyEyeSetExtInterrupt(intc_name, interrupt_num):
    c_ret = libcommon.skyeye_set_ext_interrupt(intc_name.encode("utf-8"),interrupt_num)
    return c_ret
libcommon.skyeye_set_ext_interrupt.restype = SkyeyeAPIResult
libcommon.skyeye_set_ext_interrupt.argtypes = [c_char_p, c_int]

def SkyEyeDisableDeviceWork(device):
    c_ret = _SkyEyeDisableDeviceWork(device)
    return c_ret

@exception_decorator
def _SkyEyeDisableDeviceWork(device):
    c_ret = libcommon.skyeye_disable_device_work(device.encode("utf-8"))
    return c_ret
libcommon.skyeye_disable_device_work.restype = SkyeyeAPIResult
libcommon.skyeye_disable_device_work.argtypes = [c_char_p]

def SkyEyeEnableDeviceWork(device):
    c_ret = _SkyEyeEnableDeviceWork(device)
    return None

@exception_decorator
def _SkyEyeEnableDeviceWork(device):
    c_ret = libcommon.skyeye_enable_device_work(device.encode("utf-8"))
    return c_ret
libcommon.skyeye_enable_device_work.restype = SkyeyeAPIResult
libcommon.skyeye_enable_device_work.argtypes = [c_char_p]

def SkyEyeGetCpuExceptionNameById(cpuname, exception_id):
    c_ret = _SkyEyeGetCpuExceptionNameById(cpuname, exception_id)
    return c_ret.retStrValue

@exception_decorator
def _SkyEyeGetCpuExceptionNameById(cpuname, exception_id):
    c_ret = libcommon.skyeye_get_cpu_exception_name_by_id(cpuname.encode("utf-8"),exception_id)
    return c_ret
libcommon.skyeye_get_cpu_exception_name_by_id.restype = SkyeyeAPIResult
libcommon.skyeye_get_cpu_exception_name_by_id.argtypes = [c_char_p, c_int]

def SkyEyeSetPC(cpuname, pc):
    c_ret = _SkyEyeSetPC(cpuname, pc)
    return None

@exception_decorator
def _SkyEyeSetPC(cpuname, pc):
    c_ret = libcommon.skyeye_set_pc(cpuname.encode("utf-8"),pc)
    return c_ret
libcommon.skyeye_set_pc.restype = SkyeyeAPIResult
libcommon.skyeye_set_pc.argtypes = [c_char_p, c_int]

def SkyEyeGetPC(cpuname):
    c_ret = _SkyEyeGetPC(cpuname)
    return c_ret.retIntValue

@exception_decorator
def _SkyEyeGetPC(cpuname):
    c_ret = libcommon.skyeye_get_pc(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_get_pc.restype = SkyeyeAPIResult
libcommon.skyeye_get_pc.argtypes = [c_char_p]

def SkyEyeCloseInstrRecord(cpuname):
    c_ret = _SkyEyeCloseInstrRecord(cpuname)
    return None

@exception_decorator
def _SkyEyeCloseInstrRecord(cpuname):
    c_ret = libcommon.skyeye_close_instr_record(cpuname.encode("utf-8"))
    return c_ret
libcommon.skyeye_close_instr_record.restype = SkyeyeAPIResult
libcommon.skyeye_close_instr_record.argtypes = [c_char_p]

def SkyEyeOpenInstrRecord(cpuname, filename):
    c_ret = _SkyEyeOpenInstrRecord(cpuname, filename)
    return None

@exception_decorator
def _SkyEyeOpenInstrRecord(cpuname, filename):
    c_ret = libcommon.skyeye_open_instr_record(cpuname.encode("utf-8"),filename.encode("utf-8"))
    return c_ret
libcommon.skyeye_open_instr_record.restype = SkyeyeAPIResult
libcommon.skyeye_open_instr_record.argtypes = [c_char_p, c_char_p]

def SkyEyeEnableDebug(socname):
    c_ret = _SkyEyeEnableDebug(socname)
    return None

@exception_decorator
def _SkyEyeEnableDebug(socname):
    c_ret = libcommon.skyeye_enable_debug(socname.encode("utf-8"))
    return c_ret
libcommon.skyeye_enable_debug.restype = SkyeyeAPIResult
libcommon.skyeye_enable_debug.argtypes = [c_char_p]

def SkyEyeNewLoadFile(memoryname, filename, pc_addr):
    c_ret = _SkyEyeNewLoadFile(memoryname, filename, pc_addr)
    return None

@exception_decorator
def _SkyEyeNewLoadFile(memoryname, filename, pc_addr):
    c_ret = libcommon.skyeye_new_load_file(memoryname.encode("utf-8"), filename.encode("utf-8"), pc_addr)
    return c_ret
libcommon.skyeye_new_load_file.restype = SkyeyeAPIResult
libcommon.skyeye_new_load_file.argtypes = [c_char_p, c_char_p, c_uint64]

def SkyEyeTermWaitForString(termname, string):
    c_ret = _SkyEyeTermWaitForString(termname, string)
    return None

@exception_decorator
def _SkyEyeTermWaitForString(termname, string):
    c_ret = libcommon.skyeye_term_wait_for_string(termname.encode("utf-8"), string.encode("utf-8"))
    return c_ret
libcommon.skyeye_term_wait_for_string.restype = SkyeyeAPIResult
libcommon.skyeye_term_wait_for_string.argtypes = [c_char_p, c_char_p]

def SkyEyeTermWrite(termname, string):
    c_ret = _SkyEyeTermWrite(termname, string)
    return None

@exception_decorator
def _SkyEyeTermWrite(termname, string):
    c_ret = libcommon.skyeye_term_write(termname.encode("utf-8"), string.encode("utf-8"))
    return c_ret
libcommon.skyeye_term_write.restype = SkyeyeAPIResult
libcommon.skyeye_term_write.argtypes = [c_char_p, c_char_p]

def SkyEyeTermWaitThenWrite(termname, wait_string, write_string):
    c_ret = _SkyEyeTermWaitThenWrite(termname, wait_string, write_string)
    return None

@exception_decorator
def _SkyEyeTermWaitThenWrite(termname, wait_string, write_string):
    c_ret = libcommon.skyeye_term_wait_then_write(termname.encode("utf-8"), wait_string.encode("utf-8"), write_string.encode("utf-8"))
    return c_ret
libcommon.skyeye_term_wait_then_write.restype = SkyeyeAPIResult
libcommon.skyeye_term_wait_then_write.argtypes = [c_char_p, c_char_p, c_char_p]


###################################################################
#------------  FIXME: APIs for X86       -------------------------#
###################################################################
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
def SkyEyeSetTimeNum(num):
    libcommon.set_time_num.argtypes = [c_uint]
    libcommon.set_time_num(num)
    return [True, True, None]

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
    libcommon.skyeye_get_next_logMsg.restype = SkyeyeAPIResult
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
def SkyEyeSaveImage(device):
    libcommon.com_save_image.argtypes = [c_char_p]
    ret = libcommon.com_save_image(device.encode("utf-8"))
    return [True, ret, None]

###################################################################
#--------------------   DEBUG   ----------------------------------#
###################################################################
DEBUG_MODE = True
def debug_clib_result(res):
    if DEBUG_MODE:
        print('DEBUG CLIB RES:')
        print("\tresult", res.result)
        print("\tretStrValue", res.retStrValue)
        print("\tretIntValue", res.retIntValue)
        print("\tretFloatValue", res.retFloatValue)
        print("\tretDoubleValue", res.retDoubleValue)
        print("\tretLongValue", res.retLongValue)
        print("\tconf_obj", res.conf_obj)
        print("\tlist", res.list)
        print("\terrMsg", res.errMsg)
