<?php
/**
  *+------------------
  * Fk
  *+------------------
  *  工具类
  *+------------------
  */
namespace Fkreport\fun;


class unit{
	
	/**
	 * 判断是否是POST
	 *
	 **/
	public static function is_post()
	{
	   return isset($_SERVER['REQUEST_METHOD']) && strtoupper($_SERVER['REQUEST_METHOD'])=='POST';	
	}

	/**
	 * 根据键值加载全局配置文件
	 *
	 * @param string $key 键值
	 */
	public static function gconfig($key) {
		$file = dirname(dirname(__DIR__) . DIRECTORY_SEPARATOR, 2) . DIRECTORY_SEPARATOR . 'config' . DIRECTORY_SEPARATOR . 'fkreport.php';
		if(!file_exists($file)){
			echo 'sorry,config no find!';exit;
		}
		$ret =require($file);
		return $ret[$key] ?? '';
	}
	/**
	 * 缩进设置
	 *
	 * @param string $step 步骤
	 * @param string $string 字符串
	 * @param string $size 长度
	 */
	static function tab($step = 1, $string = ' ', $size = 4)
	{
		return str_repeat($string, $size * $step);
	}
	/**
	 * 根据日期编码生成
	 *
	 */
	static function OrderNumber()
	{
		return "D" . date("Ymd").rand(1000,9999);;
	}
}

