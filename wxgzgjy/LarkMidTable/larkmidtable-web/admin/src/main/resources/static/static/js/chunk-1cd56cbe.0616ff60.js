(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([["chunk-1cd56cbe"],{"072a":function(e,t,a){},"21d5":function(e,t,a){"use strict";a("072a")},d764:function(e,t,a){"use strict";a.r(t);var n=function(){var e=this,t=e.$createElement,a=e._self._c||t;return a("div",{staticClass:"mycontent"},[a("el-button",{attrs:{icon:"el-icon-d-arrow-left",type:"info",plain:"",size:"small"},on:{click:function(t){return e.$router.go(-1)}}},[e._v("返回")]),e._v(" "),a("h2",[e._v("接口请求测试")]),e._v(" "),a("h4",[e._v("URL：")]),e._v(" "),a("el-input",{model:{value:e.url,callback:function(t){e.url=t},expression:"url"}}),e._v(" "),a("h4",{directives:[{name:"show",rawName:"v-show",value:0==e.previlege,expression:"previlege == 0"}]},[e._v("请求Header：")]),e._v(" "),a("el-form",{directives:[{name:"show",rawName:"v-show",value:0==e.previlege,expression:"previlege == 0"}],staticStyle:{width:"600px"},attrs:{"label-width":"150px",size:"medium"}},[a("el-form-item",{attrs:{label:"token:"}},[a("el-input",{model:{value:e.token,callback:function(t){e.token=t},expression:"token"}})],1)],1),e._v(" "),a("h4",[e._v("参数：")]),e._v(" "),a("el-form",{staticStyle:{width:"600px"},attrs:{"label-width":"150px",size:"medium"}},e._l(e.params,(function(t,n){return a("el-form-item",{key:n,staticStyle:{"margin-bottom":"5px"}},[a("template",{slot:"label"},[a("data-tag",{attrs:{name:t.name,type:t.type}})],1),e._v(" "),t.type.startsWith("Array")?e._e():a("el-input",{model:{value:t.value,callback:function(a){e.$set(t,"value",a)},expression:"item.value"}}),e._v(" "),a("div",{directives:[{name:"show",rawName:"v-show",value:t.type.startsWith("Array"),expression:"item.type.startsWith('Array')"}]},[e._l(t.values,(function(t,s){return a("div",{key:s},[a("el-input",{staticStyle:{width:"400px"},model:{value:t.va,callback:function(a){e.$set(t,"va",a)},expression:"childItem.va"}}),e._v(" "),a("el-button",{staticStyle:{"margin-left":"4px"},attrs:{slot:"append",icon:"el-icon-delete",type:"danger",circle:"",size:"mini"},on:{click:function(t){return e.deleteRow(n,s)}},slot:"append"})],1)})),e._v(" "),a("el-button",{attrs:{icon:"el-icon-plus",type:"primary",circle:"",size:"mini"},on:{click:function(t){return e.addRow(n)}}})],2)],2)})),1),e._v(" "),a("el-button",{on:{click:e.request}},[e._v("发送请求")]),e._v(" "),a("h4",[e._v("返回结果：")]),e._v(" "),a("el-table",{directives:[{name:"show",rawName:"v-show",value:e.showTable,expression:"showTable"}],attrs:{data:e.tableData,size:"mini",border:"",stripe:"","max-height":"700"}},e._l(e.keys,(function(e){return a("el-table-column",{key:e,attrs:{prop:e,label:e}})})),1),e._v(" "),a("el-input",{directives:[{name:"show",rawName:"v-show",value:!e.showTable,expression:"!showTable"}],staticClass:"my",attrs:{type:"textarea",autosize:{minRows:5,maxRows:20}},model:{value:e.response,callback:function(t){e.response=t},expression:"response"}}),e._v(" "),a("el-button",{staticClass:"button",attrs:{size:"small"},on:{click:e.format}},[e._v("JSON格式化")])],1)},s=[],r=(a("456d"),a("7f7f"),a("f559"),a("ac6a"),a("96cf"),a("1da1")),i={name:"request",data:function(){return{api:{},params:[],path:null,previlege:0,address:null,response:null,isSelect:null,keys:[],tableData:[],showTable:!1,token:null,url:""}},methods:{getDetail:function(){var e=Object(r["a"])(regeneratorRuntime.mark((function e(t){return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:case"end":return e.stop()}}),e)})));function t(t){return e.apply(this,arguments)}return t}(),getAddress:function(){var e=Object(r["a"])(regeneratorRuntime.mark((function e(){return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:case"end":return e.stop()}}),e)})));function t(){return e.apply(this,arguments)}return t}(),request:function(){this.response=null;var e={};this.params.forEach((function(t){if(t.type.startsWith("Array")){var a=t.values.map((function(e){return e.va}));e[t.name]=a}else e[t.name]=t.value}))},format:function(){var e=JSON.parse(this.response);this.response=JSON.stringify(e,null,4)},tableShow:function(){if(null!=this.response){var e=JSON.parse(this.response);e.success&&(this.tableData=e.data,e.data.length>0&&(this.keys=Object.keys(e.data[0]),this.showTable=!0))}},tableHide:function(){this.showTable=!1},addRow:function(e){this.params[e].values.push({va:null})},deleteRow:function(e,t){this.params[e].values.splice(t,1)}},created:function(){this.getDetail(this.$route.query.id),this.getAddress()},computed:{}},l=i,o=(a("21d5"),a("2877")),u=Object(o["a"])(l,n,s,!1,null,"66baf2c9",null);t["default"]=u.exports}}]);