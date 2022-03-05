#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <limits>
using std::string;
using std::wstring;
using std::vector;

#include <zqdb.h>

#define SMARTKB_KEY_ALL "全部"
#define SMARTKB_KEY_MAIN "主力"
#define SMARTKB_KEY_SUBSCRIBE "自选"
#define SMARTKB_KEY_ALL_SUBSCRIBE "订阅"
#define SMARTKB_KEY_ALL_W L"全部"
#define SMARTKB_KEY_MAIN_W L"主力"
#define SMARTKB_KEY_SUBSCRIBE_W L"自选"
#define SMARTKB_KEY_ALL_SUBSCRIBE_W L"订阅"

#define SMARTKB_ITEM_TYPE_CODE 0

//
//字符串匹配
//
class MatchCH
{
protected:
	typedef struct
	{
		char filedlen;
		char filednum;
		unsigned short usStart;
		unsigned short usEnd;
	}tagHead;
	tagHead head_ = { 0 };
	std::vector<char>  datas_;
	void InnerInit(const char* file);
public:
	MatchCH();
	~MatchCH();

	static void Init(const char* file);

	bool MatchPinYin(wchar_t name, wchar_t key);
	bool MatchPinYin(wchar_t* name, wchar_t* key);
};

// ----------------------------------------------------------------------------
// SmartKB
// ----------------------------------------------------------------------------

struct SmartKBItem
{
	std::wstring Code;		//唯一代码,大写代码
	std::wstring Name;		//名称，一般为中文名
	std::wstring PinYin;	//拼音，如果该值为空，则使用拼音库
	int Type = 0;			//自定义数据类型
	void* Data = nullptr;	//自定义数据
};

class SmartKB
{
protected:
	bool stop_flag_ = false;
	std::shared_ptr<std::thread> thread_;
	std::map<std::string, std::shared_ptr<std::vector<SmartKBItem>>> all_items_;
	std::shared_ptr<std::vector<SmartKBItem>> input_items_;
	std::shared_ptr<std::vector<SmartKBItem>> items_;
	size_t max_results_ = (size_t)-1;//std::numeric_limits<size_t>::max();
	bool input_changed_ = false;
	std::wstring input_text_;
	int input_flag_ = 0;
	typedef struct tagInputResult
	{
		size_t pos;
		int result;
		tagInputResult() :pos(0), result(-1) {}
		tagInputResult(size_t p, int r) :pos(p), result(r) {}
	}INPUTRESULT, *PINPUTRESULT;
	struct INPUTRESULTPosLess
	{
		bool operator()(const INPUTRESULT& x, const INPUTRESULT& y) const
		{
			return x.pos < y.pos;
		}
	};
	struct INPUTRESULTPosEqual
	{
		bool operator()(const INPUTRESULT& x, const INPUTRESULT& y) const
		{
			return x.pos == y.pos;
		}
	};
	struct INPUTRESULTResultLess
	{
		bool operator()(const INPUTRESULT& x, const INPUTRESULT& y) const
		{
			return x.result < y.result;
		}
	};
	std::vector<INPUTRESULT> input_results_;
	size_t best_results_ = 0;
	std::mutex input_mutex_;
public:
	SmartKB();
	virtual ~SmartKB();

protected:
	//设置输入文字和标志，0标识字母，1标识拼音
	void SetInputText(const std::wstring& strText, int flag);
	void GetInputText(std::wstring& strText, int& flag);
	//通知搜索结束
	virtual void NotifyResult();
	//显示结果
	virtual void ShowResult();
	//添加输入结果
	void AddInputResult(size_t pos, int result);
	//合并输入结果
	void MergeInputResult();
	//取输入集
	void GetInputs(std::vector<SmartKBItem>& results);
	//清除结果集
	void ClearInputResult();
	//取输入结果集
	void GetInputResults(std::vector<SmartKBItem>& results);
	//搜索函数
	void Search();
	//匹配函数
	const wchar_t* StrKey(const wchar_t* name, const wchar_t* key);
	const wchar_t* StrName(const wchar_t* name, const wchar_t* key);
	//搜索匹配Key
	void SearchKey(const std::wstring& strText);
public:
	//增加指定组基础数据
	void Add(const std::string& strType, SmartKBItem*pData, int nCount);
	//清除指定组基础数据
	void Clear(const std::string& strType);
	//选择指定组
	bool Select(const std::string& strType);
	//设置最大结果数量
	void SetMaxResult(size_t nMaxResult);
	//输入Key
	enum {
		INPUT_FLAG_DEFAULT = 0,
		INPUT_FLAG_CODE = 1,
		INPUT_FLAG_PINYIN = 2,
	};
	bool Input(const wchar_t* lpszKey, int flag = 0);
};
