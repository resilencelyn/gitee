#pragma once

#include "base.h"
#include "version.h"
#include "mytraderdef.h"
#include "settings.h"
#include "smartkb.h"
#include <wx/snglinst.h>
#include <wx/debugrpt.h>

extern "C" {
	void CB_LOG(int level, char const* const format, void const* const args);
	void CB_STATUS(HNNODE h, NET_NODE_STATUS status);
	int CB_MSG(HNMSG hmsg, size_t* flags);
	void CB_NOTIFY(HZQDB h, ZQDB_NOTIFY_TYPE notify);
	int CB_ORDER_SEND(HZQDB user, HZQDB code, char direction, char offset, char type, double volume, double price, HNMSG* rsp, size_t timeout);
	int CB_ORDER_CANCEL(HZQDB user, HZQDB order, HNMSG* rsp, size_t timeout);
	int CB_ORDER_CLOSE(HZQDB user, HZQDB position, char type, double volume, double price, HNMSG* rsp, size_t timeout);
}

#if !defined(APP_NAME)
#define APP_NAME "mytrader"
#endif

#if !defined(APP_VERSION)
#define APP_VERSION MYTRADER_VERSION
#endif

#define OPTION_WAIT "wait"
#define OPTION_CLEAR_SETTINGS "clearsettings"
#define OPTION_CLEAR_DATA "cleardata"
#define OPTION_TEST "test"

// this is typically in a header: it just declares MY_EVENT event type
wxDECLARE_EVENT(MY_EVENT, wxCommandEvent);

//派生一个模板类，用来携带传递非指针类型数据,指针数据在异步处理中容易失效
template<typename T>
class wxCommandEventEx :public wxCommandEvent
{
public:
	wxCommandEventEx(const T& data, wxEventType commandType = wxEVT_NULL, int winid = 0)
		: wxCommandEvent(commandType, winid), m_data(data){}
	T m_data;
};

typedef wxCommandEventEx<string> wxStringCommandEvent;
wxDECLARE_EVENT(ZQDB_NOTIFY_LOG_UPDATE_EVENT, wxStringCommandEvent);

class MySmartKBDlg;
class MyFrame;
class MyTechFrame;
class MyCalcFrame;
class MyStrategyFrame;

// -- application --

class MyEventFilter : public wxEventFilter
{
public:
	MyEventFilter();
	~MyEventFilter();
	virtual int FilterEvent(wxEvent& event);
private:
	wxDECLARE_NO_COPY_CLASS(MyEventFilter);
};

class MyApp : public zqdb::App
	, public MyEventFilter
	, public SettingsMap<MyApp>
	, public zqdb::TaskImplT<MyApp>
	, public zqdb::NotifyImplT<MyApp>
	, public zqdb::MsgImplT<MyApp>
{
	typedef zqdb::App Base;
	typedef SettingsMap<MyApp> SettingsBase;
	typedef zqdb::TaskImplT<MyApp> TaskBase;
	typedef zqdb::NotifyImplT<MyApp> NotifyBase;
	typedef zqdb::MsgImplT<MyApp> MsgBase;
protected:
	std::shared_ptr<wxSingleInstanceChecker> snglinst_checker_;
	wxTaskBarIcon *taskbaricon_ = nullptr; //系统托盘
#if defined(__WXOSX__) && wxOSX_USE_COCOA
	MyTaskBarIcon *dockicon_ = nullptr;
#endif
	std::vector<wxDialog*> modaldlgs_;
	MySmartKBDlg *smartkbdlg_ = nullptr; //键盘精灵
	MyFrame* frame_ = nullptr; //主框架
	std::vector<MyTechFrame *> tech_frames_; //技术框架列表
	std::vector<MyCalcFrame*> calc_frames_; //公式框架
	std::vector<MyStrategyFrame *> strategy_frames_; //策略框架列表
	wxScopedPtr<wxPreferencesEditor> m_prefEditor;
	size_t run_flag_ = 0; //运行标志
	size_t suspend_flag_ = 0; //挂起标志
	bool exit_flag_ = false; //退出标志
	wxTimer timer_;
	wxArrayString subscribes_; //未识别的订阅，需要等待对应市场初始化完成才能识别
	std::shared_ptr<wxBusyInfo> busy_info_;
	wxString last_tips_;
protected:
	void LoadSkinInfo();
	void DecorateSplashScreen(wxBitmap& bmp, bool about = false);
public:
	MyApp();

#if wxUSE_CMDLINE_PARSER
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser) wxOVERRIDE;
	virtual void OnInitCmdLine(wxCmdLineParser& parser) wxOVERRIDE;
#endif

    bool OnInit() wxOVERRIDE;
    int OnExit() wxOVERRIDE;

#ifndef _DEBUG
	// called when a crash occurs in this application
	virtual void OnFatalException() wxOVERRIDE;
#endif//
	// this is where we really generate the debug report
	void GenerateReport(wxDebugReport::Context ctx);

	void ShowBusyInfo(const wxString& msg);
	inline size_t IsRunFlag() { return run_flag_; }
	void Run();
	void FinalRelease();
	void FinalExit();
	void Exit();
	void DoExit();
	inline bool IsExitFlag() { return exit_flag_; }

	wxString GetAppMode();
	wxString GetAppTitle();
	wxString GetAppStatus();

	wxDialog* AnyModalDlg();
	bool RemoveModalDlg(wxDialog* dlg);
	void AddModalDlg(wxDialog* dlg);

	inline MySmartKBDlg* GetSmartKBDlg() { return smartkbdlg_; }
	inline void ResetSmartKBDlg() { smartkbdlg_ = nullptr; }

	void DoHide();
	wxFrame* GetFrame();
	bool FindTechFrame(wxFrame * frame);
	void ShowFrame(wxFrame* frame = nullptr);
	void ResetFrame(wxFrame * frame);

	void Goto(HZQDB h, wxWindow* top = nullptr);

	void ResetCalcFrame(MyCalcFrame* frame);
	void ShowCalcFrame(std::shared_ptr<zqdb::FuncContainerInfo> info_ptr);


	//运行策略一般需要两个部分一个是筛选，一个是策略
	//筛选可以是一组代码、品种、市场、条件的组合，最简单的筛选就是一个代码
	//策略就是实时处理筛选出的代码并给出交易判断
	//void RunStrategy(const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	//void ShowStrategyFrame(const wxString& name);
	void ResetStrategyFrame(MyStrategyFrame * frame);

	enum
	{
		PREFS_GENERAL = 0X01, 
		PREFS_FILTER = 0X02,
		PREFS_SORT = 0X04,
		PREFS_TECH = 0X08,
		PREFS_SCRIPT = 0X10,
		PREFS_STRATEGY = 0X20,
	};
	void ShowPreferencesEditor(wxWindow* parent, size_t flags = 0);
	void DismissPreferencesEditor();
	void Resettings(bool showMsg = true);
	void DoClearData();
	void ClearData(bool showMsg = true);
	void DoRestart(const wxString& args);
	void Restart(bool showMsg = true);
	void Test(size_t begin, size_t end, size_t speed);

	void ShowAbout(wxWindow* parent = nullptr);

	void LoadSubscribe();
	void SaveSubscribe();
	bool IsSubscribe(HZQDB h);
	void AddSubscribe(HZQDB h, bool save = true);
	void RemoveSubscribe(HZQDB h, bool save = true);

	void OnTimer(wxTimerEvent& evt);

	void OnNotifyConnect();
	void OnNotifyDisconnect();
	void OnNotifyStartListen();
	void OnNotifyStopListen();
	void OnNotifyEnable(HZQDB h);
	void OnNotifyDisable(HZQDB h);
	void OnNotifyAppend(HZQDB h);
	void OnNotifyRemove(HZQDB h);
	void OnNotifyUpdate(HZQDB h);
	void OnPreNotify(HZQDB h, ZQDB_NOTIFY_TYPE notify);
	void HandleNotify(HZQDB h, ZQDB_NOTIFY_TYPE notify);

	void OnNetStatus(HNNODE h, NET_NODE_STATUS status);
	int OnNetMsg(zqdb::Msg& msg);
	void HandleNetStatus(HNNODE h, NET_NODE_STATUS status);
	int HandleNetMsg(HNMSG hmsg, size_t* flags);

	int OrderSend(HZQDB user, HZQDB code, char direction, char offset, char type, double volume, double price, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0);
	int OrderCancel(HZQDB user, HZQDB order, HNMSG* rsp = nullptr, size_t timeout = 0, size_t flags = 0);
	int OrderClose(HZQDB user, HZQDB position, char type, double volume, double price, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0);

	void ShowTips(const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	inline const wxString& GetLastTips() { return last_tips_; }
};

wxDECLARE_APP(MyApp);
