<template>

</template>

<script>
    export default {
        name: "headerNav",
        props: {
            "iccid": String,
            "openid": String,
            "cardType":String,
             "balance":Number,
             "title":String
        },
        mounted() {
        },
        data(){
          return{

          }
        },
        methods:{
            unbind(){
              let _this=this


              _this.$ajax.post(process.env.VUE_APP_SERVER+"/web/un-bind",{
                  "iccid":_this.iccid,
                  "openid":_this.openid
              }).then(res=>{
                  if(res.data.code==="0"){
                      sessionStorage.clear()
                      _this.$toast.success(res.data.data)
                      _this.$router.push({
                          name:"bind",
                          params:{
                              "openid":_this.openid,
                              "appid":_this.appid
                          }
                      })
                  }else{
                      _this.$toast.fail(res.data.msg)
                  }

              })
            },

        }
    }
</script>

<style scoped>

</style>