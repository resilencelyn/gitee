function ws(cfg) {
    cfg = cfg || {};
    // web socket
    let client = window.client;
    if (client && (client.readyState == 1 || client.readyState == 2)) {
        console.log('Websocket连接可用不用重创建. state: ' + client.readyState);
        return;
    }
    function doCreate() {
        console.log('create websocket ...');
        try {
            let protocol = "ws://";
            if (window.location.protocol.startsWith("https")) protocol = "wss://";
            client = new WebSocket(protocol + window.location.host + (cfg.path || "/test/msg"));
            window.client = client;
        } catch (e) {
            console.log('创建websocket错误', e);
            setTimeout( () => ws(cfg), cfg.reconnection || (1000 * 60 * 2)); // 每两分钟重试
            return
        }
        client.onclose = cfg.onClose || function (e) {
            console.log('websocket close', e);
            // setTimeout(function () {
            //     ws(cfg)
            // }, cfg.reconnection || (1000 * 60 * 2)); // 每两分钟重试
        };
        client.onmessage = cfg.onMsg || function (e) { //接收websocket 消息
            console.log('ws msg: ', e.data)
        };
        client.onopen = cfg.onOpen || function() {
            console.log('websocket onopen');
            client.send('成功连接...');
        };
    }
    doCreate()
}

function toJSON(str) {
    if (typeof str == 'string') {
        try {
            let obj = JSON.parse(str);
            return obj
        } catch(e) {}
    }
    return null
}

function toQuery(data) {
    const params = new URLSearchParams();
    for (const key in data) {
        const v = data[key]
        if (v == null || (typeof v === 'number' && isNaN(v))) continue
        if (v instanceof Date) {
            params.append(key, v.getTime());
        } else {
            params.append(key, v);
        }
    }
    return params.toString()
}


function clone(src, dest = null) {
    // Handle the 3 simple types, and null or undefined
    if (null == src || "object" != typeof src) return src;

    // Handle Date
    if (src instanceof Date) {
        const copy = new Date();
        copy.setTime(src.getTime());
        return copy;
    }

    // Handle Array
    if (src instanceof Array) {
        if (dest === null || dest === undefined) dest = []
        for (let i = 0, len = src.length; i < len; ++i) {
            dest[i] = clone(src[i]);
        }
        return dest;
    }

    // Handle Object
    if (src instanceof Object) {
        if (dest === null || dest === undefined) dest = {}
        for (const attr in src) {
            if (src.hasOwnProperty(attr)) dest[attr] = clone(src[attr]);
        }
        return dest;
    }

    throw new Error("Unable to copy obj! Its type isn't supported.");

}

// js 实现 复制
function jsCopy(content) {
    const el = document.createElement('input')
    el.value = content
    document.body.appendChild(el)
    el.select()
    document.execCommand('Copy')
    el.remove()
}

function uuid() {
    return (((1+Math.random())*0x10000)|0).toString(16) + (((1+Math.random())*0x10000)|0).toString(16) + '_' + new Date().getTime()
}