#coding:utf-8
import os
import sys
import shutil
import se_path
import skyeye_common_module as scm
import errormessage

def get_license_path():
	lic_path = se_path.LicPath
	if not os.path.isdir(lic_path):
		os.makedirs(lic_path)
	return lic_path

def write_config_file(mode,args):
	lic_path=get_license_path()
	conf_file=os.path.join(lic_path,"lic_conf.dat")
	data=""
	if mode=="local":
		data="mode:local\nfile:%s"%args[0]
	elif mode=="network" and len(args)==2:
		data="mode:network\nip:%s\nport:%s"%(args[0],args[1])
	elif mode=="usb":
		data="mode:dog"
	else:
		pass
	fp=open(conf_file,"w")
	fp.write(data)
	fp.close()

def clear_license_config():
	lic_path=get_license_path()
	conf_file=os.path.join(lic_path,"lic_conf.dat")
	if os.path.isfile(conf_file):
		os.remove(conf_file)
	
def set_local_license(file_path):
	lic_path=get_license_path()
	if not os.path.isfile(file_path):
		return (False, "%s is not a file"%file_path)
	file_name=os.path.split(file_path)[1]
	try:
		shutil.copyfile(file_path,os.path.join(lic_path,file_name))
	except Exception:
		raise
	write_config_file("local",[file_name])
	return (True,)

def set_network_license(ip,port):
	write_config_file("network",[ip,port])
	return True

def set_usb_license():
	write_config_file("usb",[])
	return True

def license_verify():
	succ_id = [3, 16, 18]
	try:
		ret = scm.SkyEyeLicenseVerify()
	except:
		ret = 31
	errormessage.SetErrorInfo(0x40252500 + ret, False)
	message = errormessage.GetErrorInfo()[0]
	if ret in succ_id:
		return (True, message)
	return (False, message)

def check_usb_connect():
	ret = scm.SkyEyeCheckUsbLicConnect()
	return ret

def read_license_info(filename):
	lic_info={}
	ret=scm.SkyEyeReadLicenseInfo(filename)
	if ret==None:
		return None
	else:
		start_index=ret.find("START:")
		end_index=ret.find("END:")
		cpuid_index=ret.find("CPUID:")
		mac_index=ret.find("MAC:")
		cpus_index=ret.find("CPUS:")
		num_index=ret.find("NUM:")
		if -1 in [start_index,end_index,cpuid_index,mac_index,cpus_index,num_index]:
			return None
		lic_info["start"]=ret[start_index+6:end_index]
		lic_info["end"]=ret[end_index+4:cpuid_index]
		lic_info["mac_list"]=ret[mac_index+4:cpus_index]
		lic_info["cpu_list"]=ret[cpus_index+5:num_index]
		lic_info["num"]=ret[num_index+4:len(ret)]
		return lic_info


class gui_get_license_info():
	def __init__(self):
		self.lic_mode = None
		self.lic_file = None
		self.server_ip = None
		self.server_port = None
		self.lic_info = None
		info = {}
		lic_path = get_license_path()
		conf_file = os.path.join(lic_path,"lic_conf.dat")
		try:
			with open(conf_file, "r") as fp:
				fi = fp.readlines()
				for i in fi:
					temp = i.split(":")
					info[temp[0].strip()] = temp[1].strip()
			self.lic_mode = info.get("mode")
			self.lic_file = info.get("file")
			self.server_ip = info.get("ip")
			self.server_port = info.get("port")
		except:
			pass
		if self.lic_mode == "local":
			self.lic_info = read_license_info(os.path.join(lic_path, self.lic_file))

	def get_license_mode(self):
		v_type = "本地许可"
		if self.lic_mode == "network":
			v_type = "网络许可"
		if self.lic_mode == "usb":
			v_type = "USB Key"
		return v_type
	def get_license_time(self):
		if self.lic_info != None:
			return "%s-%s"%(self.lic_info["start"],self.lic_info["end"])
		elif self.lic_mode == "network":
			return "许可信息请查看License服务器"
		elif self.lic_mode == "dog":
			return "请联系供应商"
		else:
			return ""
	def get_license_cpus(self):
		if self.lic_info != None:
			temp = self.lic_info["cpu_list"].replace("\n","").split(":")
			return temp
		elif self.lic_mode == "network":
			return ["许可信息请查看License服务器"]
		elif self.lic_mode == "dog":
			return ["请联系供应商"]
		else:
			return []

	def get_info(self):
		if self.lic_mode == "local":
			return [self.lic_mode, self.get_license_time(), self.get_license_cpus()]
		elif self.lic_mode == "network":
			return [self.lic_mode, self.server_ip, self.server_port]
		else:
			return [None, None, None]

def get_mac_info():
	sys.coinit_flags = 0
	import wmi
	import pythoncom
	mac_addr_list = []
	pythoncom.CoInitialize ()
	c = wmi.WMI ()
	for mac in c.Win32_NetworkAdapter():
		mac_addr = mac.MACAddress
		name = mac.name
		if mac_addr != None \
			and "VMware" not in name \
				and "Virtual" not in name \
					and "Bluetooth" not in name \
						and "WAN Miniport" not in name:
			mac_addr_list.append((name, mac_addr.replace(":","")))
	pythoncom.CoUninitialize ()
	return mac_addr_list

def get_cpuid_info():
	try:
		return scm.SkyEyeGetCpuId()
	except:
		return None

def create_qrcode(cpuid, mac_addr):
	import qrcode
	import base64
	encrypt_str = None
	checksum = 0
	encrypt_str = "@%s@%s@"%(mac_addr, cpuid)
	for s in encrypt_str:
		checksum += ord(s)
	encrypt_str += "%d@"%checksum
	encrypt_str = base64.encodestring(encrypt_str.encode())
	img_name = os.path.join(get_license_path(),"fingerprint.png")
	img = qrcode.make(encrypt_str.decode())
	with open(img_name, 'wb') as f:
		img.save(f)
	return [encrypt_str.decode().replace("\n","\\n"), img_name]

def get_hardware_info(mac_addr = None):
	cpu_id = get_cpuid_info()
	if mac_addr == None:
		mac_addr_list = get_mac_info()
		if len(mac_addr_list) == 0:
			return [False, "获取硬件指纹信息失败：Network"]
		mac_addr = mac_addr_list[0][1]
	if cpu_id == None:
		return [False, "获取硬件指纹信息失败：CPU"]
	qrcode_info = create_qrcode(cpu_id, mac_addr)
	return [True] + qrcode_info