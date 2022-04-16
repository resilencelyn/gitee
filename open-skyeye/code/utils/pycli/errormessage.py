# -*- coding: UTF-8 -*-
import os
import json
import se_path
import traceback

Language = "ch"

error_message = ""
quit_flag = False

def SetErrorInfo(id, model, *args):
	global quit_flag
	global error_message
	quit_flag = model
	l_id = hex(id & 0xffff)
	h_id = hex(id - (id & 0xffff))
	try:
		with open(se_path.ErrorDataPath, "r", encoding='utf-8') as fp:
			data = fp.read()
			data = json.loads(data)
			try:
				message = data[h_id][Language]
				detail_exp = data[l_id][Language] % args
				message = "%s\n%s"%(message, detail_exp)
			except Exception as e:
				raise Exception("未找到该ID信息, ID: " + str(e))
	except Exception as e:
		message = "读取错误数据文件出错"
		message = "%s\n%s"%(message, e)
	error_message = message

def C_SetErrorInfo(error_info, model):
	global quit_flag
	global error_message
	quit_flag = model
	idx = error_info[0]
	try:
		with open(se_path.ErrorDataPath, "r", encoding='utf-8') as fp:
			data = fp.read()
			data = json.loads(data)
			try:
				if len(error_info) > 1:
					message = data[idx][Language] % tuple(error_info[1:])
				else:
					message = data[idx][Language]
			except Exception as e:
				raise Exception("未找到该ID信息, ID: " + str(e))
	except Exception as e:
		message = "读取错误数据文件出错"
		message = "%s\n%s"%(message, e)
	error_message = message

def GetErrorInfo():
	global quit_flag
	global error_message
	if error_message == "":
		error_message = "未定义的错误，详细错误信息如下：\n%s"%traceback.format_exc()
	ret_va = (error_message, quit_flag)
	error_message = ''
	quit_flag = False
	return ret_va

''' 读写文件方式
def SetErrorInfo(id, message = "", quit_flag = False):
	error_info = {"ErrorId":id, 
					"DetailExp":str(message), 
					"QuitFlag":quit_flag }
	with open(path.ErrorIdPath, "w") as fp:
		fp.write(json.dumps(error_info))

def GetErrorInfo():
	e_id = 0
	message = ''
	detail = ''
	quit_flag = False
	try:
		id_info = None
		with open(path.ErrorIdPath, "r") as fp:
			id_info = fp.read()
			id_info = json.loads(id_info)
		os.remove(path.ErrorIdPath)
		try:
			e_id = id_info['ErrorId']
			detail = id_info["DetailExp"]
			quit_flag = id_info["QuitFlag"]
		except Exception as e:
			raise Exception("获取Key值信息出错，Key: " + str(e))
	except Exception as e:
		e_id = '0x4000001f'
		detail = e
	try:
		with open(path.ErrorDataPath, "r") as fp:
			data = fp.read()
			data = json.loads(data)
			try:
				message = data[e_id][Language]
			except Exception as e:
				raise Exception("未找到该ID信息, ID: " + str(e))
	except Exception as e:
		message = "读取错误数据文件出错"
		detail = e
	return (e_id, message, detail, quit_flag)

def GetErrorInfo_Net():
	mess = GetErrorInfo()
	return "%s\n%s"%(mess[1],mess[2])
'''