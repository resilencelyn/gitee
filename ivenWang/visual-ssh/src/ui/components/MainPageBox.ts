import BaseElement from "../core/BaseElement";
import IPageBox from "../core/IPageBox";

/**
 * 全局page容器
 * @deprecated
 */
export default class MainPageBox extends BaseElement implements IPageBox{
    constructor(props:MainPageBoxConfig){
        super();
        this.style.height="100%"
        this.style.overflow="overlay";
    }
    setPage(page: BaseElement): void {
        this.setInnerElement(page);
    }

}
interface MainPageBoxConfig{
}