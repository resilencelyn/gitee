import BaseElement, { BaseElementConfig } from "../core/BaseElement";
import Icon, { IconList } from "../core/Icon";

/**
 * 右键菜单中的一项v0.1
 */
export class RightMenuListItem extends BaseElement{
    icon:Icon
    text:RightMenuListItemText
    constructor(props:RightMenuListItemConfig){
        super();
        this.icon=new Icon({
            name:props.icon
        })
        this.text=new RightMenuListItemText({
            innerElement:props.title
        })
        this.style.display="flex";
        this.style.userSelect="none";
        this.style.cursor="pointer";
    }
    render(): HTMLElement {
        this.setInnerElement(this.icon);
        this.appendChild(this.text);
        return super.render();
    }
}
class RightMenuListItemText extends BaseElement{
    constructor(props:BaseElementConfig){
        super(props);
    }
}
export interface RightMenuListItemConfig{
    icon:keyof IconList
    title:string
}