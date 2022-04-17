import IVsshElement from "../IVsshElement";
import BaseElement from "./BaseElement";

/**
 * 格子状列表组件v1.1
 */
export default class List extends BaseElement{
    data:(IVsshElement|string)[]
    private type:ListType
    constructor(props:ListConfig){
        super()
        this.data=props.data?props.data:[];
        this.type=props.type || ListType.Grid
    }
    /**
     * 添加一个元素
     * @param item 添加的元素
     * @returns 元素索引
     */
    addItem(item:IVsshElement|string){
        return this.data.push(item)-1;
    }

    /**
     * 获取列表长度
     * @returns 
     */
    getLength(){
        return this.data.length;
    }
    
    render(): HTMLElement {
        this.setInnerElement("");
        if(this.type==ListType.Grid){
            this.style.display="grid";
            this.style.gridTemplateColumns="repeat(auto-fill, 100px)";
            this.style.gridTemplateRows="repeat(auto-fill, 100px)";
            this.style.gridRowGap="5px";
            this.style.gridColumnGap="5px";
            this.style.justifyContent="center";
        }
        if(this.type==ListType.List){
            this.style.backgroundColor="#EEE"
        }
        this.data.forEach(element => {
            let listItem =new BaseElement();
            if(this.type==ListType.List){
                listItem.style.borderBottom="1px solid #BBB"
                listItem.style.padding="8px"
            }
            listItem.setInnerElement(element)
            this.appendChild(listItem);
        });
        return super.render();
    }
}

export interface ListConfig{
    data?:IVsshElement[]|string[]
    type?:ListType
}

export enum ListType{
    /**
     * 格子状列表
     */
     Grid,
    /**
     * 普通列表
     */
    List,
    
}