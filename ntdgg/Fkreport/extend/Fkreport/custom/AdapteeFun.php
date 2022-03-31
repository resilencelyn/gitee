<?php

namespace Fkreport\custom;

use think\facade\Db;

class AdapteeFun
{
    function findWhere($map){
        $info = Db::name('fk_function')->where($map)->find();
        if($info){
            return  $info;
        }else{
            return  false;
        }
    }
    /**
     * 获取设计数据
     * @param $id
     */
    function all($map,$order){
        $list = Db::name('fk_function')->where($map)->select()->order($order)->toArray();
        if($list){
            return  $list;
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
        $ret = Db::name('fk_function')->insertGetId($data);
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
    function save($data){
        $ret = Db::name('fk_function')->update($data);
        if($ret){
            return $ret;
        }else{
            return  false;
        }
    }
}