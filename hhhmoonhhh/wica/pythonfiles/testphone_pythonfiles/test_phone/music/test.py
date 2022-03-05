import os
# import pygame
from mutagen.mp3 import MP3
import time
from classuiui import MyFrame1
import wx
# import thread
import threading
import multiprocessing
from playsound import playsound
from test001 import MyFrame1

# file="cd&&cd music&&cd&&StarSky.mp3"
# os.system(file)

# pygame.mixer.init()
# audio = MP3("C:\\\\Users\\\\1\\\\Desktop\\\\test_phone\\\\music\\\\StarSky.mp3")
# track = pygame.mixer.music.load("C:\\\\Users\\\\1\\\\Desktop\\\\test_phone\\\\music\\\\StarSky.mp3")
# pygame.mixer.music.set_volume(0.7)

# pygame.mixer.music.play()
# print(audio.info.length)
# time.sleep(int(audio.info.length))
# pygame.mixer.music.stop()
# pygame.mixer.music.pause() #暂停
# pygame.mixer.music.unpause()#取消暂停
# 成功播放音乐，并有暂停，取消暂停功能。


# class CalcFrame(MyFrame1):


class CalcFrame(MyFrame1):
    def __init__(self, parent):
        MyFrame1.__init__(self, parent)

    def m1(self, event):
        p = multiprocessing.Process(target=playsound, args=("C:\\\\Users\\\\1\\\\Desktop\\\\test_phone\\\\music\\\\StarSky.mp3",))
        p.start()
        # input("press ENTER to stop playback")
        # p.terminate()
        p.join()
        event.Skip()
        return super().m1(event)


    def m2(self, event):
        global p
        p.terminate()
        event.Skip()
        return super().m2(event)


    def m3(self, event):
        event.Skip()
        return super().m3(event)





if __name__ == '__main__':
    """
    主函数
    """

    app = wx.App(False)
    frame = CalcFrame(None)
    frame.Show(True)
    # start the applications
    app.MainLoop()

    