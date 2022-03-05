import wx
from  ui02 import *
import os



class CalcFrame000002(MyFrame000001):
    def __init__(self, parent):
        MyFrame000001.__init__(self, parent)
        self.icon = wx.Icon(os.getcwd() + '\\pythonfiles\\other_tools\\favicon48.ico', wx.BITMAP_TYPE_ICO)
        self.SetIcon(self.icon) 

    # Virtual event handlers, overide them in your derived class




# if __name__ == '__main__':
    # """
    # 主函数
    # """
def otherhelpsoftheauthor():

    app = wx.App(False)
    frame = CalcFrame000002(None)
    frame.Show(True)
    # start the applications
    # app.MainLoop()
    app.MainLoop()


