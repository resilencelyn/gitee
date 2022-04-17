import { FileType, IDFile } from "./IVsshStatus";

/**
 * 虚拟文件系统，用于缓存远方的文件系统 v0.1
 */
export default class VirtualFileSystem{
    /**
     * 虚拟文件系统根目录
     */
    private fileTree:DFile=new DFile({
        name:"",
        type:FileType.folder
    })
    /**
     * 当前目录
     */
    private currentPath=this.fileTree;
    /**
     * 当前用户个人目录
     */
    private home?:string
    /**
     * 添加一个虚拟绝对路径，以/为开头
     * @param path 
     */
    addVirtualPath(path:string,type=FileType.folder){
        console.warn("添加了路径",path)
        let arr=path.split("/");
        let findInPath=this.fileTree;
        arr.forEach((e,i)=>{
            if(e){
                // console.warn(e);
                if(i<arr.length-1){
                    findInPath= findInPath.findOrCreatePath(e,FileType.folder);
                }else{
                    findInPath= findInPath.findOrCreatePath(e,type);
                }
            }
        })
        return findInPath
    }

    /**
     * 切换目录
     * @param path 
     */
    cd(path:string){
        let arr=path.split("/");
        arr.forEach((e,i)=>{
            if(e==".."){
                let parent=this.currentPath.getParent();
                if(parent){
                    this.currentPath=parent;
                }
            }else if(e=="."){
                // 无需操作
            }else if(e==""){
                // 回主页
                if(i==0){
                    this.currentPath=this.fileTree;
                }
            }else{
                let cp=this.currentPath.findPath(e);
                if(cp){
                    this.currentPath=cp;
                }else{
                    throw new Error(`在目录${this.currentPath.getFullPath()} 下找不到${e}。`);
                }
            }
        })
    }
    /**
     * 获取当前目录
     * @returns 
     */
    getCurrentFileList(){
        return this.currentPath.ls();
    }

    /**
     * 获取当前完整路径
     * @returns 
     */
    getCurrentPath(){
        return this.currentPath.getFullPath();
    }
    /**
     * 获取当前用户个人目录
     * @returns 
     */
    getHome(){
        return this.home;
    }
    /**
     * 设置当前绝对路径
     * @param path 
     */
    setCurrentPath(path:string){
        let arr=path.split("/");
        this.currentPath=this.fileTree;
        arr.forEach(e=>{
            if(e){
                let cp=this.currentPath.findPath(e)
                if(cp){
                    this.currentPath=cp
                }else{
                    throw new ReferenceError(`目录${e}不在缓存中。请先创建缓存。查询路径：${path}`)
                }
            }
        })
        console.warn("当前目录",this.currentPath);
    }
    /**
     * 设置当前用户目录，只能设一次
     * @param path 
     */
    setHome(path:string){
        if(!this.home){
            this.home=path;
        }
    }
}

/**
 * 虚拟文件系统结点
 */
export class DFile {
    private parent?:DFile;
    public name=""
    private type:FileType
    private children:DFile[]=[]
    constructor(config:DFileConfig) {
        this.name=config.name;
        this.type=config.type;
        this.parent=config.parent
    }

    /**
     * 查找或创建路径.只查询表层。
     * @param path 路径
     * @param name 文件名
     * @param type 文件类型
     * @returns 
     */
    findOrCreatePath(name:string,type=FileType.folder){
        let rel=this.findPath(name);
        if(rel){
            return rel;
        }else{
            let i=this.children.push(new DFile({
                name,
                type,
                parent:this
            }));
            return this.children[i-1];
        }
    }

    /**
     * 找到某文件夹下的某文件.只查询表层。
     * @param path 文件所在目录
     * @param name 文件名
     * @returns 
     */
    findPath(name:string){
        if(this.getType()!=FileType.folder){
            throw new Error(`${this.name}不是目录！`)
        }else{
            if(this.children){
                return this.children.find(e=>e.name==name);
            }
        }
        return undefined;
    }

    /**
     * 列出全部子项
     * @returns 
     */
    ls(){
        if(this.type==FileType.folder){
            return this.children;
        }else{
            throw new Error(`${this.name}不是目录！`)
        }
    }

    /**
     * 获取当前项的完整路径
     * @returns 
     */
    getFullPath(){
        let fullPath="";
        let path:DFile | undefined=this;
        do{
            fullPath=`/${path.name}${fullPath}`;
            path=path.parent;
        }while(path&&path.parent);
        return fullPath;
    }

    getParent(){
        return this.parent;
    }

    getType(){
        return this.type;
    }
}
interface DFileConfig{
    name:string,
    type:FileType
    parent?:DFile
}