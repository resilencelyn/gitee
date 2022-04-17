import Connect from "../Connect";
import Container from "../ui/components/Container";
import { RightMenuListItem } from "../ui/components/RightMenuListItem";
import { PopUpConfig } from "../ui/core/PopUp";
import { Tab } from "../ui/core/TabBar";
import IParser from "./IParser";

/**
 * 解析器基类ｖ1.0
 * UI逻辑不要写解析器里面！！！
 */
export default class ParserBase implements IParser{
    protected conn:Connect
    protected container:Container
    protected renderTab:Tab
    constructor(connect:Connect,tab:Tab,container:Container){
        this.conn=connect
        this.renderTab=tab
        this.container=container
    }

    copy(data:string){
        this.container.copy(data)
    }

    /**
     * 给page调用，其实就是Connect.exec
     * @param cmd 
     */
    createRightMenu(config: PopUpConfig<RightMenuListItem[]>){
        this.container.createRightMenu(config)
    }

    cut(data:string){
        this.container.cut(data)
    }

    /**
     * 给page调用，其实就是Connect.exec
     * @param cmd 
     */
    async exec(cmd:string){
        return await this.conn.exec(cmd);
    }

    onConnected(){
        throw new Error(`解析器 ${this.constructor.name} 未实现用于挂载时执行命令的onConnected方法！！！`);
    }

    paste(){
        return this.container.paste()
    }
}