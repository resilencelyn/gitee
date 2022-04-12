<?php

namespace Fkreport\custom;

use think\facade\Db;

class AdapteeCommon
{
    function query($sql){
        try{
            $data = Db::query($sql);
            return ['code'=>0,'msg'=>$data];
        }catch(\Exception $e){
            return ['code'=>-1,'msg'=>'SQL_Err:'.$sql];
        }
    }
    function file($limit,$page,$map){
        $offset = ($page - 1) * $limit;
        $list = Db::name('fk_imags')->where($map)->limit($offset, $limit)->order('id desc')->select()->toArray();
        foreach ($list as $k => $v) {
            $list[$k]['src'] = str_replace("\\", "/", $v['src']);
        }
        $count = Db::name('fk_imags')->where($map)->count();
        return ['code' => 0, 'count' => $count, 'data' => $list, 'msg' => ''];
    }

}