<template>
  <div class="app-container">


    <el-main style="padding-top: 2%;padding-left: 3%">

      <div >


        <el-divider content-position="left"> 物联卡智能诊断</el-divider>
      </div>

      <el-row>
        <el-col :span="24">

          <div style="width: 100%;">
            <el-input style="width: 60%;float: left;padding: 0px 30px;" v-model="cardNo"
                      placeholder="请输入 MSISDN、ICCID、IMSI、Vid(虚拟编号)">
              <el-select
                v-hasPermi="['yunze:card:cardMatch']"
                v-model="selType"
                placeholder="号码类型"
                slot="prepend"
                size="small"
                style="width: 120px"
              >
                <el-option
                  v-for="dict in cardNumber"
                  :key="dict.dictValue"
                  :label="dict.dictLabel"
                  :value="dict.dictValue"
                />
              </el-select>
            </el-input>
            <el-button type="primary" v-show="btnDiagnosis" v-hasPermi="['yunze:card:diagnosis']" :disabled="disabledDiagnosis" @click="oneClickDiagnosis">一 键 诊 断</el-button>
            <el-button type="primary" v-show="btnDiagnosis" v-hasPermi="['yunze:card:cardMatch']" :disabled="disabledDiagnosis" @click="oneClickMatch">智 能 匹 对</el-button>
          </div>


          <el-divider v-if="resultShow" content-position="left">匹对结果</el-divider>

          <div  v-if="resultShow">
            <div>
              <span style="margin-left: 40px;"> 匹对开头：</span>
              <span class="my_cloor_red" style="font-size: 25px;">{{cardPrefix}}</span><span style="font-size: 25px;">{{cardSuffix}}</span>
              <span style="margin-left: 40px;">共 <span>{{cardCount}}</span> 条数据</span>
            </div>
            <div style="margin-top: 10px;">
              <span style="margin-left: 40px;"> 参考卡号：</span><span style="font-size: 25px;color: #00ba26">{{cardReference}}</span>
            </div>
          </div>




          <el-divider v-if="cardInfoShow" content-position="left">{{cardInfoTitle}}</el-divider>

          <el-table
            v-if="cardInfoShow"
            :data="tablefrom"
            :show-header="false"
            border>

            <el-table-column prop="label1" >
              <template slot-scope="scope">
                <b>
                  <div :style="{'color':scope.row.label1? 'black':'black'}">
                    {{scope.row.label1}}
                  </div>
                </b>
              </template>
            </el-table-column>


            <el-table-column
              prop="value1"
            />
            <el-table-column prop="label2" >
              <template slot-scope="scope">
                <b>
                  <div :style="{'color':scope.row.label2? 'black':'black'}">
                    {{scope.row.label2}}
                  </div>
                </b>
              </template>
            </el-table-column>
            <el-table-column
              prop="value2"
            />
            <el-table-column prop="label3" >
              <template slot-scope="scope">
                <b>
                  <div :style="{'color':scope.row.label3? 'black':'black'}">
                    {{scope.row.label3}}
                  </div>
                </b>
              </template>
            </el-table-column>
            <el-table-column
              prop="value3"
            />
          </el-table>

          <el-table
            v-if="cardInfoShow"
            :data="tablefrom_remarks"
            :show-header="false"
            border>
            <el-table-column prop="label1" width="285px">
              <template slot-scope="scope">
                <b>
                  <div :style="{'color':scope.row.label1? 'black':'black'}">
                    {{scope.row.label1}}
                  </div>
                </b>
              </template>
            </el-table-column>
            <el-table-column
              prop="value1"
            />
          </el-table>
          <br><br>

          <el-divider v-if="cardOnlineShow" content-position="left">{{OnlineTitle}}   <!--【{{form.}}}】-->
            <el-button @click="getOnlineStatus" type="primary" style="margin-left: 40px" size="mini">刷新</el-button>
          </el-divider>


          <el-table v-if="cardOnlineShow"
            :data="table_Online"
            :show-header="false"
            border>
            <el-table-column prop="label1" >
              <template slot-scope="scope">
                <b>
                  <div :style="{'color':scope.row.label1? 'black':'black'}">
                    {{scope.row.label1}}
                  </div>
                </b>
              </template>
            </el-table-column>
            <el-table-column
              prop="value1"
            />
            <el-table-column prop="label2" >
              <template slot-scope="scope">
                <b>
                  <div :style="{'color':scope.row.label2? 'black':'black'}">
                    {{scope.row.label2}}
                  </div>
                </b>
              </template>
            </el-table-column>
            <el-table-column
              prop="value2"
            />
          </el-table>

          <br><br>

          <el-divider  v-if="cardChangeShow"  content-position="left">卡信息变更记录</el-divider>

          <el-steps v-if="cardChangeShow" align-center :active="cardChangeList.length"
          >
            <el-step :title="item.createTime"
                     :description="item.remark+'\n'+'变更前 '+item.cbefore+' 变更后 '+item.cafterward"
                     v-for="item in  cardChangeList">

              <h4>{{item.remark}}</h4>remark
              <p>{{}}</p>


            </el-step>

          </el-steps>


          <el-divider v-if="cardFlowShow" content-position="left">资费计划信息</el-divider>

          <el-collapse v-if="cardFlowShow" v-model="activeNames" style="margin-left: 10px; margin-top: 10px;">
            <el-collapse-item v-for="(item,index) in selIccid" :key="index" :name="index">
              <template slot="title" class="item_iccid" :data-val="item.packet_id">
                <div style="font-size:20px;">
                  {{item.iccid}} &nbsp;&nbsp;&nbsp;
                  <span style="color:#03a9f4;">
                    <span v-if="tools.Is_null(sel.package_id)">
                       {{tools.getkeyValue(packageOptions,sel.package_id,"package_id","package_agentname")}}
                    </span>
                     <span v-if="tools.Is_null(sel.package_id)==false">
                      未划分资费组！
                    </span>
                  </span>
                </div>
              </template>
              <div>
                <div>
                  <el-button @click="loadCar(item,index)" type="primary" style="margin-left: 3px">加载生效中资费计划</el-button>
                  <el-button @click="loadCar(item,index,1)" style="margin-left: 3px">加载所有资费计划</el-button>
                </div>
                <el-table
                  :row-style="{height:'5px'}"
                  :cell-style="{height:'5px,0'}"
                  :height="500"
                  :header-cell-style="{color:'#606266'}"
                  border
                  :data="item.FlowData"
                  style="width: 100%;margin-top: 1%">
                  <el-table-column
                    type="selection"
                    min-width="20%">
                  </el-table-column>
                  <el-table-column type="expand">
                    <template slot-scope="scope">
                      <el-table
                        :row-style="{height:'5px'}"
                        :cell-style="{height:'5px,0'}"
                        :height="400"
                        :header-cell-style="{color:'#606266'}"
                        border
                        :key="scope.row.index"
                        :data="scope.row.data"
                        style="width: 100%;margin-top: 1%">
                        <el-table-column
                          type="selection"
                          min-width="20%">
                        </el-table-column>
                        <el-table-column
                          label="套餐流量(MB)"
                          prop="true_flow">
                        </el-table-column>
                        <el-table-column
                          label="生效时间"
                          prop="start_time">
                        </el-table-column>
                        <el-table-column
                          label="到期时间"
                          prop="end_time">
                        </el-table-column>
                        <el-table-column
                          label="创建时间"
                          prop="create_time">
                        </el-table-column>
                        <el-table-column
                          label="生效类型"
                          prop="ord_type">
                          <template slot-scope="scope">
                            {{tools.getDkeyValue(validateTypeOptions,scope.row.ord_type)}}
                          </template>
                        </el-table-column>
                        <el-table-column
                          label="状态"
                          width="100px"
                          prop="status">
                          <template slot-scope="scope">
                            <span :class="getColor(''+scope.row.status)"> {{tools.getDkeyValue(cardValidType,""+scope.row.status)}}</span>
                          </template>
                        </el-table-column>
                        <el-table-column
                          label="已用流量(MB)"
                          prop="use_so_flow">
                        </el-table-column>

                      <!--  <el-table-column label="操作" width="120">
                          <template slot-scope="scope">
                            <el-dropdown @command="handlerCommand" >
                              <el-button type="primary">
                                操作<i class="el-icon-arrow-down el-icon&#45;&#45;right"></i>
                              </el-button>
                              <el-dropdown-menu slot="dropdown" >
                                &lt;!&ndash;生效中套餐才可退订&ndash;&gt;
                                <el-dropdown-item  v-if="scope.row.status==1"
                                                  :command="beforeHandleCommand(scope,'returnpa',item,index)" :key="0">
                                  退订套餐
                                </el-dropdown-item>
                                <el-dropdown-item
                                                  v-if="scope.row.ord_no.substr(0,15)!='zy-ord-initflow'"
                                                  :command="beforeHandleCommand(scope,'updpackagetime',item,index,'next')"
                                                  :key="1">当月生效转次月生效
                                </el-dropdown-item>
                                <el-dropdown-item
                                                  v-if="scope.row.ord_no.substr(0,15)!='zy-ord-initflow'"
                                                  :command="beforeHandleCommand(scope,'updpackagetime',item,index,'month')"
                                                  :key="2">次月生效转当月生效
                                </el-dropdown-item>

                              </el-dropdown-menu>
                            </el-dropdown>

                          </template>
                        </el-table-column>-->
                      </el-table>
                    </template>
                  </el-table-column>

                  <el-table-column
                    label="订单号"
                    prop="ord_no">
                  </el-table-column>
                  <el-table-column
                    label="充值流量包"
                    prop="packet_wx_name">
                  </el-table-column>
                  <el-table-column
                    label="订单名称"
                    prop="ord_name">
                  </el-table-column>
                  <el-table-column
                    width="100px"
                    label="支付方式"
                    prop="pay_type">
                    <template slot-scope="scope">
                      {{tools.getDkeyValue(Payment_method,scope.row.pay_type)}}
                    </template>
                  </el-table-column>
                  <!--<el-table-column
                    width="60px"
                    label="已分润"
                    prop="is_profit">
                    <template slot-scope="scope">
                      <div v-if="scope.row.is_profit===0">否</div>
                      <div v-if="scope.row.is_profit===1">是</div>
                    </template>
                  </el-table-column>-->
                  <el-table-column
                    label="交易时间"
                    prop="orderCreate_time">
                  </el-table-column>
                </el-table>
              </div>
            </el-collapse-item>
          </el-collapse>



        </el-col>

      </el-row>
      <el-empty v-show="showempty" description="未匹配到相应的数据，请重新输入！"></el-empty>
    </el-main>

  </div>
</template>

<script>
  import tools from "@/utils/yunze/tools";
  import {
    getCard,
    queryflowSimple,
    queryOnlineStatus,
    queryPackageSimple,
    queryflowData,
    cardMatch,
    simStopReason,
    onOffStatus,
    apnInfo,
    cardBindStatus,
    simChangeHistory,
    findCr,
    getIccid
  } from "@/api/yunze/flowcard/card/card";
  import { getselTime } from "@/api/yunze/flowcard/card/change";


  export default {
    name: "diagnosis",
    data() {
      return {
        tools:tools,
        showempty: false, //空状态 显示
        cardInfoShow: false, //卡基础信息 显示
        resultShow: false, //匹对结果 显示
        cardNo: '',//匹对卡号
        selType:'2',//查询类型
        cardOnlineShow: false, //在线信息 显示
        cardFlowShow: false, //资费详情 显示
        cardChangeShow: false, //变更记录 显示
        cardChangeList: [],//卡状态变更数组
        cardNo: '',//诊断卡号
        btnDiagnosis:true,//诊断按钮是否显示
        disabledDiagnosis:false,//诊断按钮是否 禁用
        internalBool:false,//内部人员
        Payment_method: [], // 支付方式
        orderTypeOptions: [],// 订单交易类型 字典
        packageOptions: [], // 简要套餐名称

        findCr_RouteOptions: [],// API通道信息
        customize_whether: [],//自定义是否

        //当前查询参数
        sel:{
          iccid:'',
          agent_id:'',
          package_id:'',
        },

        show_ds: false, //详情查询
        selTime: '',//时间选择
        mainwidth: 24,//宽度
        option_show: false, //公司所属查询
        // 遮罩层
        loading: true,
        // 导出遮罩层
        exportLoading: false,

        dateRange: [],
        // 状态数据字典
        typeOptions: [],
        // 连接管理时间查询类型 数据字典
        timetypeOptions: [],
        PacketOptions: [],//资费计划
        PacketSelOptions: [],//资费计划查询

        //详情
        tablefrom: [],

        FlowData: [],
        selIccid: [{FlowData: []}],
        activeNames: [0],

        //卡状态所属通道

        status_type: "",
        //断开网状态
        cardConnectionTypeOptions:[],
        // 卡状态 字典
        stateOptions: [
          {
            dictLabel: ''
          }
        ],
        // 卡状态
        stateShowId: [],
        packet: {
          packet_flow: '-',
          packet_valid_time: '-',
          packet_valid_name: '-'
        },
        cardNumber: "",//卡号类别


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
        // 移动 归属地 字典
        YDcard_attributionOptions: [],

        // 列信息
        columns: [
          {key: 0, label: `任务名`, visible: true},
          {key: 1, label: `创建时间`, visible: true},
          {key: 2, label: `修改时间`, visible: true},
          {key: 3, label: `开始时间`, visible: true},
          {key: 4, label: `结束时间`, visible: true},
          {key: 5, label: `创建人`, visible: true},
        ],
        table_Online: [],//在线状态信息
        DeptOptions: [],// 公司信息
        RouteOptions: [],// API通道信息
        tablefrom_remarks: [],//备注信息
        cardInfoTitle: '卡基础信息 (信息同步中)',
        cardPackageTitle: '资费信息 (信息同步中)',
        OnlineTitle: '在线信息 (信息同步中)',
        // Changequery: '卡信息变更记录查询记录',

        access_type: [],//卡板接入方式
        online_status: [],//卡板在线状态

        DxCMP_netModel_type: [],//电信CMP无线接入模式
        DxCMP_RAT: [],//电信CMP接入类型
        DxCMP_online: [],//电信CMP在线状态

        card_types: [],//卡类型
        network_types: [],//网络类型
        validateTypeOptions: [],//生效类型
        cardValidType: [ ], // 资费计划状态 字典
      };
    },


    created() {
      let str = this.$route.params.Pstr;

      //加载 卡板接入方式
      if(window['access_type']!=undefined &&  window['access_type']!=null && window['access_type']!=''){
        this.access_type = window['access_type'];
      }else{
        this.getDicts("yunze_card_access_type").then(response => {
          window['access_type'] = response.data;
          this.access_type = window['access_type'];
        });
      }


      //加载 卡号类别
      if(window['cardNumber']!=undefined &&  window['cardNumber']!=null && window['cardNumber']!=''){
        this.cardNumber = window['cardNumber'];
      }else{
        this.getDicts("cardNumber").then(response => {
          window['cardNumber'] = response.data;
          this.cardNumber = window['cardNumber'];
        });
      }



      //加载 资费组名称
      if(window['packageOptions']!=undefined &&  window['packageOptions']!=null && window['packageOptions']!=''){
        this.packageOptions = window['packageOptions'];
        if(!tools.IsArrInside(this.packageOptions,"package_id","IsNull")){
          this.packageOptions.push({"package_id":"IsNull","package_agentname":"未划分资费"});
        }
      }else{
        queryPackageSimple().then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          window['packageOptions'] = jsonobj.Data;
          this.packageOptions = window['packageOptions'];
          if(!tools.IsArrInside(this.packageOptions,"package_id","IsNull")){
            this.packageOptions.push({"package_id":"IsNull","package_agentname":"未划分资费"});
          }
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



      //加载 API通道名称
      if(window['RouteOptions']!=undefined &&  window['RouteOptions']!=null && window['RouteOptions']!=''){
        this.RouteOptions = window['RouteOptions'];
        if(!tools.IsArrInside(this.RouteOptions,"cd_id","IsNull")){
          this.RouteOptions.push({"cd_id":"IsNull","cd_alias":"未划分通道"});
        }
      }else{
        this.findSp().then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          window['RouteOptions'] = jsonobj.Data;
          this.RouteOptions = window['RouteOptions'];
          if(!tools.IsArrInside(this.RouteOptions,"cd_id","IsNull")){
            this.RouteOptions.push({"cd_id":"IsNull","cd_alias":"未划分通道"});
          }
        });
      }




      //加载 自定义 是否 0 1 否 是
      if (window['customize_whether'] != undefined && window['customize_whether'] != null && window['customize_whether'] != '') {
        this.customize_whether = window['customize_whether'];
      } else {
        this.getDicts("yunze_customize_whether").then(response => {
          window['customize_whether'] = response.data;
          this.customize_whether = window['customize_whether'];
        });
      }





      //加载 运营商类型
      if(window['operators_type']!=undefined &&  window['operators_type']!=null && window['operators_type']!=''){
        this.operators_type = window['operators_type'];
      }else{
        this.getDicts("yunze_card_operators_type").then(response => {
          window['operators_type'] = response.data;
          this.operators_type = window['operators_type'];
        });
      }

      //加载 卡状态描述
      if(window['stateOptions']!=undefined &&  window['stateOptions']!=null && window['stateOptions']!=''){
        this.stateOptions = window['stateOptions'];
      }else{
        this.getDicts("yunze_card_status_type").then(response => {
          window['stateOptions'] = response.data;
          this.stateOptions = window['stateOptions'];
        });
      }

      //加载 卡状态
      if(window['stateShowId']!=undefined &&  window['stateShowId']!=null && window['stateShowId']!=''){
        this.stateShowId = window['stateShowId'];
      }else{
        this.getDicts("yunze_card_status_ShowId").then(response => {
          window['stateShowId'] = response.data;
          this.stateShowId = window['stateShowId'];
        });
      }

      //加载  断开网状态
      if(window['cardConnectionTypeOptions']!=undefined &&  window['cardConnectionTypeOptions']!=null && window['cardConnectionTypeOptions']!=''){
        this.cardConnectionTypeOptions = window['cardConnectionTypeOptions'];
      }else{
        this.getDicts("yz_cardConnection_type").then(response => {
          window['cardConnectionTypeOptions'] = response.data;
          this.cardConnectionTypeOptions = window['cardConnectionTypeOptions'];
        });
      }


      // 支付方式：
      if(window['Payment_method']!=undefined &&  window['Payment_method']!=null && window['Payment_method']!=''){
        this.Payment_method = window['Payment_method'];
      }else{
        this.getDicts("yunze_card_pay_type").then(response => {
          window['Payment_method'] = response.data;
          this.Payment_method = window['Payment_method'];
        });
      }

      //加载 资费计划生效类别
      if(window['validateTypeOptions']!=undefined &&  window['validateTypeOptions']!=null && window['validateTypeOptions']!=''){
        this.validateTypeOptions = window['validateTypeOptions'];
      }else{
        this.getDicts("yunze_card_takeEffect_type").then(response => {
          window['validateTypeOptions'] = response.data;
          this.validateTypeOptions = window['validateTypeOptions'];
        });
      }


      //加载 电信CMP无线接入模式
      if(window['DxCMP_netModel_type']!=undefined &&  window['DxCMP_netModel_type']!=null && window['DxCMP_netModel_type']!=''){
        this.DxCMP_netModel_type = window['DxCMP_netModel_type'];
      }else{
        this.getDicts("DianXin_CMP_netModel").then(response => {
          window['DxCMP_netModel_type'] = response.data;
          this.DxCMP_netModel_type = window['DxCMP_netModel_type'];
        });
      }

      //加载 电信CMP接入类型
      if(window['DxCMP_RAT']!=undefined &&  window['DxCMP_RAT']!=null && window['DxCMP_RAT']!=''){
        this.DxCMP_RAT = window['DxCMP_RAT'];
      }else{
        this.getDicts("DianXin_CMP_RAT").then(response => {
          window['DxCMP_RAT'] = response.data;
          this.DxCMP_RAT = window['DxCMP_RAT'];
        });
      }

      //加载 电信CMP在线状态
      if(window['DxCMP_online']!=undefined &&  window['DxCMP_online']!=null && window['DxCMP_online']!=''){
        this.DxCMP_online = window['DxCMP_online'];
      }else{
        this.getDicts("DianXin_CMP_online").then(response => {
          window['DxCMP_online'] = response.data;
          this.DxCMP_online = window['DxCMP_online'];
        });
      }

      //加载 移动 归属地
      if(window['YDcard_attributionOptions']!=undefined &&  window['YDcard_attributionOptions']!=null && window['YDcard_attributionOptions']!=''){
        this.YDcard_attributionOptions = window['YDcard_attributionOptions'];
      }else{
        this.getDicts("yz_YDcard_attribution").then(response => {
          window['YDcard_attributionOptions'] = response.data;
          this.YDcard_attributionOptions = window['YDcard_attributionOptions'];
        });
      }



      //加载 卡板在线状态
      if(window['online_status']!=undefined &&  window['online_status']!=null && window['online_status']!=''){
        this.online_status = window['online_status'];
      }else{
        this.getDicts("yunze_card_online_status").then(response => {
          window['online_status'] = response.data;
          this.online_status = window['online_status'];
        });
      }

      if(str!=null && str!='' && str.length>0){
        var reg_1 = new RegExp( "%2F" , "g" );
        var reg_2 = new RegExp( " " , "g" );
        str = str.replace(reg_1, "/");//转义 /
        str = str.replace(reg_2, "+");//转义 [ ]  》 +

        let jsonobj =  JSON.parse(tools.Decrypt(str));
        if(jsonobj!=null ){
          this.cardNo = jsonobj.cardNo;
        }
        this.oneClickDiagnosis();
      }

    },
    methods: {


      //获取对应 class 颜色
      getColor(key){
        var class_color="";
        switch (key) {
          case '1':
            class_color='my_cloor_green';
            break;
          case '0':
            class_color='my_cloor_black';
            break;
          case '2':
            class_color='my_cloor_blue';
            break;
          default:
            class_color='my_cloor_black';
            break;
        }
        return class_color;
      },

      // 一键诊断
      oneClickDiagnosis() {
        let _this = this;

        if(_this.disabledDiagnosis == false){
          _this.btnDiagnosis = false;
          _this.disabledDiagnosis = true;

          if(tools.Is_null(_this.cardNo)){
            _this.showempty = false; //空状态 显示
            _this.cardInfoShow = false; //卡基础信息 显示
            _this.cardOnlineShow = false; //在线信息 显示
            _this.cardFlowShow = false; //资费详情 显示
            _this.cardChangeShow = false; //变更记录 显示

            this.sel ={
              iccid:'',
              agent_id:'',
              package_id:'',
            };
            this.form = {};

            let map ={};
            map.type = '1';
            map.value = _this.cardNo;
            let Pwd_Str = tools.encrypt(JSON.stringify(map));
            getIccid(Pwd_Str).then(response => {
              let jsonobj = JSON.parse(tools.Decrypt(response));
              //console.log(jsonobj);
              if (jsonobj.code == 200) {
                let row = jsonobj.Data;
                this.internalBool =  jsonobj.Data.is_Internal;
                this.cardInfoTitle = "卡基础信息 (信息同步中)";
                this.cardPackageTitle = "资费信息 (信息同步中)";
                this.OnlineTitle = '在线信息 (信息同步中)';

                if(this.internalBool) {
                  //加载 API通道名称
                  if (window['findCr_RouteOptions'] != undefined && window['RouteOptions'] != null && window['findCr_RouteOptions'] != '') {
                    this.findCr_RouteOptions = window['findCr_RouteOptions'];
                  } else {
                    findCr().then(response => {
                      let jsonobj = JSON.parse(tools.Decrypt(response));
                      window['findCr_RouteOptions'] = jsonobj.Data;
                      this.findCr_RouteOptions = window['findCr_RouteOptions'];
                    });
                  }

                }


                this.finddata(row,'diagnosis');
                this.sel.iccid = row.iccid;
                this.getOnlineStatus();
                this.Change(row.iccid);


                _this.btnDiagnosis = true;
                setTimeout(function(){
                  _this.disabledDiagnosis = false;
                },3000);// 3 S 内禁止再次点击
              }else{
                _this.showempty = true;
                _this.btnDiagnosis = true;
                _this.disabledDiagnosis = false;
              }
            })
          }else {
            tools.MessageShow(_this,"请输入需要诊断的 MSISDN、ICCID、IMSI、Vid(虚拟编号) 信息！！","error");
            _this.btnDiagnosis = true;
            _this.disabledDiagnosis = false;
          }
        }else{
          tools.MessageShow(_this,"请勿重复点击！3秒后重试。","error");
        }
      },



      Change(iccid) {
        this.loading = true;
        let Pwd_Str = tools.encrypt(JSON.stringify({'iccid':iccid}));
        getselTime(Pwd_Str).then(response => {
            let jsonobj =  JSON.parse(tools.Decrypt(response));
            if(jsonobj.code=='200'){
              let cardLists = jsonobj.Data;
              ///console.log(cardLists);
              let arr = [];
              for (let i = 0; i < cardLists.length; i++) {
                arr.push(cardLists[cardLists.length-i-1]);
              }
              this.cardChangeList = arr;
              this.cardChangeShow = true; //变更记录 显示
            }else{
              this.msgError("获取数据异常，请联系管理员！");
            }

          }
        );
      },



      // 表单重置
      reset() {
        this.form = {
          userId: undefined,
          deptId: undefined,
          userName: undefined,
          nickName: undefined,
          password: undefined,
          phonenumber: undefined,
          email: undefined,
          sex: undefined,
          status: "0",
          remark: undefined,
          postIds: [],
          roleIds: []
        };
        this.resetForm("form");
      },

      /*获取单卡详细信息*/
      finddata(row,type){
        this.tablefrom = [];//清空数据
        this.tablefrom_remarks = [];//清空数据
        this.reset();
        let map ={};
        map.iccid = row.iccid;
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        getCard(Pwd_Str).then(response => {
            let jsonobj =  JSON.parse(tools.Decrypt(response));
            //console.log(jsonobj);
            if(jsonobj.code==200){
              this.form = jsonobj.Data;

              //详细套餐信息
              if(type=='diagnosis'){
                let map = {};
                map.iccid = this.form.iccid;
                map.$index = 0;
                map.NotExpired = 1;
                map.package_id = this.form.package_id;
                map.agent_id = this.form.agent_id;
                this.sel.agent_id = map.agent_id;
                this.sel.package_id = map.package_id;
                this.queryflow(map);
              }

              this.form.status_id = ""+this.form.status_id+"";
              let map1 = {};
              map1.label1 = "MSISDN";
              map1.value1 = jsonobj.Data.msisdn;
              map1.label2 = "ICCID";
              map1.value2 = jsonobj.Data.iccid;
              map1.label3 = "IMSI";
              map1.value3 = jsonobj.Data.imsi;
              this.tablefrom.push(map1);
              let map2 = {};
              //console.log(this.operators_type);
              map2.label1 = "IMEI";
              map2.value1 = jsonobj.Data.imei;
              map2.label2 = "运营商";
              if(tools.Is_null(jsonobj.Data.cd_operator_type)){
                map2.value2 = tools.getDkeyValue(this.operators_type,jsonobj.Data.cd_operator_type);
              }else{
                map2.value2 = "暂未划分API通道!";
              }
              if(this.internalBool){
                if(map2.value2=='移动' && jsonobj.Data.iccid.length>12) {
                  let subStr = jsonobj.Data.iccid.substring(8, 12);
                  let attributionCode = subStr.substring(0, 2);
                  let YY = subStr.substring(2, subStr.length);
                  let attribution = tools.getDkeyValue(this.YDcard_attributionOptions, attributionCode);
                  map2.value2 += " [归属地：" + attribution + "] [ " + YY + " 年开卡] ";
                }
              }

              map2.label3 = "卡状态";
              map2.value3 = tools.getDkeyValue(this.stateShowId,jsonobj.Data.status_ShowId);

              this.tablefrom.push(map2);
              let map3 = {};
              map3.label1 = "GPRS";
              map3.value1 = tools.getDkeyValue(this.card_types,jsonobj.Data.gprs);
              map3.label2 = "卡类型";
              map3.value2 = tools.getDkeyValue(this.card_types,jsonobj.Data.type);
              map3.label3 = "网络类型";
              map3.value3 = tools.getDkeyValue(this.network_types,jsonobj.Data.network_type);
              this.tablefrom.push(map3);

              queryflowSimple(Pwd_Str).then(res => {
                  let jBj =  JSON.parse(tools.Decrypt(res));
                  //console.log(jBj);
                  //jBj.Data =

                  let map4 = {};
                  map4.label1 = "短信功能";
                  map4.value1 = tools.getDkeyValue(this.customize_whether,jsonobj.Data.is_sms);
                  map4.label2 = "短信服务号码";
                  map4.value2 = jsonobj.Data.sms_number;
                  map4.label3 = "计费";
                  map4.value3 = jBj.Data!=undefined && jBj.Data!=null?jBj.Data.packet_wx_name:'未订购';
                  this.tablefrom.push(map4);


                  let map5 = {};
                  map5.label1 = "激活时间";
                  map5.value1 = jsonobj.Data.activate_date;
                  map5.label2 = "到期时间";
                  map5.value2 = jBj.Data!=undefined && jBj.Data!=null?jBj.Data.end_time:'';
                  map5.label3 = jBj.Data!=undefined && jBj.Data!=null?"服务周期  ( "+jBj.Data.packet_valid_name+" ) ":'服务周期';
                  map5.value3 = jBj.Data!=undefined && jBj.Data!=null?jBj.Data.packet_valid_time:'';
                  this.tablefrom.push(map5);
                  let map7 = {};
                  let channelname = "";
                  // console.log(this.internalBool)
                  if(this.internalBool){
                    channelname = tools.getDkeyValue(this.findCr_RouteOptions,''+jsonobj.Data.channel_id);
                  }else{
                    channelname = tools.getkeyValue(this.RouteOptions,''+jsonobj.Data.channel_id,"cd_id","cd_alias");
                  }
                  map7.label1 = "分组";
                  map7.value1 = jsonobj.Data!=undefined && jsonobj.Data!=null?jsonobj.Data.customize_grouping:'';
                  map7.label2 = "所属通道";
                  map7.value2 = channelname;
                  map7.label3 = "断开网状态";
                  map7.value3 = jsonobj.Data.connection_status!=null?tools.getDkeyValue(cardConnectionTypeOptions,''+jsonobj.Data.connection_status):"";
                  this.tablefrom.push(map7);











                  if(this.internalBool && type=='diagnosis'){
                    let map8 = {};
                    map8.label1 = "未订购停机";
                    map8.value1 = tools.getDkeyValue(this.customize_whether,jsonobj.Data.is_Disconnected);
                    map8.label2 = "停机阈值";
                    map8.value2 = jsonobj.Data.remind_ratio;
                    map8.label3 = "卡状态描述";
                    map8.value3 = tools.getDkeyValue(this.stateOptions,jsonobj.Data.status_id);

                    this.tablefrom.push(map8);






                    //获取停机原因
                    simStopReason(Pwd_Str).then(response => {
                      let jsonobj =  JSON.parse(tools.Decrypt(response));
                      //console.log(jsonobj);
                      if(jsonobj.code==200){
                        this.fromApi.ReasonForDowntime = jsonobj.Data.stopReason;
                      }else{
                        this.msgError(response.msg);
                      }
                    })

                    //单卡开关机状态实时查询
                    onOffStatus(Pwd_Str).then(response => {
                      let jsonobj =  JSON.parse(tools.Decrypt(response));
                      //console.log(jsonobj);
                      if(jsonobj.code==200){
                        this.fromApi.switchNachine = jsonobj.Data.Openstatus;
                      }else{
                        this.msgError(jsonobj.msg);
                      }
                    })


                    //单卡已开通APN信息查询
                    apnInfo(Pwd_Str).then(response => {
                      let jsonobj =  JSON.parse(tools.Decrypt(response));
                      //console.log(jsonobj);
                      if(jsonobj.code==200){
                        let RapnList = jsonobj.Data.RapnList;
                        if(RapnList!=null && RapnList!=undefined && RapnList.length>0){
                          let table_ApnArr = [];
                          for (let i = 0; i <  RapnList.length; i++) {
                            let obj =  RapnList[i];
                            obj.label1 = "APN名称";
                            obj.value1 = obj.apnName;
                            obj.label2 = "状态";
                            obj.value2 = obj.statusNeme;
                            table_ApnArr.push(obj);
                          }
                          this.table_ApnArr = table_ApnArr;
                          this.ApnArrShow = true;
                        }

                      }else{
                        this.msgError(jsonobj.msg);
                      }
                    })

                    //物联卡机卡分离状态查询
                    map.testType = "0";
                    let BindPwd_Str = tools.encrypt(JSON.stringify(map));
                    cardBindStatus(BindPwd_Str).then(response => {
                      let jsonobj =  JSON.parse(tools.Decrypt(response));
                      //console.log(jsonobj);
                      if(jsonobj.code==200){
                        this.fromApi.MachineCardSeparation = jsonobj.Data.Bindstatus;
                      }else{
                        this.fromApi.MachineCardSeparation =  jsonobj.msg;
                        //this.msgError(jsonobj.msg);
                      }
                    })

                    //单卡状态变更历史查询
                    simChangeHistory(Pwd_Str).then(response => {
                      let jsonobj =  JSON.parse(tools.Decrypt(response));
                      //console.log(jsonobj);
                      if(jsonobj.code==200){
                        let changeHistoryList = jsonobj.Data.changeHistoryList;
                        if(changeHistoryList!=null && changeHistoryList!=undefined && changeHistoryList.length>0){
                          changeHistoryList.sort(function(a, b) {
                            return b.changeDate < a.changeDate ? 1 : -1;
                          })
                          this.cardChangeAPIList = changeHistoryList;
                          this.cardChangeAPIShow = true;//卡状态变更记录 显示 【API】
                        }
                      }else{
                        if(jsonobj.deptId=='100') {
                          this.msgError(jsonobj.msg);
                        }
                      }
                    })
                    this.cardInfoApiShow = true;//卡基础信息 显示 [API实时]
                  }

                  let map6 = {};
                  map6.label1 = "备注";
                  map6.value1 = jsonobj.Data.remarks;
                  this.tablefrom_remarks.push(map6);
                }
              );

              this.cardInfoTitle = "卡基础信息 (已同步数据)";

              this.cardInfoShow = true;//卡基础信息 显示

            }else{
              if(jsonobj.deptId=="100"){
                this.msgError(jsonobj.msg);
              }else{
                this.msgError("网络异常");
              }
            }
          }
        );
      },


      // 号码匹对
      oneClickMatch() {
        let _this = this;

        if(_this.disabledDiagnosis == false){
          _this.btnDiagnosis = false;
          _this.disabledDiagnosis = true;

          if(tools.Is_null(_this.cardNo) && tools.Is_null(_this.selType)){

            if(_this.cardNo.length>4){
              _this.showempty = false; //空状态 显示
              _this.cardInfoShow = false; //卡基础信息 显示
              _this.resultShow = false; //匹对结果 显示
              _this.cardOnlineShow = false; //在线信息 显示
              _this.cardFlowShow = false; //资费详情 显示
              _this.cardChangeShow = false; //变更记录 显示
              _this.ApnArrShow = false;//Apn 显示 [API实时]
              _this.cardInfoApiShow = false;//卡基础信息 显示 [API实时]

              this.form = {};

              let map ={};
              map.selType = _this.selType;
              map.cardNo = _this.cardNo;
              let Pwd_Str = tools.encrypt(JSON.stringify(map));
              cardMatch(Pwd_Str).then(response => {
                let jsonobj = JSON.parse(tools.Decrypt(response));
                // console.log(jsonobj);
                if (jsonobj.code == 200) {
                  let Data = jsonobj.Data;
                  _this.cardCount = Data.cardCount;

                  if(Data.cardCount>0){

                    let row = Data.cardMatchMap;

                    switch (_this.selType) {
                      case "0":
                        _this.cardReference = row.vid;
                        break;
                      case "1":
                        _this.cardReference = row.msisdn;
                        break;
                      case "2":
                        _this.cardReference = row.iccid;
                        break;
                      case "3":
                        _this.cardReference = row.imsi;
                        break;
                    }


                    _this.cardPrefix = Data.cardPrefix;
                    _this.cardSuffix = Data.cardSuffix;
                    this.resultShow = true;

                    this.internalBool =  jsonobj.Data.is_Internal;
                    this.cardInfoTitle = "卡基础信息 (信息同步中)";
                    this.cardPackageTitle = "资费信息 (信息同步中)";
                    this.OnlineTitle = '在线信息 (信息同步中)';

                    if(this.internalBool) {
                      //加载 API通道名称
                      if (window['findCr_RouteOptions'] != undefined && window['RouteOptions'] != null && window['findCr_RouteOptions'] != '') {
                        this.findCr_RouteOptions = window['findCr_RouteOptions'];
                      } else {
                        findCr().then(response => {
                          let jsonobj = JSON.parse(tools.Decrypt(response));
                          window['findCr_RouteOptions'] = jsonobj.Data;
                          this.findCr_RouteOptions = window['findCr_RouteOptions'];
                        });
                      }
                    }

                    this.finddata(row,'');
                    _this.btnDiagnosis = true;
                    setTimeout(function(){
                      _this.disabledDiagnosis = false;
                    },3000);// 3 S 内禁止再次点击
                  }else{
                    _this.showempty = true;
                    _this.btnDiagnosis = true;
                    _this.disabledDiagnosis = false;
                  }
                }else{
                  _this.showempty = true;
                  _this.btnDiagnosis = true;
                  _this.disabledDiagnosis = false;
                }
              })

            }else{
              tools.MessageShow(_this,"请输入大于4位数的号码段 !！！","error");
              _this.btnDiagnosis = true;
              _this.disabledDiagnosis = false;
            }

          }else {
            tools.MessageShow(_this,"请输入需要匹对的 MSISDN、ICCID、IMSI、Vid(虚拟编号) 信息！！","error");
            _this.btnDiagnosis = true;
            _this.disabledDiagnosis = false;
          }
        }else{
          tools.MessageShow(_this,"请勿重复点击！3秒后重试。","error");
        }
      },




      //在线信息 更新
      getOnlineStatus() {
        //  table_Online
        this.table_Online = [];//重置数据
        let map = {};
        map.iccid = this.sel.iccid;
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        queryOnlineStatus(Pwd_Str).then(res => {
          let jsonobj = JSON.parse(tools.Decrypt(res));
          // console.log(jsonobj);
          if (jsonobj.code == 200) {
            let obj = jsonobj.Data.Data;
            let map1 = {};
            let map2 = {};
            let cd_code = jsonobj.Data.cd_code;
            let createDate = '';
            if (cd_code == "YiDong_EC" || cd_code == "YiDong_EC_TOKE_ShuoLang" || cd_code == "YiDong_EC_TengYu") {
              map1.value1 = tools.getDkeyValue(this.access_type, obj.rat);
              map1.value2 = tools.getDkeyValue(this.online_status, obj.status);
              map2.value1 = obj.ip;
              map2.value2 = obj.apnId;
              createDate = obj.createDate;
            } else if (cd_code == "YiDong_ECv2") {
              map1.value1 = tools.getDkeyValue(this.access_type, obj.RAT);
              map1.value2 = tools.getDkeyValue(this.online_status, obj.GPRSSTATUS);
              map2.value1 = obj.IP;
              map2.value2 = obj.APN;
              createDate = obj.createDate;
            }else if(cd_code == "ZCWL" ) {
              map1.value1 = tools.getDkeyValue(this.access_type, obj.rat);
              map1.value2 = tools.getDkeyValue(this.online_status, obj.status);
              map2.value1 = obj.ip;
              map2.value2 = obj.apnId;
              createDate = obj.createDate;
            }else if(cd_code == "DianXin_CMP") {
                //DxCMP_RAT: [],//电信CMP接入类型
                map1.value1 = tools.getDkeyValue(this.DxCMP_RAT, obj.rattype)+" 无线接入模式 : [ "+tools.getDkeyValue(this.DxCMP_netModel_type, obj.netModel)+" ]";
                map1.value2 = tools.getDkeyValue(this.DxCMP_online, obj.result);
                map2.value1 = obj.framedIpAddress;
                map2.value2 = obj.apn;
                createDate = obj.eventTimestamp;
            }


            map1.label1 = "接入方式";
            map1.label2 = "在线状态";
            this.table_Online.push(map1);

            map2.label1 = "IP地址";
            map2.label2 = "APN";
            this.table_Online.push(map2);
            if (tools.Is_null(createDate)) {
              this.OnlineTitle = '在线信息 (会话创建时间 : ' + createDate + ')';
            } else {
              this.OnlineTitle = '在线信息 (已同步信息)';
            }
          } else {
            let map1 = {};
            map1.label1 = "返回信息";
            map1.value1 = jsonobj.msg;
            map1.label2 = "请核对API信息（未产生用量不会有在线记录！）";
            map1.value2 = "目前仅支持移动EC,移动Pboos";
            this.table_Online.push(map1);
            this.msgError(jsonobj.msg);
            this.OnlineTitle = '在线信息 (已同步信息)';
          }

          this.cardOnlineShow =  true; //在线信息 显示

        })
      },


      beforeHandleCommand(row,command,item,index,monthType){
        return {
          "row":row,
          "command":command,
          "item":item,
          "index":index,
          "monthType":monthType
        }
      },



      /*加载当前 套餐包*/
      loadCar(item,index,NotExpired){
        //
        let _this=this;
        // console.log(item);
        //  console.log(index);
        let map = {};
        map.iccid = this.sel.iccid;
        map.$index = index;
        map.agent_id = this.sel.agent_id;
        map.package_id = this.sel.package_id;

        map.NotExpired = NotExpired!=undefined && NotExpired!=null?'0':'1';
        _this.cardFlowShow = false;
        _this.queryflow(map);

      },
      queryflow(map){//查询现有流量包
        let _this = this;
        _this.FlowData = [];//重置
       // console.log(map);
        let Pwd_Str = tools.encrypt(JSON.stringify(map));
        //console.log(Pwd_Str);
        queryflowData(Pwd_Str).then(response => {
          let jsonobj =  JSON.parse(tools.Decrypt(response));
          //console.log(jsonobj);
          if (jsonobj.code == "200") {
            let arr = jsonobj.Data;

            var new_map = {},
              dest = [];
            for(var i = 0; i < arr.length; i++) {
              var ai = arr[i];
              if(!new_map[ai.ord_no]) { //依赖分组字段可自行更改！
                dest.push({
                  ord_no: ai.ord_no, //依赖分组字段可自行更改！
                  packet_wx_name: ai.packet_wx_name,
                  ord_status: ai.ord_status,
                  pay_type: ai.pay_type,
                  is_profit: ai.is_profit,
                  validate_type: ai.validate_type,
                  info: ai.info,
                  ord_name: ai.ord_name,
                  true_flow: ai.true_flow,
                  error_flow: ai.error_flow,
                  orderCreate_time: ai.orderCreate_time,
                  data: [ai]
                });
                new_map[ai.ord_no] = ai; //依赖分组字段可自行更改！
              } else {
                for(var j = 0; j < dest.length; j++) {
                  var dj = dest[j];
                  if(dj.ord_no == ai.ord_no) { //依赖分组字段可自行更改！
                    dj.data.push(ai);
                    break;
                  }
                }
              }
            }

            _this.FlowData = dest;
            _this.data = dest;
            //console.log(_this.selIccid);
            _this.selIccid[0].iccid = map.iccid;
            _this.selIccid[0].FlowData = dest;
            _this.cardFlowShow =  true; //资费详情 显示
            //window[map.iccid] = _this.FlowData;
            this.cardPackageTitle = "资费信息 (已同步数据)";
          } else {
            //tools.open(_this, jsonobj.msg);
            tools.MessageShow(_this,jsonobj.msg,"error");
          }
        })
      },




   handlerCommand(command) {
        let _this=this;
        switch (command.command) {
          case "returnpa"://退订
            _this.optino_Unsubscribe(command.row,command.item,command.index);
            break;
          case "updpackagetime"://当转次 次转当
            _this.updpackagetime(command.row,command.item,command.index,command.monthType);
            break;


        }
      },



      optino_Unsubscribe(scope,item,index){
        //套餐退订
        //套餐操作
        let _this=this;
        // console.log("套餐退订");
        _this.Refund.ord_no = "";
        _this.Refund.iccid = "";
        _this.Refund.cardType = "";
        _this.Refund.agent_id = "";
        _this.Refund.info = "";
        _this.Refund.packet_id = "";

        //console.log(scope);
        // console.log(item);
        // console.log(index);

        //如果当前套餐包生效中 才可退订
        if(scope.row.status==1){
          let map = {};
          map.packet_id = scope.row.packet_id;
          map.agent_id = item.agentId;
          _this.Refund.ord_no = scope.row.ord_no;
          _this.Refund.iccid = scope.row.iccid;
          _this.Refund.cardType = item.cardType;
          _this.Refund.agent_id = item.agentId;
          _this.Refund.packet_id = scope.row.packet_id;
          _this.Refund.$index = index;/**/
          _this.findcost_new(scope.row);

        }else{
          tools.open(_this,"已失效套餐不能退订！");
        }
      },

      updpackagetime(scope,item,index,monthType){//当转次 次转当
        console.log(scope);
        console.log(item);
        // console.log(index);
        // console.log(monthType);
        let _this = this;

        let title = monthType=='next'?"当月生效转次月生效":"次月生效转当月生效";

        _this.$confirm('流量包 ['+title+'] 操作, 是否继续?', '提示', {
          confirmButtonText: '确定',
          cancelButtonText: '取消',
          type: 'warning'
        }).then(() => {
          let map = {};
          map.package_id = scope.row.package_id;
          map.packet_id = scope.row.packet_id;
          map.agent_id = item.agentId;
          map.iccid = scope.row.iccid;
          map.table_name = "zy_"+item.cardType+"_flow";
          map.$index = index;
          map.monthType = monthType;
          map.ord_type = monthType;
          map.cardType = item.cardType;
          map.ord_no = scope.row.ord_no;
          map.id = scope.row.id;
          // map.ord_start_time = item.start_time;
          // map.ord_end_time = item.end_time;


          this.$ajax.post(process.env.VUE_APP_SERVER + "/system/pac/upd_packageTime", map).then((response) => {
            if (response.data.code == "0") {
              //tools.open(_this, response.data.data);
              tools.MessageShow(_this,response.data.data,"success");
              _this.openRefund = false;
              map.table_name = "zy_"+map.cardType+"_flow";
              //console.log(map);
              //加载套餐包

              _this.SetMealImport = false;
              _this.queryflow(map);

            } else {
              //tools.open(_this, response.data.msg);
              tools.MessageShow(_this,response.data.msg,"error");
            }

          })
        }).catch(() => {
          /* this.$message({
               type: 'info',
               message: '已取消变更套餐包'
           });*/
        });

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

  .my_cloor_red {
    background-color: #e64242;
    color: white;
    text-align: center;
  }
  .my_cloor_blue {
    background-color: #1682e6;
    color: white;
    text-align: center;
  }
  .my_cloor_Orange {
    background-color: #f2e119;
    color: white;
    text-align: center;
  }
  .my_cloor_black {
    background-color: #303133;
    color: white;
    text-align: center;
  }

  .my_cloor_green {
    background-color: #11b95c;
    color: white;
    text-align: center;
  }

  .el-divider--horizontal{
    margin: 18px 0;
  }

</style>

