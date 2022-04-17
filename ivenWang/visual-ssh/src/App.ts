import Connect from "./Connect";
import Program from "./Program";
import BaseElement from "./ui/core/BaseElement";
import { Tab } from "./ui/core/TabBar";

export default class App{
    appName="示例应用"
    page=new BaseElement()
    runtime:Program
    constructor(rt:Program){
        this.runtime=rt
    }
    /**
     * 应用启动时会自动调用这个方法
     * @param tab 应用所占用的标签页
     */
    onStart(tab?:Tab){
        throw new Error("没有重写onStart方法。它会在应用启动时自动调用");
    }
    createTab(){
        this.runtime.addNewTab({
            name:this.appName,
            page:this.page
        })
    }
}