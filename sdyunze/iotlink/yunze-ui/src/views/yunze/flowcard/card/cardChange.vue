<template>
  <div class="app-container">
    <el-row :gutter="20">

      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24" >
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">
          <!--条件选择-->
          <el-form-item label="条件选择">
          <el-input
            v-model="queryParams.value"
            placeholder="查询值"
            clearable
            size="small"
            style="width: 350px"
            @keyup.enter.native="handleQuery"
          >
            <el-select
              v-model="queryParams.type"
              placeholder="查询条件"
              clearable
              slot="prepend"
              style="width: 110px"
            >
              <el-option
                v-for="dict in typeOptions"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-input>
          </el-form-item>
            <!--变更类型   -->
          <el-form-item label="变更类型" prop="status">
          <el-select
            v-model="queryParams.taskType"
            placeholder="变更类型"
            clearable
            size="small"
            style="width: 110px"
          >
            <el-option
              v-for="dict in TaskProject"
              :key="dict.dictValue"
              :label="dict.dictLabel"
              :value="dict.dictValue"
            />
          </el-select>
        </el-form-item>
          <!--来源类型   -->
          <el-form-item label="来源类型" prop="status">
            <el-select
              v-model="queryParams.source"
              placeholder="来源类型"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in cardChange"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <!--  时间类型-->
          <el-form-item label="时间类型" prop="status">
            <el-select
              v-model="queryParams.timetype"
              placeholder="时间类型"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in timecCreateTime"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item >
            <el-date-picker
              v-model="selTime"
              type="datetimerange"
              :picker-options="pickerOptions"
              range-separator="至"
              value-format="yyyy-MM-dd"
              start-placeholder="开始日期"
              end-placeholder="结束日期"
              style="width: 360px"
              align="right">
            </el-date-picker>

          </el-form-item>

          <el-form-item >
            <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
            <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
          </el-form-item>

          <el-form-item label="执行状态" prop="status">
            <el-select
              v-model="queryParams.state"
              placeholder="状态"
              clearable
              size="small"
              style="width: 70px"
            >
              <el-option
                v-for="dict in ExecutionStatus"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>


        </el-form>


        <el-row :gutter="10" class="mb8">
          <right-toolbar :showSearch.sync="showSearch"   v-hasPermi="['yunze:change:list']" @queryTable="getList" :columns="columns"></right-toolbar>
        </el-row>

        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">
          <el-table-column   label="iccid" align="center"  prop="iccid" v-if="columns[0].visible" :show-overflow-tooltip="true" width="190"/>
          <el-table-column   label="变更类型" align="center"  prop="ctype" v-if="columns[1].visible" >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(TaskProject,scope.row.ctype)}}
            </template>
          </el-table-column>
          <el-table-column   label="来源类型" align="center"  prop="source_type" v-if="columns[2].visible">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(cardChange,scope.row.source_type)}}
            </template>
          </el-table-column>
          <el-table-column   label="变更前" align="center"  prop="cbefore" v-if="columns[3].visible" />
          <el-table-column   label="变更后" align="center"  prop="cafterward" v-if="columns[4].visible" />
          <el-table-column   label="创建时间" align="center"  prop="createTime" v-if="columns[5].visible" />
          <el-table-column   label="备注" align="center"  prop="remark" v-if="columns[6].visible" />
        </el-table>


        <pagination
          v-show="total>0"
          :total="total"
          :page.sync="queryParams.pageNum"
          :limit.sync="queryParams.pageSize"
          @pagination="getList"
        />
      </el-col>
    </el-row>
    <!-- 详情查看 -->
    <el-dialog :title="title" :visible.sync="show_ds" width="960px" append-to-body>
      <el-form ref="form" :model="form" label-width="80px">
        <el-row>
          <el-col :span="24">
            <el-form-item label="请求参数" :model="cardLists">
              <el-input type="textarea" style="min-height: 300px;" v-model="cardLists.request_args" />
            </el-form-item>
            <!--:model=""--双向绑定、 style="margin-top: -150px设置高度  -->
            <el-form-item label="响应参数" :model="cardLists" style="margin-top: -150px">
              <el-input type="textarea" style="min-height: 300px;" v-model="cardLists.response_args" />
            </el-form-item>
          </el-col>
        </el-row>
      </el-form>
    </el-dialog>


    <el-dialog :close-on-click-modal="false"
               title="下载执行日志结果" :visible.sync="executionTaskDis">
      <hr style="color:green"/>
      <el-row :gutter="20">
        <ul class="list-group" v-for="(item,index) in Urls">

        </ul>
      </el-row>
    </el-dialog>

  </div>
</template>

<script>
import { listChange } from "@/api/yunze/flowcard/card/change";
import tools from "@/utils/yunze/tools";

export default {
  name: "CardChange",
  data() {
    return {
      //保存 编辑 按钮
      executionTaskDis: false,
      Urls:[],
      cardLists:[],
      pickerOptions: {
        shortcuts: [{
          text: '最近一周',
          onClick(picker) {
            const end = new Date();
            const start = new Date();
            start.setTime(start.getTime() - 3600 * 1000 * 24 * 7);
            picker.$emit('pick', [start, end]);
          }
        }, {
          text: '最近一个月',
          onClick(picker) {
            const end = new Date();
            const start = new Date();
            start.setTime(start.getTime() - 3600 * 1000 * 24 * 30);
            picker.$emit('pick', [start, end]);
          }
        }, {
          text: '最近三个月',
          onClick(picker) {
            const end = new Date();
            const start = new Date();
            start.setTime(start.getTime() - 3600 * 1000 * 24 * 90);
            picker.$emit('pick', [start, end]);
          }
        }]
      },

      tools:tools,
      customize_whether: [],//自定义是否
      ExecutionTask_OutType : [],// 执行日志导出类别

      SetMealImport:false, //详情查询 套餐信息
      show_ds:false, //详情查询
      selTime:'',//时间选择
      mainwidth:24,//宽度
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
      cardList: [],


      // 弹出层标题
      title: "",
      // 是否显示弹出层
      open: false,
      // 默认密码
      initPassword: undefined,
      // 日期范围
      dateRange: [],
      // 状态数据字典
      typeOptions: [],
      // 连接管理时间查询类型 数据字典
      timecCreateTime: [],
      // 简要套餐名称
      packageOptions: [],
      //降序
      ORDER_BY_type: [],

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
      //任务类别
      category_type: "",
      // 任务类别 字典
      TaskProject: [],
      //来源类型
      cardChange: [],
      //状态
      ExecutionStatus: [],

      // 运营商类别 字典
      operators_type: [],
      // 执行日志状态 字典
      channelStatusOptions: [],
      // 执行日志编码 字典
      channelCodeOptions: [],
      // 表单参数
      form: {},

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
        type: null,
        value: null,
        cd_status: [],
        staTime: null,
        endTime: null,
        timetype: null,
        taskType: null,
        source: null,
        ctype: null,
        source_type: null,
      },
      //通过id查询
      getById: {
        id: ''
      },
      // 列信息
      columns: [
        { key: 0, label: `iccid`, visible: true },
        { key: 1, label: `变更类型`, visible: true },
        { key: 2, label: `来源类型`, visible: true },
        { key: 3, label: `变更前`, visible: true },
        { key: 4, label: `变更后`, visible: true },
        { key: 5, label: `创建时间`, visible: true },
        { key: 6, label: `备注`, visible: true },
      ],
      // 表单校验
      rules: {
      }
    };
  },

  created() {
    const iccid = this.$route.params.iccid;
    if(tools.Is_null(iccid)){
      this.queryParams.value = iccid;
    }
    //加载 条件选择
    if(window['typeOptions']!=undefined &&  window['typeOptions']!=null && window['typeOptions']!=''){
      this.typeOptions = window['typeOptions'];
    }else{
      this.getDicts("yz_card_info_change").then(response => {
        window['typeOptions'] = response.data;

        this.typeOptions = window['typeOptions'];
      });
    }
    console.log(this.typeOptions)
    //加载 模块类别
    if(window['TaskProject']!=undefined &&  window['TaskProject']!=null && window['TaskProject']!=''){
      this.TaskProject = window['TaskProject'];
    }else{
      this.getDicts("yz_cardChange_type").then(response => {

        window['TaskProject'] = response.data;
        this.TaskProject = window['TaskProject'];
      });
    }
    //执行 状态
    if(window['ExecutionStatus']!=undefined &&  window['ExecutionStatus']!=null && window['ExecutionStatus']!=''){
      this.ExecutionStatus = window['ExecutionStatus'];
    }else{
      this.getDicts("yz_cardChange_ExecutionStatus").then(response => {

        window['ExecutionStatus'] = response.data;
        this.ExecutionStatus = window['ExecutionStatus'];
      });
    }
    //加载 模块类别
    if(window['cardChange']!=undefined &&  window['cardChange']!=null && window['cardChange']!=''){
      this.cardChange = window['cardChange'];
    }else{
      this.getDicts("yz_cardChange_sourceType").then(response => {

        window['cardChange'] = response.data;
        this.cardChange = window['cardChange'];
      });
    }
    //加载 时间赛选查询条件
    if(window['timecCreateTime']!=undefined &&  window['timecCreateTime']!=null && window['timecCreateTime']!=''){
      this.timecCreateTime = window['timecCreateTime'];
    }else{
      this.getDicts("yunze_syslogs_sel_timetype\n").then(response => {
        window['timecCreateTime'] = response.data;
        this.timecCreateTime = window['timecCreateTime'];
      });
    }



    //加载 执行日志导出类别
    if(window['ExecutionTask_OutType']!=undefined &&  window['ExecutionTask_OutType']!=null && window['ExecutionTask_OutType']!=''){
      this.ExecutionTask_OutType = window['ExecutionTask_OutType'];
    }else{
      this.getDicts("yunze_ExecutionTask_OutType").then(response => {
        window['ExecutionTask_OutType'] = response.data;
        this.ExecutionTask_OutType = window['ExecutionTask_OutType'];
      });
    }


    this.queryParams.type = '1';
    this.getList();
  },
  methods: {



    /*获取查询参数*/
    getParams(){

      if(this.$refs.dept!=undefined & this.$refs.dept!=null & this.$refs.dept!='undefined' ){
        //console.log(this.$refs.dept.getCheckedKeys());
        if(this.$refs.dept.getCheckedKeys().length>0){
          this.queryParams.agent_id = this.$refs.dept.getCheckedKeys();
        }
      }
      if(tools.Is_null(this.queryParams.timetype) && this.selTime !=null){
        //console.log(this.selTime);
        this.queryParams.staTime = this.selTime[0];
        this.queryParams.endTime = this.selTime[1];
      }


    },
    ViewDetail(row) {
      console.log(row);
      this.show_ds = true;
      //调用数据

      this.getById.id = row.id;
      this.findSys();


    },
    /*下载执行日志*/
    getDownloadExecutionTask(row){
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API+"/yunze/change/download?Pstr="+Pwd_Str, '_blank');

    },

    //下载 xls 文件 格式
    getdownloadConversionk(row){
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API+"/yunze/change/downloadConversion?Pstr="+Pwd_Str, '_blank');

    },



    /** 查询执行日志列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      listChange(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if(jsonobj.code==200){
            this.cardList = jsonobj.Data.Data;
            this.total = jsonobj.Data.Pu.rowCount;
          }else{
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;
        }
      );
    },


    // 取消按钮
    cancel() {
      this.open = false;
    },
    /** 搜索按钮操作 */
    handleQuery() {
      this.queryParams.page = 1;
      this.getList();
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

