// import $ from 'jquery';
import {Client} from 'ssh2';
import fs from "fs";

import {contextBridge, ipcRenderer } from "electron";
import path from "path"
import Container from './ui/components/Container';
contextBridge.exposeInMainWorld('electron', {
    startDrag: (fileName: string) => {
      ipcRenderer.send('ondragstart', path.join(process.cwd(), fileName))
    }
  })
window.addEventListener('DOMContentLoaded', async () => {
    const replaceText = (selector:string, text: string|undefined) => {
      const element = document.getElementById(selector)
      if (element) element.innerText = <string>text
    }
  
    for (const dependency of ['chrome', 'node', 'electron']) {
      replaceText(`${dependency}-version`, process.versions[dependency])
    }

    let rootEle=<HTMLElement>document.getElementById("root");
    rootEle.innerText="";
    // let page =document.createElement("div");
    // page.style.overflowY="scroll"
    let windowPage=new Container();
    await windowPage.updateData();
    rootEle.appendChild(windowPage.render());
    // rootEle.appendChild(page)
    

    
})