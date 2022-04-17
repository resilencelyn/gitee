import { app, BrowserWindow, ipcMain } from 'electron';
import path from 'path';

function createWindow () {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
    title:'visual-ssh',
    webPreferences: {
      nodeIntegration: true,
      nodeIntegrationInWorker: true,
      webSecurity: false,
      preload: path.join(__dirname, 'preload.js')
    }
  })
  win.loadFile('./src/index.html');
  console.log("测试吖");
  const contents = win.webContents
  // console.log(contents)
}

app.whenReady().then(createWindow)
ipcMain.on('ondragstart', (event, filePath) => {
  console.log("触发事件",filePath)
  event.sender.startDrag({
    file: path.join(__dirname, filePath),
    icon: path.join(__dirname, '../assets/drag.png'),
  })
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow()
  }
})

ipcMain.handle ("get-user-data-path", (event, ...args) => {
  let appData=app.getPath("home")
  console.log(appData);
  return appData;
});