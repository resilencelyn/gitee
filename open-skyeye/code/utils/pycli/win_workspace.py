# -*- coding: UTF-8 -*-
import os
import shutil
import wx
import time
import operator
import se_path
import traceback
from tools import Profile
import skyeye_common_module as scm

ws_config = Profile(filename = "workspace.cfg")
LoadModule = False
ws_module_list = []

def rm_dir_file(dirname):
	files = os.listdir(dirname)
	for fi in files:
		fi_p = os.path.join(dirname, fi)
		if os.path.isdir(fi_p):
			try:
				shutil.rmtree(fi_p)
			except:
				print ("can not remove file: %s" % fi_p)
		else:
			try:
				os.remove(fi_p)
			except:
				print ("can not remove file: %s" % fi_p)
	return None


def copy_ws_to(ws):
	ret = os.path.isdir(ws)
	if ret == False:
		return False
	sky_dir = se_path.InstallDir
	ws_dir = os.path.join(sky_dir, "ws\\workspace")
	files = os.listdir(ws_dir)
	for fi in files:
		fi_p = os.path.join(ws_dir, fi)
		if os.path.isdir(fi_p):
			if os.path.exists(os.path.join(ws, fi)) == False:
				shutil.copytree(os.path.join(ws_dir, fi), os.path.join(ws, fi))
		else:
			if os.path.exists(os.path.join(ws, fi)) == False:
				shutil.copy(os.path.join(ws_dir, fi), os.path.join(ws, fi))

def create_ws_properties(ws):
	ws_p = ws
	skyeye_install_dir = se_path.InstallDir
	mingw_install_dir = se_path.MingwInstallDir
	properties = 'workspace=%s\nskyeye-root=%s\nmingw=%s\n' % (ws_p, skyeye_install_dir, mingw_install_dir)
	try:
		pro_p = os.path.join(ws, ".workspace-properties")
		fi = open(os.path.join(os.path.join(pro_p), "workspace-path"), 'w')
		fi.write(properties)
		fi.close()
		return True
	except:
		print ("Create workspace %s error" % ws)
	return False

def read_ws_properties(ws, key):
	lines = []
	pro_p = os.path.join(ws, ".workspace-properties")
	try:
		fi = open(os.path.join(os.path.join(pro_p), "workspace-path"), 'r')
		lines = fi.readlines()
		fi.close()
	except:
		return None
	for line in lines:
		line = str(line)
		line = line.rstrip('\n')
		sts = line.split('=')
		if operator.eq(sts[0], key) == True:
			return sts[1]
	return None

def update_steup_file(ws):
	skyeye_root = read_ws_properties(ws, "skyeye-root")
	common_steup = os.path.join(ws, "skyeye")
	gui_steup = os.path.join(ws, "skyeye-gui")
	try:
		fi = open(common_steup, 'r')
		lines = fi.readlines()
		line_num = len(lines)
		for i in range(0, line_num):
			lines[i] = lines[i].replace("@SKYEYE_DIR@", skyeye_root)
		fi.close()
		fi = open(common_steup, 'w')
		fi.truncate(0)
		fi.writelines(lines)
		fi.close()
	except:
		detail = traceback.format_exc()
		print (detail)
		return None

def update_msys_profile(ws):
	skyeye_root = se_path.SkyEyeBin
	mingw_root = se_path.MingwInstallDir
	if mingw_root == None:
		return
	workspace_root = ws
	profile_root = os.path.join(mingw_root,"msys\\1.0\\etc" )
	profile_default = os.path.join(profile_root, "profile_default.def")
	profile_custm = os.path.join(profile_root, "profile.def")
	profile = os.path.join(profile_root, "profile")
	workspace_root = workspace_root.replace("\\", "/")
	try:
		shutil.copy(profile_custm, profile)
	except:
		detail = traceback.format_exc()
		print (detail)
	try:
		fi = open(profile, 'r')
		lines = fi.readlines()
		line_num = len(lines)
		for i in range(0, line_num):
			lines[i] = lines[i].replace("@WORKSPACE@", workspace_root)
		fi.close()
		fi = open(profile, 'w')
		fi.truncate(0)
		fi.writelines(lines)
		fi.close()
	except:
		shutil.copy(profile_default, profile)
		detail = traceback.format_exc()
		print (detail)
		return None

def rm_relate_file(ws):
	bin_dir = os.path.join(ws, "bin")
	makefile  = os.path.join(ws, "Makefile")
	targets_dir = os.path.join(ws, "targets")
	modules_dir = os.path.join(ws, "modules")
	rm_dir_file(bin_dir)
	rm_dir_file(targets_dir)
	rm_dir_file(modules_dir)
	try:
		os.remove(makefile)
	except:
		print ("can not remove file: %s" % makefile)

def update_makefile(ws):
	skyeye_root = se_path.SkyEyeBin
	mingw_root = se_path.MingwInstallDir
	if mingw_root == None:
		#rm_relate_file(ws)
		return 
	workspace_root = ws
	sky_install_dir = os.path.join(skyeye_root, "opt\\skyeye")
	dml_install_dir = os.path.join(mingw_root, "gdml")
	sky_include_dir = os.path.join(skyeye_root, "include")
	mkf = os.path.join(ws, "bin\\def\\Makefile.def")
	mkf_for_sky = os.path.join(ws, "bin\\def\\Makefile_for_skyeye.def")
	try:
		fi = open(mkf, 'r')
		lines = fi.readlines()
		line_num = len(lines)
		for i in range(0, line_num):
			lines[i] = lines[i].replace("@SKYEYE_DIR@", sky_install_dir)
			lines[i] = lines[i].replace("@SKYEYE_INSTALL_DIR@", sky_install_dir)
			lines[i] = lines[i].replace("$GDML_DIR@", dml_install_dir)
			lines[i] = lines[i].replace("@WORKSPACE@", workspace_root)
		fi.close()
		fi = open(mkf, 'w')
		fi.truncate(0)
		fi.writelines(lines)
		fi.close()
		fi = open(mkf_for_sky, 'r')
		lines = fi.readlines()
		line_num = len(lines)
		for i in range(0, line_num):
			lines[i] = lines[i].replace("@SKYEYE_DIR@", sky_install_dir)
			lines[i] = lines[i].replace("@SKYEYE_INSTALL_DIR@", sky_install_dir)
			lines[i] = lines[i].replace("@GDML_DIR@", dml_install_dir)
			lines[i] = lines[i].replace("@WORKSPACE@", workspace_root)
		fi.close()
		fi = open(mkf_for_sky, 'w')
		fi.truncate(0)
		fi.writelines(lines)
		fi.close()
	except:
		detail = traceback.format_exc()
		print (detail)
		return None

def update_conf_path(ws):
	skyeye_bin = se_path.SkyEyeBin
	include_dir=os.path.realpath(os.path.join(skyeye_bin,"..\\include")).replace("\\","/")
	lib_dir=os.path.realpath(os.path.join(skyeye_bin,"..\\lib\\skyeye")).replace("\\","/")
	install_dir=os.path.realpath(os.path.join(ws,"win32\\lib")).replace("\\","/")
	common_dir=skyeye_bin.replace("\\","/")
	data="INCLUDE_PATH = %s\nLIB_PATH = %s\nCOMMON_PATH = %s\nINSTALL_PATH = %s"%(include_dir,lib_dir,common_dir,install_dir)
	h_install_dir=os.path.realpath(os.path.join(ws,"include")).replace("\\","/")
	intf_dll_dir=os.path.realpath(os.path.join(ws,"win32\\interface\\lib")).replace("\\","/")
	py_install_dir=os.path.realpath(os.path.join(ws,"win32\\interface\\py")).replace("\\","/")
	data=data+"\nH_INSTALL_PATH = %s\nINTF_DLL_PATH = %s\nPY_INSTALL_PATH = %s"%(h_install_dir,intf_dll_dir,py_install_dir)
	try:
		fp=open(os.path.join(ws,"conf_path.txt"),"w")
		fp.write(data)
		fp.close()
	except:
		print ("Create conf_path.txt error")
		return None

def check_workspace(ws):
	try:
		pro_p = os.path.join(ws, ".workspace-properties")
		ret = os.path.exists(pro_p)
		return ret
	except:
		return False

def change_workspace():
	dlg = wx.DirDialog(None, "选择一个工作目录来切换", style=wx.DEFAULT_DIALOG_STYLE | wx.RESIZE_BORDER | wx.DD_NEW_DIR_BUTTON)
	#dlg.SetIcons(get_window_icons())
	if dlg.ShowModal() != wx.ID_OK:
		return None
	else:
		dirname = dlg.GetPath()
	check_workspace(dirname)
	dlg.Destroy()

def create_func(dirname):
	progrdlg = wx.ProgressDialog(parent = None, title="创建工作目录", message=" ", maximum=30, style = wx.PD_AUTO_HIDE |wx.PD_APP_MODAL |wx.PD_SMOOTH)
	progrdlg.SetSize((400, 120))
	progrdlg.Show()
	mx  = 30
	keepGoing = True
	keepGoing = progrdlg.Update(10, "生成文件...")
	copy_ws_to(dirname)
	keepGoing = progrdlg.Update(20, "生成工作目录特性")
	time.sleep(0.5)
	create_ws_properties(dirname)
	time.sleep(0.5)
	keepGoing = progrdlg.Update(25, "初始化编译信息")
	update_makefile(dirname)
	update_steup_file(dirname)
	time.sleep(0.5)
	keepGoing = progrdlg.Update(30, "创建完成")
	progrdlg.Destroy()

def create_workspace():
	dlg = wx.DirDialog(None, "选择一个工作目录来创建", style=wx.DEFAULT_DIALOG_STYLE | wx.RESIZE_BORDER | wx.DD_NEW_DIR_BUTTON)
	if dlg.ShowModal() != wx.ID_OK:
		return None
	else:
		dirname = dlg.GetPath()
	dlg.Destroy()
	ret = check_workspace(dirname)
	if ret:
		dlg = wx.MessageDialog(None, "这个目录已经是工作目录,是否替换?", style=wx.YES_NO)
		if dlg.ShowModal() == wx.ID_YES:
			rm_dir_file(dirname)
			create_func(dirname)
		else:
			pass
	else:
		create_func(dirname)
	update_msys_profile(dirname)
	update_conf_path(dirname)
	return dirname

def create_workspace_API(dirname):
	create_ws_dir(dirname)
	copy_ws_to(dirname)
	create_ws_properties(dirname)
	update_makefile(dirname)
	update_steup_file(dirname)
	update_msys_profile(dirname)
	update_conf_path(dirname)
	ws_config.set("common", "lastworkspace", dirname)
	return True

def create_ws_dir(dirname):
	if os.path.exists(dirname) == False:
		os.makedirs(dirname)

def chose_workspace():
	path = None
	dlg = wx.DirDialog(None, "选择一个工作目录来创建", style=wx.DEFAULT_DIALOG_STYLE | wx.RESIZE_BORDER | wx.DD_NEW_DIR_BUTTON)
	if dlg.ShowModal() == wx.ID_OK:
		path = dlg.GetPath()
	dlg.Destroy()
	return path

def change_workspace(ws):
	dlg = wx.DirDialog(None, "选择一个工作目录来切换", style=wx.DEFAULT_DIALOG_STYLE | wx.RESIZE_BORDER | wx.DD_NEW_DIR_BUTTON)
	if dlg.ShowModal() != wx.ID_OK:
		return None
	else:
		dirname = dlg.GetPath()
	dlg.Destroy()
	if(operator.eq(ws, dirname) == True):
		dlg = wx.MessageDialog(None, "这个目录已经是当前工作目录", style=wx.OK)
		dlg.ShowModal()
		dlg.Destroy()
		return None
	ret = check_workspace(dirname)
	if ret:
		update_msys_profile(dirname)
		update_conf_path(dirname)
		return dirname
	else:
		create_func(dirname)
	update_msys_profile(dirname)
	update_conf_path(dirname)
	return dirname

class select_workspace(wx.Dialog):
	def __init__(self, parent, win):
		wx.Dialog.__init__(self, parent, wx.ID_ANY, "选择工作目录", style=wx.DEFAULT_DIALOG_STYLE)
		self.parent = parent
		self.win = win
		usrpath = os.environ["USERPROFILE"]
		default_ws = os.path.join(usrpath, "skyeye_workspace")
		default_ws = os.path.abspath(default_ws)
		font = self.GetFont()
		self.SetFont(wx.Font(font.GetPointSize(), wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL, faceName='Segoe UI'))
		txt1 = wx.StaticText(self, wx.ID_ANY, "工作目录用来存放SkyEye项目文件。SkyEye也会把启动脚本放到这个目录。")
		txt2 = wx.StaticText(self, wx.ID_ANY, "选择一个空目录来创建工作目录")
		self.wsname = wx.TextCtrl(self, wx.ID_ANY)
		self.Bind(wx.EVT_TEXT,self.ws_input,self.wsname)
		wsbutton = wx.Button(self, wx.ID_ANY, "浏览")
		self.Bind(wx.EVT_BUTTON,self.select_ws,wsbutton)
		ok_button = wx.Button(self, wx.ID_ANY, "确定")
		self.Bind(wx.EVT_BUTTON,self.ok,ok_button)
		sizer = wx.GridBagSizer(hgap=1, vgap=1)
		sizer.Add(txt1, pos = (0, 0), flag =wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 10 )
		sizer.Add(txt2, pos = (1, 0), flag =  wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 10)
		sizer.Add(self.wsname, pos = (2, 0), flag = wx.EXPAND | wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 10 )
		sizer.Add(wsbutton, pos = (2, 1), flag =  wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 10)
		sizer.Add(ok_button, pos = (3, 1), flag = wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM, border= 10 )
		self.SetSizerAndFit(sizer)
		self.wsname.SetValue(default_ws)
		self.Bind(wx.EVT_CLOSE, self.close, self)

	def close(self, event):
		self.Destroy()
		self.win.quit_skyeye(0)

	def ok_button(self):
		return [ x for x in self.GetChildren() if x.GetId() == wx.ID_OK ][0]

	def update_buttons(self):
		#oself.ok_button().Enable(cli.iff(self.wsname.GetValue(), True, False))
		pass

	def ws_input(self, event):
		pass

	def select_ws(self, event):
		ws = chose_workspace()
		if ws == None:
			return
		if  os.path.exists(ws) == True:
			if os.path.isfile(ws) == True:
				dlg = wx.MessageDialog(None, "您选择的路径不是目录, 请重新指定!", "提示", style=wx.YES)
				if dlg.ShowModal() == wx.ID_YES:
					return
		self.wsname.SetValue(ws)

	def ok(self, event):
		self.Hide()
		ws = self.wsname.GetValue()
		ret = check_workspace(ws)
		if ret:
			dlg = wx.MessageDialog(None, "这个目录已经是工作目录,是否替换?","提示", style=wx.YES_NO)
			if dlg.ShowModal() == wx.ID_YES:
				#rm_dir_file(ws)
				create_func(ws)
			else:
				self.ShowModal()
		else:
			create_ws_dir(ws)
			create_func(ws)
		update_msys_profile(ws)
		update_conf_path(ws)
		self.win.set_last_workspace(ws)
		ws_config.set("common", "lastworkspace", ws)
		self.Destroy()
		event.Skip()

def get_workspace_dir():
	if ws_config.has_key("common", "lastworkspace"):
		return ws_config.get("common", "lastworkspace")
	return None

def load_module_from_ws():
	global LoadModule
	global ws_module_list
	if LoadModule == True:
		return
	try:
		class_list1 = scm.SkyEyeGetClassList()
	except:
		class_list1 = []
	ws_dir = get_workspace_dir()
	ret = check_workspace(ws_dir)
	if ret == False:
		return
	sky_lib_dir = os.path.join(ws_dir, "win32\\lib")
	if os.path.exists(sky_lib_dir) == False:
		return
	dir_list = []
	dir_list.append(sky_lib_dir)
	files = os.listdir(sky_lib_dir)
	for fi in files:
		fi_p = os.path.join(sky_lib_dir, fi)
		if os.path.isdir(fi_p):
			dir_list.append(fi_p)
	for module_dir in dir_list:
		try:
			ret = scm.SkyEyeLoadModuleFromDir(module_dir)
		except:
			return
	try:
		class_list2 = scm.SkyEyeGetClassList()
	except:
		class_list2 = []
	ws_module_list = list(set(class_list2) - set(class_list1))
	LoadModule = True

if __name__ == '__main__':
	app = wx.PySimpleApp()
	create_workspace()
	app.MainLoop()