import SpecialElement from "./SpecialElement";
/**
 * 图标组件 v2.1.3
 */
export default class Icon extends SpecialElement{
    i:HTMLElement
    constructor(props:IconConfig){
        super();
        this.i=document.createElement("i");
        let icons= new IconList();
        this.i.className=icons[props.name]
        let size=props.size||16
        this.style.fontSize=`${size}px`;
        this.style.height=`${size+10}px`;
        this.style.width=`${size+10}px`;
        if(props.color){
            this.style.color=props.color
        }
        if(props.bgcolor){
            this.style.backgroundColor=props.bgcolor;
        }
    }
    render(): HTMLElement {
        this.style.display="flex";
        this.style.justifyContent="center";
        this.style.alignItems="center";
        this.setHTMLElement(this.i);
        return super.render();
    }
}

export interface IconConfig{
    name:keyof IconList
    size?:number
    color?:string
    bgcolor?:string
}

export class IconList{
    arrowLeft="fa fa-arrow-left";
    arrowUp="fa fa-arrow-up";
    close="fa fa-close";
    copy="fa fa-copy";
    cut="fa fa-cut";
    database="fa fa-database"
    fileO="fa fa-file-o";
    folderO="fa fa-folder-o";
    folderOpenO="fa fa-folder-open-o";
    gears="fa fa-gears";
    help="fa fa-question";
    home="fa fa-home";
    imageO="fa fa-file-image-o";
    linux="fa fa-linux";
    paste="fa fa-paste";
    question="fa-question";
    shoppingBag="fa fa-shopping-bag"
}