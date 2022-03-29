<!-- 模态框（Modal） -->
<head xmlns:form="http://www.w3.org/1999/xhtml">
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
                    index管理
                </h4>
            </div>
            <div class="modal-body">
                <form class="form-horizontal" id="userform"  style="margin-top: 20px;">
                    <input type="hidden" name="dbId" id="dbId" value="${dbSource.id}" />
                    <input type="hidden" name="columnTypeSelectValue" id="columnTypeSelectValue" value="${(typeJson)!}" />
                    <select id="columnTypeSelect" style="display: none">
                        <option value="" selected>请选择数据类型</option>
                        <#list typeList as t>
                            <option value="${(t)!}" >${(t)!}</option>
                        </#list>
                    </select>
                    <input type="hidden" name="providerType" id="providerType" value="${dbSource.providerType}"/>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > ES源：</label>
                        <div class="col-sm-10">
                            <input type="text" value="${dbSource.name}_${(dbSource.remark)!}"  readonly class="col-xs-10 col-sm-10">
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 类型：</label>
                        <div class="col-sm-10">
                            <input type="radio" name="type" checked="checked" value = 1> 新增
                            <input type="radio" name="type" value = 2> 修改
                        </div>
                    </div>
                    <div class="form-group" id="tableInputDiv">
                        <label class="col-sm-2 control-label no-padding-right" > index名：</label>
                        <div class="col-sm-10">
                            <input type="text" id="tableName" name="tableName" class="col-xs-10 col-sm-10">
                        </div>
                    </div>
                    <div class="form-group" style="display: none" id="tableSelectDiv">
                        <label class="col-sm-2 control-label no-padding-right" > index名：</label>
                        <select class="input-group-lg" id="tableSelect" style="width: 150px;margin-left: 13px">
                            <option value="-1" >请选择index</option>
                            <#if dbTableList?exists && (dbTableList?size>0)>
                                <#list dbTableList as dt>
                                    <option value="${(dt.id)!}" comment = "${(dt.tableComment)!}">${(dt.tableName)!}</option>
                                </#list>
                            </#if>
                        </select>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 描述：</label>
                        <div class="col-sm-10">
                            <input type="text" id="tableComment" name="tableComment" class="col-xs-10 col-sm-10">
                        </div>
                    </div>

                    <table class="table table-bordered" id="jobType1Insert">
                        <caption>index字段</caption>
                        <span  class="glyphicon glyphicon-plus" aria-hidden="true" style="font-size: 10px;margin-left: 30px" onclick="addTableColumn()"></span>
                        <thead>
                        <tr>
                            <th>字段名</th>
                            <th>字段类型</th>
                            <th>描述</th>
                        </tr>
                        </thead>
                        <tbody id="tableColumnInfo">

                        </tbody>
                    </table>
                </form>

            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                ${authorities?seq_contains("/dbSource/addTable")?string('<button class="btn btn-primary" type="button" style="margin-left: 20px;" onclick="add()">
                    <i class="ace-icon fa fa-check bigger-110"></i>
                    保存
                </button>','')}
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>
<script>

    /*   function delColumn(){
           alert(this.innerHTML())
           $(this).empty();
       }*/

    $("input[name=type]").click(function () {
        var type = $("input[name=type]:checked").val();
        $("#tableColumnInfo").html("");
        $("#tableComment").val("");
        if(type == 1){
            $("#tableInputDiv").show();
            $("#tableSelectDiv").hide();
        }else if(type == 2){
            $("#tableInputDiv").hide();
            $("#tableSelectDiv").show();
            $("#tableSelect").find("option:selected").attr("selected", false);
            $("#tableSelect").find("option").first().attr("selected", true);
        }
    });

    function addTableColumn() {

        var columnTypeSelect = getColumnTypeSelect("");

        // var del = "<a class='red' onclick='delColumn()'><i class='ace-icon fa fa-trash-o bigger-130'></i></a>";

        var columns = "<tr class='columnAssociation' ><td><input type='text''  class='col-xs-10 col-sm-10'></td>";
        columns= columns + "<td>" + columnTypeSelect + "</td>";
        // columns= columns + "<td><input type='text''  class='col-xs-10 col-sm-10'>"+ del +"</td></tr>";
        columns= columns + "<td><input type='text''  class='col-xs-10 col-sm-10'></td></tr>";
        $("#tableColumnInfo").append(columns);
    }

    function add() {
        var jsonColumnAssociation = buildJsonColumnAssociation();

        var tableName;
        var type = $("input[name=type]:checked").val();
        if(type == 1){
            tableName = $("#tableName").val();
        }else if (type == 2){
            tableName = $("#tableSelect").find("option:selected").text();
        }
        if(tableName.trim() == ""){
            alert("请输入表名");
            return;
        }

        if(jsonColumnAssociation == ""){
            alert("请完善字段信息");
            return;
        }
        quickAjax({
            url: '/dbtable/addTableAndColumn',
            method:"POST",
            data:{
                dbId:$("#dbId").val(),
                tableName:tableName,
                tableComment:$("#tableComment").val(),
                columnAssociation:JSON.stringify(jsonColumnAssociation)
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

    function buildJsonColumnAssociation(){
        var columnAssociation = $(".columnAssociation");
        var len = columnAssociation.length;
        var jsonColumnAssociation = [];

        var columnName;
        var columnType;
        for(var i = 0;i < len;i++) {
            var columnAssociationElement = columnAssociation[i];
            columnName = columnAssociationElement.children[0].firstElementChild.value;
            columnType = columnAssociationElement.children[1].firstElementChild.value;
            if(columnName.trim() == "" || columnType == ""){
                return ""
            }
            var arr = {
                "columnName" : columnName,
                "columnType" : columnType,
                "columnComment" : columnAssociationElement.children[2].firstElementChild.value
            };
            jsonColumnAssociation.push(arr);
        }
        return jsonColumnAssociation;
    }


    /**
     * 源表改变
     */
    $('#tableSelect').change(function(){
        if($("input[name=type]:checked").val() == 2){
            var options=$("#tableSelect option:selected")
            $("#tableComment").val(options.attr("comment"));
            quickAjax({
                url: '/dbtablecolumn/listInfo',
                method:"GET",
                data:{
                    tableId:$(this).val(),
                },
                success: function (response) {
                    if (response.code == 1){
                        var obj = response.obj;
                        var columns;
                        var columnComment;
                        // var del = "<a class='red' ><i class='ace-icon fa fa-trash-o bigger-130'></i></a>";
                        for(var i in obj){
                            columnComment = obj[i].columnComment;
                            if(columnComment == undefined){
                                columnComment = "";
                            }
                            columns = "<tr class='columnAssociation' ><td><input type='text'' value = "+ obj[i].columnName +" class='col-xs-10 col-sm-10'></td>";
                            columns = columns + "<td>" + getColumnTypeSelect(obj[i].columnType) + "</td>";
                            columns = columns + "<td><input type='text'' value = '"+ columnComment +"'  class='col-xs-10 col-sm-10'></td></tr>";
                            $("#tableColumnInfo").append(columns);
                        }
                    }
                },
                error: function (response) {
                    alert("操作失败!");
                }
            });
        }
    });

    function getColumnTypeSelect(columnType) {
        var columnTypeJson = $("#columnTypeSelectValue").val().split(",");
        var select = "<select  class='input-group-lg' style='margin-left: 13px' >";
        if(columnType == ""){
            select = select + "<option value='' selected >请选择数据类型</option>";
        }else{
            select = select + "<option value='' >请选择数据类型</option>";
        }
        for(var i in columnTypeJson){
            select = select + " <option value='"+ columnTypeJson[i] +"'";
            if(columnTypeJson[i] == columnType){
                select = select + " selected ";
            }
            select = select + ">" + columnTypeJson[i] + "</option>"
        }
        var select = select + "</select>";
        return select;
    }

</script>
