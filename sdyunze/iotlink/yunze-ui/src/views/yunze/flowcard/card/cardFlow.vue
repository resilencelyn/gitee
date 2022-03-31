<template>
  <div class="app-container">
    <el-row :gutter="20">

      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24" >
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">

          <el-form-item label="资费状态" prop="status">
            <el-select
              v-model="queryParams.status"
              placeholder="资费计划状态"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in cardValidType"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>


          <el-form-item label="排序规则" prop="status" >

            <el-select
              v-model="queryParams.iccid"
              placeholder="请输入"
              clearable
              size="small"
              style="width: 100px"
            >
              <el-option
                v-for="dict in findCr_RouteOptions "
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>

          </el-form-item>


          <!--资费计划-->
          <el-form-item label="资费计划类别" prop="status" label-width="100px">
            <el-select
              v-model="queryParams.packet"
              placeholder="请选择类别"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in RatePlan"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <!-- 状态类别 -->
          <el-form-item label="生效类别" prop="status">
            <el-select   effect
                         v-model="queryParams.effect"
                         placeholder="选择生效类别"
                         clearable
                         size="small"
                         style="width: 110px"
            >
              <el-option
                v-for="dict in takeEffect"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
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
              <el-form-item label="时间类型" prop="status" style="margin-bottom: 10px;line-height:35px">
                <el-select
                  v-model="queryParams.timetype"
                  placeholder="时间类型"
                  clearable
                  size="small"
                  style="width: 110px"
                >
                  <el-option
                    v-for="dict in flowTime"
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
              <el-form-item label="排序类型" prop="status" style="margin-bottom: 10px;line-height:35px">
                <el-select
                  v-model="queryParams.ORDER_BY_type"
                  placeholder="请输入类型"
                  clearable
                  size="small"
                  style="width: 100px"
                >
                  <el-option
                    v-for="dict in Lifting_sequence"
                    :key="dict.dictValue"
                    :label="dict.dictLabel"
                    :value="dict.dictValue"
                  />
                </el-select>
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
                @click="CardFlowExport"
                v-hasPermi="['yunze:flowhis:exportflowhis']"
              >导出</el-button>
            </el-col>
            <right-toolbar :showSearch.sync="showSearch"    @queryTable="getList" :columns="columns"></right-toolbar>
          </el-row>
        </el-form>



        <el-table v-loading="loading" :data="cardList" >
          <el-table-column   label="id" align="center" prop="id" v-if="columns[0].visible"/>
          <el-table-column   label="资费组" align="center"  prop="package_id" v-if="columns[1].visible" >
            <template slot-scope="scope">
              {{tools.getkeyValue(packageOptions,scope.row.package_id,"package_id","package_agentname")}}
            </template>
          </el-table-column>
          <el-table-column   label="资费计划" align="center"  prop="packet_id" v-if="columns[2].visible" >
            <template slot-scope="scope" >
              {{tools.getDkeyValue(PacketOptions,scope.row.packet_id)}}
            </template>
          </el-table-column>
          <el-table-column   label="订单号" align="center"  prop="ord_no" v-if="columns[3].visible" />
          <el-table-column   label="流量" align="center"  prop="true_flow" v-if="columns[4].visible" />
          <el-table-column   label="生效时间" align="center"  prop="start_time" v-if="columns[5].visible" />
          <el-table-column   label="到期时间" align="center"  prop="end_time" v-if="columns[6].visible" />
          <el-table-column   label="创建时间" align="center"  prop="create_time" v-if="columns[7].visible" />
          <el-table-column   label="状态" align="center" prop="status" v-if="columns[9].visible">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(cardValidType,""+scope.row.status)}}
            </template>
          </el-table-column>
          <el-table-column   label="资费计划类别" align="center" prop="packet_type" v-if="columns[10].visible">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(RatePlan,""+scope.row.packet_type)}}
            </template>
          </el-table-column>
          <el-table-column   label="使用流量" align="center" prop="use_flow" v-if="columns[11].visible"/>
          <el-table-column   label="对应串号" align="center" prop="iccid" v-if="columns[12].visible"/>
          <el-table-column   label="生效类别" align="center" prop="validate_type" v-if="columns[13].visible">
            <template slot-scope="scope" >
              {{tools.getDkeyValue(takeEffect,""+scope.row.validate_type)}}
            </template>
          </el-table-column>
          <el-table-column   label="更新累计包时间" align="center" prop="validate_time" v-if="columns[14].visible"/>
          <el-table-column   label="天包记录已用流量" align="center" prop="use_so_flow" v-if="columns[15].visible"/>

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
      <hr style="color:#4aba43"/>
      <el-row :gutter="20">
        <ul class="list-group" v-for="(item,index) in Urls">

        </ul>
      </el-row>
    </el-dialog>

  </div>
</template>

<script>
import { listflow,ExportCardFlow} from "@/api/yunze/flowcard/card/flow";

import tools from "@/utils/yunze/tools";
import {queryPackageSimple} from "@/api/yunze/flowcard/card/card";
import {queryPacketSimple} from "@/api/yunze/order/order";
export default {
  name: "CardFlow",
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
      showSearch: false,
      // 总条数
      total: 0,
      // 用户表格数据
      cardList: [],
      //定义
      timetypeOptions: [],

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
      flowTime: [],
      // 简要套餐名称
      packageOptions: [],
      Lifting_sequence: [],

      iccid: undefined,


      //详情
      tablefrom: [],

      FlowData : [],
      selIccid:[{FlowData:''}],
      activeNames:[0],

      //卡状态
      status_type: "",
      // 资费计划状态 字典
      cardValidType: [ ],
      // 资费计划类别
      RatePlan: [],
      // 资费计划生效类别
      takeEffect: [],

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
      PacketSelOptions:[],//资费计划查询
      // 表单参数
      form: {},
      dictType: [],

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
        channel_id: null,
        create_time: null,
        ORDER_BY_type: null,
        iccid: null,
        status: null,

      },
      //通过id查询
      getByIds: {
        id: ''
      },
      // 列信息
      columns: [
        { key: 0, label: `id`, visible: true },
        { key: 1, label: `充值套餐`, visible: true },
        { key: 2, label: `充值流量包`, visible: true },
        { key: 3, label: `订单号`, visible: true },
        { key: 4, label: `流量`, visible: true },
        { key: 5, label: `生效时间`, visible: true },
        { key: 6, label: `到期时间`, visible: true },
        { key: 7, label: `创建时间`, visible: true },
        { key: 8, label: `资费生效类别`, visible: true },
        { key: 9, label: `状态`, visible: true },
        { key: 10, label: `0-月包，1-天包`, visible: true },
        { key: 11, label: `使用流量`, visible: true },
        { key: 12, label: `对应串号`, visible: true },
        { key: 13, label: `0:正常包 1：累计包`, visible: true },
        { key: 14, label: `更新累计包时间`, visible: true },
        { key: 15, label: `天包记录已用流量`, visible: true },
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
    //加载 执行任务导出类别
    if(window['Lifting_sequence']!=undefined &&  window['Lifting_sequence']!=null && window['Lifting_sequence']!=''){
      this.Lifting_sequence = window['Lifting_sequence'];
    }else{
      this.getDicts("Lifting_sequence").then(response => {
        window['Lifting_sequence'] = response.data;
        this.Lifting_sequence = window['Lifting_sequence'];
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
        //console.log(jsonobj);
        window['PacketOptions'] = jsonobj.Data;
        this.PacketOptions = window['PacketOptions'];
      });
    }


    //加载 资费计划生效中类型
    if(window['cardValidType']!=undefined &&  window['cardValidType']!=null && window['cardValidType']!=''){
      this.cardValidType = window['cardValidType'];
    }else{
      this.getDicts("yunze_card_valid_type").then(response => {
        window['cardValidType'] = response.data;
        this.cardValidType = window['cardValidType'];
      });
    }

    // 资费计划类别
    if(window['RatePlan']!=undefined &&  window['RatePlan']!=null && window['RatePlan']!=''){
      this.RatePlan = window['RatePlan'];
    }else{
      this.getDicts("yunze_packet_type").then(response => {
        window['RatePlan'] = response.data;
        this.RatePlan = window['RatePlan'];
      });
    }

    // 资费计划生效类别
    if(window['takeEffect']!=undefined &&  window['takeEffect']!=null && window['takeEffect']!=''){
      this.takeEffect = window['takeEffect'];
    }else{
      this.getDicts("yunze_card_takeEffect_type").then(response => {
        window['takeEffect'] = response.data;
        this.takeEffect = window['takeEffect'];
      });
    }

    //加载
    if(window['yz_card_flow']!=undefined &&  window['yz_card_flow']!=null && window['yz_card_flow']!=''){
      this.typeOptions = window['yz_card_flow'];
    }else{
      this.getDicts("yz_card_flow").then(response => {
        window['yz_card_flow'] = response.data;
        this.typeOptions = window['yz_card_flow'];
      });
    }



    //加载 时间赛选查询条件
    if(window['flowTime']!=undefined &&  window['flowTime']!=null && window['flowTime']!=''){
      this.flowTime = window['flowTime'];
    }else{
      this.getDicts("yunze_card_flow").then(response => {
        window['flowTime'] = response.data;
        this.flowTime = window['flowTime'];

      });
    }


    //加载 API通道名称
    if(window['yunze_sorting_rules']!=undefined &&  window['yunze_sorting_rules']!=null && window['yunze_sorting_rules']!=''){
      this.findCr_RouteOptions = window['yunze_sorting_rules'];
    }else{
      this.getDicts("yunze_sorting_rules").then(response => {
        window['yunze_sorting_rules'] = response.data;
        this.findCr_RouteOptions = window['yunze_sorting_rules'];
      });
    }

    /**资费组选择*/

    if(tools.Is_null(this.queryParams.package_id)){
      // console.log(window['PackageOptions_'+this.queryParams.package_id]);
      this.PacketSelOptions = window['PackageOptions_'+this.queryParams.package_id];
    }

    this.selTime.push(tools.getDateMonth(3));
    this.selTime.push(tools.gitData());
    //console.log(this.selTime)
    this.queryParams.timetype = '1';
    this.queryParams.type = '1';
    // this.queryParams.iccid = '0';
    this.queryParams.ORDER_BY_type = '1';
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


    /** 查询执行日志列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      /*this.resetQuery();*/
      listflow(Pwd_Str).then(response => {
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

    CardFlowExport(){
      this.getParams();
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      ExportCardFlow(Pwd_Str).then(response => {
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

