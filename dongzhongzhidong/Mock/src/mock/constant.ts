/*
    [正则查看工具](http://www.regexper.com/)

    #26 生成规则 支持 负数，例如 number|-100-100
*/
/** 常量集合 */
export default {
    GUID: 1,
    /**
        RE_KEY
        'name|min-max': value
        'name|count': value
        'name|min-max.dmin-dmax': value
        'name|min-max.dcount': value
        'name|count.dmin-dmax': value
        'name|count.dcount': value
        'name|+step': value

        1 name, 2 step, 3 range [ min, max ], 4 drange [ dmin, dmax ]

     */
    RE_KEY: /(.+)\|(?:\+(\d+)|([\+\-]?\d+-?[\+\-]?\d*)?(?:\.(\d+-?\d*))?)/,
    RE_RANGE: /([\+\-]?\d+)-?([\+\-]?\d+)?/,
    /**
     * RE_PLACEHOLDER placeholder(*)
     * ! 这个被优化了
     */
    //  '@name(123232) @some @nnn(23232,3232) \\@mame'.replace(/(^@|(?<=[^\\])@)([^@#%&()\?\s]+)(?:\((.*?)\))?/g,(all,name,keys)=>{
    //     console.log(name,keys)
    //     return '结果替换'
    // })
    RE_PLACEHOLDER: /(^@|(?<=[^\\])@)([^@#%&()\?\s]+)(?:\((.*?)\))?/g,
};
