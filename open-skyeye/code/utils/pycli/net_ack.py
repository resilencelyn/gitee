def ack_result(name , result, error):
	ack = {"ack":{"name":name, "result":result, "error":error}}
	return ack

def response(request, result, error=None):
	return ack_result(request["name"], result, error)
