import Connect from "../Connect";
import Tools from "../Tools";
import PackageListItem from "../ui/components/PackageListItem";
import { Tab } from "../ui/core/TabBar";
import BaseElement from "../ui/core/BaseElement";
import List, { ListType } from "../ui/core/List";
import PkgManager from "../ui/pages/PkgManager";
import IParser from "./IParser";
import ParserBase from "./ParserBase";
import Container from "../ui/components/Container";

/**
 * 包管理器
 */
export default class PkgParser extends ParserBase {
    private pkgManager?:string
    private page:PkgManager
    /**
     * 是否正在设置包管理器
     */
    private isInit=true;
    private installedPackages:Array<Package>=[]
    private availablePackages:Array<Package>=[]
    constructor(connect:Connect,tab:Tab,container:Container){
        super(connect,tab,container);
        this.setPkgManager();
        this.page=new PkgManager({
            container
        });
    }
    

    /**
     * 传一个包管理器用的参数进去，比如list，install php等。
     * @param cmd 包管理器的参数
     * @returns 执行后的结果
     */
    async exec(cmd:string){
        if(this.pkgManager){
            return this.conn.exec(`${this.pkgManager} ${cmd}`);
        }else{
            throw new ReferenceError("未设置包管理器！")
        }
    }
    /**
     * 获取可用包列表
     * @returns 
     */
     async getPackageList(){
        let rel=await this.exec("list");
        this.parseYum(rel.data);
        return rel;
    }
    
    async onConnected() {
        this.renderTab.setPage(this.page);
        await this.waitInit();
        await this.getPackageList();
        let list=new List({
            type:ListType.List
        });
        this.installedPackages.forEach(ele=>{
            // list.addItem(new BaseElement({
            //     innerElement:`${ele.name} ${ele.version} ${ele.source}`
            // }))
            if(ele && ele.source){
                list.addItem(
                    new PackageListItem(ele)
                )
            }
        })
        this.page.setInstalled(list);
    }

    /**
     * 解析yum返回结果并存
     * @param data 
     */
    parseYum(data:string){
        let arr=data.split("Installed Packages");
        data=arr[arr.length-1];
        let rel=data.split("Available Packages");
        let installed=rel[0].split("\n");
        let availables=rel[1].split("\n");
        this.installedPackages=installed.map(a=>{
            let r=a.split(/[\s]+/);
            return {
                name:Tools.clearANSI(r[0]),
                version:r[1],
                source:r[2]
            }
        })
        this.availablePackages=availables.map(a=>{
            let r=a.split(/[\s]+/);
            return {
                name:Tools.clearANSI(r[0]),
                version:r[1],
                source:r[2]
            }
        })
        console.error(this.installedPackages);
        console.error(this.availablePackages);
    }

    /**
     * 设置本系统的包管理器
     * @param pkg 可选。无此参数将自动检查包管理器。
     */
    async setPkgManager(pkg?:string){
        if(pkg){
            this.pkgManager=pkg;
        }else{
            let i=0;
            let cmdCode=-1;
            while(i<pkgManagerList.length&& cmdCode>0){
                await this.conn.exec(`${pkgManagerList[i]} -v`);
                i++;
                let cmdCode= await this.conn.getLastExitCode();
                console.log(`退出代码：${cmdCode}`)
            }
            if(i>=pkgManagerList.length&&cmdCode>0){
                this.isInit=false;
                throw new ReferenceError("该系统没有本软件所支持的包管理器！请手动设置包管理器！");
            }else{
                this.pkgManager=pkgManagerList[i];
            }
        }
        this.isInit=false;
    }

    /**
     * 等待初始化
     * @returns 
     */
    async waitInit(){
        return await new Promise((resolve,reject)=>{
            let interval=setInterval(()=>{
                if(!this.isInit){
                    clearInterval(interval);
                    resolve(interval);
                }
            },100)
        });
    }
}

const pkgManagerList=[
    "yum","apt"
]

export interface Package{
    name:string;
    version:string;
    source:string;
}