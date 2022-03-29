<!DOCTYPE html>
<html lang="en">
<head>
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
    <meta charset="utf-8" />
    <title>数据源列表</title>

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
</head>

<body class="no-skin">
<div class="main-container ace-save-state" id="main-container">
    <script type="text/javascript">
        try{ace.settings.loadState('main-container')}catch(e){}
    </script>

    <div class="modal" id="deadAdd" style="display: none">
    </div>

    <div class="page-content">
        <div class="col-xs-12"><a id="adddb" data-toggle="modal" data-remote="/dbSource/edit">
            <button class="btn btn-white btn-info btn-bold">
                <i class="ace-icon fa fa-pencil-square-o bigger-120 blue"></i>
                新增
            </button></a>
            <div class="table-header" style="margin-top: 10px;">
                数据源列表
            </div>

            <!-- div.table-responsive -->

            <!-- div.dataTables_borderWrap -->
            <div>
                <div id="dynamic-table_wrapper" class="dataTables_wrapper form-inline no-footer">
                    <form method="get" action="/job/list" id="sform">
                    <div class="row"><div class="col-xs-6" style="width: 100%">
                        <div class="dataTables_length" id="dynamic-table_length">
                            <label>每页显示 <select name="pageSize" aria-controls="dynamic-table" class="form-control input-sm">
                                <option <#if pageSize==10>selected</#if>  value="10">10</option>
                                <option <#if pageSize==25>selected</#if> value="25">25</option>
                                <option <#if pageSize==50>selected</#if> value="50">50</option>
                                <option <#if pageSize==100>selected</#if> value="100">100</option></select> 条</label>
                        </div>
                    </div>
                        <div class="col-xs-300">
                            <div id="dynamic-table_filter">
                                <label>数据库名称:
                                    <input type="text" name="searchName" value="${dbListRequest.name!}" class="form-control input-sm" placeholder="" aria-controls="dynamic-table">
                                </label>
                                <label style="margin-left: 30px">类型：
                                    <select name="searchProviderType"  class="input-group-lg">
                                        <option value="" ></option>
                                        <option value="Mysql" <#if dbListRequest.providerType == 'Mysql' >selected</#if>>Mysql</option>
                                        <option value="Oracle" <#if dbListRequest.providerType == 'Oracle' >selected</#if>>Oracle</option>
                                        <option value="ElasticSearch" <#if dbListRequest.providerType == 'ElasticSearch' >selected</#if>>ElasticSearch</option>
                                    </select>
                                </label>
                                <label  style="margin-left: 30px">链接地址:
                                    <input type="text" name="searchUrl" value="${dbListRequest.url!}" class="form-control" style="width: 300px">
                                </label>
                                <label  style="margin-left: 30px">描述:
                                    <input type="text" name="searchRemark" value="${dbListRequest.remark!}" class="form-control" style="width: 200px">
                                </label>
                                <button type="button" class="btn btn-primary" style="margin-left: 30px" onclick="searchDb()">搜索</button>
                            </div>
                        </div>
                    </div>
                    </form>
                    <table id="dynamic-table" class="table table-striped table-bordered table-hover dataTable no-footer" role="grid" aria-describedby="dynamic-table_info">
                        <thead>
                        <tr role="row">
                            <th class="sorting_disabled col-xs-1" tabindex="0"  rowspan="1" colspan="1">数据库名称</th>
                            <th class="sorting_disabled col-xs-1" tabindex="0"  rowspan="1" colspan="1">类型</th>
                            <th class="sorting_disabled col-xs-1" tabindex="0"  rowspan="1" colspan="1">连接地址</th>
                            <th class="sorting_disabled col-xs-2" tabindex="0"  rowspan="1" colspan="1">描述</th>
                            <th class="hidden-480 sorting_disabled" tabindex="0"  rowspan="1" colspan="1" >创建时间</th>
                            <th class="sorting_disabled" rowspan="1" colspan="3" aria-label="">操作</th>
                        </tr>
                        </thead>

                        <tbody>
                        <#list dbSourceList as db>
                            <tr role="row" class="odd">
                                <td>${db.name}</td>
                                <td>${db.providerType}</td>
                                <td>${db.url}</td>
                                <td>${db.remark}</td>
                                <td class="hidden-480">
                                    <#if db.createTime??>${db.createTime?string("yyyy-MM-dd HH:mm:ss")}<#else >null</#if>
                                </td>
                                <td>
                                    <div class="hidden-sm hidden-xs action-buttons">
                                        ${authorities?seq_contains("/dbSource/edit")?string('<a id="editdb_${db.id}" class="green" data-toggle="modal" data-remote="/dbSource/edit?id=${db.id}">
                                        <i class="ace-icon fa fa-pencil bigger-130"></i>
                                    </a>','')}
                                        ${authorities?seq_contains("/dbSource/delete")?string('<a class="red" onclick="del(${db.id})">
                                        <i class="ace-icon fa fa-trash-o bigger-130"></i>
                                    </a>','')}
                                    <#if db.providerType == 'Mysql'>
                                            ${authorities?seq_contains("/dbSource/syncDbInfo")?string('<a class="bule" onclick="syncDbInfo(${db.id})">
                                            <i class="glyphicon glyphicon-refresh bigger-130"></i>
                                        </a>','')}
                                    </#if>
                                    <#if db.providerType == 'ElasticSearch'>
                                        ${authorities?seq_contains("/dbSource/addTable")?string('<a id="addTable_${db.id}" class="blue" data-toggle="modal" data-remote="/dbSource/addTable?id=${db.id}"">
                                        <i class="glyphicon glyphicon-list-alt"></i>
                                    </a>','')}
                                    </#if>
                                    </div>
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


    function searchDb(){
        var name = $("input[name=searchName]").val();
        var providerType = $("select[name=searchProviderType]").val();
        var url = $("input[name=searchUrl]").val();
        var remark = $("input[name=searchRemark]").val();
        location.href="/dbSource/list?name="+name + "&providerType=" +providerType + "&url="+url + "&remark="+remark;
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
            url:"/dbSource/list",params:"?pageSize=${pageSize}&name=${dbListRequest.name}" +
            "&providerType=${dbListRequest.providerType}&url=${dbListRequest.url}&remark=${dbListRequest.remark}",pakey:"page"});
    });

    /**
     * 删除数据源
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
                            url: '/dbSource/delete',
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
                                alert("操作失败!");
                            }
                        });
                    }
                }
            }
        );
    }


    /**
     * 同步数据表和列信息
     */
    function syncDbInfo(id) {
        quickAjax({
            url: '/dbSource/syncDbInfo',
            method:"POST",
            data:{
                id:id
            },
            success: function (response) {
                if (response.code == 1){
                    alert("同步成功",function(){
                        location.reload();
                    });
                }
            },
            error: function (response) {
                alert("操作失败!");
            }
        });
    }

    $("a[id^='editdb']").click(function (event) {
        //如果是a链接，阻止其跳转到url页面
        event.preventDefault();
        var id = event.currentTarget.id;
        //获取url的值
        var url = "/dbSource/edit?id=" + id.split("_")[1];
        //将id为deadAdd的页面元素作为模态框激活
        $('#deadAdd').modal();
        //从url加载数据到模态框
        $('#deadAdd').load(url);
    });

    $("a[id^='addTable']").click(function (event) {
        //如果是a链接，阻止其跳转到url页面
        event.preventDefault();
        var id = event.currentTarget.id;
        //获取url的值
        var url = "/dbSource/addTable?id=" + id.split("_")[1];
        //将id为deadAdd的页面元素作为模态框激活
        $('#deadAdd').modal();
        //从url加载数据到模态框
        $('#deadAdd').load(url);
    });

    $('#adddb').click(function (event) {
        //如果是a链接，阻止其跳转到url页面
        event.preventDefault();
        //获取url的值
        var url = $('#adddb').data('remote')||$('#adddb').attr('href');
        //将id为deadAdd的页面元素作为模态框激活
        $('#deadAdd').modal();
        //从url加载数据到模态框
        $('#deadAdd').load(url);
    });
</script>
</body>
</html>
