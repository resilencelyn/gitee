webpackJsonp([18],{Xw19:function(e,a,t){"use strict";a.b=function(){return Object(i.a)({url:"/testvariables/getvariableslist",method:"get"})},a.d=function(e){return Object(i.a)({url:"/testvariables/search",method:"post",data:e})},a.a=function(e){return Object(i.a)({url:"/testvariables",method:"post",data:e})},a.e=function(e){return Object(i.a)({url:"/testvariables/detail",method:"put",data:e})},a.c=function(e){return Object(i.a)({url:"/testvariables/"+e,method:"delete"})};var i=t("vLgD")},zTdQ:function(e,a,t){"use strict";Object.defineProperty(a,"__esModule",{value:!0});var i=t("Dd8w"),s=t.n(i),n=t("vLgD");function l(e){return Object(n.a)({url:"/apicases/variables/search",method:"post",data:e})}var r=t("0xDb"),c=t("W+bg"),o=t("2d0t"),p=t("XcM5"),u=t("NYxO"),d=t("Xw19"),m={filters:{statusFilter:function(e){return{published:"success",draft:"gray",deleted:"danger"}[e]}},data:function(){return{itemKey:null,tmpvariablesname:"",ApicasesVariablesList:[],variablesList:[],apiList:[],caseList:[],deployunitList:[],listLoading:!1,total:0,dialogStatus:"add",dialogFormVisible:!1,textMap:{updateRole:"修改用例变量",update:"修改用例变量",add:"添加用例变量"},deployunitQuery:{deployunitname:""},apiquery:{casedeployunitname:"",caseapiname:""},apicaseQuery:{apiname:""},btnLoading:!1,tmpApicasesVariables:{id:"",apiid:"",caseid:"",deployunitid:"",deployunitname:"",apiname:"",casename:"",variablesid:"",variablesname:"",memo:"",creator:""},search:{page:1,size:10,variablesname:null}}},created:function(){this.getApicasesVariablesList(),this.getdepunitList(),this.gettestvariablesallList()},computed:s()({},Object(u.b)(["name","sidebar","avatar"])),methods:{unix2CurrentTime:r.b,deployunitselectChanged:function(e){for(var a=this,t=0;t<this.deployunitList.length;t++)this.deployunitList[t].deployunitname===e&&(this.tmpApicasesVariables.deployunitid=this.deployunitList[t].id);this.tmpApicasesVariables.apiname="",this.tmpApicasesVariables.casename="",this.tmpApicasesVariables.variablesname="",this.deployunitQuery.deployunitname=e,Object(c.d)(this.deployunitQuery).then(function(e){a.apiList=e.data}).catch(function(e){a.$message.error("加载api列表失败")})},apiselectChanged:function(e){for(var a=this,t=0;t<this.apiList.length;t++)this.apiList[t].apiname===e&&(this.tmpApicasesVariables.apiid=this.apiList[t].id);this.tmpApicasesVariables.casename="",this.tmpApicasesVariables.variablesname="",this.apiquery.caseapiname=e,this.apiquery.casedeployunitname=this.tmpApicasesVariables.deployunitname,Object(o.c)(this.apiquery).then(function(e){a.caseList=e.data}).catch(function(e){a.$message.error("加载apicase列表失败")})},testcaseselectChanged:function(e){for(var a=0;a<this.caseList.length;a++)this.caseList[a].casename===e&&(this.tmpApicasesVariables.caseid=this.caseList[a].id),this.tmpApicasesVariables.variablesname=""},variablesselectChanged:function(e){for(var a=0;a<this.variablesList.length;a++)this.variablesList[a].testvariablesname===e&&(this.tmpApicasesVariables.variablesid=this.variablesList[a].id)},gettestvariablesallList:function(){var e=this;this.listLoading=!0,Object(d.b)().then(function(a){e.variablesList=a.data,e.listLoading=!1}).catch(function(a){e.$message.error("加载变量列表失败")})},getdepunitList:function(){var e=this;this.listLoading=!0,Object(p.d)(this.listQuery).then(function(a){e.deployunitList=a.data.list,e.listLoading=!1}).catch(function(a){e.$message.error("加载发布单元列表失败")})},getApicasesVariablesList:function(){var e=this;this.listLoading=!0,this.search.ApicasesVariablesname=this.tmpApicasesVariablesname,l(this.search).then(function(a){e.ApicasesVariablesList=a.data.list,e.total=a.data.total,e.listLoading=!1}).catch(function(a){e.$message.error("加载用例变量列表失败")})},searchBy:function(){var e=this;this.search.page=1,this.listLoading=!0,l(this.search).then(function(a){e.itemKey=Math.random(),e.ApicasesVariablesList=a.data.list,e.total=a.data.total}).catch(function(a){e.$message.error("搜索失败")}),this.tmpvariablesname=this.search.variablesname,this.listLoading=!1,this.btnLoading=!1},handleSizeChange:function(e){this.search.page=1,this.search.size=e,this.getApicasesVariablesList()},handleCurrentChange:function(e){this.search.page=e,this.getApicasesVariablesList()},getIndex:function(e){return(this.search.page-1)*this.search.size+e+1},showAddApicasesVariablesDialog:function(){this.dialogFormVisible=!0,this.dialogStatus="add",this.tmpApicasesVariables.id="",this.tmpApicasesVariables.caseid="",this.tmpApicasesVariables.deployunitname="",this.tmpApicasesVariables.apiname="",this.tmpApicasesVariables.casename="",this.tmpApicasesVariables.variablesid="",this.tmpApicasesVariables.variablesname="",this.tmpApicasesVariables.memo="",this.tmpApicasesVariables.creator=this.name},addApicasesVariables:function(){var e=this;this.$refs.tmpApicasesVariables.validate(function(a){var t;a&&(e.btnLoading=!0,(t=e.tmpApicasesVariables,Object(n.a)({url:"/apicases/variables",method:"post",data:t})).then(function(){e.$message.success("添加成功"),e.getApicasesVariablesList(),e.dialogFormVisible=!1,e.btnLoading=!1}).catch(function(a){e.$message.error("添加失败"),e.btnLoading=!1}))})},showUpdateApicasesVariablesDialog:function(e){var a=this;this.dialogFormVisible=!0,this.dialogStatus="update",this.tmpApicasesVariables.id=this.ApicasesVariablesList[e].id,this.tmpApicasesVariables.caseid=this.ApicasesVariablesList[e].caseid,this.tmpApicasesVariables.variablesid=this.ApicasesVariablesList[e].variablesid,this.tmpApicasesVariables.deployunitname=this.ApicasesVariablesList[e].deployunitname,this.deployunitQuery.deployunitname=this.tmpApicasesVariables.deployunitname,Object(c.d)(this.deployunitQuery).then(function(e){a.apiList=e.data}).catch(function(e){a.$message.error("加载api列表失败")}),this.tmpApicasesVariables.apiname=this.ApicasesVariablesList[e].apiname,this.apiquery.caseapiname=this.tmpApicasesVariables.apiname,this.apiquery.casedeployunitname=this.tmpApicasesVariables.deployunitname,Object(o.c)(this.apiquery).then(function(e){a.caseList=e.data}).catch(function(e){a.$message.error("加载apicase列表失败")}),this.tmpApicasesVariables.casename=this.ApicasesVariablesList[e].casename,this.tmpApicasesVariables.variablesname=this.ApicasesVariablesList[e].variablesname,this.tmpApicasesVariables.memo=this.ApicasesVariablesList[e].memo,this.tmpApicasesVariables.creator=this.name},updateApicasesVariables:function(){var e=this;this.$refs.tmpApicasesVariables.validate(function(a){var t;a&&(t=e.tmpApicasesVariables,Object(n.a)({url:"/apicases/variables/detail",method:"put",data:t})).then(function(){e.$message.success("更新成功"),e.getApicasesVariablesList(),e.dialogFormVisible=!1}).catch(function(a){e.$message.error("更新失败")})})},removeApicasesVariables:function(e){var a=this;this.$confirm("删除该用例变量？","警告",{confirmButtonText:"是",cancelButtonText:"否",type:"warning"}).then(function(){var t,i=a.ApicasesVariablesList[e].id;(t=i,Object(n.a)({url:"/apicases/variables/"+t,method:"delete"})).then(function(){a.$message.success("删除成功"),a.getApicasesVariablesList()})}).catch(function(){a.$message.info("已取消删除")})}}},b={render:function(){var e=this,a=e.$createElement,t=e._self._c||a;return t("div",{staticClass:"app-container"},[t("div",{staticClass:"filter-container"},[t("el-form",{attrs:{inline:!0}},[t("el-form-item",[e.hasPermission("ApicasesVariables:list")?t("el-button",{attrs:{type:"success",size:"mini",icon:"el-icon-refresh"},nativeOn:{click:function(a){return a.preventDefault(),e.getApicasesVariablesList(a)}}},[e._v("刷新")]):e._e(),e._v(" "),e.hasPermission("ApicasesVariables:add")?t("el-button",{attrs:{type:"primary",size:"mini",icon:"el-icon-plus"},nativeOn:{click:function(a){return a.preventDefault(),e.showAddApicasesVariablesDialog(a)}}},[e._v("添加用例变量")]):e._e()],1),e._v(" "),e.hasPermission("ApicasesVariables:search")?t("span",[t("el-form-item",[t("el-input",{attrs:{clearable:"",maxlength:"40",placeholder:"变量名"},nativeOn:{keyup:function(a){return!a.type.indexOf("key")&&e._k(a.keyCode,"enter",13,a.key,"Enter")?null:e.searchBy(a)}},model:{value:e.search.variablesname,callback:function(a){e.$set(e.search,"variablesname",a)},expression:"search.variablesname"}})],1),e._v(" "),t("el-form-item",[t("el-button",{attrs:{type:"primary",loading:e.btnLoading},on:{click:e.searchBy}},[e._v("查询")])],1)],1):e._e()],1)],1),e._v(" "),t("el-table",{directives:[{name:"loading",rawName:"v-loading.body",value:e.listLoading,expression:"listLoading",modifiers:{body:!0}}],key:e.itemKey,attrs:{data:e.ApicasesVariablesList,"element-loading-text":"loading",border:"",fit:"","highlight-current-row":""}},[t("el-table-column",{attrs:{label:"编号",align:"center",width:"60"},scopedSlots:e._u([{key:"default",fn:function(a){return[t("span",{domProps:{textContent:e._s(e.getIndex(a.$index))}})]}}])}),e._v(" "),t("el-table-column",{attrs:{label:"用例名",align:"center",prop:"casename",width:"180"}}),e._v(" "),t("el-table-column",{attrs:{label:"用例变量名",align:"center",prop:"variablesname",width:"100"}}),e._v(" "),t("el-table-column",{attrs:{label:"描述",align:"center",prop:"memo",width:"100"}}),e._v(" "),t("el-table-column",{attrs:{label:"操作人",align:"center",prop:"creator",width:"100"}}),e._v(" "),t("el-table-column",{attrs:{label:"创建时间",align:"center",prop:"createTime",width:"160"},scopedSlots:e._u([{key:"default",fn:function(a){return[e._v(e._s(e.unix2CurrentTime(a.row.createTime)))]}}])}),e._v(" "),t("el-table-column",{attrs:{label:"最后修改时间",align:"center",prop:"lastmodifyTime",width:"160"},scopedSlots:e._u([{key:"default",fn:function(a){return[e._v(e._s(e.unix2CurrentTime(a.row.lastmodifyTime))+"\n      ")]}}])}),e._v(" "),e.hasPermission("ApicasesVariables:update")||e.hasPermission("ApicasesVariables:delete")?t("el-table-column",{attrs:{label:"管理",align:"center"},scopedSlots:e._u([{key:"default",fn:function(a){return[e.hasPermission("ApicasesVariables:update")&&a.row.id!==e.id?t("el-button",{attrs:{type:"warning",size:"mini"},nativeOn:{click:function(t){return t.preventDefault(),e.showUpdateApicasesVariablesDialog(a.$index)}}},[e._v("修改")]):e._e(),e._v(" "),e.hasPermission("ApicasesVariables:delete")&&a.row.id!==e.id?t("el-button",{attrs:{type:"danger",size:"mini"},nativeOn:{click:function(t){return t.preventDefault(),e.removeApicasesVariables(a.$index)}}},[e._v("删除")]):e._e()]}}],null,!1,591169350)}):e._e()],1),e._v(" "),t("el-pagination",{attrs:{"current-page":e.search.page,"page-size":e.search.size,total:e.total,"page-sizes":[10,20,30,40],layout:"total, sizes, prev, pager, next, jumper"},on:{"size-change":e.handleSizeChange,"current-change":e.handleCurrentChange}}),e._v(" "),t("el-dialog",{attrs:{title:e.textMap[e.dialogStatus],visible:e.dialogFormVisible},on:{"update:visible":function(a){e.dialogFormVisible=a}}},[t("el-form",{ref:"tmpApicasesVariables",staticClass:"small-space",staticStyle:{width:"450px","margin-left":"50px"},attrs:{"status-icon":"","label-position":"left","label-width":"120px",model:e.tmpApicasesVariables}},[t("el-form-item",{attrs:{label:"发布单元",prop:"deployunitname",required:""}},[t("el-select",{staticStyle:{width:"100%"},attrs:{placeholder:"发布单元"},on:{change:function(a){return e.deployunitselectChanged(a)}},model:{value:e.tmpApicasesVariables.deployunitname,callback:function(a){e.$set(e.tmpApicasesVariables,"deployunitname",a)},expression:"tmpApicasesVariables.deployunitname"}},[t("el-option",{staticStyle:{display:"none"},attrs:{label:"请选择",value:"''"}}),e._v(" "),e._l(e.deployunitList,function(e,a){return t("div",{key:a},[t("el-option",{attrs:{label:e.deployunitname,value:e.deployunitname,required:""}})],1)})],2)],1),e._v(" "),t("el-form-item",{attrs:{label:"api",prop:"apiname",required:""}},[t("el-select",{staticStyle:{width:"100%"},attrs:{placeholder:"api"},on:{change:function(a){return e.apiselectChanged(a)}},model:{value:e.tmpApicasesVariables.apiname,callback:function(a){e.$set(e.tmpApicasesVariables,"apiname",a)},expression:"tmpApicasesVariables.apiname"}},[t("el-option",{staticStyle:{display:"none"},attrs:{label:"请选择",value:"''"}}),e._v(" "),e._l(e.apiList,function(e,a){return t("div",{key:a},[t("el-option",{attrs:{label:e.apiname,value:e.apiname}})],1)})],2)],1),e._v(" "),t("el-form-item",{attrs:{label:"用例",prop:"casename",required:""}},[t("el-select",{staticStyle:{width:"100%"},attrs:{placeholder:"用例"},on:{change:function(a){return e.testcaseselectChanged(a)}},model:{value:e.tmpApicasesVariables.casename,callback:function(a){e.$set(e.tmpApicasesVariables,"casename",a)},expression:"tmpApicasesVariables.casename"}},[t("el-option",{staticStyle:{display:"none"},attrs:{label:"请选择",value:"''"}}),e._v(" "),e._l(e.caseList,function(e,a){return t("div",{key:a},[t("el-option",{attrs:{label:e.casename,value:e.casename,required:""}})],1)})],2)],1),e._v(" "),t("el-form-item",{attrs:{label:"绑定变量",prop:"variablesname",required:""}},[t("el-select",{staticStyle:{width:"100%"},attrs:{placeholder:"变量名"},on:{change:function(a){return e.variablesselectChanged(a)}},model:{value:e.tmpApicasesVariables.variablesname,callback:function(a){e.$set(e.tmpApicasesVariables,"variablesname",a)},expression:"tmpApicasesVariables.variablesname"}},[t("el-option",{staticStyle:{display:"none"},attrs:{label:"请选择",value:"''"}}),e._v(" "),e._l(e.variablesList,function(e,a){return t("div",{key:a},[t("el-option",{attrs:{label:e.testvariablesname,value:e.testvariablesname,required:""}})],1)})],2)],1),e._v(" "),t("el-form-item",{attrs:{label:"备注",prop:"memo"}},[t("el-input",{attrs:{type:"text","prefix-icon":"el-icon-message","auto-complete":"off"},model:{value:e.tmpApicasesVariables.memo,callback:function(a){e.$set(e.tmpApicasesVariables,"memo",a)},expression:"tmpApicasesVariables.memo"}})],1)],1),e._v(" "),t("div",{staticClass:"dialog-footer",attrs:{slot:"footer"},slot:"footer"},[t("el-button",{nativeOn:{click:function(a){a.preventDefault(),e.dialogFormVisible=!1}}},[e._v("取消")]),e._v(" "),"add"===e.dialogStatus?t("el-button",{attrs:{type:"danger"},nativeOn:{click:function(a){return a.preventDefault(),e.$refs.tmpApicasesVariables.resetFields()}}},[e._v("重置")]):e._e(),e._v(" "),"add"===e.dialogStatus?t("el-button",{attrs:{type:"success",loading:e.btnLoading},nativeOn:{click:function(a){return a.preventDefault(),e.addApicasesVariables(a)}}},[e._v("添加")]):e._e(),e._v(" "),"update"===e.dialogStatus?t("el-button",{attrs:{type:"success",loading:e.btnLoading},nativeOn:{click:function(a){return a.preventDefault(),e.updateApicasesVariables(a)}}},[e._v("修改")]):e._e()],1)],1)],1)},staticRenderFns:[]},h=t("VU/8")(m,b,!1,null,null,null);a.default=h.exports}});
//# sourceMappingURL=18.a7b46f50b26bcc8c63ac.js.map