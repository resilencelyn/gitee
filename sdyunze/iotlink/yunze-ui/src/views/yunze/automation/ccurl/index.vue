<template>
  <div class="app-container">
    <el-form :model="queryParams" ref="queryForm" :inline="true" v-show="showSearch" label-width="68px">
      <el-form-item label="推送 API" prop="url">
        <el-input
          v-model="queryParams.PushAPI"
          placeholder="请输入 API"
          clearable
          size="small"
          style="width: 240px"
          @keyup.enter.native="handleQuery"
        />
      </el-form-item>
      <el-form-item label="推送邮件" prop="email">
        <el-input
          v-model="queryParams.mail"
          placeholder="请输入邮件"
          clearable
          size="small"
          style="width: 240px"
          @keyup.enter.native="handleQuery"
        />
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
            v-for="dict in ccurltimetypeOptions"
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
      <el-form-item>
        <el-button type="primary" icon="el-icon-search" size="mini" @click="handleQuery">搜索</el-button>
        <el-button icon="el-icon-refresh" size="mini" @click="resetQuery">重置</el-button>
      </el-form-item>
    </el-form>

    <el-table v-loading="loading" :data="cardList" @selection-change="handleSelectionChange">
      <el-table-column type="selection" width="55" align="center" />
      <el-table-column   label="id" align="center"  prop="id" />
      <el-table-column   label="对应自动化规则表" align="center" key="cc_id" prop="cc_id">
      <template slot-scope="scope">
        {{tools.getkeyValue(NameAddCcUrl,scope.row.cc_id,"id","cc_name")}}
      </template>
      </el-table-column>
      <el-table-column   label="推送API" align="center"  prop="url" />
      <el-table-column   label="推送邮件" align="center"  prop="email" />
      <el-table-column   label="创建时间" align="center"  prop="create_time" />
      <el-table-column label="操作" align="center" class-name="small-padding fixed-width">
        <template slot-scope="scope">
          <el-button
            size="mini"
            type="text"
            icon="el-icon-edit"
            @click="handleUpdate(scope.row)"
          >修改</el-button>
          <el-button
            size="mini"
            type="text"
            icon="el-icon-delete"
            @click="handleDelete(scope.row)"
          >删除</el-button>
        </template>
      </el-table-column>
    </el-table>

    <pagination
      v-show="total>0"
      :total="total"
      :page.sync="queryParams.pageNum"
      :limit.sync="queryParams.pageSize"
      @pagination="getList"
    />

    <!-- 添加或修改岗位对话框 -->
    <el-dialog :title="title" :visible.sync="open" width="500px" append-to-body>
      <el-form ref="form" :model="form" :rules="rules" label-width="80px">
        <el-form-item label="推送API" prop="url">
          <el-input v-model="form.url" placeholder="请修改API" />
        </el-form-item>
        <el-form-item label="推送邮件" prop="email">
          <el-input v-model="form.email" placeholder="请修改邮件" />
        </el-form-item>
      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button type="primary" @click="submitForm">确 定</el-button>
        <el-button @click="cancel">取 消</el-button>
      </div>
    </el-dialog>
  </div>
</template>

<script>
import { NameAdd } from "@/api/yunze/automation/cc";
import { listCcUrl,UpdateCcUrl,deleteUrl } from "@/api/yunze/automation/ccurl";
import tools from "@/utils/yunze/tools";
export default {
  name: "Post",
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

      tools : tools,

      // 遮罩层
      loading: true,
      // 导出遮罩层
      exportLoading: false,
      // 选中数组
      ids: [],
      PushAPI: null,
      mail: null,
      // 非单个禁用
      single: true,
      // 非多个禁用
      multiple: true,
      // 显示搜索条件
      showSearch: true,
      // 总条数
      ccurltimetypeOptions: [],
      NameAddCcUrl: [],
      selTime:'',//时间选择
      total: 0,
      // 岗位表格数据
      cardList: null,
      // 弹出层标题
      title: "",
      // 是否显示弹出层
      open: false,
      // 状态数据字典
      statusOptions: [],
      // 查询参数
      queryParams: {
        pageNum: 1,
        pageSize: 10,
      },

      // 表单参数
      form: {
        cc_name: null,
      },
      // 表单校验
      rules: {
      }
    };
  },
  created() {
    this.getList();
    this.getDicts("sys_normal_disable").then(response => {
      this.statusOptions = response.data;
    });

    //加载 时间赛选查询条件
    if(window['ccurltimetypeOptions']!=undefined &&  window['ccurltimetypeOptions']!=null && window['ccurltimetypeOptions']!=''){
      this.ccurltimetypeOptions = window['ccurltimetypeOptions'];
    }else{
      this.getDicts("yunze_task_sel_timetype").then(response => {
        window['ccurltimetypeOptions'] = response.data;
        this.ccurltimetypeOptions = window['ccurltimetypeOptions'];
      });
    }



    //加载 执行任务导出类别
    if(window['ExecutionTask_OutType']!=undefined &&  window['ExecutionTask_OutType']!=null && window['ExecutionTask_OutType']!=''){
      this.ExecutionTask_OutType = window['ExecutionTask_OutType'];
    }else{
      this.getDicts("yunze_ExecutionTask_OutType").then(response => {
        window['ExecutionTask_OutType'] = response.data;
        this.ExecutionTask_OutType = window['ExecutionTask_OutType'];
      });
    }

    //加载 对应自动化规则表 名字
    if(window['NameAddCcUrl']!=undefined &&  window['NameAddCcUrl']!=null && window['NameAddCcUrl']!=''){
      this.NameAddCcUrl = window['NameAddCcUrl'];
    }else{
      NameAdd().then(response => {
        let jsonobj =  JSON.parse(tools.Decrypt(response));
        window['NameAddCcUrl'] = jsonobj.Data;
        this.NameAddCcUrl = window['NameAddCcUrl'];
      });
    }



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

    /** 查询执行任务列表 */
    getList() {
      this.loading = true;
      this.getParams();
      this.queryParams.page = 1;
      //console.log(this.queryParams);
      let Pwd_Str = tools.encrypt(JSON.stringify(this.queryParams));
      //console.log(Pwd_Str);
      listCcUrl(Pwd_Str).then(response => {
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
      this.reset();
    },
    // 表单重置
    reset() {
      this.form = {
        cc_id: undefined,
        id: undefined,
        remark: undefined,
        postSort: 0,
        status: "0",
        email: undefined,
        url: undefined
      };
      this.resetForm("form");
    },
    /** 搜索按钮操作 */
    handleQuery() {
      this.queryParams.pageNum = 1;
      this.getList();
    },
    /** 重置按钮操作 */
    resetQuery() {
      this.resetForm("queryForm");
      this.handleQuery();
    },
    /** 修改按钮操作 */
    handleUpdate(row) {
      this.reset();
      this.form = row;
      this.open = true;
      this.title = "修改推送信息"
    },
    //确定按钮
    submitForm(){
      let _this = this;


      if(tools.Is_null(_this.form.url) == true || tools.Is_null(_this.form.email) == true ){
        let Pwd_Str = tools.encrypt(JSON.stringify(this.form));
        UpdateCcUrl(Pwd_Str).then(response => {
          let jsonobj = JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          //console.log(jsonobj);
          if (jsonobj.code == 200) {
            this.msgSuccess(jsonobj.msg);
            this.open = false;
            //更新数据
            this.queryParams.page = 1;
            this.getList();
          }else{
            let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
            this.msgError(msg);
          }
        })

    }else {
        tools.open(this, "必须修改其中一个")
      }
    },
    // 多选框选中数据
    handleSelectionChange(selection) {
      this.ids = selection.map(item => item.id)
      this.single = selection.length!=1
      this.multiple = !selection.length
    },
    // this.msgSuccess("删除成功");
    /** 删除按钮操作 */
    handleDelete(row) {
      let map = {};
      map.id = row.id;
      map.cc_id = row.cc_id;
      let Pwd_Str = tools.encrypt(JSON.stringify(map));
      tools.openAsk(this,'warning', "确定要删除自动化规则表为 '"+row.id+"' 的这条数据吗？", this.deleteCcUrl, Pwd_Str);
    },
    //删除自动化配置
    deleteCcUrl(Pwd_Str){
      deleteUrl(Pwd_Str).then(response => {
        let jsonobj = JSON.parse(tools.Decrypt(response));
        console.log(jsonobj);
        if (jsonobj.code == 200) {
          this.getList();
          this.msgSuccess(jsonobj.msg);
        }else{
          let msg = jsonobj.Data!=null && jsonobj.Data!=''?jsonobj.Data:jsonobj.msg;
          this.msgError(msg);
        }
      })
    },


  }
};
</script>
