<template>
    <div>
        <router-view/>
    </div>

</template>

<script>
    import tools from "../utils/tools";
    export default {
        name: "wechat",
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
                        _this.$toast.success(res)
                        console.log(res);
                        if(res.data.code==="200"){
                            _this.isLoad=false
                            _this.$toast.success("授权成功")
                            //查看当前卡号是否已经绑定，已绑定进入首页，未绑定重新绑定
                            _this.openid=res.data.data
                            _this.$store.commit("setOpenid",_this.openid)
                            if(location.href.indexOf("cardReplacement")!==-1){
                                _this.$router.push({
                                    name:"cardReplacement",
                                    params:{
                                        "openid":_this.openid,"appid":appId
                                    }
                                })
                                return
                            }else if(location.href.indexOf("byCard")!==-1){
                                _this.$router.push({
                                    name:"byCard",
                                    params:{
                                        "openid":_this.openid,"appid":appId
                                    }
                                })
                                return
                            }

                            let map = {};
                            map.openid = _this.openid;
                            let Pwd_Str = tools.encrypt(JSON.stringify(map));
                            _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/ck-bind", Pwd_Str).then((res) => {
                                let jsonobj = JSON.parse(tools.Decrypt(res.data));
                                     if (jsonobj.code == "200") {
                                        let Obj = jsonobj.Data.Data;
                                        // console.log(Obj);
                                        if(Obj==null){
                                            _this.$router.push({
                                                name:"bind",
                                                params:{"openid":_this.openid,"appid":appId}
                                            })
                                        }else{
                                            _this.iccid = Obj.iccid;
                                            _this.$store.commit("setIccid",_this.iccid);
                                            map.iccid = _this.iccid;
                                            let Pwd_Str_1 = tools.encrypt(JSON.stringify(map));
                                            _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/go-index", Pwd_Str_1).then((res1) => {
                                                let jsonObject = JSON.parse(tools.Decrypt(res1.data));
                                                //console.log(jsonObject);
                                                if (jsonObject.code == "200") {
                                                    let Obj_1 = jsonObject.Data.Data;
                                                    //console.log(Obj_1);
                                                    _this.iccid = Obj_1.iccid;
                                                    _this.cardType = Obj_1.cardType;
                                                    _this.$store.commit("setCardType",_this.cardType);
                                                    _this.$router.push({
                                                        name:"index",
                                                        params:{
                                                            "iccid":_this.iccid,
                                                            "openid":_this.openid,
                                                            "cardType":_this.cardType,
                                                            "appId":appId
                                                        }
                                                    })
                                                }else{
                                                    /*_this.$toast.fail("违规操作，请重新登录")*/
                                                    _this.$router.push({
                                                        name:"bind",
                                                        params:{"openid":_this.openid,"appid":appId}
                                                    })
                                                }
                                            })
                                        }
                                    }else{
                                         _this.$toast.fail(jsonobj.msg)
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

