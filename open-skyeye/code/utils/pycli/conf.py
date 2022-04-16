# -*- coding: UTF-8 -*-
import platform
import os
from interval import Interval
#import string
import operator
from copy import deepcopy
import errormessage
from skyeye_common_module import *
from sky_log import *
from parse_json import *
from collections import OrderedDict
import json


Config = None

Cpus = []
skyeye_script = None
reverse_state = False
image_devices = []

def str_to_uint(py_str):
	if operator.eq(py_str[0:2], "0x") == True:
		py_uint = int(py_str, 16)
	else:
		py_uint = int(py_str, 10)
	return py_uint

def is_digit(value):
	try:
		str_to_uint(value)
		return True
	except:
		return False

define_json_file = [""]
binary_l = {}
class json_conf():
	python_val = None
	file_name = None
	__parent = None
	__error_string = None
	__init_sucessfull = False

	def __init__(self, parent, filename):
		self.file_name = filename
		self.__parent = parent
		if os.path.isfile(filename) == True:
			str = loadJson(filename)
			try:
				self.python_val = json.loads(str, object_pairs_hook = OrderedDict)
				self.__init_sucessfull = True
			except ValueError as exp:
				errormessage.SetErrorInfo(0x40000000, False, exp)
				self.__init_sucessfull = False
				raise
		self.__full_file_path  = os.path.join(os.getcwd(), filename)
		self.__full_script_path  = None
		self.__maped_devices = []
		self.TmrList = []

	def get_current_fp(self):
		return self.__full_file_path

	def set_skyeye_script(self, filename):
		self.__full_script_path  = os.path.join(os.getcwd(), filename)

	def get_skyeye_script(self, filename):
		return self.__full_script_path  

	def get_init_result(self):
		return self.__init_sucessfull

	def attr_check(self):
		mach_list = self.get_mach_list()
		for mach in mach_list:
			device_list = self.get_device_list_by_mach(mach)
			for device in device_list:
				device_cls = self.get_device_classname(mach, device)
				attr_list= SkyEyeGetClassAttrList(device_cls)
				json_attr_list = self.get_device_attr_list(mach, device)
				for attr in json_attr_list:
					if attr.strip() == "":
						errormessage.SetErrorInfo(0x4002201a, False, device)
						raise
					if attr not in attr_list:
						errormessage.SetErrorInfo(0x40022002, False, device, attr)
						raise
					attr_info = SkyEyeGetClassAttrInfo(device_cls, attr)
					value_list = self.get_device_attr_value_list(mach, device, attr)
					if isinstance(value_list[0], list):
						for info in value_list:
							attr_type = info[1]
							if attr_type.strip() == "":
								errormessage.SetErrorInfo(0x4002201b, False, device, attr)
								raise
							if operator.eq(attr_info[1], attr_type) == False:
								errormessage.SetErrorInfo(0x40022003, False, device, attr, attr_type)
								raise
							if len(info) < 3:
								errormessage.SetErrorInfo(0x40022011, False, device, attr)
								raise
							if info[2].strip() == "":
								errormessage.SetErrorInfo(0x40022013, False, device, attr)
								raise
					else:
						attr_type = value_list[0]
						if attr_type.strip() == "":
							errormessage.SetErrorInfo(0x4002201b, False, device, attr)
							raise
						if operator.eq(attr_info[1], attr_type) == False:
							errormessage.SetErrorInfo(0x40022003, False, device, attr, attr_type)
							raise
						if len(value_list) < 2:
							errormessage.SetErrorInfo(0x40022011, False, device, attr)
							raise
						if value_list[1].strip() == "":
							errormessage.SetErrorInfo(0x40022013, False, device, attr)
							raise
		linker_list = self.get_linker_list()
		for linker in linker_list:
			linker_cls = self.get_linker_classname(linker)
			attr_list= SkyEyeGetClassAttrList(linker_cls)
			json_attr_list = self.get_linker_attr_list(linker)
			for attr in json_attr_list:
				if attr.strip() == "":
					errormessage.SetErrorInfo(0x4002201a, False, linker)
					raise
				if attr not in attr_list:
					errormessage.SetErrorInfo(0x40022002, False, linker, attr)
					raise
				attr_info = SkyEyeGetClassAttrInfo(linker_cls, attr)
				value_list = self.get_linker_attr_value_list(linker, attr)
				if isinstance(value_list[0], list):
					for info in value_list:
						attr_type = info[1]
						if operator.eq(attr_info[1], attr_type) == False:
							errormessage.SetErrorInfo(0x40022003, False, linker, attr, attr_type)
							raise
						if len(info) < 3:
							errormessage.SetErrorInfo(0x40022011, False, linker, attr)
							raise
						if info[2].strip() == "":
							errormessage.SetErrorInfo(0x40022013, False, linker, attr)
							raise
				else:
					attr_type = value_list[0]
					if operator.eq(attr_info[1], attr_type) == False:
						errormessage.SetErrorInfo(0x40022003, False, linker, attr, attr_type)
						raise
					if len(value_list) < 2:
						errormessage.SetErrorInfo(0x40022011, False, linker, attr)
						raise
					if value_list[1].strip() == "":
						errormessage.SetErrorInfo(0x40022013, False, linker, attr)
						raise

	def class_check(self):
		cls_list= SkyEyeGetClassList()
		mach_list = self.get_mach_list()
		for mach in mach_list:
			mach_cls = self.get_mach_classname(mach)
			if mach_cls not in cls_list:
				errormessage.SetErrorInfo(0x40022001, False, mach, mach_cls)
				raise
			device_list = self.get_device_list_by_mach(mach)
			for device in device_list:
				device_cls = self.get_device_classname(mach, device)
				if device_cls not in cls_list:
					errormessage.SetErrorInfo(0x40022001,False,device,device_cls)
					raise
			
		linker_list = self.get_linker_list()
		for linker in linker_list:
			linker_cls = self.get_linker_classname(linker)
			if linker_cls not in cls_list:
				errormessage.SetErrorInfo(0x40022001, False, linker, linker_cls)
				raise

	def iface_check(self):
		all_device_list = []
		mach_list = self.get_mach_list()
		linker_list = self.get_linker_list()
		for mach in mach_list:
			device_list = self.get_device_list_by_mach(mach)
			all_device_list.extend(device_list)
			for device in device_list:
				device_cls = self.get_device_classname(mach, device)
				conn_list = SkyEyeGetClassConnectList(device_cls)
				json_iface_list = self.get_device_iface_list(mach,device)
				for iface in json_iface_list:
					if iface.strip() == "":
						errormessage.SetErrorInfo(0x40022017, False, device)
						raise
					if iface not in conn_list:
						errormessage.SetErrorInfo(0x40022004, False, device, iface)
						raise
					value_list = self.get_device_iface_value_list(mach, device, iface)
					if isinstance(value_list[0], list):
						for info in value_list:
							if len(info) < 3:
								errormessage.SetErrorInfo(0x40022012, False, device, iface)
								raise
							conn_device = info[2]
							if conn_device.strip() == "":
								errormessage.SetErrorInfo(0x40022018, False, device, iface)
								raise
							if conn_device not in device_list and conn_device not in linker_list:
								errormessage.SetErrorInfo(0x40022005, False, device, iface, conn_device)
								raise
							device_cls = self.get_classname(conn_device)
							iface_list = SkyEyeGetClassIfaceList(device_cls)
							if iface not in  iface_list:
								errormessage.SetErrorInfo(0x40022006, False, device, conn_device, iface)
								raise
						if self.get_device_classname(mach, device) == iface == "memory_space":
							addr_list = []
							def cmp_addr(addr,length):
								start_addr = self.string_to_int(addr)
								end_addr = start_addr + self.string_to_int(length) - 1
								temp = Interval(start_addr, end_addr)
								for addr_info in addr_list:
									if addr_info.overlaps(temp) == True:
										return True
								addr_list.append(temp)
								return False
							for value in value_list:
								if len(value) < 4:
									errormessage.SetErrorInfo(0x40022012, False, device, iface)
									raise
								if isinstance(value[3], list):
									for va in value[3]:
										if len(va) < 2:
											errormessage.SetErrorInfo(0x40022012, False, device, iface)
											raise
										if va[0].strip == "" or va[1].strip == "" or \
											is_digit(va[0]) == False or is_digit(va[1]) == False :
											errormessage.SetErrorInfo(0x40022019, False, device, value[2])
											raise
										if cmp_addr(va[0], va[1]) == True:
											errormessage.SetErrorInfo(0x40022007, False, value[2])
											raise
								else:
									if len(value) < 5:
										errormessage.SetErrorInfo(0x40022012, False, device, iface)
										raise
									if value[3].strip == "" or value[4].strip == "" or \
										is_digit(value[3]) == False or is_digit(value[4]) == False :
										errormessage.SetErrorInfo(0x40022019, False, device, value[2])
										raise
									if cmp_addr(value[3],value[4]) == True:
										errormessage.SetErrorInfo(0x40022007, False, value[2])
										raise
					else:
						if len(value_list) < 2:
							errormessage.SetErrorInfo(0x40022012, False, device, iface)
							raise
						conn_device = value_list[1]
						if conn_device.strip() == "":
							errormessage.SetErrorInfo(0x40022018, False, device, iface)
							raise
						if conn_device not in device_list and conn_device not in linker_list:
							errormessage.SetErrorInfo(0x40022005, False, device, iface, conn_device)
							raise
						device_cls = self.get_classname(conn_device)
						iface_list = SkyEyeGetClassIfaceList(device_cls)
						if iface not in  iface_list:
							errormessage.SetErrorInfo(0x40022006, False, device, conn_device, iface)
							raise
		for linker in linker_list:
			linker_cls = self.get_linker_classname(linker)
			conn_list = SkyEyeGetClassConnectList(linker_cls)
			json_iface_list = self.get_linker_iface_list(linker)
			for iface in json_iface_list:
				if iface.strip() == "":
					errormessage.SetErrorInfo(0x40022017, False, linker)
					raise
				if iface not in conn_list:
					errormessage.SetErrorInfo(0x40022004, False, linker, iface)
					raise
				value_list = self.get_linker_iface_value_list(linker, iface)
				if isinstance(value_list[0], list):
					for info in value_list:
						if len(info) < 3:
							errormessage.SetErrorInfo(0x40022012, False, linker, iface)
							raise
						conn_device = info[2]
						if conn_device.strip() == "":
							errormessage.SetErrorInfo(0x40022018, False, linker, iface)
							raise
						if conn_device not in all_device_list and conn_device not in linker_list:
							errormessage.SetErrorInfo(0x40022005, False, linker, iface, conn_device)
							raise
						device_cls = self.get_classname(conn_device)
						iface_list = SkyEyeGetClassIfaceList(device_cls)
						if iface not in  iface_list:
							errormessage.SetErrorInfo(0x40022006, False, linker, conn_device, iface)
							raise
				else:
					if len(value_list) < 2:
						errormessage.SetErrorInfo(0x40022012, False, linker, iface)
						raise
					conn_device = value_list[1]
					if conn_device.strip() == "":
						errormessage.SetErrorInfo(0x40022018, False, linker, iface)
						raise
					if conn_device not in all_device_list and conn_device not in linker_list:
						errormessage.SetErrorInfo(0x40022005, False, linker, iface, conn_device)
						raise
					device_cls = self.get_classname(conn_device)
					iface_list = SkyEyeGetClassIfaceList(device_cls)
					if iface not in  iface_list:
						errormessage.SetErrorInfo(0x40022006, False, linker, conn_device, iface)
						raise

	def base_check(self):
		v_list = self.python_val.keys()
		def check(dev_name, dev_info):
			if dev_name.strip() == "":
				errormessage.SetErrorInfo(0x40022014, False)
				raise
			try:
				v_class = dev_info['class']
				v_base = dev_info['base']
			except KeyError as e:
				errormessage.SetErrorInfo(0x40022008, False, dev_name, e)
				raise
			if v_base.strip() == "":
				errormessage.SetErrorInfo(0x40022015, False, dev_name)
				raise
			if v_class.strip() == "":
				errormessage.SetErrorInfo(0x40022016, False, dev_name)
				raise
			if v_base not in ["mach", "device", "linker", "cpu"]:
				errormessage.SetErrorInfo(0x40022009, False, dev_name, v_base)
				raise
		for item in v_list:
			check(item, self.python_val[item])
			if self.python_val[item]['base'] == "mach":
				for item2 in self.python_val[item]:
					if item2 in ['base','class']:
						continue
					check(item2, self.python_val[item][item2])

	def conf_check(self):
		self.base_check()
		self.class_check()
		self.attr_check()
		self.iface_check()

	def write_to_file(self, filename):
		str = json.dumps(self.python_val)
		if filename == None:
			f = open(self.file_name, 'w')
		else:
			f = open(filename, 'w')
		f.write(str)
		f.flush()
		f.close()
		return

	def get_mach_list(self):
		v_list = self.python_val.keys()
		return_list = []
		for item in v_list:
			base = self.python_val[item]['base']
			if operator.eq(base, 'linker') != True:
				return_list.append(item)
		return return_list

	def get_cpu_list(self):
		v_list = self.get_mach_list()
		return_list = []
		for mach in v_list:
			return_list += self.get_cpuname_list_by_mach(mach)
		return return_list

	def get_device_list(self):
		v_list = self.get_mach_list()
		return_list = []
		for mach in v_list:
			return_list += self.get_device_list_by_mach(mach)
		return return_list

	def get_linker_list(self):
		v_list = self.python_val.keys()
		return_list = []
		for item in v_list:
			base = self.python_val[item]['base']
			if operator.eq(base, 'linker') == True:
				return_list.append(item)
		return return_list

	def get_memoryspace_by_cpu(self, cpuname):
		mach = self.get_cpu_mach(cpuname)
		device_list = self.get_device_list_by_mach(mach)
		for device in device_list:
			cls = self.get_device_classname(mach, device)
			if operator.eq(cls, "memory_space") == True:
				return device
		return None

	def get_memoryspace_list_by_cpu(self, cpuname):
		return_list = []
		mach = self.get_cpu_mach(cpuname)
		device_list = self.get_device_list_by_mach(mach)
		for device in device_list:
			cls = self.get_device_classname(mach, device)
			if operator.eq(cls, "memory_space") == True:
				return_list.append(device)
		return return_list

	def get_cpucls_by_mach(self, machname):
		device_list = self.get_device_list_by_mach(machname)
		for device in device_list:
			attrlist = self.python_val[machname][device].keys()
			for attr in attrlist:
				if self.is_base(attr) == True:
					value = self.python_val[machname][device][attr]
					if operator.eq(value, 'cpu') == True:
						cpucls = self.get_device_classname(machname, device)
						return cpucls
		return None

	def get_cpucls_list_by_mach(self, machname):
		device_list = self.get_device_list_by_mach(machname)
		cpucls_list = []
		for device in device_list:
			attrlist = self.python_val[machname][device].keys()
			for attr in attrlist:
				if self.is_base(attr) == True:
					value = self.python_val[machname][device][attr]
					if operator.eq(value, 'cpu') == True:
						cpucls = self.get_device_classname(machname, device)
						cpucls_list.append(cpucls)
		return cpucls_list

	def get_cpu_mach(self, cpuname):
		v_list = self.get_mach_list()
		for machname in v_list:
			device_list = self.get_device_list_by_mach(machname)
			for device in device_list:
				attrlist = self.python_val[machname][device].keys()
				for attr in attrlist:
					if self.is_base(attr) == True:
						value = self.python_val[machname][device][attr]
						if operator.eq(value, 'cpu') == True and operator.eq(device, cpuname) == True:
							return machname
		return None

	def get_cpuname_by_mach(self, machname):
		device_list = self.get_device_list_by_mach(machname)
		for device in device_list:
			attrlist = self.python_val[machname][device].keys()
			for attr in attrlist:
				if self.is_base(attr) == True:
					value = self.python_val[machname][device][attr]
					if operator.eq(value, 'cpu') == True:
						return device
		return None

	def get_cpuname_list_by_mach(self, machname):
		device_list = self.get_device_list_by_mach(machname)
		name_list = []
		for device in device_list:
			attrlist = self.python_val[machname][device].keys()
			for attr in attrlist:
				if self.is_base(attr) == True:
					value = self.python_val[machname][device][attr]
					if operator.eq(value, 'cpu') == True:
						name_list.append(device)
		return name_list

	def get_cpulist_by_mach(self, machname):
		cpu_list = []
		device_list = self.get_device_list_by_mach(machname)
		for device in device_list:
			attrlist = self.python_val[machname][device].keys()
			for attr in attrlist:
				if self.is_base(attr) == True:
					value = self.python_val[machname][device][attr]
					if operator.eq(value, 'cpu') == True:
						cpu_list.append(device)
		return cpu_list

	def get_device_list_by_mach(self, machname):
		machine_list = self.get_mach_list()
		return_list = []
		for mach in machine_list:
			if operator.eq(mach, machname) == True:
				device_list = self.python_val[mach].keys()
				for device in device_list:
					if isinstance(self.python_val[mach][device], dict):
						return_list.append(device)
				return return_list
		return None

	def get_mach_attr_list(self, machname):
		machine_list = self.get_mach_list()
		return_list = []
		if machname in machine_list:
			mach_info = self.python_val[machname]
			for attr in mach_info:
				if self.is_attr(attr, mach_info[attr]) == True:
					return_list.append(attr)
		return return_list

	def get_linker_attr_list(self, linkername):
		linker_list = self.get_linker_list()
		return_list = []
		if linkername in linker_list:
			linker_info = self.python_val[linkername]
			for attr in linker_info:
				if self.is_attr(attr, linker_info[attr]) == True:
					return_list.append(attr)
		return return_list

	def get_device_attr_list(self, machname, devicename):
		return_list = []
		if machname in self.python_val:
			if devicename in self.python_val[machname]:
				dev_info = self.python_val[machname][devicename]
				for attr in dev_info:
					if self.is_attr(attr, dev_info[attr]) == True:
						return_list.append(attr)
					else:
						if self.is_base(attr) or \
							self.is_class(attr) or \
								self.is_iface(attr, dev_info[attr]):
								continue
						else:
							errormessage.SetErrorInfo(0x40022010, False, devicename, attr)
							raise
		return return_list

	def get_device_attr_value_list(self, machname, devicename, attrname):
		attr_list = self.get_device_attr_list(machname, devicename)
		if attrname in attr_list:
			return self.python_val[machname][devicename][attrname]
		return None

	def get_linker_attr_value_list(self, linker, attrname):
		attr_list = self.get_linker_attr_list(linker)
		if attrname in attr_list:
			return self.python_val[linker][attrname]
		return None

	def get_mach_attr_value_list(self, machname, attrname):
		attr_list = self.get_mach_attr_list(machname)
		if attrname in attr_list:
			return self.python_val[machname][attrname]
		return None

	def get_device_iface_list(self, machname, devicename):
		return_list = []
		if machname in self.python_val:
			if devicename in self.python_val[machname]:
				dev_info = self.python_val[machname][devicename]
				for attr in dev_info:
					if self.is_iface(attr, dev_info[attr]) == True:
						return_list.append(attr)
		return return_list

	def get_linker_iface_list(self, linkername):
		return_list = []
		if linkername in self.python_val:
			dev_info = self.python_val[linkername]
			for attr in dev_info:
				if self.is_iface(attr, dev_info[attr]) == True:
					return_list.append(attr)
		return return_list

	def get_linker_iface_value_list(self, linkername, ifacename):
		iface_list = self.get_linker_iface_list(linkername)
		if ifacename in iface_list:
			return self.python_val[linkername][ifacename]
		return None

	def get_device_iface_value_list(self, machname, devicename, ifacename):
		iface_list = self.get_device_iface_list(machname, devicename)
		if ifacename in iface_list:
			return self.python_val[machname][devicename][ifacename]
		return None

	def get_mach_classname(self, machname):
		mach_list = self.get_mach_list()
		for mach in mach_list:
			if operator.eq(mach, machname) == True:
				device_list = self.python_val[mach].keys()
				for cls in device_list:
					if self.is_class(cls) == True:
						classname = self.python_val[mach][cls]
						return classname
				return None
		return None

	def get_linker_classname(self, linkername):
		linker_list = self.get_linker_list()
		for linker in linker_list:
			if operator.eq(linker, linkername) == True:
				device_list = self.python_val[linker].keys()
				for cls in device_list:
					if self.is_class(cls) == True:
						classname = self.python_val[linker][cls]
						return classname
				return None
		return None

	def get_device_classname(self, machname, devicename):
		device_list = self.get_device_list_by_mach(machname)
		for device in device_list:
			if operator.eq(device, devicename) == True:
				attr_list = self.python_val[machname][device].keys()
				for cls in attr_list:
					if self.is_class(cls) == True:
						classname = self.python_val[machname][devicename][cls]
						return classname
				return None
		return None

	def get_classname(self, objname):
		mach_list = self.get_mach_list()
		for machname in mach_list:
			if operator.eq(machname, objname) == True:
				attr_list = self.python_val[machname].keys()
				for cls in attr_list:
					if self.is_class(cls) == True:
						classname = self.python_val[machname][cls]
						return classname
			device_list = self.get_device_list_by_mach(machname)
			for device in device_list:
				if operator.eq(device, objname) == True:
					attr_list = self.python_val[machname][device].keys()
					for cls in attr_list:
						if self.is_class(cls) == True:
							classname = self.python_val[machname][device][cls]
							return classname
		linker_list = self.get_linker_list()
		for linker in linker_list:
			if operator.eq(linker, objname) == True:
				device_list = self.python_val[linker].keys()
				for cls in device_list:
					if self.is_class(cls) == True:
						classname = self.python_val[linker][cls]
						return classname
		return None

	def get_mach_for_device(self, devicename):
		mach_list = self.get_mach_list()
		for mach in mach_list:
			device_list = self.get_device_list_by_mach(mach)
			for device in device_list:
				if operator.eq(device, devicename) == True:
					return mach
		return None

	def get_obj_by_class(self, classname):
		mach_list = self.get_mach_list()
		for mach in mach_list:
			device_list = self.get_device_list_by_mach(mach)
			for device in device_list:
				attr_list = self.python_val[mach][device].keys()
				for cls in attr_list:
					if self.is_class(cls) == True:
						clsname = self.python_val[mach][device][cls]
						if(operator.eq(classname, clsname) == True):
							return device
		return None

	def is_base(self, name):
		if operator.eq(name, 'base') == True:
			return True
		else:
			return False

	def is_attr(self, name, value):
		if operator.eq(name, 'base') == True:
			return False
		if operator.eq(name, 'class') == True:
			return False
		if isinstance(value, dict):
			return False
		if isinstance(value, str):
			return False
		if isinstance(value[0], list):
			if operator.eq(value[0][1], "iface") == True:
				return False
			else:
				return True
		else:
			if operator.eq(value[0], "iface"):
				return False
			else:
				return True
		return False

	def is_iface(self, name, value):
		if operator.eq(name, 'base') == True:
			return False
		if operator.eq(name, 'class') == True:
			return False
		if isinstance(value, str):
			return False
		if isinstance(value[0], list):
			if operator.eq(value[0][1], "iface") == True:
				return True
		else:
			if operator.eq(value[0], "iface") == True:
				return True
		return False

	def is_class(self, name):
		if operator.eq(name, 'class') == True:
			return True
		return False

	def is_machine(self, name):
		mach_list = self.get_mach_list()
		if name in mach_list:
			return True
		return False

	def is_device(self, name):
		mach_list = self.get_mach_list()
		for mach in mach_list:
			device_list = self.get_device_list_by_mach(mach)
			if name in device_list:
				return True
		return False

	def is_machine_exist(self, machname):
		if machname in self.python_val:
			return True
		return False

	def is_device_exist(self, machname, devicename):
		if devicename in self.python_val[machname]:
			return True
		return False

	def is_custom_name_exist(self, name):
		mach_list = self.get_mach_list()
		if name in mach_list:
			return True
		for mach in mach_list:
			device_list = self.get_device_list_by_mach(mach)
			if name in device_list:
				return True
		linker_list = self.get_linker_list()
		if name in linker_list:
			return True
		return False

	def set_machine(self, base, clsname, name):
		self.python_val[name] = {}
		self.python_val[name]['class'] = clsname
		if base != None:
			self.python_val[name]['base'] = base
		return

	def set_machine_device(self, base, machname, clsname, devicename):
		self.python_val[machname][devicename] = {}
		self.python_val[machname][devicename]['class'] = clsname
		if base != None:
			self.python_val[machname][devicename]['base'] = base
		return

	def set_machine_attr(self, machname, attrname, attrtype, value):
		self.python_val[machname][attrname] = [attrtype, value]

	def set_device_attr(self, machname, devicename, attrname, attrtype, value):
		self.python_val[machname][devicename][attrname] = [attrtype, value]

	def modify_machname(self, machname, newname):
		if self.is_machine(machname):
			self.python_val[newname] = deepcopy(self.python_val[machname])
			del self.python_val[machname]

	def modify_devicename(self, machname, devicename, newname):
		if self.is_device(devicename):
			self.python_val[machname][newname] = deepcopy(self.python_val[machname][devicename])
			del self.python_val[machname][devicename]

	def delete_machine(self, machname):
		del self.python_val[machname]

	def delete_machine_attr(self, machname, attrname):
		del self.python_val[machname][attrname]

	def delete_device(self, machname, devicename):
		del self.python_val[machname][devicename]

	def delete_device_attr(self, machname, devicename, attrname):
		del self.python_val[machname][devicename][attrname]

	def create_mach(self, machname, machcls):
		try:
			ret = SkyEyeCreateMach(machname, machcls)
			return ret
		except:
			errormessage.SetErrorInfo(0x4002200a, False, machname)
			raise

	def create_linker(self, linkername, linkercls):
		try:
			ret = SkyEyeCreateLinker(linkername, linkercls)
			return ret
		except:
			errormessage.SetErrorInfo(0x4002200b, False, linkername)
			raise

	def create_cpu(self, machname, cpuname, cpucls):
		try:
			ret = SkyEyeCreateCpu(machname, cpuname, cpucls)
			return ret
		except:
			errormessage.SetErrorInfo(0x4002200c, False, cpuname)
			raise

	def create_device(self, machname, devicename, devcls):
		try:
			ret = SkyEyeCreateDevice(machname, devicename, devcls)
			image_devices.append(str(devicename))
			return ret
		except:
			errormessage.SetErrorInfo(0x4002200d, False, devicename)
			raise

	def set_attr(self, objname, key, attr_type, value):
		try:
			ret = SkyEyeSetAttr(objname, key, attr_type, value)
		except:
			errormessage.SetErrorInfo(0x4002200e, False)

	def get_maped_devices(self):
		return self.__maped_devices

	def get_list_from_valuelist(self, valuelist):
		return_list = []
		if isinstance(valuelist[0], list):
			for _list in valuelist:
				del _list[0]
				return_list.append(_list)
		else:
			return_list.append(valuelist)
		return return_list

	def instance(self):
		if self.__init_sucessfull == False:
			return False
		map_dic={"c6713":[0xc0000000, 0x10000]}
		linkerlist = self.get_linker_list()
		for linker in linkerlist:
			clsname = self.get_linker_classname(linker)
			self.create_linker(linker, clsname)

		machlist = self.get_mach_list()
		for mach in machlist:
			clsname = self.get_mach_classname(mach)
			self.create_mach(mach, clsname)
			devlist = self.get_device_list_by_mach(mach)
			idle_device = 'idle_device_%s' % mach
			self.create_device(mach,idle_device,'idle_device')
			for device in devlist:
				clsname = self.get_device_classname(mach, device)
				if operator.eq(self.python_val[mach][device]['base'], 'cpu') == True:
					if self.create_cpu(mach, device, clsname)==False:
						errormessage.SetErrorInfo(0x4002200f,False , device)
						raise
			for device in devlist:
				clsname = self.get_device_classname(mach, device)
				if operator.eq(self.python_val[mach][device]['base'], 'device') == True:
					self.create_device(mach, device, clsname)

			for device in devlist:
				if self.python_val[mach][device]['base'] in ['cpu', 'device']:
					classname = self.get_device_classname(mach,device)
					attrlist = self.get_device_attr_list(mach, device)
					for attr in attrlist:
						valuelist = self.get_device_attr_value_list(mach, device, attr)
						v_l_list = self.get_list_from_valuelist(valuelist)
						for v_l in v_l_list:
							self.set_attr(device, attr, v_l[0], v_l[1])
					ifacelist = self.get_device_iface_list(mach, device)
					for iface in ifacelist:
						valuelist = self.get_device_iface_value_list(mach, device, iface)
						v_l_list = self.get_list_from_valuelist(valuelist)
						if operator.eq(classname , "memory_space") == True and operator.eq(iface, 'memory_space') == True:
							for v_l in v_l_list:
								if isinstance(v_l[2], list):
									for index in range(0,len(v_l[2])):
										addr = str_to_uint(v_l[2][index][0])
										length = str_to_uint(v_l[2][index][1])
										SkyEyeMpAddMapGroup(device, v_l[1], addr, length - 1, index)
										self.__maped_devices.append({"name": v_l[1]+".Group%d"%index,\
												 "mach":mach,\
												 "length":length,\
												 "offset":0,\
												 "base":addr})
									continue
								addr = str_to_uint(v_l[2])
								length = str_to_uint(v_l[3])
								SkyEyeMpAddMap(device, v_l[1], addr, length - 1)
								self.__maped_devices.append({"name":v_l[1],\
										 "mach":mach,\
										 "length":length,\
										 "offset":0,\
										 "base":addr})
						else:
							for v_l in v_l_list:
								try:
									index = str_to_uint(v_l[2])
								except:
									index = 0
								SkyEyeConnect(device, v_l[1], iface, index)
						
					if operator.eq(classname , "memory_space") == True:
						soc_cls = self.get_mach_classname(mach)
						if soc_cls in map_dic.keys():
							SkyEyeMpAddMap(device, idle_device, map_dic[soc_cls][0], map_dic[soc_cls][0])
		linker_list = self.get_linker_list()
		for linker in linker_list:
			attrlist = self.get_linker_attr_list(linker)
			for attr in attrlist:
				valuelist = self.get_linker_attr_value_list(linker, attr)
				v_l_list = self.get_list_from_valuelist(valuelist)
				for v_l in v_l_list:
					self.set_attr(linker, attr, v_l[0], v_l[1])
			ifacelist = self.get_linker_iface_list(linker)
			for iface in ifacelist:
				valuelist = self.get_linker_iface_value_list(linker, iface)
				v_l_list = self.get_list_from_valuelist(valuelist)
				for v_l in v_l_list:
					try:
						index = str_to_uint(v_l[2])
					except:
						index = 0
					SkyEyeConnect(linker, v_l[1], iface, index)
		SkyEyeConfigConfObj()
		return True

	def string_to_int(self, string):
		if operator.eq(string[0:2], '0x') == True:
			py_uint = int(string, 16)
		else:
			py_uint = int(string, 10)
		return py_uint

class Cpu():
	def __init__(self, name):
		self.name = name
		self.last_steps = 0
		self.speed = 0
		self.binary = None
	def update_speed(self):
		steps = SkyEyeGetCpuSteps(self.name)
		self.speed = steps - self.last_steps
		self.last_steps = steps

def SetGlobalConfig(config):
	global Config
	global Cpus
	global binary_l
	global define_json_file
	Config = config
	if config:
		cpu_list = Config.get_cpu_list()
		Cpus = [Cpu(name = cpu) for cpu in cpu_list]
	else:
		Cpus = []
		binary_l = {}
		define_json_file = [""]



def GetGlobalConfig():
	return Config


def SetGlobalScript(filename):
	global skyeye_script
	skyeye_script = os.path.join(os.getcwd(), filename)

def GetGlobalScript():
	return skyeye_script

def SetReverseState(state):
	global reverse_state
	reverse_state =  state

def GetReverseState():
	return reverse_state 

def SetAllToDyncom(mode):
	global Config
	cpus = Config.get_cpu_list()
	for cpu in cpus:
		SkyEyeCpuSetMode(cpu, mode)
