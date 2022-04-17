import BaseElement from "./BaseElement";
import SpecialElement from "./SpecialElement";

export default class Label extends BaseElement{
    constructor(props:LabelConfig){
        super()
        this.style.fontSize="16px"
        this.style.whiteSpace="nowrap"
        this.style.textOverflow="ellipsis"
        this.style.overflow="hidden";
        this.setText(props.text);
    }

    setText(text:string){
        this.setInnerElement(text);
    }
}
export interface LabelConfig{
    text:string
}