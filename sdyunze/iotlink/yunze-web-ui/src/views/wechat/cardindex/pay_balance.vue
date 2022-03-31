<template>

</template>

<script>
    import headerNav from "../../../components/headerNav";
    import footerNav from "../../../components/footerNav";
    import rightBottom from "../../../components/rightBottom";
    import weixinPay from "../../../static/js/weixinPay";
    import tools from "../../../utils/tools";

    export default {
        name: "pay_balance",
        components:{
            "headerNav":headerNav,
            "footerNav":footerNav,
            "rightBottom":rightBottom
        },
        mounted() {


        },
        data(){
            return{


            }
        },
        methods:{

        }
    }
</script>

<style scoped>
    .body{
        width: 100%;
        height: 100%;
        background-image: url("../../../static/img/index.jpg");
        background-size: 100% 100%;
        position: fixed;
    }
     .van-grid-item__content{
        background-color: rgba(227, 214, 223, 0.1);
    }
    .van-grid-item__content:hover{
        border:1px solid #4768f3;
    }

</style>