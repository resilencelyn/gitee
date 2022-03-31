<template>
  <div class="app-container">
    <el-row :gutter="20">
      <!--部门数据-->
      <el-col :span="4" :xs="24" v-show="option_show">
        <div class="head-container">
          <el-input
            v-model="deptName"
            placeholder="请输入公司名称"
            clearable
            size="small"
            prefix-icon="el-icon-search"
            style="margin-bottom: 20px"
          />
        </div>
        <div class="head-container">
          <el-checkbox v-model="deptExpand" @change="handleCheckedTreeExpand($event, 'dept')">展开/折叠</el-checkbox>
          <el-checkbox v-model="deptNodeAll" @change="handleCheckedTreeNodeAll($event, 'dept')">全选/全不选</el-checkbox>
          <el-checkbox v-model="deptCheckStrictly" @change="handleCheckedTreeConnect($event, 'dept')">父子联动</el-checkbox>


          <el-tree
            :data="deptOptions"
            show-checkbox
            :filter-node-method="filterNode"
            default-expand-all
            node-key="id"
            ref="dept"
            highlight-current
            :check-strictly="!deptCheckStrictly"
            :props="defaultProps"/>


        </div>
      </el-col>
      <!--用户数据-->
      <el-col :span="mainwidth" :xs="24">
        <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">

          <!--任务类别   -->
          <el-form-item label="任务类别" prop="status">
            <el-select
              v-model="queryParams.taskType"
              placeholder="任务类别"
              clearable
              size="small"
              style="width: 110px"
            >
              <el-option
                v-for="dict in TaskOptions"
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
                v-for="dict in timetypeOptions"
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

        <el-form :model="queryParams" ref="queryForm" :inline="true" label-width="68px">
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
                <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
            </el-col>
            <el-col :span="1.5">
              <el-button size="mini" v-hasPermi="['yunze:card:listAgent']" @click="agentShow">公司所属</el-button>
            </el-col>
            <right-toolbar :showSearch.sync="showSearch" v-hasPermi="['yunze:executionTask:list']" @queryTable="getList"
                           :columns="columns"></right-toolbar>
          </el-row>
        </el-form>


        <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">

          <el-table-column
            label="任务名"
            v-if="columns[0].visible"
            min-width="120%">
            <template slot-scope="scope">
                <span @click="openExecutionTaskInfo(scope.row)" class="link-type"
                      v-hasPermi="['yunze:executionTask:download']">{{ scope.row.task_name }}</span>
            </template>
          </el-table-column>
<!--          <el-table-column label="创建时间" align="center" prop="create_time" v-if="columns[1].visible"/>-->
<!--          <el-table-column label="修改时间" align="center" prop="update_time" v-if="columns[2].visible"/>-->
          <el-table-column label="任务类别" align="center" prop="type" v-if="columns[1].visible">
            <template slot-scope="scope">
              {{ tools.getDkeyValue(TaskOptions, scope.row.type) }}
            </template>
          </el-table-column>
          <el-table-column label="开始时间" align="center" prop="start_time" v-if="columns[2].visible"/>
          <el-table-column label="结束时间" align="center" prop="end_time" v-if="columns[3].visible"/>
          <el-table-column label="创建人" align="center" prop="auth" v-if="columns[4].visible"/>

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


    <el-dialog :close-on-click-modal="false"
               title="下载执行日志结果" :visible.sync="executionTaskDis">
      <hr style="color:green"/>
      <el-row :gutter="20">
        <ul class="list-group" v-for="(item,index) in Urls">
          <li>
            <span @click="getDownloadExecutionTask(item)" class="link-type"
                  v-hasPermi="['yunze:executionTask:download']">{{ item.name }} CSV</span>

            <span style="margin-left: 20px;" @click="getdownloadConversionk(item)" class="link-type"
                  v-hasPermi="['yunze:executionTask:download']">{{ item.name }} XLS</span>
          </li>
        </ul>
      </el-row>
    </el-dialog>


  </div>
</template>

<script>
import {listExecutionTask, downloadExecutionTask} from "@/api/yunze/flowcard/ExecutionTask";
import {getToken} from "@/utils/auth";
import {treeselect} from "@/api/system/dept";
import Treeselect from "@riophae/vue-treeselect";
import "@riophae/vue-treeselect/dist/vue-treeselect.css";
import tools from "@/utils/yunze/tools";


export default {
  name: "executionTask",
  components: {Treeselect},
  data() {
    return {
      tools:tools,
      //保存 编辑 按钮
      executionTaskDis: false,
      Urls: [],

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


      deptCheckStrictly: true,
      deptExpand: true,
      deptNodeAll: false,

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
      typeOptions: [],
      // 连接管理时间查询类型 数据字典
      timetypeOptions: [],
      // 简要套餐名称
      packageOptions: [],

      //详情
      tablefrom: [],

      FlowData: [],
      selIccid: [{FlowData: ''}],
      activeNames: [0],

      //卡状态
      status_type: "",
      // 卡状态 字典
      stateOptions: [
        {
          dictLabel: ''
        }
      ],
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
      },
      // 列信息
      columns: [
        {key: 0, label: `任务名`, visible: true},
        {key: 1, label: `任务类别`, visible: true},
        {key: 2, label: `开始时间`, visible: true},
        {key: 3, label: `结束时间`, visible: true},
        {key: 4, label: `创建人`, visible: true},
      ],
      // 表单校验
      rules: {}
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

    const TaskId = this.$route.params.TaskId;

    //加载 查询条件
    if (window['executionTask_selType'] != undefined && window['executionTask_selType'] != null && window['executionTask_selType'] != '') {
      this.typeOptions = window['executionTask_selType'];
    } else {
      this.getDicts("executionTask_selType").then(response => {
        window['executionTask_selType'] = response.data;
        this.typeOptions = window['executionTask_selType'];
      });
    }

    //加载 任务类别
    if (window['TaskOptions'] != undefined && window['TaskOptions'] != null && window['TaskOptions'] != '') {
      this.TaskOptions = window['TaskOptions'];
    } else {
      this.getDicts("yunze_task_type").then(response => {

        window['TaskOptions'] = response.data;
        this.TaskOptions = window['TaskOptions'];
      });
    }

    //加载 时间赛选查询条件
    if (window['timetypeOptions'] != undefined && window['timetypeOptions'] != null && window['timetypeOptions'] != '') {
      this.timetypeOptions = window['timetypeOptions'];
    } else {
      this.getDicts("yunze_task_sel_timetype").then(response => {
        window['timetypeOptions'] = response.data;
        this.timetypeOptions = window['timetypeOptions'];
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

    this.getTreeselect();

    if (tools.Is_null(TaskId)) {
      this.queryParams.value = "" + TaskId;
      this.queryParams.type = '4';
      this.getList();
    } else {
      this.getList();
    }

  },
  methods: {


    //任务下载详情
    openExecutionTaskInfo(ExecutionTask) {
      // console.log(ExecutionTask.end_time)
      if (ExecutionTask.end_time) {
        let _this = this;
        _this.Urls = [];
        //console.log(ExecutionTask);
        let url = ExecutionTask.url;
        let reg = RegExp(/,/);
        let Url_list = [];
        //console.log(url.match(reg))
        //console.log(url.match(reg).index)
        if (url.match(reg)) {
          Url_list = url.split(",");
        } else {
          if (url != null) {
            Url_list.push(url);
          }
        }
        // console.log(Url_list)
        for (let i = 0; i < Url_list.length; i++) {
          let Url_map = {};
          let url_val = Url_list[i];
          for (let j = 0; j < ExecutionTask_OutType.length; j++) {
            let obj = ExecutionTask_OutType[j];
            let type = obj.dictValue;
            if (url_val.indexOf(type) != -1) {
              Url_map.name = obj.dictLabel;
            }
          }
          Url_map.value = url_val;
          _this.Urls.push(Url_map);
        }
        _this.executionTaskDis = true;
      } else {
        this.msgError("下载执行日志时间还没有结束");
      }

    },


    // 树权限（展开/折叠）
    handleCheckedTreeExpand(value, type) {
      if (type == 'dept') {
        let treeList = this.deptOptions;
        for (let i = 0; i < treeList.length; i++) {
          this.$refs.dept.store.nodesMap[treeList[i].id].expanded = value;
        }
      }
    },
    // 树权限（全选/全不选）
    handleCheckedTreeNodeAll(value, type) {
      if (type == 'dept') {
        //console.log(value);
        this.$refs.dept.setCheckedNodes(value ? this.deptOptions : []);
      }
    },
    // 树权限（父子联动）
    handleCheckedTreeConnect(value, type) {
      if (type == 'menu') {
        this.menuCheckStrictly = value ? true : false;
      } else if (type == 'dept') {
        this.$refs.dept.setCheckedNodes(false ? this.deptOptions : []);
        this.deptCheckStrictly = value ? true : false;

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
      listExecutionTask(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
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
    /*代理查询拓展*/
    agentShow() {
      let _this = this;
      _this.option_show = !_this.option_show;
      _this.mainwidth = _this.option_show ? 20 : 24;
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


    /*获取详细信息*/
    finddata(row) {
      this.form = {};//清空数据
      let map = {};
      map.cd_id = row.cd_id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      getexecutionTask(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.form = jsonobj.Data;
            this.show_ds = true;
          } else {
            this.msgError("获取数据异常，请联系管理员！");
          }
          this.loading = false;

        }
      );

    },


    /*获取字典数据中的匹对key value*/
    getDeptNamekeyValue(arr, key) {
      let value = '';
      key = "" + key + "";
      for (let i = 0; i < arr.length; i++) {
        let map = arr[i];
        if (map.dept_id == key) {
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

