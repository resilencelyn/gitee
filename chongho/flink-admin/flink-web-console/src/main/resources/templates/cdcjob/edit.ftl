<!-- 模态框（Modal） -->
<head>
    <style type="text/css">
        #tableColumnInfo tr td {
            padding-top: 2px;
            padding-bottom: 2px;
        }
        .form-group{
            margin-bottom: 6px;
            padding-top: 0px;
        }
    </style>
</head>
<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog" style="width: auto;max-width: 600px">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="myModalLabel">
                    作业编辑
                </h4>
            </div>
            <div class="modal-body">
                <form class="form-horizontal" id="userform"  style="margin-top: 20px;">
                    <input type="hidden" name="id" id="id" value="${(cdcJob.id)!}" />
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 任务名称：</label>
                        <div class="col-sm-10">
                            <input type="text" id="jobName" name="jobName" value="${(cdcJob.jobName)!}"  class="col-xs-10 col-sm-10">
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 运行集群 </label>
                        <div class="col-sm-10">
                            <select id="flinkColonyId" name="flinkColonyId"  class="input-group-lg" <#if cdcJob?? && cdcJob.status!=0 >disabled="disabled" </#if>>
                                <#list clusterList as fc>
                                    <option value="${(fc.id)!}" <#if cdcJob?? && cdcJob.flinkColonyId== fc.id >selected</#if>>${(fc.name)!}</option>
                                </#list>
                            </select>
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 任务类型：</label>
                        <div class="col-sm-10">
                            <input type="radio" name="jobType"  <#if jobType == 1>checked="checked"</#if> value = 1> 单表同步
                            <input type="radio" name="jobType"  style="margin-left: 30px"  <#if jobType == 2>checked="checked"</#if> value = 2> 多表同步
                        </div>
                    </div>
                        <span  class="glyphicon glyphicon-plus" aria-hidden="true" style="font-size: 10px;margin-left: 30px" onclick="addSourceDb()" id = "addDbFlag"></span>
                    <div id="sdbList">
                        <#list sourceDbList as sd>
                            <div class="form-group form-inline sourceDbDiv" dbIndex = "${sd_index}">
                                <label  class="col-sm-2 control-label no-padding-right" > 源数据库：</label>
                                <select class="input-group-lg sourceDb" id="sourceDb_${sd_index}" style="margin-left: 13px;width: 200px">
                                    <option value="-1" >请选择数据库</option>
                                    <#list dbSourceList as db>
                                        <option value="${(db.id)!}" <#if sd.dbId?? && sd.dbId== db.id >selected</#if> >${(db.name)!}_${(db.id)!}"["${(db.remark)!}"]"</option>
                                    </#list>
                                </select>
                                <label style="margin-left: 13px"> 表：</label>
                                <select class="input-group-lg sourceTable" id="sourceTable_${sd_index}" style="width: 150px">
                                    <option value="-1" >请选择表</option>
                                    <#if sd.dbTableList?exists && (sd.dbTableList?size>0)>
                                        <#list sd.dbTableList as sdt>
                                            <option value="${(sdt.id)!}" <#if sd.tableId == sdt.id >selected</#if>>${(sdt.tableName)!}</option>
                                        </#list>
                                    </#if>
                                </select>

                                <label class="priK" style="margin-left: 13px"> 主键：</label>
                                <input class="priK" type="text" style="width: 100px" id="sourceTablePriKey_${sd_index}" value="${(sd.tablePriKey)!}">
                            </div>
                        </#list>
                    </div>

                    <div class="form-group form-inline">
                        <label class="col-sm-2 control-label no-padding-right" > 目标数据库：</label>
                        <select id="targetDbId" name="targetDbId"  class="input-group-lg" style="margin-left: 13px;width: 200px">
                            <option value="-1" >请选择数据库</option>
                            <#list dbSourceList as db>
                                <option value="${(db.id)!}" <#if cdcJob?? && cdcJob.targetDbId == db.id >selected</#if>>${(db.name)!}_${(db.id)!}"["${(db.remark)!}"]"</option>
                            </#list>
                        </select>
                        <label style="margin-left: 13px"> 表：</label>
                        <select id="targetTableId" name="targetTableId"  class="input-group-lg" style="width: 150px">
                            <option value="-1" >请选择表</option>
                            <#list targetTableList as tt>
                                <option value="${(tt.id)!}" <#if cdcJob?? && cdcJob.targetTableId == tt.id >selected</#if>>${(tt.tableName)!}</option>
                            </#list>
                        </select>
                    </div>
                    <div class="form-group form-inline">
                        <label class="col-sm-2 control-label no-padding-right" > 并行度：</label>
                        <input type="text" name="parallelism" value="${(cdcJob.parallelism)!}"  style="margin-left: 13px">
                    </div>
                    <div class="form-group form-inline" id="savepointPathDiv">
                        <label class="col-sm-2 control-label no-padding-right" > 保存点路径：</label>
                        <select id="savepointPath" name="savepointPath"  class="input-group-lg" style="margin-left: 13px;max-width: 400px">
                            <option value="" >请选择保存点</option>
                            <#list checkPointList as cp>
                                <option value="${(cp.location)!}" <#if cdcJob?? && cp.location?? && cp.location != '' && cdcJob.savepointPath == cp.location >selected</#if> >
                                    ${(cp.location)!}_${cp.createTime?string("yyyy-MM-dd HH:mm:ss")}
                                </option>
                            </#list>
                        </select>
                    </div>
                    <div class="form-group form-inline">
                        <label class="col-sm-2 control-label no-padding-right" > 启动模式：</label>
                        <select class="input-group-lg" id="scanStartupMode" name="scanStartupMode" style="margin-left: 13px">
                            <option value="initial" <#if cdcJob?? && cdcJob.scanStartupMode == 'initial' >selected</#if>>initial</option>
                            <option value="latest-offset" <#if cdcJob?? && cdcJob.scanStartupMode == 'latest-offset' >selected</#if>>latest-offset</option>
                        </select>
                        <label  style="margin-left: 13px" > 跳过记录：</label>
                        <input type="checkbox" id="skipped_c" value="c" <#if cdcJob?? && cdcJob.skippedOperations?contains("c")>checked="checked"</#if> style="margin-left: 13px" > 插入/创建
                        <input type="checkbox" id="skipped_u" value="u" <#if cdcJob?? && cdcJob.skippedOperations?contains("u")>checked="checked"</#if> style="margin-left: 13px"> 更新
                        <input type="checkbox" id="skipped_d" value="d" <#if cdcJob?? && cdcJob.skippedOperations?contains("d")>checked="checked"</#if> style="margin-left: 13px"> 删除
                    </div>
                    <div class="form-group" >
                        <label class="col-sm-2 control-label no-padding-right" > 目标表主键:</label>
                        <input type="text" id="primaryColumn" name="primaryColumn" value="${(cdcJob.primaryColumn)!}"  class="col-xs-6 col-sm-6" style="margin-left: 13px">
                    </div>

                    <table class="table table-bordered" id="jobType1Insert">
                        <caption>对应字段</caption>
                        <thead>
                        <tr>
                            <th>源字段</th>
                            <th>源字段类型</th>
                            <th>赋值字段</th>
                        </tr>
                        </thead>
                        <tbody id="tableColumnInfo">
                        <#if columnAssociationList?exists && (columnAssociationList?size>0)>
                            <#list columnAssociationList as scl>
                                <tr class='columnAssociation' id="${scl.id}">
                                    <td id='source_${scl.id}'>${scl.columnName}</td>
                                    <td id='sourceType_${scl.id}'>${scl.columnType}</td>
                                    <td id='target_${scl.id}' class='targetColumn'>
                                        <select  class='input-group-lg' style='margin-left: 13px'>
                                            <option value='' >请选择字段</option>
                                            <#if targetDbTableColumnList?exists && (targetDbTableColumnList?size>0)>
                                                <#list targetDbTableColumnList as tcl>
                                                    <option value='${tcl.columnName}' <#if scl.targetColumnName?? && scl.targetColumnName == tcl.columnName >selected</#if> >${tcl.columnName}</option>
                                                </#list>
                                            </#if>
                                        </select>
                                    </td>
                                </tr>
                            </#list>
                        </#if>
                        </tbody>
                    </table>

                    <div class="form-group" id="jobType2Insert">
                        <label class="col-sm-2 control-label no-padding-right" > 插入sql语句: </label>
                        <label id ="insertTips" class="col-sm-30" style="color: red;margin-left: 30px"> 表名规则请遵循: dbName_dbId_tableName? </label>
                        <div class="col-sm-10">
                            <textarea  id="insertSql" name="insertSql" class="form-control" rows="10">${(cdcJob.columnAssociation)!}</textarea>
                        </div>
                    </div>
                </form>

            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                ${authorities?seq_contains("/cdcjob/edit")?string('<button class="btn btn-primary" type="button" style="margin-left: 20px;" onclick="add()">
                    <i class="ace-icon fa fa-check bigger-110"></i>
                    保存
                </button>','')}
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>
<script>

    var targetColumn = "";

    jQuery(document).ready(function(){
        var jobType = $("input[name=jobType]:checked").val();
        changeJobType(jobType);
        if($("#id").val()!=""){
            $("input[name=jobType]").attr("disabled",true);
            $("#addDbFlag").hide();
            if(jobType == 1){
                targetColumn = $(".targetColumn").html();
                targetColumn = targetColumn.replace("selected","");
            }
        }else{
            $("#savepointPathDiv").hide();
        }
    });

    function addSourceDb() {
        var dbIndex = parseInt($("#sdbList").children(".sourceDbDiv:last-child").attr("dbIndex"));

        var dbList = $("#sourceDb_"+dbIndex).html();
        dbList = dbList.replace("selected","");

        dbIndex = dbIndex + 1;

        var addDb = "<div class='form-group form-inline sourceDbDiv' dbIndex = "+ dbIndex +">";


        addDb = addDb + "<label  class='col-sm-2 control-label no-padding-right'> 源数据库：</label>";
        addDb = addDb + " <select class='input-group-lg sourceDb' id='sourceDb_"+ dbIndex +"' style='margin-left: 13px;width: 200px'>"+dbList+"</select>";


        addDb = addDb + " <label style='margin-left: 13px'> 表：</label>";
        addDb = addDb + "<select class='input-group-lg sourceTable' id='sourceTable_"+ dbIndex +"' style='width: 150px'>";
        addDb = addDb +  "<option value='-1' >请选择表</option></select>";


        addDb = addDb + "<label class='priK' style='margin-left: 13px'> 主键：</label>";
        addDb = addDb + "<input class='priK' type='text' style='width: 100px' id='sourceTablePriKey_"+dbIndex+"'>";

        addDb = addDb + "</div>";

        $("#sdbList").append(addDb);

        $("#sourceDb_"+dbIndex).bind("click",function () {
            var index = $(this).attr("id").split("_")[1];
            getDbTable($(this).val(),"sourceTable_"+index)
            $("#tableColumnInfo").html("");
        })
    }



    $("input[name=jobType]").click(function () {
        changeJobType($("input[name=jobType]:checked").val());
    });

    $("#insertTips").mouseover(function (){
        $("#insertSql").text(" INSERT  INTO dbName_dbId_tableName ( id,field1,field2,fieldN) SELECT id,field1,field2,fieldN FROM dbName_dbId_tableName");
    });

    $("#insertTips").mouseout(function (){
        $("#insertSql").text("");
    });

    function changeJobType(jobType) {
        if(jobType == 1) {
            $("#jobType1Insert").show();
            $("#jobType2Insert").hide();
            $("#addDbFlag").hide();
            $(".priK").hide();
            $(".modal-dialog").css("max-width","600px");
        }else{
            $("#jobType2Insert").show();
            $("#addDbFlag").show();
            $("#jobType1Insert").hide();
            $(".priK").show();
            $(".modal-dialog").css("max-width","800px");
        }
    }


    function add() {
        let jobType = $("input[name=jobType]:checked").val();
        let targetDbId = $("#targetDbId").val();
        let targetTableId = $("#targetTableId").val();
        let jsonSourceInfo = buildJsonSoueceInfo();
        let jobName = $("#jobName").val();

        if (jobName == "" || jobName == null) {
            alert("请完善任务名称");
            return;
        }

        if(jsonSourceInfo == null){
            alert("请完善数据源信息");
            return;
        }

        if(!checkDb()){
            return;
        }

        var columnAssociation;
        if(jobType == 1){
            var jsonColumnAssociation = buildJsonColumnAssociation();
            if(jsonColumnAssociation.length == 0){
                alert("请选择需要同步目标的对应字段");
                return;
            }
            columnAssociation = JSON.stringify(jsonColumnAssociation);
        }else{
            columnAssociation = $("#insertSql").val()
        }

        var skippedOperations = "";
        if($('#skipped_c').is(':checked')){
            skippedOperations = skippedOperations + "c,";
        }
        if($('#skipped_u').is(':checked')){
            skippedOperations = skippedOperations + "u,";
        }
        if($('#skipped_d').is(':checked')){
            skippedOperations = skippedOperations + "d,";
        }
        skippedOperations = skippedOperations.slice(0,skippedOperations.length-1)
        quickAjax({
            url: '/cdcjob/update',
            method:"POST",
            data:{
                id:$("#id").val(),
                jobType:jobType,
                jobName:$("#jobName").val(),
                sourceInfo:JSON.stringify(jsonSourceInfo),
                targetDbId:targetDbId,
                targetTableId : targetTableId,
                columnAssociation : columnAssociation,
                scanStartupMode:$("#scanStartupMode").val(),
                skippedOperations:skippedOperations,
                parallelism:$("input[name=parallelism]").val(),
                savepointPath:$("#savepointPath").val(),
                flinkColonyId:$("#flinkColonyId").val(),
                primaryColumn:$("input[name=primaryColumn]").val()
            },
            success: function (response) {
                if (response.code == 1){
                    alert("更新成功",function(){
                        location.reload();
                    });

                }
            },
            error: function (response) {
                alert("操作失败!");
            }
        });
    }

    function checkDb(){
        var targetDbId = $("#targetDbId").val();
        var targetTableId = $("#targetTableId").val();
        if(targetDbId == -1){
            alert("请选择目标数据库");
            return false;
        }
        if(targetTableId == -1){
            alert("请选择目标表");
            return false;
        }
        return true;
    }

    function buildJsonColumnAssociation(){
        var columnAssociation = $(".columnAssociation");
        var len = columnAssociation.length;
        var thisId;
        var jsonColumnAssociation = [];
        for(var i = 0;i < len;i++) {
            thisId = columnAssociation[i].id;
            if($("#target_"+thisId).find("select").val() == ""){
                continue;
            }
            var arr  =
                {
                    "sourceColumnName" : $("#source_"+thisId).html(),
                    "sourceColumnType" : $("#sourceType_"+thisId).html(),
                    "targetColumnName" : $("#target_"+thisId).find("select").val()
                };
            jsonColumnAssociation.push(arr);
        }
        return jsonColumnAssociation;
    }


    function buildJsonSoueceInfo(){
        var sourceInfo = $(".sourceDbDiv");
        var len = sourceInfo.length;
        var thisId;
        var jsonsoueceInfo = [];

        var dbId;
        var tableId;
        for(var i = 0;i < len;i++) {
            thisId = sourceInfo[i].getAttribute("dbIndex");
            dbId =  $("#sourceDb_"+thisId).val();
            tableId =  $("#sourceTable_"+thisId).val();
            if(dbId == "-1" || tableId == "-1"){
                return null;
            }
            var arr  =
                {
                    "dbId" : $("#sourceDb_"+thisId).val(),
                    // "dbName" : $("#sourceDb_"+thisId).find("option:selected").text(),
                    "tableId" : $("#sourceTable_"+thisId).val(),
                    "tablePriKey" : $("#sourceTablePriKey_"+thisId).val()
                    // "tableName" : $("#sourceTable_"+thisId).find("option:selected").text()
                };
            jsonsoueceInfo.push(arr);
        }
        return jsonsoueceInfo;
    }

    /**
     * 源库改变
     */
    $('.sourceDb').change(function(){
        var index = $(this).attr("id").split("_")[1];
        getDbTable($(this).val(),"sourceTable_"+index)
        $("#tableColumnInfo").html("");
    });


    /**
     * 目标库改变
     */
    $('#targetDbId').change(function(){
        getDbTable($(this).val(),"targetTableId")
        $(".targetColumn").html("");
        targetColumn = "";
    });

    /**
     * 目标表改变
     */
    $('#targetTableId').change(function(){
        if($("input[name=jobType]:checked").val() == 1){
            quickAjax({
                url: '/dbtablecolumn/listInfo',
                method:"GET",
                data:{
                    tableId:$(this).val(),
                },
                success: function (response) {
                    if (response.code == 1){
                        var obj = response.obj;
                        var options = "<select   class='input-group-lg' style='margin-left: 13px;height: 25px;padding-bottom: 2px;padding-top: 2px'>";
                        options = options + "<option value='' >请选择字段</option>"
                        for(var i in obj){
                            options = options + "<option value='"+obj[i].columnName+"' >"+obj[i].columnName+"</option>";
                        }
                        options = options + "</select>";
                        $(".targetColumn").html(options);
                        targetColumn = options;
                    }
                },
                error: function (response) {
                    alert("操作失败!");
                }
            });
        }
    });


    /**
     * 源表改变
     */
    $('.sourceTable').change(function(){
        if($("input[name=jobType]:checked").val() == 1){
            quickAjax({
                url: '/dbtablecolumn/listInfo',
                method:"GET",
                data:{
                    tableId:$(this).val(),
                },
                success: function (response) {
                    if (response.code == 1){
                        var obj = response.obj;
                        var columns = "";
                        var columnId;
                        for(var i in obj){
                            columnId = obj[i].id;
                            columns = columns + "<tr class='columnAssociation' id="+columnId+"><td id='source_"+columnId+"'>"+obj[i].columnName+"</td>";
                            columns = columns + "<td id='sourceType_"+columnId+"'>"+obj[i].columnType+"</td><td id='target_"+columnId+"' class='targetColumn'>"+targetColumn+"</td></tr>";
                        }
                        $("#tableColumnInfo").html(columns);
                    }
                },
                error: function (response) {
                    alert("操作失败!");
                }
            });
        }
    });

    /**
     * 返回数据库对应表
     * @param dbId
     * @param showId
     */
    function getDbTable(dbId,showId) {
        quickAjax({
            url: '/dbtable/listInfo',
            method:"GET",
            data:{
                dbId:dbId,
            },
            success: function (response) {
                if (response.code == 1){
                    var obj = response.obj;
                    var options = "<option value = '-1' >请选择表</option>";
                    for(var i in obj){
                        options = options + "<option value='"+obj[i].id+"' >"+obj[i].tableName+"</option>"
                    }
                    $("#"+showId).html(options);
                }
            },
            error: function (response) {
                alert("操作失败!");
            }
        });
    }

</script>
