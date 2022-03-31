<template>
    <div class="body">
        <headerNav :title="title" :balance="balance" :iccid="iccid" :openid="openid"></headerNav>
        <el-scrollbar style="height: 100%;margin-top: 5%">

            <div style="width: 100%;height: 100%;">
                <div class="CardInfoBox">
                    <van-row gutter="10">
                        <van-col style="padding: 5px;">
                            <img v-if="cardType=='电信'"
                                 width="80" height="80"
                                 src="../../static/img/dx.png"/>
                            <img v-if="cardType=='移动'"
                                 width="80" height="80"
                                 src="../../static/img/yd.png"/>
                            <img v-if="cardType==='联通'"
                                 width="80" height="80"
                                 src="../../static/img/lt.png"/>
                        </van-col>
                        <van-col style="margin-top: 10px;">
                            <span class="mylabel">{{'SIM: '+tel}}</span>
                            <div class="mylabel">{{'到期时间: '+EndTime}}</div>
                            <div class="mylabel">
                                状态：
                                <van-tag round plain size="medium" type="primary">{{cardStatus}}</van-tag>
                            </div>
                        </van-col>
                    </van-row>
                </div>


                <el-card style="background-color: white;
                    width: 98%;height: 28%;
                    text-align:center;margin:2% auto;border-radius: 12px">
                    <van-row gutter="10">
                        <van-col style="width: 100%" span="20">
                            <div style="font-size:12px;height: 14px;">
                                <label style="float: left;">总流量： {{total}}MB</label>
                                <label style="float: right;">已用： {{used}}MB</label>
                            </div>
                            <van-progress style="text-align: center;margin-top: 4%" :percentage="jindu"
                                          stroke-width="8"/>
                        </van-col>

                    </van-row>
                </el-card>

                <van-overlay :show="show" @click="show = false">
                    <van-loading size="50px" style="top:30%" vertical type="spinner" color="#1989fa">加载中</van-loading>
                </van-overlay>


                <van-grid>
                    <van-grid-item class="Myicon" icon="shopping-cart-o" text="资费订购" to="/wechat/packet"/>
                    <van-grid-item class="Myicon" icon="chart-trending-o" text="资费查询" to="/wechat/card_index" />
                    <van-grid-item v-if="cardType!=undefined && cardType!=null && cardType!=''" class="Myicon" icon="idcard" text="实名绑定" @click="GoAuth()" />
                    <van-grid-item class="Myicon" icon="bill-o" text="充值记录" to="/wechat/order_info" />
                    <van-grid-item class="Myicon" icon="close" text="解绑" @click="unbind()"/>
                </van-grid>


                <van-popup v-model="usedFlowShow" position="bottom" :style="{ height: '90%' }">
                    <div style="width: 95%;margin: 0 auto;text-align: center">
                        <p>历史用量</p>
                    </div>
                    <van-list
                            v-for="(i,index) in historyFlows"
                            :key="index"
                            style="width: 100%"
                    >
                        <van-cell :title="i.year+'-'+i.month+'-'+i.day" :value="i.totalFlowNow+'MB'"/>
                    </van-list>

                </van-popup>
                <div style="height: 1rem;"></div>
            </div>
        </el-scrollbar>
        <van-popup v-model="ltAuthShow" @close="ltAuthShow=false">
            <img width="200" height="200"    src="../../static/img/ltAuth.jpg"/>

        </van-popup>


    </div>

</template>

<script>
    import tools from "../../utils/tools";
    import headerNav from "../../components/headerNav";



    export default {
        name: "index",
        components: {
            "headerNav": headerNav,
        },
        mounted() {
            let _this = this;
            _this.backstageIccid = _this.$route.params.backstageIccid;
            if (_this.$check.checkIsNull(_this.backstageIccid)) {
                _this.backstageIccid = _this.$store.state.backstageIccid;
            }
            _this.iccid = _this.$route.params.iccid;
            if (_this.$check.checkIsNull(_this.iccid)) {
                _this.iccid = _this.$store.state.iccid;
            }
            _this.openid = _this.$store.state.openid;
            _this.cardType = _this.$route.params.cardType;
            if (_this.$check.checkIsNull(_this.cardType)) {
                _this.cardType = _this.$store.state.cardType;
            }

            _this.paySignNew = _this.$route.params.paySignNew;

            if (_this.$check.checkIsNull(_this.paySignNew)) {
                _this.paySignNew = _this.$route.params.paySign;
            }




            _this.code = _this.$route.params.paySign;
            _this.appId = _this.$store.state.appid;
            _this.list();
            //_this.loadCode()
            _this.getDict();

            _this.show = true


        },
        data() {
            return {
                ltAuthShow: false,
                show: false,
                appId: '',
                iccidDialog: false,
                iccidDialogPaySign: false,
                historyFlows: [],
                usedFlowShow: false,
                showCode: false,
                code: '',
                thumb: '',
                jindu: 0,
                used: 0.00,
                EndTime: '--',
                text: '',
                currentRate: 0.00,
                total: 0.00,
                remain: 0.00,
                balance: 0.01,
                desc: '',
                card: false,
                active: '',
                empty: false,
                openid: "",
                paySignNew: '',
                iccid: "",
                backstageIccid: "",
                cardType: '',
                title: '首页',
                auth: sessionStorage.getItem("auth"),
                cardStatus: sessionStorage.getItem("cardStatus"),
                tel: '',
                iccidTrue: '',
                cardTakeEffectTypeOption: [],//资费计划生效类型
            }
        },
        methods: {

            getDict() {
                //获取资费计划生效类型
                if (window['yunze_card_takeEffect_type'] != undefined && window['yunze_card_takeEffect_type'] != null && window['yunze_card_takeEffect_type'] != '') {
                    this.cardTakeEffectTypeOption = window['yunze_card_takeEffect_type'];
                } else {
                    this.getDictionary("yunze_card_takeEffect_type");
                    this.cardTakeEffectTypeOption = window['yunze_card_takeEffect_type'];
                    //延迟赋值 【解决动态编辑赋值的bug】
                    setTimeout(function () {
                        this.cardTakeEffectTypeOption = window['yunze_card_takeEffect_type'];
                    }, 1000);
                }
            },

            /*获取字典数据*/
            getDictionary(dict_type) {
                let _this = this;
                let map = {};
                map.dict_type = dict_type;
                map.iccid = _this.iccid;
                map.openid = _this.openid;
                let Pwd_Str = tools.encrypt(JSON.stringify(map));
                _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/pac/getDictionary", Pwd_Str).then((res) => {
                    let jsonobj = JSON.parse(tools.Decrypt(res.data));
                    if (jsonobj.code == "200") {
                        window[dict_type] = jsonobj.Data.Data;
                    } else {
                        _this.$toast.fail(jsonobj.msg)
                    }
                })

            },


            //解绑
            unbind() {
                let _this = this;
                let map = {};
                map.openid = _this.openid;
                map.iccid = _this.iccid;
                let Pwd_Str = tools.encrypt(JSON.stringify(map));
                _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/un-bind", Pwd_Str).then((res) => {
                    let jsonobj = JSON.parse(tools.Decrypt(res.data));
                    if (jsonobj.code == "200") {
                        let Obj = jsonobj.Data;
                        sessionStorage.clear();
                        _this.$toast.success(Obj.Message);
                        _this.$router.push({
                            name: "bind",
                            params: {
                                "openid": _this.openid,
                                "appid": _this.appid
                            }
                        })
                    } else {
                        _this.$toast.fail(res.data.msg);
                    }
                })
            },
            //去实名
            GoAuth(){
                if(tools.Is_null(this.cardType)){
                    switch (this.cardType) {
                        case "电信":
                            location.href="http://uniteapp.ctwing.cn:9090/uapp/certifhtml/certif_entry.html?userCode=r/s9Tc00hjiKcR13MIjZHg==&passWord=9u8VuY1xbez6r6k/BBnLlw==&tmstemp=1595318328145&from=groupmessage&isappinstalled=0";
                            break;
                        case "移动":
                            this.$toast.fail("移动EC不需要实名！");
                            break;
                        case "联通":
                            this.ltAuthShow = true;
                            break;
                        default:
                            this.$toast.fail("暂未获取到该设备运营商信息，请关闭界后重试！");
                            break
                    }
                }
            },


            toDouble(total, used, remain) {
                let _this = this
                _this.total = parseFloat(total).toFixed(2)
                _this.used = parseFloat(used).toFixed(2)
                _this.remain = parseFloat(remain).toFixed(2)
            },
            toPacket() {
                let _this = this
                _this.$router.push({
                    name: "packet",
                    params: {
                        "iccid": _this.iccid
                    }
                })
            },
            list() {
                let _this = this;
                let map = {};

                if (_this.iccid != null && _this.iccid != '' && _this.openid != null && _this.openid != '') {
                    map.iccid = _this.iccid;
                    map.openid = _this.openid;
                    let Pwd_Str = tools.encrypt(JSON.stringify(map));
                    _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/pac/qu-flow", Pwd_Str).then((res) => {
                        //console.log(res);
                        let jsonobj = JSON.parse(tools.Decrypt(res.data));
                        //console.log(jsonobj);
                        _this.show = false;
                        let ratiob = 0;
                        let ratio = 0.00;
                        if (jsonobj.code == "200") {
                            let Obj = jsonobj.Data;

                            //先清空state
                            _this.$store.commit("removeTotal", "");
                            _this.$store.commit("removeUsed", "");
                            _this.$store.commit("removeRemain", "");
                            _this.$store.commit("removeAuth", "");
                            _this.$store.commit("removeCardStatus", "");
                            _this.$store.commit("removeIccid", "");
                            //_this.$store.commit("removeBackstageIccid","");

                            _this.cardStatus = Obj.cardStatus;
                            _this.tel = Obj.iccid;
                            _this.iccidTrue = Obj.iccid;
                            _this.total = Obj.total;
                            _this.used = Obj.used;//需从接口获取
                            _this.remain = Obj.remain;
                            _this.EndTime = Obj.EndTime!=undefined && Obj.EndTime!=null?Obj.EndTime:"--";


                            if (_this.used === -1.00 && _this.remain === -1.00 && _this.total === -1.00) {
                                _this.used = "查询异常";
                                _this.remain = "查询异常";
                                _this.total = "查询异常";
                            } else {
                                _this.toDouble(_this.total, _this.used, _this.remain);
                                ratio = 0.00
                                if (_this.used === null || _this.used === "NaN") {
                                    _this.used = 0.00
                                }
                                if (_this.total === null || _this.total === "0.00" || _this.total === "NaN") {
                                    _this.total = 0.00
                                    ratio = 0.00
                                } else {
                                    ratio = _this.used / _this.total * 100
                                }
                                //检测是否流量已经用完
                                if (_this.total === 0.00 && _this.used > 0.00) {
                                    ratiob = 100.00;
                                    ratio = 100.00;
                                } else {
                                    ratiob = ratio.toFixed(2)
                                }
                            }
                            //缓存
                            _this.$store.commit("setTotal", _this.total);
                            _this.$store.commit("setUsed", _this.used);
                            _this.$store.commit("setRemain", _this.remain);
                            _this.$store.commit("setCardStatus", _this.cardStatus);
                            _this.$store.commit("setIccid", _this.iccid);
                            _this.$store.commit("setOpenid", _this.openid);
                            _this.$store.commit("setAppid", _this.appId);
                           // _this.$store.commit("setEndTime", _this.EndTime);
                        } else {
                            _this.$toast.fail(jsonobj.msg)
                        }
                        _this.currentRate = ratio;
                        _this.jindu = parseFloat(_this.currentRate.toFixed(2));


                    }).catch(error => {
                    })
                } else {
                    _this.$router.push({
                        name: "bind",
                        params: {
                            "openid": _this.openid,
                            "appid": _this.appid
                        }
                    })
                }


            }
        }
    }
</script>

<style scoped>
    .el-scrollbar__wrap {
        overflow: hidden;
    }

    /deep/ .van-cell {
        background-color: #efe0de;
    }

    /deep/ .van-grid-item__content {

    }

    .mylabel {
        font-size: 15px;
        color: white;
    }

    .body {
        width: 100%;
        height: 100%;
        background-size: 100% 100%;
    }

    .Myicon{
        color: #1989fa;
    }

    .CardInfoBox {
        width: 96%;
        height: 60%;
        margin-top: 1%;
        margin-left: 2%;
        background-image: -webkit-radial-gradient(top, circle cover, #e6a23c 0%, #409eff 80%);
    }

</style>