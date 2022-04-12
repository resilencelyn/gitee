<?php

namespace Fkreport\adaptive;

use Fkreport\fun\unit;

class Fun
{
    protected $mode ;
    public function  __construct(){
        if(unit::gconfig('db_mode')==1){
            $className = '\\Fkreport\\custom\\AdapteeFun';
        }else{
            $className = unit::gconfig('db_namespace').'AdapteeFun';
        }
        $this->mode = new $className();
    }
    /**
     * 查询全部设计数据
     * @param array $map
     * @param string $order
     * @return array|false
     */
    static function sapi($post)
    {
        $map[] = ['fun_name','=',$post['fun']];
        $hasFun = (new Fun())->mode->findWhere($map);
        if(!$hasFun){
            return json(['code'=>1,'msg'=>'禁止函数名称重复！']);
        }else{
            $ret = Common::query($hasFun['function']);
            return json(['Data'=>$ret['msg'],'Id'=>$post['Id'],'Type'=>$post['Type']]);
        }

    }
    /**
     * 数据添加，修改
     * @param $id
     * @param $data
     * @return false|int|string
     */
    static function save($data){
        if(!isset($data['id'])){
            $map[] = ['fun_name','=',$data['name']];
        }else{
            $map[] = [['fun_name','=',$data['name']],['id','<>',$data['id']]];
        }
        $hasname = (new Fun())->mode->findWhere($map);
        if($hasname){
            return json(['code'=>1,'msg'=>'禁止函数名称重复！']);
        }
        if(!isset($data['id'])){
            $ver = [
                'bill'=>'Fk'.date('YmdHis'),
                'title'=>$data['title'],
                'fun_name'=>$data['name'],
                'add_user'=>'Sys',
                'function'=>$data['fun'],
                'add_time'=>time()
            ];
            $ret = (new Fun())->mode->add($ver);
            if($ret){
                return json(['code'=>0,'msg'=>'操作成功！']);
            }else{
                return json(['code'=>-1,'msg'=>'更新出错']);
            }
        }else{
            $ver = [
                'id'=>$data['id'],
                'title'=>$data['title'],
                'fun_name'=>$data['name'],
                'function'=>$data['fun']
            ];
            $ret = (new Fun())->mode->save($ver);
            if($ret){
                return json(['code'=>0,'msg'=>'操作成功！']);
            }else{
                return json(['code'=>-1,'msg'=>'更新出错']);
            }
        }
    }
    /**
     * 查询全部设计数据
     * @param array $map
     * @param string $order
     * @return array|false
     */
    static function all($map=[],$order='id desc')
    {
        return (new Fun())->mode->all($map,$order);
    }

}