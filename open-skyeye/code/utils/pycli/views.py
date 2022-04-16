#coding:utf-8
from net_ack import response
import conf
import os
import json
from skyeye_common_module import *
import cli
from conf import *
import _thread
import time
import coverage
import fault_inject as fi
import win_workspace as ws
import disassembler_interface_func as dif
import regs
import mips
import tools
import net_api_func as naf
import skyeye_license as sky_lic

def SE_get_workspace(request):
	error = None
	result = 'false'
	ws_path = ws.get_workspace_dir()
	if ws_path == None:
		error = "Fail to get"
	else:
		result = {'dir':'%s' % ws_path}
	return response(request, result, error)

def SE_set_workspace(request):
	error = None
	result = 'false'
	ws_path = request["args"]["path"]
	ws_path = os.path.abspath(ws_path)
	try:
		ws_ret = ws.create_workspace_API(ws_path)
		if ws_ret == True:
			result = "true"
	except:
		error="Error setting workspace"
	return response(request, result, error)

def run_script(request):
	result = 'true'
	error = None
	sc_file = request["args"]["filename"]
	cmd = cli.GetCommand("run-script")
	SetGlobalScript(sc_file)
	if cmd != None:
		try:
			ret = cmd.run([sc_file])
		except Exception as e:
			error = errormessage.GetErrorInfo()[0]
			ret = False
		if ret == False:
			if error == None:
				error = errormessage.GetErrorInfo()[0]
			result = 'false'
	else:
		error = "Can not find command: run-script"
		result = 'false'
	return response(request, result, error)

def sim_chdir(request):
	result = 'true'
	error = None
	dirpath = request["args"]["path"]
	if(os.path.exists(dirpath)):
		os.chdir(dirpath)
	else:
		error = "The path does not exist"
		result = 'false'
	return response(request, result, error)

def sim_getcwd(request):
	dirpath = os.getcwd()
	return response(request, dirpath, None)

def soc_list(request):
	result = []
	error = None
	config  = conf.GetGlobalConfig()
	if(config):
		mach_list = config.get_mach_list()
		for mach in mach_list:
			node = {}
			clsname = config.get_classname(mach)
			node["objname"] = mach
			node["classname"] = clsname
			result.append(node)
	else:
		error="The config is None"
	return response(request, result, error)

def get_coverage_file(request):
	result = 'false'
	error = None
	cpu = request["args"]["cpu"]
	cover_file = coverage.write_coverage_file(cpu)
	if cover_file == None:
		error = "Failed to generate the coverage file"
	else:
		result = {"file":"%s" % cover_file}
	return response(request, result, error)

def get_objdump_file(request):
	result = 'false'
	error = None
	cpu = request["args"]["cpu"]
	source_dir = request["args"]["source_dir"]
	objdump_file = coverage.write_objdump_file(cpu, source_dir)
	if objdump_file ==None:
		error = "Failed to generate the objdump file"
	else:
		result = {"file":"%s" % objdump_file}
	return response(request, result, error)

def get_class_list(request):
	result = 'false'
	error = None
	class_list = []
	try:
		result = SkyEyeGetClassList()
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def get_class_type(request):
	result = 'false'
	error = None
	classname = request["args"]["class"]
	try:
		result = SkyEyeGetClassType(classname)
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def get_class_attrlist(request):
	result = 'false'
	error = None
	classname = request["args"]["class"]
	try:
		attr_l = SkyEyeGetClassAttrList(classname)
		attr_l.sort()
		result = attr_l
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def get_interface_list(request):
	result = 'false'
	error = None
	classname = request["args"]["class"]
	try:
		iface_l = SkyEyeGetClassIfaceList(classname)
		iface_l.sort()
		result = iface_l
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def get_connect_list(request):
	result = 'false'
	error = None
	classname = request["args"]["class"]
	try:
		connect_l = SkyEyeGetClassConnectList(classname)
		connect_l.sort()
		result = connect_l
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def run(request):
	result = 'true'
	error = None
	try:
		SkyEyeRun()
	except Exception as e:
		error = str(e)
		result = 'false'
	return response(request, result, error)

def reset(request):
	result = 'true'
	error = None
	try:
		SkyEyeReset()
	except Exception as e:
		error = str(e)
		result = 'false'
	return response(request, result, error)

def stop(request):
	result = 'true'
	error = None
	try:
		SkyEyeStop()
	except Exception as e:
		error = str(e)
		result = 'false'
	return response(request, result, error)

def quit_thread(delay):
	time.sleep(delay)
	try:
		SkyEyeQuit()
	except Exception as e:
		print (e)

def quit(request):
	result = 'true'
	error = None
	if mips.mips_thread != None:
		mips.mips_thread.stop()
	coverage.kill_objdump_thread()
	_thread.start_new_thread(quit_thread, (0.2, ))
	return response(request, result, error)

def get_pin_num(request):
	result = 'false'
	error = None
	deviceName = request["args"]["deviceName"]
	try:
		pinNum = SkyEyeGetPinNum(deviceName)
		result = str(pinNum)
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def get_pin_type(request):
	result = 'false'
	error = None
	deviceName = request["args"]["deviceName"]
	index = request["args"]["index"]
	try:
		pinType = SkyEyeGetPinType(deviceName, index)
		result = str(pinType)
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def read_pin_state(request):
	result = 'false'
	error = None
	deviceName = request["args"]["deviceName"]
	index = request["args"]["index"]
	try:
		pinState = SkyEyeReadPinState(deviceName, index)
		result = str(pinState)
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def write_pin_state(request):
	result='true'
	error=None
	deviceName = request["args"]["deviceName"]
	index = request["args"]["index"]
	pinState = request["args"]["state"]
	try:
		SkyEyeWritePinState(deviceName, index, pinState)
	except Exception as e:
		error = str(e)
	return response(request,result,error)

def get_class_attrinfo(request):
	result='false'
	error=None
	classname = request["args"]["class"]
	attr = request["args"]["attr"]
	try:
		info_l = SkyEyeGetClassAttrInfo(classname,attr)
		if len(info_l) != 3:
			destription = 'NULL'
		else:
			destription = info_l[2]
		if len(info_l) < 2:
			error="Info length is less than 2"
		ack_list = {}
		ack_list["attr"] = attr
		ack_list["type"] =  info_l[1]
		ack_list["des"] =  destription
		if error == None:
			result=ack_list
	except Exception as e:
		error = str(e)
	return response(request,result,error)

def get_cpu_list(request):
	result='false'
	error=None
	config  = conf.GetGlobalConfig()
	ack_list = []
	if(config):
		cpu_list = config.get_cpu_list()
		for cpu in cpu_list:
			cpu_dic = {}
			cpu_cls = config.get_classname(cpu)
			mach = config.get_cpu_mach(cpu)
			cpu_dic["objname"] = cpu
			cpu_dic["classname"] = cpu_cls
			cpu_dic["mach"] = mach
			ack_list.append(cpu_dic)
		result=ack_list
	else:
		error="Config equal to None"
	return response(request,result,error)

def SE_coverage_init(request):
	result = "false"
	error = None
	cpu = request["args"]["cpu"]
	source_dir = request["args"]["source_dir"]
	if SkyEyeRunningStatus() == False:
		try:
			result = coverage.cov_init(cpu, source_dir)
		except Exception as e:
			error = str(e)
	else:
		error = "SkyEye running"
	return response(request, result, error)

def SE_get_running_number(request):
	result=None
	error=None
	if coverage.check_variable() == True:
		result=coverage.code_running_number()
	else:
		error="No init"
	return response(request, result, error)

def SE_get_running_path(request):
	result=None
	error=None
	if coverage.check_variable() == True:
		result=coverage.code_running_path()
	else:
		error="No init"
	return response(request, result, error)

def SE_get_total_cov(request):
	result=None
	error=None
	if coverage.check_variable() == True:
		result=coverage.count_total_cov()
	else:
		error="No init"
	return response(request, result, error)

def SE_get_all_func_cov(request):
	result=None
	error=None
	if coverage.check_variable() == True:
		result=coverage.count_all_func_cov()
	else:
		error="No init"
	return response(request, result, error)

def SE_get_single_func_cov(request):
	result=None
	error=None
	if coverage.check_variable() == True:
		result=coverage.count_single_func_cov()
	else:
		error="No init"
	return response(request, result, error)

def SE_get_project_cov_info(request):
	result=None
	error=None
	if coverage.check_variable() == True:
		result=coverage.get_project_info()
	else:
		error="No init"
	return response(request, result, error)

def str_to_int(strs):
	if strs[:2]=="0x":
		return int(strs,16)
	else:
		return int(strs)

def SE_set_fault_inject(request):
	result="false"
	error=None
	machname = request["args"]["machname"]
	devname = request["args"]["devname"]
	addr = str_to_int(request["args"]["addr"])
	bit = str_to_int(request["args"]["bit"])
	mode = str_to_int(request["args"]["mode"])
	ret=fi.skyeye_set_fj(machname,devname,addr,bit,mode)
	if ret==True:
		result="true"
	else:
		error=ret
	return response(request, result,error)

def SE_get_fault_inject_list(request):
	result=None
	error=None
	ret=fi.skyeye_get_fj()
	if ret!=False:
		result=ret
	return response(request, result,error)

def SE_delete_fault(request):
	result="false"
	error=None
	machname = request["args"]["machname"]
	devname = request["args"]["devname"]
	addr = str_to_int(request["args"]["addr"])
	bit = str_to_int(request["args"]["bit"])
	mode = str_to_int(request["args"]["mode"])
	ret=fi.skyeye_clear_fj(machname,devname,addr,bit,mode)
	if ret==True:
		result='true'
	return response(request, result,error)

def SE_get_fault_inject_device_info(request):
	result='false'
	error=None
	ret=fi.get_fault_inject_device_info()
	if ret!=None:
		result=ret
	return response(request, result,error)

def SE_get_device_register_info(request):
	result='false'
	error=None
	machname = request["args"]["machname"]
	devname = request["args"]["devname"]
	ret=fi.get_device_register_info(machname,devname)
	if ret == False:
		error="该设备获取不到寄存器相关信息，请查看该设备寄存器接口实现！"
	result=ret
	return response(request, result,error)

def SE_get_all_class_info(request):
	result='false'
	error=None
	ret=naf.get_all_class_info()
	if ret[0]==True:
		result=ret[1]
	else:
		error=ret[1]
	return response(request, result,error)

def SE_create_remote_gdb(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	ip = request["args"]["ip"]
	port = request["args"]["port"]
	try:
		ret=SkyEyeCreateRemoteGdb(cpuname, port, ip)
		if ret==True:
			result="true"
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_mem_device_addr_info(request):
	result='false'
	error=None
	machname = request["args"]["machname"]
	addr = str_to_int(request["args"]["addr"])
	info=fi.get_mem_device_addr_info(machname,addr)
	if info!=False:
		result=info
	return response(request, result,error)

def SE_get_all_fault_inject_device_info(request):
	result='false'
	error=None
	ret=fi.get_all_fault_inject_device_info()
	if ret != False:
		result=ret
	return response(request, result,error)

def SE_get_running_status(request):
	result=1
	error=None
	config  = conf.GetGlobalConfig()
	if(config):
		result=2
		if SkyEyeRunningStatus()==False:
			result=3
	return response(request, result,error)

def SE_get_next_log(request):
	result='false'
	error=None
	try:
		ret=SkyEyeGetNextLog()
		if ret != None:
			ret=ret
		result=ret
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_vm_list(request):
	result='false'
	error=None
	try:
		limit = request["args"]["limit"]
	except:
		limit = "256"
	try:
		result=SkyEyeGetVms(str_to_int(limit))
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_var_list(request):
	result='false'
	error=None
	Id = request["args"]["id"]
	try:
		limit = request["args"]["limit"]
	except:
		limit = "1000"
	try:
		result=SkyEyeGetVars(str_to_int(Id),str_to_int(limit))
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_set_var_conditions(request):
	result='false'
	error=None
	Id = request["args"]["id"]
	condition = request["args"]["condition"]
	try:
		result=SkyEyeSetVarConditions(str_to_int(Id),condition)
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_clear_var_conditions(request):
	result='false'
	error=None
	Id = request["args"]["id"]
	try:
		result=SkyEyeClearVarConditions(str_to_int(Id))
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_simulation_device_info(request):
	result='false'
	error=None
	device_info={}
	config  = conf.GetGlobalConfig()
	if config!=None:
		mach_list=config.get_mach_list()
		for machname in mach_list:
			device_info[machname]={}
			device_list = config.get_device_list_by_mach(machname)
			if device_list != None:
				for device in device_list:
					cls = config.get_device_classname(machname, device)
					attr_list=config.get_device_attr_list(machname, device)
					base=SkyEyeGetClassType(cls)
					device_info[machname][device]={"class_name":cls,"attr_list":attr_list,"base":base}
		result=device_info
	else:
		error="The config is None"
	return response(request, result,error)

def SE_set_device_register_value(request):
	result='false'
	error=None
	machname = request["args"]["machname"]
	devname = request["args"]["devname"]
	regname = request["args"]["regname"]
	value = request["args"]["value"]
	try:
		regid=SkyEyeGetDevRegIdByName(machname, devname, regname)
		SkyEyeSetDevRegValueById(machname, devname, str_to_int(value), regid)
		result="true"
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_memory_value(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	base_addr = str_to_int(request["args"]["baseaddr"])
	length = str_to_int(request["args"]["length"])
	try:
		temp=[]
		for i in range(0,length):
			temp.append(SkyEyeReadMemory8(cpuname, base_addr+i))
		result=temp
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_save_checkpoint(request):
	result='false'
	error=None
	checkpoint = request["args"]["checkpoint"]
	cmd = cli.GetCommand("write-configure")
	if cmd != None:
		try:
			cmd.run([checkpoint])
			result = 'true'
		except Exception as e:
			error = str(e)
	else:
		error = "SE_save_checkpoint:Can not find command: write-configure"
	return response(request, result,error)

def SE_open_checkpoint(request):
	result='false'
	error=None
	checkpoint = request["args"]["checkpoint"]
	cmd = cli.GetCommand("read-configure")
	if cmd != None:
		try:
			cmd.run([checkpoint])
			result = 'true'
		except Exception as e:
			error = str(e)
	else:
		error = "SE_open_checkpoint:Can not find command: read-configure"
	return response(request, result,error)

def SE_open_reverse_perform(request):
	result='false'
	error=None
	cmd = cli.GetCommand("enable-reverse")
	if cmd != None:
		try:
			cmd.run([])
			result="true"
		except Exception as e:
			error = str(e)
	else:
		error = 'SE_open_reverse_perform:Can not find command "enable-reverse"'
	return response(request, result,error)

def SE_close_reverse_perform(request):
	result='false'
	error=None
	cmd = cli.GetCommand("disable-reverse")
	if cmd != None:
		try:
			cmd.run([])
			result="true"
		except Exception as e:
			error = str(e)
	else:
		error = 'SE_close_reverse_perform:Can not find command "disable-reverse"'
	return response(request, result,error)

def SE_get_disassemble_info(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	addr = str_to_int(request["args"]["baseaddr"])
	length = str_to_int(request["args"]["length"])
	try:
		result=dif.get_disassemble_info(cpuname,addr,length)
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_current_pc(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	try:
		result=dif.get_current_pc(cpuname)
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_system_step(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	try:
		ret=dif.sys_step(cpuname)
		if ret==True:
			result='true'
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_reverse_step(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	cmd = cli.GetCommand("reverse")
	if cmd != None:
		try:
			cmd.run([cpuname, "1"])
			result='true'
		except Exception as e:
			error = str(e)
	else:
		error='SE_reverse_step: Can not find command "reverse"'
	return response(request, result,error)

def SE_create_breakpoint(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	addr = str_to_int(request["args"]["addr"])
	try:
		ret=dif.create_breakpoint(cpuname,addr)
		if ret==True:
			result='true'
			SkyEyeRun()
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_delete_breakpoint(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	addr = str_to_int(request["args"]["addr"])
	try:
		ret=dif.delete_breakpoint(cpuname,addr)
		if ret==True:
			result='true'
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_cpu_register_info(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	try:
		result=regs.get_registers(cpuname)
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_cpu_mips(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	ret=mips.get()
	if cpuname not in ret.keys():
		error = "There is no information about this CPU"
	else:
		result=ret[cpuname]
	return response(request, result,error)

def SE_get_cpu_address_width(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	try:
		result=SkyEyeGetAddressWidth(cpuname)
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_set_local_license(request):
	result = 'false'
	error = None
	lic_path = request["args"]["path"]
	try:
		ret = sky_lic.set_local_license(lic_path)
		if ret[0] == True:
			result="true"
		else:
			error = ret[1]
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_set_network_license(request):
	result='false'
	error=None
	ip = request["args"]["ip"]
	port = request["args"]["port"]
	try:
		ret=sky_lic.set_network_license(ip,port)
		if ret==True:
			result="true"
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_read_license_info(request):
	result='false'
	error=None
	lic_path = request["args"]["path"]
	try:
		ret=sky_lic.read_license_info(lic_path)
		if ret!=None:
			result=ret
		else:
			error = 'license 文件读取失败，可能产生此问题的原因：\n 1.文件名称或路径包含中文。\n 2.生成license文件的密钥不匹配。\n 3.无效的license文件。'
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_license_verify(request):
	result='false'
	error=None
	try:
		ret=sky_lic.license_verify()
		if ret[0]==False:
			error=ret[1]
		else:
			result="true"
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_mac_list(request):
	result = 'false'
	error = None
	try:
		result = sky_lic.get_mac_info()
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_hardware_info(request):
	result = 'false'
	error = None
	try:
		mac_addr = request["args"]["mac_addr"]
	except:
		mac_addr = None
	try:
		ret = sky_lic.get_hardware_info(mac_addr)
		if ret[0] == False:
			error = ret[1]
		else:
			result = ret[1:]
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_simulation_run_time(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	try:
		result=SkyEyeGetSimulationRunTime(cpuname)
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_simulation_insn_number(request):
	result='false'
	error=None
	cpuname = request["args"]["cpuname"]
	try:
		result=SkyEyeGetSimulationInsnNum(cpuname)
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_binary_file_type(request):
	result='false'
	error=None
	filename = request["args"]["filename"]
	try:
		result=naf.get_binary_file_type(filename)
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_abort_gui_info(request):
	result = 'false'
	error = None
	try:
		ver_ret = naf.get_version_info()
		lic_ret = sky_lic.gui_get_license_info()
		if lic_ret != None:
			result = ver_ret
			result["lic_info"] = lic_ret.get_info()
	except Exception as e:
		error = str(e)
	return response(request, result,error)

def SE_get_cpu_exception_num(request):
	result = 'false'
	error = None
	cpuname = request["args"]["cpuname"]
	try:
		result = SkyEyeGetCpuExceptionNum(cpuname)
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def SE_set_cpu_exception(request):
	result = 'false'
	error = None
	cpuname = request["args"]["cpuname"]
	exception_id = str_to_int(request["args"]["exception_id"])
	try:
		result = SkyEyeSetCpuException(cpuname, exception_id)
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def SE_set_ext_interrupt(request):
	result = 'false'
	error = None
	intc_name = request["args"]["intc_name"]
	interrupt_num = str_to_int(request["args"]["interrupt_num"])
	try:
		result = SkyEyeSetExtInterrupt(intc_name, interrupt_num)
	except Exception as e:
		error = str(e)
	return response(request, result, error)

def SE_get_cpu_exception_name(request):
	result = 'false'
	error = None
	cpuname = request["args"]["cpuname"]
	try:
		name_list = []
		num = SkyEyeGetCpuExceptionNum(cpuname)
		for index in range(0,num):
			name = SkyEyeGetCpuExceptionNameById(cpuname, index)
			name_list.append(name)
		result = name_list
	except Exception as e:
		error = str(e)
	return response(request, result, error)

cmd_patterns = {
		'SE_get_workspace':SE_get_workspace,
		'SE_set_workspace':SE_set_workspace,
		'run_script':run_script,
		'soc_list':soc_list,
		'coverage_file':get_coverage_file,
		'objdump_file':get_objdump_file,
		'cpu_list':get_cpu_list,
		'class_list':get_class_list,
		'class_type':get_class_type,
		'attr_list':get_class_attrlist,
		'attr_info':get_class_attrinfo,
		'interface_list':get_interface_list,
		'connect_list':get_connect_list,
		'run':run,
		'chdir':sim_chdir,
		'getcwd':sim_getcwd,
		'reset':reset,
		'stop':stop,
		'quit':quit,
		'getPinNum':get_pin_num,
		'getPinType':get_pin_type,
		'readPinState':read_pin_state,
		'writePinState':write_pin_state,
		'SE_coverage_init':SE_coverage_init,
		'SE_get_running_number':SE_get_running_number,
		'SE_get_running_path':SE_get_running_path,
		'SE_get_total_cov':SE_get_total_cov,
		'SE_get_all_func_cov':SE_get_all_func_cov,
		'SE_get_single_func_cov':SE_get_single_func_cov,
		'SE_get_project_cov_info':SE_get_project_cov_info,
		'SE_set_fault_inject':SE_set_fault_inject,
		'SE_get_fault_inject_list':SE_get_fault_inject_list,
		'SE_delete_fault':SE_delete_fault,
		'SE_get_fault_inject_device_info':SE_get_fault_inject_device_info,
		'SE_get_device_register_info':SE_get_device_register_info,
		'SE_get_all_class_info':SE_get_all_class_info,
		'SE_create_remote_gdb':SE_create_remote_gdb,
		'SE_get_mem_device_addr_info':SE_get_mem_device_addr_info,
		'SE_get_all_fault_inject_device_info':SE_get_all_fault_inject_device_info,
		'SE_get_running_status':SE_get_running_status,
		'SE_get_next_log':SE_get_next_log,
		'SE_get_vm_list':SE_get_vm_list,
		'SE_get_var_list':SE_get_var_list,
		'SE_set_var_conditions':SE_set_var_conditions,
		'SE_clear_var_conditions':SE_clear_var_conditions,
		'SE_get_simulation_device_info':SE_get_simulation_device_info,
		'SE_set_device_register_value':SE_set_device_register_value,
		'SE_get_memory_value':SE_get_memory_value,
		"SE_save_checkpoint":SE_save_checkpoint,
		"SE_open_checkpoint":SE_open_checkpoint,
		"SE_open_reverse_perform":SE_open_reverse_perform,
		"SE_close_reverse_perform":SE_close_reverse_perform,
		"SE_get_disassemble_info":SE_get_disassemble_info,
		"SE_get_current_pc":SE_get_current_pc,
		"SE_system_step":SE_system_step,
		"SE_reverse_step":SE_reverse_step,
		"SE_create_breakpoint":SE_create_breakpoint,
		"SE_get_cpu_register_info":SE_get_cpu_register_info,
		"SE_get_cpu_mips":SE_get_cpu_mips,
		"SE_get_cpu_address_width":SE_get_cpu_address_width,
		"SE_set_local_license":SE_set_local_license,
		"SE_set_network_license":SE_set_network_license,
		"SE_read_license_info":SE_read_license_info,
		"SE_license_verify":SE_license_verify,
		"SE_get_mac_list":SE_get_mac_list,
		"SE_get_hardware_info":SE_get_hardware_info,
		"SE_get_simulation_run_time":SE_get_simulation_run_time,
		"SE_get_simulation_insn_number":SE_get_simulation_insn_number,
		"SE_get_binary_file_type":SE_get_binary_file_type,
		"SE_get_abort_gui_info":SE_get_abort_gui_info,
		"SE_get_cpu_exception_num":SE_get_cpu_exception_num,
		"SE_set_cpu_exception":SE_set_cpu_exception,
		"SE_set_ext_interrupt":SE_set_ext_interrupt,
		"SE_get_cpu_exception_name":SE_get_cpu_exception_name,
		"SE_delete_breakpoint":SE_delete_breakpoint,
		}
