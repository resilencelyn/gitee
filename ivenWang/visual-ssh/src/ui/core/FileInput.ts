import BaseElement from "./BaseElement";
import Button from "./Button";
import Input from "./Input";

export default class FileInput extends BaseElement{
    title="";
    urlBox:Input;
    selectFile:Input;
    fileBtn:Button
    constructor(props:FileInputConfig){
        super();
        this.title=props.title||"";
        let path=props.path||"";
        this.urlBox=new Input({
            type:"text",
            value:path
        })
        this.urlBox.innerStyle.width="116px";
        this.selectFile=new Input({
            type:"file"
        })
        // this.selectFile.style.visibility="hidden"
        this.selectFile.style.display="none";
        this.fileBtn=new Button({
            icon:"folderOpenO",
        })
        this.fileBtn.style.margin="0";
        this.fileBtn.addEventListener("click",e=>{
            this.selectFile.click();
        })
        this.selectFile.addEventListener("change",e =>{
            let files=this.selectFile.getFiles();
            if(files)
            this.urlBox.setValue(files[0].path);
        })
        this.style.display="flex";
    }
    getValue(): string {
        return this.urlBox.getValue();
    }
    setValue(v: string): void {
        this.urlBox.setValue(v);
    }
    beforeRender(): void {
        let label=new BaseElement({
            innerElement:this.title
        })
        label.style.width="100px";
        this.setInnerElement(label);
        this.appendChild(this.urlBox)
        this.appendChild(this.selectFile);
        this.appendChild(this.fileBtn);
    }
}
interface FileInputConfig{
    title?:string
    path?:string
}