<template>
    <div>
        <router-view/>
    </div>

</template>

<script>
    import tools from "../utils/tools";
    import weixinPay from "../static/js/weixinPay";
    export default {
        name: "Shopping",
        data(){
            return {
                isLoad:true,
                number:'',
                iccid:'',
                openid:"",
                cardType:''

            }
        },
        mounted() {
            let _this=this
            _this.isLoad=true
            _this.getTicket()
        },
        methods:{
            getTicket(){
                let _this=this
                let objData=_this.getRequest()
                if(objData && objData.code){
                    //根据code向后端获取当前授权用户信息
                    let appId=_this.$store.state.appid;
                    _this.$ajax.get(process.env.VUE_APP_SERVER+"/web/gt-info/"+appId,{
                        params:{
                            "code":objData.code
                        }
                    }).then(res=>{
                        _this.$toast.success(res);
                        if(res.data.code==="200"){
                            _this.isLoad=false;
                            _this.$toast.success("授权成功");
                            _this.openid=res.data.data;
                            _this.$store.commit("setOpenid",_this.openid);
                            let UserMap = {"openid":_this.openid,"appId":appId};

                            //后台核对商品价格 运费
                            let Pwd_Str = tools.encrypt(JSON.stringify(UserMap));
                            _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/shopping/setToken", Pwd_Str).then(response => {
                                let jsonobj_1 = JSON.parse(tools.Decrypt(response.data));
                                _this.$toast.fail(jsonobj_1)
                                if (jsonobj_1.code == "200") {
                                    let webToken ="yzweb "+jsonobj_1.Data.webToken;
                                    //缓存token，用于请求
                                    localStorage.setItem("web-token",webToken);
                                    //_this.$toast.fail("准备跳转")
                                    _this.$router.push({
                                        name:"byCard",
                                        params:UserMap
                                    })
                                } else {
                                    _this.$toast.fail(jsonobj_1.msg);
                                    // return false
                                }
                            })
                        }else{
                            _this.$toast.fail("授权失败")
                        }
                    })

                }else{
                    //此处写公众号配置的接入地址
                    let jumpath =window.location.href
                    let urlArr =jumpath.split("?")
                    let url =urlArr[0]
                    let param=_this.getRequest()
                    let apid=null;
                    if(param!=null){
                        apid=param.appId
                    }else{
                        apid=_this.$store.state.appid
                    }

                    const pro=require('process')
                    //console.log(pro)
                    _this.$ajax.get(process.env.VUE_APP_SERVER+"/web/authorize/"+apid,{
                        params:{
                            "returnurl":encodeURIComponent(url)
                        }
                    }).then(res=>{
                        _this.isLoad=false
                        //跳转微信授权页面
                        window.location.href = res.data.data.returnurl
                        let appId=res.data.data.appId;
                        //缓存起来
                        _this.$store.commit("setAppid",appId)

                    })

                }
            },
            getRequest(){
                let url=location.search
                let theRequest =new Object()
                if (url.indexOf("?") !== -1) {
                    let str = url.substr(1);
                    let strs = str.split("&");
                    for(let i = 0; i < strs.length; i ++) {
                        theRequest[strs[i].split("=")[0]]=unescape(strs[i].split("=")[1]);
                    }
                    return theRequest
                }
            }
        }


    }
</script>

