import time
import threading
import conf

time_num = 1
mips_thread = None

class mips(threading.Thread):
    def __init__(self, win = None):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.thread_stop = False
        self.win = win
        self.info = {}

    def run(self):
        while not self.thread_stop:
            self.config = conf.GetGlobalConfig()
            if not self.config:
                time.sleep(0.1)
                continue
            for cpu in conf.Cpus:
                try:
                    cpu.update_speed()
                except:
                    pass
            time.sleep(1)
    def stop(self):
        self.thread_stop = 1

def get(cpuname = None):
    global time_num
    if cpuname:
        pass
    else:
        mips = {}
        for cpu in conf.Cpus:
            mips[cpu.name] = cpu.speed * time_num
    return mips
