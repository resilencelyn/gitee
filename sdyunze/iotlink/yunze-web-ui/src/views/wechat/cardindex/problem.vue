<template>


</template>

<script>
    import headerNav from "../../../components/headerNav";
    import footerNav from "../../../components/footerNav";
    import rightBottom from "../../../components/rightBottom";

    export default {
        name: "problem",
        components:{
          "headerNav":headerNav,
          "footerNav":footerNav,
          "rightBottom":rightBottom
        },
        data() {
            return {
                title:'常见问题',
                activeName: ['index'],
                iccid:'',
                openid:'',
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