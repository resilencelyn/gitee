import grpc_server
import tools
import views
import csv
import os
import time
import se_path
import traceback
import json


undef_result = {"ack":{"name":"undef"}}
def json_err_ack(mesg):
	return {'ack':{'json_err':{'result':{'mesg':mesg}}}}


server = None
log_out = None
is_cancel = False

class log_output:
	def __init__(self):
		self.csv_file = os.path.join(se_path.LogPath, "grpc_log.csv")
		try:
			with open(self.csv_file, "w") as fp:
				writer = csv.writer(fp)
				writer.writerow(["func_name","type","time","info"])
				fp.close()
		except:
			print ("Failed to open the 'grpc_log.csv' file")

	def write_log(self, log):
		try:
			with open(self.csv_file, "a") as fp:
				writer = csv.writer(fp)
				writer.writerow(log)
				fp.close()
		except:
			print ("Failed to open the 'grpc_log.csv' file")

def log(log_type, subject, mesg):
	timer = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
	log_out.write_log([subject,log_type,timer,mesg])

def return_ack(dic):
	json_str = json.dumps(dic)
	return json_str

def server_callback(request):
	try:
		req_dic = json.loads(request)
		log("request", req_dic["request"]["name"], request)
	except Exception as e:
		dic = json_err_ack(e)
		log("request","NULL", request)
		return  return_ack(dic)

	cmdname =  req_dic['request']["name"]
	if cmdname in views.cmd_patterns:
		try:
			dic = views.cmd_patterns[cmdname](req_dic["request"])
		except Exception as e:
			dic ={'ack': {'error': "Command execution error\n%s"%traceback.format_exc(), 'name': cmdname, 'result': None}}
	else:
		dic = undef_result
	if is_cancel == True:
		dic ={'ack': {'error': "Cancel function run", 'name': cmdname, 'result': None}}
	try:
		temp=json.dumps(dic)
	except Exception as e:
		dic = {'ack': {'error': "Conversion message error %s"%traceback.format_exc(), 'name': cmdname, 'result': None}}
		temp=json.dumps(dic)
	log("ack", dic["ack"]["name"], temp)
	return  return_ack(dic)

def cancel_callback():
	global is_cancel
	#import coverage
	is_cancel = True
	#coverage.set_is_cancel(True)
	tools.kill_all_process()

def server_init():
	global server
	global log_out
	log_out = log_output()
	if not server:
		grpc_server.set_callback(server_callback, cancel_callback)
		server = grpc_server.ControlServer()
		server.start()

def server_stop():
	global server
	if server:
		server.stop(None)
		server = None
