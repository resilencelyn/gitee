export default {
    cptDataForm:{
        dataSource: 1, pollTime: 0,
        dataText: '[["行1列1","行1列2","行1列3"],["行2列1","行2列2","行2列3"],["行3列1","行3列2","行3列3"],["行4列1","行4列2","行4列3"],["行5列1","行5列2","行5列3"],["行6列1","行6列2","行6列3"],["行7列1","行7列2","行7列3"],["行8列1","行8列2","行8列3"],["行9列1","行9列2","行9列3"],["行10列1","行10列2","行10列3"]]'
    },
    attribute:{
        header: ['列1', '列2', '列3'],//表头数据
        rowNum: 4,//显示行数
        headerBGC:'#00BAFF',//表头背景色
        oddRowBGC:'#003B51',//奇数行背景色
        evenRowBGC:'#0A2732',//偶数行背景色
        waitTime: 2000,//轮播时间间隔(ms)
        headerHeight: 35,//表头高度
        indexHeader: '#a',//行号表头
        carousel:'single',//轮播方式'single'|'page'
        hoverPause:true,//悬浮暂停轮播
        index: true,//开启序号
        columnWidth: [50],//列宽
        align: ['center'],//对齐方式
    }
}
