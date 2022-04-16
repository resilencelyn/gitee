# -*- coding: UTF-8 -*-

import os
import re
import sys
import platform
import time
import pdb
import string
import cli
import shutil
import json
import operator
import se_path
import configparser
from sky_log import *
from conf import *
import subprocess
import shelve
from contextlib import closing

process_pool = []

os_info = platform.system()

arch = {'c67x_core':'tic6x',
	'c6713_core':'tic6x',
	'cortex_a9_core':'arm',
	'sparc_v8':'sparc'
}

def str_to_uint(str):
	py_str = str#.encode("utf-8")
	if operator.eq(py_str[0:2], "0x") == True:
		py_uint = int(py_str, 16)
	else:
		py_uint = int(py_str, 10)
	return py_uint

def is_windows():
	os_info = platform.system()
	if operator.eq(os_info, "Windows"):
		return True
	return False
def is_linux():
	os_info = platform.system()
	if operator.eq(os_info, "Linux"):
		return True
	return False


class Profile():
	__filename = "skyeye.cfg"
	def __init__(self, filename):
		if filename != None:
			self.__filename = filename
		__OsInfo = platform.system()
		__path = se_path.LogPath
		self.__configfilepath = os.path.join(__path, self.__filename)
		if operator.eq(__OsInfo, "Linux") == True:
			if not os.path.isdir(__path):
				os.makedirs(__path)
			if os.path.exists(self.__configfilepath) == False:
				os.system(r"touch %s"% self.__configfilepath)
		else:
			if os.path.exists(self.__configfilepath) == False:
				f = open(self.__configfilepath, "w")
				f.close()

		self.__config=configparser.ConfigParser()

	def has_section(self, section):
		self.__config.read(self.__configfilepath)
		ret = self.__config.has_section(section)
		return  ret

	def has_key(self, section, key):
		self.__config.read(self.__configfilepath)
		if self.has_section(section) == False:
			return False
		ret = self.__config.has_option(section, key)
		return ret

	def get(self, section, key):
		self.__config.read(self.__configfilepath)
		value = self.__config.get(section, key)
		#value  = value.decode('utf8')
		return value

	def getint(self, section, key):
		self.__config.read(self.__configfilepath)
		value = self.__config.getint(section, key)
		#value  = value.decode('utf8')
		return value

	def set(self, section, key, value):
		self.__config.read(self.__configfilepath)
		ret = self.__config.has_section(section)
		if ret == False:
			self.__config.add_section(section)
		#value = value.encode('utf8')
		self.__config.set(section, key, value)
		self.__config.write(open(self.__configfilepath, "w"))

	def sections(self):
		self.__config.read(self.__configfilepath)
		ret = self.__config.sections()
		return ret

	def keys(self, section):
		self.__config.read(self.__configfilepath)
		ret = self.__config.options(section)
		return ret

	def remove_key(self, section, key):
		self.__config.read(self.__configfilepath)
		ret = self.__config.remove_option(section, key)
		self.__config.write(open(self.__configfilepath, "w"))
		return ret

	def remove_section(self, section):
		self.__config.read(self.__configfilepath)
		ret = self.__config.remove_section(section)
		self.__config.write(open(self.__configfilepath, "w"))
		return ret



class SkyEyeConf():
	__confdic = {}
	def __init__(self, filename):
		filehandle = open(filename, "r+")
		for lines in filehandle.readlines():
			lines = lines.replace(" ", "")
			if lines.find(':')  < 0:
				continue
			key = lines[0:lines.index(':')]
			value = lines[lines.index(':')+ 1:lines.index('\n')]
			if value.find('=') < 0:
				self.__confdic[key] = value
			else:
				self.__confdic[key] = {}
				item = value.split(',')
				for subitem  in item:
					if subitem.find('=') < 0:
						continue
					subkey = subitem[0:subitem.index('=')]
					subvalue = subitem[subitem.index('=') + 1:]
					self.__confdic[key][subkey] = subvalue
		filehandle.close()

	def has_key(self, key):
		if key in self.__confdic:
			return True
		return False

	def has_subkey(self, key, subkey):
		if subkey in self.__confdic[key]:
			return True
		return False

	def get(self, key):
		return self.__confdic.get(key)

	def get_subkey(self, key, subkey):
		return self.__confdic[key].get(subkey)

	def keys(self):
		return self.__confdic.keys()

class ExitCodeManager():
	def __init__(self, filename):
		self.ExitCodeDic = {}
		LineList = None
		try:
			FileHander = open(filename)
			LineList = FileHander.readlines()
			FileHander.close()
		except:
			pass

		if LineList != None:
			for line in LineList:
				if line.find('=') < 0:
					continue
				key = line[0:line.index('=')]
				value = line[line.index('=') + 1:]
				key_int = int(key)
				self.ExitCodeDic[key_int] = value

	def GetExp(self, id):
		return self.ExitCodeDic[id]

class CommandFile():
	def __init__(self, filepath):
		self.FilePath = filepath
		self.ErrorString = None
		self.InitFlag = None
		self.CommandList = []
		if os.path.exists(filepath) == False:
			self.InitFlag = False
			self.ErrorString = "Can not find the file: %s" % (filepath)
			return
		self.ReadScript()
		self.InitFlag = True

	def SetErrorString(self, string):
			self.ErrorString = string

	def IsInitSuccess(self):
		return self.InitFlag
	def GetErrorString(self):
		return self.ErrorString

	def ReadScript(self):
		filehandle = open(self.FilePath, "r+")
		lines = filehandle.readlines()
		for line in lines:
			line = line.strip(' \n')
			if line == "" or line[0] == '#':
				continue
			try:
				args = line.split(' ')
			except:
				pass
			while(1):
				try:
					args.remove('')
				except:
					break
			cmd = args[0]
			del args[0]
			for index in range(0,len(args)):
				args[index] = args[index].replace("%20"," ")
			cmddic = {}
			cmddic[cmd] = args
			self.CommandList.append(cmddic)

	def CheckCommandExists(self):
		for cmddic in self.CommandList:
			for cmd in cmddic.keys():
				ret = cli.IsCommandExists(cmd)
				if ret == True:
					continue
				else:
					string =  "No command:%s" % (cmd)
					self.SetErrorString(string)
					return False
		return True

	def ExectueCmdFile(self):
		for cmddic in self.CommandList:
			for cmd in cmddic.keys():
				cmdobj = cli.GetCommand(cmd)
				try:
					ret = cmdobj.run(cmddic[cmd])
				except:
					raise
				if ret == False:
					return False
		return True

	def __ArgListToString(self, args_list):
		string = ""
		num = len(args_list)
		for i in range(0, num):
			string = string + " "
			string = string + args_list[i]
		print (string)
		return string

def GetCommandFile(filename):
	cmdfile = CommandFile(filepath = filename)
	ret = cmdfile.IsInitSuccess()
	if ret == False:
		return None
	return cmdfile

def ParseParameter(arg):
	arglist = []
	if arg == "":
		return arglist
	arglist = arg.split(' ')
	while 1:
		try:
			arglist.remove('')
		except:
			break
	return arglist
	
def GetPackageConfig():
	package_path = se_path.PackageFilePath
	if os.path.exists(package_path) == False:
		return ""
	package_config=configparser.ConfigParser()
	package_config.read(package_path)
	return package_config

def GetBuildDate(config):
	if config != "" and config.has_section("build") and config.has_option("build", "date"):
		return config.get("build", "date")
	else:
		return ""

def GetBuildVersion(config):
	if config != "" and config.has_section("build") and config.has_option("build", "version"):
		return config.get("build", "version")
	else:
		return ""
def GetCommitId(config):
	if config != "" and config.has_section("build") and config.has_option("build", "commit"):
		return config.get("build", "commit")
	else:
		return ""


def GetPackageDic():
	dic = {}
	if os_info != "Linux":
		import winreg
		key = r"SOFTWARE\SkyEye\PackageList"
		try:
			i = 0
			while 1:
				handle = winreg.OpenKeyEx(winreg.HKEY_LOCAL_MACHINE, key)
				name, value, ty = winreg.EnumValue(handle, i)
				dic[name] = value
				i +=1
		except WindowsError:
			pass
	return dic

def create_info_file(filename):
	dic = {}
	dic["images"] = {}
	dic["steps"] = {}
	info_str = json.dumps(dic)
	f = open(filename, 'w')
	f.write(info_str)
	f.close()

def create_chp_dir(dirname):
	full_chp_dir = dirname
	if os.path.exists(full_chp_dir):
		raise IOError("%s already exists, please specify again!" % full_chp_dir)
	os.mkdir(full_chp_dir)
	create_info_file(os.path.join(full_chp_dir, "info"))
	return full_chp_dir

def cmd_get_chp_dir(dirname):
	if is_linux():
		cwd = os.getcwd()
	else:
		import win_workspace as ws
		cwd = ws.get_workspace_dir()

	if dirname.endswith(".ckpt") == False:
		ckpt_name = ''.join([dirname, ".ckpt"])
	else:
		ckpt_name = dirname
	full_chp_dir = os.path.join(cwd, ckpt_name)
	if os.path.exists(full_chp_dir) == False:
		raise IOError("%s does not exist, please specify again!" % ckpt_name)
	return full_chp_dir

def ckpt_save_files(ckpt_dir, fp_list = []):
	for f in fp_list:
		if os.path.exists(f):
			fname = os.path.basename(f)
			shutil.copy(f, os.path.join(ckpt_dir, fname))
		else:
			raise IOError("%f does not exist!" % f)

def ckpt_save_image_info(chpt_dir, imagename, index, filename):
	info = os.path.join(chpt_dir, "info")
	info_dic = {}
	content_dic = {}

	f = open(info, 'r')
	content = f.read()
	info_dic = json.loads(content)

	node = {index:filename}
	if imagename in info_dic["images"]:
		info_dic["images"][imagename].append([index, filename])
	else:
		info_dic["images"][imagename] =  [[index, filename]]

	info_str = json.dumps(info_dic)
	f = open(info, 'w')
	f.write(info_str)
	f.close()

def ckpt_save_steps(chpt_dir, cpuname, steps):
	info = os.path.join(chpt_dir, "info")
	info_dic = {}
	content_dic = {}

	f = open(info, 'r')
	content = f.read()
	info_dic = json.loads(content)

	info_dic["steps"][cpuname] = steps
	info_str = json.dumps(info_dic)
	f = open(info, 'w')
	f.write(info_str)
	f.close()

def ckpt_get_steps(chpt_dir, cpuname):
	info = os.path.join(chpt_dir, "info")
	info_dic = {}
	content_dic = {}

	f = open(info, 'r')
	content = f.read()
	info_dic = json.loads(content)

	steps_dic = info_dic["steps"]
	if cpuname in  steps_dic:
		return steps_dic[cpuname]
	else:
		raise ValueError("No steps for %s in ckpt info file" % cpuname)



def ckpt_get_image_info(chpt_dir):
	info = os.path.join(chpt_dir, "info")
	info_dic = {}
	info_dic = {}
	content_dic = {}

	f = open(info, "r")
	content = f.read()
	f.close()
	info_dic = json.loads(content)

	content_dic = info_dic["images"]

	return content_dic

def start_ckpt_config(ckpt_dir):
	json_file = None
	script_file = None
	for parent, dirnames, filenames in os.walk(ckpt_dir):
		for filename in filenames:
			if filename.endswith(".json"):
				json_file = filename
			if filename.endswith(".skyeye"):
				script_file = filename
	if script_file != None:
		cmd = cli.GetCommand("define-conf")
		if cmd == None:
			raise IOError("Can not find command: define-conf")
		cmd.run([json_file])
		cmd = cli.GetCommand("init-ok")
		if cmd == None:
			raise IOError("Can not find command: init-ok")
		cmd.run([])
		SetGlobalScript(os.path.basename(script_file))
	else:
		raise IOError("None .skyeye file in %s dir" % ckpt_dir)

class JsonFormatter:
	def __init__(self,intend=4,stream=""):
		self.intend=intend
		self.stack=[]
		self.obj=None
		self.source=stream
		self.prepare()

	@staticmethod
	def json_str(s):
		return '"'+s+'"'

	@staticmethod
	def get_source(name):
		with open(name,'r') as f:
			return ''.join(f.read().split())

	def prepare(self):
		try:
			self.obj=json.loads(self.source)#, object_hook=_decode_dict)
		except:
			raise Exception('Invalid json string!')

	def line_intend(self,level=0):
		return '\n'+' '*self.intend*level

	def parse_dict(self,obj=None,intend_level=0):
		self.stack.append(self.line_intend(intend_level)+'{')
		intend_level+=1
		for key,value in obj.items():
			key=self.json_str(str(key))
			self.stack.append(self.line_intend(intend_level)+key+':')
			self.parse(value,intend_level)
			self.stack.append(',')
		self.stack.append(self.line_intend(intend_level-1)+'}')

	def parse_list(self,obj=None,intend_level=0):
		self.stack.append(self.line_intend(intend_level)+'[')
		intend_level+=1
		for item in obj:
			self.parse(item,intend_level)
			self.stack.append(',')
		self.stack.append(self.line_intend(intend_level-1)+']')

	def parse(self,obj,intend_level=0):
		if obj is None:
			self.stack.append('null')
		elif obj is True:
			self.stack.append('true')
		elif obj is False:
			self.stack.append('false')
		elif isinstance(obj,(int,float)):
			self.stack.append(str(obj))
		elif isinstance(obj,str):
			self.stack.append(self.json_str(obj))
		elif isinstance(obj,(list,tuple)):
			self.parse_list(obj,intend_level)
		elif isinstance(obj,dict):
			self.parse_dict(obj,intend_level)
		else:
			raise Exception('Invalid json type %s!' % obj)

	def render(self):
		self.parse(self.obj,0)
		res=''.join(self.stack)
		return res

def get_executed_pc_file(instr_process, fname):
	SkyEyeGetExecutedPcFile(instr_process, fname)

def is_elf(data):
	elf_header=chr(0x7f)+'ELF'
	elf_header = bytes(elf_header,encoding='utf-8')
	if(operator.eq(data,elf_header)!=True):
		return False
	return True

def is_coff(data):
	coff_header=[0xc1,0xc2]
	if data[0]!=coff_header[0] and data[0]!=coff_header[1]:
		return False
	return True

def is_hex(data):
	hex_header=[0x3a]
	if data[0]!=hex_header[0]:
		return False
	return True

def get_binary_file_type(filename):
	fp=open(filename,"rb")
	data=fp.read(4)
	fp.close()
	if (is_elf(data)):
		return "elf"
	elif(is_coff(data)):
		return "coff"
	elif(is_hex(data)):
		return "hex"
	else:
		return "other"

def has_lave(lave):
	if lave:
		return 1
	else:
		return 0

def addrs2lines(cpu, binary, addrs):
	try:
		archname = SkyEyeGetArchitecture(cpu)
	except:
		raise
	if archname == "tic6x":
		archname = archname+"_"+get_binary_file_type(binary)
	arch_dir =  os.path.join(se_path.ArchToolsDir, archname)
	if os.path.exists(arch_dir):
		addr2line_cmd = os.path.join(arch_dir, 'addr2line')
	else:
		raise Exception('No arch tools addr2line for %s' % archname)
	addr_parameters = ''
	for addr in addrs:
		addr_parameters = ''.join([addr_parameters, ' ' , addr])
	all_list = []
	cmd = ''.join([addr2line_cmd,' -f -e ', ' "',binary,'" ' ,addr_parameters])
	pro = subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	func_file_str = pro.stdout.decode()
	func_file_list = func_file_str.split('\n')
	func_file_list.remove('')
	for i in range(0, len(func_file_list), 2):
		item = []
		item.append(addrs[i//2])
		item.append(func_file_list[i])
		item.append(func_file_list[i+1])
		all_list.append(item)
	return all_list


def pccache_to_instrcache(cpu, binary, pc_f, instr_f, offset):
	bank_size = 700
	lines = []
	addr_dic = {}
	addr_list = []
	with open(pc_f, 'r') as f:
		lines = f.readlines()
	for line in lines:
		addr_count = line.strip('\n').split(':')
		addr = hex(int(addr_count[0],16) - offset).strip('L')
		count = addr_count[1]
		addr_dic[addr] = count
		addr_list.append(addr)

	lens = len(addr_list)
	bank_count = lens // bank_size
	lave = lens % bank_size
	pc_source_list = []
	for i in range(0, bank_count):
		once_op = addr_list[i * bank_size:i*bank_size + bank_size]
		items = addrs2lines(cpu, binary, once_op)
		pc_source_list.extend(items)
	if(lave):
		once_op = addr_list[bank_size * bank_count:bank_size *bank_count + lave]
		items = addrs2lines(cpu, binary, once_op)
		pc_source_list.extend(items)
	instr_lines = []
	for item in pc_source_list:
		instr_lines.append(''.join([hex(int(item[0],16) + offset).strip('L'), ':', item[1], ':', item[2], ':', addr_dic[item[0]],'\n']))
	with open(instr_f, 'w') as f:
		f.writelines(instr_lines)


def get_arch_cmd(cpu, binary, name):
	try:
		archname = SkyEyeGetArchitecture(cpu)
	except:
		raise
	if archname == "tic6x":
		archname = archname+"_"+get_binary_file_type(binary)
	arch_dir =  os.path.join(se_path.ArchToolsDir, archname)
	if os.path.exists(arch_dir):
		objdump_cmd = os.path.join(arch_dir, name)
	else:
		raise Exception('No arch tools %s for %s'%(name, archname))
	return objdump_cmd

def get_source_root_dir(cpu, binary):
	objdump_cmd = get_arch_cmd(cpu, binary, 'objdump')
	cmd = ''.join([objdump_cmd,' -l ', ' -d ','"', binary,'"'])
	pro = subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	file_str = pro.stdout.decode()
	lines = file_str.split('\n')
	source_lines = []
	for line in lines:
		if 'Debug' in line:
			source_lines.append(line.split('Debug')[0])
	if len(source_lines) == 0:
		return None
	else:
		return 	source_lines[0]

def get_source_root_dir_level(cpu, binary):
	root = get_source_root_dir(cpu, binary)
	if root:
		path_items = root.split('\\')
		path_items.remove('')
		return len(path_items)
	else:
		raise Exception('Get source dir root fail')

def check_binary_have_text_section(objdump_cmd, binary):
	cmd = [objdump_cmd, '-h', binary]
	process = subprocess.run(cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
	out = process.stdout.decode().split("\n")
	for info in out:
		if re.match(r"^\s*([ 0-9]{3})\s(.text)\s*.*$", info) != None:
			return True
	return False

def get_objdump_file(cpu, binary, file_path, source_dir, is_block = True):
	try:
		archname = SkyEyeGetArchitecture(cpu)
		objdump_cmd = get_arch_cmd(cpu, binary, 'objdump')
		#level = get_source_root_dir_level(cpu, binary)
	except:
		raise
	#cmd = ''.join([objdump_cmd,' -S ','"', binary,'"', ' --prefix=%s  --prefix-strip=%d ' % (source_dir.encode("gbk"), level), ' > ', file_path.encode("gbk")])
	if source_dir == '':
		cmd = [objdump_cmd, '-D', '-C', binary]
	elif archname == "tic55x":
		#cmd = ''.join([objdump_cmd,' -S -C -l ','"', binary,'"', ' > ', file_path.encode("gbk")])
		cmd = [objdump_cmd, '-S', '-C', '-l', binary]
	else:
		#cmd = ''.join([objdump_cmd,' -S -C -l ','"', binary,'"', ' -I "', source_dir.encode("gbk"), '" > ', file_path.encode("gbk")])
		cmd = [objdump_cmd, '-S', '-C' , '-l', binary, '-I', source_dir]
	if check_binary_have_text_section(objdump_cmd, binary) == True:
		cmd.insert(3, '--section=.text')
	if is_block:
		with open(file_path, "w") as fp:
			process_pool.append(subprocess.Popen(cmd,stdout=fp,stderr=subprocess.PIPE))
			process_pool[-1].wait()
			process_pool[-1].terminate()
		if process_pool[-1].returncode == 0:
			return True
		os.remove(file_path)
		return False
	else:
		child = subprocess.Popen(cmd,shell = True,bufsize = 0)
		return child

def get_bin_file(cpu, binary, file_path):
	try:
		objcopy_cmd = get_arch_cmd(cpu, binary, 'objcopy')
	except:
		raise
	cmd = ''.join([objcopy_cmd,' -O binary ','"', binary, '"', ' "' , file_path,'"'])
	os.system(cmd)

def get_objcopy_file(cpu, binary, file_path, offset):
	try:
		objcopy_cmd = get_arch_cmd(cpu, binary, 'objcopy')
	except:
		raise
	cmd = ''.join([objcopy_cmd,' --change-section-address .text-',offset,' "', binary, '"', ' "' , file_path,'"'])
	os.system(cmd)

def execute_skyeyeinit_file(fname):
	cmd = cli.GetCommand("run-script")
	initfile = os.path.join(os.getcwd(),fname)
	if cmd == None:
		print ("Can not find command: run-script")
		return
	if cmd.run([fname])==False:
		return False
	print ("Reading commands from %s...done." % initfile)
	SetGlobalScript(fname)

def kill_all_process():
	for process in process_pool:
		if process.poll() == None:
			process.kill()

def _decode_list(data):
	rv = []
	for item in data:
		if isinstance(item, str):
			item = item
		elif isinstance(item, list):
			item = _decode_list(item)
		elif isinstance(item, dict):
			item = _decode_dict(item)
		rv.append(item)
	return rv

def _decode_dict(data):
	rv = {}
	for key, value in data.items():
		if isinstance(key, str):
			key = key
		if isinstance(value, str):
			value = value
		elif isinstance(value, list):
			value = _decode_list(value)
		elif isinstance(value, dict):
			value = _decode_dict(value)
		rv[key] = value
	return rv

class local_cache:
	def __init__(self, cache):
		self.cache = cache

	def __setitem__(self, key, value):
		try:
			with closing(shelve.open(self.cache, 'c', protocol = 0)) as shelf:
				shelf[key] = value
			return True
		except:
			return False 
	
	def __getitem__(self, key):
		with closing(shelve.open(self.cache, 'c', protocol = 0)) as shelf:
			return shelf.get(key)

	def remove(self, key):
		try:
			with closing(shelve.open(self.cache, 'c', protocol = 0)) as shelf:
				del shelf[key]
				return True
		except:
			return False
