


# """
# sending files............
# """
"""main of send files."""
# 
# 



import math
import wx.stc
import sys
import time
# from pip import main
import wx
from ui01 import MyFrame1
import os
import re
# import uiautomator2 as u2
import uiautomator2 as u2
import uiautomator2


class CalcFrame0001(MyFrame1):
    def __init__(self, parent):
        MyFrame1.__init__(self, parent)
        self.icon = wx.Icon(os.getcwd() + '\\pythonfiles\\other_tools\\favicon48.ico', wx.BITMAP_TYPE_ICO)
        self.SetIcon(self.icon) 
    # Virtual event handlers, overide them in your derived class.

    def sendingthepcfiletophone(self, event):
        """
        send the file to the phone..
        """

        global phone_device
        phone_device = u2.connect()  # connect to get the message of the phone.

        print(self.m_filePicker3.GetPath())
        # phone_device.push(self.m_filePicker3.GetPath(), "/sdcard/")
        # /sdcard/
        # d.push("foo.txt", "/sdcard/")


        phone_device.push(self.m_filePicker3.GetPath(), "/sdcard/")
        time.sleep(0.7)
        wx.MessageBox(f"Successfully send: {self.m_filePicker3.GetPath()}.", "Message" ,wx.OK | wx.ICON_INFORMATION)  


        event.Skip()
        return super().sendingthepcfiletophone(event)


def sendingfiletophonesmain():
    """
    main method of sending the file....
    MAIN method.
    """
    # phone_device = u2.connect()  # connect to get the message of the phone.

    app = wx.App(False)
    frame = CalcFrame0001(None)
    frame.Show(True)
    # start the applications
    app.MainLoop()


"""
below is the test.....
"""


# if __name__ == "__main__":

#     # """
#     # 主函数
#     # """

#     # print(os.getcwd())
#     # cmd_command_0 = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb devices"
#     # cmd_command_0 = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb devices"
#     # res2 = os.popen(cmd_command_0).read()
#     # res = os.system(cmd_command_0)
#     # print(res2)
#     # phone_device = u2.connect()  # connect to get the message of the phone.
#     sendingfiletophonesmain()



    

