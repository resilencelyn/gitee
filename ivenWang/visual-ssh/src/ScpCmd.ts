/*
 * node-scp
 * <cam@onswipe.com>
 */
import {exec,spawn} from "child_process"
// let exec = require('').exec;

export default class ScpCmd{
  options:ScpOption
  constructor(props:ScpOption){
    this.options=props
  }
  /**
   * 发送一个文件
   * @param file 本地文件路径
   * @param path 服务器文件路径
   */
  send (file:string,path:string) {
    var command = [
      
      '-r',
      '-P',
      (this.options.port == undefined ? '22' : this.options.port),
      file,
      (this.options.username == undefined ? '' : this.options.username+'@') + this.options.host + ':' + path,
    ];
    console.log(command);
    // let proc=spawn('scp',command,{shell:true,stdio:[null, null, null, 'ipc']});
    let proc=spawn('scp',command,{shell:true});
    proc.stdout.pipe(process.stdout);
    process.on("message",(m)=>{
      console.warn("message",m)
    })
    process.on("data",(m)=>{
      console.warn("data",m)
    })
    process.stdout.on("message",(m)=>{
      console.warn("stdout.message",m)
    })
    process.stdout.on("data",(m)=>{
      console.warn("stdout.data",m)
    })

    proc.on("message",(m)=>{
      console.warn("proc.message",m)
    })
    proc.on("data",(m)=>{
      console.warn("proc.data",m)
    })
    proc.stdout.on("message",(m)=>{
      console.warn("proc.stdout.message",m)
    })
    proc.stdout.on("data",(m)=>{
      console.warn("proc.stdout.data",m)
    })
    // proc.on('message', (m) => {
    //   console.error(m);
    //   // proc.send(`< ${m}`);
    //   // proc.send('> 不要回答x3');
    // });
    // proc.
    // process.on("message",console.warn);
    // process.on("error",console.error);

    
    // proc.stdin.once('data', function(val) {
    //   fn(val.trim());
    // }).resume();

    // process.stdout.pipe(process.stdin);
    // console.log("fj",process)
    // proc.stdin.write(this.options.password+"\n");
    proc.on("data",(e)=>{
      console.log("数据",e)
      // proc.stdin.write(this.options.password+"\n");
    })
    process.stdout.on("data",(a)=>{
      console.log("主进程",a);
      process.stdin.write(this.options.password+"\n");
    })
    // proc.stdout.on("readable",(e: any)=>{
    //   let data;
    //   console.log("数据readable",e)
    //   while (data = proc.stdout.read()) {
    //     console.log(data);
    //   }
    //   proc.stdin.write(this.options.password+"\n");
    // })
    
  }

  // get (options, cb) {
  //   var command = [
  //     'scp',
  //     '-r',
  //     '-P',
  //     (options.port == undefined ? '22' : options.port),
  //     (options.user == undefined ? '' : options.user+'@') + options.host + ':' + options.file,
  //     options.path
  //   ];
  //   exec(command.join(' '), function (err, stdout, stderr) {
  //     if (cb) {
  //       cb(err, stdout, stderr);
  //     } else {
  //       if (err) throw new Error(err);
  //     }
  //   });
  // }
}

