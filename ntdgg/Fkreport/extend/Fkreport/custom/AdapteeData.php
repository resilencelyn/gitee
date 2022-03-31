<?php
/**
  *+------------------
  * Fk
  *+------------------
  * Fk 适配器设计器数据类
  *+------------------
  */
namespace Fkreport\custom;

use think\facade\Db;

class AdapteeData{

    /**
     * 获取设计数据
     * @param $id
     */
	function find($id){
		$info = Db::name('fk_main')->find($id);
		if($info){
			return  $info;
		}else{
			return  false;
		}
	}

    /**
     * 添加大屏数据
     * @param $data
     * @return false|int|string
     */
    function add($data){
        $data['add_time'] = date('Y-m-d H:i:s');
        $ret = Db::name('fk_main')->insertGetId($data);
        if($ret){
            return $ret;
        }else{
            return  false;
        }
    }
    /**
     * 保存设计
     * @param $id
     */
    function save($id,$data){
        $ret = Db::name('fk_main')->update(['id'=>$id,'data'=>$data,'update_time'=>time()]);
        if($ret===false){
            return  false;
        }else{
            return $ret;
        }
    }
    /**
     * 保存设计
     * @param $id
     */
    function editsave($id,$data){
        $ret = Db::name('fk_main')->where('id',$id)->update($data);
        if($ret===false){
            return  false;
        }else{
            return $ret;
        }
    }
    /**
     * 获取设计数据
     * @param $id
     */
    function all($map,$order){
        $list = Db::name('fk_main')->where($map)->select()->order($order)->toArray();
        if($list){
            return  $list;
        }else{
            return  false;
        }
    }
}