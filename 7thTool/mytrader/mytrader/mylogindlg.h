// aboutdlg.h : interface of the CMySmartKBDlg class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _H_LOGINDLG_H_
#define _H_LOGINDLG_H_

#include "myapp.h"
#include "mymodule.h"

class MyLoginDlg 
	: public wxDialog
	, public zqdb::SkinMap<MyLoginDlg, SkinInfo>
	, public zqdb::NotifyMap<MyLoginDlg>
	, public zqdb::MsgMap<MyLoginDlg>
{
	typedef wxDialog Base;
	typedef zqdb::SkinMap<MyLoginDlg, SkinInfo> SkinBase;
	typedef zqdb::NotifyMap<MyLoginDlg> NotifyBase;
	typedef zqdb::MsgMap<MyLoginDlg> MsgBase;
private:
	static MyLoginDlg* s_dlg;
public:
	static MyLoginDlg* Inst() { return s_dlg; }
private:
	struct ModuleInfo
	{
		std::shared_ptr<MyModule> module;
		MyLoginView* view;
		const char* user;
		HZQDB h;
	};
	std::vector<ModuleInfo> modules_;
	wxButton* btn_ok_ = nullptr;
	wxButton* btn_ignore_ = nullptr;
	wxButton* btn_cancel_ = nullptr;
public:
	MyLoginDlg();
	virtual ~MyLoginDlg();
	void OnUpdateStatus(wxStringCommandEvent& event);
	//
	
	//
	void OnSkinInfoChanged();
	void OnNotifyUpdate(HZQDB h);
	void OnNotifyAppend(HZQDB h);

	int OnNetMsg(zqdb::Msg& msg);

protected:
	//
	void DealUserLogin(HZQDB h);
	//void OnNotifyEvent(wxCommandEvent& event);

	void OnOK(wxCommandEvent& event);
	void OnIgnore(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

#endif//_H_LOGINDLG_H_
