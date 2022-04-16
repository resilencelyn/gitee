import os
import sys
import se_path

def get_custom_dialog():
	file_list = []
	filepath = se_path.CustomPath
	if not filepath in sys.path:
		sys.path.append(filepath)
	dialog_list = []
	module_list = []
	for root, dirs, files in os.walk(filepath):
		if "__pycache__" in root:
			continue
		for name in files:
			if name.endswith(".py") or name.endswith(".pyc"):
				list = os.path.splitext(name)
				name = list[0]
				if name not in file_list:
					module_list.append(name)
					file_list.append(name)

	for module in module_list:
		if not module in sys.modules:
			mod = __import__(module)
		mod = sys.modules[module]
		try:
			file_menu = mod.FileMenu
			cls = mod.WinCls
			dic = [file_menu, cls]
			dialog_list.append(dic)
		except:
			pass
	return dialog_list

