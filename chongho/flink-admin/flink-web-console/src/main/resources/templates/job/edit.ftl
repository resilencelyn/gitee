<!-- 模态框（Modal） -->
<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog">
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
                <form class="form-horizontal" id="userform" style="margin-top: 20px;">
                    <input type="hidden" name="id" id="id" value="${(job.id)!}" />

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 任务名称 </label>
                        <div class="col-sm-10">
                            <input type="text" id="jobName" name="jobName" value="${(job.jobName)!}"  class="col-xs-10 col-sm-10">
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 运行集群 </label>
                        <div class="col-sm-10">
                            <select id="flinkColonyId" name="flinkColonyId"  class="input-group-lg" <#if job?? && job.status!=0 >disabled="disabled" </#if>>
                                <#list flinkColonyConfigList as fc>
                                    <option value="${(fc.id)!}" <#if job?? && job.flinkColonyId== fc.id >selected</#if>>${(fc.name)!}</option>
                                </#list>
                            </select>
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 运行jar包 </label>
                        <div class="col-sm-10">
                            <select id="jarId" name="jarId"  class="input-group-lg">
                                <#list jarList as jar>
                                    <option value="${(jar.id)!}" <#if job?? && job.jarId== jar.id >selected</#if>>${(jar.fileName)!}</option>
                                </#list>
                            </select>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 运行类路径 </label>
                        <div class="col-sm-10">
                            <input type="text" id="entryClass" name="entryClass" value="${(job.entryClass)!}"  class="col-xs-10 col-sm-10">
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 目标数据库：</label>
                        <div class="col-sm-10">
                            <select id="targetDbId" name="targetDbId"  class="input-group-lg">
                                <option value="-1" >请选择数据库</option>
                                <#list dbSourceList as db>
                                    <option value="${(db.id)!}" <#if job?? && job.targetDbId?? &&job.targetDbId == db.id >selected</#if>>${(db.name)!}"["${(db.remark)!}"]"</option>
                                </#list>
                            </select>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 运行参数 </label>
                        <div class="col-sm-10">
                            <textarea  id="args" name="args" class="form-control" rows="6">${(job.args)!}</textarea>
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 并行度 </label>
                        <div class="col-sm-10">
                            <input type="text" name="parallelism" value="${(job.parallelism)!}" class="col-xs-10 col-sm-5">
                        </div>
                    </div>
                    <div class="form-group" id="savepointPathDiv">
                        <label class="col-sm-2 control-label no-padding-right" > 保存点路径 </label>
                        <div class="col-sm-10">
                            <select id="savepointPath" name="savepointPath"  class="input-group-lg" style="max-width: 400px">
                                <option value="" >请选择保存点</option>
                                <#list checkPointList as cp>
                                    <option value="${(cp.location)!}" <#if job?? && cp.location?? && cp.location != '' && job.savepointPath?? && job.savepointPath == cp.location >selected</#if> >
                                        ${(cp.location)!}_${cp.createTime?string("yyyy-MM-dd HH:mm:ss")}
                                    </option>
                                </#list>
                            </select>
                        </div>
                    </div>
                </form>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                ${authorities?seq_contains("/job/edit")?string('<button class="btn btn-primary" type="button" style="margin-left: 20px;" onclick="add()">
                    <i class="ace-icon fa fa-check bigger-110"></i>
                    保存
                </button>','')}
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>
<script>
    jQuery(document).ready(function(){
        if($("#id").val()==""){
            $("#savepointPathDiv").hide();
        }
    });

    function add() {
        if(!vali($("#entryClass"))){
            alert("运行类路径不能为空");
            return;
        }
        if($("#args").val() == ""){
            alert("运行参数不能为空");
            return;
        }
        quickAjax({
            url: '/job/update',
            method:"POST",
            data:{
                id:$("#id").val(),
                jobName:$("#jobName").val(),
                entryClass:$("#entryClass").val(),
                targetDbId:$("#targetDbId").val(),
                jarId:$("#jarId").val(),
                args : $("#args").val(),
                parallelism:$("input[name=parallelism]").val(),
                savepointPath:$("#savepointPath").val(),
                flinkColonyId:$("#flinkColonyId").val(),
                status:$("select[name=status]").val()
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
