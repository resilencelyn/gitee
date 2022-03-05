import os
from jinja2 import Template


sysPath = os.getcwd()
tempPath = f"{sysPath}/plugins/config"
structurePath = f"{sysPath}/pkg/structure"
protobufPath = f"{sysPath}/protobuf"
storagePath = f"{sysPath}/storage"
middlePath = f"{sysPath}/plugins"

def go_fmt(path: str):
    os.system(f"go fmt {path}")


def to_camel(val: str) -> str:
    camel =""
    val = val.split('_')
    for k in range(0,len(val),1):  
        if k == 0:       
            camel = camel+str(val[k])      
        else:
            camel = camel+str(val[k].capitalize())
    return camel

# 得到各个中间件的文件名
def getMiddleName(path:str)->list:
    dic = []
    dataname = os.listdir(path)
    
    for i in dataname :
        if i != 'config' and i != 'define.go':
            dic.append(i)
    return dic

#读取模板
def load_template(name: str) -> str:
    with open(f"{tempPath}/{name}", "r", encoding="utf-8") as fp:
        return fp.read()

# 获取各个中间件的目录
def getMiddleDir(keys:list)->list:
    dirs = []
    middleware = []
    for middlename in keys:
        middleware=[]
        name = to_camel(middlename.replace("-","_"))
        dir = middlename
        middleware.append(name)
        middleware.append(dir)
        dirs.append(middleware)
    return dirs

def format_code_go():
    go_fmt(f"{middlePath}/config/middle.gen.go")

def set_middle_server():

    tem_text = load_template("middle.template")
    temp = Template(tem_text)
    keys = getMiddleName(middlePath)
    dirs = getMiddleDir(keys)
    text = temp.render(dirs=dirs)
    temp_path = f"{middlePath}/config/middle.gen.go"
    with open(temp_path,'w',encoding='utf-8') as f:
        f.write(text)


if __name__ == "__main__":
   set_middle_server()
   format_code_go()
    
    
