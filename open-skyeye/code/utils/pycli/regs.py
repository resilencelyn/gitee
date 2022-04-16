import skyeye_common_module as scm


def get_registers(devicename):
	reg_list = []
	try:
		num = scm.SkyEyeGetDevRegNum(None, devicename)
	except:
		return None
	for i in range(0, num):
		try:
			name = scm.SkyEyeGetDevRegNameById(None, devicename, i)
			if name == None:
				continue
		except:
			return  None
		try:
			value = scm.SkyEyeGetDevRegValueById(None, devicename, i)
		except:
			return None
		try:
			offset = scm.SkyEyeGetDevRegOffsetById(None, devicename, i)
		except:
			return None
		if offset == 0xffffffff:
			offset = i * 4
		reg_attr_dic = {}
		reg_attr_dic["offset"] = offset
		reg_attr_dic["size"] = 4
		reg_attr_dic["value"] = value
		reg_attr_dic["name"] = name
		reg_attr_dic["desc"] = "NULL"
		reg_attr_dic["id"] = i
		reg_list.append(reg_attr_dic)
	return reg_list
def set_register(devicename, value, reg_id):
	try:
		scm.SkyEyeSetDevRegValueById(None, devicename, value, reg_id)
		return True
	except:
		return False
def get_register(devicename, reg_id):
	try:
		return scm.SkyEyeGetDevRegValueById(None, devicename, reg_id)
	except:
		return None




