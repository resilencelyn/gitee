import SpecialElement from "./SpecialElement";

export default class Title extends SpecialElement{
    constructor(props:TitleConfig){
        super();
        let h1=document.createElement("h1");
        h1.innerText=props.text
        this.setHTMLElement(h1);
    }
}
interface TitleConfig{
    text:string
}