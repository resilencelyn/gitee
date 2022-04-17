import IVsshElement from "../IVsshElement";

/**
 * 本组件仅用于测试
 */
export default class Test implements IVsshElement{
    dispatchEvent(event: Event): void {
        throw new Error("Method not implemented.");
    }
    addEventListener(type: keyof HTMLElementEventMap, listener: (e: Event) => void): void {
        throw new Error("Method not implemented.");
    }
    render(): HTMLElement {
        let div=document.createElement("div");
        div.innerText="这是测试内容";
        return div;
    }
}