<div id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="width: auto">
    <div class="modal-dialog" style="width: auto;max-width: 600px">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">
                    &times;
                </button>
                <h4 class="modal-title" id="myModalLabel">
                    角色列表
                </h4>
            </div>
            <div class="modal-body">
                <div id="dynamic-table_wrapper" class="dataTables_wrapper form-inline no-footer">
                    <table id="dynamic-table" class="table table-striped table-bordered table-hover dataTable no-footer" role="grid" aria-describedby="dynamic-table_info">
                        <thead>
                        <tr role="row">
                            <th class="center sorting_disabled" rowspan="1" colspan="1" aria-label="">
                                <label class="pos-rel">
                                    <input type="checkbox" class="ace" id="check_all">
                                    <span class="lbl"></span>
                                </label>
                            </th>
                            <th class="sorting_disabled" tabindex="0" rowspan="1" colspan="1" >ID</th>
                            <th class="sorting_disabled" tabindex="0"  rowspan="1" colspan="1">角色名</th>
                            <th class="hidden-480 sorting_disabled" tabindex="0" rowspan="1" colspan="1">创建者</th>
                            <th class="sorting_disabled" tabindex="0"  rowspan="1" colspan="1" >描述</th>
                            <th class="hidden-480 sorting_disabled" tabindex="0"  rowspan="1" colspan="1" >创建时间</th>
                        </tr>
                        </thead>

                        <tbody>
                        <#list roles as role>
                            <tr role="row" class="odd">
                                <td class="center">
                                    <label class="pos-rel">
                                        <input type="checkbox" name="checkbox" <#if (role.userid != 0)>checked</#if> value="${role.id}" class="ace">
                                        <span class="lbl"></span>
                                    </label>
                                </td>

                                <td>
                                    <a href="#">${role.id}</a>
                                </td>
                                <td>${role.name}</td>
                                <td class="hidden-480">${role.createuser!}</td>
                                <td>${role.description!}</td>
                                <td class="hidden-480">
                                    ${role.createtime?string("yyyy-MM-dd HH:mm:ss")}
                                </td>
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
    jQuery(function($) {
        $("#check_all").change(function () {
            if($("#check_all").is(':checked')){
                $("input[name=checkbox]").prop('checked', true);
            }else{
                $("input[name=checkbox]").prop('checked', false);
            }
        });
        $("select[name=pageSize]").change(function () {
            $("#sform").submit();
        });

        $("input[name=checkbox]").change(function () {
            var isAll = true;
            $("input[name=checkbox]").each(function () {
                if(!$(this).is(':checked')){
                    isAll = false;
                    $("#check_all").prop('checked', false);
                    return;
                }
            });
            if(isAll){
                $("#check_all").prop('checked', true);
            }
        });

    });

    /**
     * 批量删除
     */
    function update(){
        var id = "";
        $("input[name=checkbox]").each(function () {
            if($(this).is(':checked')){
               id +=  $(this).val()+",";
            }
        });
        quickAjax({
            url: '/admin/user/role',
            method:"POST",
            data:{
                ids:id,
                userId:${userid}
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
