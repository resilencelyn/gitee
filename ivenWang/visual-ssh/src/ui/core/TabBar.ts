import IVsshElement from "../IVsshElement";
import BaseElement from "./BaseElement";
import Icon from "./Icon";
import IPageBox from "./IPageBox";
import Label from "./Label";

/**
 * Tab Bar v2.4
 * 接下来总标签页不应该直接实例化这个类，而应该再写一个类，继承自这个类。这个类单纯做为通用组件
 */
export default class TabBar extends BaseElement{
    bar:BaseElement;
    tabs:Tab[]=[]
    pageBox:IPageBox;
    // tabPage:Container
    constructor(props:TabBarConfig){
        super();
        this.bar=new BaseElement();
        this.bar.style.display="flex";
        this.bar.style.backgroundColor="#444"
        this.bar.style.color="#EEE"
        this.pageBox=props.pageBox
        // this.tabPage=props.tabPage;
    }
    /**
     * 添加 一个标签页
     * @param tab 
     * @returns 标签页的索引
     */
    addTab(tab:Tab){
        tab.joinBar(this);
        this.tabs.push(tab);
        this.render();
        return this.tabs.length-1;
    }

    /**
     * 根据索引激活标签
     * @param index 标签的索引
     */
    activated(index:number){
        this.freezeAll();
        this.tabs[index].activated();
    }

    /**
     * 移除标签
     * @param tab 
     */
    removeTab(tab:Tab){
        let index=this.tabs.indexOf(tab);
        if(tab.active){
            this.activated(index?(index-1):1);
        }
        this.tabs=this.tabs.filter((e,i)=>i!=index);
        this.render();
        console.error(index,this.tabs.length)
    }

    /**
     * 根据索引获取标签
     * @param index 标签的索引
     * @returns 
     */
    getTab(index:number){
        return this.tabs[index]
    }

    /**
     * 设置页面内容，切换标签时使用
     * @param page 页面内容
     */
    setPage(page:BaseElement){
        this.pageBox.setPage(page);
        // this.forceRerender();
        // this.tabPage.setPage(page);
    }
    /**
     * 冻结全部标签，以便切换标签
     */
    freezeAll(){
        this.tabs.forEach(e=>{
            e.freeze();
        })
    }
    beforeRender(): void {
        this.style.display="flex"
        this.style.flexDirection="column"
        this.style.height="100%";
        this.bar.setInnerElement("");
        this.tabs.forEach(e=>{
            e.addEventListener("mousedown",ev=>{
                e.style.transition="transform 0s"
                e.isDroping=true;
                e.dropStartMouseX=(<MouseEvent>ev).clientX
            })
            e.addEventListener("mousemove",ev=>{
                if(e.isDroping){
                    e.style.transform=`translateX(${(<MouseEvent>ev).clientX-e.dropStartMouseX}px)`
                }
            })
            e.addEventListener("mouseout",e.moveCancel);
            e.addEventListener("mouseup",e.moveCancel);
            this.bar.appendChild(e);
        })
        this.setInnerElement(this.bar);
        // this.appendChild(new BottomLine());
        this.appendChild(this.pageBox);
    }
}

/**
 * 底下那条线
 */
class BottomLine extends BaseElement{
    constructor(){
        super();
        this.style.borderBottom="solid 1px #999999"
        this.style.display="flex"
        this.style.minHeight="5px"
    }
}
export interface TabBarConfig{
    pageBox:IPageBox
}

export class Tab extends BaseElement{
    active=false;
    bar?:TabBar
    /**
     * 标签对应的页面内容
     */
    page:BaseElement
    label:Label
    closeBtn:Icon
    isDroping=false
    dropStartMouseX=0
    constructor(props:TabConfig){
        super();
        this.label=new Label({
            text:props.name
        })
        
        this.closeBtn=new Icon({
            name:"close"
        })
        this.style.width="100px";
        this.style.textAlign="center";
        this.style.cursor="pointer";
        this.style.display="flex";
        this.style.justifyContent="space-around"
        this.style.userSelect="none"
        this.setInnerElement(this.label);
        this.active=!!props.active;
        this.bar=props.bar;
        this.addEventListener("click",e =>{
            // alert(this.getText())
            if(this.bar){
                this.bar.freezeAll();
                this.activated();
            }else{
                throw new ReferenceError("标签脱离了组织！")
            }
        })
        this.page=props.page;
        // this.page=document.createElement("div");
    }

    close=(e:Event | MouseEvent)=>{
        e.stopPropagation();
        this.bar?.removeTab(this);
    }
    addEventListener(type: keyof TabEventMap, listener: (e: Event|MouseEvent) => void): void {
        if(type=="dragover" || type== "drop"){
            this.page.addEventListener(type,listener);
        }else if(type=="close"){
            this.closeBtn.addEventListener("click",listener);
        } else{
            super.addEventListener(type,listener)
        }
    }

    /**
     * 将标签加进标签组
     * @param party 标签组
     */
    joinBar(party:TabBar){
        this.bar=party
    }

    /**
     * 设置标签名
     * @param name 
     */
    setName(name:string){
        this.setInnerElement(name);
    }
    /**
     * 
     * @param pageElement 
     */
    setPage(pageElement:BaseElement){
        this.page=pageElement;
        this.page.forceRerender()
    }
    /**
     * 激活本标签
     */
    activated(){
        // alert("激活")
        this.active=true;
        this.bar?.setPage(this.page);
        this.render();
        this.closeBtn.style.display="flex"
    }
    /**
     * 获取标签文本
     * @returns 标签的文字
     */
    getText(){
        return super.getText()
    }

    /**
     * 移动取消
     * @param e 
     */
    moveCancel=(e:Event|MouseEvent)=>{
        this.style.transition="transform 0.5s"
        this.isDroping=false;
        this.style.transform=`translateX(0px)`
    }
    /**
     * 设置标签文本
     * @param str 文本
     */
    setText(str:string){
        this.label.setText(str);
        this.render();
    }
    /**
     * 冻结本标签
     */
    freeze(){
        this.active=false;
        this.closeBtn.style.display="none";
        this.render();
    }
    beforeRender(): void {
        this.closeBtn.addEventListener("click",this.close);
        this.style.backgroundColor=this.active?"#ccc":"inherit";
        this.setInnerElement(this.label);
        this.appendChild(this.closeBtn);
    }
}
export interface TabConfig{
    name:string
    active?:boolean
    bar?:TabBar
    page:BaseElement
}

interface TabEventMap extends HTMLElementEventMap{
    "close":Event
}