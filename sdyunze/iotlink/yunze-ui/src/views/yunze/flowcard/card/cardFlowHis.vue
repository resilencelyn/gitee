<template>
  <div class="app-container">
    <el-row :gutter="20">

      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24" >
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">


        </el-form>

          <el-form :model="queryParams" ref="queryForm" :inline="true" label-width="68px" style="margin-bottom: -10px;line-height:35px">
            <el-row :gutter="10" class="mb8">
              <el-col :span="1.5">
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
              </el-col>
              <el-col :span="1.5">
                <!--  时间类型-->
                <el-form-item label="时间类型" prop="status" style="margin-bottom: -10px;line-height:35px">
                  <el-select
                    v-model="queryParams.timetype"
                    placeholder="时间类型"
                    clearable
                    size="small"
                    style="width: 110px"
                  >
                    <el-option
                      v-for="dict in flowHisTime"
                      :key="dict.dictValue"
                      :label="dict.dictLabel"
                      :value="dict.dictValue"
                    />
                  </el-select>
                </el-form-item>
                <el-form-item >
                  <el-date-picker
                    v-model="selTime"
                    type="daterange"
                    :picker-options="pickerOptions"
                    range-separator="至"
                    value-format="yyyy-MM-dd"
                    start-placeholder="开始日期"
                    end-placeholder="结束日期"
                    style="width: 360px"
                    align="right">
                  </el-date-picker>
                </el-form-item>
              </el-col>
              <el-col :span="1.5">
                  <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
              </el-col>
              <el-col :span="1.5">
                <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
              </el-col>
              <el-col :span="1.5">
                <el-button
                  type="warning"
                  plain
                  icon="el-icon-download"
                  size="mini"
                  :loading="exportLoading"
                  @click="CardFlowHisExport"
                  v-hasPermi="['yunze:flowhis:exportflowhis']"
                >导出</el-button>
              </el-col>
              <el-col :span="1.5">
                <el-button
                  type="danger"
                  plain
                  icon="el-icon-warning"
                  size="mini"
                  @click="illustrateDialogVisible = true"
                >必读说明</el-button>
              </el-col>



                <right-toolbar :showSearch.sync="showSearch"   v-hasPermi="['yunze:flowhis:list']" @queryTable="getList" :columns="columns"></right-toolbar>
            </el-row>
          </el-form>





        <el-dialog
            title="用量详情说明"
            :visible.sync="illustrateDialogVisible"
            width="30%"
            :before-close="illustrateHandleClose">
          <ul style="font-size: 25px;line-height: 50px;">
            <li>来源于当天轮询或手动同步 【上游接口】返回</li>
            <li>日用量 = 本月内上一记录用量推演计算而来， <span style="color: red">数据仅供参考！</span></li>
          </ul>
          <span slot="footer" class="dialog-footer">
            <el-button type="primary" @click="illustrateDialogVisible = false">确 定</el-button>
          </span>
        </el-dialog>


        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">

          <el-table-column label="对应串号" align="center" prop="iccid" v-if="columns[0].visible" :show-overflow-tooltip="true" width="190"  >
            <template slot-scope="scope">
                <span  @click="ViewDetail(scope.row)" class="link-type"
                       v-hasPermi="['yunze:flowhis:download']" >{{ scope.row.iccid }}
                </span>
            </template>
          </el-table-column>

<!--          <el-table-column   label="对应串号" align="center"  prop="iccid" v-if="columns[1].visible" />-->
          <el-table-column   label="年份" align="center"  prop="year" v-if="columns[1].visible" />
          <el-table-column   label="月份" align="center"  prop="month" v-if="columns[2].visible" />
          <el-table-column   label="日" align="center"  prop="day" v-if="columns[3].visible" />
          <el-table-column   label="创建时间" align="center"  prop="create_time" v-if="columns[4].visible" />
          <el-table-column   label="修改时间" align="center"  prop="update_time" v-if="columns[5].visible" />
          <el-table-column   label="当天使用总量" align="center"  prop="total_show_flow_now" v-if="columns[6].visible" />
          <el-table-column   label="当月使用总量" align="center"  prop="total_show_flow" v-if="columns[7].visible" />

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
            <el-form-item label="当天使用" >
            <el-input
              v-model="cardLists.total_flow_now"
              style="width: 200px"
            />
            </el-form-item>
            <br>
            <el-form-item label="当月使用">
              <el-input
                v-model="cardLists.total_flow"
                style="width: 200px"
              />
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
import { listSysLog,listgetbyid,ExportCardFlowHis } from "@/api/yunze/flowcard/card/flowhis";
import tools from "@/utils/yunze/tools";

export default {
  name: "CardFlowHis",
  data() {
    return {
      //保存 编辑 按钮
      executionTaskDis: false,
      Urls:[],
      cardLists:[],
      illustrateDialogVisible:false,//说明 显示
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
      selTime: [],//时间选择
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
      flowHisTime: [],
      // 简要套餐名称
      packageOptions: [],

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
      TaskProject: [ ],

      // 运营商类别 字典
      operators_type: [],
      // 执行日志状态 字典
      channelStatusOptions: [],
      // 执行日志编码 字典
      channelCodeOptions: [],
      // 表单参数
      form: {},
      iccid: undefined,
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
        iccid: null,
      },
      //通过id查询
      getByIds: {
        id: ''
      },
      // 列信息
      columns: [
        // { key: 0, label: `id`, visible: true },
        { key: 0, label: `对应串号`, visible: true },
        { key: 1, label: `年份`, visible: true },
        { key: 2, label: `月份`, visible: true },
        { key: 3, label: `日`, visible: true },
        { key: 4, label: `创建时间`, visible: true },
        { key: 5, label: `修改时间`, visible: true },
        { key: 6, label: `当天使用总量`, visible: true },
        { key: 7, label: `当月使用总量`, visible: true },

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
      this.getDicts("dosage_details_flow_his").then(response => {
        window['typeOptions'] = response.data;
        this.typeOptions = window['typeOptions'];
      });
    }

    //加载 时间赛选查询条件
    if(window['flowHisTime']!=undefined &&  window['flowHisTime']!=null && window['flowHisTime']!=''){
      this.flowHisTime = window['flowHisTime'];
    }else{
      this.getDicts("yunze_yz_card_flow_his").then(response => {
        window['flowHisTime'] = response.data;
        this.flowHisTime = window['flowHisTime'];
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

    this.queryParams.staTime = this.selTime.push(tools.gitDataCurrent())[0];//获取当前时间 年 月 日
    this.queryParams.endTime = this.selTime.push(tools.gitDataCurrent())[1];
    // console.log(tools.gitDataCurrent())
    this.queryParams.timetype = '1';
    this.queryParams.type = '1';
    this.getList();

  },
  methods: {


    //说明关闭
    illustrateHandleClose(){
      this.illustrateDialogVisible = false;
    },


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
      // console.log(this.queryParams)

    },
    ViewDetail(row) {
      // console.log(row);
      this.show_ds = true;
      //调用数据

      this.getByIds.id = row.id;
      this.findSys();


    },
    /*下载执行日志*/
    getDownloadExecutionTask(row){
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API+"/yunze/flowhis/download?Pstr="+Pwd_Str, '_blank');

    },

    //下载 xls 文件 格式
    getdownloadConversionk(row){
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API+"/yunze/flowhis/downloadConversion?Pstr="+Pwd_Str, '_blank');

    },

    CardFlowHisExport(){
      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      ExportCardFlowHis(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        //console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.msgSuccess(jsonobj.Data);
        }else{
          let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
          this.msgError(msg);
        }

      })
    },



    /** 查询执行日志列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
       //console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      listSysLog(Pwd_Str).then(response => {
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
    /** 查询执行日志列表 */
    findSys() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      //console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.getByIds));
      //console.log(Pwd_Str);
      listgetbyid(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.cardLists = jsonobj.Data;
            // this.cardLists.request_args = tools.formatJson(this.cardLists.request_args);
            // this.cardLists.response_args = tools.formatJson(this.cardLists.response_args);

          } else {
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
      this.queryParams.page = 1;
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

