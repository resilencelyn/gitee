<!DOCTYPE html>
<html lang="en">
<head>
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
    <meta charset="utf-8" />
    <title>任务运行配置信息</title>

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
        <div class="col-xs-12"><a id="addRunConfig" data-toggle="modal" data-remote="/runConfig/edit">
            <button class="btn btn-white btn-info btn-bold">
                <i class="ace-icon fa fa-pencil-square-o bigger-120 blue"></i>
                新增
            </button></a>
            <div class="table-header" style="margin-top: 10px;">
                任务运行配置列表
            </div>

            <!-- div.table-responsive -->

            <!-- div.dataTables_borderWrap -->
            <div>
                <div id="dynamic-table_wrapper" class="dataTables_wrapper form-inline no-footer">

                    <table id="dynamic-table" class="table table-striped table-bordered table-hover dataTable no-footer" role="grid" aria-describedby="dynamic-table_info">
                        <thead>
                        <tr role="row">
                            <th class="sorting_disabled col-xs-3" tabindex="0"  rowspan="1" colspan="1">任务类型</th>
                            <th class="sorting_disabled col-xs-3" tabindex="0"  rowspan="1" colspan="1">运行类路径</th>
                            <th class="sorting_disabled" rowspan="1" colspan="2" aria-label="">运行jar包</th>
                            <th class="sorting_disabled" rowspan="1" colspan="2" aria-label="">jar上传时间</th>
                        </tr>
                        </thead>
                        <tbody>
                        <#list runConfigs as runConfig>
                            <tr role="row" class="odd">
                                <td>${runConfig.jobType}</td>
                                <td>${runConfig.entryClass}</td>
                                <td>${runConfig.jarName}</td>
                                <td>
                                    <#if runConfig.jarTime??>${runConfig.jarTime?string("yyyy-MM-dd HH:mm:ss")}<#else >null</#if>
                                </td>
                                <td>
                                    <div class="hidden-sm hidden-xs action-buttons">
                                    ${authorities?seq_contains("/runConfig/edit")?string('<a id="editRunConfig_${runConfig.id}" class="green" data-toggle="modal"  data-remote="/runConfig/edit?id=${runConfig.id}">
                                        <i class="ace-icon fa fa-pencil bigger-130"></i>
                                        </a>','')}
                                    </div>
                                </td>
                            </tr>
                        </#list>

                        </tbody>
                    </table>
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

    $("a[id^='editRunConfig']").click(function (event) {
        //如果是a链接，阻止其跳转到url页面
        event.preventDefault();
        //获取url的值
        var id = event.currentTarget.id;
        //获取url的值
        var url = "/runConfig/edit?id=" + id.split("_")[1];
        //将id为deadAdd的页面元素作为模态框激活
        $('#deadAdd').modal();
        //从url加载数据到模态框
        $('#deadAdd').load(url);
    })

    $('#addRunConfig').click(function (event) {
        //如果是a链接，阻止其跳转到url页面
        event.preventDefault();
        //获取url的值
        var url = $('#addRunConfig').data('remote')||$('#addRunConfig').attr('href');
        //将id为deadAdd的页面元素作为模态框激活
        $('#deadAdd').modal();
        //从url加载数据到模态框
        $('#deadAdd').load(url);
    })
</script>
</body>
</html>
