<!-- 模态框（Modal） -->
<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="myModalLabel">
                    flink集群编辑
                </h4>
            </div>
            <div class="modal-body">
                <form class="form-horizontal" id="userform" style="margin-top: 20px;">
                    <input type="hidden" name="id" id="id" value="${(cluster.id)!}" />

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 集群名称 </label>
                        <div class="col-sm-10">
                            <input type="text" id="colonyName" name="colonyName" value="${(cluster.name)!}"  class="col-xs-10 col-sm-10">
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 集群地址 </label>
                        <div class="col-sm-10">
                            <input type="text" id="colonyUrl" name="colonyUrl" value="${(cluster.url)!}"  class="col-xs-10 col-sm-10">
                        </div>
                    </div>
                </form>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                ${authorities?seq_contains("/cluster/edit")?string('<button class="btn btn-primary" type="button" style="margin-left: 20px;" onclick="add()">
                    <i class="ace-icon fa fa-check bigger-110"></i>
                    保存
                </button>','')}
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>
<script>

    function add() {
        if(!vali($("#colonyName"))){
            alert("集群名称不能为空");
            return;
        }
        if(!vali($("#colonyUrl"))){
            alert("集群地址不能为空");
            return;
        }
        quickAjax({
            url: '/cluster/update',
            method:"POST",
            data:{
                id:$("#id").val(),
                name:$("#colonyName").val(),
                url:$("#colonyUrl").val()
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
