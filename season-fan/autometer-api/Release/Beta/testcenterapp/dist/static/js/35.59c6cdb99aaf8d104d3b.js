webpackJsonp([35],{Pdqj:function(e,t,n){"use strict";Object.defineProperty(t,"__esModule",{value:!0});var i=n("Dd8w"),a=n.n(i),s=n("vLgD");function r(e){return Object(s.a)({url:"/envmachine/search",method:"post",data:e})}var c=n("4I8B"),o=n("W/rB"),m=n("0xDb"),l=n("NYxO"),h={filters:{statusFilter:function(e){return{published:"success",draft:"gray",deleted:"danger"}[e]}},data:function(){return{itemKey:null,tmpenviromentname:"",envmachineList:[],enviromentnameList:[],machinenameList:[],listLoading:!1,total:0,listQuery:{page:1,size:10,listLoading:!0,enviromentname:""},dialogStatus:"add",dialogFormVisible:!1,textMap:{updateRole:"修改测试环境",update:"修改测试环境",add:"添加测试环境"},btnLoading:!1,tmpenvmachine:{id:"",envid:"",machineid:"",enviromentname:"",machinename:"",creator:""},search:{page:1,size:10,enviromentname:null}}},computed:a()({},Object(l.b)(["name","sidebar","avatar"])),created:function(){this.getenvmachineList(),this.getenviromentallList(),this.getmachineLists()},methods:{unix2CurrentTime:m.b,selectChangedEN:function(e){for(var t=0;t<this.enviromentnameList.length;t++)this.enviromentnameList[t].enviromentname===e&&(this.tmpenvmachine.envid=this.enviromentnameList[t].id),console.log(this.enviromentnameList[t].id)},selectChangedMN:function(e){for(var t=0;t<this.machinenameList.length;t++)this.machinenameList[t].machinename===e&&(this.tmpenvmachine.machineid=this.machinenameList[t].id),console.log(this.machinenameList[t].id)},getenvmachineList:function(){var e=this;this.listLoading=!0,this.search.enviromentname=this.tmpenviromentname,r(this.search).then(function(t){e.envmachineList=t.data.list,e.total=t.data.total,e.listLoading=!1}).catch(function(t){e.$message.error("加载测试环境服务器列表失败")})},getenviromentallList:function(){var e=this;this.listLoading=!0,Object(o.c)().then(function(t){e.enviromentnameList=t.data,e.total=t.data.total,e.listLoading=!1}).catch(function(t){e.$message.error("加载测试环境列表失败")})},getmachineLists:function(){var e=this;this.listLoading=!0,Object(c.b)().then(function(t){e.machinenameList=t.data,e.total=t.data.total,e.listLoading=!1}).catch(function(t){e.$message.error("加载服务器列表失败")})},searchBy:function(){var e=this;this.search.page=1,this.listLoading=!0,r(this.search).then(function(t){e.itemKey=Math.random(),e.envmachineList=t.data.list,e.total=t.data.total}).catch(function(t){e.$message.error("搜索失败")}),this.tmpenviromentname=this.search.enviromentname,this.listLoading=!1,this.btnLoading=!1},handleSizeChange:function(e){this.search.page=1,this.search.size=e,this.getenvmachineList()},handleCurrentChange:function(e){this.search.page=e,this.getenvmachineList()},getIndex:function(e){return(this.search.page-1)*this.search.size+e+1},showAddenvmachineDialog:function(){this.dialogFormVisible=!0,this.dialogStatus="add",this.tmpenvmachine.id="",this.tmpenvmachine.envid="",this.tmpenvmachine.machineid="",this.tmpenvmachine.machinename="",this.tmpenvmachine.enviromentname="",this.tmpenvmachine.creator=this.name},addenvmachine:function(){var e=this;this.$refs.tmpenvmachine.validate(function(t){var n;t&&(e.btnLoading=!0,(n=e.tmpenvmachine,Object(s.a)({url:"/envmachine",method:"post",data:n})).then(function(){e.$message.success("添加成功"),e.getenvmachineList(),e.dialogFormVisible=!1,e.btnLoading=!1}).catch(function(t){e.$message.error("添加失败"),e.btnLoading=!1}))})},showUpdateenvmachineDialog:function(e){this.dialogFormVisible=!0,this.dialogStatus="update",this.tmpenvmachine.id=this.envmachineList[e].id,this.tmpenvmachine.envid=this.envmachineList[e].envid,this.tmpenvmachine.machineid=this.envmachineList[e].machineid,this.tmpenvmachine.machinename=this.envmachineList[e].machinename,this.tmpenvmachine.enviromentname=this.envmachineList[e].enviromentname,this.tmpenvmachine.creator=this.name},updateenvmachine:function(){var e=this;this.$refs.tmpenvmachine.validate(function(t){var n;t&&(n=e.tmpenvmachine,Object(s.a)({url:"/envmachine/detail",method:"put",data:n})).then(function(){e.$message.success("更新成功"),e.getenvmachineList(),e.dialogFormVisible=!1}).catch(function(t){e.$message.error("更新失败")})})},removeenvmachine:function(e){var t=this;this.$confirm("删除该测试环境？","警告",{confirmButtonText:"是",cancelButtonText:"否",type:"warning"}).then(function(){var n,i=t.envmachineList[e].id;(n=i,Object(s.a)({url:"/envmachine/"+n,method:"delete"})).then(function(){t.$message.success("删除成功"),t.getenvmachineList()})}).catch(function(){t.$message.info("已取消删除")})},isUniqueDetail:function(e){for(var t=0;t<this.envmachineList.length;t++)if(this.envmachineList[t].id!==e.id&&this.envmachineList[t].enviromentname===e.enviromentname&&this.envmachineList[t].machinename===e.machinename)return this.$message.error("测试环境名已存在"),!1;return!0}}},d={render:function(){var e=this,t=e.$createElement,n=e._self._c||t;return n("div",{staticClass:"app-container"},[n("div",{staticClass:"filter-container"},[n("el-form",{attrs:{inline:!0}},[n("el-form-item",[e.hasPermission("envmachine:list")?n("el-button",{attrs:{type:"success",size:"mini",icon:"el-icon-refresh"},nativeOn:{click:function(t){return t.preventDefault(),e.getenvmachineList(t)}}},[e._v("刷新")]):e._e(),e._v(" "),e.hasPermission("envmachine:add")?n("el-button",{attrs:{type:"primary",size:"mini",icon:"el-icon-plus"},nativeOn:{click:function(t){return t.preventDefault(),e.showAddenvmachineDialog(t)}}},[e._v("添加环境服务器")]):e._e()],1),e._v(" "),e.hasPermission("envmachine:search")?n("span",[n("el-form-item",[n("el-input",{attrs:{clearable:"",placeholder:"测试环境名"},nativeOn:{keyup:function(t){return!t.type.indexOf("key")&&e._k(t.keyCode,"enter",13,t.key,"Enter")?null:e.searchBy(t)}},model:{value:e.search.enviromentname,callback:function(t){e.$set(e.search,"enviromentname",t)},expression:"search.enviromentname"}})],1),e._v(" "),n("el-form-item",[n("el-button",{attrs:{type:"primary",loading:e.btnLoading},on:{click:e.searchBy}},[e._v("查询")])],1)],1):e._e()],1)],1),e._v(" "),n("el-table",{directives:[{name:"loading",rawName:"v-loading.body",value:e.listLoading,expression:"listLoading",modifiers:{body:!0}}],key:e.itemKey,attrs:{data:e.envmachineList,"element-loading-text":"loading",border:"",fit:"","highlight-current-row":""}},[n("el-table-column",{attrs:{label:"编号",align:"center",width:"60"},scopedSlots:e._u([{key:"default",fn:function(t){return[n("span",{domProps:{textContent:e._s(e.getIndex(t.$index))}})]}}])}),e._v(" "),n("el-table-column",{attrs:{label:"测试环境",align:"center",prop:"enviromentname",width:"120"}}),e._v(" "),n("el-table-column",{attrs:{label:"服务器",align:"center",prop:"machinename",width:"120"}}),e._v(" "),n("el-table-column",{attrs:{label:"ip",align:"center",prop:"ip",width:"120"}}),e._v(" "),n("el-table-column",{attrs:{label:"操作人",align:"center",prop:"creator",width:"100"}}),e._v(" "),n("el-table-column",{attrs:{label:"创建时间",align:"center",prop:"createTime",width:"160"},scopedSlots:e._u([{key:"default",fn:function(t){return[e._v(e._s(e.unix2CurrentTime(t.row.createTime)))]}}])}),e._v(" "),n("el-table-column",{attrs:{label:"最后修改时间",align:"center",prop:"lastmodifyTime",width:"160"},scopedSlots:e._u([{key:"default",fn:function(t){return[e._v(e._s(e.unix2CurrentTime(t.row.lastmodifyTime))+"\n      ")]}}])}),e._v(" "),e.hasPermission("envmachine:update")||e.hasPermission("envmachine:delete")?n("el-table-column",{attrs:{label:"管理",align:"center"},scopedSlots:e._u([{key:"default",fn:function(t){return[e.hasPermission("envmachine:update")&&t.row.id!==e.id?n("el-button",{attrs:{type:"warning",size:"mini"},nativeOn:{click:function(n){return n.preventDefault(),e.showUpdateenvmachineDialog(t.$index)}}},[e._v("修改")]):e._e(),e._v(" "),e.hasPermission("envmachine:delete")&&t.row.id!==e.id?n("el-button",{attrs:{type:"danger",size:"mini"},nativeOn:{click:function(n){return n.preventDefault(),e.removeenvmachine(t.$index)}}},[e._v("删除")]):e._e()]}}],null,!1,2812579302)}):e._e()],1),e._v(" "),n("el-pagination",{attrs:{"current-page":e.search.page,"page-size":e.search.size,total:e.total,"page-sizes":[10,20,30,40],layout:"total, sizes, prev, pager, next, jumper"},on:{"size-change":e.handleSizeChange,"current-change":e.handleCurrentChange}}),e._v(" "),n("el-dialog",{attrs:{title:e.textMap[e.dialogStatus],visible:e.dialogFormVisible},on:{"update:visible":function(t){e.dialogFormVisible=t}}},[n("el-form",{ref:"tmpenvmachine",staticClass:"small-space",staticStyle:{width:"400px","margin-left":"50px"},attrs:{"status-icon":"","label-position":"left","label-width":"120px",model:e.tmpenvmachine}},[n("el-form-item",{attrs:{label:"测试环境",prop:"enviromentname",required:""}},[n("el-select",{staticStyle:{width:"100%"},attrs:{placeholder:"测试环境名"},on:{change:function(t){return e.selectChangedEN(t)}},model:{value:e.tmpenvmachine.enviromentname,callback:function(t){e.$set(e.tmpenvmachine,"enviromentname",t)},expression:"tmpenvmachine.enviromentname"}},[n("el-option",{staticStyle:{display:"none"},attrs:{label:"请选择",value:"''"}}),e._v(" "),e._l(e.enviromentnameList,function(e,t){return n("div",{key:t},[n("el-option",{attrs:{label:e.enviromentname,value:e.enviromentname,required:""}})],1)})],2)],1),e._v(" "),n("el-form-item",{attrs:{label:"服务器",prop:"machinename",required:""}},[n("el-select",{staticStyle:{width:"100%"},attrs:{placeholder:"服务器"},on:{change:function(t){return e.selectChangedMN(t)}},model:{value:e.tmpenvmachine.machinename,callback:function(t){e.$set(e.tmpenvmachine,"machinename",t)},expression:"tmpenvmachine.machinename"}},[n("el-option",{staticStyle:{display:"none"},attrs:{label:"请选择",value:"''"}}),e._v(" "),e._l(e.machinenameList,function(e,t){return n("div",{key:t},[n("el-option",{attrs:{label:e.machinename+" ："+e.ip,value:e.machinename,required:""}})],1)})],2)],1)],1),e._v(" "),n("div",{staticClass:"dialog-footer",attrs:{slot:"footer"},slot:"footer"},[n("el-button",{nativeOn:{click:function(t){t.preventDefault(),e.dialogFormVisible=!1}}},[e._v("取消")]),e._v(" "),"add"===e.dialogStatus?n("el-button",{attrs:{type:"danger"},nativeOn:{click:function(t){return t.preventDefault(),e.$refs.tmpenvmachine.resetFields()}}},[e._v("重置")]):e._e(),e._v(" "),"add"===e.dialogStatus?n("el-button",{attrs:{type:"success",loading:e.btnLoading},nativeOn:{click:function(t){return t.preventDefault(),e.addenvmachine(t)}}},[e._v("添加")]):e._e(),e._v(" "),"update"===e.dialogStatus?n("el-button",{attrs:{type:"success",loading:e.btnLoading},nativeOn:{click:function(t){return t.preventDefault(),e.updateenvmachine(t)}}},[e._v("修改")]):e._e()],1)],1)],1)},staticRenderFns:[]},v=n("VU/8")(h,d,!1,null,null,null);t.default=v.exports}});
//# sourceMappingURL=35.59c6cdb99aaf8d104d3b.js.map