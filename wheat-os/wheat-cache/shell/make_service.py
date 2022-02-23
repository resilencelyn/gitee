import os
from typing import Dict, List
import re
from jinja2.environment import Template

sysPath = os.getcwd()
protoAddress = f"{sysPath}/protobuf"
tempPath = f"{sysPath}/storage/temp"
daoPath = f"{sysPath}/storage/dao"
servicePath = f"{sysPath}/storage/service"
aofPath = f"{sysPath}/storage/persisted"


class ProtoOption(object):
    def __init__(self, method: str) -> None:
        super().__init__()
        self.method = method
        self.option = []
        self.ret = []
        self.external = False

    def add_option(self, opt: List[str]):
        self.option.extend(opt)

    def add_ret(self, opt: List[str]):
        self.ret.extend(opt)

    def __str__(self) -> str:
        req = ", ".join([f"{i[0]} {i[1]}" for i in self.option])
        resp = ", ".join([f"{i[0]} {i[1]}" for i in self.ret])
        return f"{self.method}({req})({resp})"


def dist_to_ProOpt(req, resp) -> List[ProtoOption]:

    def to_camel(val: str) -> str:
        return "".join([k.capitalize() for k in val.split('_')])

    def parse_type(l: str) -> List[str]:
        l = l.strip()
        if l == "":
            return [], False

        opt = l.split(";")
        result = []
        f = False
        for l_opt in opt:
            l_opt = l_opt.strip()
            l_list = l_opt.split()
            if len(l_list) == 0:
                continue

            val = l_list[0]
            if val == "BaseKey":
                val = "*proto.BaseKey"
                result.append([to_camel(l_list[1]), val])

            elif val == "repeated":
                val = f"[]{l_list[1]}"
                result.append([to_camel(l_list[2]), val])
            elif "map" in val:
                resMap = re.findall(
                    r"^map\s*<(.*?)\s*,\s*(.*?)\s*>.*?(\w+).*?", l_opt)
                if len(resMap[0]) == 3:
                    mapKey, mapVal, var = resMap[0]
                    result.append([to_camel(var), f"map[{mapKey}]{mapVal}"])

            elif "External" in val:
                f = True

            else:
                result.append([to_camel(l_list[1]), val])
        return result, f

    lists = []
    for key, value in req.items():
        p = ProtoOption(method=key)
        p.add_option(parse_type(value)[0])

        ret, e = parse_type(resp.get(key, ""))
        if e:
            p.external = True
        p.add_ret(ret)
        lists.append(p)
    return lists


def parse_protobuf_to_Opt(name: str) -> List[ProtoOption]:

    gather_req = re.compile(r'^(.*?)Request\s+{(.*?)}$')
    gather_resp = re.compile(r'^(.*?)Response\s+{(.*?)}$')

    with open(f"{protoAddress}/{name}", "r", encoding="utf-8") as fp:
        text = fp.read()
        text = re.sub(r'//.+', "", text)  # 去掉注释
        text = text.replace("\n", "")
        result = text.split("message")[1:]

    reqDist = {}
    respDist = {}

    for r in result:
        req = gather_req.findall(r)
        resp = gather_resp.findall(r)
        if len(req) != 0:
            req = req[0]
            reqDist[req[0].strip()] = req[1].strip()
        if len(resp) != 0:
            resp = resp[0]
            respDist[resp[0].strip()] = resp[1].strip()

    return dist_to_ProOpt(reqDist, respDist)


def load_protobuf() -> List[ProtoOption]:
    for _, _, file_name_list in os.walk(protoAddress):
        break

    li = []
    for name in file_name_list:
        if name.endswith("x.proto"):
            li.extend(parse_protobuf_to_Opt(name))
    return li


def go_fmt(path: str):
    os.system(f"go fmt {path}")


def load_template(name: str) -> str:
    with open(f"{tempPath}/{name}", "r", encoding="utf-8") as fp:
        return fp.read()


def gen_dao_interface(proto):

    tem_text = load_template("dao.template")
    template = Template(tem_text)

    text = template.render(keys=proto)

    temp_path = f"{daoPath}/interface.gen.go"
    with open(temp_path, 'w', encoding='utf-8') as f:
        f.write(text)


def gen_single_service(proto):
    tem_text = load_template("service.template")
    template = Template(tem_text)

    text = template.render(keys=proto)

    temp_path = f"{servicePath}/single_service.gen.go"
    with open(temp_path, 'w', encoding='utf-8') as f:
        f.write(text)


def gen_aof(proto):
    tem_text = load_template("aof.template")
    template = Template(tem_text)

    text = template.render(keys=proto)

    temp_path = f"{aofPath}/codec.gen.go"
    with open(temp_path, 'w', encoding='utf-8') as f:
        f.write(text)

    # 生成 AOF 恢复机制
    tem_text = load_template("dao_aof.template")
    template = Template(tem_text)

    text = template.render(keys=proto)

    temp_path = f"{daoPath}/dao.gen.go"
    with open(temp_path, 'w', encoding='utf-8') as f:
        f.write(text)


def format_code_go():
    go_fmt(f"{daoPath}/interface.gen.go")
    go_fmt(f"{servicePath}/single_service.gen.go")
    go_fmt(f"{aofPath}/codec.gen.go")
    go_fmt(f"{daoPath}/dao.gen.go")


def main():
    # 加载 protobuf
    protobuf = load_protobuf()

    # 生成 dao 接口
    gen_dao_interface(protobuf)

    # 生成服务代码
    gen_single_service(protobuf)

    # 生成 aof 解码方案
    gen_aof(protobuf)
    format_code_go()


if __name__ == "__main__":
    main()
