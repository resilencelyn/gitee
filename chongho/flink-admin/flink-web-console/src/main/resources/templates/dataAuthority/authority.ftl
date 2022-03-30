<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog" style="width: auto;max-width: 600px">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="myModalLabel">
                    责任人列表
                </h4>
            </div>
            <div class="modal-body">
                <div id="dynamic-table_wrapper" class="dataTables_wrapper form-inline no-footer">
                    <input type="hidden" name="dataId" id="dataId" value="${(dataId)!}" />
                    <input type="hidden" name="dataType" id="dataType" value="${(dataType)!}" />
                    <table id="dynamic-table" class="table table-striped table-bordered table-hover dataTable no-footer" role="grid" aria-describedby="dynamic-table_info">
                        <thead>
                        <tr role="row">
                            <th class="center sorting_disabled" rowspan="1" colspan="1" aria-label="">
                                <label class="pos-rel">
                                    <span class="lbl"></span>
                                </label>
                            </th>
                            <th class="sorting_disabled" tabindex="0"  rowspan="1" colspan="1">用户名</th>
                        </tr>
                        </thead>

                        <tbody>
                        <#list userDataAuthorities as user>
                            <tr role="row" class="odd">
                                <td class="center">
                                    <label class="pos-rel">
                                        <input type="checkbox" name="checkbox" <#if (user.authority != 0)>checked</#if> value="${user.id}" class="ace">
                                        <span class="lbl"></span>
                                    </label>
                                </td>
                                <td>${user.name}</td>
                            </tr>
                        </#list>
                        </tbody>
                    </table>
                </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">关闭
                </button>
                    ${authorities?seq_contains("/admin/user/role")?string('<button class="btn btn-white btn-info btn-bold" onclick="update()">
                        <i class="ace-icon fa fa-floppy-o bigger-120 blue"></i>保存</button>
                    </button>','')}
            </div>
        </div><!-- /.modal-content -->
    </div><!-- /.modal -->
</div>



<script type="text/javascript">

    function update(){
        var id = "";
        $("input[name=checkbox]").each(function () {
            if($(this).is(':checked')){
               id +=  $(this).val()+",";
            }
        });
        quickAjax({
            url: '/data-authority/save',
            method:"post",
            data:{
                dataType:$("#dataType").val(),
                dataId:$("#dataId").val(),
                userIds:id
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
