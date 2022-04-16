import os,sys,time
import cli
from conf import *
from skyeye_common_module import *
import se_system as ss
import threading
from fuzzywuzzy import fuzz
import fault_inject as fi
import se_func as sf
import skyeye_autotest_command as sac
import pytimer

MO_READ = 1
MO_WRITE = 2

OP_CHAR_TYPE  = 1
OP_SHORT_TYPE = 2
OP_WORD_TYPE  = 4
OP_DWORD_TYPE = 8

TY_INT = "int"
TY_FLOAT = "float"
TY_DOUBLE = "double"

def SE_define_conf(filename):
	try:
		Config = json_conf(parent = None, filename = filename)
		if Config.get_init_result() == False:
			print ("load_config error")
			return False
		Config.instance()
		SetGlobalConfig(Config)
		define_json_file[0] = os.path.join(os.getcwd(), filename)
	except:
		print ("load_config error")
		return  False

def SE_load_binary(cpuname,filename):
	if os.path.isfile(filename)==False:
		print ("No such binary file")
		return False
	if SkyEyeLoadBinary(cpuname,filename)!=1:
		print ("load_binary error")
		return False
	binary_l[cpuname] = os.path.join(os.getcwd(), filename)

def SE_init_ok():
	SkyEyePrepareToRun()
	if ss.system ==None:
		ss.CreateClass()

def SE_run_script(filename):
	fullfilename = os.path.realpath(filename)
	if not os.path.exists(fullfilename):
		print ("%s file does not exists!" % fullfilename)
		return False
	fp = open(fullfilename,'r')
	lines = fp.readlines()
	fp.close()
	cmd = cli.GetCommand("run-command")
	if cmd == None:
		SE_print("Can not find command: run-command")
		return False
	for line in lines:
		line = line.strip(' \n')
		if line == "" or line[0] == '#' or "run-pyfile" in line:
			continue
		try:
			ret=cmd.run(line)
		except Exception as e:
			SE_print(e)
			return False
	return True

def SE_run():
	pytimer.set_running(True)
	SkyEyeRun()

def SE_stop():
	pytimer.set_running(False)
	SkyEyeStop()

def SE_stop_autotest():
	if sac.ac != None:
		sac.ac.autotest_run = False

def SE_reset():
	SkyEyeReset()

def SE_restart():
	SE_reset()
	SE_define_conf(define_json_file[0])
	for binary in binary_l.keys():
		SE_load_binary(binary,os.path.normpath(binary_l[binary]))
	SE_init_ok()

def SE_running_status():
	return SkyEyeRunningStatus()

def SE_pre_conf_obj(classname, objname):
	SkyEyePreConfObj(objname,classname)
	obj = ss.SkyEyeBaseClass(objname,classname,None)
	return obj

def SE_sleep(s):
	time.sleep(s)

log=None

def SE_print(data):
	global log
	if log == None:
		return
	r_obj=ss.__redirection__()
	r_obj.set_out()
	print (data)
	r_obj.reset()
	log.WriteText(r_obj.buff)

def SE_compare(actual_out,expect_out,accuracy):
	sj_output=actual_out
	try:
		fp1=open(sj_output,'r')
	except:
		SE_print('The actual output file was not found')
		return
	actual=fp1.read()
	fp1.close()
	rmlist=['\t','\r',' ','\n']
	for i in rmlist:
		actual=actual.replace(i,'')
		expect_out=expect_out.replace(i,'')
	if len(actual)==0:
		SE_print('The actual output file is empty')
		return
	if len(expect_out)==0:
		SE_print('The desired output is empty')
		return
	accuracy_list=[]
	accuracy_list.append(fuzz.ratio(actual,expect_out))
	accuracy_list.append(fuzz.partial_ratio(actual,expect_out))
	if accuracy!=100:
		accuracy_list.append(fuzz.token_set_ratio(actual,expect_out))
	max_accuracy = max(accuracy_list)
	if max_accuracy >= accuracy:
		SE_print("Proofreading success")
		return True
	else:
		SE_print('Check failure. expect accuracy: %d%%  max actual accuracy: %d%%'%(accuracy,max_accuracy))
		return False

def SE_set_fault_inject(machname,addr,bit,mode):
	return fi.skyeye_set_fj_by_aadr(machname,addr,bit,mode)

def SE_get_fault_inject():
	return fi.skyeye_get_fj()

def SE_clear_fault_injece(machname,addr,bit,mode):
	return fi.skyeye_clear_fj_by_addr(machname,addr,bit,mode)

def SE_set_register_value(machname,addr,value):
	return sf.set_reg_value_by_addr(machname,addr,value)

def SE_get_register_value(machname,addr):
	return sf.get_reg_value_by_addr(machname,addr)

def SE_get_simulation_time(cpuname):
	return SkyEyeGetSimulationRunTime(cpuname)

def SE_enable_parse_symbol(cpuname,binary):
	cmd = cli.GetCommand("parse-symbol")
	if cmd == None:
		SE_print("Can not find command: parse-symbol")
		return False
	try:
		ret=cmd.run([cpuname,binary])
	except Exception as e:
		SE_print(e)
		return False
	return ret

def SE_get_global_variable_addr(cpuname,varname):
	return SkyEyeGetSymbolAddr(cpuname,varname)

def SE_get_global_variable_value(cpuname,varname,value_bytes_number,value_type):
	if value_type == TY_INT:
		return SkyEyeGetSymbolValue(cpuname,varname,value_bytes_number)
	elif value_type == TY_FLOAT:
		return SkyEyeGetFloatSymbolValue(cpuname,varname,value_bytes_number)
	elif value_type == TY_DOUBLE:
		return SkyEyeGetDoubleSymbolValue(cpuname,varname,value_bytes_number)
	else:
		pass

def SE_set_global_variable_value(cpuname,varname,value,value_bytes_number,value_type):
	if value_type == TY_INT:
		return SkyEyeSetSymbolValue(cpuname,varname,value,value_bytes_number)
	elif value_type == TY_FLOAT:
		return SkyEyeSetFloatSymbolValue(cpuname,varname,value,value_bytes_number)
	elif value_type == TY_DOUBLE:
		return SkyEyeSetDoubleSymbolValue(cpuname,varname,value,value_bytes_number)
	else:
		pass

def SE_log_output(filename,content):
	now_time = time.strftime('%m%d%H%M%S',time.localtime(time.time()))
	new_filename = filename+"_"+now_time+".txt"
	try:
		fp = open(new_filename,"w")
		fp.write(content)
		fp.close()
	except:
		print ("Write file failed")

def SE_out_error_log(log):
	if sac.ac != None:
		sac.ac.error_test.append([sac.ac.error_reset(sac.ac.test_info),log])

def SE_run_to_time(cpuname,time_s):
	sf.run_to_time(cpuname,time_s)

def SE_create_breakpoint(cpuname,addr):
	try:
		SkyEyeCreateBreakpoint(cpuname,addr)
	except Exception as e:
		SE_print(e)

def SE_get_current_pc(cpuname):
	try:
		return SkyEyeGetPcByCoreName(cpuname)
	except Exception as e:
		SE_print(e)

def SE_get_cpu_freq(cpuname):
	try:
		return SkyEyeGetCpuFreq(cpuname)
	except Exception as e:
		SE_print(e)

def SE_read_byte(cpuname,addr):
	try:
		return SkyEyeReadByte(cpuname,addr)
	except Exception as e:
		SE_print(e)

def SE_read_dword(cpuname,addr):
	try:
		return SkyEyeReadDWord(cpuname,addr)
	except Exception as e:
		SE_print(e)
def SE_write_byte(cpuname,addr,value):
	try:
		return SkyEyeWriteByte(cpuname,addr,value)
	except Exception as e:
		SE_print(e)

def SE_write_dword(cpuname,addr,value):
	try:
		return SkyEyeWriteDWord(cpuname,addr,value)
	except Exception as e:
		SE_print(e)

def SE_sync_callback(cpuname,my_callback,argument):
	return pytimer.PyTmrCreate(cpuname,0,pytimer.TMR_ONE_SHOT,my_callback, argument)

def SE_create_timer(cpuname,time_ms,my_callback,argument):
	return pytimer.PyTmrCreate(cpuname,time_ms,pytimer.TMR_ONE_SHOT,my_callback, argument)

def SE_delete_timer(tmr):
	try:
		return pytimer.PyTmrDelete(tmr)
	except:
		return False

def SE_set_watch_on_pc(cpuname,pc_addr,my_callback,argument):
	return pytimer.PySetWatchOnPc(cpuname,pc_addr,my_callback,argument)

def SE_del_watch_on_pc(watch):
	try:
		return pytimer.PyUnWatchOnPc(watch)
	except:
		return False

def SE_set_watch_on_mem(ms_name,mm_type,addr,data_type,length,my_callback,argument):
	return pytimer.PySetWatchOnMem(ms_name,mm_type,addr,data_type,length,my_callback,argument)

def SE_del_watch_on_mem(watch):
	try:
		return pytimer.PyUnWatchOnMem(watch)
	except:
		return False

def SE_get_func_addr(cpuname,funcname):
	try:
		return SkyEyeGetFuncAddr(cpuname,funcname)
	except Exception as e:
		SE_print(e)

def SE_get_func_length(cpuname,funcname):
	try:
		return SkyEyeGetFuncLength(cpuname,funcname)
	except Exception as e:
		SE_print(e)

def SE_load_file(memoryname, filename, pc_addr):
	if os.path.isfile(filename)==False:
		print ("No such binary file")
		return False
	if SkyEyeNewLoadFile(memoryname, filename, pc_addr) != 1:
		print ("load_file error")
		return False

def SE_term_wait_for_string(termname, string):
	return SkyEyeTermWaitForString(termname, string)

def SE_term_write(termname, string):
	return SkyEyeTermWrite(termname, string)

def SE_term_wait_then_write(termname, wait_string, write_string):
	return SkyEyeTermWaitThenWrite(termname, wait_string, write_string)
