/*layui-JS简单的通用方法 date:2017-11-22~至今 @Author:root;@mailbox:546558392@qq.com;*/
/**
 * 判断是否为空值
 * @param val
 * @returns
 */
function Is_null(val){
	val = isNumber(val)==true ?val+"":val;//如果是一个数字类型过滤成字符串
	return val!=undefined && val!=null && val.trim()!="" && val!="undefined" && val!="null"?true:false;
}