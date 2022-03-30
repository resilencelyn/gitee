<template>
  <div class="app-container">


      <el-main style="padding-top: 2%;padding-left: 3%">
        <el-row >


          <el-col :span="12">
            <el-form :model="queryParams" ref="queryForm"   label-width="87px">
            <el-form-item  prop="status">
              <h4>ICCID批量充值(大于100条/次操作上传文件)</h4>
            </el-form-item>
            <el-form-item  prop="status">
               <span slot="label">
                资费选择
                <el-tooltip placement="right">
                  <div slot="content">
                    选择充值资费组 与 资费计划 进行充值
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
                @change="packageSelChange"
                size="small"
                style="width: 210px"
              >
                <el-option
                  v-for="dict in packageOptions"
                  :key="dict.package_id"
                  :label="dict.package_agentname"
                  :value="dict.package_id"
                />
              </el-select>

              <el-select
                v-hasPermi="['yunze:cardFlow:queryPackageSimple']"
                v-model="queryParams.packet_id"
                placeholder="资费计划"
                clearable
                @change="PacketSelChange"
                size="small"
                style="width: 210px"
              >
                <el-option
                  v-for="dict in PacketSelOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>

            </el-form-item>
            <el-form-item  prop="status">
              <span slot="label">
                生效类型
                <el-tooltip placement="right">
                  <div slot="content">
                    选择  资费计划 时生效类型会根据当前资费计划切换为推荐类型
                    <br />当然，生效类型是可选择的 灵活的，可根据实际需求切换。
                  </div>
                  <i class="el-icon-question"></i>
                </el-tooltip>
              </span>

              <el-select
                v-model="queryParams.validate_type"
                placeholder="生效类型"
                clearable
                ref="packageSel"
                size="small"
                @change="validateTypeChange"
                style="width: 210px"
              >
                <el-option
                  v-for="dict in validateTypeOptions"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-form-item>
            <el-form-item>
                <span slot="label">
                  资费详情
                  <el-tooltip placement="right">
                    <div slot="content">
                      资费计划详情
                    </div>
                    <i class="el-icon-question"></i>
                  </el-tooltip>
                </span>

                <el-row >
                  <el-col :span="4" >
                  <span>资费周期 : </span>  <el-tag size="small">{{packet.packet_valid_time}}</el-tag>
                  </el-col>
                  <el-col :span="4" ><span>周期单位 : </span><el-tag size="small">{{packet.packet_valid_name}}</el-tag></el-col>
                  <el-col :span="8"  ><span>周期用量MB : </span><el-tag size="small">{{packet.packet_flow}}</el-tag></el-col>
                </el-row>
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
import { FindPacket } from "@/api/yunze/flowcard/rechargeTariff";
import { queryPacketSimple } from "@/api/yunze/order/order";
import { queryPackageSimple} from "@/api/yunze/flowcard/card/card";
import { getToken } from "@/utils/auth";
import tools from "@/utils/yunze/tools";



export default {
  name: "RechargeTariff",
  data() {
    return {
      //保存 编辑 按钮
      executionTaskDis: false,
      iccids:[],
      validateTypeOptions:[],//生效类型


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
        url: process.env.VUE_APP_BASE_API + "/yunze/order/importRecharge",
        Pstr:'',//携带参数
      },





      //当前查询参数
      sel:{
        iccid:'',
        agent_id:'',
        package_id:'',
      },
      // 查询参数
      queryParams: {
        pageNum: 1,
        pageSize: 10,
        packet_id:null,
        validate_type:null,
        type: null,
        value: null,
        cd_status: [],
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


    //加载 资费计划生效类别
    if(window['validateTypeOptions']!=undefined &&  window['validateTypeOptions']!=null && window['validateTypeOptions']!=''){
      this.validateTypeOptions = window['validateTypeOptions'];
    }else{
      this.getDicts("yunze_card_takeEffect_type").then(response => {
        window['validateTypeOptions'] = response.data;
        this.validateTypeOptions = window['validateTypeOptions'];
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


    //加载 资费计划
    if(window['PacketOptions']!=undefined &&  window['PacketOptions']!=null && window['PacketOptions']!=''){
      this.PacketOptions = window['PacketOptions'];
    }else{
      queryPacketSimple().then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        window['PacketOptions'] = jsonobj.Data;
        let arr = jsonobj.Data;
        var new_map = {},
          dest = [];
        if(arr!=null && arr.length>0){
          for(var i = 0; i < arr.length; i++) {
            var ai = arr[i];
            if(!new_map[ai.package_id]) { //依赖分组字段可自行更改！
              dest.push({
                package_id: ai.package_id, //依赖分组字段可自行更改！
                data: [ai]
              });
              new_map[ai.package_id] = ai; //依赖分组字段可自行更改！
            } else {
              for(var j = 0; j < dest.length; j++) {
                var dj = dest[j];
                if(dj.package_id == ai.package_id) { //依赖分组字段可自行更改！
                  dj.data.push(ai);
                  break;
                }
              }
            }
          }
          //存储资费计划
          for (let i = 0; i < dest.length; i++) {
            let obj = dest[i];
            window['PackageOptions_'+obj.package_id] = obj.data;
            //存储生效类型
            if(obj.data!=null && obj.data.length>0){
              for (let j = 0; j < obj.data.length; j++) {
                let Pobj = obj.data[j];
                window['validateTypeOptions_'+Pobj.dictValue] = Pobj.packet_valid_type;
              }
            }
          }
        }

        this.PacketOptions = window['PacketOptions'];
      });
    }


  },
  methods: {

    /**资费组选择*/
    packageSelChange(){
      if(tools.Is_null(this.queryParams.package_id)){
        //console.log(window['PackageOptions_'+this.queryParams.package_id]);
        //重置选项
        this.queryParams.packet_id =null;
        this.queryParams.validate_type =null;
        this.PacketSelOptions = window['PackageOptions_'+this.queryParams.package_id];
        this.packet.packet_flow = '-';
        this.packet.packet_valid_time = '-';
        this.packet.packet_valid_name = '-';
      }
      this.getPwd_Str();
    },

    //资费计划切换
    PacketSelChange(){
      if(tools.Is_null(this.queryParams.packet_id)){
        this.queryParams.validate_type =  ""+window['validateTypeOptions_'+this.queryParams.packet_id];
        let map = {};
        map.package_id = this.queryParams.package_id;
        map.packet_id = this.queryParams.packet_id;
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        FindPacket(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          if(jsonobj.code==200){
            this.packet = jsonobj.Data;
          }else{
            this.msgError("查询单条 资费计划  简要 异常！");
          }
        });
        this.getPwd_Str();
      }
    },

    validateTypeChange(){
      this.getPwd_Str();
    },
    // 加密数据
    getPwd_Str(){
      let map ={};
      map.package_id = this.queryParams.package_id;
      map.packet_id = this.queryParams.packet_id;
      map.validate_type = this.queryParams.validate_type;
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
      this.submitFileFormBtn = false;
      let _this = this;
      if (tools.VerificationsText(_this, _this.queryParams.package_id, "请选择资费组！") == true &&
        tools.VerificationsText(_this, _this.queryParams.packet_id, "请选择资费计划！") == true &&
        tools.VerificationsText(_this, _this.queryParams.validate_type, "请选择生效类型！") == true )
      {
        this.$refs.upload.submit();
      }else {
        this.submitFileFormBtn = true;
      }
    },


    //任务下载详情
    openExecutionTaskInfo(ExecutionTask){
      let _this=this;
      _this.Urls = [];
      //console.log(ExecutionTask);
      let url  = ExecutionTask.url;
      let reg   = RegExp(/,/);
      let Url_list = [];
      //console.log(url.match(reg))
      //console.log(url.match(reg).index)
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

