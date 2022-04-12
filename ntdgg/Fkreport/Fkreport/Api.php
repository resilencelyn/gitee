<?php
/**
 *+------------------
 * 方块报表入口
 *+------------------
 */
declare (strict_types=1);

namespace Fkreport;

use Fkreport\adaptive\Common;
use Fkreport\adaptive\Data;
use Fkreport\adaptive\Fun;
use Fkreport\fun\unit;
use Fkreport\service\Control;

define('Fk_URL', realpath(dirname(__FILE__)));

define('fk_Ver', '1.0');

class Api
{
    protected $tpl;//模板路径

    public function __construct()
    {
        $this->tpl = Fk_URL . '/view/';
    }

    /**
     * 方块报表统一API接口
     * @param string $act 调用接口方法
     * @param string $id 传递id
     * @return \think\response\View|void
     */
    public function Api($act = 'index', $id = '')
    {
        if ($act == 'index') {
            return view($this->tpl . 'index.html', ['data' => Data::all()]);
        }
        if ($act == 'view' || $act == 'desc') {
            $row = Data::find($id);
            return view($this->tpl . $act.'.html', ['id'=>$id,'data'=>$row,'url_api'=>unit::gconfig('url_api')]);
        }
        if ($act == 'add') {
            if (unit::is_post()) {
                $data = input('post.');
                return Control::api('save', 0, $data);
            } else {
                return view($this->tpl . 'add.html',['url'=>'/fk/api?act=add']);
            }
        }
        if ($act == 'edit') {
            if (unit::is_post()) {
                $data = input('post.');
                return Control::api('editsave', $id, $data);
            } else {
                $row = Data::find($id);
                return view($this->tpl . 'add.html',['data'=>$row['data'],'url'=>'/fk/api?act=edit&id='.$id]);
            }
        }
        if ($act == 'save') {
            if (unit::is_post()) {
                $data = input('post.');
                return Control::api('save', $id, $data);
            } else {
                return view($this->tpl . 'add.html');
            }
        }
    }

    /**
     * 函数接口
     * @param string $act
     * @return Json
     */
    public function fun($act = 'index')
    {
        if ($act == 'index') {
            return view($this->tpl . 'fun.html', ['data' => Fun::all()]);
        }
        if ($act == 'add') {
            $data = input('post.');
            return Fun::save($data);
        }
        if ($act == 'sapi') {
            $data = input('post.');
            return Fun::sapi($data);
        }
    }

    /**
     * 资源接口数据
     * @param string $act
     * @return Json
     */
    public function data($act = 'list')
    {
        if ($act == 'list') {
            if (unit::is_post()) {
                $data = Common::file(input('limit'),input('page'));
                return json($data);
            } else {
                return view($this->tpl . 'files.html',['upload'=>unit::gconfig('upload_file')]);
            }
        }
    }
}
	