<?php

namespace Fkreport\adaptive;

use Fkreport\fun\unit;

class Common
{
    protected $mode ;
    public function  __construct(){
        if(unit::gconfig('db_mode')==1){
            $className = '\\Fkreport\\custom\\AdapteeCommon';
        }else{
            $className = unit::gconfig('db_namespace').'AdapteeCommon';
        }
        $this->mode = new $className();
    }
    static function file($limit,$page,$map=[]){
        return (new Common())->mode->file($limit,$page,$map);
    }


    static function query($sql){
        return (new Common())->mode->query($sql);
    }


}