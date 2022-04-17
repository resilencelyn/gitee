
import Container from "./ui/components/Container";

export default interface IVsshStatus{
    /**
     * 可输入命令的标志物，类似于[root@VM_0_6_centos ~]# iven@iven-Lenovo-TianYi-310-15ISK:~$ 
     */
    expect:string
    /**
     * 可输入命令的标志物需要系统自动判断。因为最先出现的玩意儿它不是。
     */
    lineTitleVerified:boolean
    /**
     * 当前登录的用户名
     */ 
    username:string
    password:string
    host:string
    port:string |number
    /**
     * 根元素
     */
    container:Container
    /**
     * 数据锁，当数据锁上锁时等待解锁再执行操作；
     */
    dataLock:boolean
    /**
     * 当前路径
     */
    path:string
    /**
     * 剪切板。现阶段只有string。
     */
    ClipBoard:string

    /**
     * 剪切板类型
     */
    ClipBoardOption:ClipBoardOption
}

/**
 * 文件类型
 */
export enum FileType{
    folder,
    exe,
    other,
    image
}

/**
 * 剪切板原操作类型
 */
export enum ClipBoardOption{
    Copy,
    Cut
}

export interface ClipBoardData{
    data:string
    option:ClipBoardOption
}

/**
 * 文件／文件夹列表项
 */
export interface IDFile{
    parent?:IDFile
    name:string,
    type:FileType
    children?:IDFile[]
}
