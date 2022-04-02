var Fkreport = {
    //基础数据初始化
    setgril: (width, height) => {  //格式化网格
        var pxwid = height / 90
        console.log(pxwid)
        $(".gril-fkreport").css({
            width: '100%',
            height: '100%',
            backgroundSize: `${pxwid}px ${pxwid}px`,
            backgroundImage: `-webkit-linear-gradient(top, transparent ${pxwid - 1}px, #e9ebed ${pxwid}px), -webkit-linear-gradient(left, transparent ${pxwid - 1}px, #e9ebed ${pxwid}px)`
        })
    },
    //基础数据初始化
    init: function (id) {
        return {
            id: id,//表单名称
            f_id: 'Fk' + Fkreport.dateFormat(new Date(), "mmssS"),//表单ID
            f_list: {
                //设计数据
            },
            chart: {
                //图表数据缓存
            },
            f_ver: 'v1.0'//表单设计器版本
        };
    },
    int_data: function (int_data, id, tplType = 'add') {
        if (int_data == null || int_data === 1) {
            localStorage.setItem("fk_data", JSON.stringify(Fkreport.init(id)));
        } else {
            localStorage.setItem("fk_data", JSON.stringify(int_data));
            Fkreport.recovery(int_data, tplType)
        }
    },
    recovery: function (data, tplType) {
        for (x in data.f_list) {
            var InitData = data.f_list[x];
            Fkreport.convertFun(InitData.f_type, InitData.id, InitData, 1, tplType);
        }
    },
    copyelement: () => {  //复制图层
        var id
        $(".fk_element").each((index, item) => {
            if ($(item).hasClass("this_select")) {
                id = $(item).attr("id")

            }
        })
        var alocadata = JSON.parse(localStorage.getItem("fk_data"))
        var copyData = Fkreport.dataSave("", id, "find_data")
        var keys = Fkreport.dateFormat(new Date(), "mmssS");
        keys = `${copyData.id.split("_")[0]}_${keys}`
        copyData.id = keys
        alocadata.f_list[keys] = copyData
        localStorage.setItem("fk_data", JSON.stringify(alocadata))
        Fkreport.convertFun(copyData.f_type, copyData.id, copyData, 1, "add");
        Fkreport.ajax_save()  //复制的图层同步到接口

        $(".fk_element").each((index, item) => {  //添加成功后自动选中该复制的图层
            if (copyData.id == $(item).attr("id")) {
                Fkreport.set_att(item,copyData.f_type)
            }
        })
    },
    deleteElment: () => {  //通过id删除指定图层

        layui.use('layer', function () {
            var layer = layui.layer;
            layer.confirm('确认删除该图层？', {
                btn: ['删除','取消'] //按钮
              }, () => {
              
                var id
                var deleteIndex = 0
                var selectInde = 0
                $(".fk_element").each((index, item) => {
                    if ($(item).hasClass("this_select")) {
                        id = $(item).attr("id")
                        deleteIndex = index
                    }
                })
        
                $(".fk-item").each((index, item) => {
                    if ($(item).attr("fk-id") == id) {
                        selectInde = index
                    }
                })
        
                layui.use(['form'], function () {  //清除掉表单信息
                    var form = layui.form
                    form.val('example', {
                        "left": "",
                        "top": "",
                        "zindex": "",
                        "id": "",
                        "f_title": "",
                        "f_size": "",
                        "f_color": "",
                        "f_type": "",
                        "f_height": "",
                        "f_width": "",
                        "fundata": "",
                        "r_data": "",
                        "d_type": "",
                    });
                });
                $(".fk_element").eq(deleteIndex).remove()  //删除图层元素
                $(".fk-item").eq(selectInde).remove()  //删除侧边栏的图层标识
                Fkreport.dataSave('', id, 'Del');  //数据同步到本地
                Fkreport.ajax_save();  //数据同步到线上
                layer.msg("已删除")
                Fkreport.setLinePosition(0,0)
              });
        });
       

    },
    set_remove: function (id) {
        var posX;
        var posY;
        obj = document.getElementById(id);
        obj.onmousedown = function (e) {
            if (e.which == 1) {
                posX = event.x - obj.offsetLeft; //获得横坐标x
                posY = event.y - obj.offsetTop; //获得纵坐标y
                document.onmousemove = mousemove; //调用函数，只要一直按着按钮就能一直调用
            }
        }
        document.onmouseup = function (e) {
            if (e.which == 1) {
                document.onmousemove = null; //鼠标举起，停止
                Fkreport.ajax_save()
            }

        }
        function mousemove(ev) {
            if (ev == null) ev = window.event; //IE
            obj.style.left = (ev.clientX - posX) + "px";
            obj.style.top = (ev.clientY - posY) + "px";
            var fk_data = JSON.parse(localStorage.getItem("fk_data"));
            var fk_list_data = fk_data.f_list[id];
            fk_list_data.top = ev.clientY - posY;
            fk_list_data.left = ev.clientX - posX;


            layui.use(['form'], function () {
                var form = layui.form
                form.val('example', {
                    "left": fk_list_data.left,
                    "top": fk_list_data.top
                });
            });
            Fkreport.setLinePosition(fk_list_data.left,fk_list_data.top)
            Fkreport.dataSave(fk_list_data, id, 'Position');
        }
    },
    convertFun: function (type, id, data, is_new = 1, tplType = 'add') {
        var html = '';
        var style = 'position:absolute;font-size:' + data.f_size + 'px;color:' + data.f_color + ';z-index:' + data.zindex + ';width:' + (data.f_width || 120) + ';height:' + (data.f_height || 120) + ';left:' + data.left + 'px;top:' + data.top + 'px;';
        var att = 'class="fk_element" id="' + id + '"  style="' + style + '"';
        if (tplType == 'add') {
            att += 'onclick=Fkreport.set_att(this,"' + type + '")';
        }
        switch (type) {
            case 'text':
                html = '<span ' + att + '>' + (data.f_title || '文本文字') + '</span>';
                break;
            case 'date':
                html = '<span ' + att + '>天气组件</span>';
                break;
            case 'img':
                html = '<span ' + att + '><img width="100%" height="100%" src="' + (data.f_title || '/static/fk/img.png') + '"></span>';
                break;
            case 'table':
                html = '<span ' + att + '><table><tr><td>id</td><td>内容</td></tr><tr><td>1</td><td>2</td></tr></table> </span>';
                break;
            case 'bar':
                html = '<span ' + att + '></span>';
                break;
            case 'line':
                html = '<span ' + att + '></span>';
                break;
            case 'pie':
                html = '<span ' + att + '></span>';
                break;
            case 'area':
                html = '<span ' + att + '></span>';
                break;
            case 'rosePlot':
                html = '<span ' + att + '></span>';
                break;
            case 'gauge':
                html = '<span ' + att + '></span>';
                break;
            case 'Liquid':
                html = '<span ' + att + '></span>';
                break;
            default:
        }
        if (is_new == 0) {
            return html;
        }
        $("#fkreport").append(html);
        if (type == 'bar' || type == 'line' || type == 'pie' || type == 'area' || type == 'rosePlot' || type == 'gauge' || type == 'Liquid') {
            Fkreport.dataSave({ type: (data.d_type || 1), fun: data.fundata }, id, 'Chart');
            if (tplType != 'add' && data.r_data != '' && data.r_data > 0) {
                Fkreport.runset(data.r_data + '00', type, id);
            } else {
                desk.int(type, id);
            }
        }
        if (type == 'date') {
            Fkreport.showTime(id);
        }
    },
    runset: function (time, type, id) {
        $('#' + id).empty();
        desk.int(type, id);
        setTimeout("Fkreport.runset('" + time + "','" + type + "','" + id + "')", time);
    },
    //本地数据保存方法参数
    dataSave: function (data, key, type = 'List') {
        var fk_data = JSON.parse(localStorage.getItem("fk_data"));
        switch (type) {
            case 'Position':   //执行的拖动位移
                fk_data.f_list[key] = data;
                break;
            case 'List':
                fk_data.f_list[key] = data;
                break;
            case 'Del':
                delete fk_data.f_list[key];
                break;
            case 'Chart':
                fk_data.chart[key] = data;
                break;
            case 'config':
                break;
            case 'find_data':
                return fk_data.f_list[key];
                break;
            default:
        }
        localStorage.setItem("fk_data", JSON.stringify(fk_data));

    },
    //保存提交
    ajax_save: function () {
        $.ajax({
            url: server_url,
            data: { data: localStorage.getItem("fk_data") },
            type: 'post',
            cache: true,
            dataType: 'json',
            success: function (ret) {
                if (ret.code !== 0) {
                    // layer.alert(ret.msg, {title: "错误信息", icon: 2});
                }
                // layer.msg(ret.msg);
            },
            error: function () {
                // layer.alert('请求出错！', {title: "错误信息", icon: 2});
            }
        });
    },
    //日期格式化
    dateFormat: function (oDate, fmt) {
        var o = {
            "M+": oDate.getMonth() + 1, //月份
            "d+": oDate.getDate(), //日
            "h+": oDate.getHours(), //小时
            "m+": oDate.getMinutes(), //分
            "s+": oDate.getSeconds(), //秒
            "q+": Math.floor((oDate.getMonth() + 3) / 3), //季度
            "S": oDate.getMilliseconds()//毫秒
        };
        if (/(y+)/.test(fmt)) {
            fmt = fmt.replace(RegExp.$1, (oDate.getFullYear() + "").substr(4 - RegExp.$1.length));
        }
        for (var k in o) {
            if (new RegExp("(" + k + ")").test(fmt)) {
                fmt = fmt.replace(RegExp.$1, (RegExp.$1.length === 1) ? (o[k]) : (("00" + o[k]).substr(("" + o[k]).length)));
            }
        }
        return fmt;
    },
    sAjax: function (url, data) {
        $.ajax({
            url: url,
            data: data,
            type: 'post',
            cache: true,
            dataType: 'json',
            success: function (ret) {
                if (ret.code === 0) {
                    layer.msg(ret.msg, { icon: 1, time: 1500 }, function () {
                        location.reload();
                    });
                } else {
                    layer.alert(ret.msg, { title: "错误信息", icon: 2 });
                }
            },
            error: function () {
                layer.alert('请求出错！', { title: "错误信息", icon: 2 });
            }
        });
    },
    setLinePosition(left,top){//设置辅助线的定位
        // left  是当前图层对于父元素的距离
        // top  同理
        $(".shu-line").css({
            left:left + 'px',
            height:top + 'px'
        })
        $(".hen-line").css({
            top:top + 'px',
            width:left + 'px'
        })
        $(".line-value").css({
            top:top + 'px',
            left:left + 'px'
        }).text(`${left},${top}`)

    },
    auto_size: function (dw, dh) {
        $('#screen-container').css('height', $('body').height() - 45 + 'px');
        $('.fk-design').css('width', $('body').width() - 500 + 'px');
        $('.fk-design').css('height', $('body').height() + 'px');
        var w = $('.fk-design').width();//获取页面可视宽度
        $('#fkreport').css('transform', 'scale(' + w / dw + ',' + w / dw + ')');
    },
    set_att: function (obj, type = '') {
        if ($(obj).hasClass("this_select")) {  //如果点击的是当前选中的就不要再执行下面下种的代码了
            return
        }
        $('.fk_element').removeClass('this_select');   //取消掉所有选中
        $('.fk_element .zoom_right,.fk_element .zoom_rb,.fk_element .zoom_bottom, .switted-c').remove();//取消四角上的点
        $(obj).addClass('this_select');  //给当前点击的添加选中
        $('#f_color_item,#f_title_item,#f_size_item,#pic').hide();
        if (type == 'text') {
            $('#f_title_item,#f_size_item,#f_color_item').show();
        }
        if (type == 'img') {
            $('#f_title_item,#pic').show();
        }
        if (type == 'date') {
            $('#f_color_item,#f_size_item').show();
        }
        Fkreport.set_remove($(obj).attr("id"));/*id激活可以移动属性*/
        $('#' + $(obj).attr("id")).l_zoom('free');/*id激活可以放大拖动*/

       
        layui.use(['form', 'layedit', 'laydate'], function () {
            var form = layui.form
            let data = Fkreport.dataSave('', $(obj).attr("id"), 'find_data');
            Fkreport.setLinePosition(data.left,data.top)
            form.val('example', {
                "left": data.left,
                "top": data.top,
                "zindex": data.zindex,
                "id": data.id,
                "f_title": data.f_title,
                "f_size": data.f_size,
                "f_color": data.f_color,
                "f_type": data.f_type,
                "f_height": data.f_height,
                "f_width": data.f_width,
                "fundata": data.fundata,
                "r_data": data.r_data,
                "d_type": data.d_type,
            });
            form.on('select(r_data)', function (data) {
                let f_id = $('#f_id').val();
                Fkreport.set_class(f_id, 'r_data', '', data.value);
            });
        });
    },
    set_select: function (id) {
        $('#' + id).click();
    },
    set_class: function (id, data, datatype, val) {
        if (id == '') {
            layer.msg('Sorry,未选中图层！');
            return;
        }
        if (datatype == 'class') {
            $('#' + id).css(data, val);
        }
        let f_type = $('#f_type').val();
        if (datatype == 'html') {
            if (f_type == 'text') {
                $('#' + id).html(val);
            } else {
                $('#' + id).html('<img width="' + (data.f_width || 50) + 'px" height="' + (data.f_height || 50) + 'px" src="' + val + '">');
            }
        }
        layui.use(['form', 'layedit', 'laydate'], function () {
            var form = layui.form
            var dataform = form.val("example")
            Fkreport.textAsyncInputUpdate(dataform, id)
            Fkreport.dataSave(form.val("example"), id);
        });
        layer.msg('调整成功！');
    },
    textAsyncInputUpdate: (dataform, id) => {
        $(`#${id}`).css({
            "fontSize": `${dataform.f_size}px`,
            "width": dataform.f_width + 'px',
            height: dataform.f_height + 'px'
        })
    },
    sFun: function (url, data, setActive) {
        $.ajax({
            url: url,
            data: data,
            type: 'post',
            dataType: 'json',
            success: function (ret) {
                setActive(ret);
                return ret;
            },
            error: function () {
                layer.alert('请求出错！', { title: "错误信息", icon: 2 });
            }
        });
    },
    layer_open: function (Url, title = '新增') {
        layer.open({
            type: 2,
            title: title,
            shadeClose: true,
            shade: false,
            maxmin: true, //开启最大化最小化按钮
            area: ['60%', '80%'],
            content: Url
        });
    },
    open_pic: function () {
        let f_id = $('#f_id').val();
        if (f_id == '') {
            layer.msg('Sorry,未选中图层！');
            return;
        }
        layer.open({
            type: 2,
            title: '设置图片',
            shadeClose: true,
            shade: false,
            maxmin: true,
            area: ['893px', '600px'],
            content: files_url
        });
    },
    showTime: function (dom, Type = 'datetime') {
        var myDate = new Date();
        var year = myDate.getFullYear();
        var month = myDate.getMonth() + 1;
        var date = myDate.getDate();
        var dayArray = new Array(7);
        dayArray[0] = "星期日";
        dayArray[1] = "星期一";
        dayArray[2] = "星期二";
        dayArray[3] = "星期三";
        dayArray[4] = "星期四";
        dayArray[5] = "星期五";
        dayArray[6] = "星期六";
        var day1 = myDate.getDay();
        var day = dayArray[day1];
        var hour = myDate.getHours();
        var minute = myDate.getMinutes();
        var second = myDate.getSeconds();
        var min = Fkreport.checkTime(minute);
        var sec = Fkreport.checkTime(second);
        var time1 = year + "年" + month + "月" + date + "日";
        var time2 = hour + "：" + min + "：" + sec;
        $('#' + dom).html(time1 + day + time2);
        setTimeout("Fkreport.showTime('" + dom + "')", 500);
    },
    checkTime: function (i) {
        if (i < 10) {
            i = "0" + i;
        }
        return i;
    }
}