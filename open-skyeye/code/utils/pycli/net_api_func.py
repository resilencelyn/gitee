import skyeye_common_module as scm
import json
import os
import se_path
import tools
import win_workspace as ws
import collections
import operator

def get_soc_all_class_info(soc_info):
	attr_list=[]
	attr_info_list=[]
	iface_list=[]
	conn_list=[]
	for mach in soc_info.keys():
		for dev in soc_info[mach].keys():
			classname=soc_info[mach][dev]['class']
			try:
				attr_l = scm.SkyEyeGetClassAttrList(classname)
				attr_l.sort()
			except:
				attr_l = []
			for attr in attr_l:
				try:
					attr_info=scm.SkyEyeGetClassAttrInfo(classname, attr)
				except:
					attr_info=[]
				attr_info[0]=dev+"@@@"+attr_info[0]
				attr_info_list.append(attr_info)
			for i in range(0,len(attr_l)):
				attr_l[i]=dev+"@@@"+attr_l[i]
			attr_list=attr_list+attr_l
			try:
				iface_l = scm.SkyEyeGetClassIfaceList(classname)
				iface_l.sort()
			except:
				iface_l = []
			for i in range(0,len(iface_l)):
				iface_l[i]=dev+"@@@"+iface_l[i]
			iface_list=iface_list+iface_l
			try:
				conn_l = scm.SkyEyeGetClassConnectList(classname)
				conn_l.sort()
			except:
				conn_l = []
			for i in range(0,len(conn_l)):
				conn_l[i]=dev+"@@@"+conn_l[i]
			conn_list = conn_list+conn_l
	return [attr_list,attr_info_list,iface_list,conn_list]

def get_all_class_info():
	class_info={}
	try:
		class_list = scm.SkyEyeGetClassList()
	except Exception as e:
		return (False,e)
	for classname in class_list:
		in_ws = False
		try:
			classtype = scm.SkyEyeGetClassType(classname)
		except:
			classtype = "NULL"
		try:
			attr_l = scm.SkyEyeGetClassAttrList(classname)
			attr_l.sort()
		except:
			attr_l=[]
		attr_info_list=[]
		for attr in attr_l:
			try:
				attr_info=scm.SkyEyeGetClassAttrInfo(classname, attr)
			except:
				attr_info=[]
			attr_info_list.append(attr_info)
		try:
			iface_l = scm.SkyEyeGetClassIfaceList(classname)
			iface_l.sort()
		except:
			iface_l=[]
		try:
			connect_l = scm.SkyEyeGetClassConnectList(classname)
			connect_l.sort()
		except:
			connect_l=[]
		if classtype=="device":
			moduletype="OffChip"
			try:
				ret=scm.SkyEyeGetDeviceModuleType(classname)
				if ret==1:
					moduletype="OnChip"
			except:
				pass
		else:
			moduletype="NULL"
		if classname in ws.ws_module_list:
			in_ws = True
		class_info[classname]={"class_type":classtype,
				"module_type":moduletype,
				"in_ws":in_ws,
				"attr_list":attr_l,
				"iface_list":iface_l,
				"connect_list":connect_l,
				"attr_info_list":attr_info_list}
	return (True,class_info)

def get_version_info():
	info={}
	package_config = tools.GetPackageConfig()
	info["build_date"] = tools.GetBuildDate(package_config)
	info["build_version"] = tools.GetBuildVersion(package_config)
	info["build_commit"] = tools.GetCommitId(package_config)
	return info
