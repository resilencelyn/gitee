<template>
    <div style="height: 100vh">
        <el-scrollbar style="height: 100%">
            <div style="width: 100%;height: 100%">
                <van-tabs @click="clickTab">
                    <van-tab title="生效中" name="1"/>
                    <van-tab title="已失效" name="0"/>
                    <van-tab title="未生效" name="2"/>
                </van-tabs>




                <van-empty v-if="emptyImage" :description="title"/>


                <van-collapse v-model="activeNames"
                              v-for="(item,index) in orders"
                              :key="index"
                >
                    <van-collapse-item :name="index" >
                        <template #title>
                            <div>
                                <van-icon name="orders-o"/>
                                {{tools.getkeyValue(PackageNames,item.packet_id,"packet_id","packet_wx_name")+" | "+item.true_flow+" MB"}}
                            </div>
                        </template>

                        <van-cell-group>
                            <van-field v-model="item.start_time" label="生效时间："/>
                            <van-field v-model="item.end_time" label="到期时间："/>
                            <van-field v-model="item.create_time" label="创建时间："/>
                            <van-field v-model='item.validate_type' label="生效类型：" ></van-field>
                        </van-cell-group>
                    </van-collapse-item>

                </van-collapse>

                <div v-if="showMax" style="text-align: center;color: #F7691A;text-align: center;margin-top: 10px;">仅显示近期10条到期订购记录</div>
            </div>
            <div style="height: 5rem"></div>
        </el-scrollbar>
    </div>
</template>

<script>
    import tools from "../../../utils/tools";

    export default {
        name: "card_index",
        components: {
        },
        data() {
            return {
                orders: [],
                activeNames: ['1'],
                emptyImage: false,
                showMax: false,
                status: 1,
                iccid: "",
                openid: '',
                customize_whether: [{"dict_value": "1", "dict_label": "是"}, {"dict_value": "0", "dict_label": "否"}],
                PackageNames: [],
                cardTakeEffectTypeOption: [],//资费计划生效类型
                payTypeOption: [],//支付方式

                title: '您没有 [待生效] 的资费计划。',
                tools: tools,
                from: {
                    ord_no: '',
                    iccid: '',
                    pay_type: '',
                    create_time: '',
                    validate_type: '',
                    status: '',
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
            //console.log(this.cardTakeEffectTypeOption)
            //console.log(this.payTypeOption)
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
                    this.cardTakeEffectTypeOption = window['yunze_card_takeEffect_type'];
                    //延迟赋值 【解决动态编辑赋值的bug】
                    setTimeout(function () {
                        this.cardTakeEffectTypeOption = window['yunze_card_takeEffect_type'];
                    }, 1000);
                }
            },


            clickTab(title, name) {
                let _this = this;
                _this.emptyImage = false;
                _this.showMax = false;
                _this.status = parseInt(title);
                _this.title = '您没有 [' + name + '] 的资费计划。';
                _this.list();
            },
            //查看详情
            OrdercollapseChange(id) {

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
                map.status = _this.status;
                map.iccid = _this.iccid;
                map.openid = _this.openid;
                let Pwd_Str = tools.encrypt(JSON.stringify(map));
                _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/pac/getOrderFlow", Pwd_Str).then((res) => {
                    let jsonobj = JSON.parse(tools.Decrypt(res.data));
                    //console.log(jsonobj);
                    if (jsonobj.code == "200") {
                        let arr = jsonobj.Data.Data;
                        let FArr = [];
                        for (let i = 0; i < arr.length; i++) {
                            let Obj = arr[i];
                            Obj.validate_type = tools.getkeyValue(this.cardTakeEffectTypeOption,""+Obj.validate_type,"dict_value", "dict_label");
                            FArr.push(Obj);
                        }
                        if (FArr.length == 0) {
                            _this.emptyImage = true;
                        } else {
                            _this.emptyImage = false;
                            if (FArr.length == 10 && _this.status == '0') {
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