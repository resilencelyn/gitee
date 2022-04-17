import Container from "../components/Container";
import { RightMenuListItem } from "../components/RightMenuListItem";
import BaseElement from "../core/BaseElement";
import { PopUpConfig } from "../core/PopUp";

/**
 * 页面基类v0.1。所有全局标签页的页面都应该继承自这个类。
 */
export default class PageBase extends BaseElement{
    container:Container
    constructor(config:PageConfig){
        super();
        this.container=config.container
    }
    /**
     * 创建右键菜单
     * @param config 
     */
    createRightMenu(config: PopUpConfig<RightMenuListItem[]>){
        this.container.createRightMenu(config);
    }
}

export interface PageConfig{
    container:Container
}