import conf,sys
import skyeye_common_module as scm
import functools
import os
import win_workspace as ws

system = None

iface_table = {}


def get_device_interface():
	global iface_table
	ws_dir = ws.get_workspace_dir()
	if ws_dir == None:
		print ("Failed to get workspace")
		return
	py_path = os.path.join(ws_dir,"win32/interface/py")
	module_list = []
	if not py_path in sys.path:
		sys.path.append(py_path)
	for root, dirs, files in os.walk(py_path):
		for name in files:
			if name.endswith(".py"):
				temp = os.path.splitext(name)
				module_list.append(temp[0])
	for module in module_list:
		if module not in sys.modules:
			mod = __import__(module)
			try:
				ret = mod.init_func()
				iface_table.update(ret)
			except:
				pass

get_device_interface()

def set_reg_value(machname,devname,regname,val):
	try:
		regid = scm.SkyEyeGetDevRegIdByName(machname, devname, regname)
		scm.SkyEyeSetDevRegValueById(machname, devname, val, regid)
	except:
		output_log("set_reg_value call error!")

def get_reg_value(machname,devname,regname):
	try:
		regid = scm.SkyEyeGetDevRegIdByName(machname, devname, regname)
		return scm.SkyEyeGetDevRegValueById(machname, devname, regid)
	except:
		output_log("get_reg_value call error!")
		return None


def connect(conobj,ifaceobj, conname):
	if conname in ifaceobj.interfaces:
		try:
			scm.SkyEyeConnect(conobj.objname, ifaceobj.objname, conname, 0)
		except:
			output_log("SkyEyeConnect call error!")
			return
		ifaceobj.machname = conobj.machname
		system.__dict__[conobj.machname].__dict__[ifaceobj.objname] = ifaceobj
	else:
		raise AttributeError(conname + ' not exists in the ' + ifaceobj.objname)

def set_dev_attr(objname,classname,key,value):
	try:
		info = scm.SkyEyeGetClassAttrInfo(classname,key)
		scm.SkyEyeSetAttr(objname, key, info[1], value)
	except:
		output_log("set_dev_attr call error!")

def get_node(root,level = 1):
	print (root.objname)
	for i in root.__dict__:
		if isinstance(root.__dict__[i],(SystemClass,SkyEyeBaseClass)) == True:
			print ("     |"*level+"--", end = "")
			get_node(root.__dict__[i],level+1)
		elif isinstance(root.__dict__[i],list) == True:
			if len(root.__dict__[i][:2]) == 0:
				continue
			print ("     |"*level+"--", end = "")
			print (i+"  ", end = "")
			print (root.__dict__[i])
		else:
			pass

class SkyEyeIfaceClass(object):
	def __init__(self, objname):
		self.objname = objname
	@classmethod
	def set_instance_method(cls, func):
		@functools.wraps(func)
		def dummy(self, *args, **kwargs):
			func(self.objname, *args, **kwargs)
		setattr(cls, func.func_name, dummy)

class SkyEyeBaseClass:
	def __init__(self,objname,classname,machname):
		Ifaces = []
		regs = []
		connects = []
		attrs = []
		try:
			Ifaces = scm.SkyEyeGetClassIfaceList(classname)
		except:
			output_log("SkyEyeGetClassIfaceList call error!")
		try:
			if "skyeye_register_intf" in Ifaces:
				RegNum = scm.SkyEyeGetDevRegNum(machname,objname)
				for regid in range(0,RegNum):
					name = scm.SkyEyeGetDevRegNameById(machname,objname,regid)
					if name != None:
						regs.append(name)
		except:
			output_log("SkyEyeGetDevRegName call error!")
		try:
			connects = scm.SkyEyeGetClassConnectList(classname)
		except:
			output_log("SkyEyeGetClassConnectList call error!")
		try:
			attrs = scm.SkyEyeGetClassAttrList(classname)
		except:
			output_log("SkyEyeGetClassAttrList call error!")
		self.__dict__["objname"] = objname
		self.__dict__["classname"] = classname
		self.__dict__["machname"] = machname
		self.__dict__["regs"] = regs
		self.__dict__["connects"] = connects
		self.__dict__["interfaces"] = Ifaces
		self.__dict__["attrs"] = attrs
		ifaceObject = {}
		for iface in Ifaces:
			obj = SkyEyeIfaceClass(objname)
			ifaceObject[iface] = obj
			if iface not in iface_table:
				continue
			for api in iface_table[iface]:
				obj.set_instance_method(api)
		self.__dict__["interfaces_obj"] = ifaceObject

	def get_tree(self):
		r_obj = __redirection__()
		r_obj.set_out()
		get_node(self)
		r_obj.reset()
		return r_obj.buff

	def __setattr__(self, attr, value):
		if attr in self.__dict__:
			self.__dict__[attr] = value
		elif attr in self.__dict__['regs']:
			set_reg_value(self.machname,self.objname,attr, value)
		elif attr in self.__dict__['connects']:
			connect(self,value,attr)
		elif attr in self.__dict__['attrs']:
			set_dev_attr(self.objname,self.classname,attr,value)
		else:
			self.__dict__[attr] = value

	def __getattr__(self, attr):
		if attr in self.__dict__:
			return self.__dict__[attr]
		elif attr in self.__dict__['regs']:
			return get_reg_value(self.machname,self.objname,attr)
		elif attr in self.__dict__['interfaces']:
			return self.__dict__["interfaces_obj"][attr]
		else:
			raise AttributeError(attr + ' not exists')

class SystemClass():
	def __init__(self):
		self.__dict__["objname"] = "system"
		pass
	def get_tree(self):
		r_obj=__redirection__()
		r_obj.set_out()
		get_node(self)
		r_obj.reset()
		return r_obj.buff

def CreateClass():
	global system
	system = SystemClass()
	config = conf.GetGlobalConfig()
	if config == None:
		return None
	mach_list = config.get_mach_list()
	for mach in mach_list:
		cpu_cls_list = config.get_cpucls_list_by_mach(mach)
		if "x86" in cpu_cls_list:
			continue
		mach_cls_name = config.get_mach_classname(mach)
		mach_cls = SkyEyeBaseClass(mach,mach_cls_name,mach)
		dev_list = config.get_device_list_by_mach(mach)
		for dev in dev_list:
			dev_cls_name = config.get_device_classname(mach,dev)
			DevCls = SkyEyeBaseClass(dev,dev_cls_name,mach)
			mach_cls.__dict__[dev] = DevCls
		system.__dict__[mach] = mach_cls
	return system

def output_log(log):
	r_obj=__redirection__()
	r_obj.set_out()
	print (log)
	r_obj.reset()
	return r_obj.buff

class __redirection__():
	def __init__(self):
		self.save_stdlog = sys.stdout
		self.buff = ""
	def set_out(self):
		sys.stdout = self
	def write(self,info):
		self.buff = self.buff+info
	def reset(self):
		sys.stdout = self.save_stdlog
