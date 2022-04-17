import IVsshElement from "../IVsshElement";
import BaseElement from "../core/BaseElement";

import TabBar, { Tab, TabConfig } from "../core/TabBar";
import MainPageBox from "./MainPageBox";
import GlobalTabBar from "./GlobalTabBar";
import Login from "../pages/Login";
import {readFileSync} from 'fs';
import Program from "../../Program";
import LsParser from "../../parser/LsParser";
import PopUp, { PopUpConfig } from "../core/PopUp";
import RightMenu from "./RightMenu";
import Modal, { ModalConfig } from "./Modal";
import { RightMenuListItem } from "./RightMenuListItem";
import AppManagerApp from "../../apps/AppManager/AppManagerApp";
import FileManagerApp from "../../apps/FileManager/FileManagerApp";
import { app, ipcRenderer } from "electron";
import Connect from "../../Connect";

/**
 * 整个程序的界面v2.1-20210121
 * v2.2-20210204 添加关闭弹窗的方法
 */
export default class Container extends BaseElement{
    tabBar:GlobalTabBar;
    pageArea:BaseElement;
    rightMenu:PopUp<Array<RightMenuListItem>>;
    modal:PopUp<ModalConfig>;
    runtime:Program|undefined;
    modalBody=new Modal();
    userDataPath="";
    constructor(){
        super();
        this.setId("vssh-container")
        this.pageArea=new BaseElement();
        this.pageArea.style.height="100%"
        this.pageArea.style.overflow="overlay";
        this.tabBar= new TabBar({pageBox:new MainPageBox({
            container:this
        })});
        this.rightMenu=new PopUp<Array<RightMenuListItem>>(
            new RightMenu()
        );
        this.modal=new PopUp<ModalConfig>(
            this.modalBody
        );
        this.modalBody.addEventListener("ok",this.modal.close);
        this.modalBody.addEventListener("cancel",this.modal.close);
    }
    addEventListener(type: keyof HTMLElementEventMap, listener: (e: Event) => void): void {
        throw new Error("Method not implemented.");
    }

    /**
     * 新建标签页
     * @param props 标签页设置
     * @returns 标签索引
     */
    addNewTab(props:TabConfig){
        return this.tabBar.addTab(new Tab(props));
    }

    /**
     * 添加一个已经实例化的标签页
     * @param tab 
     * @returns 标签索引
     */
    addTab(tab:Tab){
        return this.tabBar.addTab(tab);
    }

    /**
     * 复制
     * @param data 
     */
    copy(data:string){
        this.runtime?.copy(data);
    }

    /**
     * 关闭弹窗
     */
    closeModal(){
        this.modal.hidden()
    }

    /**
     * 创建弹窗
     * @param config 
     */
    createModal(config:PopUpConfig<ModalConfig>){
        this.modal.open(config);
        if(config.data.onCancel){
            this.modalBody.addEventListener("cancel",config.data.onCancel);
        }
        if(config.data.onClose){
            this.modalBody.addEventListener("close",config.data.onClose);
        }
        if(config.data.onOk){
            this.modalBody.addEventListener("ok",config.data.onOk);
        }
    }

    /**
     * 创建右键菜单
     * @param config 
     */
    createRightMenu(config:PopUpConfig<Array<RightMenuListItem>>){
        if(config.data.length>0){
            this.rightMenu.open(config);
        }
    }

    /**
     * 剪切
     * @param data 
     */
    cut(data:string){
        this.runtime?.cut(data);
    }

    /**
     * 根据索引获取标签
     * @param index 标签的索引
     * @returns 
     */
    getTab(index:number){
        return this.tabBar.getTab(index);
    }

    /**
     * 粘贴
     * @returns 
     */
    paste(){
        return this.runtime?.paste();
    }
    /**
     * 设置页面内容
     * @param page 页面内容
     */
    setPage(page:IVsshElement){
        this.pageArea.setInnerElement(page);
    }

    async updateData(){
        this.userDataPath=await ipcRenderer.invoke ("get-user-data-path")
    }
    beforeRender() {
        this.appendChild(this.tabBar);
        this.appendChild(this.pageArea);
        this.appendChild(this.rightMenu);
        this.appendChild(this.modal)

        let RsaPath=localStorage.getItem("privateKeyPath");
        console.error(RsaPath);
        if(!RsaPath){
            RsaPath=`/home/${process.env['USER']}/.ssh/id_rsa`;//默认是linux
            if(process.platform=="darwin"){
                RsaPath=`/Users/${process.env['USER']}/.ssh/id_rsa`; //mac
            }else if(process.platform=="win32"){
                console.warn("路径",this.userDataPath)
                RsaPath=`${this.userDataPath}/.ssh/id_rsa`; //windows
            }
        }
        console.error(RsaPath);
        let login =new Login({
            host:<string>localStorage.getItem("host"),
            port:<string>localStorage.getItem("port"),
            username:<string>localStorage.getItem("username"),
            password:<string>localStorage.getItem("password"),
            privateKeyPath:RsaPath
        })
        // buttonLogin.innerText=`使用${process.platform}登录，位于${process.env['USER']},${localStorage.getItem("host")}`;
    
        
        // 初始化标签
        let pageId=this.addNewTab({
            name:"登录",
            page:login
        });
        
        let defaultTab:Tab=this.getTab(pageId);
        
        defaultTab.activated()
    
        login.addEventListener('login',async ()=>{
            console.log(login.getPassword());
            let password=login.getPassword();
            let username=login.getUsername();
            localStorage.setItem("host", login.getHost());
            localStorage.setItem("port", login.getPort());
            localStorage.setItem("username", login.getUsername());
            localStorage.setItem("password", login.getPassword());
            localStorage.setItem("privateKeyPath",login.getPrivateKeyPath())
    
            let privateKey:Buffer|undefined
            try{
                privateKey=readFileSync(login.getPrivateKeyPath())
            }catch(e){
                alert("无法读取私钥！")
                login.closeLoadding();
            }
            
            // console.log(privateKey);
    
            if(privateKey){
                this.runtime=new Program({
                    container:this,
                    username,
                    password,
                    port:login.getPort(),
                    host:login.getHost(),
                    privateKey,
                })

                let connected=false;
                try{
                    connected=await this.runtime.connectTest();
                }catch(e){
                    console.error(e);
                    alert("登录参数错误，请检查用户名密码主机端口是否正确");
                    login.closeLoadding()
                }
    
                if(connected){
                    let am=new AppManagerApp(this.runtime);
                    am.onStart();
                    let fm=new FileManagerApp(this.runtime);
                    fm.onStart(defaultTab);
                }
                
                
            }
            
           
            // this.runtime.createConnect(defaultTab,(conn,tab)=> new LsParser(conn, tab,this)).then(e=>{
            //   defaultTab.setText("文件管理器")
            // })
            // const conn = new Client();
            // conn.on('ready', () => {
            //     console.log('Client :: ready');
            //     conn.shell((err, stream) => {
            //         if (err) throw err;
            //         stream.on('close', () => {
            //             console.log('Stream :: close');
            //             conn.end();
            //         }).on('end',()=>{
            //             console.log("数据传输完毕");
            //         });
            //         stream.on("data",(data:Buffer)=>{
            //             fs.mkdir("./logs/",{recursive:true},(err,path)=>{
            //             })
            //         })
            //     });
            // }).connect({
            //     host: login.getHost(),
            //     port: Number(login.getPort()),
            //     username,
            //     password,
            //     readyTimeout:150000
            // })
            // conn.on('connect',console.warn);
            // conn.on('change password',console.warn);
            // conn.on('banner',console.warn);
            // conn.on('close',console.warn);
            // conn.on('continue',console.warn);
            // conn.on('end',console.warn);
            // conn.on('greeting',console.warn);
            // conn.on('keyboard-interactive',console.warn);
            // conn.on('ready',console.warn);
            // conn.on('tcp connection',console.warn);
            // conn.on('timeout',console.warn);
            // conn.on('x11',console.warn);
        });
    }
}
export interface containerInject{
    tabBar:GlobalTabBar
}