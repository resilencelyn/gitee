<template>

</template>


<script>

    import tools from "../../utils/tools";
    import City from "../../static/js/citySet";
    import weixinPay from "../../static/js/weixinPay";

    export default {
        name: 'cardReplacement',
        components: {
            //Pagination,
        },
        mounted: function () {


        },
        data() {
            return {


                PaymentAmount:0,
                card_replace: {
                    //微信支付参数
                    body:"change-Card",
                    price:0.01,
                    openid:"",
                    active:false,
                    appId:'wxb9c885c722d8cd36',

                    agent_id:'',
                    consignee: '',
                    express_price:0,
                    tel: '',
                    card_no: "",
                    province: '',
                    city: '',
                    area: '',
                    address: '',
                    new_package_id: '',
                    cre_time: '',
                    city: '',
                    express: '',
                    pwd: '',
                    replace_price: 0,
                    is_web: '否',
                    addressVal:'',
                    id:'',
                },
                is_status: 0,
                express_optino: [{"label": "申通", "value": "申通"}, {"label": "邮政", "value": "邮政"}, {
                    "label": "顺丰",
                    "value": "顺丰"
                }],
                package_option: [

                ],

            }
        },
        methods: {



        }
    }
</script>

