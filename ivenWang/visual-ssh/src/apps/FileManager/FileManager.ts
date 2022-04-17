import IVsshElement from "../../ui/IVsshElement";
import NavBar, { NavBarElementEventMap } from "../../ui/components/NavBar";
import BaseElement from "../../ui/core/BaseElement";
import { FileType } from "../../IVsshStatus";
import FileIcon from "../../ui/components/FileIcon";
import LsParser from "../../parser/LsParser";
import { RightMenuListItem } from "../../ui/components/RightMenuListItem";
import List from "../../ui/core/List";
import { DFile } from "../../VirtualFileSystem";
import Loading from "../../ui/core/Loading";

export default class FileManager extends BaseElement{
    renderArea:BaseElement
    navBar:NavBar
    paser:LsParser
    loading:Loading
    constructor(lsPaser:LsParser){
        super();
        this.paser=lsPaser;
        this.style.display="flex";
        this.style.height="100%";
        this.style.flexDirection="column";
        this.renderArea=new BaseElement({});
        this.renderArea.style.overflowY="scroll";
        this.renderArea.style.height="100%";
        this.navBar=new NavBar({
            url:"~"
        })
        this.loading=new Loading({
            type:"LoadingTest2"
        })
        this.setPage(this.loading);
    }
    addEventListener(type: keyof NavBarElementEventMap, listener: (e: Event) => void): void {
        if(type=="last" || type =="back" || type == "root" || type =="home"){
            this.navBar.addEventListener(type,listener);
        }else{
            super.addEventListener(type,listener);
        }
    }
    getUrl(){
        return this.navBar.getUrl();
    }
    /**
     * 显示当前目录全部文件
     */
    ls(){
        let data =this.paser.getCurrentFileList();
        this.setData(data);
    }
    setData(data:DFile[]){
        let icons: FileIcon[]=data.map((item,i,arr)=>{
            let icon=new FileIcon({
                name:item.name,
                type:item.getType()
            })
            if(item.getType()==FileType.folder){
                icon.addEventListener("dblclick",async (e)=>{
                    await this.paser.cd(item.getFullPath());
                    this.ls();
                })
            }
            icon.addEventListener("mousedown",(e)=>{
                let ev=<MouseEvent>e;
                e.stopPropagation()
                if(ev.button ==2){
                    let copy=new RightMenuListItem({
                        icon:"copy",
                        title:"复制"
                    });
                    copy.addEventListener("click",(e)=>{
                        this.paser.copy(`${this.getUrl()}/${item.name}`)
                    })
                    let cut=new RightMenuListItem({
                        icon:"cut",
                        title:"剪切"
                    })
                    cut.addEventListener("click",e=>{
                        this.paser.cut(`${this.getUrl()}/${item.name}`)
                    })
                    let paste=new RightMenuListItem({
                        icon:"paste",
                        title:"粘贴"
                    })
                    paste.addEventListener("click",this.paser.onParse)
                    let items:Array<RightMenuListItem>=[]
                    items.push(copy);
                    items.push(cut);

                    let clipBoardData=this.paser.paste()
                    // 那么，剪切过一个文件，如果粘贴再次会怎样呢？？？
                    if(clipBoardData?.data){
                        items.push(paste);
                    }
                    let menu=this.paser.createRightMenu({
                        x:ev.x,
                        y:ev.y,
                        data:items,
                    })
                }
            })
            return icon
        })
        console.warn("要被渲染的",icons);
        let ele=new List({data:icons});
        this.setPage(ele);
        this.loading.hidden()
    }
    setPage(pageEle:IVsshElement){
        this.renderArea.setInnerElement(pageEle);
    }
    setUrl(url:string){
        this.navBar.setUrl(url);
    }
    beforeRender(): void {
        this.setInnerElement(this.navBar);
        this.appendChild(this.renderArea);
    }
}