import BaseElement from "../core/BaseElement";
import Button from "../core/Button";
import FormInput from "../core/FormInput";

export default class NavBar extends BaseElement{
    url=""
    backButton:Button;
    lastButton:Button;
    homeButton:Button;
    rootButton:Button;
    urlBox:FormInput;
    constructor(props:NavBarConfig){
        super();
        this.backButton=new Button({
            icon:"arrowLeft"
        })
        this.lastButton=new Button({
            icon:"arrowUp"
        })
        this.homeButton=new Button({
            icon:"home"
        })
        this.rootButton=new Button({
            icon:"database"
        })
        
        this.urlBox=new FormInput({
            value:props.url
        })
    }
    setUrl(url:string){
        this.urlBox.setValue(url);
    }
    getUrl():string{
        return this.urlBox.getValue();
    }
    addEventListener(type: keyof NavBarElementEventMap, listener: (e: Event) => void): void {
        if(type=="back"){
            this.backButton.addEventListener("click",listener);
        }
        if(type=="last"){
            this.lastButton.addEventListener("click",listener);
        }
        if(type=="root"){
            this.rootButton.addEventListener("click",listener);
        }
        if(type=="home"){
            this.homeButton.addEventListener("click",listener);
        }
    }
    beforeRender(): void {
        this.style.borderBottom="solid 1px #999999"
        this.style.display="flex"
        this.setInnerElement(this.backButton)
        this.appendChild(this.lastButton);
        this.appendChild(this.urlBox);
        this.appendChild(this.homeButton);
        this.appendChild(this.rootButton);
    }
}

export interface NavBarConfig{
    url:string
}

export interface NavBarElementEventMap extends HTMLElementEventMap{
    back:Event;
    last:Event;
    home:Event;
    root:Event;
}