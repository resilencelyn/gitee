# Music Control...
# Music Player:





# from glob import glob




from re import I
from tkinter import *
from tkinter import filedialog
from traceback import *
from win32com.client import Dispatch
import time,eyed3,threading
import os, sys
from sys import *
from os import *
import time




# from time import *
# do not import time *, or there will be errors, although i do not know why, bu do not do this....




# global variables.
name = []
list_name = None
var_scale = None
progress_scal = None
vio_scale = None
root = None




def openfile(index = [1]):
  """
  open the files.
  return none.
  """
  global total,name, list_name, wmp


  filenames = filedialog.askopenfilenames(title = "SelectingMusicPlayer.",filetypes =[("mp3Files.","*.mp3"),("WMAFiles.","*.wma"),("WAVFiles.","*.wav")])
  if filenames:
    for i in range(len(filenames)):
      media = wmp.newMedia(filenames[i])
      wmp.currentPlaylist.appendItem(media)

      print(filenames[i])
      coco = eyed3.load(filenames[i])
      # eyed3 module read mp3 messages..

      total = int(coco.info.time_secs)
      minute = int(coco.info.time_secs)//60
      sec = int(coco.info.time_secs)%60
      length = int(coco.info.time_secs)

      name = filenames[i].split("/")

      i =index[-1]
      list_name.insert(END,str(i)+"."+name[-1])
      list_name.insert(END," "*6)
      if sec >=10:
        list_name.insert(END,"0%d:%d" %(minute,sec)+ "\n")
      else:
        list_name.insert(END,"0%s:0%d" %(minute,sec)+ "\n")
      i = i +1
      index.append(i)


def play(event = None):
  """
  play the song.
  return none.
  """
  global wmp
  # root.title("%s" % name[-1])
  per_thread = threading.Thread(target = per)
  per_thread.daemnon = True
  wmp.controls.play()
  per_thread.start()
  # print(wmp.currentMedia.duration)


def per():
  """
  reset the processing.
  return none.
  """
  global total, progress_scal, root, wmp
  while wmp.playState !=1:
    progress_scal.set(int(wmp.controls.currentPosition))
    progress_scal.config(label = wmp.controls.currentPositionString)
    progress_scal.config(to = total,tickinterval = 50)
    time.sleep(1)
    root.title("%s" % wmp.currentMedia.name)


def stop():
  """
  stop the player.
  return none.
  """
  global wmp
  wmp.controls.stop()


def pause(event = None):
  """
  pause the player.
  return none.
  """
  global wmp
  wmp.controls.pause()


def uselist():
    """
    none.
    return none.
    """
    pass


def fullscr():
    """
    none.
    return none.
    """
    pass


def exitit():
  """
  destroy the ui player.
  return none.
  """
  global root
  root.destroy()


def Previous_it():
  """
  the prior song.
  return none.
  """
  global wmp
  wmp.controls.previous()


def Next_it():
  """
  the next song.
  return none.
  """
  global wmp
  wmp.controls.next()


def Volume_ctr(none):
  """
  get the volume.
  return none.
  """
  global wmp
  wmp.settings.Volume = vio_scale.get()


def Volume_add(i=[0]):
  """
  volume up.
  return none.
  """
  global vio_scale, wmp
  wmp.settings.Volume =wmp.settings.Volume+5
  i.append(wmp.settings.Volume)
  vio_scale.set(wmp.settings.Volume)


def Volume_minus(i=[0]):
  """
  volume down.
  return none.
  """
  global vio_scale, wmp
  wmp.settings.Volume = wmp.settings.Volume -5
  i.append(wmp.settings.Volume)
  vio_scale.set(wmp.settings.Volume)


def Scale_ctr(none):
  """
  get the current position.
  return none.
  """
  global wmp
  wmp.controls.currentPosition = var_scale.get()
  print(wmp.currentMedia.duration)


def Clear_list():
  """
  clear the song list.
  return none.
  """
  global list_name, name, index, wmp
  wmp.currentPlaylist.clear()
  list_name.delete(1.0,END)
  name = []
  index = []


def List_random():
  """
  random playing.
  return none.
  """
  global wmp
  wmp.settings.setMode("shuffle",True)
  play()


def List_loop():
  """
  loop playing.
  return none.
  """
  global wmp
  wmp.settings.setMode("loop",True)
  play()


# global variable...
wmp = None
# a global variable.




def main_music_playing():
    """
    the method to control the playing of the music..
    just like the main method.....
    """

    global wmp, list_name, var_scale, name, progress_scal, vio_scale, root

    root =Tk()
    root.title('MusicPlayer.')
    root.geometry("704x467")
    # set the size of the video player.
    wmp = Dispatch("WMPlayer.OCX")

    canvas = Canvas(root,width =220,height = 150,bg = "#DCDCDC")
    img = PhotoImage(file =os.getcwd() + '\\\\pythonfiles\\\\music_control\\\\111.jpg')
    canvas.create_image((110,77),image = img)
    canvas.place(x=0,y=0)
    canvas.coords(img,100,50)
    canvas.grid(row =0,column = 0,sticky = "nw",rowspan =2)

    progress_lab = LabelFrame(root,text = "PlayingProcess.")
    progress_lab.grid(row =2,column =0,sticky = "we",rowspan = 2)
    var_scale = DoubleVar()
    progress_scal = Scale(progress_lab,orient = HORIZONTAL,showvalue = 0,length =180,variable = var_scale)
    progress_scal.bind("<Button-1>",pause)
    progress_scal.bind("")
    progress_scal.bind("<ButtonRelease-1>",play)
    progress_scal.grid(row =3,column =0)

    modee_lab = LabelFrame(root,text = "PlayingMethod.")
    modee_lab.grid(row =4,column =0,rowspan =2,sticky = "ws")
    var_mode = IntVar()
    randomradio = Radiobutton(modee_lab,variable = var_mode,value = 1,text ="RandomPlay.",command =List_random )
    randomradio.grid(row =4,column =2)
    # below the two are bonded..
    inturnradio = Radiobutton(modee_lab,variable = var_mode,value =2,text= "OneByOnePlay.",command = play)
    inturnradio.grid(row=4,column =3)
    alloop = Radiobutton(modee_lab,variable = var_mode,value =2,text = "AllCirclePlay.",command = List_loop)
    alloop.grid(row =5,column = 2)
    sinloop = Radiobutton(modee_lab,variable = var_mode,value =3,text = "OneCirclePlay.")
    sinloop.grid(row =5,column =3)
    previous_play = Button(modee_lab,text = "<<=PriorSong",command = Previous_it)
    previous_play.grid(row =6,column =2,rowspan =2,pady =10)
    next_play = Button(modee_lab,text = "NextSong=>>",command = Next_it)
    next_play.grid(row =6,column =3,rowspan =2,pady =10)

    var_volume = IntVar()
    vioce_lab = LabelFrame(root,text = "VolumeControl.")
    vioce_lab.grid(row =8,column =0,sticky = "wes")
    vio_scale = Scale(vioce_lab,orient = HORIZONTAL,length =170,variable = var_volume,command =Volume_ctr)
    vio_scale.set(30)
    vio_scale.grid(row =8,column =0)
    vio_plus = Button(vioce_lab,width =8,text = "Volume -",command =Volume_minus)
    vio_plus.grid(row =9,column =0,sticky = "w")
    vio_minus = Button(vioce_lab,width =8,text ="Volume +",command =Volume_add)
    vio_minus.grid(row =9,column =0,sticky ="e")

    ctr_lab = LabelFrame(root,text = "PlayingControl.",height =130)
    ctr_lab.grid(row =0,column =1,rowspan =12,sticky = "ns")
    btn_open = Button(ctr_lab,text ="SelectMusicFiles",width =15,command = openfile, height=3)
    btn_open.grid(row=0,column =1)
    btn_play = Button(ctr_lab,text ="PlayMusic",width =15,command = play, height=3)
    btn_play.grid(row =1,column =1,pady =5)
    btn_stop = Button(ctr_lab,text ="StopMusic",width =15,command = stop, height=3)
    btn_stop.grid(row =2,column =1,pady =5)
    btn_pause = Button(ctr_lab,text ="PauseMusic",width =15,command = pause, height=3)
    btn_pause.grid(row =3,column =1,pady =5)
    listdel_all = Button(ctr_lab,width =15,text = "ClearMusicList",command = Clear_list, height=3)
    listdel_all.grid(row =8,column =1,sticky ="nw",pady =5)
    min_btn = Button(ctr_lab,width =15,text = "MinmizeWindow",command = root.iconify, height=3)
    min_btn.grid(row =13,column =1)

    list_name = Text(root,height =28,width =50)
    list_name.grid(row =0,column =2,sticky = "n",rowspan =6)

    root.mainloop()


    # Reference: https://www.jb51.net/article/86641.htm





# main - > test. 
# if __name__ == "__main__":
    # """
    # test the method of controlling the music playing..
    # main method..
    # """

# need to run....
# main_music_playing()
# 

    # pass




