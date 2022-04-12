<?php
/**
 *+------------------
 * Fk
 *+------------------
 * Sfdp 设计类
 *+------------------
 */
namespace Fkreport\adaptive;

use Fkreport\fun\unit;


class Data{

    protected $mode ;
    public function  __construct(){
        if(unit::gconfig('db_mode')==1){
            $className = '\\Fkreport\\custom\\AdapteeData';
        }else{
            $className = unit::gconfig('db_namespace').'AdapteeData';
        }
        $this->mode = new $className();
    }
    /**
     * find 获取设计信息  getDesign
     * @param array $whereRaw raw查询条件
     * @param array $map 查询条件
     */
    static function find($id)
    {
        $row = (new Data())->mode->find($id);
        $Json = json_decode($row['data'],true);
        if($Json == null){
            $Json['f_list'] = 1;
            $row['data'] = 1;
        }
        return ['data'=>$row,'Json_data'=>$Json['f_list']];
    }

    /**
     * 查询全部设计数据
     * @param array $map
     * @param string $order
     * @return array|false
     */
    static function all($map=[],$order='id desc')
    {
        return (new Data())->mode->all($map,$order);
    }

    /**
     * 数据添加，修改
     * @param $id
     * @param $data
     * @return false|int|string
     */
    static function save($id,$data){
        if($id==0){
            $ret = (new Data())->mode->add($data);
            if($ret){
                return ['code'=>0,'msg'=>'success'];
            }else{
                return ['code'=>1,'msg'=>$ret];
            }
        }else{
            $ret = (new Data())->mode->save($id,$data['data']);
            if($ret){
                return ['code'=>0,'msg'=>'success'];
            }else{
                return ['code'=>1,'msg'=>$ret];
            }
        }
    }
    /**
     * 数据添加，修改
     * @param $id
     * @param $data
     * @return false|int|string
     */
    static function editsave($id,$data){
        $ret = (new Data())->mode->editsave($id,$data);
        if($ret){
            return ['code'=>0,'msg'=>'success'];
        }else{
            return ['code'=>1,'msg'=>$ret];
        }
    }
}