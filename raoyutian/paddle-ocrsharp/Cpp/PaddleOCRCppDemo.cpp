#include <iostream>
#include <Windows.h>
#include "include/PaddleOCR.h"
#include "include/OCRResult.h"
#include <tchar.h>
#include "string"
#pragma comment (lib,"PaddleOCR.lib")
using namespace std;
int main()
{
	LpOCRResult lpocrreult;
	modeldata md;
	OCRParameter parameter;
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
	imagefile += "\\test.png";
	md.cls_infer = const_cast<char*>(cls_infer.c_str());
	md.rec_infer = const_cast<char*>(rec_infer.c_str());
	md.det_infer = const_cast<char*>(det_infer.c_str());
	md.keys = const_cast<char*>(ocrkeys.c_str());
	md.imagefile = const_cast<char*>(imagefile.c_str());
	int  cout =Detect(md.det_infer, md.cls_infer, md.rec_infer, md.keys, md.imagefile, parameter, &lpocrreult);
	for (size_t i = 0; i < cout; i++)
	{
		wstring ss =(WCHAR*)(lpocrreult->pOCRText[i].ptext);
		std::wcout <<ss;
	}
	FreeDetectMem(lpocrreult);
}

