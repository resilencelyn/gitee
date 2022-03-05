
#include "smartkb.h"
#include <algorithm>
#include <assert.h>
#include <XUtil\XXml.hpp>
#include <zqstr.hpp>

static MatchCH g_match_ch;

MatchCH::MatchCH()
{
	
}

MatchCH::~MatchCH()
{
	
}

void MatchCH::InnerInit(const char* file)
{
	try {
		//auto path_run_dir = boost::dll::this_line_location().remove_leaf();
		//读取拼音映射表
		{
			//boost::filesystem::path path_pinyin_file = path_run_dir;
			//path_pinyin_file.append("pinyin.py");
			std::ifstream infile(file, std::ifstream::in | std::ifstream::binary);
			if (infile.is_open()) {
				infile.seekg(0, infile.end);
				size_t totalBytes = infile.tellg();
				infile.seekg(0, infile.beg);
				//auto sz_head = sizeof(tagHead);
				if (totalBytes > sizeof(tagHead))
				{
					infile.read((char *)&head_, sizeof(tagHead));
					size_t totalDatas = totalBytes - sizeof(tagHead);
					size_t sizeDatas = (head_.usEnd - head_.usStart) * head_.filednum;
					if (sizeDatas <= totalDatas)
					{
						datas_.resize(sizeDatas);
						infile.read(datas_.data(), datas_.size());
					}
				}
			}
		}
		//转换拼音映射表
		/*{
			boost::filesystem::path path_pinyin_txtfile = path_run_dir;
			path_pinyin_txtfile.append("pinyin.txt");
			std::ofstream outfile(path_pinyin_txtfile.string(), std::ofstream::out | std::ofstream::trunc);
			for (size_t i = 0, j = (head_.usEnd - head_.usStart); i < j; i++)
			{
				wchar_t wstr[2] = { (wchar_t)(i + head_.usStart), 0 };
				outfile << zqdb::unicode_to_utf8(wstr) << "=" << &datas_[i * head_.filednum] << "\n";
			}
			outfile.close();
		}*/
	}
	catch (...) {

	}
}

void MatchCH::Init(const char* file)
{
	g_match_ch.InnerInit(file);
}

bool MatchCH::MatchPinYin(wchar_t name, wchar_t key)
{
	bool ret = false;
	unsigned short usName = name;
	if (usName >= head_.usStart && usName < head_.usEnd) {
		for (int i = usName - head_.usStart, j = 0; j < head_.filednum; j++) {
			if (datas_[i * head_.filednum + j] == 0) {
				break;
			}
			else if (datas_[i * head_.filednum + j] == key) {
				ret = true;
				break;
			}
		}
	}
	return ret;
}

bool MatchCH::MatchPinYin(wchar_t* name, wchar_t* key)
{
	bool ret = false;
	while (*name && *key) {
		if (!(ret = MatchPinYin(*name++, *key++))) {
			break;
		}
	}
	return ret;
}

// ----------------------------------------------------------------------------
// SmartKB implementation
// ----------------------------------------------------------------------------

SmartKB::SmartKB()
{
	stop_flag_ = false;
	thread_ = std::make_shared<std::thread>(std::bind(&SmartKB::Search, this));
}

SmartKB::~SmartKB()
{
	if (thread_)
	{
		stop_flag_ = true;
		thread_->join();
		thread_.reset();
	}
}

void SmartKB::SetInputText(const std::wstring& strText, int flag)
{
	std::lock_guard<std::mutex> lock(input_mutex_);
	input_text_ = strText;
	input_flag_ = flag;
	input_changed_ = true;
}

void SmartKB::GetInputText(std::wstring& strText, int& flag)
{
	std::lock_guard<std::mutex> lock(input_mutex_);
	items_ = input_items_;
	strText = input_text_;
	//strText.Trim(false);
	//strText.Trim(true);
	//strText.MakeUpper();
	flag = input_flag_;
	input_changed_ = false;
	input_results_.clear();
	best_results_ = 0;
}

void SmartKB::NotifyResult()
{
	//QueueEvent(new wxCommandEvent(SMARTKB_SEARCH_RESULT_EVENT));
}

void SmartKB::ShowResult()
{
	//std::vector<INPUTRESULT> results;
	//GetInputResults(results);
	///*int Count = listbox_results_.GetCount();
	//for (int i = Count;i >= 0; i--)
	//{
	//listbox_results_.DeleteString(i);
	//}
	//listbox_results_.ResetContent();*/
	//for (size_t i = 0; i < results.size() && i < 2 * max_results_; i++)
	//{
	//	wxString strResult;
	//	strResult.Format(_T("%s %s %s"), (*items_)[results[i].pos]->Code.c_str(), (*items_)[results[i].pos]->PinYin.c_str(), (*items_)[results[i].pos]->Name.c_str());
	//	listbox_results_.AddString(strResult);
	//	listbox_results_.SetItemData(i, results[i].pos);
	//}
	//if (!results.empty())
	//{
	//	listbox_results_.SetCurSel(0);
	//}
	////listbox_results_.UpdateWindow();
}

void SmartKB::AddInputResult(size_t pos, int result)
{
	std::lock_guard<std::mutex> lock(input_mutex_);
	if (result == 0)
	{
		best_results_++;
	}
	input_results_.emplace_back(pos, result);
}

void SmartKB::MergeInputResult()
{
	std::lock_guard<std::mutex> lock(input_mutex_);
	// sort INPUTRESULT so we can find the duplicates 
	if (!input_changed_ && !stop_flag_)
	{
		std::sort(input_results_.begin(), input_results_.end(), INPUTRESULTResultLess());
	}
	//if (!input_changed_ && !stop_flag_)
	//{
	//	std::stable_sort(input_results_.begin(), input_results_.end(), INPUTRESULTPosLess());
	//}
	//if (!input_changed_ && !stop_flag_)
	//{
	//	/* eliminate duplicate INPUTRESULT:
	//	* unique reorders INPUTRESULT so that each INPUTRESULT appears once in the
	//	* front portion of words and returns an iterator one past the unique range;
	//	* erase uses a vector operation to remove the nonunique elements
	//	*/
	//	std::vector<INPUTRESULT>::iterator ite = std::unique(input_results_.begin(), input_results_.end(), INPUTRESULTPosEqual());
	//	input_results_.erase(ite, input_results_.end());
	//}
}

void SmartKB::GetInputs(std::vector<SmartKBItem>& results)
{
	std::lock_guard<std::mutex> lock(input_mutex_);
	auto items = input_items_;
	if (items) {
		results = *items;
	}
	else {
		results.clear();
	}
}

void SmartKB::ClearInputResult()
{
	std::lock_guard<std::mutex> lock(input_mutex_);
	input_results_.clear();
	best_results_ = 0;
}

void SmartKB::GetInputResults(std::vector<SmartKBItem>& results)
{
	std::lock_guard<std::mutex> lock(input_mutex_);
	//results = input_results_;
	size_t i = results.size();
	results.resize(i + input_results_.size());
	for (size_t j = 0; i < results.size(); i++,j++)
	{
		results[i] = (*items_)[input_results_[j].pos];
	}
}

void SmartKB::Search()
{
	while (!stop_flag_)
	{
		if (input_changed_)
		{
			std::wstring strText;
			int flag = 0;
			GetInputText(strText, flag);
			if (!strText.empty() && items_)
			{
				if (!input_changed_ && !stop_flag_)
				{
					SearchKey(strText);
				}
				if (!input_changed_ && !stop_flag_)
				{
					MergeInputResult();
				}
			}
			if (stop_flag_)
			{
				break;
			}
			if (!input_changed_)
			{
				NotifyResult();
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
}

const wchar_t* SmartKB::StrKey(const wchar_t* name, const wchar_t* key)
{
	const wchar_t* str = name;
	const wchar_t* src = name;
	const wchar_t* dst = key;
	while (*src && *dst)
	{
		if (*src == *dst)
		{
			src++; dst++;
		}
		else
		{
			src++; str = src; dst = key;
		}
	}
	if (!*dst)
	{
		return str;
	}
	return nullptr;
}

const wchar_t* SmartKB::StrName(const wchar_t* name, const wchar_t* key)
{
	const wchar_t* str = name;
	const wchar_t* src = name;
	const wchar_t* dst = key;
	while (*src && *dst)
	{
		if (*src == *dst)
		{
			src++; dst++;
		}
		else if (g_match_ch.MatchPinYin(*src, *dst))
		{
			src++; dst++;
		}
		else
		{
			src++; str = src; dst = key;
		}
	}
	if (!*dst)
	{
		return str;
	}
	return nullptr;
}

void SmartKB::SearchKey(const std::wstring& strText)
{
	if (strText == SMARTKB_KEY_MAIN_W) {
		for (size_t i = 0; i < items_->size() && !input_changed_ && !stop_flag_; i++)
		{
			const auto& item = (*items_)[i];
			auto codelen = item.Code.size();
			if (codelen > 2) {
				if (item.Code[codelen - 2] == '0' && item.Code[codelen - 1] == '0') {
					AddInputResult(i, 0);
				}
			}
		}
	}
	else if (strText == SMARTKB_KEY_SUBSCRIBE_W) {
		for (size_t i = 0; i < items_->size() && !input_changed_ && !stop_flag_; i++)
		{
			const auto& item = (*items_)[i];
			if (item.Type == SMARTKB_ITEM_TYPE_CODE) {
				if (ZQDBGetFlags((HZQDB)item.Data) & ZQDB_CODE_FLAG_SUBSCRIBE) {
					AddInputResult(i, 0);
				}
			}
		}
	}
	else if (strText == SMARTKB_KEY_ALL_SUBSCRIBE_W) {
		for (size_t i = 0; i < items_->size() && !input_changed_ && !stop_flag_; i++)
		{
			const auto& item = (*items_)[i];
			if(item.Type == SMARTKB_ITEM_TYPE_CODE) {
				if (ZQDBIsSubscribeMarketData((HZQDB)item.Data)) {
					AddInputResult(i, 0);
				}
			}
		}
	}
	else {
		auto key = strText.c_str();
		for (size_t i = 0; i < items_->size() && !input_changed_ && !stop_flag_; i++)
		{
			int result1 = -1, result2 = -1;
			const wchar_t* str = nullptr;
			if (input_flag_ == 0 || input_flag_ & INPUT_FLAG_CODE) {
				auto code = (*items_)[i].Code.c_str();
				str = StrKey(code, key);
				if (str) {
					result1 = str - code;
					assert(result1 >= 0);
				}
			}
			if (input_flag_ == 0 || input_flag_ & INPUT_FLAG_PINYIN) {
				if (result1 != 0) {
					if ((*items_)[i].PinYin.empty()) {
						auto name = (*items_)[i].Name.c_str();
						str = StrName(name, key);
						if (str) {
							result2 = str - name;
							assert(result2 >= 0);
						}
					}
					else {
						auto pinyin = (*items_)[i].PinYin.c_str();
						str = StrKey(pinyin, key);
						if (str) {
							result2 = str - pinyin;
							assert(result2 >= 0);
						}
					}
				}
			}
			if (result1 >= 0 && result2 >= 0) {
				AddInputResult(i, std::min<>(result1, result2));
			}
			else if (result1 >= 0) {
				AddInputResult(i, result1);
			}
			else if (result2 >= 0) {
				AddInputResult(i, result2);
			}
			if (best_results_ >= max_results_) {
				break;
			}
		}
	}
}

void SmartKB::Add(const std::string& strType, SmartKBItem *pData, int nCount)
{
	std::shared_ptr<std::vector<SmartKBItem>>& items = all_items_[strType];
	if (!items) {
		items = std::make_shared<std::vector<SmartKBItem>>();
	}
	for (int i = 0; i < nCount; i++)
	{
		if (pData[i].PinYin.empty()) {
			/*pData[i].PinYin.resize(pData[i].Name.size());
			auto pPinYin = (wchar_t*)pData[i].PinYin.data();
			auto pName = pData[i].Name.data();
			while (*pName)
			{
				*pPinYin++ = FirstLetter(*pName++);
			}*/
		}
	}
	items->insert(items->end(), pData, pData + nCount);
}

void SmartKB::Clear(const std::string& strType)
{
	all_items_.erase(strType);
	input_items_.reset();
}

bool SmartKB::Select(const std::string& strType)
{
	auto it = all_items_.find(strType);
	if (it == all_items_.end()) {
		return false;
	}
	input_items_ = all_items_[strType];
	return true;
}

void SmartKB::SetMaxResult(size_t nMaxResult)
{
	max_results_ = nMaxResult;
}

bool SmartKB::Input(const wchar_t* lpszKey, int flag)
{
	if (!input_items_) {
		//没有选择分组
		return false;
	}
	SetInputText(lpszKey, flag);
	return true;
}
