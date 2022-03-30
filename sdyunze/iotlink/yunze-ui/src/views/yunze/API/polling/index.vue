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
          <el-form-item label="轮询类型" prop="status">
            <el-select
              v-model="queryParams.polling_type"
              placeholder="请输入类型"
              clearable
              size="small"
              style="width: 200px"
            >
              <el-option
                v-for="dict in yunze_polling_type"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>

          <el-form-item label="通道名称" prop="status" >

            <el-select
              v-model="queryParams.channel_id"
              placeholder="所属通道"
              clearable
              v-hasPermi="['yunze:card:queryParamsChannelId']"
              size="small"
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
          <!--  时间类型-->
          <el-form-item label="时间类型" prop="year">
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
        </el-form>


        <el-row :gutter="10" class="mb8">
          <right-toolbar :showSearch.sync="showSearch"   v-hasPermi="['yunze:flowhis:list']" @queryTable="getList" :columns="columns"></right-toolbar>
        </el-row>

        <el-table v-loading="loading" :data="cardList" >
          <el-table-column   label="通道编号" align="center" prop="cd_id" v-if="columns[0].visible" >
          <template slot-scope="scope">
            {{tools.getDkeyValue(findCr_RouteOptions,scope.row.cd_id)}}
          </template>
          </el-table-column>

          <el-table-column   label="轮序批次编码" align="center"  prop="polling_id" v-if="columns[1].visible"/>
          <el-table-column   label="轮询类型" align="center"  prop="polling_id" v-if="columns[2].visible">
            <template slot-scope="scope">
              {{tools.getDkeyValue(yunze_polling_type,scope.row.polling_type)}}
            </template>
          </el-table-column>
          <el-table-column   label="轮序总数" align="center"  prop="cd_count" v-if="columns[3].visible" />
          <el-table-column   label="当前轮序数量" align="center"  prop="cd_current" v-if="columns[4].visible" />
          <el-table-column   label="创建时间" align="center"  prop="create_date" v-if="columns[5].visible" />
          <el-table-column   label="修改时间" align="center"  prop="upd_date" v-if="columns[6].visible" />
          <el-table-column   label="最近一次同步时间" align="center"  prop="syn_date" v-if="columns[7].visible" />



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

    <el-dialog :close-on-click-modal="false"
               title="下载执行任务结果" :visible.sync="executionTaskDis">
      <hr style="color:green"/>
      <el-row :gutter="20">
        <ul class="list-group" v-for="(item,index) in Urls">

        </ul>
      </el-row>
    </el-dialog>

  </div>
</template>

<script>
import { findCr} from "@/api/yunze/flowcard/card/card";
import { listSysLog } from "@/api/yunze/passageway/polling";
import tools from "@/utils/yunze/tools";

export default {
  name: "flowhis",
  data() {
    return {
      tools:tools,
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


      customize_whether: [],//自定义是否
      ExecutionTask_OutType : [],// 执行任务导出类别

      SetMealImport:false, //详情查询 套餐信息
      show_ds:false, //详情查询
      selTime:[],//时间选择
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

      findCr_RouteOptions: [],
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
      yunze_polling_type: [],

      cd_id: undefined,

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
      // 执行任务状态 字典
      channelStatusOptions: [],
      // 执行任务编码 字典
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
        timetype: '0',
        channel_id: null,
        polling_type: null,
      },
      //通过id查询
      getByIds: {
        id: ''
      },
      // 列信息
      columns: [
        { key: 0, label: `通道编号`, visible: true },
        { key: 1, label: `轮序批次编码`, visible: true },
        { key: 2, label: `轮询类型`, visible: true },
        { key: 3, label: `轮序总数`, visible: true },
        { key: 4, label: `当前轮序数量`, visible: true },
        { key: 5, label: `创建时间`, visible: true },
        { key: 6, label: `修改时间`, visible: true },
        { key: 7, label: `最近一次同步时间`, visible: true },

      ],
      // 表单校验
      rules: {
      }
    };
  },


  created() {

    //加载 执行任务导出类别
    if(window['yunze_polling_type']!=undefined &&  window['yunze_polling_type']!=null && window['yunze_polling_type']!=''){
      this.yunze_polling_type = window['yunze_polling_type'];
    }else{
      this.getDicts("yunze_polling_type").then(response => {
        window['yunze_polling_type'] = response.data;
        this.yunze_polling_type = window['yunze_polling_type'];
      });
    }


    const cd_id = this.$route.params && this.$route.params.cd_id;


    //加载 编号名称
    if(window['yz_passageway_polling']!=undefined &&  window['yz_passageway_polling']!=null && window['yz_passageway_polling']!=''){
      this.typeOptions = window['yz_passageway_polling'];
    }else{
      this.getDicts("yz_passageway_polling").then(response => {
        window['yz_passageway_polling'] = response.data;
        this.typeOptions = window['yz_passageway_polling'];
      });
    }



    //加载 时间赛选查询条件
    if(window['flowHisTime']!=undefined &&  window['flowHisTime']!=null && window['flowHisTime']!=''){
      this.flowHisTime = window['flowHisTime'];
    }else{
      this.getDicts("yunze_yz_passageway").then(response => {
        window['flowHisTime'] = response.data;
        this.flowHisTime = window['flowHisTime']
      });
    }


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


    // 赋值默认 查询时间
    let Today  = tools.gitData();
    console.log(Today);
    this.selTime.push(Today+' 00:00:00');
    this.selTime.push(Today+' 23:59:59');
    if(tools.Is_null(cd_id)){
      this.queryParams.channel_id = Number(cd_id);
    }
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

    /*下载执行任务*/
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




    /** 查询执行任务列表 */
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
  //  多选框选中数据
    handleSelectionChange(selection) {
      this.ids = selection.map(item => item.cd_id);
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

