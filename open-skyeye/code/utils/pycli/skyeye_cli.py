# -*- coding: UTF-8 -*-
import cmd, sys, time, platform
import errormessage
from exception import *
from conf import *
from skyeye_common_module import *
import win_workspace as ws
import tools
import cli_init
import cli
import _thread
from sky_log import *
from x86 import x86
import mips
import os
import se
import se_path
from se_system import system
import pytimer
import functools
import operator
import fault_inject as sfi
import readline
import argparse

cli_init.InitCommandList()
sys.path.append(se_path.SkyEyeBin)

def add_help_method(cls):
    attrs = dir(cls)
    if 'print_help' not in attrs:
        raise NotImplementedError('No "print_help" method in class %s' % cls.__name__)

    def create_help_method(cmd):
        def help_method(self):
            self.print_help(cmd)
        return help_method

    cmds = [attr[3:] for attr in attrs if attr.startswith('do_')]
    for cmd in cmds:
        setattr(cls, 'help_'+cmd, create_help_method(cmd))

    return cls

@add_help_method
class SkyEyeCli(cmd.Cmd):
    Config = None
    # skyeye prompt
    prompt = '(skyeye) '

    def __init__(self, args=[]):
        cmd.Cmd.__init__(self)
        self.args_list = args
        welcome_message()
        # log=output_log()
        # se.log=log
        self.open_conf_flag = False
        #if "-gui" in self.args_list and tools.is_windows():
        #    from skyeye_gui import *
        #    win_control()
        RegisterLog(print)
        mips.mips_thread = mips.mips()
        mips.mips_thread.start()
        if(os.path.exists('.skyeyeinit')):
            tools.execute_skyeyeinit_file('.skyeyeinit')
        if tools.is_windows():
            ws.load_module_from_ws()
            try:
                NetCtrl.server_init()
            except:
                print ("网络服务启动失败,检查GRPC服务是否配置正确!")
        pytimer.PyTmrInit()

        # 为命令补齐配置readline
        self.readline_cfg()

        # 命令参赛解析器
        self.cmd_parsers = {}

    def readline_cfg(self):
        s = readline.get_completer_delims()
        for c in '<>-':
            s = s.replace(c, '')
        s += '.'
        readline.set_completer_delims(s)

    def quit_skyeye(self, arg):
        libcommon.com_quit(arg)

    def preloop(self):
        self.history_file = os.path.join(os.path.expanduser('~'), '.skyeye/cmd_history')
        if not os.path.exists(self.history_file):
            print('not exists')
            with open(self.history_file, 'w'):
                pass
        readline.read_history_file(self.history_file)

    def postloop(self):
        print('in postloop')
        readline.write_history_file(self.history_file)

    def precmd(self, arg):
        if arg:
            arg_list = arg.split()
            if arg_list:
                arg_list[0] = arg_list[0].replace('-', '_')
            return ' '.join(arg_list)
        return arg

    def postcmd(self, stop, line):
        time.sleep(0.01)
        if libcommon.SIM_is_running():
            self.prompt = "(running) "
        else:
            self.prompt = "(skyeye) "

        # his_len = readline.get_current_history_length()
        # print('len:"%s"' % his_len)
        return stop

    def print_help(self, cmd):
        parser = self.get_argparser(cmd)
        parser.print_help()

    def completenames(self, text, *ignored):
        # TODO: 补齐形如'<cpc-700>'对象名， 
        # if text.startswith('<'):
        #     pass

        # 支持"list-cpu" "list-device"带有"-"的命令
        hack = False
        if '-' in text:
            text = text.replace('-', '_')
            hack = True
        res = super().completenames(text, *ignored)
        res = [s.replace('_', '-') for s in res]
        return res

    def completedefault(self, text, line, begidx, endidx):
        # TODO: 实现参数补齐功能
        pass

    def default(self, line):
        print('*** Unknown command: %s\n'%line)
        print('*** Get commands with \'help\' cmd\n')

    def get_argparser(self, cmd):
        parser = self.cmd_parsers.get(cmd)
        if not parser:
            parser = getattr(self, 'create_argparser_'+cmd)()
        return parser
    
    def create_argparser_help(self):
        parser = self.cmd_parsers['help'] = argparse.ArgumentParser(
                prog='help',
                description='Print help info.',
                add_help=False,
                )

        parser.add_argument(
                'cmd', 
                metavar='<cmd>',
                nargs='?', 
                help='command name',
                )
        return parser

    def do_help(self, arg):
        arg = arg.replace('-', '_')
        return super().do_help(arg)

    def create_argparser_history(self):
        parser = self.cmd_parsers['history'] = argparse.ArgumentParser(
                prog='history',
                description='Show command history.',
                add_help=False)
        return parser

    def do_history(self, arg):
        parser = self.get_argparser('history')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return 

        hist_len = readline.get_current_history_length()
        for i in range(1, hist_len+1):
            hist_item = readline.get_history_item(i)
            print('%-5s %s' % (i, hist_item))

    def create_argparser_list_modules(self):
        parser = self.cmd_parsers['list_modules'] = argparse.ArgumentParser(
                prog='list_modules',
                description='List all the loaded module.',
                add_help=False)
        return parser

    def do_list_modules(self, arg):
        '''list-modules : List all the loaded module.'''
        parser = self.get_argparser('list_modules')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return 

        try:
            SkyEyeListModules()
        except Exception as e:
            print(e)

    def create_argparser_list_machines(self):
        parser = self.cmd_parsers['list_machines'] = argparse.ArgumentParser(
                prog='list_machines',
                description='List all the supported machines for SkyEye.',
                add_help=False)
        return parser

    def do_list_machines(self, arg):
        '''List all the supported machines for SkyEye.'''
        parser = self.get_argparser('list_machines')
        try:
            arg_ns = parser.parse_args(arg.split())
        except:
            return 

        config = GetGlobalConfig()
        if config:
            ml = config.get_mach_list()
            print("%-20s%-20s" % ("Id", "MachinesName"))
            for i in range(len(ml)):
                print("%-20s%-20s" % (i+1, ml[i]))

    def create_argparser_run_script(self):
        parser = self.cmd_parsers['run_script'] = argparse.ArgumentParser(
                prog='run_script',
                description='Run skyeye script.',
                add_help=False)

        parser.add_argument(
                'script_name', 
                metavar='<skyeye-script>',
                help='name of skyeye-script',
                )

        return parser

    def do_run_script(self, arg):
        parser = self.get_argparser('run_script')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return 

        cmd = cli.GetCommand("run-script")
        if not cmd:
            print ("Can not find command: run-script")
            return 
        if self.open_conf_flag:
            LogReport("Do not reload until RESET is performed")
            return False

        SetGlobalScript(ns.script_name)
        try:
            res = cmd.run([ns.script_name])
        except Exception as e:
            LogReportPack(errormessage.GetErrorInfo())
            res = False
        if res:
            self.open_conf_flag = True

    def create_argparser_run_command(self):
        parser = self.cmd_parsers['run_command'] = argparse.ArgumentParser(
                prog='run_command',
                description='Run a skyeye command.',
                add_help=False)

        parser.add_argument(
                'script_name', 
                metavar='<skyeye-cmd-part>',
                nargs='+',
                help='part of skyeye-command',
                )

        return parser

    def do_run_command(self, arg):
        parser = self.get_argparser('run_command')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return 

        cmd = cli.GetCommand("run-command")
        if not cmd:
            print ("Can not find command: run-command")
            return 
        cmd.run(arg)

    def create_argparser_bp_create(self):
        parser = self.cmd_parsers['bp_create'] = argparse.ArgumentParser(
                prog='bp_create',
                description='Insert a break-point on a cpu-core.',
                add_help=False)
        
        parser.add_argument(
                'cpu', 
                metavar='<cpu-core>',
                help='cpu-core name',
                )

        parser.add_argument(
                'bp_addr', 
                metavar='<bp-addr>',
                type=convert_int,
                help='break-point address (int)',
                )
        return parser

    def do_bp_create(self, arg):
        parser = self.get_argparser('bp_create')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return 

        try:
            SkyEyeCreateBreakpoint(ns.cpu, ns.bp_addr)
        except:
            print ("Insert breakpoint error")

    def create_argparser_bp_delete(self):
        parser = self.cmd_parsers['bp_create'] = argparse.ArgumentParser(
                prog='bp_create',
                description='Delete a break-point on a cpu-core.',
                add_help=False)
        
        parser.add_argument(
                'cpu', 
                metavar='<cpu-core>',
                help='cpu-core name',
                )


        parser.add_argument(
                'bp_addr', 
                metavar='<bp-addr>',
                type=convert_int,
                help='break-point address (int)',
                )
        return parser

    def do_bp_delete(self, arg):
        parser = self.get_argparser('bp_delete')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return 

        try:
            SkyEyeDeleteBreakpointByAddr(ns.cpu, ns.bp_addr)
        except:
            print ("Delete breakpoint error")

    def create_argparser_bp_list(self):
        parser = self.cmd_parsers['bp_list'] = argparse.ArgumentParser(
                prog='bp_list',
                description='Show all break-points.',
                add_help=False)
        parser.add_argument(
                'cpu', 
                metavar='<cpu-core>',
                help='cpu-core name',
                )
        return parser

    def do_bp_list(self, arg):
        parser = self.get_argparser('bp_list')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return 

        try:
            temp = []
            ret = SkyEyeGetBpNumbers(ns.cpu)
            for i in range(0, ret):
                temp.append(SkyEyeGetBreakpointAddrById(ns.cpu, i))
            print ("%-25s%-25s"%("CPU","Address(HEX)"))
            for i in temp:
                print ("%-25s0x%-25x"%(ns.cpu, i))
        except:
            print ("Breakpoint get error")

    def create_argparser_list_cpu(self):
        parser = self.cmd_parsers['list_cpu'] = argparse.ArgumentParser(
                prog='list_cpu',
                description='Show all available cpu.',
                add_help=False)
        return parser

    def do_list_cpu(self, arg):
        parser = self.get_argparser('list_cpu')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return 

        config = GetGlobalConfig()
        if not config:
            return 
        cpus = config.get_cpu_list()
        print ("%-20s%-20s" % ("ID", "CpuName"))
        count = 0
        for cpu in cpus:
            count = count + 1
            print  ("%-20d%-20s" % (count, cpu))

    def create_argparser_list_register(self):
        parser = self.cmd_parsers['list_register'] = argparse.ArgumentParser(
                prog='list_register',
                description='Show all registers of a device.',
                add_help=False)

        parser.add_argument(
                'machine', 
                metavar='<machine-name>',
                help='machine name',
                )

        parser.add_argument(
                'device', 
                metavar='<device-name>',
                help='device name',
                )
        return parser

    def do_list_register(self, arg):
        parser = self.get_argparser('list_register')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        config = GetGlobalConfig()
        if not config:
            return
        ret = sfi.get_device_register_info(ns.machine, ns.device)
        print ("%-20s%-20s%-20s" % ("Register", "Value(HEX)", "Address(HEX)"))
        for i in ret.keys():
            print ("%-20s%-20x%-20x" % (i, ret[i]["value"], ret[i]["addr"]))

    def create_argparser_list_device(self):
        parser = self.cmd_parsers['list_device'] = argparse.ArgumentParser(
                prog='list_device',
                description='Show all current device.',
                add_help=False)

        parser.add_argument(
                'machine', 
                metavar='<machine-name>',
                nargs='?',
                help='machine name',
                )
        return parser

    def do_list_device(self, arg):
        parser = self.get_argparser('list_device')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        config = GetGlobalConfig()
        if not config:
            return 

        if ns.machine is None:
            print ("%-30s%-30s" % ("DeviceName", "BelongBoard"))
            machlist = config.get_mach_list()
            for mach in machlist:
                mach_cls = config.get_mach_classname(mach)
                if mach_cls == 'pc_mach':
                    cpu = config.get_cpuname_by_mach(mach)
                    if cpu:
                        x86_obj = x86(cpu)
                        for device in x86_obj.devices:
                            print ("%-30s%-30s" % (device.name, mach))
                else:
                    device_list = config.get_device_list_by_mach(mach)
                    for device in device_list:
                        print ("%-30s%-30s" % (device, mach))
        else:
            print ("%-30s%-30s" % ("ID", "DeviceName"))
            device_list = config.get_device_list_by_mach(ns.machine)
            for i in range(0, len(device_list)):
                print ("%-30s%-30s" % (i+1, device_list[i]))

    def create_argparser_set_register(self):
        parser = self.cmd_parsers['set_register'] = argparse.ArgumentParser(
                prog='set_register',
                description='Set a value  to a register.',
                add_help=False)

        parser.add_argument(
                'machine', 
                metavar='<machine-name>',
                help='machine name',
                )

        parser.add_argument(
                'device', 
                metavar='<device-name>',
                help='device name',
                )

        parser.add_argument(
                'register', 
                metavar='<register-name>',
                help='device name',
                )

        parser.add_argument(
                'value', 
                metavar='<int value>',
                type=convert_int,
                help='value of int',
                )

        return parser

    def do_set_register(self, arg):
        parser = self.get_argparser('set_register')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            regid = SkyEyeGetDevRegIdByName(ns.machine, ns.device, ns.register)
            ret = SkyEyeSetDevRegValueById(ns.machine, ns.device, ns.value, regid)
        except Exception as e:
            print (e)
            return

    def create_argparser_list_class(self):
        parser = self.cmd_parsers['list_class'] = argparse.ArgumentParser(
                prog='list_class',
                description='Show all classes.',
                add_help=False)

        return parser

    def do_list_class(self, arg):
        parser = self.get_argparser('list_class')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            cls_l = SkyEyeGetClassList()
            cls_l.sort()
            #formot_output(cls_l)
            table_print(cls_l)
        except Exception as e:
            print (e)

    def create_argparser_list_iface(self):
        parser = self.cmd_parsers['list_iface'] = argparse.ArgumentParser(
                prog='list_iface',
                description='Show the information of a class.',
                add_help=False)

        parser.add_argument(
                'cls', 
                metavar='<class-name>',
                help='class name',
                )

    def do_list_iface(self, arg):
        parser = self.get_argparser('list_iface')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            cls_l = SkyEyeGetClassList()
            if ns.cls not in cls_l:
                print ("%s IFACE information was not found" % ns.cls)
                return 
            iface_l = SkyEyeGetClassIfaceList(ns.cls)
            iface_l.sort()
            table_print(iface_l)
        except Exception as e:
            print (e)

    def create_argparser_list_connect(self):
        parser = self.cmd_parsers['list_connect'] = argparse.ArgumentParser(
                prog='list_connect',
                description='Show the connect information of a class.',
                add_help=False)

        parser.add_argument(
                'cls', 
                metavar='<class-name>',
                help='class name',
                )

        return parser

    def do_list_connect(self, arg):
        parser = self.get_argparser('list_connect')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            cls_l = SkyEyeGetClassList()
            if ns.cls not in cls_l:
                print ("%s CONNECT information was not found" % ns.cls)
                return 
            connect_l = SkyEyeGetClassConnectList(ns.cls)
            connect_l.sort()
            #formot_output(connect_l)
            table_print(connect_l)
        except Exception as e:
            print (e)


    def create_argparser_list_attr(self):
        parser = self.cmd_parsers['list_attr'] = argparse.ArgumentParser(
                prog='list_attr',
                description='Show attribute of a class.',
                add_help=False)

        parser.add_argument(
                'cls', 
                metavar='<class-name>',
                help='class name',
                )

        return parser

    def do_list_attr(self, arg):
        parser = self.get_argparser('list_attr')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        attr_info_l = []
        try:
            cls_l = SkyEyeGetClassList()
            if ns.cls not in cls_l:
                print ("%s ATTR information was not found" % ns.cls)
                return 
            attr_l = SkyEyeGetClassAttrList(ns.cls)
            attr_l.sort()
            attr_info_l = []
            for attr in attr_l: 
                info_l = SkyEyeGetClassAttrInfo(ns.cls, attr)
                attr_info_l.append(info_l)
        except Exception as e:
            print (e)
            return
        print ("%-20s%-20s%s" % ("AttrName", "Type", "Description"))
        for info_l in attr_info_l:
            if len(info_l) != 3:
                destription = 'NULL'
            else:
                destription = info_l[2]
            print ("%-20s%-20s%s" % (info_l[0], info_l[1], destription))

##########################fault inject##################################
    def create_argparser_fj_set(self):
        parser = self.cmd_parsers['fj_set'] = argparse.ArgumentParser(
                prog='fj_set',
                description='(xxxxxx)',
                add_help=False)

        parser.add_argument(
                'machine', 
                metavar='<machine-name>',
                help='machine name',
                )

        parser.add_argument(
                'device', 
                metavar='<device-name>',
                help='device name',
                )

        parser.add_argument(
                'device_addr', 
                metavar='<device-addr>',
                type=convert_int,
                help='device addr',
                )

        parser.add_argument(
                'bit_index', 
                metavar='<bit-index>',
                type=convert_int,
                help='bit index',
                )

        parser.add_argument(
                'mode', 
                metavar='<mode>',
                type=convert_int,
                choices=range(3),
                help='mode',
                )

        return parser


    def do_fj_set(self, arg):
        parser = self.get_argparser('fj_set')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return
        ret = sfi.skyeye_set_fj(ns.machine, ns.device, ns.device_addr, ns.bit_index, ns.mode)
        if ret:
            print("Injection failure success")
        else:
            print("Injection failure failed")

    def create_argparser_fj_list(self):
        parser = self.cmd_parsers['fj_list'] = argparse.ArgumentParser(
                prog='fj_list',
                description='(xxxxxx)',
                add_help=False)
        return parser

    def do_fj_list(self, arg):
        parser = self.get_argparser('fj_list')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        ret=sfi.skyeye_get_fj()
        print ("%-20s%-20s%-20s%-20s%-10s%-10s" % ("Device","Mach","Register","Address(HEX)", "Bit(DEC)", "Mode(DEC)"))
        for fj in ret:
            print ("%-20s%-20s%-20s%-20x%-10d%-10d" % (fj[0],fj[1],fj[2],fj[3],fj[4],fj[5]))

    def create_argparser_fj_clear(self):
        parser = self.cmd_parsers['fj_clear'] = argparse.ArgumentParser(
                prog='fj_clear',
                description='(xxxxxx)',
                add_help=False)

        parser.add_argument(
                'machine', 
                metavar='<machine-name>',
                help='machine name',
                )

        parser.add_argument(
                'device', 
                metavar='<device-name>',
                help='device name',
                )

        parser.add_argument(
                'device_addr', 
                metavar='<device-addr>',
                type=convert_int,
                help='device addr',
                )

        parser.add_argument(
                'bit_index', 
                metavar='<bit-index>',
                type=convert_int,
                help='bit index',
                )

        parser.add_argument(
                'mode', 
                metavar='<mode>',
                type=convert_int,
                choices=range(3),
                help='mode',
                )

        return parser

    def do_fj_clear(self, arg):
        parser = self.get_argparser('fj_clear')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        ret = sfi.skyeye_clear_fj(ns.machine, ns.device, ns.device_addr, ns.bit_index, ns.mode)
        if ret:
            print ("Failure deletion successful")
        else:
            print ("Failure deletion failed")

    def create_argparser_md(self):
        parser = self.cmd_parsers['md'] = argparse.ArgumentParser(
                prog='md',
                description='Get the value at a address of memory.',
                add_help=False)

        parser.add_argument(
                'cpu',
                metavar='<cpu-name>',
                help='cpu name',
                )

        parser.add_argument(
                'addr', 
                metavar='<addr>',
                type=convert_int,
                help='memory address',
                )

        return parser

    def do_md(self, arg):
        parser = self.get_argparser('md')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        config = GetGlobalConfig()
        if not config:
            return 
        try:
            ret = SkyEyeReadMemory8(ns.cpu, ns.addr)
            print ("%-20s%-20s" % ("Addr(HEX)", "Value(HEX)"))
            print ("%-20x%-20x" % (ns.addr, ret))
        except Exception as e:
            print (e)

    def create_argparser_disassemble(self):
        parser = self.cmd_parsers['disassemble'] = argparse.ArgumentParser(
                prog='md',
                description='Show infomation of disassembleat a address.',
                add_help=False)

        parser.add_argument(
                'cpu',
                metavar='<cpu-name>',
                help='cpu name',
                )

        parser.add_argument(
                'addr', 
                metavar='<addr>',
                type=convert_int,
                help='memory address',
                )

        return parser

    def do_disassemble(self, arg):
        parser = self.get_argparser('disassemble')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        config = GetGlobalConfig()
        if not config:
            return

        disas_str =SkyEyeDisassemble(ns.cpu, ns.addr)
        print ("%-20s%-20s" % ("Addr(HEX)", "Value"))
        print ("%-20x%-20s" % (ns.addr, disas_str))

    def create_argparser_remote_gdb(self):
        parser = self.cmd_parsers['remote_gdb'] = argparse.ArgumentParser(
                prog='remote_gdb',
                description='Connect remote gdb.',
                add_help=False)

        parser.add_argument(
                'cpu',
                metavar='<cpu-name>',
                help='cpu name',
                )

        parser.add_argument(
                'ip', 
                metavar='<ip>',
                nargs='?',
                default='0.0.0.0',
                help='ip address',
                )

        parser.add_argument(
                'port', 
                metavar='<port>',
                type=convert_int,
                help='port number',
                )

        return parser

    def do_remote_gdb(self, arg):
        # FIXME: BUG
        parser = self.get_argparser('remote_gdb')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            ret = SkyEyeCreateRemoteGdb(ns.cpu, ns.port, ns.ip)
            if ret != 1:
                print("Remote Gdb Start error, Target Cpu Name: %s, Ip: %s, Port: %d" % (ns.cpu, ns.ip, ns.port))
            else:
                print("Remote Gdb Start OK!")
        except:
            print("An error has occurred happend when remote gdb start!")
            print("Target CPU name: %s, Ip: %s, Port: %d" % (ns.cpu, ns.ip, ns.port))

    def create_argparser_set_mode(self):
        parser = self.cmd_parsers['set_mode'] = argparse.ArgumentParser(
                prog='set_mode',
                description='Set the cpu running mode',
                add_help=False)

        parser.add_argument(
                'mode', 
                metavar='<mode>',
                type=convert_int,
                choices=range(4),
                help='mode (choose from 0, 1, 2, 3)',
                )

        return parser

    def do_set_mode(self, arg):
        parser = self.get_argparser('set_mode')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        config = GetGlobalConfig()
        if not config:
            return 
        print ("--%d--" % ns.mode)
        SetAllToDyncom(ns.mode)
        
    def create_argparser_speed(self):
        parser = self.cmd_parsers['speed'] = argparse.ArgumentParser(
                prog='speed',
                description='Show the current running speed of the cpu.',
                add_help=False)

        return parser

    def do_speed(self, arg):
        parser = self.get_argparser('speed')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        mips_dic = mips.get()
        for item in mips_dic.keys():
            print ("CPU:%20s  SPEED:%d" % (item, mips_dic[item]))

    def create_argparser_ls(self):
        parser = self.cmd_parsers['ls'] = argparse.ArgumentParser(
                prog='ls',
                description='list directory contents',
                add_help=False)

        parser.add_argument(
                'path', 
                metavar='<path>',
                nargs='?',
                default='',
                help='the path of file or directory',
                )

        return parser

    def do_ls(self, arg):
        parser = self.get_argparser('ls')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        if ns.path and not os.path.exists(ns.path):
            print("No such file or directory")
            return
        try:
            SkyEyeListDir(arg)
        except Exception as e:
            print (e)

    def create_argparser_quit(self):
        parser = self.cmd_parsers['quit'] = argparse.ArgumentParser(
                prog='quit',
                description='Quit skyeye.',
                add_help=False)

        return parser

    def do_quit(self, arg):
        parser = self.get_argparser('quit')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return
        self.postloop()
        if mips.mips_thread != None:
            mips.mips_thread.stop()
        try:
            SkyEyeQuit()
        except Exception as e:
            print (e)
    do_q = do_quit

    def create_argparser_run(self):
        parser = self.cmd_parsers['run'] = argparse.ArgumentParser(
                prog='run',
                description='Run.',
                add_help=False)

        return parser

    def do_run(self, arg):
        parser = self.get_argparser('run')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            SkyEyeRun()
            pytimer.set_running(True)
        except Exception as e:
            print (e)

    def create_argparser_stop(self):
        parser = self.cmd_parsers['stop'] = argparse.ArgumentParser(
                prog='stop',
                description='Stop.',
                add_help=False)

        return parser

    def do_stop(self, arg):
        parser = self.get_argparser('stop')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            SkyEyeStop()
            pytimer.set_running(False)
        except Exception as e:
            print (e)

    def create_argparser_stepi(self):
        parser = self.cmd_parsers['stepi'] = argparse.ArgumentParser(
                prog='stepi',
                description='Step one instruction exactly.',
                add_help=False)

        parser.add_argument(
                'cpu', 
                metavar='<cpu-name>',
                help='cpu name',
                )

        parser.add_argument(
                'size', 
                metavar='<step-size>',
                type=convert_int,
                help='step size',
                )

        return parser

    def do_stepi(self, arg):
        parser = self.get_argparser('stepi')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            SkyEyeStepi(ns.cpu, str(ns.size))
        except Exception as e:
            print (e)

    def create_argparser_reset(self):
        parser = self.cmd_parsers['reset'] = argparse.ArgumentParser(
                prog='reset',
                description='Reset.',
                add_help=False)

        return parser

    def do_reset(self, arg):
        parser = self.get_argparser('reset')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            SkyEyeReset()
            pytimer.set_running(False)
            self.open_conf_flag = False
        except Exception as e:
            print (e)

    def create_argparser_reverse(self):
        parser = self.cmd_parsers['reverse'] = argparse.ArgumentParser(
                prog='reverse',
                description='reverse',
                add_help=False)

        parser.add_argument(
                'cpu', 
                metavar='<cpu-name>',
                help='cpu name',
                )

        parser.add_argument(
                'size', 
                metavar='<step-size>',
                type=convert_int,
                help='step size',
                )

        return parser

    def do_reverse(self, arg):
        parser = self.get_argparser('reverse')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("reverse")
        if not cmd:
            LogReport('reverse: Can not find command "reverse"')
            return 
        try:
            cmd.run([ns.cpu, str(ns.size)])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_reverse_enable(self):
        parser = self.cmd_parsers['reverse_enable'] = argparse.ArgumentParser(
                prog='reverse_enable',
                description='reverse_enable',
                add_help=False)

        return parser

    def do_reverse_enable(self, arg):
        parser = self.get_argparser('reverse_enable')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("enable-reverse")
        if not cmd:
            LogReport('reverse_enable: Can not find command "enable-reverse"')
            return 
        try:
            cmd.run([])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_reverse_disable(self):
        parser = self.cmd_parsers['reverse_disable'] = argparse.ArgumentParser(
                prog='reverse_disable',
                description='reverse_disable',
                add_help=False)

        return parser

    def do_reverse_disable(self, arg):
        parser = self.get_argparser('reverse_disable')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("disable-reverse")
        if not cmd:
            LogReport('reverse_disable: Can not find command "disable-reverse"')
            return 
        try:
            cmd.run([])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_disable_device_work(self):
        parser = self.cmd_parsers['disable_device_work'] = argparse.ArgumentParser(
                prog='disable_device_work',
                description='disable_device_work',
                add_help=False)

        parser.add_argument(
                'device', 
                metavar='<device-name>',
                help='device name',
                )

        return parser

    def do_disable_device_work(self, arg):
        parser = self.get_argparser('disable_device_work')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("disable-device-work")
        if not cmd:
            LogReport('disable-device-work: Can not find command "disable-device-work"')
            return
        try:
            cmd.run([ns.device])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_enable_device_work(self):
        parser = self.cmd_parsers['enable_device_work'] = argparse.ArgumentParser(
                prog='enable_device_work',
                description='enable_device_work',
                add_help=False)

        parser.add_argument(
                'device', 
                metavar='<device-name>',
                help='device name',
                )

        return parser

    def do_enable_device_work(self, arg):
        parser = self.get_argparser('enable_device_work')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("enable-device-work")
        if not cmd:
            LogReport('enable-device-work: Can not find command "enable-device-work"')
            return
        try:
            cmd.run(arg_list)
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_configure_write(self):
        parser = self.cmd_parsers['configure_write'] = argparse.ArgumentParser(
                prog='configure_write',
                description='configure_write',
                add_help=False)

        parser.add_argument(
                'snapshot', 
                metavar='<vmware-snapshot>',
                help='vmware snapshot',
                )

        return parser

    def do_configure_write(self, arg):
        parser = self.get_argparser('configure_write')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("write-configure")
        if not cmd:
            LogReport('write-configure: Can not find command "write-configure"')
            return 
        try:
            cmd.run([ns.snapshot])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_configure_read(self):
        parser = self.cmd_parsers['configure_read'] = argparse.ArgumentParser(
                prog='configure_read',
                description='configure_read',
                add_help=False)

        parser.add_argument(
                'snapshot', 
                metavar='<snapshot>',
                help='snapshot',
                )

        return parser

    def do_configure_read(self, arg):
        parser = self.get_argparser('configure_read')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("read-configure")
        if not cmd:
            LogReport('read-configure: Can not find command "read-configure"')
            return 
        try:
            cmd.run([ns.snapshot])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_open_instr_record(self):
        parser = self.cmd_parsers['open_instr_record'] = argparse.ArgumentParser(
                prog='open_instr_record',
                description='configure_read',
                add_help=False)

        parser.add_argument(
                'cpu', 
                metavar='<cpu-name>',
                help='cpu name',
                )

        parser.add_argument(
                'log_file', 
                metavar='<log-file>',
                help='the log file path',
                )

        return parser

    def do_open_instr_record(self, arg):
        parser = self.get_argparser('open_instr_record')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("open-instr-record")
        if not cmd:
            LogReport('open-instr-record: Can not find command "open-instr-record"')
            return
        try:
            cmd.run([ns.cpu, ns.log_file])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_close_instr_record(self):
        parser = self.cmd_parsers['close_instr_record'] = argparse.ArgumentParser(
                prog='close_instr_record',
                description='close_instr_record',
                add_help=False)

        parser.add_argument(
                'cpu', 
                metavar='<cpu-name>',
                help='cpu name',
                )

        return parser

    def do_close_instr_record(self,arg):
        parser = self.get_argparser('close_instr_record')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("close-instr-record")
        if not cmd:
            LogReport('close-instr-record: Can not find command "close-instr-record"')
            return
        try:
            cmd.run([ns.cpu])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_set_pc(self):
        parser = self.cmd_parsers['set_pc'] = argparse.ArgumentParser(
                prog='set_pc',
                description='set_pc',
                add_help=False)

        parser.add_argument(
                'cpu', 
                metavar='<cpu-name>',
                help='cpu name',
                )

        parser.add_argument(
                'value', 
                metavar='<value>',
                type=convert_int,
                help='value',
                )

        return parser

    def do_set_pc(self,arg):
        parser = self.get_argparser('set_pc')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        try:
            ret = SkyEyeSetPC(ns.cpu, ns.value)
            if ret == 0:
                print ("SkyEyeSetPC Failed!")
        except:
            print("set-pc: SkyEyeSetPC CALL ERROR!")
            raise

    def create_argparser_get_pc(self):
        parser = self.cmd_parsers['get_pc'] = argparse.ArgumentParser(
                prog='get_pc',
                description='get_pc',
                add_help=False)

        parser.add_argument(
                'cpu', 
                metavar='<cpu-name>',
                help='cpu name',
                )

        return parser

    def do_get_pc(self,arg):
        parser = self.get_argparser('get_pc')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        cmd = cli.GetCommand("get-pc")
        if cmd == None:
            LogReport('get-pc: Can not find command "get-pc"')
            return
        try:
            cmd.run([ns.cpu])
        except Exception as e:
            print(e, cmd.synopses(), cmd.doc())

    def create_argparser_mm_info(self):
        parser = self.cmd_parsers['mm_info'] = argparse.ArgumentParser(
                prog='mm_info',
                description='mm_info',
                add_help=False)

        parser.add_argument(
                'func', 
                nargs='?',
                default='',
                metavar='<func-name>',
                help='function name',
                )

        return parser

    def do_mm_info(self, arg):
        parser = self.get_argparser('mm_info')
        try:
            ns = parser.parse_args(arg.split())
        except:
            return

        SkyEyeMemoryInfo(ns.func)

    def do_shell(self, arg):
        return self.exec_py_cmd(arg)

    def exec_py_cmd(self, cmd):
        try:
            exec(cmd)
        except:
            import traceback
            print(traceback.format_exc())

################################################################################
# util func
def convert_int(x):
    return int(x, 0)
convert_int.__name__ = 'int'

def table_print(l):
    COL_NUM = 5
    ITEM_WIDTH = 20
    for i in range(0, len(l), COL_NUM):
        print(' '.join([item.ljust(ITEM_WIDTH) for item in l[i:i+COL_NUM]]))

def welcome_message():
    os_info = platform.system()
    if operator.eq(os_info, "Linux"):
        system = "Linux"
    else:
        system = "Windows"

    try:
        config = tools.GetPackageConfig()
        version = tools.GetBuildVersion(config)
        date = tools.GetBuildDate(config)
    except:
        version = "unknown"
        date = "unknown"
    welcome_message = "SkyEye %s <build %s %s> Copyright 2010-2022 Digiproto Corporation\n" % (version, date, system)
    print(welcome_message)
