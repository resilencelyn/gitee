<!-- 模态框（Modal） -->
<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="myModalLabel">
                    任务运行配置编辑
                </h4>
            </div>
            <div class="modal-body">
                <form class="form-horizontal" id="userform" style="margin-top: 20px;">
                    <input type="hidden" name="id" id="id" value="${(runConfig.id)!}" />

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 任务类型 </label>
                        <div class="col-sm-10">
                            <input type="text" id="jobType" name="jobType" value="${(runConfig.jobType)!}"  class="col-xs-10 col-sm-10">
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 运行类 </label>
                        <div class="col-sm-10">
                            <input type="text" id="entryClass" name="entryClass" value="${(runConfig.entryClass)!}"  class="col-xs-10 col-sm-10">
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 运行jar包 </label>
                        <div class="col-sm-10">
                            <select id="jarId" name="jarId"  class="input-group-lg">
                                <#list jarList as jar>
                                    <option value="${(jar.id)!}" <#if runConfig?? && runConfig.jarId== jar.id >selected</#if>>${(jar.fileName)!}</option>
                                </#list>
                            </select>
                        </div>
                    </div>
                </form>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                ${authorities?seq_contains("/runConfig/edit")?string('<button class="btn btn-primary" type="button" style="margin-left: 20px;" onclick="add()">
                    <i class="ace-icon fa fa-check bigger-110"></i>
                    保存
                </button>','')}
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>
<script>

    function add() {
        if(!vali($("#jobType"))){
            alert("任务类型不能为空");
            return;
        }
        if(!vali($("#entryClass"))){
            alert("运行类路径不能为空");
            return;
        }
        quickAjax({
            url: '/runConfig/update',
            method:"POST",
            data:{
                id:$("#id").val(),
                jobType:$("#jobType").val(),
                entryClass:$("#entryClass").val(),
                entryClass:$("#entryClass").val(),
                jarId:$("#jarId").val()
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
</script>
