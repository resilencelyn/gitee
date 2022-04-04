<!DOCTYPE html>
<html lang="en">
<head>
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
    <meta http-equiv="Content-Type" content="multipart/form-data; charset=utf-8" />
    <meta charset="utf-8" />
    <title>资源列表</title>

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

    <!-- 模态框（Modal） -->
    <div class="modal fade" id="addJarModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="padding-top: 8%">
        <div class="modal-dialog">
            <div class="modal-content">
                <div class="modal-header">
                    <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                    <h4 class="modal-title" id="myModalLabel">jar上传</h4>
                </div>
                <div class="modal-body">
                    <form id="uploadForm">
                        <input type="file" name="jarFile" class="file-loading" accept=".jar" ><br>
                        <button type="button" class="btn btn-default" data-dismiss="modal">关闭</button>
                        <input type="button" id="uploadSubmit" value="提交" class="btn btn-primary" onclick="upload()">
                    </form>
                </div>
            </div>

            <div class="progress" style="display:none;height: 40px;">
                <div class="progress-bar" role="progressbar" aria-valuenow="60" aria-valuemin="0" aria-valuemax="100" style="width: 0%;font-size: 20px"></div>
            </div>
        </div>
    </div>

    <div class="page-content">
        <div class="col-xs-12"><a href="#">
            <button class="btn btn-white btn-info btn-bold" data-toggle="modal" data-target="#addJarModal">
                <i class="ace-icon fa fa-pencil-square-o bigger-120 blue"></i>
                新增
            </button></a>
            <div class="table-header" style="margin-top: 10px;">
                资源列表
            </div>
            <!-- div.table-responsive -->

            <!-- div.dataTables_borderWrap -->
            <div>
                <div id="dynamic-table_wrapper" class="dataTables_wrapper form-inline no-footer">
                    <form method="get" action="/jars/list" id="sform">
                    <div class="row"><div class="col-xs-300" >
                        <div class="dataTables_length" id="dynamic-table_length">
                            <label>每页显示 <select name="pageSize" aria-controls="dynamic-table" class="form-control input-sm">
                                <option <#if pageSize==10>selected</#if>  value="10">10</option>
                                <option <#if pageSize==25>selected</#if> value="25">25</option>
                                <option <#if pageSize==50>selected</#if> value="50">50</option>
                                <option <#if pageSize==100>selected</#if> value="100">100</option></select> 条</label>
                        </div>
                        <div id="dynamic-table_filter" >
                                <div id="dynamic-table_filter">
                                    <label>文件名称:
                                        <input type="search" name="fileName" value="${jarListRequest.fileName!}" class="form-control input-sm" placeholder="" aria-controls="dynamic-table">
                                    </label>
                                    <label style="margin-left: 30px">上传时间:
                                        <input type="date" value="${jarListRequest.startTime!}" name="startTime" >-
                                        <input type="date" value="${jarListRequest.endTime!}" name="endTime" >
                                    </label>
                                    <button type="button" class="btn btn-primary" style="margin-left: 30px" onclick="searchJar()">搜索</button>
                                </div>
                        </div>
                    </div>
                    </div>
                    </form>
                    <table id="dynamic-table" class="table table-striped table-bordered table-hover dataTable no-footer" role="grid" aria-describedby="dynamic-table_info">
                        <thead>
                        <tr role="row">
                         <#--   <th class="center sorting_disabled" rowspan="1" colspan="1" aria-label="">
                                <label class="pos-rel">
                                    <input type="checkbox" class="ace" id="check_all">
                                    <span class="lbl"></span>
                                </label>
                            </th>-->
                            <th class="sorting_disabled col-xs-3" tabindex="0"  rowspan="1" colspan="1">文件名称</th>
                            <th class="sorting_disabled col-xs-3" tabindex="0"  rowspan="1" colspan="1" >文件标识</th>
                            <th class="hidden-480 sorting_disabled" tabindex="0"  rowspan="1" colspan="1" >上传时间</th>
                            <th class="sorting_disabled" rowspan="1" colspan="2" aria-label="">操作</th>
                        </tr>
                        </thead>

                        <tbody>
                        <#list jars as jar>

                        <tr role="row" class="odd">
                         <#--   <td class="center">
                                <label class="pos-rel">
                                    <input type="checkbox" name="checkbox" value="${jar.id}" class="ace">
                                    <span class="lbl"></span>
                                </label>
                            </td>-->
                            <td>${jar.fileName}</td>
                            <td>${jar.jobKey}</td>
                            <td class="hidden-480">
                                <#if jar.createTime??>${jar.createTime?string("yyyy-MM-dd HH:mm:ss")}<#else >null</#if>
                            </td>

                            <td>
                                <div class="hidden-sm hidden-xs action-buttons">
                        ${authorities?seq_contains("/jars/delete")?string('<a class="red" onclick="del(${jar.id})">
                                        <i class="ace-icon fa fa-trash-o bigger-130"></i>
                                    </a>','')}

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


    function searchJar(){
        var fileName = $("input[name=fileName]").val();
        var startTime = $("input[name=startTime]").val();
        var endTime = $("input[name=endTime]").val();
        location.href="/jars/list?fileName="+fileName + "&startTime=" +startTime + "&endTime="+endTime;
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
            url:"/jars/list",params:"?pageSize=${pageSize}&fileName=${jarListRequest.fileName}&startTime=${jarListRequest.startTime}&endTime=${jarListRequest.endTime}",pakey:"page"});
    });

    /**
     * 删除jar
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
                            url: '/jars/delete',
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

    /**
     * 文件上传返回
     */
    $("#uploadForm").load(function(){
        //获取到的是json的字符串
        var bodyString = $(this).contents().find("body").text();
        //json字符串转换成json对象
        var bodyJson = $.parseJSON(bodyString);
        alert(bodyJson.code);
        if (bodyJson.code == 1){
            alert("上传成功",function(){
                location.reload();
            });
        }
    })

    function upload() {
        var formData = new FormData(document.getElementById("uploadForm"));
        $.ajax({
            type: "POST",
            enctype:'multipart/form-data',
            url: '/jars/upload',
            data: formData,
            cache:false,
            processData:false,
            contentType:false,
            error:function(result){
            },
            success: function(result){
                alert(result.msg);
                location.reload();
            },
            xhr: function () {
                var xhr = $.ajaxSettings.xhr();
                if (xhr.upload) {
                    //处理进度条的事件
                    xhr.upload.addEventListener("progress", progressHandle, false);
                    //开始显示进度条
                    beginUpload();
                    return xhr;
                }
            }
        });
    }

    function beginUpload() {
        $(".modal-content").css("display","none");
        $(".progress").css("display","block");
    }

    //进度条更新
    function progressHandle(e) {
        var percent = e.loaded / e.total * 100;
        percent = Math.round(percent);
        $("div[class=progress-bar]").css("width",percent + "%").text(percent + "%");
    };
</script>
</body>
</html>