export default interface IVsshElement{
    // style:CSSStyleDeclaration
    /**
     * 注册事件，用法同原生的事件
     * @param type 
     * @param listener 
     */
    addEventListener(type:keyof HTMLElementEventMap,listener:(e:Event)=>void):void
    /**
     * 触发自定义事件
     * @param event 
     */
    dispatchEvent(event:Event):void
    /**
     * 渲染函数。每次父元素发生改变，都会调用子元素的render。
     * 所以在写的时候要注意，添加新元素前时要清空容器
     */
    render():HTMLElement
}