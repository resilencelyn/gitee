<template>
    <div class="home Mybgclor">
        <img  style="    float: left;margin-top: 17%;margin-left: 36%;"
              src="./../../static/img/logo.png"/>
        <div style="width: 80%;margin-left: 10%;margin-top: 50%">
            <van-search
                    v-model="number"
                    style="border-radius: 14px;"
                    placeholder="卡号绑定"
            />
            <van-button
                        :disabled="show_btn"
                        @click="bind" round
                        size="large"
                        style="margin-top: 10%;color: white;background-color: rgb(0 0 0 / 40%);"
                        native-type="submit"
                        >
                绑定
            </van-button>
        </div>

        </div>
</template>

<script>

    import tools from "../../utils/tools";

    export default {
        name: "bind",
        mounted() {
          let _this=this

          _this.openid=_this.$route.params.openid
          if(_this.$check.checkIsNull(_this.openid)){
              _this.openid=_this.$store.state.openid
          }

        _this.appid=_this.$route.params.appid
        if(_this.$check.checkIsNull(_this.appid)){
            _this.appid=_this.$store.state.appid
        }
           // _this.dis=false
        },
        data(){
            return{

                show_btn:false,
                iccid:"",
                openid:'',
                fits: ['fill', 'contain', 'cover', 'none', 'scale-down'],
                number:'',
                cardType:'',
                appid:"",
                webToken:'',
                //dis:false

            }
        },
        methods:{
            bind(){
                let _this=this;
                _this.show_btn=true;
                _this.iccid=_this.number.trim();
                if(_this.iccid===""){
                   _this.$toast.fail("请输入物联网卡号");
                    return false;
                }
                _this.openid=_this.$route.params.openid;
                if(_this.$check.checkIsNull(_this.openid)){
                    _this.openid=_this.$store.state.openid
                }
                //_this.dis=true
                let map = {};
                map.iccid = _this.iccid;
                map.openid = _this.openid;
                map.appid = _this.appid;
                let Pwd_Str = tools.encrypt(JSON.stringify(map));
                _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/bind-card", Pwd_Str).then((res) => {
                    let jsonobj = JSON.parse(tools.Decrypt(res.data));
                    if (jsonobj.code == "200") {
                        //绑定成功，保存当前的iccid，openid
                        let Obj = jsonobj.Data.Data;
                        _this.iccid = Obj.iccid;
                        _this.backstageIccid = Obj.iccid;
                        _this.cardType = Obj.cardType;
                        _this.webToken ="yzweb "+Obj.webToken;
                        //缓存token，用于请求
                        localStorage.setItem("web-token",_this.webToken);

                        _this.$router.push({
                            name:"index",
                            params:{
                                "iccid":_this.iccid,
                                "openid":_this.openid,
                                "cardType":_this.cardType,
                                "backstageIccid":_this.backstageIccid
                            }


                            /**/
                        })
                        _this.$store.commit("setCardType",_this.cardType);
                    }else{
                        _this.$toast.fail(jsonobj.msg);
                        _this.show_btn=false;
                        return false;

                    }

                })

            }
        }
    }
</script>

<style scoped>
.home {
    width: 100%;
    height: 100%;
    background-size:cover;
    position: fixed;

}


    .Mybgclor{
        background: -webkit-linear-gradient(left,#6519fa, #e6a23c,#67c23a);
        background: -o-linear-gradient(right,#6519fa, #e6a23c,#67c23a);
        background: -moz-linear-gradient(right,#6519fa, #e6a23c,#67c23a);
        background: linear-gradient(to right, #6519fa, #e6a23c,#67c23a);
    }


/deep/ .el-input--small .el-input__inner{
    height: 40px;
}
</style>