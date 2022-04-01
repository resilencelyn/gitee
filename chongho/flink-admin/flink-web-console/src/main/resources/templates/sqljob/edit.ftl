<!DOCTYPE html>
<html lang="en">
<head>
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
    <meta charset="utf-8" />
    <title>Treeview - Ace Admin</title>

    <meta name="description" content="with selectable elements and custom icons" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0" />

    <!-- bootstrap & fontawesome -->
    <link rel="stylesheet" href="/assets/css/bootstrap.min.css" />
    <link rel="stylesheet" href="/assets/font-awesome/4.5.0/css/font-awesome.min.css" />

    <!-- page specific plugin styles -->

    <!-- text fonts -->
    <link rel="stylesheet" href="/assets/css/fonts.googleapis.com.css" />

    <!-- ace styles -->
    <link rel="stylesheet" href="/assets/css/ace.min.css" class="ace-main-stylesheet" id="main-ace-style" />

    <!--[if lte IE 9]>
    <link rel="stylesheet" href="/assets/css/ace-part2.min.css" class="ace-main-stylesheet" />
    <![endif]-->
    <link rel="stylesheet" href="/assets/css/ace-skins.min.css" />
    <link rel="stylesheet" href="/assets/css/ace-rtl.min.css" />

    <!--[if lte IE 9]>
    <link rel="stylesheet" href="/assets/css/ace-ie.min.css" />
    <![endif]-->

    <!-- inline styles related to this page -->

    <!-- ace settings handler -->
    <script src="/assets/js/ace-extra.min.js"></script>

    <!-- HTML5shiv and Respond.js for IE8 to support HTML5 elements and media queries -->

    <!--[if lte IE 8]>
    <script src="/assets/js/html5shiv.min.js"></script>
    <script src="/assets/js/respond.min.js"></script>
    <![endif]-->
</head>

<body class="no-skin">

<div class="main-container ace-save-state" id="main-container">
    <script type="text/javascript">
        try{ace.settings.loadState('main-container')}catch(e){}
    </script>


            <div class="page-content">
                <div>
                    <button class="btn btn-white btn-warning btn-bold" onclick="javascript:history.back();">
                        <i class="ace-icon fa fa-chevron-left bigger-120 orange"></i>返回上一页</button>
                ${authorities?seq_contains("/sqljob/edit")?string('<button class="btn btn-white btn-info btn-bold" onclick="edit()">
                        <i class="ace-icon fa fa-floppy-o bigger-120 blue"></i>保存</button>
                    </button>','')}
                </div>
                        <div class="row">
                            <div class="col-sm-3" style="width: 100%">
                                <input type="hidden" name="id" id="id" value="${(job.id)!}" />
                                <div class="widget-box widget-color-blue2">
                                    <div class="widget-header">
                                        <h4 class="widget-title lighter smaller">作业编辑</h4>
                                    </div>
                                    <form class="form-horizontal" role="form">
                                        <div class="form-group" style="padding-top: 10px">
                                            <label class="col-sm-2 control-label">任务名称</label>
                                            <div class="col-sm-10">
                                                <input class="form-control" id="jobName" type="text" name="jobName" value="${(job.jobName)!}">
                                            </div>
                                        </div>
                                        <div class="form-group">
                                            <label class="col-sm-2 control-label">运行集群</label>
                                            <div class="col-sm-10">
                                                <select id="flinkColonyId" name="flinkColonyId"  class="form-control" <#if job?? && job.status!=0 >disabled="disabled" </#if>>
                                                    <#list clusterList as fc>
                                                        <option value="${(fc.id)!}" <#if job?? && job.flinkColonyId== fc.id >selected</#if>>${(fc.name)!}</option>
                                                    </#list>
                                                </select>
                                            </div>
                                        </div>
                                        <div class="form-group">
                                            <label class="col-sm-2 control-label">并行度</label>
                                            <div class="col-sm-10">
                                                <input type="text" name="parallelism" value="${(job.parallelism)!}" class="form-control">
                                            </div>
                                        </div>
                                        <div class="form-group" id="savepointPathDiv">
                                            <label class="col-sm-2 control-label" > 保存点路径 </label>
                                            <div class="col-sm-10">
                                                <select id="savepointPath" name="savepointPath"  class="form-control" >
                                                    <option value="" >请选择保存点</option>
                                                    <#list checkPointList as cp>
                                                        <option value="${(cp.location)!}" <#if job?? && cp.location?? && cp.location != '' && job.savepointPath?? && job.savepointPath?? && job.savepointPath == cp.location >selected</#if> >
                                                            ${(cp.location)!}_${cp.createTime?string("yyyy-MM-dd HH:mm:ss")}
                                                        </option>
                                                    </#list>
                                                </select>
                                            </div>
                                        </div>
                                        <div class="form-group">
                                            <label class="col-sm-2 control-label" > 运行SQL </label>
                                            <div class="col-sm-10">
                                                <textarea  style="background-color: #fffec9" id="args" name="args" class="form-control" rows="25">${(job.args)!}</textarea>
                                            </div>
                                        </div>
                                    </form>
                                </div>
                            </div>


                        </div>

                        <!-- PAGE CONTENT ENDS -->
            </div><!-- /.page-content -->


</div><!-- /.main-container -->

<!-- basic scripts -->

<!--[if !IE]> -->
<script src="/assets/js/jquery-2.1.4.min.js"></script>

<!-- <![endif]-->

<!--[if IE]>
<script src="/assets/js/jquery-1.11.3.min.js"></script>
<![endif]-->
<script type="text/javascript">
    if('ontouchstart' in document.documentElement) document.write("<script src='/assets/js/jquery.mobile.custom.min.js'>"+"<"+"/script>");
</script>
<script src="/assets/js/bootstrap.min.js"></script>

<!-- page specific plugin scripts -->
<script src="/assets/js/tree.min.js"></script>

<script src="/assets/js/spinbox.min.js"></script>
<script src="/assets/js/jquery.inputlimiter.min.js"></script>
<script src="/assets/js/jquery.maskedinput.min.js"></script>
<!-- ace scripts -->
<script src="/assets/js/ace-elements.min.js"></script>
<script src="/assets/js/ace.min.js"></script>

<script src="/assets/js/bootbox.js"></script>
<script src="/assets/js/utils.js"></script>
<!-- inline scripts related to this page -->
<script type="text/javascript">
    jQuery(document).ready(function(){
        if($("#id").val()==""){
            $("#savepointPathDiv").hide();
        }
    });

    function edit() {
        if($("#args").val() == ""){
            alert("运行Sql不能为空");
            return;
        }
        quickAjax({
            url: '/sqljob/update',
            method:"POST",
            data:{
                id:$("#id").val(),
                jobName:$("#jobName").val(),
                args : $("#args").val(),
                parallelism:$("input[name=parallelism]").val(),
                savepointPath:$("#savepointPath").val(),
                flinkColonyId:$("#flinkColonyId").val()
            },
            success: function (response) {
                if (response.code == 1){
                    alert("更新成功",function(){
                        history.back()
                    });
                }
            },
            error: function (response) {
                alert("链接服务器失败");
            }
        });
    }
</script>
</body>
</html>
