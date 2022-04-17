/**
 * 工具方法集v0.2
 */
export default class Tools{
    /**
     * 清除ANSI
     * @param str 字符串
     * @returns 
     */
    static clearANSI(str:string){
        str=str.replace(/\x1b\[[0-9;]*m/gi,"")
        return str.replace(/\x0f/gi,"")
    }
    /**
     * 获取uri中表示文件名的部分
     * 如果传/bin/bash 返回bash
     * 如果伟/bin/返回空字符串
     * @param path 一个类似/bin/bash那样的字符串
     * @returns 
     */
    static getFileNameFormUri(path:string){
        let pathArr=path.split("/")
        return pathArr[pathArr.length-1];
    }
    /**
     * 等待
     * @param time 毫秒数
     * @returns 
     */
    static async sleep(time:number){
        return await new Promise((resolve,reject)=>{
            setTimeout(resolve,time);
        })
    }
}