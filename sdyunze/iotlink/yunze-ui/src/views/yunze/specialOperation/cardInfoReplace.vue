<template>
  <div class="app-container">

    <div>
        <el-main style="padding-top: 2%;padding-left: 3%">
        <el-divider content-position="left">卡信息批量更新</el-divider>

          <el-row >
            <el-col :span="12">
              <el-form  ref="queryForm"   label-width="120px">
                <el-form-item prop="status">
                  <span slot="label">
                        修改依据字段
                  </span>
                  <el-tooltip placement="right">
                    <div slot="content">
                      如：修改依据字段 选择【ICCID】那么 ICCID 为必填！且根据匹对的ICCID 进行修改
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                  <el-select
                    v-model="updFome.myUpdType"
                    placeholder="请选择 修改依据字段"
                    clearable
                    size="small"
                    @change="SelectChange()"
                    style="width: 200px"
                  >
                    <el-option
                      v-for="dict in cardNumber"
                      :key="dict.dictValue"
                      :label="dict.dictLabel"
                      :value="dict.dictValue"
                    />
                  </el-select>
                </el-form-item>

                <el-form-item>
                  <el-upload
                    ref="upload"
                    :limit="1"
                    accept=".xlsx, .xls"
                    :headers="upload.headers"
                    :action="upload.url"
                    :disabled="upload.isUploading"
                    :on-progress="handleFileUploadProgress"
                    :on-success="handleFileSuccess"
                    :auto-upload="false"
                    :data="{Pstr:this.upload.Pstr}"
                    drag
                  >
                    <i class="el-icon-upload"></i>
                    <div class="el-upload__text">
                      将文件拖到此处，或
                      <em>点击上传</em>
                    </div>
                    <div class="el-upload__tip" slot="tip">
                      <el-link type="info" style="font-size:12px" @click="importTemplate">下载模板</el-link>
                    </div>
                    <div class="el-upload__tip" style="color:red" slot="tip">提示：仅允许导入“xls”或“xlsx”格式文件！</div>
                  </el-upload>
                </el-form-item>
                <el-form-item>
                  <el-button type="primary" v-if="submitFileFormBtn" @click="submitFileForm">确 定</el-button>
                </el-form-item>
              </el-form>
            </el-col>
          </el-row>
        </el-main>
      </div>

  </div>
</template>

<script>
import { queryPackageSimple,findCr} from "@/api/yunze/flowcard/card/card";
import { getToken } from "@/utils/auth";
import tools from "@/utils/yunze/tools";



export default {
  name: "cardInfoReplace",
  data() {
    return {


      submitFileFormBtn:true,//导入 btn
      cardNumber:[],
      updFome:{
        myUpdType:'',//修改依据
      },

      // 导入参数
      upload: {
        // 是否显示弹出层（用户导入）
        open: false,
        // 弹出层标题（用户导入）
        title: "",
        // 是否禁用上传
        isUploading: false,
        // 是否更新已经存在的用户数据
        updateSupport: 0,
        // 设置上传的请求头部
        headers: { Authorization: "Bearer " + getToken() },
        // 上传的地址
        url: process.env.VUE_APP_BASE_API + "/yunze/card/importCardReplace",
        Pstr:'',//携带参数
      },










    };
  },


  created() {


    //加载 卡号类别
    if(window['cardNumber']!=undefined &&  window['cardNumber']!=null && window['cardNumber']!=''){
      this.cardNumber = window['cardNumber'];
    }else{
      this.getDicts("cardNumber").then(response => {
        window['cardNumber'] = response.data;
        this.cardNumber = window['cardNumber'];
      });
    }



  },
  methods: {






    SelectChange(){
      this.getPwd_Str();
    },
    // 加密数据
    getPwd_Str(){
      let Pwd_Str = tools.encrypt(JSON.stringify(this.updFome));
      this.upload.Pstr = Pwd_Str;
    },


    /** 下载模板操作 */
    importTemplate() {
      let map = {};
      map.path = "/getcsv/bulkUpdateCard.cvs";
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      window.open(process.env.VUE_APP_BASE_API+"/yunze/ExecutionTask/downloadConversion?Pstr="+Pwd_Str, '_blank');
    },




    // 文件上传中处理
    handleFileUploadProgress(event, file, fileList) {
      this.upload.isUploading = true;
    },
    // 文件上传成功处理
    handleFileSuccess(response, file, fileList) {
      this.upload.open = false;
      this.upload.isUploading = false;
      this.$refs.upload.clearFiles();
      this.$alert(response.msg, "导入结果", { dangerouslyUseHTMLString: true });
      this.submitFileFormBtn = true;
    },
    // 提交上传文件
    submitFileForm() {
      var files = this.$refs.upload.uploadFiles;//判断files数组的长度是否大于0，不大于0 则未选择附件
      if(files.length>0){
        if(tools.VerificationsText(this, this.updFome.myUpdType, "请选择 修改依据字段！") == true ){
          tools.openAsk(this,'warning', "是否确认 批量更新卡信息？", this.uploadUp,this, this.FalseFun,null);
        }
      }else{
        tools.open(this,"请选择上传文件！");
      }
    },

    titleUp(_this){

     },

    uploadUp(_this){
      _this.$refs.upload.submit();
    },

    FalseFun(Pwd_Str){
      this.submitFileFormBtn = true;
    },





    /*获取查询参数*/
    getParams(){

    },






    /*获取字典数据中的匹对key value*/
    getDeptNamekeyValue(arr,key){
      let value = '';
      key = ""+key+"";
      for (let i = 0; i < arr.length; i++) {
        let map =  arr[i];
        if(map.dept_id==key){
          value = map.dept_name;
          break;
        }
      }
      return value;
    },




  }
};
</script>

