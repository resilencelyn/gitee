
**人脸搜索服务API**


**简介**：<p>人脸搜索服务API</p>


**联系人**:divenswu@163.com


**Version**:1.0.0

**接口路径**：/v2/api-docs


# 01、集合(数据库)管理
    

## 1、创建一个集合(数据库)


**接口描述**:


**接口地址**:`/visual/collect/create`


**请求方式**：`POST`


**consumes**:`["application/json"]`


**produces**:`["*/*"]`


**请求示例**：
```json
{
	"namespace": "",
	"collectionName": "",
	"collectionComment": "",
	"maxDocsPerSegment": 0,
	"shardsNum": 0,
	"sampleColumns": [
		{
			"name": "",
			"comment": "",
			"dataType": ""
		}
	],
	"faceColumns": [
		{
			"name": "",
			"comment": "",
			"dataType": ""
		}
	],
	"syncBinLog": true
}
```


**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|collect| 集合信息  | body | true |CollectReqVo  | CollectReqVo   |

**schema属性说明**



**CollectReqVo**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|namespace| 命名空间：最大12个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|collectionName| 集合名称：最大24个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|collectionComment| 集合描述：最大128个字符  | body | false |string  |    |
|maxDocsPerSegment| 数据分片中最大的文件个数，默认为0（不限制）,仅对Proxima引擎生效  | body | false |integer(int64)  |    |
|shardsNum| 要创建的集合的分片数，默认为0（即系统默认）,仅对Milvus引擎生效  | body | false |integer(int32)  |    |
|sampleColumns| 自定义的样本属性字段  | body | false |array  | FiledColumn   |
|faceColumns| 自定义的人脸属性字段  | body | false |array  | FiledColumn   |
|syncBinLog| 启用binlog同步。扩展字段，暂不支持该功能。  | body | false |boolean  |    |

**FiledColumn**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|name| 字段名称,支持小写字母、数字和下划线的组合，最大32个字符  | body | true |string  |    |
|comment| 字段描述,最大64个字符  | body | false |string  |    |
|dataType| 字段类型，不能为UNDEFINED类型,可用值:UNDEFINED,STRING,BOOL,INT,FLOAT,DOUBLE  | body | true |string  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": true
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |boolean  |    |





**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«boolean»|
## 2、根据命名空间，集合名称删除集合


**接口描述**:


**接口地址**:`/visual/collect/delete`


**请求方式**：`GET`


**consumes**:``


**produces**:`["*/*"]`



**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|collectionName| 集合名称  | query | true |string  |    |
|namespace| 命名空间  | query | true |string  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": true
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |boolean  |    |





**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«boolean»|
## 3、根据命名空间，集合名称查看集合信息


**接口描述**:


**接口地址**:`/visual/collect/get`


**请求方式**：`GET`


**consumes**:``


**produces**:`["*/*"]`



**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|collectionName| 集合名称  | query | true |string  |    |
|namespace| 命名空间  | query | true |string  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": {
		"namespace": "",
		"collectionName": "",
		"collectionComment": "",
		"maxDocsPerSegment": 0,
		"shardsNum": 0,
		"sampleColumns": [
			{
				"name": "",
				"comment": "",
				"dataType": ""
			}
		],
		"faceColumns": [
			{
				"name": "",
				"comment": "",
				"dataType": ""
			}
		],
		"syncBinLog": true
	}
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |CollectRepVo  | CollectRepVo   |



**schema属性说明**




**CollectRepVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|namespace | 命名空间：最大12个字符,支持小写字母、数字和下划线的组合   |string  |    |
|collectionName | 集合名称：最大24个字符,支持小写字母、数字和下划线的组合   |string  |    |
|collectionComment | 集合描述：最大128个字符   |string  |    |
|maxDocsPerSegment | 数据分片中最大的文件个数，默认为0（不限制）,仅对Proxima引擎生效   |integer(int64)  |    |
|shardsNum | 要创建的集合的分片数，默认为0（即系统默认）,仅对Milvus引擎生效   |integer(int32)  |    |
|sampleColumns | 自定义的样本属性字段   |array  | FiledColumn   |
|faceColumns | 自定义的人脸属性字段   |array  | FiledColumn   |
|syncBinLog | 启用binlog同步。扩展字段，暂不支持该功能。   |boolean  |    |

**FiledColumn**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|name | 字段名称,支持小写字母、数字和下划线的组合，最大32个字符   |string  |    |
|comment | 字段描述,最大64个字符   |string  |    |
|dataType | 字段类型，不能为UNDEFINED类型,可用值:UNDEFINED,STRING,BOOL,INT,FLOAT,DOUBLE   |string  |    |

**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«CollectRepVo»|
## 4、根据命名空间查看集合列表


**接口描述**:


**接口地址**:`/visual/collect/list`


**请求方式**：`GET`


**consumes**:``


**produces**:`["*/*"]`



**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|namespace| 命名空间  | query | true |string  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": [
		{
			"namespace": "",
			"collectionName": "",
			"collectionComment": "",
			"maxDocsPerSegment": 0,
			"shardsNum": 0,
			"sampleColumns": [
				{
					"name": "",
					"comment": "",
					"dataType": ""
				}
			],
			"faceColumns": [
				{
					"name": "",
					"comment": "",
					"dataType": ""
				}
			],
			"syncBinLog": true
		}
	]
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |array  | CollectRepVo   |



**schema属性说明**




**CollectRepVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|namespace | 命名空间：最大12个字符,支持小写字母、数字和下划线的组合   |string  |    |
|collectionName | 集合名称：最大24个字符,支持小写字母、数字和下划线的组合   |string  |    |
|collectionComment | 集合描述：最大128个字符   |string  |    |
|maxDocsPerSegment | 数据分片中最大的文件个数，默认为0（不限制）,仅对Proxima引擎生效   |integer(int64)  |    |
|shardsNum | 要创建的集合的分片数，默认为0（即系统默认）,仅对Milvus引擎生效   |integer(int32)  |    |
|sampleColumns | 自定义的样本属性字段   |array  | FiledColumn   |
|faceColumns | 自定义的人脸属性字段   |array  | FiledColumn   |
|syncBinLog | 启用binlog同步。扩展字段，暂不支持该功能。   |boolean  |    |

**FiledColumn**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|name | 字段名称,支持小写字母、数字和下划线的组合，最大32个字符   |string  |    |
|comment | 字段描述,最大64个字符   |string  |    |
|dataType | 字段类型，不能为UNDEFINED类型,可用值:UNDEFINED,STRING,BOOL,INT,FLOAT,DOUBLE   |string  |    |

**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«List«CollectRepVo»»|
# 02、人脸样本管理

## 1、创建一个样本


**接口描述**:


**接口地址**:`/visual/sample/create`


**请求方式**：`POST`


**consumes**:`["application/json"]`


**produces**:`["*/*"]`


**请求示例**：
```json
{
	"namespace": "",
	"collectionName": "",
	"sampleId": "",
	"sampleData": [
		{
			"key": "",
			"value": {}
		}
	]
}
```


**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|sample| 样本信息  | body | true |SampleDataReqVo  | SampleDataReqVo   |

**schema属性说明**



**SampleDataReqVo**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|namespace| 命名空间：最大12个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|collectionName| 集合名称：最大24个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|sampleId| 样本ID：最大32个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|sampleData| 扩展字段  | body | false |array  | FieldKeyValue   |

**FieldKeyValue**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|key| 字段名，与创建集合时给定的字段名一致  | body | true |string  |    |
|value| 字段值，与创建集合时给定的类型一致，若为字符串，最大为512个字符  | body | false |object  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": true
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |boolean  |    |





**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«boolean»|
## 3、根据条件删除样本


**接口描述**:


**接口地址**:`/visual/sample/delete`


**请求方式**：`GET`


**consumes**:``


**produces**:`["*/*"]`



**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|collectionName| 集合名称  | query | true |string  |    |
|namespace| 命名空间  | query | true |string  |    |
|sampleId| 样本ID  | query | true |string  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": true
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |boolean  |    |





**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«boolean»|
## 4、根据条件查看样本


**接口描述**:


**接口地址**:`/visual/sample/get`


**请求方式**：`GET`


**consumes**:``


**produces**:`["*/*"]`



**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|collectionName| 集合名称  | query | true |string  |    |
|namespace| 命名空间  | query | true |string  |    |
|sampleId| 样本ID  | query | true |string  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": {
		"namespace": "",
		"collectionName": "",
		"sampleId": "",
		"sampleData": [
			{
				"key": "",
				"value": {}
			}
		],
		"faces": [
			{
				"faceId": "",
				"faceData": [
					{
						"key": "",
						"value": {}
					}
				],
				"faceScore": 0
			}
		]
	}
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |SampleDataRepVo  | SampleDataRepVo   |



**schema属性说明**




**SampleDataRepVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|namespace | 命名空间：最大12个字符,支持小写字母、数字和下划线的组合   |string  |    |
|collectionName | 集合名称：最大24个字符,支持小写字母、数字和下划线的组合   |string  |    |
|sampleId | 样本ID：最大32个字符,支持小写字母、数字和下划线的组合   |string  |    |
|sampleData | 扩展字段   |array  | FieldKeyValue   |
|faces | 人脸数据   |array  | SimpleFaceVo   |

**FieldKeyValue**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|key | 字段名，与创建集合时给定的字段名一致   |string  |    |
|value | 字段值，与创建集合时给定的类型一致，若为字符串，最大为512个字符   |object  |    |

**SimpleFaceVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|faceId | 人脸ID   |string  |    |
|faceData | 人脸扩展的额外数据   |array  | FieldKeyValue   |
|faceScore | 人脸分数   |number(float)  |    |

**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«SampleDataRepVo»|
## 5、根据查询信息查看样本列表


**接口描述**:


**接口地址**:`/visual/sample/list`


**请求方式**：`GET`


**consumes**:``


**produces**:`["*/*"]`



**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|collectionName| 集合名称  | query | true |string  |    |
|limit| 样本数目：默认10  | query | true |integer  |    |
|namespace| 命名空间  | query | true |string  |    |
|offset| 起始记录:默认0  | query | true |integer  |    |
|order| 排列方式：默认asc，包括asc（升序）和desc（降序）  | query | true |string  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": [
		{
			"namespace": "",
			"collectionName": "",
			"sampleId": "",
			"sampleData": [
				{
					"key": "",
					"value": {}
				}
			],
			"faces": [
				{
					"faceId": "",
					"faceData": [
						{
							"key": "",
							"value": {}
						}
					],
					"faceScore": 0
				}
			]
		}
	]
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |array  | SampleDataRepVo   |



**schema属性说明**




**SampleDataRepVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|namespace | 命名空间：最大12个字符,支持小写字母、数字和下划线的组合   |string  |    |
|collectionName | 集合名称：最大24个字符,支持小写字母、数字和下划线的组合   |string  |    |
|sampleId | 样本ID：最大32个字符,支持小写字母、数字和下划线的组合   |string  |    |
|sampleData | 扩展字段   |array  | FieldKeyValue   |
|faces | 人脸数据   |array  | SimpleFaceVo   |

**FieldKeyValue**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|key | 字段名，与创建集合时给定的字段名一致   |string  |    |
|value | 字段值，与创建集合时给定的类型一致，若为字符串，最大为512个字符   |object  |    |

**SimpleFaceVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|faceId | 人脸ID   |string  |    |
|faceData | 人脸扩展的额外数据   |array  | FieldKeyValue   |
|faceScore | 人脸分数   |number(float)  |    |

**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«List«SampleDataRepVo»»|
## 2、更新一个样本


**接口描述**:


**接口地址**:`/visual/sample/update`


**请求方式**：`POST`


**consumes**:`["application/json"]`


**produces**:`["*/*"]`


**请求示例**：
```json
{
	"namespace": "",
	"collectionName": "",
	"sampleId": "",
	"sampleData": [
		{
			"key": "",
			"value": {}
		}
	]
}
```


**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|sample| 样本信息  | body | true |SampleDataReqVo  | SampleDataReqVo   |

**schema属性说明**



**SampleDataReqVo**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|namespace| 命名空间：最大12个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|collectionName| 集合名称：最大24个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|sampleId| 样本ID：最大32个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|sampleData| 扩展字段  | body | false |array  | FieldKeyValue   |

**FieldKeyValue**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|key| 字段名，与创建集合时给定的字段名一致  | body | true |string  |    |
|value| 字段值，与创建集合时给定的类型一致，若为字符串，最大为512个字符  | body | false |object  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": true
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |boolean  |    |





**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«boolean»|
# 03、人脸数据管理

## 1、创建一个人脸数据


**接口描述**:


**接口地址**:`/visual/face/create`


**请求方式**：`POST`


**consumes**:`["application/json"]`


**produces**:`["*/*"]`


**请求示例**：
```json
{
	"namespace": "",
	"collectionName": "",
	"sampleId": "",
	"imageBase64": "",
	"faceScoreThreshold": 0,
	"minConfidenceThresholdWithThisSample": 0,
	"maxConfidenceThresholdWithOtherSample": 0,
	"faceData": [
		{
			"key": "",
			"value": {}
		}
	]
}
```


**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|face| face  | body | true |FaceDataReqVo  | FaceDataReqVo   |

**schema属性说明**



**FaceDataReqVo**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|namespace| 命名空间：最大12个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|collectionName| 集合名称：最大24个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|sampleId| 样本ID：最大32个字符,支持小写字母、数字和下划线的组合  | body | true |string  |    |
|imageBase64| 图像Base64编码值  | body | true |string  |    |
|faceScoreThreshold| 人脸质量分数阈值,范围：[0,100]：默认0。当设置为0时，会默认使用当前模型的默认值，该方法为推荐使用方式  | body | false |number(float)  |    |
|minConfidenceThresholdWithThisSample| 当前样本的人脸相似度的最小阈值,范围：[0,100]：默认0。当设置为0时，表示不做类间相似度判断逻辑,开启后对效率有较大影响  | body | false |number(float)  |    |
|maxConfidenceThresholdWithOtherSample| 当前样本与其他样本的人脸相似度的最大阈值,范围：[0,100]：默认0。当设置为0时，表示不做类间相似度判断逻辑,开启后对效率有较大影响  | body | false |number(float)  |    |
|faceData| 扩展字段  | body | false |array  | FieldKeyValue   |

**FieldKeyValue**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|key| 字段名，与创建集合时给定的字段名一致  | body | true |string  |    |
|value| 字段值，与创建集合时给定的类型一致，若为字符串，最大为512个字符  | body | false |object  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": {
		"namespace": "",
		"collectionName": "",
		"sampleId": "",
		"faceId": "",
		"faceScore": 0,
		"faceData": [
			{
				"key": "",
				"value": {}
			}
		]
	}
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |FaceDataRepVo  | FaceDataRepVo   |



**schema属性说明**




**FaceDataRepVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|namespace | 命名空间：最大12个字符,支持小写字母、数字和下划线的组合   |string  |    |
|collectionName | 集合名称：最大24个字符,支持小写字母、数字和下划线的组合   |string  |    |
|sampleId | 样本ID：最大32个字符,支持小写字母、数字和下划线的组合   |string  |    |
|faceId | 人脸ID   |string  |    |
|faceScore | 人脸人数质量   |number(float)  |    |
|faceData | 扩展字段   |array  | FieldKeyValue   |

**FieldKeyValue**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|key | 字段名，与创建集合时给定的字段名一致   |string  |    |
|value | 字段值，与创建集合时给定的类型一致，若为字符串，最大为512个字符   |object  |    |

**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«FaceDataRepVo»|
## 2、根据条件删除人脸数据


**接口描述**:


**接口地址**:`/visual/face/delete`


**请求方式**：`GET`


**consumes**:``


**produces**:`["*/*"]`



**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|collectionName| 集合名称  | query | true |string  |    |
|faceId| 人脸ID  | query | true |string  |    |
|namespace| 命名空间  | query | true |string  |    |
|sampleId| 样本ID  | query | true |string  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": true
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |boolean  |    |





**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«boolean»|
# 04、人脸搜索服务

## 1、人脸搜索1:N


**接口描述**:


**接口地址**:`/visual/search/do`


**请求方式**：`POST`


**consumes**:`["application/json"]`


**produces**:`["*/*"]`


**请求示例**：
```json
{
	"namespace": "",
	"collectionName": "",
	"imageBase64": "",
	"faceScoreThreshold": 0,
	"confidenceThreshold": 0,
	"limit": 0,
	"maxFaceNum": 0
}
```


**请求参数**：

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|search| 人脸搜索参数  | body | true |FaceSearchReqVo  | FaceSearchReqVo   |

**schema属性说明**



**FaceSearchReqVo**

| 参数名称         | 参数说明     |     in |  是否必须      |  数据类型  |  schema  |
| ------------ | -------------------------------- |-----------|--------|----|--- |
|namespace| 命名空间  | body | true |string  |    |
|collectionName| 集合名称  | body | true |string  |    |
|imageBase64| 图像Base64编码值  | body | true |string  |    |
|faceScoreThreshold| 人脸质量分数阈值,范围：[0,100]：默认0。当设置为0时，会默认使用当前模型的默认值，该方法为推荐使用方式  | body | false |number(float)  |    |
|confidenceThreshold| 人脸匹配分数阈值，范围：[-100,100]：默认0  | body | false |number(float)  |    |
|limit| 最大搜索条数：默认5  | body | false |integer(int32)  |    |
|maxFaceNum| 对输入图像中多少个人脸进行检索比对：默认5  | body | false |integer(int32)  |    |

**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": [
		{
			"location": {
				"x": 0,
				"y": 0,
				"w": 0,
				"h": 0
			},
			"faceScore": 0,
			"match": [
				{
					"sampleId": "",
					"faceId": "",
					"faceScore": 0,
					"distance": 0,
					"confidence": 0,
					"sampleData": [
						{
							"key": "",
							"value": {}
						}
					],
					"faceData": [
						{
							"key": "",
							"value": {}
						}
					]
				}
			]
		}
	]
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |array  | FaceSearchRepVo   |



**schema属性说明**




**FaceSearchRepVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|location | 人脸位置信息   |FaceLocation  | FaceLocation   |
|faceScore | 人脸分数:[0,100]   |number(float)  |    |
|match | 匹配的人脸列表   |array  | SampleFaceVo   |

**FaceLocation**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|x | 左上角x坐标   |integer(int32)  |    |
|y | 左上角y坐标   |integer(int32)  |    |
|w | 人脸宽度   |integer(int32)  |    |
|h | 人脸高度   |integer(int32)  |    |

**SampleFaceVo**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|sampleId | 样本ID   |string  |    |
|faceId | 人脸ID   |string  |    |
|faceScore | 人脸分数:[0,100]   |number(float)  |    |
|distance | 向量距离:>=0   |number(float)  |    |
|confidence | 转换后的置信度:[-100,100]，值越大，相似度越高。   |number(float)  |    |
|sampleData | 样本扩展的额外数据   |array  | FieldKeyValue   |
|faceData | 人脸扩展的额外数据   |array  | FieldKeyValue   |

**FieldKeyValue**

| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | ------------------|--------|----------- |
|key | 字段名，与创建集合时给定的字段名一致   |string  |    |
|value | 字段值，与创建集合时给定的类型一致，若为字符串，最大为512个字符   |object  |    |

**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«List«FaceSearchRepVo»»|
# 05、公共服务-健康检测

## 公共-服务健康检测


**接口描述**:


**接口地址**:`/common/health/check`


**请求方式**：`GET`


**consumes**:``


**produces**:`["*/*"]`



**请求参数**：
暂无



**响应示例**:

```json
{
	"code": 0,
	"message": "",
	"data": ""
}
```

**响应参数**:


| 参数名称         | 参数说明                             |    类型 |  schema |
| ------------ | -------------------|-------|----------- |
|code| 返回代码  |integer(int32)  | integer(int32)   |
|message| 返回信息  |string  |    |
|data| 数据信息  |string  |    |





**响应状态**:


| 状态码         | 说明                            |    schema                         |
| ------------ | -------------------------------- |---------------------- |
| 200 | OK  |ResponseInfo«string»|
