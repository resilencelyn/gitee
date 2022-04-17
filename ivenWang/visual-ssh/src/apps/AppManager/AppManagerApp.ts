import App from "../../App";
import Program from "../../Program";
import { Tab } from "../../ui/core/TabBar";
import AppStart from "./AppStart";
import AppStartParser from "./AppStartParser";

export default class AppManagerApp extends App{
    constructor(rt:Program){
        super(rt);
    }
    async onStart() {
        this.page=new AppStart({
            runtime:this.runtime,
            container:this.runtime.getContainer()
        })
        let tabId=this.runtime.addNewTab({
            name:'应用管理器',
            page:this.page
        })
        let tab:Tab=this.runtime.getTab(tabId);
        let connectId=await this.runtime.createConnect(tab,(conn,tab)=> new AppStartParser());
        
        
    }
}