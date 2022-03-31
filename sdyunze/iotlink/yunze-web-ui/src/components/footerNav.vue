<template>

</template>

<script>
    export default {
        name: "footerNav",
        props:{
            iccid:String
        },
        methods:{
            goIndex(){
                this.$router.push({
                    name:"index",
                    params:{
                        "iccid":this.iccid
                    }
                })
            },

        }
    }
</script>

<style scoped>

</style>