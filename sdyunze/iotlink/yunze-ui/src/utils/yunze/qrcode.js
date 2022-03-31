import QRCode from 'qrcodejs2'

export default {
    createQRCode:function (id,url) {
        let qrcode = new QRCode(id, {
            width: 200,
            height: 200, // 高度
            text: url, // 二维码内容
            background: '#f0f',
            foreground: '#ff0',
        })
    },

}