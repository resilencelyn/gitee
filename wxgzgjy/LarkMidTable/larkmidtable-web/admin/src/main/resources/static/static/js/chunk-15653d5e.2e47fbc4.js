(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([["chunk-15653d5e"],{"1c18":function(e,t,a){},"333d":function(e,t,a){"use strict";var i=function(){var e=this,t=e.$createElement,a=e._self._c||t;return a("div",{staticClass:"pagination-container",class:{hidden:e.hidden}},[a("el-pagination",e._b({attrs:{background:e.background,"current-page":e.currentPage,"page-size":e.pageSize,layout:e.layout,"page-sizes":e.pageSizes,total:e.total},on:{"update:currentPage":function(t){e.currentPage=t},"update:current-page":function(t){e.currentPage=t},"update:pageSize":function(t){e.pageSize=t},"update:page-size":function(t){e.pageSize=t},"size-change":e.handleSizeChange,"current-change":e.handleCurrentChange}},"el-pagination",e.$attrs,!1))],1)},n=[];a("a9e3");Math.easeInOutQuad=function(e,t,a,i){return e/=i/2,e<1?a/2*e*e+t:(e--,-a/2*(e*(e-2)-1)+t)};var l=function(){return window.requestAnimationFrame||window.webkitRequestAnimationFrame||window.mozRequestAnimationFrame||function(e){window.setTimeout(e,1e3/60)}}();function o(e){document.documentElement.scrollTop=e,document.body.parentNode.scrollTop=e,document.body.scrollTop=e}function r(){return document.documentElement.scrollTop||document.body.parentNode.scrollTop||document.body.scrollTop}function s(e,t,a){var i=r(),n=e-i,s=20,u=0;t="undefined"===typeof t?500:t;var c=function e(){u+=s;var r=Math.easeInOutQuad(u,i,n,t);o(r),u<t?l(e):a&&"function"===typeof a&&a()};c()}var u={name:"Pagination",props:{total:{required:!0,type:Number},page:{type:Number,default:1},limit:{type:Number,default:20},pageSizes:{type:Array,default:function(){return[10,20,30,50]}},layout:{type:String,default:"total, sizes, prev, pager, next, jumper"},background:{type:Boolean,default:!0},autoScroll:{type:Boolean,default:!0},hidden:{type:Boolean,default:!1}},computed:{currentPage:{get:function(){return this.page},set:function(e){this.$emit("update:page",e)}},pageSize:{get:function(){return this.limit},set:function(e){this.$emit("update:limit",e)}}},methods:{handleSizeChange:function(e){this.$emit("pagination",{page:this.currentPage,limit:e}),this.autoScroll&&s(0,800)},handleCurrentChange:function(e){this.$emit("pagination",{page:e,limit:this.pageSize}),this.autoScroll&&s(0,800)}}},c=u,d=(a("e498"),a("2877")),p=Object(d["a"])(c,i,n,!1,null,"6af373ef",null);t["a"]=p.exports},"4c95":function(e,t,a){"use strict";a.r(t);var i={};a.r(i),a.d(i,"list",(function(){return r})),a.d(i,"updated",(function(){return s})),a.d(i,"created",(function(){return u})),a.d(i,"deleted",(function(){return c})),a.d(i,"getDevEnvSettingList",(function(){return d}));var n=function(){var e=this,t=e.$createElement,a=e._self._c||t;return a("div",{staticClass:"app-container"},[a("div",{staticClass:"filter-container"},[a("el-input",{staticClass:"filter-item",staticStyle:{width:"200px"},attrs:{clearable:"",placeholder:"属性"},nativeOn:{keyup:function(t){return!t.type.indexOf("key")&&e._k(t.keyCode,"enter",13,t.key,"Enter")?null:e.handleFilter(t)}},model:{value:e.listQuery.searchVal,callback:function(t){e.$set(e.listQuery,"searchVal",t)},expression:"listQuery.searchVal"}}),a("el-button",{directives:[{name:"waves",rawName:"v-waves"}],staticClass:"filter-item",attrs:{type:"primary",icon:"el-icon-search"},on:{click:e.fetchData}},[e._v(" 搜索 ")]),a("el-button",{staticClass:"filter-item",staticStyle:{"margin-left":"10px"},attrs:{type:"primary",icon:"el-icon-edit"},on:{click:e.handleCreate}},[e._v(" 添加 ")])],1),a("el-table",{directives:[{name:"loading",rawName:"v-loading",value:e.listLoading,expression:"listLoading"}],attrs:{data:e.list,"element-loading-text":"Loading",border:"",fit:"","highlight-current-row":""}},[a("el-table-column",{attrs:{align:"center",label:"序号",width:"95"},scopedSlots:e._u([{key:"default",fn:function(t){return[e._v(e._s(t.$index+1))]}}])}),a("el-table-column",{attrs:{label:"属性",align:"center"},scopedSlots:e._u([{key:"default",fn:function(t){return[e._v(e._s(t.row.name))]}}])}),a("el-table-column",{attrs:{label:"属性值",width:"200",align:"center"},scopedSlots:e._u([{key:"default",fn:function(t){return[e._v(e._s(t.row.propValue)+" ")]}}])}),a("el-table-column",{attrs:{label:"属性描述",align:"center"},scopedSlots:e._u([{key:"default",fn:function(t){return[e._v(e._s(t.row.description))]}}])}),a("el-table-column",{attrs:{label:"创建时间",width:"200",align:"center"},scopedSlots:e._u([{key:"default",fn:function(t){return[e._v(e._s(t.row.createTime))]}}])}),a("el-table-column",{attrs:{label:"操作",align:"center",width:"230","class-name":"small-padding fixed-width"},scopedSlots:e._u([{key:"default",fn:function(t){var i=t.row;return[a("el-button",{attrs:{type:"primary",size:"mini"},on:{click:function(t){return e.handleUpdate(i)}}},[e._v(" 编辑 ")]),"deleted"!==i.status?a("el-button",{attrs:{size:"mini",type:"danger"},on:{click:function(t){return e.handleDelete(i)}}},[e._v(" 删除 ")]):e._e()]}}])})],1),a("pagination",{directives:[{name:"show",rawName:"v-show",value:e.total>0,expression:"total>0"}],attrs:{total:e.total,page:e.listQuery.pageNo,limit:e.listQuery.pageSize},on:{"update:page":function(t){return e.$set(e.listQuery,"pageNo",t)},"update:limit":function(t){return e.$set(e.listQuery,"pageSize",t)},pagination:e.fetchData}}),a("el-dialog",{attrs:{title:e.textMap[e.dialogStatus],visible:e.dialogFormVisible,width:"800px"},on:{"update:visible":function(t){e.dialogFormVisible=t}}},[a("el-form",{ref:"dataForm",attrs:{rules:e.rules,model:e.temp,"label-position":"left","label-width":"100px"}},[a("el-form-item",{attrs:{label:"属性",prop:"name"}},[a("el-input",{staticStyle:{width:"80%"},attrs:{placeholder:"属性"},model:{value:e.temp.name,callback:function(t){e.$set(e.temp,"name",t)},expression:"temp.name"}})],1),a("el-form-item",{attrs:{label:"属性值",prop:"name"}},[a("el-input",{staticStyle:{width:"80%"},attrs:{placeholder:"属性值"},model:{value:e.temp.propValue,callback:function(t){e.$set(e.temp,"propValue",t)},expression:"temp.propValue"}})],1),a("el-form-item",{attrs:{label:"属性描述",prop:"description"}},[a("el-input",{staticStyle:{width:"80%"},attrs:{type:"textarea",rows:"2",placeholder:"属性描述"},model:{value:e.temp.description,callback:function(t){e.$set(e.temp,"description",t)},expression:"temp.description"}})],1)],1),a("div",{staticClass:"dialog-footer",attrs:{slot:"footer"},slot:"footer"},[a("el-button",{on:{click:function(t){e.dialogFormVisible=!1}}},[e._v(" 取消 ")]),a("el-button",{attrs:{type:"primary"},on:{click:function(t){"create"===e.dialogStatus?e.createData():e.updateData()}}},[e._v(" 确认 ")])],1)],1),a("el-dialog",{attrs:{visible:e.dialogPluginVisible,title:"Reading statistics"},on:{"update:visible":function(t){e.dialogPluginVisible=t}}},[a("el-table",{staticStyle:{width:"100%"},attrs:{data:e.pluginData,border:"",fit:"","highlight-current-row":""}},[a("el-table-column",{attrs:{prop:"key",label:"Channel"}}),a("el-table-column",{attrs:{prop:"pv",label:"Pv"}})],1),a("span",{staticClass:"dialog-footer",attrs:{slot:"footer"},slot:"footer"},[a("el-button",{attrs:{type:"primary"},on:{click:function(t){e.dialogPvVisible=!1}}},[e._v("Confirm")])],1)],1)],1)},l=[],o=a("b775");function r(e){return Object(o["a"])({url:"/api/devEnvSetting",method:"get",params:e})}function s(e){return Object(o["a"])({url:"/api/devEnvSetting",method:"put",data:e})}function u(e){return Object(o["a"])({url:"/api/devEnvSetting",method:"post",data:e})}function c(e){return Object(o["a"])({url:"/api/devEnvSetting",method:"delete",params:e})}function d(e){return Object(o["a"])({url:"api/devEnvSetting/list",method:"get",params:e})}var p=a("6724"),f=a("333d"),m={name:"DevEnvSetting",components:{Pagination:f["a"]},directives:{waves:p["a"]},filters:{statusFilter:function(e){var t={published:"success",draft:"gray",deleted:"danger"};return t[e]}},data:function(){return{list:null,listLoading:!0,total:0,listQuery:{pageNo:1,pageSize:10,searchVal:""},pluginTypeOptions:["reader","writer"],dialogPluginVisible:!1,pluginData:[],dialogFormVisible:!1,dialogStatus:"",textMap:{update:"Edit",create:"Create"},rules:{name:[{required:!0,message:"this is required",trigger:"blur"}],description:[{required:!0,message:"this is required",trigger:"blur"}]},temp:{id:void 0,name:"",description:""},visible:!0}},created:function(){this.fetchData()},methods:{fetchData:function(){var e=this;console.log(1),this.listLoading=!0,console.log(i),r(this.listQuery).then((function(t){var a=t.records,i=t.total;e.total=i,e.list=a,e.listLoading=!1}))},resetTemp:function(){this.temp={id:void 0,name:"",description:""}},handleCreate:function(){var e=this;this.resetTemp(),this.dialogStatus="create",this.dialogFormVisible=!0,this.$nextTick((function(){e.$refs["dataForm"].clearValidate()}))},createData:function(){var e=this;this.$refs["dataForm"].validate((function(t){t&&u(e.temp).then((function(){e.fetchData(),e.dialogFormVisible=!1,e.$notify({title:"Success",message:"Created Successfully",type:"success",duration:2e3})}))}))},handleUpdate:function(e){var t=this;this.temp=Object.assign({},e),this.dialogStatus="update",this.dialogFormVisible=!0,this.$nextTick((function(){t.$refs["dataForm"].clearValidate()}))},updateData:function(){var e=this;this.$refs["dataForm"].validate((function(t){if(t){var a=Object.assign({},e.temp);s(a).then((function(){e.fetchData(),e.dialogFormVisible=!1,e.$notify({title:"Success",message:"Update Successfully",type:"success",duration:2e3})}))}}))},handleDelete:function(e){var t=this;console.log("删除");var a=[];a.push(e.id),c({idList:e.id}).then((function(e){t.fetchData(),t.$notify({title:"Success",message:"Delete Successfully",type:"success",duration:2e3})}))}}},g=m,h=a("2877"),v=Object(h["a"])(g,n,l,!1,null,null,null);t["default"]=v.exports},6724:function(e,t,a){"use strict";a("8d41");var i="@@wavesContext";function n(e,t){function a(a){var i=Object.assign({},t.value),n=Object.assign({ele:e,type:"hit",color:"rgba(0, 0, 0, 0.15)"},i),l=n.ele;if(l){l.style.position="relative",l.style.overflow="hidden";var o=l.getBoundingClientRect(),r=l.querySelector(".waves-ripple");switch(r?r.className="waves-ripple":(r=document.createElement("span"),r.className="waves-ripple",r.style.height=r.style.width=Math.max(o.width,o.height)+"px",l.appendChild(r)),n.type){case"center":r.style.top=o.height/2-r.offsetHeight/2+"px",r.style.left=o.width/2-r.offsetWidth/2+"px";break;default:r.style.top=(a.pageY-o.top-r.offsetHeight/2-document.documentElement.scrollTop||document.body.scrollTop)+"px",r.style.left=(a.pageX-o.left-r.offsetWidth/2-document.documentElement.scrollLeft||document.body.scrollLeft)+"px"}return r.style.backgroundColor=n.color,r.className="waves-ripple z-active",!1}}return e[i]?e[i].removeHandle=a:e[i]={removeHandle:a},a}var l={bind:function(e,t){e.addEventListener("click",n(e,t),!1)},update:function(e,t){e.removeEventListener("click",e[i].removeHandle,!1),e.addEventListener("click",n(e,t),!1)},unbind:function(e){e.removeEventListener("click",e[i].removeHandle,!1),e[i]=null,delete e[i]}},o=function(e){e.directive("waves",l)};window.Vue&&(window.waves=l,Vue.use(o)),l.install=o;t["a"]=l},"8d41":function(e,t,a){},e498:function(e,t,a){"use strict";a("1c18")}}]);