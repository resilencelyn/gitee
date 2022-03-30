<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog" style="width: auto;max-width: 600px">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="myModalLabel">
                    编辑用户
                </h4>
            </div>
            <div class="modal-body">
                <form class="form-horizontal" id="userform" user="form" style="margin-top: 20px;">
                    <input type="hidden" name="id" id="id" value="${(user.id)!}" />

                    <div class="form-group">
                        <label class="col-sm-3 control-label no-padding-right" > 用户名称 </label>
                        <div class="col-sm-9">
                            <input type="text" id="name" name="name" value="${(user.name)!}" placeholder="用户名称" class="col-xs-10 col-sm-5">
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-3 control-label no-padding-right" > 用户邮箱 </label>
                        <div class="col-sm-9">
                            <input type="text" name="email" value="${(user.email)!}" placeholder="用户邮箱" class="col-xs-10 col-sm-5">
                        </div>
                    </div>
                    <#if user??>
                        <input type="hidden" name="psw" placeholder="用户密码" class="col-xs-10 col-sm-5">
                    <#else >
                        <div class="form-group">
                            <label class="col-sm-3 control-label no-padding-right" > 用户密码 </label>
                            <div class="col-sm-9">
                                <input type="password"  name="psw" placeholder="用户密码" class="col-xs-10 col-sm-5">
                            </div>
                        </div>
                    </#if>

                    <div class="form-group">
                        <label class="col-sm-3 control-label no-padding-right" > 是否启用 </label>
                        <div class="col-sm-9 dataTables_length">
                            <select name="flag"  class="input-sm">
                                <option value="0" <#if user?? && user.flag==0>selected</#if>>禁用</option>
                                <option value="1" <#if user?? && user.flag==1>selected</#if>>启用</option>
                            </select>
                        </div>
                    </div>
                </form>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                ${authorities?seq_contains("/admin/user/edit")?string('<button class="btn btn-info" type="button" style="margin-left: 20px;" onclick="add()">
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
            alert("请输入用户名称");
            return;
        }
        if(!vali($("input[name=email]"))){
            alert("请输入用户邮箱");
            return;
        }
        if($("#id").val()=="" && !vali($("input[name=psw]"))){
            alert("请输入用户密码");
            return;
        }

        quickAjax({
            url: '/admin/user/edit',
            method:"POST",
            data:{
                id:$("#id").val(),
                name:$("#name").val(),
                psw : $("input[name=psw]").val(),
                email:$("input[name=email]").val(),
                flag:$("select[name=flag]").val()
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
</body>

</html>
