import IVsshElement from '../IVsshElement';
import BaseElement from './BaseElement';
import Input from './Input';
export default class FormInput extends BaseElement{
    label?:string;
    type?:string;
    input:Input;
    constructor(props:FormInputConfig){
        super();
        this.label=props.label;
        this.type=props.type;

        this.input=new Input({
            type:this.type?this.type:"text",
            placeholder:props.placeholder
        })
        if(props.value){
            this.setValue(props.value);
        }
        
        if(props.value){
            this.input.setValue(props.value);
        }
        
    }
    dispatchEvent(event: Event): void {
        throw new Error('Method not implemented.');
    }
    setValue(v:string):void{
        this.input.setValue(v);
    }
    getValue():string{
        return this.input.getValue();
    }
    render(): HTMLElement {
        let box=document.createElement("div");
        box.style.display="flex";
        box.style.padding="8px 15px";
        let title=document.createElement("span");
        title.style.flexShrink="1";
        title.style.width="100px";
        
        if(this.label){
            title.innerText=this.label;
            box.appendChild(title);
        }
        box.appendChild(this.input.render());
        return box;
    }
}
export interface FormInputConfig{
    label?:string;
    placeholder?:string;
    type?:string;
    value?:string;
}