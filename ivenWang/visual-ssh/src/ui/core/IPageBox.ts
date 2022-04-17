import IVsshElement from "../IVsshElement";
import BaseElement from "./BaseElement";

/**
 * Tab用的page接口。为了在tab  page中实现右键菜单，同时保证组件解耦合，所以要单独实现
 *  
 */
export default interface IPageBox extends IVsshElement{
    /**
     * 设置页面
     * @param page 页面
     */
    setPage(page:BaseElement):void
}