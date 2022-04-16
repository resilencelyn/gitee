import errormessage
#from exception import *
#from conf import *
#from skyeye_common_module import *
#import tools


Command = {
}


class CommandClass():
	def __init__(self, cmd_name):
		self.FuncList = Command[cmd_name]
	def run(self, args):
		return self.FuncList[0](args)

	def synopses(self):
		return self.FuncList[1]()

	def doc(self):
		return self.FuncList[2]()

	def see_also(self):
		return self.FuncList[3]()


def GetCommand(cmd_name):
	cmd_name = cmd_name.replace('_', '-')
	if cmd_name not in Command.keys():
		return None
	cmd = CommandClass(cmd_name = cmd_name)
	return cmd


def NewCommand(cmd_name, cmd, synopses, doc, see_also):
	cmd_name = cmd_name.replace('_', '-')
	if cmd_name in Command.keys():
		string = "%s already exists" % (cmd_name)
		errormessage.SetErrorInfo(0x401d0000, False, string)
		raise
	Command[cmd_name] = []
	Command[cmd_name].append(cmd)
	Command[cmd_name].append(synopses)
	Command[cmd_name].append(doc)
	Command[cmd_name].append(see_also)
	return True

def IsCommandExists(cmd_name):
	cmd_name = cmd_name.replace('_', '-')
	if cmd_name not in Command.keys():
		return False
	return True

def CommandKeys():
	return Command.keys()

def Commands():
	return CommandKeys()

