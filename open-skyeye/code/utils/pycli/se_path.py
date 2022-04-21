import operator
import platform, os


def GetPicPath(filename):
	basedir = os.path.join(SkyEyeBin, "picture")
	path = os.path.join(basedir, filename)
	return os.path.abspath(path)

def GetDocPath(filename):
	basedir = os.path.join(SkyEyeBin, "doc")
	path = os.path.join(basedir, filename)
	return os.path.abspath(path)

def get_bin_env_path(path_name):
	config_path=os.getenv("CONFIG_PATH")
	config_name="/config.txt"
	bin_name="SKYEYEBIN"
	if config_path!=None:
		fp=open(config_path+config_name,"r")
		info_list=fp.readlines()
		fp.close()
		for info in info_list:
			if info.split(":::")[0]==path_name:
				bin_name=info.strip().split(":::")[1]
	return os.getenv(bin_name)

def get_mingw_install_dir():
	import winreg
	key = r"SOFTWARE\SkyEye\MingwInstallDir"
	try:
		i = 0
		while 1:
			handle = winreg.OpenKeyEx(winreg.HKEY_LOCAL_MACHINE, key)
			name, value, ty = winreg.EnumValue(handle, i)
			i +=1
			if operator.eq(name, "path") == True:
				if os.path.exists(value):
					return value
	except WindowsError:
		pass
	return None

SkyEyeBin = get_bin_env_path("BIN")

InstallDir = os.path.abspath(SkyEyeBin + "/../../../")
EtcDir = os.path.abspath(InstallDir + "/etc/")
InfoDir = os.path.abspath(InstallDir + "/info/")
os_info = platform.system()
if operator.eq(os_info, "Linux") == False:
	LogPath = os.path.abspath(SkyEyeBin)
	LicPath = os.path.abspath(os.path.join(InfoDir,"license"))
	default_workspace =  os.path.abspath(SkyEyeBin + "/../../../targets")
	DefaultModulePath = os.path.abspath(SkyEyeBin + "/../lib/skyeye")
	MingwInstallDir = get_mingw_install_dir()
	ButtonConfPath = os.path.join(InstallDir, 'button.conf')
else:
	delimiter = "/"
	LogPath = os.getenv("HOME") + "/.skyeye/"
	LicPath = "."
	default_workspace = os.getenv("HOME")
	default_workspace = os.path.abspath(default_workspace)
	DefaultModulePath = os.path.abspath(SkyEyeBin + "/../lib/skyeye")
	ButtonConfPath = os.path.join(LogPath, 'button.conf')

GuiLogPath = os.path.join(LogPath, "gui.log")
RunLogPath = os.path.join(LogPath, "run.log")
CommandPath = os.path.join(SkyEyeBin, "command")
CustomPath = os.path.join(SkyEyeBin, "custom")
PackageFilePath = os.path.join(InfoDir, "package.cfg")
ModelPath = os.path.join(InstallDir, "uiConfig/Modeling.exe")
#SeCoveragePath = os.path.join(InstallDir, "SECoverage/CoverageTools.exe")
LicenseDir = os.path.join(InfoDir, "license")
CacheFileDir = os.path.join(InstallDir, '.cache')
ArchToolsDir = os.path.join(InstallDir, 'arch')
ErrorDataPath = os.path.join(SkyEyeBin, "error_message_data.json")





