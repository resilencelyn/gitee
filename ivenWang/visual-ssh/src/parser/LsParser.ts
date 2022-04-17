import Connect from "../Connect";
import { ClipBoardOption, FileType, IDFile } from "../IVsshStatus";
import { Tab } from "../ui/core/TabBar";
import FileManager from "../apps/FileManager/FileManager";
import ParserBase from "./ParserBase";
import Container from "../ui/components/Container";
import { RightMenuListItem } from "../ui/components/RightMenuListItem";
import Tools from "../Tools";
import CreateFolder from "../ui/components/CreateFolder";
import { DFile } from "../VirtualFileSystem";

/**
 * 文件列表渲染器
 */
export default class LsParser extends ParserBase{
    page:FileManager
    constructor(connect:Connect,tab:Tab,container:Container){
        super(connect,tab,container);
        this.page=new FileManager(this);
        this.page.addEventListener("mousedown",(e)=>{
            let ev=<MouseEvent>e;
            if(ev.button ==2){
                let test=new RightMenuListItem({
                    icon:"folderO",
                    title:"创建目录"
                })
                test.addEventListener("click",(e)=>{
                    let mkdir=new CreateFolder();
                    container.createModal({
                        x:200,
                        y:100,
                        data:{
                            title:"创建目录",
                            body:mkdir,
                            onOk:async ()=>{
                                let dirName=mkdir.getValue();
                                await this.conn.exec(`mkdir ${this.page.getUrl()}/${dirName}`);
                                await this.ls(this.page.getUrl());
                                await this.page.ls();
                            },
                        }
                    })
                })
                // 有大量重复代码，可以共用 
                let paste=new RightMenuListItem({
                    icon:"paste",
                    title:"粘贴"
                })
                paste.addEventListener("click",this.onParse);
                let items:Array<RightMenuListItem>=[];
                let clipBoardData=this.container.paste()
                // 那么，剪切过一个文件，如果粘贴再次会怎样呢？？？
                if(clipBoardData?.data){
                    items.push(paste);
                }
                items.push(test);
                let menu=this.container.createRightMenu({
                    x:ev.x,
                    y:ev.y,
                    data:items,
                })
            }
        })
    }
    async onConnected(): Promise<void> {
        let pwdstr=await this.pwd();
        await this.ls(pwdstr);
        this.lsParser(pwdstr);
        this.renderTab.render();
        this.page.addEventListener("last",this.last);
        this.page.addEventListener("back",this.back);
        this.page.addEventListener("home",this.goHome);
        this.page.addEventListener("root",this.root);
    }
    
    /**
     * 后退
     */
    back = async ()=>{
        await this.cd("..");
        this.page.ls();
    }
    
    /**
     * 切换目录
     * @param path 
     */
    async cd(path: string){
        await this.ls(path);
        this.conn.getVirtualFileSystem().cd(path);
        this.page.setUrl(this.conn.getVirtualFileSystem().getCurrentPath());
    }

    /**
     * 获取文件类型
     * @param path 
     */
    async fileType(path:string):Promise<MIME>{
        let rel=await this.exec(`file -ib ${path}`);
        let arr=rel.data.split(",");
        return {
            MIME:arr[0],
            charset:arr[1]
        }
    }

    /**
     * 返回当前目录文件列表。
     * @returns 
     */
    getCurrentFileList(){
        return this.conn.getVirtualFileSystem().getCurrentFileList();
    }

    /**
     * 跳转到用户目录
     */
     goHome=async ()=>{
        let homepath = await this.home();
        await this.cd(homepath);
        this.page.ls();
    }

    /**
     * 设置当前用户主目录
     */
    async home(){
        let rel=await this.conn.exec("echo ~");
        return rel.data
    }

    /**
     * 返回上级的目录
     */
    last = async ()=>{
        await this.cd("..");
        this.page.ls();
    }

    /**
     * 执行ls -AlF
     * 从服务器端获取某目录下的文件列表。本方法应只用于数据更新
     * @param path 更新的路径，默认为/
     */
    async ls(path:string="/"){
        let rel= await this.conn.exec(`ls -AlF ${path}\n`);
        let arr=rel.data.split("\n");
        let totalInfo=arr.shift();// 总用量
        console.log("总用量",totalInfo);
        let asyncArr= arr.map(async ele=>{
            let file =ele.trim().split(" ");
            let fileName=file[file.length-1]
            //　先去除用于显示颜色的ＡＮＳＩ转义序列，以后也许需要用它区分文件类型
            fileName=Tools.clearANSI(fileName)
            if(fileName){
                let link:IDFile;
                switch(fileName.charAt(fileName.length-1)){
                    case "/":
                    case "\\":
                        fileName=fileName.substr(0,fileName.length-1);
                        link={
                            name:fileName,
                            type:FileType.folder
                        }
                        break;
                    case "*":
                        fileName=fileName.substr(0,fileName.length-1),
                        link={
                            name:fileName,
                            type:FileType.exe
                        }
                        break;
                    default:
                        let typeRes=await this.fileType(`${path}/${fileName}`);
                        let fileType=FileType.other
                        if(typeRes.MIME.search("image")>=0){
                            fileType=FileType.image
                        }
                        link={
                            name:fileName,
                            type:fileType
                        }
                }
                console.log(fileName);
                // 这里没判断重复诶。。。把link加进去
                if(path!="~"){
                    this.conn.getVirtualFileSystem().addVirtualPath(`${path}/${link.name}`,link.type);
                }
            }
        })
        await Promise.all(asyncArr);
    }

    /**
     * 点击粘贴时的事件处理
     */
    onParse=async ()=>{
        let rel=this.container.paste();
        if(rel?.option==ClipBoardOption.Cut){
            this.conn.exec(`mv ${rel.data} ${Tools.getFileNameFormUri(rel.data)}`)
        }
        if(rel?.option==ClipBoardOption.Copy){
            // 应该是剪切板里面的文件名
            this.conn.exec(`cp ${rel.data} ${Tools.getFileNameFormUri(rel.data)}`)
        }
        await this.ls(this.page.getUrl());
        this.page.ls();
    }

    async pwd(){
        let rel=await this.conn.exec("pwd");
        this.page.setUrl(rel.data);
        this.conn.getVirtualFileSystem().addVirtualPath(rel.data);
        this.conn.getVirtualFileSystem().setCurrentPath(rel.data);
        return rel.data
    }

    /**
     * @deprecated
     * @param rel 
     * @returns 
     */
    async pwdPaser(rel:string){
        rel=rel.replace("pwd","").trim();
        this.page.setUrl(rel);
        this.conn.getVirtualFileSystem().setCurrentPath(rel);
        return rel
    }

    root=async ()=>{
        await this.cd("/");
        this.page.ls();
    }

    userHome=async ()=>{
        let homePath=await this.home();
        await this.cd(homePath);
        this.page.ls();
    }

    /**
     * 
     * @param rel ls命令的返回
     * @param path 临时使用，当前路径
     */
    lsParser(path:string){  
        this.renderTab.setPage(this.page);
        this.conn.getVirtualFileSystem().setCurrentPath(path);
        this.page.ls();
        this.renderTab.activated();
        this.renderTab.addEventListener("dragover",(e)=>{
            e.stopPropagation();  
            e.preventDefault();
        });
        this.renderTab.addEventListener("drop",(e)=>{
            e.stopPropagation();  
            e.preventDefault(); 
            let dt=<DataTransfer>(<DragEvent>e).dataTransfer
            for(let i=0;i<dt.files.length;i++){
                console.log("发文件",dt.files[i].path,i)
                // scp.send(dt.files[i].path,(<NavBar>this.Status.navBar).getUrl());
                // client.scp(dt.files[i].path, {
                //     host: this.Status.host,
                //     username: this.Status.username,
                //     password: this.Status.password,
                //     path: (<NavBar>this.Status.navBar).getUrl()
                // }, function(err) {
                //     console.error(err);
                // });
            }
        }/*, false*/)
    }
}

