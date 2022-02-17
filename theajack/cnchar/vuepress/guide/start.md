
## 1. npm安装

cnchar包含了一个基本库和六个功能库，用于实现不同的功能，您可以按需安装其中的功能库，但是 `cnchar` 这个基础库是必须安装的（draw、idiom、xhy、radical四个库可以独立使用）

安装[主库](https://npmjs.com/package/cnchar)：

<div>
  <highlight-code>
npm i cnchar
  </highlight-code>
</div>

按需安装功能库：

<div>
  <highlight-code>
npm i cnchar-poly cnchar-order cnchar-trad cnchar-draw cnchar-idiom cnchar-xhy cnchar-radical
  </highlight-code>
</div>

浏览器环境中功能库可以直接使用：

<div>
  <highlight-code lang='javascript'>
    import cnchar from 'cnchar';
    // 以下功能库请按需使用
    import 'cnchar-poly';
    import 'cnchar-order';
    import 'cnchar-trad';
    import 'cnchar-draw';
    import 'cnchar-idiom';
    import 'cnchar-xhy';
    import 'cnchar-radical';
  </highlight-code>
</div>

非浏览器环境中功能库需要使用use方法加载，且不支持 `cnchar-draw` 库：

<div>
  <highlight-code lang='javascript'>
    import cnchar from 'cnchar';
    // 以下功能库请按需使用
    import poly from 'cnchar-poly';
    import order from 'cnchar-order';
    import trad from 'cnchar-trad';
    import idiom from 'cnchar-idiom';
    import xhy from 'cnchar-xhy';
    import radical from 'cnchar-radical';
    cnchar.use(poly, order, trad, idiom, xhy, radical);
  </highlight-code>
</div>

commonjs 模块

<div>
  <highlight-code lang='javascript'>
    var cnchar = require('cnchar');
    // 以下功能库请按需使用
    var poly = require('cnchar-poly');
    var order = require('cnchar-order');
    var trad = require('cnchar-trad');
    var idiom = require('cnchar-idiom');
    var xhy = require('cnchar-xhy');
    var radical = require('cnchar-radical');
    cnchar.use(poly, order, trad, idiom, xhy, radical);
  </highlight-code>
</div>

## 2. CDN引用

使用 script 标签使用：

<div>
  <highlight-code lang='html'>
      &lt;script src="https://cdn.jsdelivr.net/npm/cnchar/cnchar.min.js">&lt;/script>
      &lt;!--以下功能库请按需使用-->
      &lt;script src="https://cdn.jsdelivr.net/npm/cnchar-poly/cnchar.poly.min.js">&lt;/script>
      &lt;script src="https://cdn.jsdelivr.net/npm/cnchar-order/cnchar.order.min.js">&lt;/script>
      &lt;script src="https://cdn.jsdelivr.net/npm/cnchar-trad/cnchar.trad.min.js">&lt;/script>
      &lt;script src="https://cdn.jsdelivr.net/npm/cnchar-draw/cnchar.draw.min.js">&lt;/script>
      &lt;script src="https://cdn.jsdelivr.net/npm/cnchar-idiom/cnchar.idiom.min.js"> &lt;/script>
      &lt;script src="https://cdn.jsdelivr.net/npm/cnchar-radical/cnchar.radical.min.js"> &lt;/script>
      &lt;script src="https://cdn.jsdelivr.net/npm/cnchar-xhy/cnchar.xhy.min.js"> &lt;/script>
  </highlight-code>
</div>

## 3. cnchar-all

如果您需要使用cnchar及其插件的所有功能，可以通过安装`cnchar-all`来使用完整功能，这个库引用了上面的七个库

<div>
  <highlight-code>
npm i cnchar-all
  </highlight-code>
</div>

commonjs 模块

<div>
  <highlight-code lang='javascript'>
    import cnchar from 'cnchar-all';
  </highlight-code>
</div>

cdn方式

<div>
  <highlight-code lang='html'>
    &lt;script src="https://cdn.jsdelivr.net/npm/cnchar-all/cnchar.all.min.js">&lt;/script>
  </highlight-code>
</div>

## 4. 简单使用

cnchar 具有两个最核心的方法`spell` 和 `stroke`，分别用于获取汉字的拼音和笔画数：

<div>
  <codebox id='easy-use' title='spell/stroke'></codebox>
</div>

这只是一个最简单的使用，这两个方法具有很多参数可选，具体请参阅 [cnchar](/cnchar/doc/cnchar) 部分

<!-- <codebox title='spell' id='spell' desc='拼写测试测试2'></codebox> -->


<!-- <baseComponent-star></baseComponent-star> -->

