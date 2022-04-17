declare module 'scp2'{
    var scp:(file:string,option:Scp2Options,cb:(err:string)=>void) =>void ;
}

/**
 * MIME类型
 */
interface MIME{
    MIME:string,
    charset:string
}

interface ModalEventMap extends HTMLElementEventMap{
    cancel:Event,
    ok:Event,
    close:Event
}

interface ScpOption{
    username: string,
    host: string,
    port: string,
    /**
     * 服务器密码
     */
    password: string,
}

interface Scp2Options{
    host: string,
    username: string,
    password: string,
    path: string
}



/**
 * ssh的返回数据
 */
interface SSHResult{
    data:string;
    exitCode:number
}