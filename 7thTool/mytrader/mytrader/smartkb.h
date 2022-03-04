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

#define SMARTKB_KEY_ALL "ȫ��"
#define SMARTKB_KEY_MAIN "����"
#define SMARTKB_KEY_SUBSCRIBE "��ѡ"
#define SMARTKB_KEY_ALL_SUBSCRIBE "����"
#define SMARTKB_KEY_ALL_W L"ȫ��"
#define SMARTKB_KEY_MAIN_W L"����"
#define SMARTKB_KEY_SUBSCRIBE_W L"��ѡ"
#define SMARTKB_KEY_ALL_SUBSCRIBE_W L"����"

#define SMARTKB_ITEM_TYPE_CODE 0

//
//�ַ���ƥ��
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
	std::wstring Code;		//Ψһ����,��д����
	std::wstring Name;		//���ƣ�һ��Ϊ������
	std::wstring PinYin;	//ƴ���������ֵΪ�գ���ʹ��ƴ����
	int Type = 0;			//�Զ�����������
	void* Data = nullptr;	//�Զ�������
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
	//�����������ֺͱ�־��0��ʶ��ĸ��1��ʶƴ��
	void SetInputText(const std::wstring& strText, int flag);
	void GetInputText(std::wstring& strText, int& flag);
	//֪ͨ��������
	virtual void NotifyResult();
	//��ʾ���
	virtual void ShowResult();
	//���������
	void AddInputResult(size_t pos, int result);
	//�ϲ�������
	void MergeInputResult();
	//ȡ���뼯
	void GetInputs(std::vector<SmartKBItem>& results);
	//��������
	void ClearInputResult();
	//ȡ��������
	void GetInputResults(std::vector<SmartKBItem>& results);
	//��������
	void Search();
	//ƥ�亯��
	const wchar_t* StrKey(const wchar_t* name, const wchar_t* key);
	const wchar_t* StrName(const wchar_t* name, const wchar_t* key);
	//����ƥ��Key
	void SearchKey(const std::wstring& strText);
public:
	//����ָ�����������
	void Add(const std::string& strType, SmartKBItem*pData, int nCount);
	//���ָ�����������
	void Clear(const std::string& strType);
	//ѡ��ָ����
	bool Select(const std::string& strType);
	//�������������
	void SetMaxResult(size_t nMaxResult);
	//����Key
	enum {
		INPUT_FLAG_DEFAULT = 0,
		INPUT_FLAG_CODE = 1,
		INPUT_FLAG_PINYIN = 2,
	};
	bool Input(const wchar_t* lpszKey, int flag = 0);
};
