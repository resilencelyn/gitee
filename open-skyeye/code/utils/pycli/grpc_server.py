#coding:utf-8
"""The Python implementation of the GRPC SkyEye server."""

from concurrent import futures
import logging
import grpc
from grpc_package import skyeye_rpc_pb2
from grpc_package import skyeye_rpc_pb2_grpc
import threading
import os
import socket
import time

net_port = None

server_callback = None
cancel_callback = None
log_message = None

def get_free_port():
	global net_port
	if net_port != None:
		return
	tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	tcp.bind(("", 0))
	_, port = tcp.getsockname()
	tcp.close()
	net_port = str(port)

get_free_port()

def set_net_port(port):
	global net_port
	net_port = port

def set_callback(server_call, cancel_call):
	global server_callback
	global cancel_callback
	server_callback = server_call
	cancel_callback =cancel_call

class SkyEyeJSONServicer(skyeye_rpc_pb2_grpc.JSONTransmissionServicer):

	def CallSkyEye(self, request, context):
		stop_event = threading.Event()
		process_done = False

		def on_rpc_done():
			if (not process_done):
				if (cancel_callback):
					cancel_callback(None)
			stop_event.set()

		context.add_callback(on_rpc_done)
		result = server_callback(request.request)
		process_done = True
		return skyeye_rpc_pb2.JSONResponse(response=result)

	def SkyEyeLogReport(self, request, context):
		global log_message
		while context.is_active():
			if (log_message is not None):
				yield skyeye_rpc_pb2.JSONResponse(response=log_message)
				log_message = None
			time.sleep(0.1)
	


def ControlServer():
	server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
	skyeye_rpc_pb2_grpc.add_JSONTransmissionServicer_to_server(SkyEyeJSONServicer(), server)
	server.add_insecure_port('[::]:'+ net_port)
	return server
	# server.wait_for_termination()
