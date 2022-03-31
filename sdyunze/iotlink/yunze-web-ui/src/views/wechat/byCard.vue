<template>
    <div>
        <div id="Masterimg"></div>
        <van-form >

            <van-field
                    v-model="fieldValue"
                    is-link
                    readonly
                    label="商品"
                    placeholder="请选择商品"
                    @click="show = true"
            />
            <van-popup v-model="show" round position="bottom">
                <van-cascader
                        v-model="cascaderValue"
                        title="请选择商品"
                        :options="options"
                        @close="show = false"
                        @finish="onFinish"
                />
            </van-popup>
            <van-field
                    v-model="from.price"
                    name="单价"
                    label="单价"
                    readonly
            />
            <van-field
                    v-model="from.ByCardFare"
                    name="运费"
                    label="运费"
                    readonly
            />
            <van-field
                    v-model="from.count"
                    name="数量"
                    label="数量"
                    readonly
            />
            <van-stepper v-model="from.count" style="float: right; margin-top: -37px; margin-right: 25%;" theme="round"  min="1" max="999"  button-size="28" disable-input />

            <van-field
                    v-model="from.userName"
                    name="姓名"
                    label="姓名"
                    placeholder="姓名"
                    maxlength="8"
                    :rules="[{ required: true, message: '请填写姓名' }]"
            />


            <van-field
                    label="联系电话"
                    readonly
                    clickable
                    :value="from.userPhone"
                    @touchstart.native.stop="userPhoneShow = true"
            />
            <van-number-keyboard
                    v-model="from.userPhone"
                    :show="userPhoneShow"
                    :maxlength="11"
                    @blur="userPhoneShow = false"
            />



            <van-field
                    readonly
                    name="area"
                    :value="from.address"
                    label="地址"
                    placeholder="点击选择省市区"
                    @click="showArea = true"
            />
            <van-popup v-model="showArea" position="bottom">
                <van-area
                        title="地址选择"
                        :area-list="areaList"
                        @confirm="onConfirm"
                        @cancel="showArea = false"
                />
            </van-popup>
            <van-field
                    v-model="from.DetailedAddress"
                    name="详细地址"
                    label="详细地址"
                    placeholder="详细地址"
                    maxlength="80"
                    :rules="[{ required: true, message: '请填写详细地址' }]"
            />
            <van-field
                    v-model="from.info"
                    name="备注信息"
                    label="备注信息"
                    placeholder="备注信息"
                    maxlength="100"
            />
            <van-field
                    readonly
                    clickable
                    label="代理商"
                    :value="from.depName"
                    placeholder="选择代理商"
                    @click="showPicker  = true"
            />
            <van-popup v-model="showPicker" round position="bottom">
                <van-picker
                        show-toolbar
                        :columns="agentoptionArr"
                        @cancel="showPicker = false"
                        @confirm="onConfirmAgent"
                />
            </van-popup>

            <div style="margin: 16px;">
               <!-- <van-checkbox v-model="checked" shape="square">我已阅读并同意</van-checkbox>-->

                <van-button round block style="color: #000;background-color: #e6a23c;font-size: 30px;" type="info" native-type="submit" @click="sub" >立即付款包邮到家</van-button>
            </div>

        </van-form>



        <div id="imgData"></div>



        <van-dialog v-model="dialogShow" @confirm="Wxpay()"	 title="支付核对中"  :message="Bytitle"  show-cancel-button>

        </van-dialog>

    </div>

</template>


<script>

    import { areaList } from '@vant/area-data';
    import $ from 'jquery'
    import tools from "../../utils/tools";
    import weixinPay from "../../static/js/weixinPay";
    import { Toast } from 'vant';

    export default {
        name: 'byCard',
        components: {
            //Pagination,
        },
        mounted: function () {
            let _this = this;
            _this.from.openid=_this.$route.params.openid;
            if(!tools.Is_null(_this.from.openid)){
                _this.from.openid = sessionStorage.getItem("openid");
            }
            _this.from.appId=_this.$store.state.appid;
            this.loadList();

        },
        data() {
            return {
                dialogShow: false,
                Bytitle:'',
                checked: false,
                userPhoneShow: false,
                show: false,
                fieldValue: '',
                cascaderValue: '',
                // 选项列表，children 代表子选项，支持多级嵌套
                options: [
                    /*{
                        text: '便携式设备',
                        value: '33000000',
                            children: [{ text: 'Ufli', value: '33010000' },{ text: 'Mfli', value: '33020000' }],
                    },
                    {
                        text: '电源式设备',
                        value: '32000000',
                        children: [{ text: '4G路由器', value: '32010000' },{ text: '5G路由器', value: '32020000' }],
                    },*/
                ],
                productArr:[],
                pIdArr:[],
                imgArr:[],
                showPicker:false,
                agentoptionArr: [],
                agentIDArr: [],
                agentArr: [],
                from:{
                    appId:'',
                    ByCardFare:0.0,
                    product_core:'',
                    price:0.0,
                    info:'',
                    depId:'',
                    depName:'',
                    userName:'',
                    userPhone:'',
                    address:'',
                    DetailedAddress:'',
                    count:1,
                    province:'',
                    city:'',
                    district:'',
                    product_id:'',
                    body:"web-Shopping",
                },
                areaList:areaList,
                value: '',
                showArea: false,

                PaymentAmount:0,



            }
        },
        methods: {
            // 全部选项选择完毕后，会触发 finish 事件
            onFinish({ selectedOptions }) {
                this.show = false;
                this.from.product_core = selectedOptions.map((option) => option.value).join('/');
                this.fieldValue = selectedOptions.map((option) => option.text).join('/');
                if(tools.Is_null(this.from.product_core)){
                    let arr = this.from.product_core.split("/");
                    this.from.price = tools.getkeyValue(this.productArr,""+arr[arr.length-1],"key","value");
                    this.from.product_id = tools.getkeyValue(this.pIdArr,""+arr[arr.length-1],"key","value");
                    this.readImage(this.from.product_id);
                }
            },


            onConfirmAgent(value) {
                this.from.depName = value;
                if(tools.Is_null(this.from.depName)) {
                    this.from.depId = tools.getkeyValue(this.agentIDArr, "" + this.from.depName, "key", "value");
                }
                this.showPicker = false;
                //Toast(`当前值：${value} `);
            },

            onConfirm(values) {
                this.from.address = values
                    .filter((item) => !!item)
                    .map((item) => item.name)
                    .join('/');
                this.showArea = false;
            },
            Parameter_verification() {
                let bool = false;
                let _this = this;
                _this.from.province = '';
                _this.from.city = '';
                _this.from.area = '';

                if (tools.VerificationsText_wx(_this, _this.from.price, "请选择商品！") == true &&
                    tools.VerificationsText_wx(_this, _this.from.depId, "请选择代理商！") == true &&
                    tools.VerificationsText_wx(_this, _this.from.userName, "请填写收货人姓名！") == true &&
                    tools.VerificationsText_wx(_this, _this.from.userPhone, "请填写收件人联系电话！") == true &&
                    tools.VerificationsText_wx(_this, _this.from.address, "收货地址不能为空！") == true &&
                    tools.VerificationsText_wx(_this, _this.from.count, "数量不能为空！！") == true &&
                    tools.VerificationsText_wx(_this, _this.from.DetailedAddress, "详细地址不能为空！") == true ) {
                    let addressVal = _this.from.address.split("/");

                    var reg_tel = /^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/;    //11位手机号码正则
                    if (!reg_tel.test(_this.from.userPhone)){
                        Toast("请正确填写您的手机号码！");
                        return false;
                    }
                        //console.log(addressVal);
                    if(addressVal.length==3){
                        _this.from.province = addressVal[0];
                        _this.from.city = addressVal[1];
                        _this.from.district = addressVal[2];
                    }else{
                        Toast("请按提示选择收货地址！");
                        return false;
                    }
                    bool = true;
                }
                return bool;
            },
            sub(){
                if(this.Parameter_verification()){
                    this.dialogShow = true;
                    let Yf = this.from.ByCardFare > 0?" + 运费 ["+this.from.ByCardFare+"]":"";
                    let paySum = tools.NumberAdd(tools.accMul(this.from.price,this.from.count),this.from.ByCardFare);
                    this.Bytitle = "["+this.fieldValue+"]  \n 单价 ["+this.from.price+"] * 数量 ["+this.from.count+"] "+Yf+"  \n = 待支付金额 ["+paySum+"]";
                }
                console.log(this.from)
            },


            Wxpay() {
                let _this = this;
                //后台核对商品价格 运费
                    let Pwd_Str = tools.encrypt(JSON.stringify(_this.from));
                    _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/pay/ShoppingPay", Pwd_Str).then(response => {
                        let jsonobj_1 = JSON.parse(tools.Decrypt(response.data));
                        if (jsonobj_1.code == "200") {
                            weixinPay(jsonobj_1.Data).then(res => {
                                //_this.$toast.success("success")
                            }).catch(err => {
                                //console.log(err)
                                _this.$toast.fail("error" + err)
                            })
                        } else {
                            _this.$toast.fail(jsonobj_1.msg);
                            // return false
                        }
                    })
            },




            loadList()
            {
                let _this = this;
                let map = {};
                map.openid = _this.from.openid;
                if ( _this.from.openid != null && _this.from.openid != '') {
                    let Pwd_Str = tools.encrypt(JSON.stringify(map));
                    _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/shopping/cy-category", Pwd_Str).then((res) => {
                        let jsonobj = JSON.parse(tools.Decrypt(res.data));
                         //console.log(jsonobj);
                        _this.show = false;
                        if (jsonobj.code == "200") {
                            this.options = jsonobj.Data.Data;

                            for (let i = 0; i < this.options.length ; i++) {
                                for (let j = 0; j < this.options[i].children.length; j++) {
                                    let Pobj = this.options[i].children[j];
                                    this.productArr.push({"key":Pobj.value,"value":Pobj.price});
                                    this.pIdArr.push({"key":Pobj.value,"value":Pobj.product_id});
                                }
                            }
                            this.from.ByCardFare = jsonobj.Data.ByCardFare;
                            this.fieldValue = jsonobj.Data.fieldValue;
                            this.from.price = jsonobj.Data.price;
                            this.from.product_id = jsonobj.Data.product_id;


                            this.imgArr = jsonobj.Data.imgData;
                            let imgArr = this.imgArr;
                            this.agentArr = jsonobj.Data.agentArr;
                            for (let i = 0; i < this.agentArr.length ; i++) {
                                let angent = this.agentArr[i];
                                this.agentoptionArr.push(angent.dept_name);
                                this.agentIDArr.push({"key":angent.dept_name,"value":angent.dept_id});

                            }
                            let outHtml = '';
                            let MasterOutHtml = '';
                            for (let i = 0; i < imgArr.length; i++) {
                                let obj = imgArr[i];
                                let html = "<img  style='width: 100%;' src='"+process.env.VUE_APP_SERVER+"/web/weixin/shopping/cy-readImage?map="+tools.encrypt(JSON.stringify({"openid":this.from.openid,"imagePath":""+obj.pic_url+""}))+"' / >";
                                if(obj.is_master==1){
                                    MasterOutHtml+=html;
                                }else{
                                    outHtml+=html;
                                }
                            }
                            $('#Masterimg').html(MasterOutHtml);
                            $('#imgData').html(outHtml);
                        } else {
                            _this.$toast.fail(jsonobj.msg)
                        }
                    })
                } else {
                    Toast(`未获取到openId！`);
                }
            },


            readImage(product_id){
                let _this = this;
                let map = {};
                map.openid = _this.from.openid;
                map.product_id = product_id;
                //console.log(map);
                if ( map.openid != null && map.openid != '' && map.product_id != null && map.product_id != '' ) {
                    let Pwd_Str = tools.encrypt(JSON.stringify(map));
                    _this.$ajax.post(process.env.VUE_APP_SERVER + "/web/weixin/shopping/cy-img", Pwd_Str).then((res) => {
                        let jsonobj = JSON.parse(tools.Decrypt(res.data));
                        //console.log(jsonobj);
                        _this.show = false;
                        if (jsonobj.code == "200") {
                            this.imgArr = jsonobj.Data.Data;
                            let imgArr = this.imgArr;
                            let outHtml = '';
                            let MasterOutHtml = '';
                            for (let i = 0; i < imgArr.length; i++) {
                                let obj = imgArr[i];
                                let html = "<img  style='width: 100%;' src='"+process.env.VUE_APP_SERVER+"/web/weixin/shopping/cy-readImage?map="+tools.encrypt(JSON.stringify({"openid":this.from.openid,"imagePath":""+obj.pic_url+""}))+"' / >";
                                if(obj.is_master==1){
                                    MasterOutHtml+=html;
                                }else{
                                    outHtml+=html;
                                }
                            }
                            $('#Masterimg').html(MasterOutHtml);
                            $('#imgData').html(outHtml);
                        } else {
                            _this.$toast.fail(jsonobj.msg)
                        }
                    })
                }
            },


        }
    }
</script>

<style>

</style>

