import {Client, ClientChannel} from 'ssh2';
import fs from "fs";
import FileIcon from './ui/components/FileIcon';
import { FileType } from './IVsshStatus';
import List from './ui/core/List';
import client from 'scp2';
import { Tab } from './ui/core/TabBar';
import IParser from './parser/IParser';
import Program from './Program';
/**
 * 我决定为每个任务单独建立一个连接。建立连接在外面建吧。
 * v1.0
 */
export default class Connect{
    private stream:ClientChannel;
    private dataLock=false;
    private expect=""
    private renderPage:Tab;
    private parser:IParser
    private mainProgram:Program
    /**
     * 一个连接的封装
     * @param stream 连接的流
     * @param render 渲染结果的元素
     * @param nparser 创建渲染器，临时这么写。
     */
    constructor(conf:ConnectInnerConfig){
        this.mainProgram=conf.mainProgram;
        this.stream=conf.stream
        this.renderPage=conf.render;

        this.parser=conf.parserCreator(this,this.renderPage)
        console.error("有expect吧",this.mainProgram.getExpect())
        this.setExpect(this.mainProgram.getExpect()).then(()=>{
            this.parser.onConnected();
        })
    }

    /**
     * ssh是一收一发的，所以我需要验证一下是否接收完成。
     */
    public async checkCommandEnd(){
        
        return new Promise<boolean>((resovlve,reject)=>{
            let verifyCode=`checkEnd${Math.random()}`;
            this.stream.write(`echo ${verifyCode+"\n"}`);
            let onData=(data: Buffer)=>{
                let rel=data.toString();
                if(data.toString().trim().indexOf(verifyCode)>=0){
                    console.log("命令执行成功");
                    resovlve(true)
                }else{
                    this.stream.once('data',onData)
                }
            }
            this.stream.once('data',onData)
        })
    }
    
    /**
     * @deprecated
     * 获取上一条命令的退出代码。
     * @returns 0为正常，127为找不到命令
     */
    async getLastExitCode(){
        let exitCode=(await this.exec("echo $?")).data.trim();
        // 临时解决返回带命令的问题。这个根治要从exec方法中解决。
        let el=exitCode.trim().split('\n');
        exitCode=el[el.length-1];
        console.warn(`退出代码测试${exitCode}`);
        return Number(exitCode);
    }

    /**
     * 获取文件系统缓存系统
     * @returns 
     */
    getVirtualFileSystem(){
        return this.mainProgram.getVirtualFileSystem()
    }

    /**
     * 测试连接
     * @param config 
     */
    static async testConnect(conf:BaseConnectConfig){
        return await new Promise((resolve:(ok:boolean)=>void,reject)=>{
            let seed=Math.random();
            const conn = new Client();
            conn.on('ready', () => {
                resolve(true);
            }).connect({
                host: conf.host,
                port: conf.port,
                username:conf.username,
                privateKey:conf.privateKey,
                password:conf.password,
                readyTimeout:150000,
                
            })
            conn.on('error',(e)=>{
                alert("连接失败！");
                reject(e);
            });
        })
    }

    /**
     * 创建一个连接
     * @param conf 
     * @returns 一个连接的实例
     */
    static async create(conf:ConnectConfig){
        return await new Promise((resolve:(stream:Connect)=>void,reject)=>{
            let seed=Math.random();
            const conn = new Client();
            conn.on('ready', () => {
                console.log('Client :: ready');
                conn.shell((err, stream) => {
                    if (err) reject(err);
                    // 有些过长的命令会被换行。。。
                    stream.setWindow(256,1024,2048,8192);
                    stream.on('close', () => {
                        console.log('Stream :: close');
                        conn.end();
                    }).on('end',()=>{
                        console.log("数据传输完毕");
                    });
                    stream.on("data",(data:Buffer)=>{
                        fs.mkdir("./logs/",{recursive:true},(err,path)=>{
                            if(err){
                                console.error(err);
                            }
                            fs.writeFile(`./logs/${conf.username}@${conf.host}_seed_${seed}.log`,data,{flag:"a"},()=>{});
                        })
                        
                    })
                    resolve(new Connect({stream,
                        render:conf.renderPage,
                        mainProgram:conf.mainProgram,
                        parserCreator:conf.parserCreator
                        
                    }));
                    
                });
            }).connect({
                host: conf.host,
                port: conf.port,
                username:conf.username,
                privateKey:conf.privateKey,
                password:conf.password,
                readyTimeout:150000,
                
            })
            conn.on('error',(e)=>{
                alert("连接失败！");
                reject(e);
            });
        })
    }

    /**
     * 执行一个shell 命令
     * 这个方法实现起来蛮复杂的。
     * 我遇到过服务端以“添油战术”返回内容的，也见过一次性返回全部内容的。
     * ssh也没个表示传输开始和结束的东西，需要抗得干扰蛮多的。
     * @param cmd 要执行的命令
     * @returns 命令完整的返回会出现在resolve的字符串中
     */
     async exec(cmd:string){
        let sendCmd=`${cmd.trim()};echo VisualSSHStatus:$?\n`;
        await this.waitDataLockRelease();
        return await new Promise((resolve:(v:SSHResult)=>void,reject)=>{
            this.dataLock=true;
            this.stream.write(sendCmd);
            
            let rel="";
            let onData=(data: Buffer)=>{
                let hasCmd = data.toString().indexOf(sendCmd.trim());
                let hasTatile=data.toString().indexOf(this.expect);
                // console.log("%c 接收到的数据","color:green",data.toString());
                // 输出命令行，一定是开始
                if(hasCmd>=0){
 
                }
                // 有这个 标志，一定是结束
                // 但是也有可能把之前的命令行
                if(hasTatile>=0){
                        this.dataLock=false;
                        rel +=data.toString().substr(0,data.toString().indexOf(this.expect,-1))
                        rel=rel.replace(sendCmd.trim(),"");
                        let relArr=rel.split("VisualSSHStatus:")
                        let sshResult:SSHResult={
                            data: relArr[0].trim(),
                            exitCode: Number(relArr[1])
                        }
                        // console.log("返回内容",rel);
                        if(sshResult.exitCode!==0){
                            console.warn(`警告：命令${cmd}异常退出。退出码${relArr[1]}`)
                        }
                        resolve(sshResult)
                        return ;
                }
                rel=rel+data.toString();
                // console.log("来数据"+data.toString());
                this.stream.once('data', onData);
            }
            this.stream.once('data', onData);
        })
    }
    

    /**
     * 设置结束标志。没参数时是自动设置
     * --------
     * 对于这个双工流来说，没有办法保证发送和接收的数据是一一对应的，
     * 也没法保证每次接收的数据是完整的，所以第一步是测出数据结束标志物
     * @param title 
     */
    async setExpect (title?:string){
        if(title){
            this.expect=title;
            return;
        }
        let sendTest=async ()=>{
            return await new Promise((resolve:(v:string)=>void,reject)=>{
                this.stream.write("\n");
                this.stream.on('data', (data: Buffer) => {
                    let rel=data.toString("utf-8").trim();
                    resolve(rel);
                });
            });
        }
        let test1=await sendTest();
        let test2=await sendTest();
        let test3=await sendTest();
        if((test1==test2 || test2==test3)&& test2!=""){
            let expect=test2.split("~");
            this.expect=expect[0].trim();
            await this.checkCommandEnd();
            this.mainProgram.setExpect(this.expect);
        }else{
            await this.setExpect();
        }
    }

    /**
     * 等待数据锁
     * @returns 
     */
     private async waitDataLockRelease(){
        return await new Promise<void>((resolve, reject) => {
            let interval=setInterval(()=>{
                if(!this.dataLock){
                    resolve();
                }
            },100);
        })
    }   
}

interface ConnectConfig{
    username:string,
    password:string,
    host:string,
    port:number,
    privateKey:Buffer,
    renderPage:Tab,
    mainProgram:Program,
    parserCreator:(conn:Connect,tab:Tab)=>IParser
}

interface ConnectInnerConfig{
    stream:ClientChannel,
    render:Tab,
    mainProgram:Program,
    parserCreator:(conn:Connect,tab:Tab)=>IParser
}

interface BaseConnectConfig{
    username:string,
    password:string,
    host:string,
    port:number,
    privateKey:Buffer,
}