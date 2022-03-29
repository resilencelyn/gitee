<!-- 模态框（Modal） -->
<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="myModalLabel">
                    数据源编辑
                </h4>
            </div>
            <div class="modal-body">
                <form class="form-horizontal" id="userform" job="form" style="margin-top: 20px;">
                    <input type="hidden" name="id" id="id" value="${(dbSource.id)!}" />

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 数据库名 </label>
                        <div class="col-sm-10">
                            <input type="text" id="name" name="name" value="${(dbSource.name)!}"  class="col-xs-10 col-sm-10">
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 数据库类型 </label>
                        <div class="col-sm-10">
                            <select id="providerType" name="providerType"  class="input-group-lg">
                                    <option value="Mysql" <#if dbSource?? && dbSource.providerType == "Mysql" >selected</#if>>Mysql</option>
                                    <option value="Oracle" <#if dbSource?? && dbSource.providerType == "Oracle" >selected</#if>>Oracle</option>
                                    <option value="ElasticSearch" <#if dbSource?? && dbSource.providerType == "ElasticSearch" >selected</#if>>ElasticSearch</option>
                            </select>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 链接Url </label>
                        <div class="col-sm-10">
                            <textarea  id="url" name="url" class="form-control" rows="3">${(dbSource.url)!}</textarea>
                        </div>
                    </div>
                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 用户名 </label>
                        <div class="col-sm-10">
                            <input  type="text" id="username" name="username" placeholder="请输入用户名" class="col-xs-10 col-sm-5">
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 密码 </label>
                        <div class="col-sm-10">
                            <input type="text" id="password" name="password" placeholder="请输入密码" class="col-xs-10 col-sm-5">
                        </div>
                    </div>

                    <div class="form-group">
                        <label class="col-sm-2 control-label no-padding-right" > 描述 </label>
                        <div class="col-sm-10">
                            <input type="text" id="remark" name="remark" value="${(dbSource.remark)!}" class="col-xs-10 col-sm-10">
                        </div>
                    </div>
                </form>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                ${authorities?seq_contains("/dbSource/edit")?string('<button class="btn btn-primary" type="button" style="margin-left: 20px;" onclick="add()">
                    <i class="ace-icon fa fa-check bigger-110"></i>
                    保存
                </button>','')}
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>
<script>
    function add() {
        if($("#url").val() == ""){
            alert("数据库地址不能为空");
            return;
        }
        if(!vali($("#name"))){
            alert("数据库名称不能为空");
            return;
        }
        var providerType = $("#providerType").val();
        var dbSourceId = $("#id").val();
        var isAdd = dbSourceId == "" || dbSourceId == null;
        var url;
        if(isAdd){
            if(providerType != "ElasticSearch"){
                if(!vali($("#username"))){
                    alert("用户名不能为空");
                    return;
                }
                if(!vali($("#password"))){
                    alert("密码不能为空");
                    return;
                }
            }
            url = "/dbSource/add";
        }else{
            url = "/dbSource/update"
        }
        quickAjax({
            url: url ,
            method:"POST",
            data:{
                id:$("#id").val(),
                name:$("#name").val(),
                providerType:$("#providerType").val(),
                url:$("#url").val(),
                username : $("#username").val(),
                password : $("#password").val(),
                remark : $("#remark").val()
            },
            success: function (response) {
                if (response.code == 1){
                    alert("编辑成功",function(){
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
