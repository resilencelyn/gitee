var Fkreport = {
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
    set_remove: function (id) {
        var posX;
        var posY;
        obj = document.getElementById(id);
        obj.onmousedown = function (e) {
            posX = event.x - obj.offsetLeft; //获得横坐标x
            posY = event.y - obj.offsetTop; //获得纵坐标y
            document.onmousemove = mousemove; //调用函数，只要一直按着按钮就能一直调用
        }
        document.onmouseup = function () {
            console.log(31231)
            document.onmousemove = null; //鼠标举起，停止
            
            Fkreport.ajax_save()
        }
        function mousemove(ev) {
            if (ev == null) ev = window.event; //IE
            obj.style.left = (ev.clientX - posX) + "px";
            obj.style.top = (ev.clientY - posY) + "px";
            var fk_data = JSON.parse(localStorage.getItem("fk_data"));
            var fk_list_data = fk_data.f_list[id];
            fk_list_data.top = ev.clientY - posY;
            fk_list_data.left = ev.clientX - posX;
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
            Fkreport.dataSave({type: (data.d_type || 1), fun: data.fundata}, id, 'Chart');
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
            case 'Position':
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
            data: {data: localStorage.getItem("fk_data")},
            type: 'post',
            cache: true,
            dataType: 'json',
            success: function (ret) {
                console.log(ret)
                if (ret.code !== 0) {
                    layer.alert(ret.msg, {title: "错误信息", icon: 2});
                }
                layer.msg(ret.msg);
            },
            error: function () {
                layer.alert('请求出错！', {title: "错误信息", icon: 2});
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
                    layer.msg(ret.msg, {icon: 1, time: 1500}, function () {
                        location.reload();
                    });
                } else {
                    layer.alert(ret.msg, {title: "错误信息", icon: 2});
                }
            },
            error: function () {
                layer.alert('请求出错！', {title: "错误信息", icon: 2});
            }
        });
    },
    auto_size:function(dw,dh){
        $('#screen-container').css('height', $('body').height() -45+'px');
        $('.fk-design').css('width', $('body').width() - 500 +'px');
        $('.fk-design').css('height',  $('body').height()+'px');
        var w = $('.fk-design').width();//获取页面可视宽度
        $('#fkreport').css('transform', 'scale('+w/dw+','+w/dw+')');
    },
    set_att:function(obj, type = ''){
        $('.fk_element').removeClass('this_select');
        $('.fk_element .zoom_right,.fk_element .zoom_rb,.fk_element .zoom_bottom').remove();
        $(obj).addClass('this_select');
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
    set_select:function(id){
        $('#'+id).click();
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
            Fkreport.dataSave(form.val("example"), id);
        });
        layer.msg('调整成功！');
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
                layer.alert('请求出错！', {title: "错误信息", icon: 2});
            }
        });
    },
    layer_open:function(Url,title='新增'){
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