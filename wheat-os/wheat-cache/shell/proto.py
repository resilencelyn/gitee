import os

sysPath = os.getcwd()
protoPath = f"{sysPath}/protobuf"

comm = """
 protoc --proto_path={} --proto_path={} --go_out=plugins=grpc:{} {} 
"""


def out_proto():
    # 到达 storage
    for root, dirs, files in os.walk(protoPath):
        for f in files:
            commend = comm.format(protoPath, root, sysPath, f).strip()
            err = os.system(commend)

            if err:
                print(f, "-> out put err")
                return os._exit(-1)
            else:
                print(f, "->  success")


if __name__ == '__main__':
    out_proto()
