import BaseElement from "../core/BaseElement";
import Button from "../core/Button";
import { PopUpBody } from "../core/PopUp";

/**
 * 弹窗 v0.3 
 */
export default class Modal extends BaseElement implements PopUpBody<ModalConfig>{
    modalTitle:ModalTitle
    modalBody:ModalBody
    modalFooter:ModalFooter

    onCancel=false
    onOk=false
    constructor() {
        super()
        this.modalTitle=new ModalTitle("");
        this.modalBody=new ModalBody("");
        this.modalFooter=new ModalFooter();
        this.style.position="absolute";
        this.style.width="200px";
        this.style.backgroundColor="#CCC";
        this.style.boxShadow="2px 2px 10px #000"
        let eventStop=(e:Event)=>{
            e.stopImmediatePropagation();
            e.stopPropagation();
        }
        this.modalTitle.addEventListener("click",eventStop);
        this.modalBody.addEventListener("click",eventStop);
        this.modalFooter.addEventListener("click",eventStop);
    }

    addEventListener(type: keyof ModalEventMap, listener: (this: HTMLDivElement, e: Event | MouseEvent) => any): void {
        switch(type){
            case "close":
                break;
            case "ok":
            case "cancel":
                this.modalFooter.addEventListener(type,listener);
                break;
            default:
                super.addEventListener(type,listener);
        }
    }

    getHeight(){
        return this.offsetHeight
    }

    getWidth(): number {
        return this.offsetWidth
    }

    setData(data: ModalConfig): void {
        this.modalTitle.setInnerElement(data.title)
        this.modalBody.setInnerElement(data.body);
        this.onCancel=!!data.onCancel;
        this.onOk=!!data.onOk;
        this.checkFootSetting();
    }
    beforeRender(): void {
        this.setInnerElement(this.modalTitle);
        this.appendChild(this.modalBody);
        this.checkFootSetting();
    }

    private checkFootSetting(){
        if(this.onCancel || this.onOk){
            this.appendChild(this.modalFooter);
        }
    }

    setLocate(x:number,y:number){
        if(x+this.getWidth()> window.innerWidth){
            this.style.left=(window.innerWidth-this.getWidth())+"px";
        }else{
            this.style.left=x+"px";
        }
        
        if(y+this.getHeight()> window.innerHeight){
            this.style.top=(window.innerHeight-this.getHeight())+"px";
        }else{
            this.style.top=y+"px";
        }
    }
}

class ModalTitle extends BaseElement {
    constructor(title:string) {
        super({
            innerElement:title
        })
        this.style.padding="5px";
    }
}
class ModalBody extends BaseElement {
    constructor(body:BaseElement|string) {
        super({
            innerElement:body
        })
        this.style.padding="5px";
    }
}
class ModalFooter extends BaseElement{
    okBtn:Button
    cancelBtn:Button
    constructor(){
        super();
        this.okBtn=new Button({
            text:"确认"
        })
        this.cancelBtn=new Button({
            text:"关闭"
        })
    }

    addEventListener(type: keyof ModalEventMap, listener: (this: HTMLDivElement, e: Event | MouseEvent) => any): void {
        switch(type){
            case "close":
                break;
            case "ok":
                this.okBtn.addEventListener("click",listener);
                break;
            case "cancel":
                this.cancelBtn.addEventListener("click",listener);
                break;
            default:
                super.addEventListener(type,listener);
        }
    }

    beforeRender(): void {
        this.setInnerElement(this.okBtn);
        this.appendChild(this.cancelBtn);
    }
}

export interface ModalConfig{
    title:string
    body:BaseElement
    onClose?:()=>void
    onCancel?:()=>void
    onOk?:()=>void
}

