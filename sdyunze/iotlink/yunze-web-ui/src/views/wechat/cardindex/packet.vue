<template>
    <div style="height: 100vh" class="bodyo">
        <el-scrollbar style="height: 100%">
            <headerNav :title="title" :iccid="iccid" :openid="openid"></headerNav>
            <van-tabs @click.native="clickTabs" style="width: 95%;height: 28%;
            text-align:center;margin:3% auto;">
                <van-tab style="color: black" title="当月续费"></van-tab>
                <van-tab style="color: black" title="资费预购"></van-tab>
            </van-tabs>

            <div>
                <van-cell-group style="width: 95%;
                          margin: 1% auto;border-radius: 12px">
                    <template #title>
                        <span style="font-size: 17px;color: black">基础资费</span>
                    </template>
                </van-cell-group>
                <van-cell class="box-card textSpan"
                          @click="clickSwipe(o)"
                          v-for="(o,i) in basePackets"
                          :key="i"
                >
                    <template #title>
                        <span class="textSpan" style="font-size:15px;">{{o.packet_wx_name+" | "+o.packet_valid_time+"*"+o.packet_valid_name+" | "+o.packet_price+" 元"}}</span>
                    </template>
                    <template>
                        <span class="textSpan" style="font-size:15px">{{o.packet_flow}}MB</span>
                    </template>


                </van-cell>
            </div>


            <div v-if="isNext">
                <van-cell-group style="width: 95%;
                          margin: 1% auto;border-radius: 12px">
                    <template #title>
                        <span style="font-size: 17px;color: black">叠加资费</span>
                    </template>
                </van-cell-group>
                <van-cell class="box-card textSpan"
                          @click.native="clickGrid(value)"
                          v-for="(value,index) in packets"
                          :key="index"
                >
                    <template #title>
                        <span class="textSpan" style="font-size:15px;">{{value.packet_wx_name+" | "+value.packet_valid_time+"*"+value.packet_valid_name+" | "+value.packet_price+" 元"}}</span>
                    </template>
                    <template>
                        <span class="textSpan" style="font-size:15px">{{value.packet_flow}}MB</span>
                    </template>
                </van-cell>
            </div>


            <div style="width:100%;height: 100%;">
                <van-dialog
                        :before-close="clickDialog"
                        v-model="show" title="支付类型"
                        confirm-button-text="微信"
                        cancel-button-text="余额"
                        show-cancel-button>
                </van-dialog>

                <van-popup v-model="pupshow" position="top" :style="{ height: '30%' }">
                    <van-steps :active="active">
                        <van-step>发起支付</van-step>
                        <van-step>开始支付</van-step>
                        <van-step>检测支付</van-step>
                        <van-step>支付完成</van-step>
                    </van-steps>
                    <div style="width: 100%;">
                        <span style="margin-left: 10%;width:95%;font-size: 14px;text-align: center;">{{msg}}</span>
                    </div>
                </van-popup>

                <van-popup round @close="closePop" v-model="showCode" :style="{ height: '30%',width:'90%' }">
                    <div style="margin:2% auto;text-align: center">
                        <van-form @submit="paySubmit">
                            <van-field
                                    style="margin-top: 3%"
                                    v-model="code"
                                    name="code"
                                    label="支付密码"
                                    placeholder="请输入您的6位支付密码"
                                    :rules="[{ required: true, message: '请填写支付密码' }]"
                            />
                            <div style="margin: 16px;">
                                <van-button :disabled="dis" round block type="info" native-type="submit">
                                    确认
                                </van-button>
                            </div>
                        </van-form>
                    </div>
                </van-popup>
            </div>
            <div style="height: 5rem"></div>
        </el-scrollbar>
        <rightBottom v-bind:iccid="iccid" v-bind:openid="openid"></rightBottom>
        <van-submit-bar :price="price" v-if="SubBt" button-text="提交订单" @submit="onSubmit"/>
    </div>


</template>

<script>
    import headerNav from "../../../components/headerNav";
    import footerNav from "../../../components/footerNav";
    import rightBottom from "../../../components/rightBottom"
    import weixinPay from "../../../static/js/weixinPay";
    import tools from "../../../utils/tools";


    export default {
        name: "packet",
        components: {
            "headerNav": headerNav,
            "footerNav": footerNav,
            "rightBottom": rightBottom
        },
        mounted() {
            let _this = this
            _this.iccid = _this.$route.params.iccid
            if (_this.$check.checkIsNull(_this.iccid)) {
                _this.iccid = _this.$store.state.iccid
            }
            _this.openid = _this.$route.params.openid
            if (_this.$check.checkIsNull(_this.openid)) {
                _this.openid = _this.$store.state.openid
            }
            _this.appId = _this.$store.state.appid

            _this.dis = false
            _this.loadList()
        },
        data() {
            return {
                SubBt: true,
                isNext: true,
                dis: false,
                title: '资费订购',
                active: 1,
                msg: '正在支付...',
                pupshow: false,
                show: false,
                packets: [],
                basePackets: [],
                price: 0.00,
                priceStr: '',
                openid: '',
                iccid: '',
                packetId: "",
                monthType: "month",
                nextBasePackets: [],
                nextPackets: [],
                code: '',
                showCode: false,
                appId: '',
            }
        },
        methods: {
            closePop() {

            },
            paySubmit(value){

            },

            wxPay() {
                let _this = this;
                //先查询是否可以进行余额支付
                let map = {};
                map.iccid = _this.iccid;
                map.openid = _this.openid;
                map.packet_id = _this.packetId;


                if (_this.iccid != null && _this.iccid != '' && _this.openid != null && _this.openid != '') {
                    let Pwd_Str = tools.encrypt(JSON.stringify(map));
                    _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/pac/querPayType", Pwd_Str).then((res) => {
                        let jsonobj = JSON.parse(tools.Decrypt(res.data));
                        if (jsonobj.code == "200") {
                            let cardPackets = jsonobj.Data.cardPacket;
                            let payType = parseInt(cardPackets.wechat_pay);
                            if (payType === 1) {
                                //获取后台数据
                                let encrypt_text = tools.encrypt(JSON.stringify({
                                    "iccid": _this.iccid,
                                    "body": "Renewal_recharge",
                                    "packet_id": _this.packetId,
                                    "price": _this.priceStr,
                                    "monthType": _this.monthType,
                                    "openid": _this.openid,
                                    "appId": _this.appId
                                }));
                                _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/pay/weixinPay", encrypt_text).then(response => {
                                    let jsonobj_1 = JSON.parse(tools.Decrypt(response.data));
                                    if (jsonobj_1.code == "200") {
                                        weixinPay(jsonobj_1.Data).then(res => {
                                            _this.$toast.success("success")
                                        }).catch(err => {
                                            //console.log(err)
                                            _this.$toast.fail("error" + err)
                                        })
                                    } else {
                                        _this.$toast.fail(jsonobj_1.msg);
                                        // return false
                                    }
                                })
                            } else {
                                _this.$toast.fail("不支持微信支付");
                                //return false
                            }
                        }
                        _this.$toast.fail(jsonobj.msg);
                        _this.SubBt = true;
                    })
                }
            },


                 clickDialog(event,done){

                   },
                   startBalance(){

                   },
                clickTabs(e)
                {
                    let _this = this;
                    if (e.target.outerText === "当月续费") {
                        _this.monthType = "month";
                        _this.isNext = true;
                        _this.loadList();
                    } else {
                        _this.monthType = "next";
                        _this.isNext = false;
                        //只显示基础包
                        let nextBase = new Array();
                        _this.basePackets.forEach((item, index) => {
                            //console.log(item);
                            if (item.packet_type == "0" && item.packet_valid_type != '4') {
                                nextBase.push(item)
                            }
                        })
                        _this.basePackets = nextBase;
                    }
                }
            ,
                loadList()
                {
                    //根据当前iccid查询所在的代理商
                    //若没有代理商，使用平台资费包
                    //若有使用代理商设定的加包
                    let _this = this;
                    let map = {};
                    map.iccid = _this.iccid;
                    map.openid = _this.openid;
                    if (_this.iccid != null && _this.iccid != '' && _this.openid != null && _this.openid != '') {
                        let Pwd_Str = tools.encrypt(JSON.stringify(map));
                        _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/pac/qu-packet", Pwd_Str).then((res) => {
                            //console.log(res);
                            let jsonobj = JSON.parse(tools.Decrypt(res.data));
                            //console.log(jsonobj);
                            _this.show = false;
                            let ratiob = 0;
                            let ratio = 0.00;
                            if (jsonobj.code == "200") {
                                let cardPackets = jsonobj.Data.cardPackets;
                                let basePackets = [];
                                let packets = [];
                                //console.log(cardPackets);
                                for (let i = 0; i < cardPackets.length; i++) {
                                    let obj = cardPackets[i];
                                    if (obj.packet_type == "0" || obj.packet_type == "2") {
                                        basePackets.push(obj);
                                    } else if (obj.packet_type == "1") {
                                        packets.push(obj);
                                    }
                                }
                                _this.basePackets = basePackets;
                                _this.packets = packets;
                            } else {
                                _this.$toast.fail(jsonobj.msg)
                            }
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
            ,


                clickGrid(value)
                {
                    let _this = this
                    _this.price = parseFloat(value.packet_price) * 100
                    _this.priceStr = parseFloat(_this.price / 100).toString()
                    _this.packetId = value.packet_id

                }
            ,
                clickSwipe(event)
                {
                    let _this = this;
                    _this.price = parseFloat(event.packet_price) * 100;
                    _this.priceStr = parseFloat(_this.price / 100).toString();
                    _this.packetId = event.packet_id;
                }
            ,
                onSubmit()
                {
                   let _this = this;
                    if (_this.packetId == null || _this.packetId === "") {
                        _this.$toast.fail("请先选择资费");
                        return false;
                    }
                    _this.wxPay();
                    _this.SubBt = false;
                }
            ,
            },
        }
</script>

<style scoped>
    .box-card:hover {
        color: #ffffff;
        background-color: white;
        background-color: #F7691A;
    }

    .box-card {
        width: 96%;
        margin: 1% auto;
        height: 60px;
        border-radius: 12px;
        border: 1px solid orange;
    }

    .textSpan {
        color: #F7691A;
    }

    .textSpan:hover span {
        color: #ffffff;
    }


</style>