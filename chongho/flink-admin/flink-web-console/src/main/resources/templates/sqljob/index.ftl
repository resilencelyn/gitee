<!DOCTYPE html>
<html lang="en">
<head>
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
    <meta charset="utf-8" />
    <title>SQL作业列表</title>

    <meta name="description" content="Static &amp; Dynamic Tables" />
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

    <style type="text/css">
       /* .modal-dialog{
            width: auto;
            max-width: 1300px;
        }*/
    </style>
</head>

<body class="no-skin">
<div class="main-container ace-save-state" id="main-container">
    <script type="text/javascript">
        try{ace.settings.loadState('main-container')}catch(e){}
    </script>


    <div class="modal" id="deadAdd" style="display: none">
    </div>

    <div class="modal fade" id="loadingModal">
        <div style="width: 200px;height:20px; z-index: 20000; position: absolute; text-align: center; left: 50%; top: 50%;margin-left:-100px;margin-top:-10px">
            <div class="progress progress-striped active" style="margin-bottom: 0;">
                <div class="progress-bar" style="width: 100%;"></div>
            </div>
            <h5>执行中...</h5>
        </div>
    </div>


    <div class="page-content">
        <div class="col-xs-12"><a href="/sqljob/edit">
            <button class="btn btn-white btn-info btn-bold">
                <i class="ace-icon fa fa-pencil-square-o bigger-120 blue"></i>
                新增
            </button></a>
            <div class="table-header" style="margin-top: 10px;">
                作业列表
            </div>

            <!-- div.table-responsive -->

            <!-- div.dataTables_borderWrap -->
            <div>
                <div id="dynamic-table_wrapper" class="dataTables_wrapper form-inline no-footer">
                    <form method="get" action="/job/list" id="sform">
                    <div class="row">
                        <div class="col-xs-300" >
                            <div class="dataTables_length" id="dynamic-table_length">
                                <label>每页显示 <select name="pageSize" aria-controls="dynamic-table" class="form-control input-sm">
                                        <option <#if pageSize==10>selected</#if>  value="10">10</option>
                                        <option <#if pageSize==25>selected</#if> value="25">25</option>
                                        <option <#if pageSize==50>selected</#if> value="50">50</option>
                                        <option <#if pageSize==100>selected</#if> value="100">100</option></select> 条</label>
                            </div>
                            <div class="col-xs-300">
                            <div id="dynamic-table_filter">
                                <label>任务名:
                                    <input type="search" name="searchJobName" value="${jobListRequest.jobName!}" class="form-control input-sm" placeholder="" aria-controls="dynamic-table">
                                </label>

                                <label style="margin-left: 30px">运行集群:
                                    <select name="searchFlinkColonyId"  class="input-group-lg">
                                        <option value=-1 ></option>
                                        <#list clusterList as fc>
                                            <option value="${(fc.id)!}" <#if jobListRequest.flinkColonyId== fc.id >selected</#if>>${(fc.name)!}</option>
                                        </#list>
                                    </select>
                                </label>

                                <label style="margin-left: 30px">状态
                                    <select name="searchStatus"  class="input-group-lg">
                                        <option value=-1 ></option>
                                         <option value=0 <#if jobListRequest.status == 0 >selected</#if>>canceled</option>
                                         <option value=1 <#if jobListRequest.status == 1 >selected</#if>>running</option>
                                         <option value=2 <#if jobListRequest.status == 2 >selected</#if>>restarting</option>
                                    </select>
                                </label>
                                <button type="button" class="btn btn-primary" style="margin-left: 30px" onclick="searchJob()">搜索</button>
                            </div>
                            </div>
                    </div>

                    </div>
                    </form>
                    <table id="dynamic-table" class="table table-striped table-bordered table-hover dataTable no-footer" role="grid" aria-describedby="dynamic-table_info">
                        <thead>
                        <tr role="row">
                            <th class="sorting_disabled col-xs-2" tabindex="0"  rowspan="1" colspan="1">任务名称</th>
                            <th class="sorting_disabled col-xs-1" tabindex="0"  rowspan="1" colspan="1">运行集群</th>
<#--                            <th class="sorting_disabled col-xs-3" tabindex="0"  rowspan="1" colspan="1" >运行参数</th>-->
                            <th class="sorting_disabled col-xs-1" tabindex="0"  rowspan="1" colspan="1">并行度</th>
                            <th class="sorting_disabled col-xs-1" tabindex="0"  rowspan="1" colspan="1">状态</th>
                            <th class="hidden-480 sorting_disabled" tabindex="0"  rowspan="1" colspan="1" >创建时间</th>
                            <th class="sorting_disabled" rowspan="1" colspan="2" aria-label="">操作</th>
                        </tr>
                        </thead>

                        <tbody>
                        <#list jobs as job>

                        <tr role="row" class="odd">
                            <td>${(job.jobName)!}</td>
                            <td>${job.flinkColonyName}</td>
                            <td>${job.parallelism}</td>
                            <td>
                                <#if job.status==1>
                                    <p class="green">RUNNING
                                <#elseif job.status==2>
                                    <p style="color: #FF9E08">RESTARTING
                                <#else>
                                    <p class="red">CANCELED
                                </#if>
                                </p>
                            </td>
                            <td>
                                <#if job.createTime??>${job.createTime?string("yyyy-MM-dd HH:mm:ss")}<#else >null</#if>
                            </td>
                            <td>
                                <#if loginUserId == job.createUserId || loginUserId == -1 || job.dataAuthority != 0>
                                    <div class="hidden-sm hidden-xs action-buttons">
                                        ${authorities?seq_contains("/sqljob/edit")?string('<a id="editjob_${job.id}" class="green" href="/sqljob/edit?id=${job.id}" title="编辑任务">
                                        <i class="ace-icon fa fa-pencil bigger-130"></i>
                                    </a>','')}
                                        <#if job.status == 0 || job.status == 3 || job.status == 4>
                                            ${authorities?seq_contains("/sqljob/run")?string(' <a class="green" onclick="run(${job.id})" title="运行任务">
                                                <i class="ace-icon fa fa-paper-plane bigger-130"></i>
                                            </a>','')}
                                            ${authorities?seq_contains("/sqljob/delete")?string('<a class="red" onclick="del(${job.id})" title="删除任务">
                                                <i class="ace-icon fa fa-trash-o bigger-130"></i>
                                            </a>','')}
                                        <#else >
                                            ${authorities?seq_contains("/sqljob/run")?string(' <a class="green" target="_blank" href="${job.flinkColonyUrl}/#/job/${job.jobId}/overview" title="任务运行详情">
                                                <i class="glyphicon glyphicon-eye-open"></i>
                                            </a>','')}
                                            ${authorities?seq_contains("/sqljob/run")?string(' <a class="green" target="_blank" onclick="savepoint(${job.id})" title="新增保存点">
                                                <i class="fa fa-camera-retro"></i>
                                            </a>','')}
                                            ${authorities?seq_contains("/sqljob/stop")?string(' <a class="red" target="_blank" onclick="stop(${job.id})" title="停止任务">
                                                <i class="glyphicon glyphicon-stop"></i>
                                            </a>','')}
                                        </#if>
                                       <#if loginUserId == job.createUserId>
                                           ${authorities?seq_contains("/job/editUser")?string('<a id="editUser_${job.id}" class="green" data-toggle="modal"  data-remote="/data-authority/listUserDataAuthority?dataType=1&dataId=${job.id}" title="负责人员">
                                                <i class="glyphicon glyphicon-user"></i>
                                            </a>','')}
                                       </#if>
                                    </div>
                                </#if>
                            </td>
                        </tr>
                        </#list>

                        </tbody>
                    </table>
                    <div class="row">
                        <div class="col-xs-6">
                            <div class="dataTables_info" id="dynamic-table_info" role="status" aria-live="polite">第${nowBegin}-${nowEnd}条，共${count}条
                            </div>
                        </div>
                        <div class="col-xs-6"><div class="dataTables_paginate paging_simple_numbers" id="dynamic-table_paginate">
                            <ul class="pagination" id="pagination"></ul>
                        </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>

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
<script src="/assets/js/jquery.dataTables.min.js"></script>
<script src="/assets/js/jquery.dataTables.bootstrap.min.js"></script>
<script src="/assets/js/dataTables.buttons.min.js"></script>
<script src="/assets/js/dataTables.select.min.js"></script>

<!-- ace scripts -->
<script src="/assets/js/ace-elements.min.js"></script>
<script src="/assets/js/ace.min.js"></script>
<script src="/assets/js/page.js" ></script>

<script src="/assets/js/bootbox.js"></script>
<script src="/assets/js/utils.js"></script>
<!-- inline scripts related to this page -->
<script type="text/javascript">

    function searchJob(){
        var jobName = $("input[name=searchJobName]").val();
        var status = $("select[name=searchStatus]").val();
        var flinkColonyId = $("select[name=searchFlinkColonyId]").val();
        location.href="/sqljob/list?jobName="+jobName+"&status="+status +"&flinkColonyId="+flinkColonyId;
    }

    jQuery(function($) {
        $("#check_all").change(function () {
            if($("#check_all").is(':checked')){
                $("input[name=checkbox]").prop('checked', true);
            }else{
                $("input[name=checkbox]").prop('checked', false);
            }
        });
        $("select[name=pageSize]").change(function () {
            $("#sform").submit();
        });

        $("input[name=checkbox]").change(function () {
            var isAll = true;
            $("input[name=checkbox]").each(function () {
                if(!$(this).is(':checked')){
                    isAll = false;
                    $("#check_all").prop('checked', false);
                    return;
                }
            });
            if(isAll){
                $("#check_all").prop('checked', true);
            }
        });
        new page({pageMain:"pagination",nowPage:${page!'1'},count:${count},pageSize:${pageSize!'10'},
            url:"/sqljob/list",params:"?pageSize=${pageSize}&jobName=${jobListRequest.jobName}"+
                "&status=${jobListRequest.status}&flinkColonyId=${jobListRequest.flinkColonyId}",pakey:"page"});
    });
    /**
     * 删除
     */
    function del(id) {
        bootbox.confirm({
                    message: "是否删除?",
                    buttons: {
                        confirm: {
                            label: "删除",
                            className: "btn-sm",
                        },
                        cancel: {
                            label: "取消",
                            className: "btn-sm btn-primary",
                        }
                    },
                    callback: function(result) {
                        if(result) {
                            quickAjax({
                                url: '/sqljob/delete',
                                method:"POST",
                                data:{
                                    id:id
                                },
                                success: function (response) {
                                    if (response.code == 1){
                                        alert("删除成功",function(){
                                            location.reload();
                                        });

                                    }
                                },
                                error: function (response) {
                                    alert("链接服务器失败");
                                }
                            });
                        }
                    }
                }
        );
    }
    /**
     * 批量删除
     */
    function delSelected(){
        var id = "";
        $("input[name=checkbox]").each(function () {
            if($(this).is(':checked')){
               id +=  $(this).val()+",";
            }
        });
        if(id != ""){
            del(id);
        }
    }

    function savepoint(id) {
        bootbox.confirm({
                message: "是否触发Savepoint?",
                buttons: {
                    confirm: {
                        label: "确定",
                        className: "btn-sm",
                    },
                    cancel: {
                        label: "取消",
                        className: "btn-sm btn-primary",
                    }
                },
                callback: function(result) {
                    if(result) {
                        $('#loadingModal').modal({backdrop: 'static', keyboard: false});
                        $("#loadingModal").modal('show');
                        $.ajax({
                            url: '/sqljob/savepoint',
                            method:"POST",
                            data:{
                                id:id
                            },
                            success: function (response) {
                                $("#loadingModal").modal('hide');
                                if (response.code == 1){
                                    alert("成功",function(){
                                        location.reload();
                                    });
                                }else{
                                    alert(response.msg,function(){
                                        location.reload();
                                    });
                                }
                            },
                            error: function (response) {
                                $("#loadingModal").modal('hide');
                                alert("处理失败");
                            }
                        });
                    }
                }
            }
        );
    }

    /**
     * 启动任务
     */
    function run(id) {
        bootbox.confirm({
                message: "是否启动?",
                buttons: {
                    confirm: {
                        label: "启动",
                        className: "btn-sm",
                    },
                    cancel: {
                        label: "取消",
                        className: "btn-sm btn-primary",
                    }
                },
                callback: function(result) {
                    if(result) {
                        $('#loadingModal').modal({backdrop: 'static', keyboard: false});
                        $("#loadingModal").modal('show');
                        $.ajax({
                            url: '/sqljob/run',
                            method:"POST",
                            data:{
                                id:id
                            },
                            success: function (response) {
                                $("#loadingModal").modal('hide');
                                if (response.code == 1){
                                    alert("启动成功",function(){
                                        location.reload();
                                    });
                                }else{
                                    alert(response.msg,function(){
                                        location.reload();
                                    });
                                }
                            },
                            error: function (response) {
                                $("#loadingModal").modal('hide');
                                alert("连接服务器失败",function(){
                                    location.reload();
                                });
                            }
                        });
                    }
                }
            }
        );
    }


    /**
     * 停止任务
     */
    function stop(id) {
        bootbox.confirm({
                message: "是否停止?",
                buttons: {
                    confirm: {
                        label: "停止",
                        className: "btn-sm",
                    },
                    cancel: {
                        label: "取消",
                        className: "btn-sm btn-primary",
                    }
                },
                callback: function(result) {
                    if(result) {
                        $('#loadingModal').modal({backdrop: 'static', keyboard: false});
                        $("#loadingModal").modal('show');
                        $.ajax({
                            url: '/sqljob/stop',
                            method:"POST",
                            data:{
                                id:id
                            },
                            success: function (response) {
                                $("#loadingModal").modal('hide');
                                if (response.code == 1){
                                    alert("停止成功",function(){
                                        location.reload();
                                    });
                                }else{
                                    alert(response.msg,function(){
                                        location.reload();
                                    });
                                }
                            },
                            error: function (response) {
                                $("#loadingModal").modal('hide');
                                alert("连接服务器失败",function(){
                                    location.reload();
                                });
                            }
                        });
                    }
                }
            }
        );
    }

    $("a[id^='editUser']").click(function (event) {
        //如果是a链接，阻止其跳转到url页面
        event.preventDefault();
        //获取url的值
        var id = event.currentTarget.id;
        //获取url的值
        var url = "/data-authority/listUserDataAuthority?dataType=1&dataId=" + id.split("_")[1];
        //将id为deadAdd的页面元素作为模态框激活
        $('#deadAdd').modal();
        //从url加载数据到模态框
        $('#deadAdd').load(url);
    })
</script>
</body>
</html>
