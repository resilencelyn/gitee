#coding:utf-8
from skyeye_common_module import *

class attr():
	def __init__(self, objname, attr_type, name):
		self.objname = objname
		self.attr_type = attr_type
		self.name = name
	def get(self):
		print (self.objname)
		print (self.attr_type)
		print (self.name)
	def set(self, value):
		SkyEyeSetAttr(self.objname, self.name, self.attr_type, value)

class module():
	def __init__(self,objname,dic):
		self.objname = objname
		self.clsname = dic['cls']
		self.attr_list = dic['attr_list']
		self.iface_list = dic['iface_list']

	def set_attr(self, attrname, attr_value):
		for attr in self.attr_list:
			if attr.name == attrname:
				return attr.set(attr_value)
		return None
	def get_attr(self, attrname):
		for attr in self.attr_list:
			if attr.name == attrname:
				return attr.get()
		return None
	def connect(self,connect_obj,intf_name):
		if intf_name in self.iface_list:
			SkyEyeConnect(connect_obj, self.objname, intf_name,0)	
		return None

def pre_conf_obj(objname, cls):
	obj=SkyEyePreConfObj(objname,cls)
	if obj==0:
		return 0
	attrname_list=SkyEyeGetClassAttrList(obj.class_name)
	iface_list=SkyEyeGetClassIfaceList(obj.class_name)
	attr_list=[]
	for attrname in attrname_list:
		info_l = SkyEyeGetClassAttrInfo(obj.class_name,attrname)
		attr_list.append(attr(objname,info_l[1],info_l[0]))
	dic = {"cls":cls, 'attr_list':attr_list,'iface_list':iface_list}
	obj = module(obj.objname,dic)
	return obj
