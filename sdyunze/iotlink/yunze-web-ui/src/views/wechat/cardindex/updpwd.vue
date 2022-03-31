<template>

</template>

<script>
    import headerNav from "../../../components/headerNav";
    import footerNav from "../../../components/footerNav";
    import rightBottom from "../../../components/rightBottom";
    /*import Vue from 'vue';
    import { Form } from 'vant';*/
    export default {
        name: "updpwd",
        components:{
            "headerNav":headerNav,
            "footerNav":footerNav,
            "rightBottom":rightBottom
        },
        data() {
            return {
                title:'修改密码',
                activeName: ['index'],
                iccid:'',
                openid:'',
                Ordpassword: '',
                password: '',
                Newpassword:'',
                contents:{},
            };
        },
        mounted() {


        },
        methods:{



        }
    }
</script>

<style scoped>

</style>