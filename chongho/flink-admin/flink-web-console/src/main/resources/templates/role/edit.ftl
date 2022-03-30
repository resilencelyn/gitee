<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog" style="width: auto;max-width: 600px">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="myModalLabel">
                    编辑角色
                </h4>
            </div>
            <div class="modal-body">
                <form class="form-horizontal" id="roleform" role="form" style="margin-top: 20px;">
                    <input type="hidden" name="id" id="id" value="${(role.id)!}" />
                    <div class="form-group">
                        <label class="col-sm-3 control-label no-padding-right" > 角色名称 </label>
                        <div class="col-sm-9">
                            <input type="text" id="name" name="name" value="${(role.name)!}" placeholder="角色名称" class="col-xs-10 col-sm-5">
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-3 control-label no-padding-right" > 角色描述 </label>
                        <div class="col-sm-9">
                            <textarea class="form-control" name="description" id="form-field-8" placeholder="角色描述" style="margin: 0px -0.015625px 0px 0px; height: 150px; width: 409px;">${(role.description)!}</textarea>
                        </div>
                    </div>
                </form>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                ${authorities?seq_contains("/admin/role/edit")?string('<button class="btn btn-info" type="button" style="margin-left: 20px;" onclick="add()">
                    <i class="ace-icon fa fa-check bigger-110"></i>
                    保存
                </button>','')}
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>


<script>
    function add() {
        if(!vali($("#name"))){
            alert("请输入角色名称");
            return;
        }
        quickAjax({
            url: '/admin/role/edit',
            method:"POST",
            data:{
                id:$("#id").val(),
                name:$("#name").val(),
                description:$("#form-field-8").val()
            },
            success: function (response) {
                if (response.code == 1){
                    alert("更新成功",function(){
                        location.reload();
                    });
                }
            },
            error: function (response) {
                alert("链接服务器失败");
            }
        });
    }
</script>
