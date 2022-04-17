import BaseElement from './BaseElement'
/**
 * 加载动画v0.2
 */
export default class Loading extends BaseElement{
    transparent:LoadingTransparent=new LoadingTransparent();
    circleContainer:LoadingContainer=new LoadingContainer();
    circle:BaseElement;
    constructor(props:LoadingConfig){
        super();
        this.style.position="fixed";
        this.style.width="100%";
        this.style.height="100%";
        this.style.left="0";
        this.style.top="0";
        switch (props.type){
            case "LoadingTest2":
                this.circle= new LoadingTest2();
                break;
            case "LoadingCircle":
            default:
                this.circle=new LoadingCircle();
        }
        this.circleContainer.setInnerElement(this.circle);
        
    }
    show(){
        this.style.display="block";
    }
    hidden(){
        this.style.display="none";
    }
    beforeRender(): void {
        this.setInnerElement(this.transparent);
        this.appendChild(this.circleContainer);
    }
}

class LoadingTransparent extends BaseElement{
    constructor(){
        super()
        this.style.position="absolute";
        this.style.width="100%";
        this.style.height="100%";
        this.style.backgroundColor="rgba(0,0,0,0.8)";
    }
}

class LoadingContainer extends BaseElement{
    constructor(){
        super()
        this.style.position="absolute";
        this.style.zIndex="265359124";
        this.style.left="50%";
        this.style.top="50%";
    }
}

/**
 * 测试款加载
 */
class LoadingCircle extends BaseElement{
    constructor(){
        super();
        this.style.position="absolute";
        this.style.zIndex="2653591247";
        this.style.border="5px solid #FFF";
        this.style.borderRadius="50%"
        this.style.width="100px";
        this.style.height="100px";
        this.style.margin="-50px";
        this.style.animation="scaleout 1.0s infinite ease-in-out";
    }
}

class LoadingTest2 extends BaseElement{
    constructor(){
        super();
        this.style.border= "16px solid #f3f3f3";
        this.style.borderRadius= "50%";
        this.style.borderTop= "16px solid #3498db";
        this.style.width= "120px";
        this.style.height= "120px";
        this.style.animation= "loadingTest2 2s linear infinite";
        this.style.margin="-60px";
    }
}

interface LoadingTypeList{
    "LoadingCircle":LoadingCircle,
    "LoadingTest2":LoadingTest2
}

export interface LoadingConfig{
    type:keyof LoadingTypeList
}
