import App from "../../App";
import LsParser from "../../parser/LsParser";
import Loading from "../../ui/core/Loading";
import { Tab } from "../../ui/core/TabBar";
import FileManager from "./FileManager";

export default class FileManagerApp extends App{
    async onStart(tab?: Tab) {
        // this.page=new FileManager()
        this.page.setInnerElement(new Loading({
            type:"LoadingTest2"
        }))
        
        if(!tab){
            let tabId=this.runtime.addNewTab({
                name:'文件管理器',
                page:this.page,
                active:false
            })
            tab=this.runtime.getTab(tabId);
        }
        tab.setName("文件管理器")
        let connectId=await this.runtime.createConnect(tab,(conn,tab)=> new LsParser(conn,tab,this.runtime.getContainer()));
    }
}