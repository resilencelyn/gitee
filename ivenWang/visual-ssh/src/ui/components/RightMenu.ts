import BaseElement from "../core/BaseElement";
import List, { ListType } from "../core/List";
import { PopUpBody } from "../core/PopUp";
import { RightMenuListItem } from "./RightMenuListItem";

/**
 * 右键菜单v0.3 
 * 列表容器
 */
export default class RightMenu extends BaseElement implements PopUpBody<Array<RightMenuListItem>> {
    list:List;
    constructor() {
        super();
        this.list=new List({
            type:ListType.List,
            data:[new RightMenuListItem({
                icon:"help",
                title:"帮助"
            })]//测试
        });
        this.style.position="absolute";
        this.style.width="200px";
    }
    onClose(): void {
    }

    getHeight(){
        return this.offsetHeight
    }

    getWidth(): number {
        return this.offsetWidth
    }

    /**
     * 设置菜单项
     * @param menuItems 
     */
    setData(data: RightMenuListItem[]): void {
        this.list=new List({
            type:ListType.List,
            data
        })
        this.forceRerender();
    }

    /**
     * 设置菜单坐标
     * @param x 
     * @param y 
     */
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

    render(): HTMLElement {
        this.setInnerElement(this.list);
        return super.render();
    }
}

export interface RightMenuConfig{
    x:number;
    y:number;
}

