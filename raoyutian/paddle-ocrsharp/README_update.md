### v1.1.0

1.移除命令行日志输出；

2.修复PaddleOCRSharp.dll文本识别不到报错的问题

### v1.2.0

1.调整模型加载模式，可以加载一次模型，多次识别；

2.增加参数类OCRParameter增加Enable_mkldnn参数，默认开启；

3.优化模型库路径可自定义修改位置；

4.增加C++demo示例项目

 **PaddleOCRHelper静态类的检测方法，将在下一版本移除，请使用PaddleOCREngine类代替。** 

### v1.2.2

1.增加net35;net48,net6框架支持

2.补齐参数类OCRParameter参数（对标官方PaddleOCR参数）[官方PaddleOCR参数](https://gitee.com/paddlepaddle/PaddleOCR/tree/release/2.4/deploy/cpp_infer)
 
3.增加纯检测接口DetectImage

4.增加检测得分结果输出

5.Demo增加显示识别结果的文字区域
 

