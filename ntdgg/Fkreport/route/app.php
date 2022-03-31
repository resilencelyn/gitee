<?php
// +----------------------------------------------------------------------
// | ThinkPHP [ WE CAN DO IT JUST THINK ]
// +----------------------------------------------------------------------
// | Copyright (c) 2006~2018 http://thinkphp.cn All rights reserved.
// +----------------------------------------------------------------------
// | Licensed ( http://www.apache.org/licenses/LICENSE-2.0 )
// +----------------------------------------------------------------------
// | Author: liu21st <liu21st@gmail.com>
// +----------------------------------------------------------------------
use think\facade\Route;

/**
 * 方块报表路由接口
 * api 设计器数据接口
 * fun 函数设计接口
 * data 资源路由接口
 */
Route::group('fk', function () {
    Route::rule('api', '\Fkreport\Api@Api');//方块报表入库API接口
    Route::rule('fun', '\Fkreport\Api@fun');//方块报表统一函数接口
    Route::rule('data', '\Fkreport\Api@data');//方块报表 资源统一接口
});

