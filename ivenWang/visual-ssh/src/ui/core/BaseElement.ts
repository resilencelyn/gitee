import IVsshElement from "../IVsshElement";
/**
 * 元素的基类，以后所有元素都会逐渐改为继承这个类，而不是实现IvsshElement接口。
 * 这个类设计是比较失败的，没有达到预想中的封装效果
 * 计划拆成两个类：ContainterElement,TextElement，分别容纳元素和对象？
 * v2.6
 */
export default class BaseElement implements IVsshElement{
    /**
     * 请把所有东西都添加至这个元素中。
     */
    private rel = document.createElement("div");
    constructor(props?:BaseElementConfig){
        if(props){
            if(props.innerElement){
                this.setInnerElement(props.innerElement);
            }
        }
    }
    get clientHeight(){
        return this.rel.clientHeight
    }
    get clientWidth(){
        return this.rel.clientWidth
    }
    get style(){
        return this.rel.style;
    }

    get offsetHeight(){
        return this.rel.offsetHeight
    }

    get offsetWidth(){
        return this.rel.offsetWidth;
    }

    /**
     * 注册事件监听。其内部调用了元素的addEventListener。
     * @param type 
     * @param listener 
     */
    addEventListener(type: keyof HTMLElementEventMap, listener: (this: HTMLDivElement,e: Event|MouseEvent) => any): void {
        this.rel.addEventListener(type,listener);
    }

    /**
     * 以后不要重写render了，太麻烦。重写这个beforeRender，每次渲染前都会调用它～～～
     */
    beforeRender(){
        console.warn("未重写beforeRender方法。")
    }

    /**
     * 手动触发事件
     * @param event 
     */
    dispatchEvent(event:Event){
        this.rel.dispatchEvent(event);
    }

    /**
     * 强制重新渲染。大部分情况不建议调用。
     */
    forceRerender(){
        this.rel=<HTMLDivElement>this.render();
    }

    /**
     * 设置元素id
     * @param id 
     */
    setId(id:string){
        this.rel.id=id;
    }

    /**
     * 设置内部元素。会覆盖原有元素。相当于给innerHTML或innerText赋值。
     * @param innerElement 
     */
    setInnerElement(innerElement:string|IVsshElement){
        if((<IVsshElement>innerElement).render){
            this.rel.innerText="";
            this.rel.appendChild((<IVsshElement>innerElement).render());
            
        }else{
            this.rel.innerText=<string>innerElement;
            console.warn("通过setInnerElement直接插入HTML将在后续版本中取消。");
        }
    }

    getText(){
        return this.rel.innerText;
    }

    /**
     * 设置是否可拖动
     * @param d 
     */
    setDraggable(d:boolean){
        this.rel.draggable=d;
    }

    /**
     * 设置高
     * @param height 当值为number时，单位为px；string时需自己设置单位
     */
    setHeight(height:string|number){
        if(typeof height=="number"){
            this.style.height=height+"px";
        }else{
            this.style.height=height;
        }
    }

    /**
     * 设置宽
     * @param width  当值为number时，单位为px；string时需自己设置单位
     */
    setWidth(width:string|number){
        if(typeof width=="number"){
            this.style.width=width+"px";
        }else{
            this.style.width=width;
        }
    }

    /**
     * 添加子元素
     * @param child 
     */
    appendChild(child:IVsshElement){
        this.rel.appendChild((<IVsshElement>child).render());
    }
    /**
     * 渲染。重写这个方法时，只需要将元素添加进rel再将super.render()返回即可。
     * * 2022.1.30 起，不要重写这个方法了，去重写@see {@link beforeRender} 吧。
     * @returns 
     */
    render(): HTMLElement {
        this.beforeRender();
        this.rel.classList.add(this.constructor.name)
        return this.rel;
    }
}
export interface BaseElementConfig{
    innerElement?:string|IVsshElement
}