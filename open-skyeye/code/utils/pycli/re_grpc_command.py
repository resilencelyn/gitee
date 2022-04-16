import json
import grpc_server

def RSE_log_report(level,info):
	cmd_dict = {}
	cmd_dict["name"] = "RSE_log_report"
	cmd_dict["level"] = {"level" : level}
	cmd_dict["info"] = {"info" : info}
	cmd_str = json.dumps({"request":cmd_dict})
	grpc_server.log_message=cmd_str
