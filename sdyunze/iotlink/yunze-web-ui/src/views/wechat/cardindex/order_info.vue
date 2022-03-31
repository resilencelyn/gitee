<template>
    <div style="height: 100vh">
        <footerNav :iccid="iccid"></footerNav>
        <el-scrollbar style="height: 100%">
            <div style="width: 100%;height: 100%">
                <van-tabs @click="clickTab">
                    <van-tab title="待生效" name="0"/>
                    <van-tab title="已完成" name="1"/>
                </van-tabs>

                <van-empty v-if="emptyImage" :description="title"/>


                <van-collapse v-model="activeNames"
                              v-for="(item,index) in orders"
                              :key="index"
                              @change="OrdercollapseChange(item.id)"
                >
                    <van-collapse-item :name="index" v-if="item.ord_type=='1'">
                        <template #title>
                            <div>
                                <van-icon name="orders-o"/>
                                {{item.ord_name+" | " +item.price+" 元"}}
                            </div>
                        </template>
                        <van-cell-group>
                            <van-field v-model="item.ord_no" label="订单号："/>
                            <van-field v-model="item.iccid" label="ICCID："/>
                            <van-field v-model="item.pay_type" label="支付方式："/>
                            <van-field v-model="item.create_time" label="充值时间："/>
                        </van-cell-group>
                    </van-collapse-item>
                    <van-collapse-item :name="index" v-if="item.ord_type=='2'">
                        <template #title>
                            <div>
                                <van-icon name="orders-o"/>
                                {{tools.getkeyValue(PackageNames,item.packet_id,"packet_id","packet_wx_name")+" | "+item.packet_valid_time+"*"+item.packet_valid_name+" | "+item.price+" 元"}}
                            </div>
                        </template>
                        <van-cell-group>
                            <van-field v-model="item.ord_no" label="订单号："/>
                            <van-field v-model="item.iccid" label="ICCID："/>
                            <van-field v-model="item.pay_type" label="支付方式："/>
                            <van-field v-model="item.create_time" label="充值时间："/>
                            <van-field v-model="item.packet_flow" label="规格 MB："/>
                            <van-field v-model="item.validate_type" label="生效类型："/>
                            <van-field v-model="item.add_package" label="是否到账："/>
                        </van-cell-group>
                    </van-collapse-item>

                </van-collapse>

                <div v-if="showMax" style="text-align: center;color: #F7691A;text-align: center;margin-top: 10px;">仅显示近期10笔交易记录</div>
            </div>
            <div style="height: 5rem"></div>
        </el-scrollbar>
        <rightBottom v-bind:iccid="iccid" v-bind:openid="openid"></rightBottom>
    </div>
</template>

<script>
    import footerNav from "../../../components/footerNav";
    import rightBottom from "../../../components/rightBottom";
    import tools from "../../../utils/tools";

    export default {
        name: "order_info",
        components: {
            "footerNav": footerNav,
            "rightBottom": rightBottom
        },
        data() {
            return {
                orders: [],
                activeNames: ['1'],
                emptyImage: false,
                showMax: false,
                add_package: 0,
                iccid: "",
                openid: '',
                customize_whether: [{"dict_value": "1", "dict_label": "是"}, {"dict_value": "0", "dict_label": "否"}],
                PackageNames: [],
                cardTakeEffectTypeOption: [],//资费计划生效类型
                payTypeOption: [],//支付方式

                title: '您没有 [待生效] 的订单。',
                tools: tools,
                from: {
                    ord_no: '',
                    iccid: '',
                    pay_type: '',
                    create_time: '',
                    validate_type: '',
                    add_package: '',
                },
            };
        },
        created() {
            let _this = this;
            _this.iccid = _this.$route.params.iccid;
            if (_this.$check.checkIsNull(_this.iccid)) {
                _this.iccid = _this.$store.state.iccid;
            }
            _this.openid = _this.$route.params.openid;
            if (_this.$check.checkIsNull(_this.openid)) {
                _this.openid = _this.$store.state.openid;
            }
            //加载 微信订单获取资费名称
            if (window['PackageNames'] != undefined && window['PackageNames'] != null && window['PackageNames'] != '') {
                this.PackageNames = window['PackageNames'];
            } else {
                let _this = this;
                let map = {};
                map.iccid = _this.iccid;
                map.openid = _this.openid;
                let Pwd_Str = tools.encrypt(JSON.stringify(map));
                _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/ord/queryPackage", Pwd_Str).then((res) => {
                    //console.log(res);
                    let jsonobj = JSON.parse(tools.Decrypt(res.data));
                    if (jsonobj.code == "200") {
                        window['PackageNames'] = jsonobj.Data.Data;
                        this.PackageNames = window['PackageNames'];
                    } else {
                        _this.$toast.fail(jsonobj.msg)
                    }
                })
            }
            this.getDict();
            this.list();
        },
        mounted() {


        },
        methods: {


            getDict() {
                //获取资费计划生效类型
                if (window['yunze_card_takeEffect_type'] != undefined && window['yunze_card_takeEffect_type'] != null && window['yunze_card_takeEffect_type'] != '') {
                    this.cardTakeEffectTypeOption = window['yunze_card_takeEffect_type'];
                } else {
                    this.getDictionary("yunze_card_takeEffect_type");
                    //延迟赋值 【解决动态编辑赋值的bug】
                    setTimeout(function () {
                        this.cardTakeEffectTypeOption = window['yunze_card_takeEffect_type'];
                    }, 1000);
                }
                //获取 支付方式
                if (window['yunze_card_pay_type'] != undefined && window['yunze_card_pay_type'] != null && window['yunze_card_pay_type'] != '') {
                    this.payTypeOption = window['yunze_card_pay_type'];
                } else {
                    this.getDictionary("yunze_card_pay_type");
                    //延迟赋值 【解决动态编辑赋值的bug】
                    setTimeout(function () {
                        this.payTypeOption = window['yunze_card_pay_type'];
                    }, 1000);
                }
            },


            clickTab(title, name) {
                let _this = this;
                _this.emptyImage = false;
                _this.showMax = false;
                _this.add_package = parseInt(title);
                _this.title = '您没有 [' + name + '] 的订单。';
                _this.list();
            },
            //查看订单详情
            OrdercollapseChange(id) {

            },

            WxfindOrderDs(id) {
                let _this = this;
                let map = {};
                map.id = id;
                map.iccid = _this.iccid;
                map.openid = _this.openid;
                let Pwd_Str = tools.encrypt(JSON.stringify(map));
                _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/ord/WxfindOrderDs", Pwd_Str).then((res) => {
                    let jsonobj = JSON.parse(tools.Decrypt(res.data));
                    if (jsonobj.code == "200") {
                        let obj = jsonobj.Data.Data;
                        this.getDict();
                        obj.pay_type = tools.getkeyValue(this.payTypeOption, "" + obj.pay_type, "dict_value", "dict_label");
                        obj.validate_type = tools.getkeyValue(this.cardTakeEffectTypeOption, "" + obj.validate_type, "dict_value", "dict_label");
                        obj.add_package = tools.getkeyValue(this.customize_whether, "" + obj.add_package, "dict_value", "dict_label");
                        //console.log(obj)
                        _this.from = obj;
                    } else {
                        _this.$toast.fail(jsonobj.msg)
                    }
                })
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


            list() {
                let _this = this;
                let map = {};
                map.add_package = _this.add_package;
                map.iccid = _this.iccid;
                map.openid = _this.openid;
                let Pwd_Str = tools.encrypt(JSON.stringify(map));
                _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/ord/qu-ord", Pwd_Str).then((res) => {
                    let jsonobj = JSON.parse(tools.Decrypt(res.data));
                    //console.log(jsonobj);
                    this.getDict();
                    if (jsonobj.code == "200") {
                        let arr = jsonobj.Data.Data;
                        let FArr = [];
                        for (let i = 0; i < arr.length; i++) {
                            let obj = arr[i];
                            obj.pay_type = tools.getkeyValue(this.payTypeOption, "" + obj.pay_type, "dict_value", "dict_label");
                            obj.validate_type = tools.getkeyValue(this.cardTakeEffectTypeOption, "" + obj.validate_type, "dict_value", "dict_label");
                            obj.add_package = tools.getkeyValue(this.customize_whether, "" + obj.add_package, "dict_value", "dict_label");
                            FArr.push(obj);
                        }
                        if (FArr.length == 0) {
                            _this.emptyImage = true;
                        } else {
                            _this.emptyImage = false;
                            if (FArr.length == 10) {
                                _this.showMax = true;
                            }
                        }
                        _this.orders = FArr;
                    } else {
                        _this.$toast.fail(jsonobj.msg)
                    }
                })

            }

        },
    }
</script>

<style scoped>
    .el-scrollbar__wrap {
        overflow-x: hidden;
    }

    .van-card__content my-content {
        margin-left: -10%;
    }

    .van-cell {
        padding: 4px 16px;
    }

    .van-divider {
        margin: 6px 0;
    }
</style>