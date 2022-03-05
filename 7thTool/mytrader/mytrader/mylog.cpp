#include "mylog.h"
#include "myapp.h"
#include "mylogdlg.h"
#include <net.h>

#include <set>
#include <queue>
#include <fstream>
#include <sstream>

static size_t max_record_count_ = 100;
static mylog* g_log = nullptr;
static wxLogFormatter* g_formater = nullptr;
static bool g_async = false;

wxString mylog::Format(const wxLogRecord& record)
{
	//return g_formater->Format(record.level, record.msg, record.info);
	const char* level = nullptr;
	switch (record.level)
	{
	case wxLOG_FatalError:
	case wxLOG_Error:
		level = ("Error");
		break;
	case wxLOG_Warning:
		level = ("Warn ");
		break;
	case wxLOG_Message:
	case wxLOG_Status:
	case wxLOG_Info:
		level = ("Info ");
		break;
	case wxLOG_Debug:
	case wxLOG_Trace:
	default:
		level = ("Debug");
		break;
	}
	return wxString::Format(wxT("%s %09zu %s: %s"), wxDateTime(record.info.timestamp).Format("%H:%M:%S").c_str(), (size_t)record.info.threadId, level, record.msg.c_str());
}

void mylog::Init(const char* xml, size_t xmlflag)
{
	//char fielname[260] = { 0 };
	//std::time_t now_time; 
	//std::time(&now_time);
	//std::strftime(fielname, 260, "mytrader_%Y-%m-%d.log", std::localtime(&now_time));
	//auto filename = wxString2utf(wxDateTime::Now().Format(wxS("mytrader_%Y-%m-%d.log")));
	boost::system::error_code ec;
	for (boost::filesystem::directory_iterator it(".", ec); it != boost::filesystem::directory_iterator(); ++it) {
		const auto& path = it->path();
		if (boost::filesystem::is_regular(path, ec) && path.extension() == ".log") {
			auto modify_time = boost::filesystem::last_write_time(path, ec);
			std::chrono::system_clock::time_point modify_tp = std::chrono::system_clock::from_time_t(modify_time);
			std::chrono::system_clock::time_point now_tp = std::chrono::system_clock::now();
			if ((now_tp - modify_tp) > std::chrono::hours(24 * 2)) {
				boost::filesystem::remove(path, ec);
			}
		}
	}

	g_log = new mylog(xml, xmlflag);
	g_formater = g_log->SetFormatter(nullptr);
	g_log->SetFormatter(g_formater);
#ifndef _DEBUG
	g_log->SetLogLevel(wxLOG_Info);
#endif//
    SetActiveTarget(g_log);
}

void mylog::Term()
{
	g_log = nullptr;
	delete SetActiveTarget(nullptr);
}

void mylog::SetAsync(bool async)
{
	g_async = async;
}

size_t mylog::GetRecentLogCount()
{
	return g_log->GetRecordCount();
}

wxLogRecord mylog::GetRecentLog(size_t pos)
{
	return g_log->GetRecord(pos);
}
//
//bool mylog::GetRecentLog(wxLogRecord& record, wxLogLevel level)
//{
//	return g_log->GetRecentRecord(record, level);
//}

mylog::mylog(const char* xml, size_t xmlflag):records_(max_record_count_)
{
	
}

mylog::~mylog()
{
	DoFlush();
}

size_t mylog::GetRecordCount()
{
	return records_.size();
}

wxLogRecord mylog::GetRecord(size_t pos)
{
	wxCriticalSectionLocker lock(section_);
	return records_[pos];
}
//
//bool mylog::GetRecentRecord(wxLogRecord& record, wxLogLevel level)
//{
//	wxCriticalSectionLocker lock(section_);
//	for (int pos = records_.size() - 1; pos >= 0; pos--)
//	{
//		if (records_[pos].level = level) {
//			record = records_[pos];
//			return true;
//		}
//	}
//	return false;
//}

//std::vector<wxLogRecord> mylog::GetLogs() {
//	wxCriticalSectionLocker lock(section_);
//	return records_;
//}

void mylog::DoFlush()
{
	try {
		wxCriticalSectionLocker lock(section_);
		auto record_count = records_.size();
		if (last_flush_pos_ < record_count) {
			wxString filename = wxDateTime::Now().Format(wxS(APP_NAME "_%Y-%m-%d.log"));
			std::wofstream outfile(filename.wc_str(), std::ios::out | std::ios::app);
			//outfile.imbue(std::locale("zh_CN"));
			if (outfile.is_open())
			{
				for (size_t pos = last_flush_pos_; pos < record_count; pos++)
				{
					try {
						outfile << Format(records_[pos]);
					}
					catch (...) {
						
					}
					outfile << std::endl;
				}
				if (record_count > max_record_count_) {
					size_t erase_count = records_.size() - max_record_count_;
					records_.erase(records_.begin(), records_.begin() + erase_count);
					wxASSERT(max_record_count_ == records_.size());
					last_flush_pos_ = records_.size();
				}
				else {
					last_flush_pos_ = record_count;
				}
				outfile.close();
			}
		}
	}
	catch (...) {

	}
}

void mylog::Flush()
{
	if (g_async) {
		net::Post([](void* data) {
			mylog* pT = (mylog*)data;
			pT->DoFlush();
		}, this);
	}
	else {
		DoFlush();
	}
}

void mylog::DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info)
{
	wxASSERT(GetLogLevel() >= level);
	wxLogRecord record(level, msg, info);
	{
		wxCriticalSectionLocker lock(section_);
		if (records_.size() >= max_record_count_) {
			if (last_flush_pos_ > 0) {
				last_flush_pos_--;
			}
		}
		records_.push_back(std::move(record));
	}
	MyLogDlg::AppendLog(record);
}

