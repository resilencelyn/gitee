"""
the methods after connecting the phone.
"""


# from tkinter.messagebox import NO
# from sys import ps1
# from tkinter import E
# no need above anymore.


import wx
import uiautomator2 as u2


def dev_open_screen(dev0):
    """
    m01.open screen.....
    return none........
    """
    try:
        dev0.screen_on()  
        # open the screen -> on
    except Exception as e:
        print("error at dev_open_screen.")
        pass
    else:
        pass
    finally:
        return None


def dev_close_screen(dev0):
    """
    m02.close screen.....
    return none........
    """
    try:
        dev0.screen_off()  
        # close the screen -> off
    except Exception as e:
        print("error at dev_close_screen.")
        pass
    else:
        pass
    finally:
        return None


def dev_back(dev0):
    """
    m03.press back.....
    return none........
    """
    try:
        dev0.press("back")  
        # press back
    except Exception as e:
        print("error at dev_back.")
        pass
    else:
        pass
    finally:
        return None


def dev_home(dev0):
    """
    m04.press home.....
    return none........
    """
    try:
        dev0.press("home")  
        # press home
    except Exception as e:
        print("error at dev_home.")
        pass
    else:
        pass
    finally:
        return None


def dev_notifications_showing(dev0):
    """
    m05.show the notifications.....
    return none.........
    """
    try:
        dev0.open_notification()
    except Exception as e:
        print("error at dev_notifications_showing.")
        pass
    else:   
        pass
    finally:
        return None


def dev_quick_settings(dev0):
    """
    m06.use the quick settings.....
    return none........
    """
    try:
        dev0.open_quick_settings()
    except Exception as e:
        print("error at dev_quick_settings.")
        pass
    else:
        pass
    finally:
        return None


def dev_turn_left(dev0):
    """
    m07.turn left.....
    return none........
    """
    try:
        dev0.swipe(150, 1000, 850, 1000)
        # (150, 1000) -> (850, 1000)
    except Exception as e:
        print("error at dev_turn_left.")
        pass
    else:
        pass
    finally:
        return None


def dev_turn_right(dev0):
    """
    m08.turn right.....
    return none.........
    """
    try:
        dev0.swipe(850, 1000, 150, 1000)
        # (850, 1000) -> (150, 1000)(turn right)
    except Exception as e:
        print("error at dev_turn_right.")
        pass
    else:
        pass
    finally:
        return None


def dev_turn_forward(dev0):
    """
    m09.turn forward.....
    return none........
    """
    try:
        dev0.swipe(540, 400, 540, 1600)
        # (540, 400) -> (540, 1600)
    except Exception as e:
        print("error at dev_turn_forward.")
        pass
    else:
        pass
    finally:
        return None


def dev_turn_back(dev0):
    """
    m10.turn back.....
    return none........
    """
    try:
        dev0.swipe(540, 1600, 540, 400)
        # (540, 1600) -> (540, 400)
    except Exception as e:
        print("error at dev_turn_back.")
        pass
    else:
        pass
    finally:
        return None


def dev_voice_up(dev0):
    """
    m11.turn the voice of phone up.....
    return none........
    """
    try:
        dev0.press("volume_up")
    except Exception as e:
        print("error at dev_voice_up.")
        pass
    else:
        pass
    finally:
        return None


def dev_voice_down(dev0):
    """
    m12.turn the voice of phone down.....
    return none........
    """
    try:
        dev0.press("volume_down")
    except Exception as e:
        print("error at dev_voice_down.")
        pass
    else:
        pass
    finally:
        return None


def dev_voice_shuttingup(dev0):
    """
    m13.shut up the voice of the phone.....
    return none........
    """
    try:
        dev0.press("volume_mute")
    except Exception as e:
        print("error at dev_voice_shuttingup.")
        pass
    else:
        pass
    finally:
        return None


def dev_press_power(dev0):
    """
    m14.press the button of power of the phone.....
    return none........
    """
    try:
        dev0.press("power")
    except Exception as e:
        print("error at dev_press_power.")
        pass
    else:
        pass
    finally:
        return None


def dev_recent_apps(dev0):
    """
    m15.show the apps recently run.....
    return none........
    """
    try:
        dev0.press("recent")
    except Exception as e:
        print("error at dev_recent_apps.")
        pass
    else:
        pass
    finally:
        return None


def dev_check_processing(dev0):
    """
    m16.check the processing of the phone.....
    return none........
    """
    try:
        res = dev0.healthcheck()

        if res == None:
            # the return result is none.
            print("healthing checking true.")
            # use the message to tell the user.
            wx.MessageBox(f"The state of health checking is true.", "Message" ,wx.OK | wx.ICON_INFORMATION) 
        else:
            print("no healthing checking.")
            # use this to tell the user.
            wx.MessageBox(f"The state of health checking is false.", "Message" ,wx.OK | wx.ICON_INFORMATION) 

    except Exception as e:
        print("error at dev_check_processing.")
        pass
    else:
        pass
    finally:
        return None


def dev_running_processing(dev0):
    """
    m17.show the running processings.....
    return none........
    """
    try:
        res = dev0.app_list_running()
        print(res)
        # use thw wx module.
        wx.MessageBox(f"{res}", "Message" ,wx.OK | wx.ICON_INFORMATION)  
        # use the message box to show the processes.
    except Exception as e:
        print("error at dev_running_processing.")
        pass
    else:
        pass
    finally:
        return None


def dev_show_the_phone_settings(dev0):
    """
    m18.show the settings of the phone.....
    return none........
    """
    try:
        res = dev0.settings
        print(res)
        # message box
        wx.MessageBox(f"{res}", "Message" ,wx.OK | wx.ICON_INFORMATION) 
        # use the message box to show the settings.
    except Exception as e:
        print("error at dev_show_the_phone_settings.")
        pass
    else:
        pass
    finally:
        return None


def dev_stop_all_apps(dev0):
    """
    m19.stop all the apps.....
    return none........
    """
    try:
        dev0.app_stop_all()
    except Exception as e:
        print("error at dev_stop_all_apps.")
        pass
    else:
        pass
    finally:
        return None


def dev_show_phone_detail_mesages(dev0):
    """
    m20.show the details of the messages of the phone.....
    return none........
    """
    try:
        res1 = dev0.info
        # info.
        res2 = dev0.device_info
        # device_info.
        res_dict0 = {
            "info": res1,
            "device_info": res2
        }
        # two features.
        print(res_dict0)
        # show the dict.
        wx.MessageBox(f"{res_dict0}", "Message" ,wx.OK | wx.ICON_INFORMATION) 
    except Exception as e:
        print("error at dev_show_phone_detail_mesages.")
        pass
    else:
        pass
    finally:
        return None






