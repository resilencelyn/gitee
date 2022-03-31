<template>
  <div class="app-container">
    <el-row :gutter="20">
      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24">
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">

          <!--任务类别   -->
          <el-form-item label="抄送状态" prop="status">
            <el-select
              v-model="queryParams.cc_state"
              placeholder="抄送状态"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in CCstatus"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>

          <!--抄送结果   -->
          <el-form-item label="抄送结果" prop="status">
            <el-select
              v-model="queryParams.cc_result"
              placeholder="抄送结果"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in Cchis_result"
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
                v-for="dict in timetypeHis"
                :key="dict.dictValue"
                :label="dict.dictLabel"
                :value="dict.dictValue"
              />
            </el-select>
          </el-form-item>
          <el-form-item>
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

        </el-form>

        <el-form :model="queryParams" ref="queryForm" :inline="true" label-width="68px" style="margin-bottom: 10px">
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
                    v-for="dict in typeOptionsCcUrl"
                    :key="dict.dictValue"
                    :label="dict.dictLabel"
                    :value="dict.dictValue"
                  />
                </el-select>
              </el-input>
            </el-col>
            <el-col :span="1.5">
              <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
            </el-col>
            <right-toolbar :showSearch.sync="showSearch" @queryTable="getList"></right-toolbar>
          </el-row>
        </el-form>

        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">
          <el-table-column label="抄送名称" align="center" prop="cc_name" v-if="columns[0].visible">
            <template slot-scope="scope">
                 <span @click="CcPatr(scope.row)" class="link-type">
                  {{ tools.getkeyValue(NameAddCcUrl, scope.row.cc_id, "id", "cc_name") }}
                 </span>
            </template>
          </el-table-column>
          <el-table-column label="推送API" align="center" prop="cc_url" v-if="columns[1].visible"/>
          <el-table-column label="推送邮件" align="center" prop="cc_email" v-if="columns[2].visible"/>
          <el-table-column label="抄送状态" align="center" prop="cc_state" v-if="columns[3].visible">
            <template slot-scope="scope">
                  {{ tools.getDkeyValue(CCstatus, scope.row.cc_state) }}
            </template>
          </el-table-column>
          <el-table-column label="抄送结果" align="center" prop="cc_result" v-if="columns[4].visible">
            <template slot-scope="scope">
              {{ tools.getDkeyValue(Cchis_result, scope.row.cc_result) }}
            </template>
          </el-table-column>
          <el-table-column label="抄送次数" align="center" prop="cc_count" v-if="columns[5].visible"/>
          <el-table-column label="备注" align="center" prop="remark" v-if="columns[6].visible"/>
          <el-table-column label="创建时间" align="center" prop="create_time" v-if="columns[8].visible"/>
          <el-table-column label="最近一次抄送时间" align="center" prop="cc_time" v-if="columns[9].visible"/>

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
    <el-dialog :modal-append-to-body="false"  :title="title" :visible.sync="open" width="500px" append-to-body>
      <el-input
        type="textarea"
        autosize
        placeholder="请输入内容"
        v-model="textarea1">
      </el-input>
    </el-dialog>

  </div>
</template>

<script>
import {listCcHis} from "@/api/yunze/automation/cchis";
import {getToken} from "@/utils/auth";
import {treeselect} from "@/api/system/dept";
import Treeselect from "@riophae/vue-treeselect";
import tools from "@/utils/yunze/tools";
import {NameAdd} from "@/api/yunze/automation/cc";

export default {
  name: "Channel",
  components: {Treeselect},
  data() {
    return {

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
      tools: tools,
      //抄送任务id 名字
      NameAddCcUrl: [],
      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,
      textarea1:'',
      customize_whether: [],//自定义是否
      ExecutionTask_OutType: [],// 执行日志导出类别

      SetMealImport: false, //详情查询 套餐信息
      show_ds: false, //详情查询
      selTime: '',//时间选择
      mainwidth: 24,//宽度
      option_show: false, //公司所属查询
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
      typeOptionsCcUrl: [],
      // 连接管理时间查询类型 数据字典
      timetypeHis: [],
      // 简要套餐名称
      packageOptions: [],

      //详情
      tablefrom: [],

      FlowData: [],
      selIccid: [{FlowData: ''}],
      activeNames: [0],
      //抄送状态
      CCstatus: [],
      //卡状态
      status_type: "",
      // 卡状态 字典
      stateOptions: [
        {
          dictLabel: ''
        }
      ],
      Cchis_result: [],
      //任务类别
      category_type: "",
      // 任务类别 字典
      TaskOptions: [],

      // 运营商类别 字典
      operators_type: [],
      // 执行日志状态 字典
      channelStatusOptions: [],
      // 执行日志编码 字典
      channelCodeOptions: [],
      // 表单参数
      form: {},
      defaultProps: {
        children: "children",
        label: "label"
      },

      //当前查询参数
      sel: {
        iccid: '',
        agent_id: '',
        package_id: '',
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
        ccstate: null,
        cc_result: null,
      },
      // 列信息
      columns: [
        {key: 1, label: `抄送任务id`, visible: true},
        {key: 2, label: `推送API`, visible: true},
        {key: 3, label: `推送邮件`, visible: true},
        {key: 4, label: `抄送状态`, visible: true},
        {key: 5, label: `抄送结果`, visible: true},
        {key: 6, label: `抄送次数`, visible: true},
        {key: 7, label: `备注`, visible: true},
        {key: 8, label: `抄送参数`, visible: true},
        {key: 9, label: `创建时间`, visible: true},
        {key: 10, label: `最近一次抄送时间`, visible: true},
      ],
      // 表单校验
      rules: {}
    };
  },

  created() {

    const cc_id = this.$route.params.id;
    console.log(cc_id)
    if(tools.Is_null(cc_id)){
      this.queryParams.value = cc_id;
      this.queryParams.type = '1';
    }

    //加载 查询条件
    if (window['typeOptionsCcUrl'] != undefined && window['typeOptionsCcUrl'] != null && window['typeOptionsCcUrl'] != '') {
      this.typeOptionsCcUrl = window['typeOptionsCcUrl'];
    } else {
      this.getDicts("yunze_typeOptionsCcUrl").then(response => {
        window['typeOptionsCcUrl'] = response.data;
        this.typeOptionsCcUrl = window['typeOptionsCcUrl'];
      });
    }

    //加载 抄送状态
    if (window['CCstatus'] != undefined && window['CCstatus'] != null && window['CCstatus'] != '') {
      this.CCstatus = window['CCstatus'];
    } else {
      this.getDicts("yz_cc_HisStatus").then(response => {
        window['CCstatus'] = response.data;
        this.CCstatus = window['CCstatus'];
      });
    }

    //加载 抄送结果
    if (window['Cchis_result'] != undefined && window['Cchis_result'] != null && window['Cchis_result'] != '') {
      this.Cchis_result = window['Cchis_result'];
    } else {
      this.getDicts("yz_cc_result").then(response => {
        window['Cchis_result'] = response.data;
        this.Cchis_result = window['Cchis_result'];
      });
    }

    //加载 时间赛选查询条件
    if (window['timetypeHis'] != undefined && window['timetypeHis'] != null && window['timetypeHis'] != '') {
      this.timetypeHis = window['timetypeHis'];
    } else {
      this.getDicts("yunze_CcHis_timetype").then(response => {
        window['timetypeHis'] = response.data;
        this.timetypeHis = window['timetypeHis'];
      });
    }


    //加载 执行日志导出类别
    if (window['ExecutionTask_OutType'] != undefined && window['ExecutionTask_OutType'] != null && window['ExecutionTask_OutType'] != '') {
      this.ExecutionTask_OutType = window['ExecutionTask_OutType'];
    } else {
      this.getDicts("yunze_ExecutionTask_OutType").then(response => {
        window['ExecutionTask_OutType'] = response.data;
        this.ExecutionTask_OutType = window['ExecutionTask_OutType'];
      });
    }

    //加载 抄送任务id 名字
    if (window['NameAddCcUrl'] != undefined && window['NameAddCcUrl'] != null && window['NameAddCcUrl'] != '') {
      this.NameAddCcUrl = window['NameAddCcUrl'];
    } else {
      NameAdd().then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        window['NameAddCcUrl'] = jsonobj.Data;
        this.NameAddCcUrl = window['NameAddCcUrl'];
      });
    }

    this.getTreeselect();
    this.getList();
  },
  methods: {

    // 树权限（父子联动）
    handleCheckedTreeConnect(value, type) {
      if (type == 'menu') {
        this.menuCheckStrictly = value ? true : false;
      } else if (type == 'dept') {
        this.$refs.dept.setCheckedNodes(false ? this.deptOptions : []);
        this.deptCheckStrictly = value ? true : false;

      }
    },

    CcPatr(row) {
      // console.log(row.cc_parameter)
      if (row.cc_parameter != null && row.cc_parameter != '') {
        this.title ="抄送参数";
        this.open = true;
          console.log(row.cc_parameter)
        this.textarea1 =  tools.formatJson(row.cc_parameter);
      } else {
        this.msgError("没有参数");
      }
    },

    /*获取查询参数*/
    getParams() {

      if (this.$refs.dept != undefined & this.$refs.dept != null & this.$refs.dept != 'undefined') {
        //console.log(this.$refs.dept.getCheckedKeys());
        if (this.$refs.dept.getCheckedKeys().length > 0) {
          this.queryParams.agent_id = this.$refs.dept.getCheckedKeys();
        }
      }
      if (tools.Is_null(this.queryParams.timetype) && this.selTime != null) {
        //console.log(this.selTime);
        this.queryParams.staTime = this.selTime[0];
        this.queryParams.endTime = this.selTime[1];
      }


    },
    /*下载执行日志*/
    getDownloadExecutionTask(row) {
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API + "/yunze/ExecutionTask/download?Pstr=" + Pwd_Str, '_blank');

    },

    //下载 xls 文件 格式
    getdownloadConversionk(row) {
      let map = {};
      map.path = row.value;
      map.token = getToken();
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      //console.log(map);
      window.open(process.env.VUE_APP_BASE_API + "/yunze/ExecutionTask/downloadConversion?Pstr=" + Pwd_Str, '_blank');

    },


    /** 查询执行日志列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      //console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      listCcHis(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          // console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.cardList = jsonobj.Data.Data;
            this.total = jsonobj.Data.Pu.rowCount;
          } else {
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;
        }
      );
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
    // 节点单击事件
    handleNodeClick(data) {
      //console.log(data);
      this.getList();
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
      this.queryParams.pageNum = 1;
      this.queryParams.pageSize = 10;
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
      this.form.cd_code = "YiDong_EC";
      this.form.cd_operator_type = "1";
      this.form.cd_lunxun = "1";
      this.form.cd_status = "1";
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

.el-table .cell {
  line-height: 17px;
  padding-left: 10px;
  padding-right: 0px;
}

.el-form-item {
  margin-bottom: 10px;
}

.my_checkbox__inner {
  border: 1px;
}

</style>

