import winreg
import wx
import operator


def get_framework_version():
	key = r"SOFTWARE\Microsoft\NET Framework Setup\NDP\V4\Full"
	try:
		i = 0
		while 1:
			handle = winreg.OpenKeyEx(winreg.HKEY_LOCAL_MACHINE, key)
			name, value, ty = winreg.EnumValue(handle, i)
			if operator.eq(name, "Version") == True:
				return value
			i +=1
	except WindowsError:
		return None


def check_framework_version():
	v = get_framework_version()
	v = v[0:3]
	try:
		version = float(v)
	except:
		version = 0
	if version >= 4.0:
		return True
	else:
		return False
