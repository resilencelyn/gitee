# tag1 -> basic functions;
# tag2 -> add yolov5;
# tag3 -> add sending files;
# tag4 -> ...
# tag5 -> ...
# tag6 -> ...
# .......
# .........


"""
utf-8

the main method of the project includes: (MAIN.).

1.click callback method.
2.the ui class.
3.adb shell method.
4.main method (__main__).



"""


# import modules.
import re
import win32con
import win32api
from glob import glob
from matplotlib.pyplot import connect
import matplotlib.pyplot as plt
# import pygame
import math
import qt5_tools, qt5_applications
import wx  # wxpython.
import cv2  # opencv.
# import pymysql
import numpy as np
import uiautomator2 as u2
import os
import sys
import time
from time import *
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))  # add roads.
sys.path.append(BASE_DIR + "\\test_phone")
sys.path.append(BASE_DIR)
import connecting
import uiclass
from uiclass import *
from connecting import *
import warnings  # warnings.filterwarnings('error') 
import subprocess
from subprocess import *
import scipy
import pandas
# import pickletools
import yolov5_using
# import yolov5_using.using_yolov5
from yolov5_using import *
from yolov5_using import using_yolov5 
from yolov5_using import using_yolov5 as yloo5
# import the yolov5 running files.


def on_EVENT_LBUTTONDOWN(event, x, y, flags, param):
    """
    the onclick method.(buttoncallback.). 
    """
    xy = "(%d,%d)." % (x, y) 
    # get the position.
    global phone_device, shrink

    if event == cv2.EVENT_LBUTTONDOWN:
        """L.
        left button -> click the dot.
        L."""
        cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)  # circle
        # 1.1.circle.  # draw the cricle
        cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 0), thickness=1)  # put text
        # 1.2.text.  # put the text on the picture
        cv2.imshow("image", shrink)  # show picture
        # 1.3.show.
        print(x,y)

        phone_device.click(x / 0.4, y / 0.4)
        # the most inportant code.
        return None
        # this can speed up.


    if event == cv2.EVENT_RBUTTONDOWN:
        """R.
        right button -> back.
        R."""
        cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # 2.1.circle.
        cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 0), thickness=1)
        # 2.2.text.
        cv2.imshow("image", shrink)
        # 2.3.show.
        print(x,y)    

        phone_device.press("back")
        return None


    if event == cv2.EVENT_MBUTTONDOWN:
        """M.
        middle button -> home.
        M."""
        cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # 3.1.circle.
        cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 0), thickness=1)
        # 3.2.text.
        cv2.imshow("image", shrink)
        # 3.3.show.
        print(x,y)   

        phone_device.press("home")
        return None


    if event == cv2.EVENT_MOUSEWHEEL:
        """MW.
        mouse's wheel -> scroll.
        MW."""
        cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # 4.1.circle.
        cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 0), thickness=1)
        # 4.2.text.
        cv2.imshow("image", shrink)
        # 4.3.show.
        print(x,y) 

        if flags > 0:  # flags -> scrolling 
            print('turn forward.')
            phone_device.swipe(540, 400, 540, 1600)
            # (540, 400) -> (540, 1600) (use this to scroll)
            pass
        
        else:
            print('turn backward.')
            phone_device.swipe(540, 1600, 540, 400)
            pass

        return None


    if event == cv2.EVENT_LBUTTONDBLCLK:
        """DL.
        double click left button -> turn left.
        DL."""
        cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # 5.1.circle.
        cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 0), thickness=1)
        # 5.2.text.
        cv2.imshow("image", shrink)
        # 5.3.show.
        print(x,y)
        print('double click left button.')

        phone_device.swipe(150, 1000, 850, 1000)
        # (150, 1000) -> (850, 1000) (use this to scroll.).

        return None


    if event == cv2.EVENT_MBUTTONDBLCLK:
        """DM.
        click the double middle button -> quick settings.
        DM."""
        cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # 6.1.circle.
        cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 0), thickness=1)
        # 6.2.text.
        cv2.imshow("image", shrink)
        # 6.3.show.
        print(x,y)
        print('double click middle button.')

        phone_device.open_quick_settings()  # quick settings.
        return None


    if event == cv2.EVENT_RBUTTONDBLCLK:
        """DR.
        double click right button -> turn right.
        DR."""
        cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # 7.1.circle.
        cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 0), thickness=1)        
        # 7.2.text.
        cv2.imshow("image", shrink)
        # 7.3.show.
        print(x,y)
        print('double click right button.')

        phone_device.swipe(850, 1000, 150, 1000)
        return None


class CalcFrame1(MyFrame1):
    """
    ui class -> main ui.
    """

    global phone_device, shrink
    global phone_device_uid, phone_device_ip_eare  # global variables.

    def __init__(self, parent):
        """
        init method -> son class.
        initial set:
        1.ip.
        2.pd_number_first_define0.
        """

        global phone_device_uid, phone_device_ip_eare
        MyFrame1.__init__(self, parent)
        # variables.
        self.m_textCtrl1.SetValue(phone_device_ip_eare)
        self.pd_number_first_define0 = 0  
        # intial is 0.
        # define a variable to make the stopping below simple.

    # Virtual event handlers, overide them in your derived class.


    def set_initial_parameters_of_phone(self):
        """
        (the method of the son class.) set the initial ip and port -> has done in the __init__ method.
        return none.
        """

        global phone_device_uid, phone_device_ip_eare
        self.m_spinCtrl1.SetValue(phone_device_ip_eare)
        # IP
        return None


    def m01to_connectthe_cap(self, event):
        global phone_device
        """
        m01.
        to connect the computer.
        """
        # pass

        phoneIP = self.m_textCtrl1.GetValue()
        phoneport = self.m_textCtrl2.GetValue()
        # get the messages.
        """cmd command."""
        # here will not really connect, because it has connected before, here only need to test.
        cmd_command_1 = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb devices"
        # check the connection.

        res1 = os.system(cmd_command_1)  # print(res1)
        res2 = os.popen(cmd_command_1).read()  # must read.

        if res1 == 0 and res2[-8:-2] == "device":  # the network is fine.
            time.sleep(1.25)  
            # to make the connection seems real.
            wx.MessageBox(f"The correction state is true.", "Message" ,wx.OK | wx.ICON_INFORMATION)  # just check the connection.
        else:  # the network is terriable.
            wx.MessageBox(f"The correction state is false, please check the network state.", "Message" ,wx.OK | wx.ICON_INFORMATION)  
        # check the network.
        
        event.Skip()
        return super().m01to_connectthe_cap(event)
        # super -> father class.


    def m02checkatx(self, event):
        global phone_device
        """
        m02.
        to check the atx-agent.
        """
        # pass

        try:  # just start the atx again.
            # check the atx-agent.
            u2_cmd00 = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb shell /data/local/tmp/atx-agent server -d&&exit"  # '/data/local/tmp/atx-agent server -d'"  # &&/data/local/tmp/atx-agent server -d"
            os.system(u2_cmd00)
            # run the cmd commands.
        except Exception as e:
            wx.MessageBox(f"The atx-agent (running) state is false.", "Message" ,wx.OK | wx.ICON_INFORMATION) 
            print("error at m02checkatx.")
            pass
        else:  # no problems -> already start.
            wx.MessageBox(f"The atx-agent (running) state is true.", "Message" ,wx.OK | wx.ICON_INFORMATION) 
            pass
        finally:
            pass 

        event.Skip()
        return super().m02checkatx(event)


    def m03startatx(self, event):
        """
        m03.
        start the atx-agent.
        """
        # pass

        try:  # just connect again and start the atx-agent again.
            # almost the same as the code above.
            u2_cmd00 = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb shell /data/local/tmp/atx-agent server -d&&exit"  # '/data/local/tmp/atx-agent server -d'"  # &&/data/local/tmp/atx-agent server -d"
            os.system(u2_cmd00)
        except Exception as e:
            wx.MessageBox(f"Starting error.", "Message" ,wx.OK | wx.ICON_INFORMATION) 
            print("error at m03startatx.")
            pass
        else:
            wx.MessageBox(f"The atx-agent has already successfully started.", "Message" ,wx.OK | wx.ICON_INFORMATION) 
            pass
        finally:
            pass

        event.Skip()
        return super().m03startatx(event)


    def m04startcontrol(self, event):
        """
        m04.
        start to control.(one of the most import methods.).
        """
        # pass.

        global phone_device, shrink

        IP_of_phone = self.m_textCtrl1.GetValue()
        port_of_phone = self.m_spinCtrl1.GetValue()
        server_id_of_phone = str(IP_of_phone) + ":" + str(port_of_phone)       
        print("IP: " + str(self.m_textCtrl1.GetValue()))
        print("port: " + str(self.m_spinCtrl1.GetValue()))
        print("Server: " + server_id_of_phone)

        phone_device = u2.connect(server_id_of_phone)
        # reconnect again.
        cv2.namedWindow("image")
        # name the window -> image.
        while True:  # while True.
            """
            while Ture -> always run.(opencv -> cv2.).
            """
            image = phone_device.screenshot(format="opencv")  # use the opencv format.
            # res = d.screenshot("test.jpg")  
            # img = cv2.imread('C:\\Users\\1\\Desktop\\test_phone\\test.jpg')
            height, width = image.shape[:2]  
            size = (int(width*0.4), int(height*0.4))  # update the size in order to show in the computer screen.
            shrink = cv2.resize(image, size, interpolation=cv2.INTER_AREA)
            """imshow."""
            cv2.imshow("image", shrink)
            """# cv2.setMouseCallback"""
            cv2.setMouseCallback("image", on_EVENT_LBUTTONDOWN)  # click method.
            # cv2.setMouseCallback("image", on_EVENT_LBUTTONDOWN)
            if (cv2.waitKey(1) &0xFF == ord('q')) or (self.pd_number_first_define0 == 1):  # press the q key to break the circle.
                # (q keyboard -> quit.) or (number == 1 -> quit.).
                """
                there are two methods:
                1.q pressed.
                2.number == 1.
                """
                self.pd_number_first_define0 = 0  # reset to zero(0)
                break

        # cv2.destroyAllWindows()
        cv2.destroyAllWindows()  
        # must destroy.
        # end the circle and then destroy all the windows.

        event.Skip()
        return super().m04startcontrol(event)


    # a,  here is an example:
    # def click_btn( self, event ):
    #     global d, shrink
    #     print(self.m_textCtrl1.GetValue())
    #     cv2.namedWindow("image")
    #     while True:
    #         # time.sleep(0.001)
    #         res = d.screenshot("test5.jpg")  
    #         # print(res)
    #         img = cv2.imread('C:\\Users\\1\\Desktop\\test_phone\\test5.jpg')
    #         height, width = img.shape[:2]  
    #         size = (int(width*0.4), int(height*0.4))  
    #         shrink = cv2.resize(img, size, interpolation=cv2.INTER_AREA)  
    #         cv2.imshow('image',shrink) 
    #         x0 = 0
    #         y0 = 0
    #         # x, y = 
    #         cv2.setMouseCallback("image", on_EVENT_LBUTTONDOWN)
    #         # print(x0, y0)
    #         if cv2.waitKey(1) &0xFF ==ord('q'):  #按q键退出
    #             break
    #     cv2.destroyAllWindows()
    #     event.Skip()        


    def m05endcontrol(self, event):
        """
        m05.
        end the control of the computer to the phone.
        we must click the picture first, and then press q button, however, i do not know how to do this autoly.
        """
        # pass.

        win32api.keybd_event(81,0,0,0)  # q keydown
        win32api.keybd_event(81,0,0,0)  # q keyup
        # click (or press) the q keyboard.
        self.pd_number_first_define0 = 1  # set the number to 1.
        # to start the control or not.

        event.Skip()
        return super().m05endcontrol(event)


    def m06openscreen(self, event):
        global phone_device
        # connecting.dev_open_screen() wrong.
        # connecting.dev_open_screen(dev0=phone_device) right.
        """.
        m06.
        open the screen.
        ."""

        # .m01.
        connecting.dev_open_screen(dev0=phone_device)  # dev_open_screen.
        # dev_open_screen.

        event.Skip()
        return super().m06openscreen(event)

    
    def m07clickback(self, event):
        global phone_device
        """.
        m07.
        press the button of back.
        ."""

        # .m02.
        connecting.dev_back(dev0=phone_device)  # dev_back.
        # dev_back.

        event.Skip()
        return super().m07clickback(event)


    def m08shownotifications(self, event):
        global phone_device
        """.
        m08.
        show the notifications.
        ."""

        # .m03.
        connecting.dev_notifications_showing(dev0=phone_device)  # dev_notifications_showing.
        # dev_notifications_showing.

        event.Skip()
        return super().m08shownotifications(event)


    def m09turnleft(self, event):
        global phone_device
        """.
        m09.
        turn left.
        ."""

        # .m04.
        connecting.dev_turn_left(dev0=phone_device)  # dev_turn_left.
        # dev_turn_left.

        event.Skip()
        return super().m09turnleft(event)


    def m10turnforward(self, event):
        global phone_device
        """.
        m10.
        turn forward.
        ."""

        # .m05.
        connecting.dev_turn_forward(dev0=phone_device)  # dev_turn_forward.
        # dev_turn_forward.

        event.Skip()
        return super().m10turnforward(event)


    def m11voiceup(self, event):
        global phone_device
        """.
        m11.
        turn the voice up.
        ."""

        # .m06.
        connecting.dev_voice_up(dev0=phone_device)   # dev_voice_up.
        # dev_voice_up.

        event.Skip()
        return super().m11voiceup(event)


    def m12novoice(self, event):
        global phone_device
        """.
        m12.
        shut up the voice.
        ."""

        # .m07.
        connecting.dev_voice_shuttingup(dev0=phone_device)   # dev_voice_shuttingup.
        # dev_voice_shuttingup.

        event.Skip()
        return super().m12novoice(event)


    def m13recentapps(self, event):
        global phone_device
        """.
        m13.
        show the rencent apps.
        ."""

        # .m08.
        connecting.dev_recent_apps(dev0=phone_device)  # dev_recent_apps.
        # dev_recent_apps.

        event.Skip()
        return super().m13recentapps(event)


    def m14runprocess(self, event):
        global phone_device
        """.
        m14.
        show the running processes.
        ."""

        # .m09.
        connecting.dev_running_processing(dev0=phone_device)   # dev_running_processing.
        # dev_running_processing.

        event.Skip()
        return super().m14runprocess(event)


    def m15stopallapps(self, event):
        global phone_device
        """.
        m15.
        stop all the apps.
        ."""

        # .m10.
        connecting.dev_stop_all_apps(dev0=phone_device)  # dev_stop_all_apps.
        # dev_stop_all_apps.

        event.Skip()
        return super().m15stopallapps(event)


    def m16closescreen(self, event):
        global phone_device
        """.
        m16.
        close the screen.
        ."""

        # .m11.
        connecting.dev_close_screen(dev0=phone_device)  # dev_close_screen.
        # dev_close_screen.

        event.Skip()
        return super().m16closescreen(event)


    def m17clickhome(self, event):
        global phone_device
        """.
        m17.
        click the button of the phone home.
        ."""

        # .m12.
        connecting.dev_home(dev0=phone_device)  # dev_home.
        # dev_home.

        event.Skip()
        return super().m17clickhome(event)


    def m18quicksettings(self, event):
        global phone_device
        """.
        m18.
        show the quick settings.
        ."""

        # .m13.
        connecting.dev_quick_settings(dev0=phone_device)  # dev_quick_settings.
        # dev_quick_settings.

        event.Skip()
        return super().m18quicksettings(event)


    def m19turnright(self, event):
        global phone_device
        """.
        m19.
        turn right.
        ."""

        # .m14.
        connecting.dev_turn_right(dev0=phone_device)  # dev_turn_right.
        # dev_turn_right.

        event.Skip()
        return super().m19turnright(event)


    def m20turnback(self, event):
        global phone_device
        """.
        m20.
        turn back.
        ."""

        # .m15.
        connecting.dev_turn_back(dev0=phone_device)  # dev_turn_back.
        # dev_turn_back.

        event.Skip()
        return super().m20turnback(event)


    def m21voicedown(self, event):
        global phone_device
        """.
        m21.
        turn the voice of the phone down.
        ."""

        # .m16.
        connecting.dev_voice_down(dev0=phone_device)  # dev_voice_down.
        # dev_voice_down.

        event.Skip()
        return super().m21voicedown(event)


    def m22clickpower(self, event):
        global phone_device
        """.
        m22.
        click the button of the power.
        ."""

        # .m17.
        connecting.dev_press_power(dev0=phone_device)  # dev_press_power.
        # dev_press_power.

        event.Skip()
        return super().m22clickpower(event)


    def m23healthprocessing(self, event):
        global phone_device
        """.
        m23.
        look the healthy processing.
        ."""

        # .m18.
        connecting.dev_check_processing(dev0=phone_device)  # dev_check_processing.
        # dev_check_processing.

        event.Skip()
        return super().m23healthprocessing(event)
    

    def m24settingsmsg(self, event):
        global phone_device
        """.
        m24.
        look the settings massage.
        ."""

        # .m19.
        connecting.dev_show_the_phone_settings(dev0=phone_device)  # dev_show_the_phone_settings.
        # dev_show_the_phone_settings.

        event.Skip()
        return super().m24settingsmsg(event)

    
    def m25phonemsg(self, event):
        global phone_device
        """.
        m25.
        show the phone details.
        ."""

        # .m20.
        connecting.dev_show_phone_detail_mesages(dev0=phone_device)  # dev_show_phone_detail_mesages.
        # dev_show_phone_detail_mesages.

        event.Skip()
        return super().m25phonemsg(event)


    def m26sending_texts(self, event):
        global phone_device
        """
        m26.
        send the messages you want to send to the text dialogs.
        """
        text_of_send = self.m_textCtrl2.GetValue()  # get the content of the text.
        # phone_device.set_fastinput_ime(True)
        phone_device.clear_text()

        try:
            phone_device.send_keys(text_of_send)  # try to send.
        except Exception as e:
            # the error here most probably is caused by not selecting the dialogue box. 
            wx.MessageBox(f"Please select the dialogue box.", "Message" ,wx.OK | wx.ICON_INFORMATION)  
            print("error at m26sending_texts.")
            pass
        else:
            try:
                phone_device.press("enter")  
                # press enter.
                # phone_device.keyevent("enter")
            except Exception as e:
                # the error here is caused by incorrect settings.
                wx.MessageBox(f"Please set the 'press enter to send'.", "Message" ,wx.OK | wx.ICON_INFORMATION)
                print("error at m26sending_texts.")
                pass
            else:
                self.m_textCtrl2.SetValue("")  
                # clear.
                pass
            finally:
                pass
            pass

        finally:
            # phone_device.set_fastinput_ime(False)
            pass

        event.Skip()
        return super().m26sending_texts(event)


    def m27phonefileslooking(self, event):
        global phone_device
        """
        m27.
        tool -> show the phone files.
        """

        event.Skip()
        return super().m27phonefileslooking(event)


    def m28usephoneyolov5(self, event):
        global phone_device
        """
        m28.
        tool -> yolov5.(use the yolov5 to identify the things..)..
        the tool of the yolov5 detcet.py. -> yolo!!
        """

        if self.m_toggleBtn2.GetValue() == True:  # true..
            # click -> true..(click again -> false.).
            # yloo5() -> yolov5..
            yloo5()  # use the yloo5() method...
            # run the code..
            pass
        else:
            # false..
            wx.MessageBox("Please click the the button again.", "Message" ,wx.OK | wx.ICON_INFORMATION)
            # need to click again..
            pass  
        pass


        event.Skip()
        return super().m28usephoneyolov5(event)

    
    def m29appintroductions(self, event):
        global phone_device
        """
        m29.
        app introductions.
        """

        introductions_of_app = "  这个软件的中文名称是: 智慧操控, (智慧操控: 基于uiautomator2模块的电脑操控手机的功能的实现), 英文的名称是Windows Intelligently Controlling Android , 英文缩写是WICA. 该软件的功能是通过电脑来对手机进行操控, 这样一来, 很多的手机操作可以在电脑上直接完成, 不需要切换两个设备就可以完成自己想要的功能, 十分方便快捷. 该软件可以较为灵敏的通过电脑来实现控制手机的功能, 最后, 祝您使用愉快! "  #  ！！"
        # use the dots of english because chinese dot does not match.

        # wx -> messages.
        wx.MessageBox(f"{introductions_of_app}", "Message" ,wx.OK | wx.ICON_INFORMATION)  
        # wxpython.

        event.Skip()
        return super().m29appintroductions(event)


    def m30usingintroductions(self, event):
        global phone_device
        """
        m30.
        how to use.
        """

        # using mathods.
        using_1 = "1. 首先打开软件到达当前的这个页面说明手机以及电脑的配置都是合适的, 可以正常使用这个软件了; "
        using_2 = "2. 然后, 点击确认连接按钮可以将电脑与手机连接起来 (这里的IP以及端口会自动获取, 不需要修改) ; "
        using_3 = "3. 然后, 点击检查ATX以及开启ATX可以分别检查ATX是否已经开启服务或者直接开启ATX的服务; "
        using_4 = "4. 完成以上的操作之后, 就实现了电脑与手机连接以及开启 (或者检查) ATX服务的功能, 之后, 我们就可以点击开始操控按钮来实现电脑对手机的操控了, 操控结束以后点击结束操控按钮可以终止操控;"
        using_5 = "5. 在使用电脑操控手机的过程中, 可能会存在一些手机操作电脑不太容易实现, 因此, 在当前界面的中间部分以及右边的部分设置有一些比较快捷的按钮来方便用户的操控 (比如: 通知栏、快速设置、最近应用、发送消息等等) ; "
        using_6 = "6. 此外, 我们还对这个软甲添加了一些附加的功能, 比如: 手机文件管理系统、YOLOv5识别图像、音乐播放器、手机软件管理等等, 这些功能都可以在右边的部分的下半部分或者工具里面找到, 点击按钮就可以直接使用; "
        using_7 = "7. 以上就是使用该软件的方法, 如果还有问题请联系作者或者获取其他帮助, 最后, 感谢下载与使用, 祝您使用愉快! "
        # how to use. 
        wx.MessageBox(f" {using_1} \n {using_2} \n {using_3} \n {using_4} \n {using_5} \n {using_6} \n {using_7} ", "Message" ,wx.OK | wx.ICON_INFORMATION)  
        # show the messages of using the software.

        event.Skip()
        return super().m30usingintroductions(event)


    def m31helpings(self, event):
        global phone_device
        """
        m31.
        helps.(other helps.).
        """

        event.Skip()
        return super().m31helpings(event)


    def m32sendingfiles(self, event):
        global phone_device
        """
        m32.
        send files.
        """

        event.Skip()
        return super().m32sendingfiles(event)


    def m33appsmanager(self, event):
        global phone_device
        """
        m33.
        app managing.
        """

        event.Skip()
        return super().m33appsmanager(event)


    def m34processinglooking(self, event):
        global phone_device
        """
        m34.
        look the process.
        """

        event.Skip()
        return super().m34processinglooking(event)


    def m35musicvideo(self, event):
        global phone_device
        """
        m35.
        music.
        """

        event.Skip()
        return super().m35musicvideo(event)


    def m36authormessage(self, event):
        global phone_device
        """
        m36.
        show the author's message to the users.
        """

        # basic messages.
        author_msges = " Author: 胡宇轩 (Y.X.Hu) \n School: BUPT \n Class ID: 2020211115 \n Student ID: 2020210449 \n CSDN Blog: https://hyxmoon.blog.csdn.net/ \n GitHub: https://github.com/Huyuxuan-github \n Gitee: https://gitee.com/hhhmoonhhh"
        wx.MessageBox(f"{author_msges}", "Message" ,wx.OK | wx.ICON_INFORMATION)  
        # show the message of the author.

        event.Skip()
        return super().m36authormessage(event)


    def m37moneytheproject(self, event):
        global phone_device
        """
        # m37.
        money the project.
        """

        event.Skip()
        return super().m37moneytheproject(event)


def adb_shell(cmd: str):  # string -> str.
    """
    run the adb shell.
    return sh.
    """

    sh = subprocess.Popen(cmd, stdin=subprocess.PIPE)
    return sh


# for an example:(to run the adb shell.)
# def adb_shell(cmd):
#     sh = subprocess.Popen(cmd,stdin=subprocess.PIPE)
#     return sh


if __name__ == "__main__":
    """.the main method.(MAIN.)."""

    # start the app first.
    app = wx.App(False)

    # set the language of the apps(language.).
    app.locale = wx.Locale(wx.LANGUAGE_CHINESE_SIMPLIFIED)
    # including the chinese language.

    # the first time to connect.
    while True:   # while.
        # test the adb connection.
        time.sleep(1.21)

        try:
            # try to connect adb.
            cmd_command_0 = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb devices"
            res = os.system(cmd_command_0)
            # print(res)
            res2 = os.popen(cmd_command_0).read()

            print(res2)
            print(res2[-8:-2])

        except Exception as e:
            wx.MessageBox(f"Please connect the phone with wire first.", "Message" ,wx.OK | wx.ICON_INFORMATION)  
            # at the first time, we should connect the phone using wire.
            print("wait until connect the phone...")
            pass

        else:
            # must install the uiautomator2.(ATX.apk).
            if res == 0 and res2[-8:-2] == "device":
                # checku2_cmd_command = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb shell exit"
                # test the cmd.(no need to use)

                """
                check how many devices are on the pc.
                """
                # re module.
                res_lists = re.findall(r"device", res2)
                if len(res_lists) == 3:  # 3 devices -> error caused by wire connection.
                    wx.MessageBox(f"Please disconnect the phone with the wire.", "Message" ,wx.OK | wx.ICON_INFORMATION)  
                    time.sleep(6.2)  # if error, then check and wait, or, no need to do so.
                    # normal people can disconnect the python with wire in 6 seconds.

                cmd123 = f"cd&&python -m uiautomator2 init"
                # init u2 (install.).
                os.system(cmd123)
                # install the ATX.apk(atx-agent) (init -> install.).

                break

            else:
                wx.MessageBox(f"Please connect the phone with wire first.", "Message" ,wx.OK | wx.ICON_INFORMATION)  
            pass

        finally:
            pass    

    # start the atx server.
    u2_cmd = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb shell /data/local/tmp/atx-agent server -d&&exit"  # '/data/local/tmp/atx-agent server -d'"  # &&/data/local/tmp/atx-agent server -d"
    #  /data/local/tmp/atx-agent server -d
    # os.system(u2_cmd)
    os.system(u2_cmd)  # run the code.
    # /data/local/tmp/atx-agent server -d
    # adb_sh = adb_shell(u2_cmd)
    # adb_sh.communicate('/data/lo/cal/tmp/atx-agent server -d')

    # get the information.(ip and so on.)
    phone_device = u2.connect()  # connect to get the message of the phone.
    # phone_device = u2.connect("192.168.0.104:5555")
    print(phone_device.device_info["serial"])  # print(phone_device.device_info)
    print(phone_device.wlan_ip)
    phone_device_uid = phone_device.device_info["serial"]
    phone_device_ip_eare = phone_device.wlan_ip

    # the second time to connect.
    cmd_command_000 = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb devices&&adb tcpip 5555&&adb kill-server&&adb connect {phone_device_ip_eare}:5555"
    os.system(cmd_command_000)  # reconnect.
    # reconnect the adb.  # https://hyxmoon.blog.csdn.net/

    wx.MessageBox(f"Please disconnect the phone with wire if you do not disconnect.", "Message" ,wx.OK | wx.ICON_INFORMATION)  
    # need to be checked.

    # u2_cmd = f"cd&&cd tools&&cd adbtool&&cd platform-tools_r32.0.0-windows&&cd platform-tools&&adb shell /data/local/tmp/atx-agent server -d"  # '/data/local/tmp/atx-agent server -d'"  # &&/data/local/tmp/atx-agent server -d"
    #  /data/local/tmp/atx-agent server -d
    # os.system(u2_cmd)
    os.system(u2_cmd)
    # os.system(u2_cmd)
    # start the atx-agent again.

    phone_device = u2.connect(f"{phone_device_ip_eare}:5555")
    # reconnect the server.

    # notifications phone messages.
    wx.MessageBox(f"Your phone's uid is {phone_device_uid}", "Message" ,wx.OK | wx.ICON_INFORMATION)
    wx.MessageBox(f"Your phone's ip is {phone_device_ip_eare}", "Message" ,wx.OK | wx.ICON_INFORMATION) 

    # to connect the device.

    image = phone_device.screenshot(format="opencv")  
    # use the opencv format.
    # res = d.screenshot("test.jpg")  # print(res)
    # img = cv2.imread('C:\\Users\\1\\Desktop\\test_phone\\test.jpg')
    height, width = image.shape[:2]  
    size = (int(width*0.4), int(height*0.4))  # update the size in order to show in the computer screen.
    shrink = cv2.resize(image, size, interpolation=cv2.INTER_AREA)  # update the size (resize.)
    
    """example:
    app = wx.App(False)
    frame = CalcFrame(None)
    frame.Show(True)  # start the applications
    app.MainLoop()
    """  # move to above.
    # app = wx.App(False)    
    # app.locale = wx.Locale(wx.LANGUAGE_CHINESE_SIMPLIFIED)
    # set the language.
    frame = CalcFrame1(None)
    frame.Show(True)
    app.MainLoop()  
    # mainloop -> execute the apps.
    # return None.  # pass.

