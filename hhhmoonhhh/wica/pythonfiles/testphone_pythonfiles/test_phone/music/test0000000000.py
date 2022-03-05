# file:A_simple_music_tool.py
# Author : westwind
 
import wx
from win32com.client import Dispatch
from wx.lib.wordwrap import wordwrap
 
# Create a Menu Class
class CreateMenu():
       def __init__(self,parent):
              self.menuBar=wx.MenuBar()      # create menubar
              # create "File" menuitem
              self.menufile=wx.Menu()
              self.menuBar.Append(self.menufile,"文件(&F)")
              self.open=self.menufile.Append(-1,"打开","Open a new music file")
              self.exit=self.menufile.Append(-1,"退出","Exit the MiNiPlayer")
             
              # create "Play" menuitem
              self.menuplay=wx.Menu()
              self.menuBar.Append(self.menuplay,"播放(&P)")
              self.play=self.menuplay.Append(-1,"播放","Play the music file")
              self.pause=self.menuplay.Append(-1,"暂停","pause")
              self.next=self.menuplay.Append(-1,"下一个","Play Next song")
              self.stop=self.menuplay.Append(-1,"停止","stop")
              self.menuplay.AppendSeparator()
             
              #Volume Control menu
              submenu = wx.Menu()
              self.volumeDown=submenu.Append(-1,"下一个","Decrease volume")
              self.volumeUp=submenu.Append(-1,"上一个","Increase volume")
              self.volumeMute=submenu.Append(-1,"静音","NO voice")
              self.menuplay.AppendMenu(-1, "音量", submenu,"Control the volume")
              self.menuplay.AppendSeparator()
              self.option=self.menuplay.Append(-1,"选项...","Display Option")
             
              self.view=wx.Menu()
              self.menuBar.Append(self.view,"视图(&V)")
              #create "Help" menuitem
              self.menuhelp=wx.Menu()
              self.menuBar.Append(self.menuhelp,"帮助(&H)")
              self.about=self.menuhelp.Append(-1,"关于","Information about this programe")
             
              parent.SetMenuBar(self.menuBar)  # Display MenuBar
              
# subclass of Frame
class MyFrame(wx.Frame):
       def __init__(self,parent,id):
              wx.Frame.__init__(self,parent,id,title='MiNiPlayer',size=(650,460))
              self.statusBar=self.CreateStatusBar()  # Display StatusBar
              self.toolBar=self.CreateToolBar()
            #   self.toolBar_open=self.toolBar.AddSimpleTool(-1,wx.Image('./bitmap/open.png',wx.BITMAP_TYPE_PNG).ConvertToBitmap(),"open","Open a new file")
            #   self.toolBar_play=self.toolBar.AddSimpleTool(-1,wx.Image('./bitmap/play.png',wx.BITMAP_TYPE_PNG).ConvertToBitmap(),"play","Start to play")
            #   self.toolBar_next=self.toolBar.AddSimpleTool(-1,wx.Image('./bitmap/next.png',wx.BITMAP_TYPE_PNG).ConvertToBitmap(),"Next","play next file")
            #   self.toolBar_stop=self.toolBar.AddSimpleTool(-1,wx.Image('./bitmap/stop.png',wx.BITMAP_TYPE_PNG).ConvertToBitmap(),"Stop","Stop the music")
            #   self.toolBar_tools=self.toolBar.AddSimpleTool(-1,wx.Image('./bitmap/tool.png',wx.BITMAP_TYPE_PNG).ConvertToBitmap(),"Tools","Tools")
              self.toolBar.Realize()
              self.panel=wx.Panel(self,-1)
              self.panel.SetBackgroundColour('yellow')
             
              # load an icon for Frame
            #   icon = wx.Icon('./bitmap/miniplayer.ico', wx.BITMAP_TYPE_ICO)
            #   self.SetIcon(icon) 
              #self.listctrl=wx.ListCtrl(self.panel,id=-1,pos=(490,15),size=(-1,-1))
             
# subclass of application             
class MyApp(wx.App):
       def OnInit(self):
              self.locale = wx.Locale(wx.LANGUAGE_ENGLISH)
              self.frame=MyFrame(parent=None,id=-1)
              
              #self.panel=wx.Panel(self.frame,-1)
              self.menu=CreateMenu(self.frame)                          # Bind event
              self.Bind(wx.EVT_MENU,self.OnOpen,self.menu.open)
              self.Bind(wx.EVT_MENU,self.OnPlay,self.menu.play)
              self.Bind(wx.EVT_MENU,self.OnPause,self.menu.pause)
              self.Bind(wx.EVT_MENU,self.OnNext,self.menu.next)
              self.Bind(wx.EVT_MENU,self.OnStop,self.menu.stop)
              self.Bind(wx.EVT_MENU,self.OnAbout,self.menu.about)
              self.Bind(wx.EVT_MENU,self.OnExit,self.menu.exit)
              self.Bind(wx.EVT_MENU,self.OnVolumeDown,self.menu.volumeDown)
              self.Bind(wx.EVT_MENU,self.OnVolumeUp,self.menu.volumeUp)
              self.Bind(wx.EVT_MENU,self.OnVolumeMute,self.menu.volumeMute)
            #   self.Bind(wx.EVT_TOOL,self.OnOpen,self.frame.toolBar_open)
            #   self.Bind(wx.EVT_TOOL,self.OnPlay,self.frame.toolBar_play)
            #   self.Bind(wx.EVT_TOOL,self.OnStop,self.frame.toolBar_stop)
            #   self.Bind(wx.EVT_TOOL,self.OnNext,self.frame.toolBar_next)
              self.wmp=Dispatch('WMPlayer.OCX')
              self.frame.Show()
              self.SetTopWindow(self.frame)
              return True
       def OnOpen(self,event):
              dialog=wx.FileDialog(None,"open",style=wx.OPEN,wildcard="*.wav|*.mp3")
              if dialog.ShowModal()==wx.ID_OK:
                     self.filename=dialog.GetPath()
                     if self.filename:
                            media=self.wmp.newMedia(self.filename)
                            self.wmp.currentPlaylist.appendItem(media)
       def OnPlay(self,event):
              self.wmp.controls.play()
       def OnPause(self,event):
              self.wmp.controls.pause()
       def OnNext(self,event):
              self.wmp.controls.next()
       def OnStop(self,event):
              self.wmp.controls.stop()
             
       # Volume Control
       def OnVolumeDown(self,event):
              pass
       def OnVolumeUp(self,event):
              pass
       def OnVolumeMute(self,event):
              pass
             
             
       # Exit MiNiPlayer
       def OnExit(self,event):
              self.frame.Close()
             
       # About Dialog
       def OnAbout(self,event):
              info = wx.AboutDialogInfo()
              info.Name = "MiNiPlayer"
              info.Version = "1.0.0"
              info.Copyright = "(C) 2017 Programmers and Coders Everywhere"
              info.Description = wordwrap(
                     "A \"MiNiPlayer \" program is a software program that can play somekinds of music "
                     "just like MediaPlayer. It is used in many introductory "
                     "tutorials for teaching a programming language."
                
                     "\n\n    Such a program is typically one of the simplest programs possible "
                     "in a wxpython language. A \"MiNiPlayer\" program can be a useful "
                     "sanity test to make sure that a language's compiler, development "
                     "environment, and run-time environment are correctly installed.",
                     350, wx.ClientDC(self.frame))
              info.WebSite = ("http://blog.csdn.net/zhuzipan", "Welcome to my csdn Blog!")
              info.Developers = [ "westwind", ]
              # Then we call wx.AboutBox giving it that info object
              wx.AboutBox(info)
             
             
if __name__=='__main__':
       app=MyApp()
       app.MainLoop()