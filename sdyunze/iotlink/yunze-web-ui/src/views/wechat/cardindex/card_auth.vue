<template>

</template>

<script>
    import headerNav from "../../../components/headerNav";
    import footerNav from "../../../components/footerNav";
    import rightBottom from "../../../components/rightBottom";


    export default {
        name: "card_auth",
        components:{
            "headerNav":headerNav,
            "footerNav":footerNav,
            "rightBottom":rightBottom
        },
        mounted() {

        },
        data() {
            return {
                title:'实名认证',
                realName: '',
                idCardNo: '',
                mobile:"",
                iccid:"",
                balance:0.00,
                openid:''

            };
        },
        methods: {

        },
    }
</script>

<style scoped>

</style>