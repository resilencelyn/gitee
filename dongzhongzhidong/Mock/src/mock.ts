// global require, module, window
import Handler from './mock/handler';
import * as Util from './mock/util';
import * as Random from './mock/random/index';
// import { toJSONSchema } from './mock/schema/index';
// import { valid } from './mock/valid/index';
// import { _mocked } from './mock/_mocked';
import { mock } from './mock/mock';

/* !
    Mock - 模拟请求 & 模拟数据
    https://github.com/nuysoft/Mock
    墨智 mozhi.gyy@taobao.com nuysoft@gmail.com

    此版本为 esm 版本
    KonghaYao 修改于 2021/6/11
    https://github.com/KonghaYao/
*/

const Mock = {
    Handler,
    Random,
    Util,

    heredoc: Util.heredoc,

    mock,
};

// //ifdef browser
// // ! 浏览器中开始代理 XHR
// import { XHR, mockFetch } from './mock/xhr/index';
// const window = globalThis.window || {};
// // 拦截 XHR
// if (window.XMLHttpRequest !== XHR) window.XMLHttpRequest = XHR;
// Mock.XHR = XHR;
// Mock.fetch = fetch;
// mockFetch();
// Mock.setup = (settings) => XHR.setup(settings);
// //endif

// Mock.version = '1.1.1-es6';

/*
    * Mock.mock( template )
    * Mock.mock( function() )
    * Mock.mock( rurl, template )
    * Mock.mock( rurl, function(options) )
    * Mock.mock( rurl, rtype, template )
    * Mock.mock( rurl, rtype, function(options) )

    根据数据模板生成模拟数据。
*/

export default Mock;
