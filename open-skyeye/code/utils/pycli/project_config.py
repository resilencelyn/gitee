import os
import json
import hashlib
import tools

script_path = "."
prj_path = '.prjConfig'
config_file_name = "config"
conf_cache = None


def set_script_path(path):
	global script_path
	global prj_path
	global conf_cache
	script_path = os.path.split(path)[0]
	prj_path = os.path.join(script_path, prj_path)
	if not os.path.exists(prj_path):
		os.makedirs(prj_path)
	conf_cache = tools.local_cache(os.path.join(prj_path, config_file_name))

def read_conf(key):
	if conf_cache != None:
		return conf_cache[key]
	return None

def write_conf(key, value):
	if conf_cache != None:
		conf_cache[key] = value
	else:
		print ("can not set script_path")

def cmp_md5(file_name):
	md5 = get_file_md5(file_name)
	file_name = os.path.split(file_name)[1]
	if md5 != None and read_conf(file_name) == md5:
		return True
	else:
		return False

def write_file_md5(file_name):
	md5 = get_file_md5(file_name)
	file_name = os.path.split(file_name)[1]
	if md5 != None:
		write_conf(file_name, md5)

def get_file_md5(file_name):
	if os.path.exists(file_name) == False:
		return None
	try:
		hs = hashlib.md5()
		with open(file_name, "rb") as fp:
			res = fp.read()
			hs.update(res)
		return hs.hexdigest()
	except:
		return None

def file_bind(file1_name,file2_name):
	info = read_conf("FILE_BIND")
	if info == None:
		info = {}
	file1_md5 = get_file_md5(file1_name)
	file2_md5 = get_file_md5(file2_name)
	file1_name = os.path.split(file1_name)[1]
	info[file1_name] = [file1_md5,file2_md5]
	write_conf("FILE_BIND",info)

def check_bind(file1_name,file2_name):
	info = read_conf("FILE_BIND")
	if info == None:
		return False
	file1_md5 = get_file_md5(file1_name)
	file2_md5 = get_file_md5(file2_name)
	file1_name = os.path.split(file1_name)[1]
	md5 = info.get(file1_name)
	if md5 == None or \
		md5[0] != file1_md5 or\
		md5[1] != file2_md5:
		return False
	return True