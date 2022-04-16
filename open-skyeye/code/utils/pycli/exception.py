


class JsonConfInitErrorException(Exception):
	pass

class DeviceAddrClashException(Exception):
	pass

class SKYError(Exception):
	pass

class CMDError(SKYError):
	def __init__(self, value):
		self.message = value
	def __str__(self):
		return repr(self.message)

class DLLError(SKYError):
	def __init__(self, value):
		self.message = value
	def __str__(self):
		return repr(self.message)
