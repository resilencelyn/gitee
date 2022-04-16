import skyeye_common_module as scm
import fault_inject as fi
import time

def set_reg_value_by_addr(machname,addr,value):
	addr = fi.check_value(addr)
	devname = fi.get_devname_by_addr(machname,addr)
	if devname == None:
		print ("cannot found dev")
		return False
	reg_info = fi.get_device_register_info(machname,devname)
	if reg_info == False:
		return False
	regname = fi.get_regname_by_addr(machname,devname,addr)
	if regname == None:
		return False
	try:
		regid = scm.SkyEyeGetDevRegIdByName(machname, devname, regname)
		scm.SkyEyeSetDevRegValueById(machname, devname, value, regid)
	except:
		print ("set_reg_value_by_addr call error!")
		return False
	return True

def get_reg_value_by_addr(machname,addr):
	addr = fi.check_value(addr)
	devname = fi.get_devname_by_addr(machname,addr)
	if devname == None:
		print ("cannot found dev")
		return False
	reg_info = fi.get_device_register_info(machname,devname)
	if reg_info == False:
		return False
	regname = fi.get_regname_by_addr(machname,devname,addr)
	if regname == None:
		return False
	try:
		regid = scm.SkyEyeGetDevRegIdByName(machname, devname, regname)
		return scm.SkyEyeGetDevRegValueById(machname, devname, regid)
	except:
		print ("get_reg_value_by_addr call error!")
		return None
	
def run_to_time(cpu, time_s):
	while(scm.SkyEyeGetSimulationRunTime(cpu) < time_s):
		time.sleep(0.00001)
	scm.SkyEyeStop()