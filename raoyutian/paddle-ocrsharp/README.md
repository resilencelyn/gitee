# PaddleOCRSharp       [版本更新记录](https://gitee.com/raoyutian/paddle-ocrsharp/blob/master/README_update.md)

# 介绍

本项目是一个基于PaddleOCR的C++代码修改并封装的.NET的工具类库。包含文本识别、文本检测、基于文本检测结果的统计分析的表格识别功能，同时针对小图识别不准的情况下，做了优化，提高识别准确率。包含总模型仅8.6M的超轻量级中文OCR，单模型支持中英文数字组合识别、竖排文本识别、长文本识别。同时支持多种文本检测。
项目封装极其简化，实际调用仅几行代码，极大的方便了中下游开发者的使用和降低了PaddleOCR的使用入门级别，同时提供不同的.NET框架使用，方便各个行业应用开发与部署。Nuget包即装即用，可以离线部署，不需要网络就可以识别的高精度中英文OCR。  

本项目中PaddleOCR.dll文件是基于开源项目PaddleOCR的C++代码修改而成的C++动态库，基于opencv的x64编译而成的。
支持以下NET框架：

```
net35;net40;net461;net48;netstandard2.0;netcoreapp3.1;net5.0;net6.0;
```


[百度飞桨PaddleOCR项目地址（码云）](https://gitee.com/paddlepaddle/PaddleOCR)

[百度飞桨PaddleOCR项目地址（GitHub）](https://github.com/paddlepaddle/PaddleOCR)

模型库支持轻量版（本项目）、服务器版模型库（更准确），可以自行更改模型库适用实际需求。

[百度飞桨PaddleOCR模型下载地址](https://gitee.com/paddlepaddle/PaddleOCR/blob/release/2.4/doc/doc_ch/models_list.md)

如果需要修改成服务器版模型库，参考代码如下：(假设服务器版模型库在运行目录的文件夹inferenceserver下)
```

           OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "*.*|*.bmp;*.jpg;*.jpeg;*.tiff;*.tiff;*.png";
            if (ofd.ShowDialog() != DialogResult.OK) return;
            var imagebyte = File.ReadAllBytes(ofd.FileName);
            Bitmap bitmap = new Bitmap(new MemoryStream(imagebyte));

             //OCRModelConfig config = null;
             // 使用服务器模型
             OCRModelConfig config = new OCRModelConfig();
             string root =Environment.CurrentDirectory;
             config = new OCRModelConfig();
             string modelPathroot = root + @"\inferenceserver";
             config.det_infer = modelPathroot + @"\ch_ppocr_server_v2.0_det_infer";
             config.cls_infer = modelPathroot + @"\ch_ppocr_mobile_v2.0_cls_infer";
             config.rec_infer = modelPathroot + @"\ch_ppocr_server_v2.0_rec_infer";
             config.keys = modelPathroot + @"\ppocr_keys.txt"; 


            OCRParameter oCRParameter = new  OCRParameter ();
            OCRResult ocrResult = new OCRResult();
            using (PaddleOCREngine engine = new PaddleOCREngine(config, oCRParameter))
            {
                ocrResult = engine.DetectText(bitmap );
            }
            if (ocrResult != null)
            {
                MessageBox.Show(ocrResult.Text,"识别结果");
            }


```

 [百度飞桨windows下C++预测库下载地址](https://paddleinference.paddlepaddle.org.cn/user_guides/download_lib.html#windows)


 **关于源码编译，建议采用vs2022版本编译，如果遇到无法编译，请切换成release后再切换回debug即可。** 

如果因框架编译问题无法编译，请修改PaddleOCRSharp\PaddleOCRSharp.csproj文件，删除当前电脑没有的框架
```
 <TargetFrameworks>net35;net40;net461;net48;netstandard2.0;netcoreapp3.1;net5.0;net6.0;</TargetFrameworks>
```

全部调用参数 [官方PaddleOCR参数](https://gitee.com/paddlepaddle/PaddleOCR/tree/release/2.4/deploy/cpp_infer)

```
 #region 通用参数
        /// <summary>
        /// 是否使用GPU，默认关闭
        /// </summary>
        public byte use_gpu { get; set; } = 0;
        /// <summary>
        /// GPU id，使用GPU时有效
        /// </summary>
        public int gpu_id { get; set; } = 0;
        /// <summary>
        /// 申请的GPU内存，使用GPU时有效
        /// </summary>
        public int gpu_mem { get; set; } = 4000;
        /// <summary>
        /// 使用线程数，默认2
        /// </summary>
        public int numThread { get; set; } = 2;
        /// <summary>
        /// 启用mkldnn加速，默认开启
        /// </summary>
        public byte Enable_mkldnn { get; set; } = 1;

        #endregion


        #region 检测模型相关
        /// <summary>
        /// 补白边，默认50，暂时没有用
        /// </summary>
        public int Padding { get; set; } = 50;
        /// <summary>
        /// 输入图像长宽大于960时，等比例缩放图像，使得图像最长边为960
        /// </summary>
        public int MaxSideLen { get; set; } = 960;
        /// <summary>
        /// DB后处理过滤box的阈值，如果检测存在漏框情况，可酌情减小 
        /// </summary>
        public float BoxScoreThresh { get; set; } = 0.5f;
        /// <summary>
        /// 用于过滤DB预测的二值化图像，设置为0.-0.3对结果影响不明显
        /// </summary>
        public float BoxThresh { get; set; } = 0.3f;
        /// <summary>
        /// 表示文本框的紧致程度，越小则文本框更靠近文本  
        /// </summary>
        public float UnClipRatio { get; set; } = 1.6f;
        /// <summary>
        /// DoAngle 默认1启用
        /// </summary>
        public byte DoAngle { get; set; } = 1;
        /// <summary>
        /// MostAngle 默认1启用
        /// </summary>
        public byte MostAngle { get; set; } = 1;

        /// <summary>
        /// 是否使用多边形框计算bbox score，false表示使用矩形框计算。矩形框计算速度更快，多边形框对弯曲文本区域计算更准确。
        /// </summary>
        public byte use_polygon_score { get; set; } = 0;
        /// <summary>
        /// 是否对结果进行可视化，为1时，会在当前文件夹下保存文件名为ocr_vis.png的预测结果。
        /// </summary>
        public byte visualize { get; set; } = 0;
        #endregion

        #region 方向分类器相关

        /// <summary>
        /// 启用方向选择器，默认关闭
        /// </summary>
        public byte use_angle_cls { get; set; } = 0;
        /// <summary>
        /// 方向分类器的得分阈值
        /// </summary>
        public float cls_thresh { get; set; } = 0.9f;
        #endregion

```

# 文件夹结构

 
```

Cpp                          //PaddleOCR.dll的头文件和库文件，方便C++调用PaddleOCR.dll
PaddleOCRLib                 //OCR运行需要的文件
|--inference                 //OCR的模型库文件夹
|--libiomp5md.dll            //第三方引用库
|--mkldnn.dll                //第三方引用库
|--mklml.dll                 //第三方引用库
|--opencv_world411.dll       //第三方引用库
|--paddle_inference.dll      //飞桨库
|--PaddleOCR.dll             //基于开源项目PaddleOCR修改的C++动态库
PaddleOCRSharp               //.NET封装库项目
PaddleOCRCppDemo             //C++调用示例项目
PaddleOCRSharpDemo           //.NET调用示例项目

```
[C++示例代码](https://gitee.com/raoyutian/paddle-ocrsharp/blob/master/PaddleOCRCppDemo/PaddleOCRCppDemo.cpp)

```
﻿#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include "string"
#include <include/Parameter.h>
#include <string.h>
using namespace std;
#pragma comment (lib,"PaddleOCR.lib")
extern "C" {
	/// <summary>
	/// PaddleOCREngine引擎初始化
	/// </summary>
	/// <param name="det_infer"></param>
	/// <param name="cls_infer"></param>
	/// <param name="rec_infer"></param>
	/// <param name="keys"></param>
	/// <param name="parameter"></param>
	/// <returns></returns>
	__declspec(dllimport) int* Initialize(char* det_infer, char* cls_infer, char* rec_infer, char* keys, OCRParameter  parameter);
	/// <summary>
	/// 文本检测
	/// </summary>
	/// <param name="engine"></param>
	/// <param name="imagefile"></param>
	/// <param name="pOCRResult">返回结果</param>
	/// <returns></returns>
	__declspec(dllimport) int  Detect(int* engine, char* imagefile, LpOCRResult* pOCRResult);
	/// <summary>
	/// 释放引擎对象
	/// </summary>
	/// <param name="engine"></param>
	__declspec(dllimport) void FreeEngine(int* engine);
	/// <summary>
	/// 释放文本识别结果对象
	/// </summary>
	/// <param name="pOCRResult"></param>
	__declspec(dllimport) void FreeDetectResult(LpOCRResult pOCRResult);
};

std::wstring string2wstring(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

int main()
{
	LpOCRResult lpocrreult;
	OCRParameter parameter;
	/*parameter.enable_mkldnn = false;*/
	char path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);
 
	string cls_infer(path);
	cls_infer += "\\inference\\ch_ppocr_mobile_v2.0_cls_infer";
	string rec_infer(path);
	rec_infer += "\\inference\\ch_PP-OCRv2_rec_infer";
	string det_infer(path);
	det_infer += "\\inference\\ch_PP-OCRv2_det_infer";
	string ocrkeys(path);
	ocrkeys += "\\inference\\ppocr_keys.txt";
	string imagefile(path);
	imagefile += "\\test.jpg";
	
	int*  pEngine = Initialize(const_cast<char*>(det_infer.c_str()),
							 const_cast<char*>(cls_infer.c_str()), 
						     const_cast<char*>(rec_infer.c_str()),
							 const_cast<char*>(ocrkeys.c_str()),
		                     parameter);
	
	int  cout = Detect(pEngine, const_cast<char*>(imagefile.c_str()), &lpocrreult);
	std::wcout.imbue(std::locale("chs"));
	for (size_t i = 0; i < cout; i++)
	{
		wstring ss = (WCHAR*)(lpocrreult->pOCRText[i].ptext);
		std::wcout << ss; 
	}
	FreeDetectResult(lpocrreult);
	FreeEngine(pEngine);
	std::cin.get();
}

```



#.net使用示例
```
           OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "*.*|*.bmp;*.jpg;*.jpeg;*.tiff;*.tiff;*.png";
            if (ofd.ShowDialog() != DialogResult.OK) return;
            var imagebyte = File.ReadAllBytes(ofd.FileName);
            Bitmap bitmap = new Bitmap(new MemoryStream(imagebyte));

            OCRModelConfig config = null;
            OCRParameter oCRParameter = new  OCRParameter ();
           
           //oCRParameter.use_gpu=1;当使用GPU版本的预测库时，该参数打开才有效果


            OCRResult ocrResult = new OCRResult();
            using (PaddleOCREngine engine = new PaddleOCREngine(config, oCRParameter))
            {
                ocrResult = engine.DetectText(bitmap );
            }
            if (ocrResult != null)
            {
                MessageBox.Show(ocrResult.Text,"识别结果");
            }
```
# 喜欢的给个星，谢谢
# QQ交流群：318860399
 

