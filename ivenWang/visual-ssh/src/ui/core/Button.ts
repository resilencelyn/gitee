import IVsshElement from "../IVsshElement";
import Icon, { IconList } from "./Icon";

/**
 * 按钮组件也欠重构
 */
export default class Button implements IVsshElement{
    button:HTMLButtonElement
    constructor(props:ButtonConfig){
        this.button=document.createElement("button");
        this.button.innerText=props.text||""
        this.button.style.margin="8px";
        this.button.style.border="none";
        if(props.icon){
            this.button.appendChild((new Icon({
                name:props.icon
            })).render())
        }
    }
    get style(){
        return this.button.style
    }
    dispatchEvent(event: Event): void {
        throw new Error("Method not implemented.");
    }
    addEventListener(type:keyof HTMLElementEventMap,listener:(e:Event)=>void){
        this.button.addEventListener(type,listener);
    }
    render(): HTMLElement {
        return this.button;
    }
}
export interface ButtonConfig{
    text?:string
    icon?:keyof IconList
}