<template>
  <div class="app-container">
    <div>
      <el-main style="padding-top: 2%;padding-left: 3%">
        <el-row >
          <el-col :span="12">
            <el-form  ref="queryName"   label-width="120px">
              <el-form-item  prop="status">
                <h4> 批量取消实名
                  <el-tooltip placement="bottom">
                    <div slot="content">
                      下载模板，上传需要查询的 iccid
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                </h4>
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
import { getToken } from "@/utils/auth";
import tools from "@/utils/yunze/tools";

export default {
  name: "realname",
  data() {
    return {
      //保存 编辑 按钮
      selImei: false,
      iccids:[],
      validateTypeOptions:[],//生效类型
      findCr_RouteOptions: [],// API通道信息

      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,

      customize_whether: [],//自定义是否
      ExecutionTask_OutType : [],// 执行任务导出类别

      SetMealImport:false, //详情查询 套餐信息
      show_ds:false, //详情查询
      selTime:'',//时间选择
      mainwidth:24,//宽度
      option_show:false, //公司所属查询
      // 遮罩层
      loading: true,
      // 导出遮罩层
      exportLoading: false,
      // 选中数组
      ids: [],
      // 非单个禁用
      single: true,
      // 非多个禁用
      multiple: true,
      // 显示搜索条件
      showSearch: true,
      // 总条数
      total: 0,
      // 用户表格数据
      cardList: null,
      // 弹出层标题
      title: "",
      // 部门树选项
      deptOptions: undefined,
      // 是否显示弹出层
      open: false,
      // 部门名称
      deptName: undefined,
      // 默认密码
      initPassword: undefined,
      // 日期范围
      dateRange: [],
      // 状态数据字典
      typeOptions: [],
      // 连接管理时间查询类型 数据字典
      timetypeOptions: [],
      // 简要套餐名称
      packageOptions: [],
      PacketOptions: [],//资费计划
      PacketSelOptions:[],//资费计划查询
      submitFileFormBtn:true,//导入 btn

      //详情
      tablefrom: [],

      FlowData : [],
      selIccid:[{FlowData:''}],
      activeNames:[0],

      //卡状态
      status_type: "",
      // 卡状态 字典
      stateOptions: [
        {
          dictLabel:''
        }
      ],
      packet:{
        packet_flow:'-',
        packet_valid_time:'-',
        packet_valid_name:'-'
      },

      // 运营商类别 字典
      operators_type: [],
      // 执行任务状态 字典
      channelStatusOptions: [],
      // 执行任务编码 字典
      channelCodeOptions: [],
      // 表单参数
      form: {},
      defaultProps: {
        children: "children",
        label: "label"
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
        url: process.env.VUE_APP_BASE_API + "/yunze/card/cancelrealname",
        Pstr:'',//携带参数
      },





      // 列信息
      columns: [
        { key: 0, label: `任务名`, visible: true },
        { key: 1, label: `创建时间`, visible: true },
        { key: 2, label: `修改时间`, visible: true },
        { key: 3, label: `开始时间`, visible: true },
        { key: 4, label: `结束时间`, visible: true },
        { key: 5, label: `创建人`, visible: true },
      ],
      // 表单校验
      rules: {
      }




    };
  },
  watch: {
    // 根据名称筛选部门树
    deptName(val) {
      //console.log(val);
      this.$refs.dept.filter(val);

    }
  },

  created() {






  },
  methods: {


    handleInput(e) {
      e.target.value=e.target.value.replace(/[^\d]/g,'');
    },

    SelectChange(){
      this.getPwd_Str();
    },
    // 加密数据
    getPwd_Str(){
      let map ={};
      this.upload.Pstr = Pwd_Str;
    },


    /** 下载模板操作 */
    importTemplate() {
      let map = {};
      map.path = "/getcsv/ImportRecharge.cvs";
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
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
        tools.openAsk(this,'warning', "是否确认 批量取消实名？", this.uploadUp,this, this.FalseFun,null);
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

<style>

.el-table .cell{
  line-height: 17px;
  padding-left: 10px;
  padding-right: 0px;
}

.el-form-item {
  margin-bottom: 10px;
}

.my_checkbox__inner{
  border: 1px;
}

</style>

