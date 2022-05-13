# HTTP API

NanoMQ Broker provides HTTP APIs for integration with external systems, such as querying broker statistics information, clients information, subscribe topics information, and restart with new config file.

NanoMQ Broker's HTTP API service listens on port 8081 by default. You can modify the listening port through the configuration file of `etc/nanomq.conf`. All API calls with `api/v1`.

## Interface security

NanoMQ Broker's HTTP API uses the method of [Basic Authentication](https://en.wikipedia.org/wiki/Basic_access_authentication) or [JWT Authentication](https://jwt.io/introduction). The `username` and `password` must be filled. The default `username` and `password` are: `admin/public`. You can modify username and password through the configuration file of `etc/nanomq.conf`.

## Response code

### HTTP status codes

The NanoMQ Broker interface always returns 200 OK when the call is successful, and the response content is returned in JSON format.

The possible status codes are as follows:

| Status Code | Description                                                  |
| ----------- | ------------------------------------------------------------ |
| 200         | Succeed, and the returned JSON data will provide more information |
| 400         | Invalid client request, such as wrong request body or parameters |
| 401         | Client authentication failed , maybe because of invalid authentication credentials |
| 404         | The requested path cannot be found or the requested object does not exist |
| 500         | An internal error occurred while the server was processing the request |

### result codes

The response message body of the NanoMQ Broker interface is in JSON format, which always contains the returned `code`.

The possible returned codes are as follows:

| Return Code | Description                                     |
| ----------- | ----------------------------------------------- |
| 0           | Succeed                                         |
| 101         | RPC error                                       |
| 102         | unknown mistake                                 |
| 103         | wrong user name or password                     |
| 104         | Empty username or password                      |
| 105         | User does not exist                             |
| 106         | Administrator account cannot be deleted         |
| 107         | Missing key request parameters                  |
| 108         | Request parameter error                         |
| 109         | Request parameters are not in legal JSON format |
| 110         | Plug-in is enabled                              |
| 111         | Plugin is closed                                |
| 112         | Client is offline                               |
| 113         | User already exists                             |
| 114         | Old password is wrong                           |
| 115         | Illegal subject                                 |
| 116         | Token expired                                   |

## API Endpoints | POST /api/v1

### Broker info

Returns the information of broker.

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 2      | Request Code _2_.                                            |
| seq  | Integer | Required | unique | The seq is a unique number, response will carry this field. So you can know correspondence between request and response. |

**Success Response Body (JSON):**

| Name              | Type    | Description                                                  |
| ----------------- | ------- | ------------------------------------------------------------ |
| code              | Integer | 0                                                            |
| seq               | Integer | The seq is a unique number, response get this value from request. So you can know correspondence between request and response. |
| rep               | Integer | The rep equal 2 as response to req equal 2.                  |
| data.client_size  | Integer | Subscribe client size.                                       |
| data.message_in   | Integer | Statistic broker  message in.                                |
| data.message_out  | Integer | Statistic broker message out.                                |
| data.message_drop | Integer | Statistic broker message dropped.                            |

**Examples**:

```shell
$ curl -i --basic -u admin:public -X POST "http://localhost:8081/api/v1" -d '{"req": 2,"seq": 1111111}'
{"code":0,"seq":1111111,"rep":2,"data":{"client_size":1,"message_in":4,"message_out":0,"message_drop":4}}
```

### Topic info

Returns the information of all subscribe topics with client identifier and qos.

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 4      | Request Code _4_.                                            |
| seq  | Integer | Required | unique | The seq is a unique number, response will carry this field. So you can know correspondence between request and response. |

**Success Response Body (JSON):**

| Name                           | Type    | Description                                                  |
| ------------------------------ | ------- | ------------------------------------------------------------ |
| code                           | Integer | 0                                                            |
| seq                            | Integer | The seq is a unique number, response get this value from request. So you can know correspondence between request and response. |
| rep                            | Integer | The rep equal 4 as response to req equal 4.                  |
| data[0].client_id              | String  | client identifier.                                           |
| data[0].subscriptions[0].topic | String  | Subscribe topic.                                             |
| data[0].subscriptions[0].qos   | Integer | Subscribe qos.                                               |

**Examples**:

```shell
$ curl -i --basic -u admin:public -X POST "http://localhost:8081/api/v1" -d '{"req": 4,"seq": 1111111}'
{"code":0,"seq":1111111,"rep":4,"data":[{"client_id":"nanomq-ebd54382","subscriptions":[{"topic":"a/b/c","qos":0}]}]}
```

### Client info

Returns the information of all clients.

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 5      | Request Code _5_.                                            |
| seq  | Integer | Required | unique | The seq is a unique number, response will carry this field. So you can know correspondence between request and response. |

**Success Response Body (JSON):**

| Name                    | Type    | Description                                                  |
| ----------------------- | ------- | ------------------------------------------------------------ |
| code                    | Integer | 0                                                            |
| seq                     | Integer | The seq is a unique number, response get this value from request. So you can know correspondence between request and response. |
| rep                     | Integer | The rep equal 5 as response to req equal 5.                  |
| data[0].client_id       | String  | client identifier.                                           |
| data[0].username        | String  | Username.                                                    |
| data[0].keepalive       | Integer | Keepalive.                                                   |
| data[0].protocol        | Integer | Protocol version.                                            |
| data[0].connect_status  | Integer | Connected status.                                            |
| data[0].message_receive | Integer | Received message of this client.                             |

**Examples:**

```shell
$ curl -i --basic -u admin:public -X POST "http://localhost:8081/api/v1" -d '{"req": 5,"seq": 1111111}'
{"code":0,"seq":1111111,"rep":5,"data":[{"client_id":"nanomq-ebd54382","username":"nanmq","keepalive":60,"protocol":4,"connect_status":1,"message_receive":0}]
```

### Get Configuration

Read all of configure parameters from broker.

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 11     | Request Code _11_.                                           |
| seq  | Integer | Required | unique | The seq is a unique number, response will carry this field. So you can know correspondence between request and response. |

**Success Response Body (JSON):**

| Name                  | Type    | Description                                                  |
| --------------------- | ------- | ------------------------------------------------------------ |
| code                  | Integer | 0                                                            |
| seq                   | Integer | The seq is a unique number, response will carry this field. So you can know correspondence between request and response. |
| rep                   | Integer | The rep equal 11 as response to req equal 11. |
| data.url              | String  | Url of listener.                        |
|data.num_taskq_thread | Integer | Number of taskq threads used. |
|data.max_taskq_thread | Integer | Maximum number of taskq threads used。 |
|data.parallel |Long  | Number of parallel.                                          |
|data.property_size |Integer  | Max size for a MQTT property. |
|data.msq_len | Integer | Queue length for resending messages. |
|data.qos_duration | Integer | The interval of the qos timer.                               |
|data.allow_anonymous | Boolean | Allow anonymous login.                                       |
|data.tls.enable | Boolean | Enable TLS listener.                                         |
|data.tls.url | String | URL of TLS listener. |
|data.tls.key | String | User's private PEM-encoded key. |
|data.tls.keypass | String | String containing the user's password. Only used if the private keyfile is password-protected. |
|data.tls.cert |String  | User certificate data.                                       |
|data.tls.cacert | String | User's PEM-encoded CA certificates.                          |
|data.tls.verify_peer | Boolean | Verify peer certificate.                                     |
|data.tls.fail_if_no_peer_cert | Boolean | Server will fail if the client does not have a certificate to send. |
|data.websocket.enable | Boolean | Enable websocket listener. |
|data.websocket.url | String  | URL of websocket listener. |
|data.websocket.tls_url |  String | URL of TLS over websocket listerner. |
|data.http_server.enable| Boolean | Enable http server listerner. |
|data.http_server.port | Integer | Port of http server. |
|data.http_server.username | String | User name of http server. |
|data.http_server.password | String | Password of http server. |
|data.bridge.bridge_mode | Boolean | Enter MQTT bridge mode .                                           |
|data.bridge.address | String | Remote Broker address. |
|data.bridge.proto_ver | String | MQTT client version（3｜4｜5）。 |
|data.bridge.clientid | String | MQTT client identifier. |
|data.bridge.keepalive | Integer | Interval of keepalive.                                       |
|data.bridge.clean_start | Boolean | Clean seeson.                                                |
|data.bridge.parallel | Long | Parallel of mqtt client。 |
|data.bridge.username | String | Login user name. |
|data.bridge.password | String | Login password. |
|data.bridge.forwards | Array[String] | Array of forward topics. |
| data.bridge.forwards[0]           | String        | Topic. |
| data.bridge.subscription          | Array[Object] | Array of subscriptions.                         |
| data.bridge.subscription[0].topic | String        | Topic.                                                 |
| data.bridge.subscription[0].qos   | Integer       | Qos. |

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
            "password": "public"
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

### Set Configuration

Set configure parameters for broker.

**Parameters (json):**

| Name | Type    | Required | Value  | Description                                                  |
| ---- | ------- | -------- | ------ | ------------------------------------------------------------ |
| req  | Integer | Required | 12     | Request Code  _12_.                                          |
| seq  | Integer | Required | unique | The seq is a unique number, response will carry this field. So you can know correspondence between request and response. |
| data | Object  | Required |        | See _Get Configuration_ [data](#Get Configuration).          |

**Success Response Body (JSON):**

| Name | Type    | Description                                                  |
| ---- | ------- | ------------------------------------------------------------ |
| code | Integer | 0                                                            |
| seq  | Integer | The seq is a unique number, response will carry this field. So you can know correspondence between request and response. |
| rep  | Integer | The rep equal 12 as response to req equal 12.                |

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
            "password": "public",
            "auth_type": "basic"
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
