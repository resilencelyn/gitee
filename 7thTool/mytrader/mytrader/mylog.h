#pragma once

#include "wx/log.h"
#include "wx/datetime.h"
//#include <XUtil/XXml.hpp>
#include <boost/circular_buffer.hpp>

class mylog : public wxLog
{
public:
    static void Init(const char* xml = nullptr, size_t xmlflag=0/* XUtil::XML_FLAG_JSON_STRING*/);
	static void Term();
	static void SetAsync(bool async = true);
	static size_t GetRecentLogCount();
	static wxLogRecord GetRecentLog(size_t pos);
	//static bool GetRecentLog(wxLogRecord& record, wxLogLevel level);
	static wxString Format(const wxLogRecord& record);
private:
	wxCriticalSection section_;
	boost::circular_buffer<wxLogRecord> records_;
	size_t last_flush_pos_ = 0;
	void DoFlush();
public:
	mylog(const char* xml = nullptr, size_t xmlflag = 0/* XUtil::XML_FLAG_JSON_STRING*/);
	~mylog();
	size_t GetRecordCount();
	wxLogRecord GetRecord(size_t pos);
	//bool GetRecentRecord(wxLogRecord& record, wxLogLevel level);
    void Flush() override;
    void DoLogRecord(wxLogLevel level,const wxString& msg,const wxLogRecordInfo& info) override; 
	//void DoLogTextAtLevel(wxLogLevel level, const wxString& msg) override;
};


/*#define LOG4C(level,format, ...) \
    do { \
		if(mylog::IsLogLevelEnable(wxLOG_##level)) { \
			wxDO_LOGV(level, format, ##__VA_ARGS__); \
		} \
    } while(0)

#define LOG4E(format,...) LOG4C(wxLOG_Error, format, ##__VA_ARGS__)
#define LOG4W(format,...) LOG4C(wxLOG_Warning, format, ##__VA_ARGS__)
#define LOG4I(format,...) LOG4C(wxLOG_Info, format, ##__VA_ARGS__)
#define LOG4D(format,...) LOG4C(wxLOG_Debug, format, ##__VA_ARGS__)
#ifdef ZQDB_LOG_LEVEL_TIP
#define LOG4T(format,...) LOG4C(wxLOG_Message, format, ##__VA_ARGS__)
#else
#define LOG4T(format, ...) LOG4I(format, ##__VA_ARGS__)
#endif//ZQDB_LOG_LEVEL_TIP*/
