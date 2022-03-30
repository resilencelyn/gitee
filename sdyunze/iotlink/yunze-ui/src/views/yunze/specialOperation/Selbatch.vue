<template>
  <div class="app-container">

    <div>
      <el-main style="padding-top: 2%;padding-left: 3%">
        <el-row >
          <el-col :span="12">
            <el-form :model="queryParams" ref="queryForm"   label-width="120px">

              <el-form-item  prop="status">
                <h4> 批量停复机、断开网 功能
                  <el-tooltip placement="bottom">
                    <div slot="content">
                      1.先下载模板 2.选中你要停复机或者断开网的号码！进行以下操作~~~
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                </h4>
              </el-form-item>

              <el-form-item  >
              <span slot="label">
                停复机操作
                <el-tooltip placement="right">
                  <div slot="content">
                    选择 是否 停机 复机 默认选中未操作
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
                <template>
                  <el-radio-group>
                    <label class="el-radio " v-for="(item,index) in customize_batch">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="queryParams.Is_remind_ratio"
                                           name="IS_STOP"
                                           @click="SelectChange()"
                                           :value="item.dictValue"/>
                                     </span>
                      <span class="el-radio__label ">{{item.dictLabel}}</span>
                    </label>
                  </el-radio-group>
                </template>
              </el-form-item>

              <el-form-item  >
              <span slot="label">
                断开网操作
                <el-tooltip placement="right">
                  <div slot="content">
                    选择 是否 断网 开网 默认选中未操作
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
                <template>
                  <el-radio-group>
                    <label class="el-radio " v-for="(item,index) in Switch_network">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="queryParams.Switch_network"
                                           name="in_stock"
                                           @click="SelectChange()"
                                           :value="item.dictValue"/>
                                     </span>
                      <span class="el-radio__label ">{{item.dictLabel}}</span>
                    </label>
                  </el-radio-group>
                </template>
              </el-form-item>

              <!--              停复机操作   Is_remind_ratio-->
              <!--              断开网操作   Switch_network-->
              <!--           【是否停复机】   on 开机 off 停机   停复机操作   1 不操作 2 停机  3 复机  Is_remind_ratio-->
              <!--           【断网复机】     是否   0 开机 1 停机  段开网操作   1 不操作 2 断网  3 开网  Switch_network-->

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
  name: "Selbatch",
  data() {
    return {
      //保存 编辑 按钮
      selImei: false,
      iccids:[],
      validateTypeOptions:[],//生效类型
      findCr_RouteOptions: [],// API通道信息
      customize_batch: [],//自定义是否

      Switch_network:[],



      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,

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
        url: process.env.VUE_APP_BASE_API + "/yunze/card/status",
        Pstr:'',//携带参数
      },



      // 查询参数
      queryParams: {
        package_id:null,
        Switch_network: "1",
        Is_remind_ratio:'1',
        channel_id:null,
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

    //加载 自定义 是否 1 2 3 不操作 停机 复机
    if(window['customize_batch']!=undefined &&  window['customize_batch']!=null && window['customize_batch']!=''){
      this.customize_batch = window['customize_batch'];
    }else{
      this.getDicts("Stop_and_resume").then(response => {
        window['customize_batch'] = response.data;
        this.customize_batch = window['customize_batch'];
      });
    }

    //加载 自定义 是否 0 1 否 是
    if(window['Switch_network']!=undefined &&  window['Switch_network']!=null && window['Switch_network']!=''){
      this.Switch_network = window['Switch_network'];
    }else{
      this.getDicts("Disconnect_the_network").then(response => {
        window['Switch_network'] = response.data;
        this.Switch_network = window['Switch_network'];
      });
    }

  },
  methods: {


    handleInput(e) {
      e.target.value=e.target.value.replace(/[^\d]/g,'');
    },




    SelectChange(){
      this.getPwd_Str();
    },
    // 127.0.0.1
    getPwd_Str(){
      let map ={};
      // map.package_id = this.queryParams.package_id;
      // map.remind_ratio = this.queryParams.remind_ratio;
      map.Switch_network= this.queryParams.Switch_network;
      map.Is_remind_ratio = this.queryParams.Is_remind_ratio;
      // map.channel_id = this.queryParams.channel_id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
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

    /*
    * 特殊操作查询IMEI 指令 已发送，连接设置详细信息请在 【执行任务管理】查询！
    * */


    // 提交上传文件
    submitFileForm() {
      let _this = this;
      this.SelectChange();
      // console.log(this.queryParams)
      if (tools.Is_null(this.queryParams.Is_remind_ratio) == true &&
        tools.Is_null(this.queryParams.Switch_network) == true
      ) {
        if(this.queryParams.Is_remind_ratio!="1" && this.queryParams.Switch_network!="1" ){
          tools.open(this, "以上不允许操作~请单选")

        }else if(this.queryParams.Is_remind_ratio!="1" || this.queryParams.Switch_network!="1" ){
          this.titleUp(_this);
          this.submitFileFormBtn = true;
          this.submitFileFormBtn = false;
        }else{
          tools.open(this, "请选择需要设置操作的属性条件！")
        }
      }
    },

    titleUp(_this){
      let msg = '批量 变更';


      if(tools.Is_null( _this.queryParams.Is_remind_ratio)){
        msg += ' 卡状态为：【'+tools.getkeyValue(customize_batch,_this.queryParams.Is_remind_ratio,"dictValue","dictLabel")+'】 \n ';
      }

      if(tools.Is_null( _this.queryParams.Switch_network)){
        msg += ' 卡状态为：【'+tools.getkeyValue(Switch_network,_this.queryParams.Switch_network,"dictValue","dictLabel")+'】 \n ';
      }


      msg += ' 是否确定以上操作 ？ ';

      tools.openAsk(this,'warning', msg, this.uploadUp,_this, this.FalseFun,null);



    },

    uploadUp(_this){
      _this.$refs.upload.submit();
    },

    FalseFun(Pwd_Str){
      this.submitFileFormBtn = true;
    },





    /*获取查询参数*/
    getParams(){

      if(this.$refs.dept!=undefined & this.$refs.dept!=null & this.$refs.dept!='undefined' ){
        //console.log(this.$refs.dept.getCheckedKeys());
        if(this.$refs.dept.getCheckedKeys().length>0){
          this.queryParams.agent_id = this.$refs.dept.getCheckedKeys();
        }
      }

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

<style scoped>

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
