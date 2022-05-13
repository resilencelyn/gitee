# HTTP API

NanoMQ 提供了 HTTP API 以实现与外部系统的集成，例如查询 broker 统计信息、客户端信息、发布消息，订阅主题信息和远程修改配置/重启等。

NanoMQ 的 HTTP API 服务默认监听 8081 端口。可通过 `etc/nanomq.conf` 配置文件修改监听端口，所有 API 调用均以 `api/v1` 开头。

## 接口安全

NanoMQ 的 HTTP API 使用 [Basic 认证](https://en.wikipedia.org/wiki/Basic_access_authentication)或[JWT 认证](https://jwt.io/introduction)方式。`username` 和 `password` 须分别填写。 默认的`username` 和 `password` 是：`admin/public`。 可通过 `etc/nanomq.conf` 配置文件修改 `username` 和 `password` 。



## 响应码

### HTTP 状态码 (status codes)

NanoMQ 接口在调用成功时总是返回 200 OK，响应内容则以 JSON 格式返回。

可能的状态码如下：

| Status Code | Description                                              |
| ----------- | -------------------------------------------------------- |
| 200         | 成功，返回的 JSON 数据将提供更多信息                     |
| 400         | 客户端请求无效，例如请求体或参数错误                     |
| 401         | 客户端未通过服务端认证，使用无效的身份验证凭据可能会发生 |
| 404         | 找不到请求的路径或者请求的对象不存在                     |
| 500         | 服务端处理请求时发生内部错误                             |

### 返回码 (result codes)

NanoMQ 接口的响应消息体为 JSON 格式，其中总是包含返回码 `code`。

可能的返回码如下：

| Return Code | Description                |
| ----------- | -------------------------- |
| 0           | 成功                       |
| 101         | RPC 错误                   |
| 102         | 未知错误                   |
| 103         | 用户名或密码错误           |
| 104         | 空用户名或密码             |
| 105         | 用户不存在                 |
| 106         | 管理员账户不可删除         |
| 107         | 关键请求参数缺失           |
| 108         | 请求参数错误               |
| 109         | 请求参数不是合法 JSON 格式 |
| 110         | 插件已开启                 |
| 111         | 插件已关闭                 |
| 112         | 客户端不在线               |
| 113         | 用户已存在                 |
| 114         | 旧密码错误                 |
| 115         | 不合法的主题               |
| 116         | Token已过期               |



## API Endpoints | POST /api/v1

### NanoMQ 统计信息

返回 NanoMQ 的统计信息.

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 2      | 请求码 _2_。                                             |
| seq  | Integer | Required | unique | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |

**Success Response Body (JSON):**

| Name              | Type    | Description                                                  |
| ----------------- | ------- | ------------------------------------------------------------ |
| code              | Integer | 0                                                            |
| seq               | Integer | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |
| rep               | Integer | rep 是 2 作为 req 2 的响应。                                 |
| data.client_size  | Integer | 订阅客户端的数量。                                           |
| data.message_in   | Integer | NanoMQ 流入的消息数量。                                      |
| data.message_out  | Integer | NanoMQ 流出的消息数量。                                      |
| data.message_drop | Integer | NanoMQ 丢弃的消息数量。                                      |

#### **Examples:**

```shell
$ curl -i --basic -u admin:public -X POST "http://localhost:8081/api/v1" -d '{"req": 2,"seq": 1111111}'
{"code":0,"seq":1111111,"rep":2,"data":{"client_size":1,"message_in":4,"message_out":0,"message_drop":4}}
```



### 主题信息

返回客户端标识符对应的主题和 qos 信息。

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 4      | 请求码 _4_。                                             |
| seq  | Integer | Required | unique | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |

**Success Response Body (JSON):**

| Name                           | Type    | Description                                                  |
| ------------------------------ | ------- | ------------------------------------------------------------ |
| code                           | Integer | 0                                                            |
| seq                            | Integer | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |
| rep                            | Integer | rep 是 4 作为 req 4 的响应。                                 |
| data[0].client_id              | String  | 客户端订阅标识符。                                           |
| data[0].subscriptions[0].topic | String  | 订阅的主题。                                                 |
| data[0].subscriptions[0].qos   | Integer | 订阅的 qos                                                   |

#### **Examples:**

```shell
$ curl -i --basic -u admin:public -X POST "http://localhost:8081/api/v1" -d '{"req": 4,"seq": 1111111}'
{"code":0,"seq":1111111,"rep":4,"data":[{"client_id":"nanomq-ebd54382","subscriptions":[{"topic":"a/b/c","qos":0}]}]}
```

#### 客户端信息

返回所有的客户端信息。

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 5      | 请求码 _5_。                                             |
| seq  | Integer | Required | unique | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |

**Success Response Body (JSON):**

| Name                    | Type    | Description                                                  |
| ----------------------- | ------- | ------------------------------------------------------------ |
| code                    | Integer | 0                                                            |
| seq                     | Integer | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |
| rep                     | Integer | rep 是 5 作为 req 5 的响应。                                 |
| data[0].client_id       | String  | 客户端订阅标识符。                                           |
| data[0].username        | String  | 用户名。                                                     |
| data[0].keepalive       | Integer | 保活。                                                       |
| data[0].protocol        | Integer | 协议版本。                                                   |
| data[0].connect_status  | Integer | 连接状态。                                                   |
| data[0].message_receive | Integer | 该客户端接受的消息。                                         |

#### **Examples:**

```shell
$ curl -i --basic -u admin:public -X POST "http://localhost:8081/api/v1" -d '{"req": 5,"seq": 1111111}'
{"code":0,"seq":1111111,"rep":5,"data":[{"client_id":"nanomq-ebd54382","username":"nanmq","keepalive":60,"protocol":4,"connect_status":1,"message_receive":0}]
```



### 获取当前配置

返回当前broker所有配置参数。

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 11     | 请求码 _11_。                                            |
| seq  | Integer | Required | unique | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |

**Success Response Body (JSON):**

| Name                  | Type    | Description                                                  |
| --------------------- | ------- | ------------------------------------------------------------ |
| code                  | Integer | 0                                                            |
| seq                   | Integer | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |
| rep                   | Integer | rep 是 11 作为 req 11 的响应。                               |
| data.url              | String  | 监听url。                                                    |
|data.num_taskq_thread | Integer | 任务线程数。 |
|data.max_taskq_thread | Integer | 最大任务线程数。 |
|data.parallel |Long  | 并行数。 |
|data.property_size |Integer  | 最大属性长度。 |
|data.msq_len | Integer | 队列长度。 |
|data.qos_duration | Integer | QOS消息定时间隔时间。 |
|data.allow_anonymous | Boolean | 允许匿名登录。 |
|data.tls.enable | Boolean | 启动TLS监听。 |
|data.tls.url |String  | TLS监听URL。 |
|data.tls.key | String | TLS私钥数据。 |
|data.tls.keypass | String | TLS私钥密码。 |
|data.tls.cert |String  | TLS Cert证书数据。 |
|data.tls.cacert | String | TLS CA证书数据。|
|data.tls.verify_peer | Boolean | 验证客户端证书 |
|data.tls.fail_if_no_peer_cert | Boolean | 拒绝无证书连接，与_.tls.verify_peer_配合使用。 |
|data.websocket.enable | Boolean | 启动websocket监听。 |
|data.websocket.url | String  | Websocket监听URL。 |
|data.websocket.tls_url |  String | TLS over Websocket监听URL。 |
|data.http_server.enable| Boolean | 启动Http服务监听。 |
|data.http_server.port | Integer | Http服务端监听端口。 |
|data.http_server.username | String | 访问Http服务用户名。 |
|data.http_server.password | String | 访问Http服务密码。 |
|data.bridge.bridge_mode | Boolean | 启动桥接功能。  |
|data.bridge.address | String | 桥接目标broker地址。|
|data.bridge.proto_ver | String | 桥接客户端MQTT版本（3｜4｜5）。 |
|data.bridge.clientid | String | 桥接客户端ID。（NULL为自动生成随机ID） |
|data.bridge.keepalive | Integer | 保活间隔时间。 |
|data.bridge.clean_start | Boolean | 清除会话。 |
|data.bridge.parallel | Long | 桥接客户端并发数。 |
|data.bridge.username | String | 登录用户名。 |
|data.bridge.password | String | 登录密码。 |
|data.bridge.forwards | Array[String] | 转发Topic数组。 |
| data.bridge.forwards[0]           | String        | 转发Topic。 |
| data.bridge.subscription          | Array[Object] | 订阅信息数组。                                               |
| data.bridge.subscription[0].topic | String        | 订阅Topic。                                                  |
| data.bridge.subscription[0].qos   | Integer       | 订阅消息质量Qos。 |

Example:

```shell
$ curl --location --request GET 'http://localhost:8081/api/v1' \
--header 'Authorization: Basic YWRtaW46cHVibGlj' \
--header 'Content-Type: application/json' \
--data-raw '{
    "req": 11,
    "seq": 9999
}'
{
    "code": 0,
    "seq": 9999,
    "rep": 11,
    "data": {
        "url": "nmq-tcp://0.0.0.0:1883",
        "num_taskq_thread": 4,
        "max_taskq_thread": 4,
        "parallel": 32,
        "property_size": 32,
        "msq_len": 64,
        "allow_anonymous": true,
        "daemon": false,
        "tls": {
            "enable": false,
            "url": "tls+nmq-tcp://0.0.0.0:8883",
            "key_password": null,
            "key": null,
            "cert": null,
            "cacert": null,
            "verify_peer": false,
            "fail_if_no_peer_cert": false
        },
        "websocket": {
            "enable": true,
            "url": "nmq-ws://0.0.0.0:8083/mqtt",
            "tls_url": "nmq-wss://0.0.0.0:8084/mqtt"
        },
        "http_server": {
            "enable": true,
            "port": 8081,
            "username": "admin",
            "password": "public",
            "auth_type": "basic"
        },
        "bridge": {
            "bridge_mode": false,
            "address": "mqtt-tcp://broker.emqx.io:1883",
            "proto_ver": 4,
            "clientid": "bridge_client",
            "clean_start": true,
            "username": "username",
            "password": "passwd",
            "keepalive": 60,
            "parallel": 2,
            "forwards": [
                "topic1/#",
                "topic2/#"
            ],
            "subscription": [
                {
                    "topic": "cmd/topic1",
                    "qos": 1
                },
                {
                    "topic": "cmd/topic2",
                    "qos": 2
                }
            ]
        }
    }
}
```



### 设置配置参数

设置broker配置参数。

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 12     | 请求码 _12_。                                            |
| seq  | Integer | Required | unique | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |
| data | Object  | Required |        | 同获取配置一致[data](#获取当前配置)。                        |

**Success Response Body (JSON):**

| Name | Type    | Description                                                  |
| ---- | ------- | ------------------------------------------------------------ |
| code | Integer | 0                                                            |
| seq  | Integer | seq 是全局唯一的，请求/响应信息都会携带该信息，可以通过该值确定对应的请求响应。 |
| rep  | Integer | rep 是 12 作为 req 12 的响应。                               |

Example:

```shell
$ curl --location --request POST 'http://localhost:8081/api/v1' \
--header 'Authorization: Basic YWRtaW46cHVibGlj' \
--header 'Content-Type: application/json' \
--data-raw '{
    "req": 12,
    "seq": 111,
    "data": {
        "url": "nmq-tcp://0.0.0.0:1883",
        "num_taskq_thread": 10,
        "max_taskq_thread": 10,
        "parallel": 10,
        "property_size": 32,
        "msq_len": 64,
        "allow_anonymous": true,
        "daemon": false,
        "tls": {
            "enable": false,
            "url": "tls+nmq-tcp://0.0.0.0:8883",
            "key_password": null,
            "key": null,
            "cert": null,
            "cacert": null,
            "verify_peer": false,
            "fail_if_no_peer_cert": false
        },
        "websocket": {
            "enable": true,
            "url": "nmq-ws://0.0.0.0:8083/mqtt",
            "tls_url": "nmq-wss://0.0.0.0:8084/mqtt"
        },
        "http_server": {
            "enable": true,
            "port": 8081,
            "username": "admin",
            "password": "public"
        },
        "bridge": {
            "bridge_mode": true,
            "address": "mqtt-tcp://broker.emqx.io:1883",
            "proto_ver": 4,
            "clientid": "bridge_client",
            "clean_start": true,
            "username": "username",
            "password": "passwd",
            "keepalive": 60,
            "parallel": 2,
            "forwards": [
                "topic1/#",
                "topic2/#"
            ],
            "subscription": [
                {
                    "topic": "cmd/topic1",
                    "qos": 1
                },
                {
                    "topic": "cmd/topic2",
                    "qos": 2
                }
            ]
        }
    }
}'

{"code":0,"seq":111,"rep":12}
```