import SpecialElement from "./SpecialElement";

export default class Input extends SpecialElement{
    input:HTMLInputElement
    constructor(props:InputConfig){
        super();
        this.input=document.createElement("input");
        this.input.type=props.type
        this.input.style.border="none";
        this.input.style.outline="none";
        if(props.placeholder){
            this.input.placeholder=props.placeholder;
        }
        if(props.value){
            this.input.value=props.value;
        }
        this.setHTMLElement(this.input);
        this.style.backgroundColor="#FFF";
        this.style.padding="4px"
    }

    get innerStyle(){
        return this.input.style;
    }
    /**
     * 触发表单
     */
    click(){
        this.input.click();
    }
    addEventListener(type: keyof HTMLElementEventMap, listener: (e: Event | MouseEvent) => void): void {
        this.input.addEventListener(type,listener);
    }
    setValue(v:string):void{
        this.input.value=v;
    }
    getValue():string{
        return this.input.value;
    }
    /**
     * 文件表单专用，返回文件
     * @returns 
     */
    getFiles(){
        return this.input.files;
    }
}
export interface InputConfig{
    type:string
    placeholder?:string
    value?:string;
}