import LsParser from "../../parser/LsParser";
import PkgParser from "../../parser/PkgParser";
import Program from "../../Program";
import BaseElement from "../../ui/core/BaseElement"
import { IconList } from "../../ui/core/Icon";
import List from "../../ui/core/List";
import Container from "../../ui/components/Container";
import ListIcon from "../../ui/components/ListIcon";
import PkgManager from "../../ui/pages/PkgManager";
import { Tab } from "../../ui/core/TabBar";
import Test from "../../ui/components/Test";
import PageBase, { PageConfig } from "../../ui/pages/PageBase";
import App from "../../App";
import FileManagerApp from "../FileManager/FileManagerApp";


export default class AppStart extends PageBase{
    list:List
    runtime:Program
    localAppList:LocalAppInfo[] 
    constructor(props:AppStartConfig){
        super({
            container:props.container
        });

        // 这个不应该写在app Start 里面，应该写在核心某个地方
        this.localAppList = [
            {
                name:"文件管理器",
                icon:"folderO",
                bgcolor:"red",
                app:new FileManagerApp(<Program>props.container.runtime)
            },
            { 
                name:"软件包商店",
                icon:"shoppingBag",
                bgcolor:"purple"
            },
            {
                name:"图片查看器",
                icon:"imageO",
                bgcolor:"gold"
            }
        ]

        this.runtime=props.runtime;
        this.style.overflowY="scroll" 
        this.style.height="100%"
        let listData:ListIcon[]=this.localAppList.map(item =>{
            let icon=new ListIcon({iconId:item.icon,text:item.name})
            icon.addEventListener("click",(e)=>{
                item.app?.onStart();
                // let tab=new Tab({
                //     name:item.name,
                //     page:new PkgManager({
                //         container:props.container
                //     })
                // })
                // this.runtime.createConnect(tab,(conn,tab)=> new PkgParser(conn, tab,props.container)).then((n)=>{
                //     this.runtime.addTab(tab);
                // })
            })
            return icon;
        });
        this.list=new List({data:listData});
    }
    render(): HTMLElement {
        this.setInnerElement(this.list);
        console.warn(this.list.getLength());
        return super.render();
    }
}

/**
 * VSSH的应用信息
 */
interface LocalAppInfo{
    name:string
    icon:keyof IconList
    bgcolor:string
    app?:App
}

interface AppStartConfig extends PageConfig{
    runtime:Program
}