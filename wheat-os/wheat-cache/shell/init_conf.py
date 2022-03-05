import os
from pathlib import Path
import shutil

sysPath = os.getcwd()
confInPath = f"{sysPath}/conf/wheat-cache.yaml"
confOutPath = "/etc/wheat-cache"


def check_and_make_conf_dir():
    conf_dir = Path(confOutPath)
    if not conf_dir.is_dir():
        os.makedirs(confOutPath)


def copy_conf():
    shutil.copy(confInPath, confOutPath)


if __name__ == '__main__':
    check_and_make_conf_dir()
    copy_conf()
