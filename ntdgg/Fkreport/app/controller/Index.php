<?php
namespace app\controller;

use app\BaseController;
use think\facade\Db;
use think\facade\Request;
use think\facade\Filesystem;
use Fkreport\Api;

class Index extends BaseController
{
    public function index(){
	echo '<style type="text/css"> a{color:#2E5CD5;cursor: pointer;text-decoration: none} 
	body{ background: #fff; font-family: "Century Gothic","Microsoft yahei"; color: #333;font-size:18px;} 
	h1{ font-size: 100px; font-weight: normal; margin-bottom: 12px; } 
	p{ line-height: 1.2em; font-size: 36px }
	li {font-size: x-large;margin: 10px;}
	</style>
	<title>Fkreport 方块报表 企业级BI报表 PHP数据报表</title>
	<meta name="keywords" content="Fkreport 方块报表 企业级BI报表 PHP数据报表">
	<meta name="description" content="Fkreport 方块报表 企业级BI报表 PHP数据报表">
	<div style="padding: 12px 48px;"> <h1>(●◡●)</h1>
	<p> Fkreport V1.0.0<br/>
	<span style="font-size:30px;">很高兴，在此与您相遇~ </span><br/>
	</p>
	<p>您可以：
		<a href="/fk/api?act=index" target="_blank"  class="c-primary">演示</a>
		<span class="ml-20">|</span>
		<a href="https://gitee.com/ntdgg/Fkreport" target="_blank"  class="c-primary">下载</a>
		<span class="ml-20">|</span>
		<a href="https://gadmin8.com/index/product.html" target="_blank"  class="c-primary ml-20">文档 </a>
		<span class="ml-20">|</span>
		<a href="https://www.cojz8.com/topic/2" target="_blank" class="c-primary ml-20">授权 </a>
	</p>
	</div>
	<div style="padding: 0px 48px;">
	<b>全新发布 </b>
	<ul>
	    <li>
			Tpflow工作流引擎
				<a href="http://tpflow.gadmin8.com" target="_blank"  class="c-primary">演示</a>
				<span class="ml-20">|</span>
				<a href="https://gadmin8.com/index/product.html" target="_blank"  class="c-primary ml-20">文档 </a>
				<span class="ml-20">|</span>
				<a href="https://www.cojz8.com/topic/2" target="_blank" class="c-primary ml-20">授权 </a>
		</li>
		<li>
			Gadmin3.0企业级极速开发框架：
				<a href="http://demo.gadmin8.com" target="_blank"  class="c-primary">演示</a>
				<span class="ml-20">|</span>
				<a href="https://gadmin8.com/index/product.html" target="_blank"  class="c-primary ml-20">文档 </a>
				<span class="ml-20">|</span>
				<a href="https://gadmin8.com/index/Page/index.html?cate=14" target="_blank" class="c-primary ml-20">授权 </a>
		</li>
		<li>
			SFDP5.0超级表单开发系统：
				<a href="http://sfdp.gadmin8.com" target="_blank"  class="c-primary">演示</a>
				<span class="ml-20">|</span>
				<a href="https://gadmin8.com/index/product.html" target="_blank"  class="c-primary ml-20">文档 </a>
				
		</li>
	</ul>
	</div>
	<div style="padding: 0px 48px;">
	<b>友情链接 </b><br/>
	        <a href="http://liuzhiyun.com" target="_blank">流之云科技</a>|
			<a href="http://ccflow.org" target="_blank">Java工作流</a>|
			<a href="https://www.luhu.co" target="_blank">芦虎导航</a>|
			<a href="https://www.df81.com" target="_blank">企业网站管理系统</a>|
			<a href="https://www.erdaicms.cn" target="_blank">驴赞旅游saas平台</a>|
			<a href="https://www.jiuliankeji.com" target="_blank">甘肃软件开发</a>|
			
	</ul>
	</div>
<script>
var _hmt = _hmt || [];
(function() {
  var hm = document.createElement("script");
  hm.src = "https://hm.baidu.com/hm.js?2c75a2c57ea0b828b54c23a42dd62fd5";
  var s = document.getElementsByTagName("script")[0]; 
  s.parentNode.insertBefore(hm, s);
})();
</script>
';

    }
	public function upload(){
		$post = $this->request->post();
		$files = $this->request->file('file');
		$info = Filesystem::disk('public')->putFile('uploads', $files);
		if($info){
		    $f['title'] = $files->getOriginalName();
            $f['src'] = str_replace("\\",'/',$info);
            $f['ext'] = $files->getOriginalExtension();
            Db::name('fk_imags')->insertGetId($f);
        }
		return json(['code' => 0, 'data' => $info]);
	}
}
