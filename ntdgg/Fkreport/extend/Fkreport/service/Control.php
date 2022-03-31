<?php
/**
  *+------------------
  * SFDP-超级表单开发平台V5.0
  *+------------------
  * Sfdp 核心驱动类
  *+------------------
  * Copyright (c) 2018~2020 https://cojz8.com All rights reserved.
  *+------------------
  * Author: guoguo(1838188896@qq.com)
  *+------------------ 
  */
namespace Fkreport\service;

use Fkreport\adaptive\Data;


class Control{
	/**
	 * API设计统一调用接口
	 *
	 * @param  str $act 调用方法
	 * @param  Array $data post数据，或者sid值
	 */
	static function api($act,$id,$data=''){
        if($act=='save'){
           return json(Data::save($id,$data));
        }
        if($act=='editsave'){
            return json(Data::editsave($id,$data));
        }
	}

}