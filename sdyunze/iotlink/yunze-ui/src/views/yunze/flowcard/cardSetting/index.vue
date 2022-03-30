<template>
  <div class="app-container">


      <el-main style="padding-top: 2%;padding-left: 3%">
        <el-row >
          <el-col :span="12">
            <el-form :model="queryParams" ref="queryForm"   label-width="120px">
            <el-form-item  prop="status">
                <h4> 资费组划分、通道划分 、超量停机阈值设定
                  <el-tooltip placement="bottom">
                    <div slot="content">
                      选择需要修改的属性后 需选择是否同步修改
                      <br />同步修改 选择为 是:同步修改该属性，<br />
                      如： 超量停机阈值 不填写 且 为同步修改 将会不再对该卡进行超量停机管控。
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                </h4>
            </el-form-item>
            <el-form-item  prop="status">
               <span slot="label">
                资费选择
                <el-tooltip placement="right">
                  <div slot="content">
                    选择 划分 资费组
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>

              <el-select
                v-hasPermi="['yunze:cardFlow:queryPackageSimple']"
                v-model="queryParams.package_id"
                placeholder="资费组"
                clearable
                ref="packageSel"
                size="small"
                @change="SelectChange()"
                style="width: 310px"
              >
                <el-option
                  v-for="dict in packageOptions"
                  :key="dict.package_id"
                  :label="dict.package_agentname"
                  :value="dict.package_id"
                />
              </el-select>

            </el-form-item>
            <el-form-item  prop="status">
              <span slot="label">
                所属上游通道
                <el-tooltip placement="right">
                  <div slot="content">
                    调用API请求通道，划分错误会导致同步信息数据失败。
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
              <el-select
                v-model="queryParams.channel_id"
                placeholder="所属通道"
                clearable
                v-hasPermi="['yunze:card:queryParamsChannelId']"
                size="small"
                @change="SelectChange()"
                style="width: 310px"
              >
                <el-option
                  v-for="dict in findCr_RouteOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
              <el-form-item  >
              <span slot="label">
                超停阈值MB
                <el-tooltip placement="right">
                  <div slot="content">
                    超量停机阈值 达到指定 MB 后会 进行停机 指令，停机后需手动复机
                    <br />且在同一上游结算周期内，停机后复机不扩大超停阈值会继续触发超量停机！
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
                <el-input style="width: 310px" @keydown.native="handleInput" @blur="SelectChange()"    v-model="queryParams.remind_ratio"  placeholder="请输入超量停机 阈值（MB）" />

              </el-form-item>

              <el-form-item  >
              <span slot="label">
                超停是否同步
                <el-tooltip placement="right">
                  <div slot="content">
                    设置超量停机时需要 选择是
                    <br /> 如 想取消超量停机 》 超停阈值MB （不用填写） ，超停是否同步（选择是）【此时会更新超量停机阈值】
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
                <el-radio-group>
                  <label class="el-radio " v-for="(item,index) in customize_whether">
                                    <span class="el-radio__input my_checkbox__inner">
                                    <input type="radio" v-model="queryParams.Is_remind_ratio"
                                           name="in_stock"
                                           @click="SelectChange()"
                                           :value="item.dictValue"/>
                                     </span>
                    <span class="el-radio__label ">{{item.dictLabel}}</span>
                  </label>
                </el-radio-group>
              </el-form-item>
            <!--未订购资费 是否 轮询停机-->
              <el-form-item label-width="200px" @change="SelectChange()">
              <span slot="label">
                未订购资费 是否 轮询停机
                <el-tooltip placement="right">
                  <div slot="content">
                   未订购资费 轮询断网 选择否不轮询停机或断网,相当于取消
                    <br />
                    选择是?如果他未订购资费会进行断网,或者停机操作!~~~默认选中不操作.
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>
                <el-radio-group v-model="queryParams.is_Disconnected">
                <el-radio  :label="999">不操作</el-radio>
                <el-radio  :label="0">否</el-radio>
                <el-radio  :label="1">是</el-radio>
                </el-radio-group>
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
</template>

<script>
import { queryPackageSimple,findCr} from "@/api/yunze/flowcard/card/card";
import { getToken } from "@/utils/auth";
import tools from "@/utils/yunze/tools";



export default {
  name: "cardSetting",
  data() {
    return {
      //保存 编辑 按钮
      executionTaskDis: false,
      iccids:[],
      validateTypeOptions:[],//生效类型
      findCr_RouteOptions: [],// API通道信息
      customize_whether: [],//自定义是否

      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,
      // radio: '1',

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
        url: process.env.VUE_APP_BASE_API + "/yunze/card/importSet",
        Pstr:'',//携带参数
      },

      // 查询参数
      queryParams: {
        package_id:null,
        remind_ratio: null,
        Is_remind_ratio:'0',
        channel_id:null,
        is_Disconnected:999,
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
    //加载 API通道名称
    if(window['findCr_RouteOptions']!=undefined &&  window['RouteOptions']!=null && window['findCr_RouteOptions']!=''){
      this.findCr_RouteOptions = window['findCr_RouteOptions'];
    }else{
     findCr().then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        window['findCr_RouteOptions'] = jsonobj.Data;
        this.findCr_RouteOptions = window['findCr_RouteOptions'];
      });
    }


    //加载 自定义 是否 0 1 否 是
    if(window['customize_whether']!=undefined &&  window['customize_whether']!=null && window['customize_whether']!=''){
      this.customize_whether = window['customize_whether'];
    }else{
      this.getDicts("yunze_customize_whether").then(response => {
        window['customize_whether'] = response.data;
        this.customize_whether = window['customize_whether'];
      });
    }


    //加载 资费组名称
    if(window['packageOptions']!=undefined &&  window['packageOptions']!=null && window['packageOptions']!=''){
      this.packageOptions = window['packageOptions'];
    }else{
      queryPackageSimple().then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        window['packageOptions'] = jsonobj.Data;
        this.packageOptions = window['packageOptions'];
      });
    }



  },
  methods: {


    handleInput(e) {
      e.target.value=e.target.value.replace(/[^\d]/g,'');
    },




    SelectChange(){
      this.getPwd_Str();
      console.log("SelectChange");
    },
    // 加密数据
    getPwd_Str(){
      let map ={};
      map.package_id = this.queryParams.package_id;
      map.remind_ratio = this.queryParams.remind_ratio;
      map.Is_remind_ratio = this.queryParams.Is_remind_ratio;
      map.channel_id = this.queryParams.channel_id;
      map.is_Disconnected = this.queryParams.is_Disconnected;
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
    // 提交上传文件
    submitFileForm() {
      let _this = this;
      this.SelectChange();
      this.submitFileFormBtn = false;
      if (tools.Is_null(this.queryParams.package_id) == true||
        tools.Is_null(this.queryParams.channel_id) == true
      ){
        this.titleUp(_this);
      }else  if( this.queryParams.Is_remind_ratio=="1"){
        this.titleUp(_this);
      }else if(this.queryParams.is_Disconnected=="0") {
        this.titleUp(_this);
      }else if(this.queryParams.is_Disconnected=="1"){
        this.titleUp(_this);
      }else{
        this.submitFileFormBtn = true;
        tools.open(this, "请选择需要设置更新的属性条件！")
      }

    },

    titleUp(_this){
      let msg = '';
      if(tools.Is_null(_this.queryParams.package_id)){
        msg += ' 更新 资费组为【'+tools.getPackageNNamekeyValue(packageOptions,_this.queryParams.package_id)+'】 ';
      }
      if(tools.Is_null( _this.queryParams.channel_id)){
        msg += ' 更新 上游通道为【'+tools.getkeyValue(findCr_RouteOptions,_this.queryParams.channel_id,"dictValue","dictLabel")+'】 ';
      }
      if(_this.queryParams.Is_remind_ratio=="1"){
        if(tools.Is_null( _this.queryParams.remind_ratio)){
          msg += ' 更新 》设置 超量停机阈值 为【'+_this.queryParams.remind_ratio+'】MB ';
        }else{
          _this.queryParams.remind_ratio =  '';
          msg += ' 更新 》取消 超量停机设置  ';
        }
      }
      if(_this.queryParams.is_Disconnected=="0"){
        if(tools.Is_null( _this.queryParams.remind_ratio)){
          msg += ' 轮询 》设置 未订购资费 轮询断网 为【'+_this.queryParams.remind_ratio+'】MB ';
        }else{
          _this.queryParams.remind_ratio =  '';
          msg += ' 轮询 》设置 未订购资费 轮询断网  ';
        }
      }
      if(_this.queryParams.is_Disconnected=="1"){
        if(tools.Is_null( _this.queryParams.remind_ratio)){
          msg += ' 轮询 》设置 未订购资费 轮询断网 为【'+_this.queryParams.remind_ratio+'】MB ';
        }else{
          _this.queryParams.remind_ratio =  '';
          msg += ' 轮询 》设置 未订购资费 轮询断网  ';
        }
      }
      msg += ' 是否确定以上操作 ？ ';
      tools.openAsk(this,'warning', msg, this.uploadUp,_this, this.FalseFun,null);
  },

    uploadUp(_this){
       console.log("uploadUp  ")
      _this.$refs.upload.submit();
    },

    FalseFun(Pwd_Str){
      this.submitFileFormBtn = true;
    },

    //任务下载详情
    openExecutionTaskInfo(ExecutionTask){
      let _this=this;
      _this.Urls = [];
      let url  = ExecutionTask.url;
      let reg   = RegExp(/,/);
      let Url_list = [];
      if(url.match(reg)){
        Url_list = url.split(",");
      }else{
        if(url!=null){
          Url_list.push(url);
        }
      }
      // console.log(Url_list)
      for (let i = 0; i <Url_list.length ; i++) {
        let Url_map = {};
        let url_val = Url_list[i];

        for (let j = 0; j <ExecutionTask_OutType.length ; j++) {
          let obj  = ExecutionTask_OutType[j];
            let type = obj.dictValue;
          if(url_val.indexOf(type) != -1){
            Url_map.name = obj.dictLabel;
          }
        }
        Url_map.value = url_val;
        _this.Urls.push(Url_map);
      }

      _this.executionTaskDis=true;
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






    /*代理查询拓展*/
    agentShow(){
      let _this = this;
      _this.option_show=!_this.option_show;
      _this.mainwidth = _this.option_show?20:24;
    },
    /** 查询部门下拉树结构 */
    getTreeselect() {
      treeselect().then(response => {
        this.deptOptions = response.data;
      });
    },
    // 筛选节点
    filterNode(value, data) {
      if (!value) return true;
      return data.label.indexOf(value) !== -1;
    },


    // 取消按钮
    cancel() {
      this.open = false;
    },
    /** 搜索按钮操作 */
    handleQuery() {
      this.queryParams.page = 1;
    },
    /** 重置按钮操作 */
    resetQuery() {
      this.queryParams = {};
      this.queryParams.pageNum= 1;
      this.queryParams.pageSize= 10;
      this.selTime = null;
      this.handleQuery();
    },
    // 多选框选中数据
    handleSelectionChange(selection) {
      this.ids = selection.map(item => item.userId);
      this.single = selection.length != 1;
      this.multiple = !selection.length;
    },
    /** 新增按钮操作 */
    handleAdd() {

      this.form = {};//清空数据
      //初始化 新增 数据
      this.form.cd_code= "YiDong_EC";
      this.form.cd_operator_type= "1";
      this.form.cd_lunxun= "1";
      this.form.cd_status= "1";
      this.show_ds = true;
      this.executionTaskAdd = true;
      this.executionTaskEdie = false;
    },
    /** 修改按钮操作 */
    handleUpdate(row) {
      this.executionTaskAdd = false;
      this.executionTaskEdie = true;
      this.finddata(row);
    },
    /** 详情按钮操作 */
    ViewDetails(row) {
      //console.log(row);
      this.executionTaskAdd = false;
      this.executionTaskEdie = false;
        this.finddata(row);
    },



    /*获取详细信息*/
    finddata(row){
       this.form = {};//清空数据
       let map ={};
       map.cd_id = row.cd_id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      getexecutionTask(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){
            this.form = jsonobj.Data;
            this.show_ds = true;
          }else{
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;

        }
      );

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

