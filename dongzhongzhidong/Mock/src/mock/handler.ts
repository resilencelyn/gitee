/*
    ## Handler

    处理数据模板。

    * Handler.gen( template, name?, context? )

        入口方法。

    * Data Template Definition, DTD

        处理数据模板定义。

        * Handler.array( options )
        * Handler.object( options )
        * Handler.number( options )
        * Handler.boolean( options )
        * Handler.string( options )
        * Handler.function( options )
        * Handler.regexp( options )

        处理路径（相对和绝对）。

        * Handler.getValueByKeyPath( key, options )

    * Data Placeholder Definition, DPD

        处理数据占位符定义

        * Handler.placeholder( placeholder, context, templateContext, options )

*/

import { gen } from './handle/gen.js';
/*
    template        属性值（即数据模板）
    name            属性名
    context         数据上下文，生成后的数据
    templateContext 模板上下文，

    Handle.gen(template, name, options)
    context
        currentContext, templateCurrentContext,
        path, templatePath
        root, templateRoot
*/

import * as Handle from './handle/index';

import { placeholder } from './handle/placeholder.js';

const defaultExport = {
    gen,
    placeholder,
    ...Handle,
};
export default defaultExport;
