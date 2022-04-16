from skyeye_common_module import *

class regs(object):
	def __init__(self, obj, did, rid):
		self.name =  SkyEyeX86GetRegName(obj, did, rid)
		self.value =  SkyEyeX86GetRegValue(obj, did, rid)
		self.addr =  SkyEyeX86GetRegAddr(obj, did, rid)
		self.ID = rid

class x86_device(object):
	def __init__(self, obj, did):
		self.regs = []
		self.name = SkyEyeX86GetDeviceName(obj, did)
		self.width = SkyEyeX86GetDeviceRegWidth(obj, did)
		self.ID = did
		num = SkyEyeX86GetDeviceRegNum(obj, did)
		for i in range(0, num):
			self.regs.append(regs(obj, did, i))
		def get_reg(self, name):
			for reg in self.regs:
				if reg.name == name:
					return reg
			return None

class x86(object):
	def __init__(self, obj):
		self.name = obj
		self.devices  = []
		num = SkyEyeX86GetDeviceNum(obj)
		for i in range(0, num):
			self.devices.append(x86_device(obj, i))
	def get_device(self, name):
		for device in self.devices:
			if device.name == name:
				return device
		return None
	def get_id_device(self, did):
		return self.devices[did]


