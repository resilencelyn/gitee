import skyeye_common_module as scm


def error_addr_to_right(instr_len,addr):
	while(addr % instr_len != 0):
		addr += 1
	return addr

def get_disassemble_info(cpuname,addr,length):
	try:
		width=scm.SkyEyeGetAddressWidth(cpuname)
	except:
		raise
	addr=error_addr_to_right(width,addr)
	dis_list=[]
	for i in range(0,length):
		temp=addr+width*i
		try:
			info=scm.SkyEyeDisassemble(cpuname,temp)
		except:
			info='unknown'
		if temp > 0xffffffff:
			temp=""
		func="?"
		dis_list.append([temp,func,info])
	return dis_list

def get_current_pc(cpuname):
	try:
		return scm.SkyEyeGetPcByCoreName(cpuname)
	except:
		raise

def sys_step(cpuname):
	try:
		return scm.SkyEyeStepi(cpuname,'1')
	except:
		raise

def create_breakpoint(cpuname,addr):
	try:
		return scm.SkyEyeCreateBreakpoint(cpuname,addr)
	except:
		raise

def delete_breakpoint(cpuname,addr):
	try:
		return scm.SkyEyeDeleteBreakpointByAddr(cpuname,addr)
	except:
		raise
