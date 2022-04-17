import BaseElement from "../core/BaseElement";
import FormInput from "../core/FormInput";

export default class CreateFolder extends BaseElement{
    input:FormInput
    constructor(){
        super();
        this.input=new FormInput({});
    }

    /**
     * 获取目录名
     * @returns 
     */
    getValue(){
        return this.input.getValue();
    }

    beforeRender(): void {
        this.setInnerElement(this.input);
    }
}