#include <iostream>
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
	/// <summary>
	/// PaddleOCR检测
	/// </summary>
	/// <param name="det_infer"></param>
	/// <param name="imagefile"></param>
	/// <param name="parameter"></param>
	/// <returns></returns>
	__declspec(dllimport) void DetectImage(char* modelPath_det_infer, char* imagefile, OCRParameter parameter);

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

