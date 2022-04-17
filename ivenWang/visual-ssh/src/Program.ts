import { ClientChannel } from "ssh2";
import IVsshStatus, { ClipBoardData, ClipBoardOption, FileType, IDFile } from "./IVsshStatus";

import Container from "./ui/components/Container";
import Connect from "./Connect";
import { Tab, TabConfig } from "./ui/core/TabBar";
import IParser from "./parser/IParser";
import VirtualFileSystem from "./VirtualFileSystem";

export default class Program{
    private Status:IVsshStatus;
    privateKey:Buffer
    connectPool:Connect[]=[];
    private fileSystemCache:VirtualFileSystem=new VirtualFileSystem();
    constructor(config:RuntimeConfig){
        this.Status={
            expect: "",
            lineTitleVerified: false,
            username: config.username,
            password: config.password,
            host: config.host,
            port: config.port,
            container: config.container,
            dataLock: false,
            path: "~",
            ClipBoard:"",
            ClipBoardOption:ClipBoardOption.Copy
        }
        this.privateKey=config.privateKey;
        // this.run();
    }

    /**
     * 新建标签页
     * @param props 标签页设置
     * @returns 标签索引
     */
    addNewTab(props:TabConfig){
        return this.Status.container.addNewTab(props);
    }

    /**
     * 添加一个已经实例化的标签页
     * @param tab 
     * @returns 标签页索引
     */
    addTab(tab:Tab){
        return this.Status.container.addTab(tab);
    }

    /**
     * 将数据以复制模式加入剪切板
     * @param data 
     */
    copy(data:string){
        this.Status.ClipBoard=data;
        this.Status.ClipBoardOption=ClipBoardOption.Copy
    }

    /**
     * 测试连接
     * @returns 
     */
    async connectTest(){
        return await Connect.testConnect({
            username:this.Status.username,
            password:this.Status.password,
            host:this.Status.host,
            port:Number(this.Status.port),
            privateKey:this.privateKey,
        })
    }

    async createConnect(page:Tab,parserCreator:(conn:Connect,tab:Tab)=>IParser){
        let l= this.connectPool.push(await Connect.create({
            username:this.Status.username,
            password:this.Status.password,
            host:this.Status.host,
            port:Number(this.Status.port),
            privateKey:this.privateKey,
            renderPage:page,
            mainProgram:this,
            parserCreator
        }))
        return l-1
    }

    /**
     * 将数据以剪切模式加入剪切板
     * @param data 
     */
    cut(data:string){
        this.Status.ClipBoard=data;
        this.Status.ClipBoardOption=ClipBoardOption.Cut;
    }

    getContainer(){
        return this.Status.container;
    }

    getExpect(){
        return this.Status.expect;
    }

    /**
     * 获取标签的索引
     * @param index 标签的id
     * @returns 
     */
    getTab(index:number){
        return this.Status.container.getTab(index);
    }

    /**
     * 获取文件系统缓存
     * @returns 
     */
    getVirtualFileSystem(){
        return this.fileSystemCache;
    }

    paste():ClipBoardData{
        return {
            data:this.Status.ClipBoard,
            option:this.Status.ClipBoardOption
        };
    }

    setExpect(expect:string){
        this.Status.expect=expect;
    }
}

export interface RuntimeConfig{
    container:Container,
    username:string,
    password:string
    host:string
    port:string |number
    privateKey:Buffer,
}