# from unittest.main import main
import wx
import re
import uiautomator2 as u2
from ui03 import MyFrame0000000001
import os


class CalcFrame0001(MyFrame0000000001):
    def __init__(self, parent):
        MyFrame0000000001.__init__(self, parent)
        self.icon = wx.Icon(os.getcwd() + '\\pythonfiles\\other_tools\\favicon48.ico', wx.BITMAP_TYPE_ICO)
        self.SetIcon(self.icon) 
    # Virtual event handlers, overide them in your derived class.

    def lookallthe_apps0(self, event):
        d = u2.connect()
        # wx.MessageBox(f"Successfully.", "Message")  
        # wx.MessageBox(f"Successfully.", "Message") # wx.MessageBox(f"Successfully.", "Message")  
        # wx.MessageBox(f"Successfully.", "Message")  

        res0 = []
        for t in d.app_list():
            res0.append(t)

        n = 1
        str00 = ""
        for i in res0:
            if n < 10:
                str00 += f"00{n}.{i};\n"
            elif n < 100:
                str00 += f"0{n}.{i};\n"
            else:
                str00 += f"{n}.{i};\n"
            n += 1
        str00 = str00[:-2] + "."

        # print(str00)
        #        
        wx.MessageBox(f"{str00}", "Message" ,wx.OK | wx.ICON_INFORMATION)  
        event.Skip()
        return super().lookallthe_apps0(event)

    
    def lookrunning_apps0(self, event):
        d = u2.connect()
        str000 = ""
        str000 += str(d.app_current()["package"])
        str000 += "\n"
        # print(dev_phone0.app_info(dev_phone0.app_current()["package"]))# print(dev_phone0.app_info(dev_phone0.app_current()["package"]))
        # print(d.app_info(d.app_current()["package"]))
        sss = d.app_info(d.app_current()["package"])
        str000 += f'{sss}'

        wx.MessageBox(f"{str000}", "Message" ,wx.OK | wx.ICON_INFORMATION)  

        event.Skip()
        return super().lookrunning_apps0(event)


    def close_apps0(self, event):
        d = u2.connect()
        d.app_stop_all()

        event.Skip()
        return super().close_apps0(event)

    
    def install_apps0(self, event):
        d = u2.connect()
        app_to_install = self.m_filePicker1.GetPath()
        print(app_to_install)
        d.app_install(app_to_install)
        event.Skip()
        return super().install_apps0(event)

    
    def delete_apps0(self, event):
        d = u2.connect()
        app_to_uninstall = self.m_textCtrl1.GetValue()
        print(app_to_uninstall)
        d.app_uninstall(app_to_uninstall)
        event.Skip()
        return super().delete_apps0(event)

    

def apps_main_managing():
    app = wx.App(False)
    frame = CalcFrame0001(None)
    frame.Show(True)
    # start the applications
    app.MainLoop()

# if __name__ == "__main__":
    # apps_main_managing()


# dev_phone0 = u2.connect()
# print(dev_phone0.app_current())
# print(dev_phone0.app_list())
# # wx.MessageBox(f"Successfully.", "Message")  
# 
# res0 = []
# for t in dev_phone0.app_list():
#     res0.append(t)
# 
# n = 1
# str00 = ""
# for i in res0:
#     if n < 10:
#         str00 += f"00{n}.{i};\n"
#     elif n < 100:
#         str00 += f"0{n}.{i};\n"
#     else:
#         str00 += f"{n}.{i};\n"
#     n += 1
# str00 = str00[:-2] + "."
# print(str00)
# 
# 
# print(dev_phone0.app_info(dev_phone0.app_current()["package"]))

