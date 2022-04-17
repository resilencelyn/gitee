import IVsshElement from "../IVsshElement";

/**
 * 如果一个基本标签要添加图片，图标，表单之类的东西，应该继承这个类。BaseElement不要管这些玩意儿。
 */
export default class SpecialElement implements IVsshElement{
    protected ele:HTMLElement
    constructor(){
        this.ele= document.createElement("div");
    }
    get style(){
        return this.ele.style
    }
    addEventListener(type: keyof HTMLElementEventMap, listener: (e: Event | MouseEvent) => void): void {
        this.ele.addEventListener(type,listener);
    }
    dispatchEvent(event: Event): void {
        this.ele.dispatchEvent(event);
    }
    setHTMLElement(ele:HTMLElement){
        this.ele.innerText=""
        this.ele.appendChild(ele)
    }
    render(): HTMLElement {
        this.ele.classList.add(this.constructor.name)
        return this.ele;
    }
}