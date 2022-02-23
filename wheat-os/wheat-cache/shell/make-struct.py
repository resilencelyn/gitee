import os

import yaml
from jinja2 import Template

sysPath = os.getcwd()
tempPath = f"{sysPath}/storage/temp"
protobufPath = f"{sysPath}/protobuf"
storagePath = f"{sysPath}/storage"


class KeyMap(object):
    def __init__(self, key: str, val) -> None:
        self.key = key
        self.val = val
        self.upper = val


def go_fmt(path: str):
    os.system(f"go fmt {path}")


def to_camel(val: str) -> str:
    return "".join([k.capitalize() for k in val.split('_')])


class KeyOption(object):
    def __init__(self, key, option):
        self.key = key
        self.option = option

    def __str__(self):
        return "{}:{}".format(self.key, self.option)


def load_template(name: str) -> str:
    with open(f"{tempPath}/{name}", "r", encoding="utf-8") as fp:
        return fp.read()


def load_conf():
    conf_path = f"{tempPath}/tem.yaml"
    with open(conf_path, 'r', encoding='utf-8') as f:
        cfg = f.read()

    cfg = yaml.load(cfg, Loader=yaml.FullLoader)

    cfg_camel = {}

    for key, val in cfg.items():
        cfg_camel[key] = [to_camel(v) for v in val]

    return cfg, cfg_camel


# 生成常量
def set_structure_const_template(conf: dict):
    tem_text = load_template("const.template")
    keys = conf.keys()
    key_map = []
    val_set = []
    for k, v in conf.items():
        key_map.append(KeyMap(key=k, val=v))

        for val in v:
            val_set.append(val)

    template = Template(tem_text)
    text = template.render(keys=keys, key_maps=key_map, sets=val_set)

    temp_path = f"{tempPath}/const.gen.go"
    with open(temp_path, 'w', encoding='utf-8') as f:
        f.write(text)


def format_code_go():
    go_fmt(f"{tempPath}/const.gen.go")


if __name__ == "__main__":
    conf, cfg_camel = load_conf()

    set_structure_const_template(cfg_camel)
    # 格式化代码
    format_code_go()
