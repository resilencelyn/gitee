/*
const CompressionWebpackPlugin = require('compression-webpack-plugin')
const productionGzipExtensions = ['js', 'css']

*/

module.exports={
    devServer: {
        disableHostCheck: true,
    },
   /* configureWebpack:{
        resolve:{
            alias:{
                '@':path.resolve(__dirname, './src'),
                '@i':path.resolve(__dirname, './src/assets'),
            }
        },
        plugins: [
            // 配置compression-webpack-plugin压缩
            new CompressionWebpackPlugin({
                algorithm: 'gzip',
                test: new RegExp('\\.(' + productionGzipExtensions.join('|') + ')$'),
                threshold: 10240,
                minRatio: 0.8
            }),
            new webpack.optimize.LimitChunkCountPlugin({
                maxChunks: 5,
                minChunkSize: 100
            })
        ]
    },*/
    productionSourceMap: false,
    publicPath:process.env.NODE_ENV=== 'production'?'/':'/'


}
