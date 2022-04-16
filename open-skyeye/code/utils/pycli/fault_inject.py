#coding:utf-8
import conf
import operator
import regs as regs_func
import skyeye_common_module as scm
from x86 import *
import errormessage

def str_to_int(strs):
	if strs[:2]=="0x":
		return int(strs,16)
	else:
		return int(strs)

def get_memoryspace_by_machname_devname(machname,devname):
	config  = conf.GetGlobalConfig()
	if config!=None:
		device_list = config.get_device_list_by_mach(machname)
		if device_list == None:
			return None
		for device in device_list:
			cls = config.get_device_classname(machname, device)
			if operator.eq(cls, "memory_space") == True:
				for attr in config.get_device_attr_list(machname,device):
					for value in config.get_device_attr_value_list(machname,device,attr):
						if devname in value:
							return device
	return None

def get_device_id_by_machname_devname(machname,devname):
	config=conf.GetGlobalConfig()
	if config==None:
		return None,None
	cpu=config.get_cpuname_by_mach(machname)

	x86_obj=x86(cpu)
	for dev in x86_obj.devices:
		if dev.name == devname:
			return dev.ID,cpu
	return None,None

def set_x86_device_fj(machname,devname,addr,bit,mode):
	did,cpu=get_device_id_by_machname_devname(machname,devname)
	if did == None or cpu ==None:
		return False
	try:
		scm.SkyEyeX86SaveFin(cpu, did, addr, bit, mode)
	except:
		return False
	return True

def set_general_device_fj(machname,devname,addr,bit,mode):
	mach_info = get_all_fault_inject_device_info()
	if mach_info[machname][devname]['type'] == "MemDev":
		count = 1
	else:
		reg_name = get_regname_by_addr(machname, devname, addr)
		count = mach_info[machname][devname]["register"][reg_name]["width"]
	memname=get_memoryspace_by_machname_devname(machname,devname)
	if memname ==None:
		return False
	try:
		scm.SkyEyeSetFj(memname,addr,bit,mode,count)
	except:
		return False
	return True

def check_fj_list(machname,devname):
	fj_list=skyeye_get_fj()
	temp=[]
	for fj in fj_list:
		temp.append(fj[1]+"."+fj[0])
	if temp.count(machname+"."+devname)>=20:
		return True
	return False


def skyeye_set_fj(machname,devname,addr,bit,mode):
	config  = conf.GetGlobalConfig()
	if config==None:
		return False
	if check_fj_list(machname,devname)==True:
		print ("The device can inject up to 20 failures")
		return False
	cpu = config.get_cpuname_by_mach(machname)
	cpu_cls =config.get_classname(cpu)
	if cpu_cls=="x86":
		return set_x86_device_fj(machname,devname,addr,bit,mode)
	else:
		return set_general_device_fj(machname,devname,addr,bit,mode)

def check_value(va):
	if isinstance(va,str):
		if va[:2] == "0x":
			return int(va,16)
		else:
			return int(va,10)
	else:
		return va

def get_devname_by_addr(machname,addr):
	dev_info = get_fault_inject_device_info()
	if dev_info == False:
		return None
	for dev in dev_info[machname].keys():
		baseaddr = dev_info[machname][dev]["baseaddr"]
		length = dev_info[machname][dev]["length"]
		if addr >= baseaddr and addr <= baseaddr+length:
			return dev
	return None

def skyeye_set_fj_by_aadr(machname,addr,bit,mode):
	addr = check_value(addr)
	devname = get_devname_by_addr(machname,addr)
	if devname == None:
		return False
	skyeye_set_fj(machname,devname,addr,bit,mode)

def get_regname_by_addr(machname,devname,addr):
	reg_info=get_device_register_info(machname,devname)
	for reg in reg_info.keys():
		if reg_info[reg]['addr']==addr:
			return reg
	return None

def get_general_device_fj(config,mach):
	device_list = config.get_maped_devices()
	fault_list=[]
	for dev in device_list:
		if dev['mach']==mach:
			memname=get_memoryspace_by_machname_devname(dev["mach"],dev["name"])
			if memname ==None:
				return []
			try:
				fj_list=scm.SkyEyeGetFj(memname,dev["base"]+1)
			except:
				fj_list = []
			for fj in fj_list:
				reg_name=get_regname_by_addr(dev['mach'],dev['name'],fj['addr'])
				temp=[dev['name'],dev['mach'],reg_name,fj['addr'],fj['bit'],fj['mode']]
				fault_list.append(temp)
	return fault_list

def get_x86_device_fj(config,cpu,mach):
	fault_list=[]
	x86_obj=x86(cpu)
	for device in x86_obj.devices:
		try:
			fj_list=scm.SkyEyeX86GetFin(cpu, device.ID)
		except:
			fj_list = []
		for fj in fj_list:
			reg_name=get_regname_by_addr(mach,device.name,fj['addr'])
			temp=[device.name,mach,reg_name,fj['addr'],fj['bit'],fj['mode']]
			fault_list.append(temp)
	return fault_list

def skyeye_get_fj():
	config  = conf.GetGlobalConfig()
	if config==None:
		return False
	fault_list=[]
	mach_list = config.get_mach_list()
	for mach in mach_list:
		cpu = config.get_cpuname_by_mach(mach)
		cpu_cls =config.get_classname(cpu)
		if cpu_cls=="x86":
			temp=get_x86_device_fj(config,cpu,mach)
		else:
			temp=get_general_device_fj(config,mach)
		fault_list=fault_list+temp
	return fault_list

def clear_general_device_fj(machname,devname,addr,bit,mode):
	memname=get_memoryspace_by_machname_devname(machname,devname)
	if memname == None:
		return False
	try:
		scm.SkyEyeClearFj(memname, addr, bit, mode)
	except:
		return False
	return True

def clear_x86_device_fj(machname,devname,addr,bit,mode):
	did,cpu=get_device_id_by_machname_devname(machname,devname)
	if did == None or cpu ==None:
		return False
	try:
		scm.SkyEyeX86ClearFin(cpu, did, addr, bit, mode)
	except:
		return False
	return True

def skyeye_clear_fj(machname,devname,addr,bit,mode):
	config  = conf.GetGlobalConfig()
	if config==None:
		return False
	cpu = config.get_cpuname_by_mach(machname)
	cpu_cls =config.get_classname(cpu)
	if cpu_cls=="x86":
		return clear_x86_device_fj(machname,devname,addr,bit,mode)
	else:
		return clear_general_device_fj(machname,devname,addr,bit,mode)

def skyeye_clear_fj_by_addr(machname,addr,bit,mode):
	addr = check_value(addr)
	devname = get_devname_by_addr(machname,addr)
	if devname == None:
		return False
	skyeye_clear_fj(machname,devname,addr,bit,mode)

def get_x86_device_info(cpu):
	x86_obj=x86(cpu)
	temp={}
	for device in x86_obj.devices:
		reg_len=len(device.regs)
		temp[device.name]={"baseaddr":0,"length":reg_len}
	return temp

def get_general_device_info(config,mach):
	temp={}
	device_list = config.get_maped_devices()
	for dev in device_list:
		clsname = config.get_device_classname(mach, dev["name"])
		if mach==dev["mach"]:
			temp[dev['name']]={'baseaddr':dev['base'],'length':dev['length'],'classname':clsname}
	return temp


def get_fault_inject_device_info():
	config  = conf.GetGlobalConfig()
	if config==None:
		return False
	dev_info={}
	mach_list = config.get_mach_list()
	for mach in mach_list:
		cpu = config.get_cpuname_by_mach(mach)
		cpu_cls =config.get_classname(cpu)
		if cpu_cls=="x86":
			dev_info[mach]=get_x86_device_info(cpu)
		else:
			dev_info[mach]=get_general_device_info(config,mach)
	return dev_info

def get_general_device_register_info(devname,baseaddr):
	regs_list = regs_func.get_registers(devname)
	temp={}
	for reg in regs_list:
		temp[reg['name']]={"offset":reg['offset'],"width":reg['size'],"value":reg['value'],"bits":reg['size']*8,"addr":baseaddr+reg["offset"]}
	return temp


def get_x86_device_register_info(cpu,devname,baseaddr):
	x86_obj=x86(cpu)
	temp={}
	for device in x86_obj.devices:
		if device.name==devname:
			wi=device.width
			for reg in device.regs:
				temp[reg.name]={"offset":reg.addr,"width":wi,"value":reg.value,"bits":wi*8,"addr":baseaddr+reg.addr}
	return temp


def get_device_register_info(machname,devname):
	config = conf.GetGlobalConfig()
	if config==None:
		return False
	cpu = config.get_cpuname_by_mach(machname)
	cpu_cls =config.get_classname(cpu)
	dev_info=get_fault_inject_device_info()
	if devname not in dev_info[machname]:
		return False
	baseaddr=dev_info[machname][devname]["baseaddr"]
	if cpu_cls=="x86":
		return get_x86_device_register_info(cpu,devname,baseaddr)
	else:
		return get_general_device_register_info(devname,baseaddr)

def get_mem_device_addr_info(machname,addr):
	config = conf.GetGlobalConfig()
	if config==None:
		return False
	try:
		cpu = config.get_cpuname_by_mach(machname)
		try:
			value=scm.SkyEyeReadMemory8(cpu, addr)
		except:
			return False
		bits=8
		return {"value":value,"bits":bits}
	except:
		return False

def get_all_fault_inject_device_info():
	mach_info=get_fault_inject_device_info()
	if mach_info == False:
		return False
	for machname in mach_info.keys():
		for devname in list(mach_info[machname].keys()):
			if mach_info[machname][devname].get("classname") in ["ram", "ram16"]:
				reg_info = {}
				dev_type="MemDev"
			else:
				dev_type="IODev"
				reg_info = get_device_register_info(machname,devname)
				if reg_info == False:
					return False
				if not reg_info:
					mach_info[machname].pop(devname)
					continue
			mach_info[machname][devname]["type"]=dev_type
			mach_info[machname][devname]["register"]=reg_info
	return mach_info



