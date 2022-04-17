import { Close } from "../events";
import IVsshElement from "../IVsshElement";
import BaseElement from "./BaseElement";

/**
 * 弹出层控件容器 v0.2
 */
export default class PopUp<T> extends BaseElement{
    x:number=0;
    y:number=0;
    instance:PopUpBody<T>
    constructor(
        popUpBodyInstance:PopUpBody<T>
    ){
        super();
        this.instance=popUpBodyInstance;
        this.style.position="absolute";
        this.style.width="100%";
        this.style.height="100%";
        this.style.left="0";
        this.style.top="0"
        this.addEventListener("click",(e)=>{
            this.hidden();
        })
        this.hidden();
        
    }

    /**
     * 供组件调用的关闭;
     */
    close=()=>{
        this.hidden()
    }

    /**
     * 设置坐标
     * @param x 
     * @param y 
     */
    setLocate(x:number,y:number){
        this.instance.setLocate(x,y);
    }
    
    /**
     * 隐藏，事实上的关闭。
     */
    hidden(){
        this.style.display="none";
        this.instance.dispatchEvent(new Close())
    }

    show(){
        this.style.display="block";
    }

    /**
     * 开启弹出层控件。全局应该只有这一个弹出层控件，本系统所有创建右键菜单的方法底层都应该都是调用的这个方法。
     * @param config 
     */
    open(config:PopUpConfig<T>){
        this.instance.setData(config.data);
        this.show();
        this.setLocate(config.x,config.y);
        
    }
    
    beforeRender(): void {
        this.setInnerElement(this.instance);
    }
}

export interface PopUpBody<T> extends IVsshElement{
    setLocate(x:number,y:number):void,
    setData(data:T):void
}

export interface PopUpConfig<T>{
    x:number,
    y:number,
    data:T
}