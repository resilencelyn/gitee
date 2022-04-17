import BaseElement from "../core/BaseElement";
import Icon, { IconList } from "../core/Icon";

export default class ListIcon extends BaseElement{
    protected icon:Icon
    protected label:BaseElement
    constructor(props:ListIconConfig){
        super();
        if(props.bgColor){
            this.style.backgroundColor=props.bgColor
        }
        if(props.fontColor){
            this.style.color=props.fontColor
        }
        this.style.height="100px"
        this.icon=new Icon({
            name:props.iconId,
            size:72,
            color:"#FFF"
        })
        this.label=new BaseElement({
            innerElement:props.text
        })
        this.label.style.lineHeight="1"
        this.label.style.whiteSpace="nowrap"
        this.label.style.textOverflow="ellipsis"
        this.label.style.overflow="hidden";
    }
    beforeRender(): void {
        this.style.display="flex"
        this.style.flexDirection="column"
        this.style.alignItems="center"
        this.setInnerElement(this.icon);
        let title=new BaseElement();
        title.appendChild(this.label)
        title.style.height="18px";
        title.style.lineHeight="1";
        title.style.width="100%";
        this.appendChild(title);
        this.style.textAlign="center";
    }
}
export interface ListIconConfig{
    iconId:keyof IconList
    text:string;
    bgColor?:string;
    iconColor?:string;
    fontColor?:string;
}