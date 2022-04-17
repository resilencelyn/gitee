import IVsshElement from "../IVsshElement";
import Loading from "../core/Loading";
import BaseElement from "../core/BaseElement";
import Tools from "../../Tools";
import PageBase, { PageConfig } from "./PageBase";

export default class PkgManager extends PageBase{
    loading:Loading
    constructor(config:PageConfig){
        super(config);
        this.loading=new Loading({
            type:"LoadingCircle"
        });
        this.setInnerElement(this.loading);

    }
    setInstalled(page:IVsshElement){
        this.setInnerElement(page);
    }
    render(){
        return super.render();
    }
}