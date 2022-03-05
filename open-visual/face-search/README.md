## 人脸搜索M:N

* 本项目是阿里云视觉智能开放平台的人脸1：N的开源替代，项目中使用的模型均为开源模型，项目支持milvus和proxima向量存储库，并具有较高的自定义能力。

* 项目使用纯Java开发，免去使用Python带来的服务不稳定性。

* 1：N是通过采集某人的人像后，从海量的人像数据底库中找到与当前使用者人脸数据相符合的图像，通过数据库的比对找出"你是谁"，常见的办公楼宇的人脸考勤门禁、社区门禁、工地考勤、会签等等场景。

* M：N 是通过计算机对场景内所有人进行面部识别并与人像数据库进行比对的过程。M:N作为一种动态人脸比对，其使用率非常高，能充分应用于多种场景，例如公共安防，迎宾，机器人应用等。

* 欢迎大家贡献代码，如果你觉得项目还不错，请给个star。

### 项目简介

* 整体架构图

 ![输入图片说明](scripts/images/%E4%BA%BA%E8%84%B8%E6%90%9C%E7%B4%A2%E6%B5%81%E7%A8%8B%E5%9B%BE.jpg)

* 项目使用组件

&ensp; &ensp; 1、spring boot

&ensp; &ensp; 2、[onnx](https://github.com/onnx/onnx)

&ensp; &ensp; 3、[milvus](https://github.com/milvus-io/milvus/)

&ensp; &ensp; 4、[proxima](https://github.com/alibaba/proximabilin)

* 深度学习模型

&ensp; &ensp; 1、[insightface](https://github.com/deepinsight/insightface)

&ensp; &ensp; 2、[PCN](https://github.com/Rock-100/FaceKit/tree/master/PCN)

### 项目文档

* 在线文档：[文档-1.0.0](https://gitee.com/open-visual/face-search/blob/master/scripts/docs/doc-1.0.0.md)

* swagger文档：启动项目且开启swagger，访问：host:port/doc.html, 如 http://127.0.0.1:8080/doc.html

### 搜索客户端

* Java依赖,未发布到中央仓库，需要自行编译发布到私有仓库
```
<dependency>
    <groupId>com.visual.face.search</groupId>
    <artifactId>face-search-client</artifactId>
    <version>1.0.0</version>
</dependency>
```
* 其他语言依赖

&ensp; &ensp;使用restful接口：[文档-1.0.0](https://gitee.com/open-visual/face-search/blob/master/scripts/docs/doc-1.0.0.md)


### 项目部署

* docker部署，脚本目录：face-search/scripts
```
1、使用milvus作为向量搜索引擎
  docker-compose -f docker-compose-milvus.yml --compatibility up -d

2、使用proxima作为向量搜索引擎
   docker-compose -f docker-compose-proxima.yml --compatibility up -d
```

* 项目编译
```
1、克隆项目
  git clone https://gitee.com/open-visual/face-search.git
2、项目打包
   cd face-search && sh scripts/docker_build.sh
```

* 部署参数

| 参数        | 描述   |  默认值  | 可选值|
| --------   | -----:  | :----:  |--------|
| VISUAL_SWAGGER_ENABLE                      | 是否开启swagger   	|   true      |                                    |
| SPRING_DATASOURCE_URL                      | 数据库地址   		|             |                                    |
| SPRING_DATASOURCE_USERNAME                 | 数据库用户名    		|             |                                    |
| SPRING_DATASOURCE_PASSWORD                 | 数据库密码    		|             |                                    |
| VISUAL_ENGINE_SELECTED                     | 向量存储引擎    		|  proxima    |proxima,milvus                      |
| VISUAL_ENGINE_PROXIMA_HOST                 | PROXIMA地址   		|             |VISUAL_ENGINE_SELECTED=proxima时生效 |
| VISUAL_ENGINE_PROXIMA_PORT                 | PROXIMA端口    		|  16000      |VISUAL_ENGINE_SELECTED=proxima时生效 |
| VISUAL_ENGINE_MILVUS_HOST                  | MILVUS地址    		|             |VISUAL_ENGINE_SELECTED=milvus时生效  |
| VISUAL_ENGINE_MILVUS_PORT                  | MILVUS端口    		|  19530      |VISUAL_ENGINE_SELECTED=milvus时生效  |
| VISUAL_MODEL_FACEDETECTION_NAME            | 人脸检测模型名称    	|  PcnNetworkFaceDetection    |PcnNetworkFaceDetection，InsightScrfdFaceDetection                    |
| VISUAL_MODEL_FACEDETECTION_BACKUP_NAME     | 备用人脸检测模型名称         | InsightScrfdFaceDetection  |PcnNetworkFaceDetection，InsightScrfdFaceDetection                    |
| VISUAL_MODEL_FACEKEYPOINT_NAME             | 人脸关键点模型名称          | InsightCoordFaceKeyPoint  |InsightCoordFaceKeyPoint                    |
| VISUAL_MODEL_FACEALIGNMENT_NAME            | 人脸对齐模型名称            | Simple106pFaceAlignment  |Simple106pFaceAlignment，Simple005pFaceAlignment                    |
| VISUAL_MODEL_FACERECOGNITION_NAME          | 人脸特征提取模型名称         | InsightArcFaceRecognition  |InsightArcFaceRecognition                    |

### 性能优化

* 项目中为了提高人脸的检出率，使用了主要和次要的人脸检测模型，目前实现了两种人脸检测模型insightface和PCN，在docker的服务中，默认主服务为PCN，备用服务为insightface。insightface的效率高，但针对于旋转了大角度的人脸检出率不高，而pcn则可以识别大角度旋转的图片，但效率低一些。若图像均为正脸的图像，建议使用insightface为主模型，pcn为备用模型，如何切换，请查看部署参数。

* 在测试过程中，针对milvus和proxima，发现proxima的速度比milvus稍快，但稳定性没有milvus好，线上服务使用时，还是建议使用milvus作为向量检索引擎。

### 项目演示

* 测试用例：face-search-test[测试用例-FaceSearchExample](https://gitee.com/open-visual/face-search/blob/master/face-search-test/src/main/java/com/visual/face/search/valid/exps/FaceSearchExample.java)

* ![输入图片说明](scripts/images/validate.jpg)

### 交流群

* 钉钉交流群

    关注微信公众号回复：钉钉群

* 微信交流群

    关注微信公众号回复：微信群

* 微信公众号：关注一下，是对我最大的支持

![微信公众号](scripts/images/%E5%85%AC%E4%BC%97%E5%8F%B7-%E5%BE%AE%E4%BF%A1.jpg)