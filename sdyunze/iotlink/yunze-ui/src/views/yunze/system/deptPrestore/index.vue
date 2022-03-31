<template>
  <div class="app-container">


    <el-main style="padding-top: 2%;padding-left: 3%">

      <div >
        <span>企业预存</span>
        <el-divider content-position="left">企业预存金额支付</el-divider>
      </div>

        <el-row :gutter="50" style="width: 90%">
          <el-form :model="deptForm"   >
            <el-col :sm="6" :lg="6" :xs="24">
              <el-form-item label="企业名称" >
                <el-input disabled v-model="deptForm.deptName"/>
              </el-form-item>
            </el-col>
            <el-col :sm="6" :lg="6" :xs="24">
              <el-form-item label="预存金额" prop="price">
                <el-input @blur="handleInput3" @keydown.native="handleInput3"  v-model="deptForm.deposit"/>
              </el-form-item>
            </el-col>
          </el-form>
        </el-row>

      <div id="qrcode" style="margin: 0 auto"></div>
      <div style="margin: 10px auto">{{this.Rmasge}}</div>
      <el-button type="primary" @click="addPrestore()">确 定</el-button>



    </el-main>

  </div>
</template>

<script>
  import tools from "@/utils/yunze/tools";
  import { queryWxOrder,initiatePreSave } from "@/api/yunze/system/mySysDept";



  export default {
    name: "deptPrestore",
    data() {
      return {
        deptForm:{
          deptName:'',
          deposit:100.00,
          deptId:'',
        },
        timer:null,
        sumTime:0,
        tools:tools,
        Rmasge:'',//返回消息
      };
    },


    created() {
      let str = this.$route.params.Pstr;
      if(str!=null && str!='' && str.length>0){
        var reg_1 = new RegExp( "%2F" , "g" );
        var reg_2 = new RegExp( " " , "g" );

        str = str.replace(reg_1, "/");//转义 /
        str = str.replace(reg_2, "+");//转义 [ ]  》 +
        let jsonobj =  JSON.parse(tools.Decrypt(str));
        if(jsonobj!=null ){
          this.deptForm.deptId = jsonobj.deptId;
          this.deptForm.deptName = jsonobj.deptName;
          if(jsonobj.deposit!=undefined && jsonobj.deposit!=null && jsonobj.deposit!='' && jsonobj.deposit.length>0){
            this.deptForm.deposit = Number(jsonobj.deposit);
          }

        }

      }

    },
    beforeDestroy(){

      //就是这个界面关闭的时候会触发这个，在这里写你需要销毁的数据
      // console.log("就是这个界面关闭的时候会触发这个，在这里写你需要销毁的数据")
      this.closePrestore();
    },
    methods: {

      addPrestore() {
        let _this = this;
        if (tools.VerificationsText(_this, _this.deptForm.deposit, "预存金额不能为空！") == true &&
          tools.VerificationsText(_this, _this.deptForm.deptId, "必要参数缺失请刷新后重试！") == true) {
          let deposit = Number(this.deptForm.deposit);
          this.sumTime = 0;
          if (deposit > 0.0) {
            clearInterval(_this.timer);
            document.getElementById("qrcode").innerHTML = "";
            let Pwd_Str = tools.encrypt(JSON.stringify(this.deptForm));
            initiatePreSave(Pwd_Str).then(response => {
              let jsonobj = JSON.parse(tools.Decrypt(response));
              //console.log(jsonobj);
              if (jsonobj.code == 200) {
                this.$nextTick(function () {
                  _this.$qrcode.createQRCode(document.getElementById('qrcode'), jsonobj.Data.Data)
                })
                _this.timer = setInterval(function () {
                  _this.findOrder({'ord_no': jsonobj.Data.ordNo})
                }, 4000)
              } else {
                this.msgError(jsonobj.msg);
              }
            })
          }
        } else {
          tools.open(_this, "请输入大于 0 的预存金额！！！")
        }
      },


      findOrder(map) {
        let _this = this;
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        queryWxOrder(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          if (_this.sumTime >= 30) {
            this.Rmasge = "支付状态 ：时效已过，请点击确定刷新二维码";
            document.getElementById("qrcode").innerHTML= "";
            clearInterval(_this.timer);
          }else{
            if (jsonobj.code == 200) {
              this.Rmasge = "支付状态 ："+jsonobj.msg;
              this.msgSuccess(jsonobj.msg);
              setTimeout(_this.closePrestore, 3000);
            } else {
              this.Rmasge = "支付状态 ："+jsonobj.msg;
            }
          }
          _this.sumTime++;

        })


      },

      closePrestore(){
        let _this=this;
        clearInterval(_this.timer);
        document.getElementById("qrcode").innerHTML = "";
        this.Rmasge = "";
        _this.deptForm.price = 100.00;
      },

      handleInput3(e) {
        // 通过正则过滤小数点后两位
        e.target.value = (e.target.value.match(/^\d*(\.?\d{0,1})/g)[0]) || null;
      },

    }

  };
</script>
