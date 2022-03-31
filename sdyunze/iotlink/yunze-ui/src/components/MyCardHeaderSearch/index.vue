<template>
  <div style="width: 260px;">
    <el-input style="width: 100%;border-radius: 30px;" v-model="cardNo" @keyup.enter.native="selCard"
              placeholder="MSISDN/ICCID/IMSI">
      <el-button  slot="append" icon="el-icon-search" @click="selCard"></el-button>
    </el-input>
  </div>
</template>

<script>

import tools from "@/utils/yunze/tools";

export default {
  name: 'MyCardHeaderSearch',
  data() {
    return {
      cardNo: '',
    }
  },
  computed: {
    routes() {
      return this.$store.getters.permission_routes
    }
  },

  mounted() {

  },
  methods: {
    selCard(){
     let _this = this;
      if(_this.cardNo!=null && _this.cardNo.length>0){
        this.$router.push('/card/card/'+tools.encrypt(JSON.stringify({'type':'1','value':_this.cardNo})));
      }else{
        this.msgError('内容不能为空，请输入正确的MSISDN/ICCID');
      }
    },
  }
}
</script>

<style scoped>
  .el-input-group--append .el-input__inner{
    padding: 0px 5px;
  }
  .el-input-group__append, .el-input-group__prepend{
    width: 25px;
    padding: 0px 0px;
  }


</style>

