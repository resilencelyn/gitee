import electron, { ipcRenderer } from "electron";
import { FileType } from "../../IVsshStatus";
import Icon, { IconList } from "../core/Icon";
import ListIcon from "./ListIcon";

/**
 * 文件类型对应的图标
 */
let FileIconList:Array<keyof IconList>=[]
FileIconList[FileType.folder]="folderO"
FileIconList[FileType.exe]="gears"
FileIconList[FileType.other]="fileO"
FileIconList[FileType.image]="imageO"

/**
 * 文件类型对应的卡片颜色
 */
let CardBg:string[]=[]
CardBg[FileType.folder]="#0000CC"
CardBg[FileType.exe]="#00CC00"
CardBg[FileType.other]="#333333"
CardBg[FileType.image]="gold"
/**
 * 文件图标
 */
export default class FileIcon extends ListIcon{
    constructor(props:FileIconConfig){
        super({
            iconId:<keyof IconList>FileIconList[props.type],
            text:props.name,
            bgColor:CardBg[props.type],
            fontColor:"#FFFFFF"
        })
        this.style.transform="scale(1)"
        this.style.transition="transform 0.5s"
        this.addEventListener("mousemove",()=>{
            this.style.transform="scale(1.05)";
        })
        this.addEventListener("mouseout",()=>{
            this.style.transform="scale(1)";
        })
        this.setDraggable(true);
        this.addEventListener("dragstart",(event)=>{
            event.preventDefault()
            ipcRenderer.send('ondragstart', "hello")
        })
    }
}

export interface FileIconConfig{
    /**
     * 文件名
     */
    name:string
    /**
     * 文件类型
     */
    type:FileType
}
