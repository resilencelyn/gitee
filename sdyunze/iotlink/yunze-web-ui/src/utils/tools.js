/**
 *  基于 VUE + Element 的常用 函数 封装
 *  @Data 2020-12-12 ~ 至今
 *  @Author:root;
 *  @mailbox:546558392@qq.com;
 */
import CryptoJS from 'crypto-js/crypto-js'
import "crypto-js/aes"
import "crypto-js/pad-zeropadding"
import $ from "jquery";
import {Toast} from "vant";




let tools = {}

/**
 * 查询当前用户是否有操作权限
 */
tools.hasResource=function(id){
    let _this=this
    let requests =JSON.parse(sessionStorage.getItem("loginUser"))
    if(requests.account===null){
        return false
    }
    for(let i=0;i<requests.functionList.length;i++){
        if(id===requests.functionList[i].id){
            return true
        }
    }
    return false
}

// import qs from 'qs'

tools.encrypt = function (data, key, iv) {
    key = tools.Is_null(key) ? key : "YunZeIot@5iot.cn";
    iv = tools.Is_null(iv) ? iv : "www_5iot_cn@2022";
    //window.//console.log(key.length)
    //console.log(key+" "+iv);
    //console.log(key+"  =  "+iv);
    key = CryptoJS.enc.Utf8.parse(key);
    iv = CryptoJS.enc.Utf8.parse(iv);
   /* return CryptoJS.AES.encrypt(data, key, {
        iv: iv,
        mode: CryptoJS.mode.CBC,
        padding: CryptoJS.pad.ZeroPadding
    }).toString();
*/
    // //console.log(key+"  =  "+iv);
    return CryptoJS.AES.encrypt(data, key, {iv:iv,mode:CryptoJS.mode.CBC,padding:CryptoJS.pad.ZeroPadding}).toString();
}

tools.install = function (Vue, options) {
    Vue.prototype.$tools = tools
    Vue.tools = tools
}


// 解密
 tools.Decrypt = function(text, key, iv) {
    key = tools.Is_null(key) ? key : "YunZeIot@5iot.cn";
    iv = tools.Is_null(iv) ? iv : "www_5iot_cn@2022";
    let decrypted = CryptoJS.AES.decrypt(text, CryptoJS.enc.Utf8.parse(key), {
        iv: CryptoJS.enc.Utf8.parse(iv),
        mode: CryptoJS.mode.CBC,
        padding: CryptoJS.pad.ZeroPadding
    })
    return decrypted.toString(CryptoJS.enc.Utf8)
}

tools.open = function (_this, Message) {
   _this.$notify.fail(Message);
}


//-----[验证文本并提示]
tools.VerificationsText = function (_this, _val, Message) {
    //console.log(_val);
    let bool = tools.Is_null(_val);
    //console.log(bool);
    if (bool == false) {
        tools.open(_this, Message)
    }
    return bool;
}

//-----[验证文本并提示]
tools.VerificationsText_wx = function (_this, _val, Message) {
    //console.log(_val);
    let bool = tools.Is_null(_val);
    //console.log(bool);
    if (bool == false) {
        Toast(Message);
    }
    return bool;
}


//-----[验证数组并提示]
tools.VerificationsArr = function (_this, _arr, Message) {
    let bool = _arr.length>0;
    if (bool == false) {
        tools.open(_this, Message)
    }
    return bool;
}
/**
 * 判断是否为空值
 * @param val
 * @returns
 */
tools.Is_null = function (val) {
    //console.log(val)
    //console.log(Object.is(val, NaN))
    if(Object.is(val, NaN)){
        //console.log("fffffff")
        return  false;
    }else{
        val = tools.isNumber(val) == true ? val + "" : val;//如果是一个数字类型过滤成字符串 && isNaN(val)==false
        return val != undefined && val != null  && val != "" && val.trim() != "" && val != "undefined" && val != "null" && val != 'NaN'  ? true : false;//
    }
}


/**
 * 通道接口调用时 计算 Sign
 * @param _this
 * @param map
 * @param key
 * @returns {string}
 */
tools.getSign = function (_this,map, key) {
    // //console.log("getSign====")
    let signstr = "";
    let key_arr = [];
    $.each(map, function (key, val) {
        key_arr.push(key)
    })
    //console.log(key_arr)
    key_arr.sort();
    //console.log(key_arr)
    $.each(key_arr, function (i, o) {
        signstr += o + "=" + map.o + "&";
    })
    signstr = "key=" + key;
    //console.log(signstr);
    let sign = _this.$md5(signstr).toUpperCase();
    // //console.log(sign);
    return sign;
}

/**
 * obj 判断是否是一个数（number）
 * @param obj
 * @returns
 */
tools.isNumber = function (obj) {
    return typeof obj === 'number' && !isNaN(obj);
}




/**
 * 询问对话框
 * @param: _this = this
 * @param: _type = 提示类型 【success，error，info，warning】
 * @param: Message = 消息主体
 * @param: TrueFun = 确定时 执行 函数 例【this.add】 前提是 add 必须有
 * @param: TrunPar = 确定时 执行 函数 参数
 * @param: FalseFun = 取消时 执行 函数 例【this.add】 前提是 add 必须有
 * @param: FalsePar = 取消时 执行 函数 参数
 */
tools.openAsk = function (_this,_type, Message, TrueFun, TrunPar, FalseFun, FalsePar) {
    _type = tools.Is_null(_type)?_type:"info";
    _this.$confirm(Message, {
        confirmButtonText: '确定',
        cancelButtonText: '取消',
        type: _type
    }).then(() => {
       TrueFun(TrunPar)
    }).catch(() => {
        if(FalseFun!=null){
            FalseFun(FalsePar);
        }
    });
}











/**
 * 获取一个double值
 * @param P_number 传入值【任意字符】
 * @param De_val 【默认值】选参
 * @returns
 */
tools.set_double = function(val,De_val){
    //console.log(val);
    //console.log(De_val);
    De_val=tools.Is_null(De_val)==true?De_val:1;//默认值为空时给 1
    let reg_Tow = /^(?!0{2,})(?:\d{1,9}(\.\d+)?|10{9})$/;
    let map={};
    if(val!=undefined && val!=null && val.trim()!=""){
        val=parseFloat(val);
        val=tools.toFixed(val,2);//保留两位小数数据格式化
        val = reg_Tow.test(val)?val:De_val;//单价
        val  = val =="0.00"? De_val:val;
    }else{//默认单价
        val=De_val;
    }
    return val;
}



/**
 * toFixed(四舍五入重写)
 * @param number
 * @param decimal
 * @returns
 */
tools.toFixed = function (number, decimal) {
    decimal = decimal || 0;
    let s = String(number);
    let decimalIndex = s.indexOf('.');
    if (decimalIndex < 0) {
        let fraction = '';
        for (let i = 0; i < decimal; i++) {
            fraction += '0';
        }
        return s + '.' + fraction;
    }
    let numDigits = s.length - 1 - decimalIndex;
    if (numDigits <= decimal) {
        let fraction = '';
        for (let i = 0; i < decimal - numDigits; i++) {
            fraction += '0';
        }
        return s + fraction;
    }
    let digits = s.split('');
    let pos = decimalIndex + decimal;
    let roundDigit = digits[pos + 1];
    if (roundDigit > 4) {
        //跳过小数点
        if (pos == decimalIndex) {
            --pos;
        }
        digits[pos] = Number(digits[pos] || 0) + 1;
        //循环进位
        while (digits[pos] == 10) {
            digits[pos] = 0;
            --pos;
            if (pos == decimalIndex) {
                --pos;
            }
            digits[pos] = Number(digits[pos] || 0) + 1;
        }
    }
    //避免包含末尾的.符号
    if (decimal == 0) {
        decimal--;
    }
    return digits.slice(0, decimalIndex + decimal + 1).join('');
}


/**
 * 针对文本域 换行符切割 过滤前后空格获取数组
 * @param lets
 */
tools.textareaGet = function (vars){
    vars = vars.split(/[\s\n]/);
    //console.log(vars);
    vars =  $.grep(vars, function (x) { return $.trim(x).length > 0; });
    //console.log(vars);
    return vars;
}



//-----[获取当前时间]
tools.getTime =function () {
    let myDate = new Date();
    let h = myDate.getHours(); //获取当前小时数(0-23)
    let m = myDate.getMinutes(); //获取当前分钟数(0-59)
    let s = myDate.getSeconds();
    let now = tools.p(h) + ':' + tools.p(m) + ":" + tools.p(s);
    return now;
}


//-----[获取当前时间]
tools.getdatetime =function () {
    let myDate = new Date();
    //获取当前年
    let year = myDate.getFullYear();
    //获取当前月
    let month = myDate.getMonth() + 1;
    //获取当前日
    let date = myDate.getDate();
    let h = myDate.getHours(); //获取当前小时数(0-23)
    let m = myDate.getMinutes(); //获取当前分钟数(0-59)
    let s = myDate.getSeconds();
    let now = year + '-' +tools.p(month) + "-" + tools.p(date) + " " + tools.p(h) + ':' + tools.p(m) + ":" + tools.p(s);

    return now;
}

tools.p =function (s) {
    return s < 10 ? '0' + s : s;
}


//-----[获取前n天的日期]
tools.getBeforeDate =function (n) {

    let d = new Date();
    let year = d.getFullYear();
    let mon = d.getMonth() + 1;
    let day = d.getDate();
    if(day <= n) {
        if(mon > 1) {
            mon = mon - 1;
        } else {
            year = year - 1;
            mon = 12;
        }
    }
    d.setDate(d.getDate() - n);
    year = d.getFullYear();
    mon = d.getMonth() + 1;
    day = d.getDate();
    let s = year + "-" + (mon < 10 ? ('0' + mon) : mon) + "-" + (day < 10 ? ('0' + day) : day);
    return s;
}



//-- 获取两个日期之间所有日期
Date.prototype.format = function() {
    let s = '';
    let mouth = (this.getMonth() + 1) >= 10 ? (this.getMonth() + 1) : ('0' + (this.getMonth() + 1));
    let day = this.getDate() >= 10 ? this.getDate() : ('0' + this.getDate());
    s += this.getFullYear() + '-'; // 获取年份。
    s += mouth + "-"; // 获取月份。
    s += day; // 获取日。
    return(s); // 返回日期。
};

tools.getdateAll =function (begin, end) {
    let $resdata = [];
    let ab = begin.split("-");
    let ae = end.split("-");
    let db = new Date();
    db.setUTCFullYear(ab[0], ab[1] - 1, ab[2]);
    let de = new Date();
    de.setUTCFullYear(ae[0], ae[1] - 1, ae[2]);
    let unixDb = db.getTime();
    let unixDe = de.getTime();
    for(let k = unixDb; k <= unixDe;) {
        $resdata.push(new Date(parseInt(k)).format());
        k = k + 24 * 60 * 60 * 1000;
    }
    return $resdata;
}



//----[获取当前日期]
tools.gitData =function () {
    function p(s) {
        return s < 10 ? '0' + s : s;
    }
    let myDate = new Date();
    //获取当前年
    let year = myDate.getFullYear();
    //获取当前月
    let month = myDate.getMonth() + 1;
    //获取当前日
    let date = myDate.getDate();
    return(year + '-' + p(month) + "-" + p(date));
}


/**
 * 格式化 数字每三位加逗号的方法（包含小数）
 * @param str
 * @returns {string}
 */
tools.formatNum =function (str) {
    let newStr = "";
    let count = 0;
    str = str+"";
    //console.log(str)
    // 当数字是整数
    if (str.indexOf(".") == -1) {
        for (let i = str.length - 1; i >= 0; i--) {
            if (count % 3 == 0 && count != 0) {
                newStr = str.charAt(i) + "," + newStr;
            } else {
                newStr = str.charAt(i) + newStr;
            }
            count++;
        }
        str = newStr ; //自动补小数点后两位
        return str;
    }
    // 当数字带有小数
    else {
        for (let i = str.indexOf(".") - 1; i >= 0; i--) {
            if (count % 3 == 0 && count != 0) {
                newStr = str.charAt(i) + "," + newStr;
            } else {
                newStr = str.charAt(i) + newStr; //逐个字符相接起来
            }
            count++;
        }
        str = newStr + (str + "00").substr((str + "00").indexOf("."), 3);
        return str;
    }
}


/**
 * 获取增长百分比
 * @param val1
 * @param val2
 * @returns {{}}
 */
tools.getPercentage =function (val1,val2) {
    let map ={};

    //let percentage = tools.NumberMul(tools.NumberSub((val1/val2),'1'),'100');
    //let percentage = tools.NumberMul(((val1/val2)-1),'100');
    //let percentage = ((val1/val2)-1)*100;

    let percentage = tools.NumberMul(tools.NumberSub(tools.NumberDiv(val1,val2,4),'1'),'100');
    //console.log("percentage   = "+percentage);
    //let percentage = ((180/80)-1)*100;
    //console.log(percentage>0);
    // //console.log(parseFloat(percentage)>0);
    percentage =parseFloat(percentage);
    // //console.log(percentage>0);
    // //console.log(percentage);

    if(percentage>0){
       map.type="up";
       map.percentage = percentage+"%";
    }else {
        percentage = tools.Is_null(percentage)?percentage:0;
        map.type="down";
        map.percentage = Math.abs(percentage)+"%";
    }
    return map;
}

/**
 * 乘法
 * @param arg1
 * @param arg2
 * @returns {number}
 * @constructor
 */
tools.NumberMul = function(arg1, arg2) {
    var m = 0;
    var s1 = arg1.toString();
    var s2 = arg2.toString();
    try {
        m += s1.split(".")[1].length;
    } catch (e) {}
    try {
        m += s2.split(".")[1].length;
    } catch (e) {}

    return Number(s1.replace(".", "")) * Number(s2.replace(".", "")) / Math.pow(10, m);
}

/**
 * 加法
 * @param arg1
 * @param arg2
 * @returns {string}
 * @constructor
 */
tools.NumberAdd = function(arg1, arg2) {
    var r1, r2, m, n;
    arg1 = arg1+"";
    arg2 = arg2+"";
    try {
        r1 = arg1.toString().split(".")[1].length
    } catch (e) {
        r1 = 0
    }
    try {
        r2 = arg2.toString().split(".")[1].length
    } catch (e) {
        r2 = 0
    }
    m = Math.pow(10, Math.max(r1, r2))
    n = (r1 >= r2) ? r1 : r2;
    return Number(((arg1 * m + arg2 * m) / m).toFixed(n));
}


/**
 * 减法
 * @param arg1
 * @param arg2
 * @returns {string}
 * @constructor
 */
tools.NumberSub = function(arg1, arg2) {
    var re1, re2, m, n;
    arg1 = arg1+"";
    arg2 = arg2+"";
    try {
        re1 = arg1.toString().split(".")[1].length;
    } catch (e) {
        re1 = 0;
    }
    try {
        re2 = arg2.toString().split(".")[1].length;
    } catch (e) {
        re2 = 0;
    }
    m = Math.pow(10, Math.max(re1, re2));
    n = (re1 >= re2) ? re1 : re2;
    return Number(((arg1 * m - arg2 * m) / m).toFixed(n));
}


/**
 * 除法
 * @param arg1 除数
 * @param arg2 被除数
 * @param digit 保留的小数点后的位数
 * @returns {number}
 * @constructor
 */
tools.NumberDiv = function (arg1,arg2,digit){
    arg1 = arg1+"";
    arg2 = arg2+"";
    var t1=0,t2=0,r1,r2;
    try{t1=arg1.toString().split(".")[1].length}catch(e){}
    try{t2=arg2.toString().split(".")[1].length}catch(e){}
    r1=Number(arg1.toString().replace(".",""))
    r2=Number(arg2.toString().replace(".",""))
    //获取小数点后的计算值
    var result= ((r1/r2)*Math.pow(10,t2-t1)).toString()
    var result2=result.split(".")[1];
    result2 = result2+"";
    result2=result2.substring(0,digit>result2.length?result2.length:digit);

    return Number(result.split(".")[0]+"."+result2);
}


/**
 * 消息提示
 * @param _this
 * @param _message
 * @param _type
 * @constructor
 */
tools.MessageShow = function (_this,_message,_type){
    _type = tools.Is_null(_type)?_type:"";
       Toast(_message);
}


/*
tools.set_double = function (val,Defaults_val){
    Defaults_val=tools.Is_null(Defaults_val)==true?Defaults_val:1;//默认值为空时给 1
    let reg_Tow = /^(?!0{2,})(?:\d{1,9}(\.\d+)?|10{9})$/;
    let map={};
    if(val!=undefined && val!=null && val.trim()!=""){
        val=parseFloat(val);
        val=tools.toFixed(val,2);//保留两位小数数据格式化
        val = reg_Tow.test(val)?val:Defaults_val;//单价
        val  = val =="0.00"? Defaults_val:val;
    }else{//默认单价
        val=Defaults_val;
    }
    return val;
}*/



/**
 * 获取一个整数值
 * @param P_number 传入值【任意字符】
 * @param Defaults_val 【默认值】选参
 * @returns
 */
tools.set_number = function (P_number,Defaults_val){
    Defaults_val=tools.Is_null(Defaults_val)==true?Defaults_val:1;//默认值为空时给 1
    let reg_Tow = /^(?!0{2,})(?:\d{1,9}(\.\d+)?|10{9})$/;
    if(P_number!=undefined && P_number!=null && P_number.trim()!=""){
        P_number =parseInt(P_number);
        P_number  = reg_Tow.test(P_number)?P_number:Defaults_val;//数量
        P_number  = P_number =="0"? Defaults_val:P_number;
    }else{//默认 值 赋值
        P_number=Defaults_val;
    }
    return P_number;
}



//----[多少毫秒后刷新界面]
tools.loadSX = function (s) {
    setTimeout(function() {
        window.location.reload();
    }, s);
}
//----[多少毫秒后刷新界面]


/**
 * 减法运算
 * @param num1
 * @param num2
 * @returns
 */
tools.numSub = function (num1, num2) {
    var baseNum, baseNum1, baseNum2;
    try {
        baseNum1 = num1.toString().split(".")[1].length;
    } catch (e) {
        baseNum1 = 0;
    }
    try {
        baseNum2 = num2.toString().split(".")[1].length;
    } catch (e) {
        baseNum2 = 0;
    }
    baseNum = Math.pow(10, Math.max(baseNum1, baseNum2));
    var precision = (baseNum1 >= baseNum2) ? baseNum1 : baseNum2;
    return ((num1 * baseNum - num2 * baseNum) / baseNum).toFixed(precision);
};

/**
 * 除法运算，避免数据相除小数点后产生多位数和计算精度损失。
 * @param num1
 * @param num2
 * @data 2019-01-14
 * @returns
 */
tools.numDiv = function (num1, num2) {
    var baseNum1 = 0, baseNum2 = 0;
    var baseNum3, baseNum4;
    try {
        baseNum1 = num1.toString().split(".")[1].length;
    } catch (e) {
        baseNum1 = 0;
    }
    try {
        baseNum2 = num2.toString().split(".")[1].length;
    } catch (e) {
        baseNum2 = 0;
    }
   // with (Math) {
        baseNum3 = Number(num1.toString().replace(".", ""));
        baseNum4 = Number(num2.toString().replace(".", ""));
        return (baseNum3 / baseNum4) * Math.pow(10, baseNum2 - baseNum1);
    //}
};

//-----[加法函数，用来得到精确的加法结果]2018-12-23
tools.accAdd = function (arg1,arg2){

    var r1,r2,m;

    try{r1=arg1.toString().split(".")[1].length}catch(e){r1=0}

    try{r2=arg2.toString().split(".")[1].length}catch(e){r2=0}

    m=Math.pow(10,Math.max(r1,r2));

    return (accMul(arg1,m)+accMul(arg2,m))/m;

}
//-----[加法函数，用来得到精确的加法结果END]
//-----[乘法]2018-12-23
tools.accMul = function (arg1,arg2){
    var m=0,s1=arg1.toString(),s2=arg2.toString();
    try{m+=s1.split(".")[1].length}catch(e){}
    try{m+=s2.split(".")[1].length}catch(e){}
    return Number(s1.replace(".",""))*Number(s2.replace(".",""))/Math.pow(10,m)
}
//-----[乘法end]



/**
 * obj 判断是否是一个数（number）
 * @param obj
 * @returns
 */
tools.isNumber = function   (obj){
    return   typeof obj === 'number' && !isNaN(obj);
}

//-----[计算时间差]
tools.GetDateDiff = function (startTime, endTime, diffType) {
    //将xxxx-xx-xx的时间格式，转换为 xxxx/xx/xx的格式
    //startTime = startTime.replace(/\-/g, "/");
    startTime.replace(RegExp("-", "g"), "/");
    //endTime = endTime.replace(/\-/g, "/");
    endTime.replace(RegExp("-", "g"), "/");
    //将计算间隔类性字符转换为小写
    diffType = diffType.toLowerCase();
    var sTime = new Date(startTime); //开始时间
    var eTime = new Date(endTime); //结束时间
    //作为除数的数字
    var timeType = 1;
    switch(diffType) {
        case "second":
            timeType = 1000;
            break;
        case "minute":
            timeType = 1000 * 60;
            break;
        case "hour":
            timeType = 1000 * 3600;
            break;
        case "day":
            timeType = 1000 * 3600 * 24;
            break;
        default:
            break;
    }
    return parseInt((eTime.getTime() - sTime.getTime()) / parseInt(timeType));
}
//-----[END][计算时间差]




//-----[清除特殊字符并提示element: 元素，df_val:默认值,MaxLeng 最大长度]2019-01-04
tools.KeyUP_Cler = function (_this,$val,df_val,MaxLeng,Msg) {
  var $bool = tools.RegeMatch($val) ? false : true;
  MaxLeng = tools.Is_null(MaxLeng)?MaxLeng:50;
  //console.log($bool)
  if(!$bool){
    Msg = tools.Is_null(Msg)?Msg:"不能有特殊字符哦~";
    _this.msgError(Msg);
    $val = window["df_val"];
  }
  else{window["df_val"]=$val;}//赋值默认val
  if(tools.Is_null($val)  &&  $val.length>MaxLeng ){
    _this.msgError("最大长度为["+MaxLeng+"] !");
    $val = $val.substring(0,MaxLeng);
  }
  return $bool;
}
//-----[END][清除特殊字符并提示]

//-----[过滤特殊字符]
tools.RegeMatch = function ($name) {
  var pattern = new RegExp("[-` ~!@#$%^&*()+=|{}':;',\\[\\].<>/?~！@#￥%……&*（）—+|{}【】‘；：”“’。，、？《》]");
  if($name != undefined && $name != null  && $name != "" && $name.length > 0) {
    if(pattern.test($name)) {
      return true;
    } else {
      return false;
    }
  }
}
//-----[END][过滤特殊字符]



//-----[过滤特殊字符]
tools.Textfilter = function(element,df_val){
  // 键盘按下过滤
  element.on("keyup",function(){
    tools.KeyUP_Cler(element,df_val);
  })
  // 编辑文本框 失焦过滤
  element.on("blur",function(){
    tools.KeyUP_Cler(element,df_val);
  })
}



/**
 * 设置 编辑值为 int 或者 double 类型
 * @param element 【元素】如： $("#A")
 * @param type 获取数类型 int or double
 * @param Defaults_val 默认值 选填
 * @param maxlength 最大长度
 * @returns
 */
tools.val_number = function (element,type,Defaults_val,maxlength){
  //焦时获取元素 当前行,当前td
  element.live("focus",function(data){
    window["_parent"]=$(this).parent().parent();
    window["_e"]=$(this).parent();
  })

  // 键盘按下过滤
  element.live("keyup",function(data){
    $(this).attr("maxlength",maxlength);
    var value=num_value($(this).val());//只能输入两位数小数和整数
    $(this).val(value);
  })
  // 编辑文本框 失焦过滤
  element.live("blur",function(data){
    var _parent=window["_parent"];
    var _this=window["_e"];
    var val=_this.html();
    if("int"==type.toLowerCase()){
      val=set_number(val,Defaults_val)
    }else if("double"==type.toLowerCase()){
      val=set_double(val,Defaults_val)
    }
    //延迟赋值 【解决动态编辑赋值的bug】
    setTimeout(function(){
      _this.html(val);
    },10);
  })

}

//格式化 Double  保留后两位
tools.handleInput2 = function (e) {
  // 通过正则过滤小数点后两位
  e.target.value = (e.target.value.match(/^\d*(\.?\d{0,1})/g)[0]) || null
},


//获取cookie
tools.getCookie = function (cname){
  var name = cname + "=";
  var ca = document.cookie.split(';');
  //console.log("获取cookie,现在循环")
  for (var i = 0; i < ca.length; i++) {
    var c = ca[i];
    //console.log(c)
    while (c.charAt(0) == ' ') c = c.substring(1);
    if (c.indexOf(name) != -1){
      return c.substring(name.length, c.length);
    }
  }
  return "";
}


/*获取字典数据中的匹对key value*/
tools.getDkeyValue = function(arr,key){
  /*let value = '';
  key = ""+key+"";
  for (let i = 0; i < arr.length; i++) {
    let map =  arr[i];
    if(map.dictValue==key){
      value = map.dictLabel;
      break;
    }
  }
  return value;*/
  return tools.getkeyValue(arr,key,"dictValue","dictLabel");
}


/*获取字典数据中的匹对key value 部门数据*/
tools.getDeptNamekeyValue = function (arr,key){
  /*let value = '';
  key = ""+key+"";
  for (let i = 0; i < arr.length; i++) {
    let map =  arr[i];
    if(map.dept_id==key){
      value = map.dept_name;
      break;
    }
  }
  return value;*/
  return tools.getkeyValue(arr,key,"dept_id","dept_name");
}


/*获取字典数据中的匹对key value 用户数据*/
tools.getUserNamekeyValue = function (arr,key){
 /* let value = '';
  key = ""+key+"";
  for (let i = 0; i < arr.length; i++) {
    let map =  arr[i];
    if(map.user_id==key){
      value = map.nick_name;
      break;
    }
  }*/
  //return value;
  return tools.getkeyValue(arr,key,"user_id","nick_name");
}


/*获取 资费组 中的匹对key value 用户数据*/
tools.getPackageNNamekeyValue = function (arr,key){
  /*let value = '';
  key = ""+key+"";
  for (let i = 0; i < arr.length; i++) {
    let map =  arr[i];
    if(map.package_id==key){
      value = map.package_agentname;
      break;
    }
  }*/
  return tools.getkeyValue(arr,key,"package_id","package_agentname");
}




/*获取 arr 中的匹对 keyVlue Rvalue 数据*/
tools.getkeyValue = function (arr,key,keyVlue,Rvalue){

  let value = '';
  key = ""+key+"";
  for (let i = 0; i < arr.length; i++) {
    let map =  arr[i];
    if(map[keyVlue]==key){
      value = map[Rvalue];
      break;
    }
  }
  return value;
}




export default tools
