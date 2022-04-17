import BaseElement from "../core/BaseElement";
import Button from "../core/Button";
import FileInput from "../core/FileInput";
import FormInput from "../core/FormInput";
import Loading from "../core/Loading";
import Title from "../core/Title";

/**
 * 登录页面v2.
 */
export default class Login extends BaseElement{
    host:FormInput;
    port:FormInput;
    username:FormInput;
    password:FormInput;
    keyFile:FileInput;
    submit:Button;
    loading:Loading;
    constructor(props:LoginConfig){
        super();
        this.host=new FormInput({
            label:"主机IP",
            value:<string>props.host
        })
        this.port=new FormInput({
            label:"端口",
            value:<string>props.port,
            placeholder:"一般是22"
        })
        this.username=new FormInput({
            label:"用户名",
            value:<string>props.username
        })
        this.password=new FormInput({
            label:"密码",
            type:"password",
            value:<string>props.password
        })
        this.keyFile=new FileInput({
            title:"私钥",
            path:props.privateKeyPath
        })
        this.keyFile.addEventListener("focus",e =>{
            alert(this.keyFile.getValue());
        })
        this.submit=new Button({
            text:"登录"
        })
        this.loading=new Loading({
            type:"LoadingTest2"
        })
        this.loading.hidden();
        this.submit.addEventListener("click",()=>{
            this.loading.show();
        })
    }
    addEventListener(type: keyof LoginIvsshEventMap, listener: (e: Event) => void): void {
        if(type=='login'){
            this.submit.addEventListener("click",listener);
        }
    }
    getHost():string{
        return this.host.getValue();
    }
    getPort():string{
        return this.port.getValue();
    }
    getUsername():string{
        return this.username.getValue();
    }
    getPassword():string{
        return this.password.getValue();
    }
    getPrivateKeyPath(){
        return this.keyFile.getValue();
    }
    closeLoadding(){
        this.loading.hidden();
    }
    beforeRender(): void {
        this.style.display="flex";
        this.style.flexDirection="column"
        this.style.justifyContent="center";
        this.style.alignItems="center";
        let title=new Title({
            text:"登录"
        })
        this.setInnerElement(title);
        this.appendChild(this.host);
        this.appendChild(this.port);
        this.appendChild(this.username);
        this.appendChild(this.password);
        this.appendChild(this.keyFile);
        this.appendChild(this.submit);
        this.appendChild(this.loading);
    }
}

export interface LoginConfig{
    host?:string
    port?:string
    username?:string
    password?:string
    privateKeyPath?:string
}
export interface LoginIvsshEventMap extends HTMLElementEventMap{
    "login":Event
}