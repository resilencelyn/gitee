#include "mylogindlg.h"


// ----------------------------------------------------------------------------
// MyLoginDlg implementation
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyLoginDlg, Base)
	EVT_BUTTON(wxID_OK, MyLoginDlg::OnOK)
	EVT_BUTTON(wxID_IGNORE, MyLoginDlg::OnIgnore)
	EVT_BUTTON(wxID_CANCEL, MyLoginDlg::OnCancel)
wxEND_EVENT_TABLE()

MyLoginDlg* MyLoginDlg::s_dlg = nullptr;

MyLoginDlg::MyLoginDlg()
	:Base(nullptr, wxID_ANY, wxT("登录"), wxDefaultPosition, wxDefaultSize)
{
	wxASSERT(!s_dlg);
	s_dlg = this; //

	//
	//|广|LoginView|
	//|告|LoginView|
	//|栏|LoginView|
	//    登录 取消
	//
	MyModule::Broadcast([this](std::shared_ptr<MyModule> module)->bool {
		if ((*module)->flags & MODULE_FLAG_MARKET_LOGIN) {
			auto view = module->NewLoginView(this);
			if (view) {
				ModuleInfo info = { module, view, nullptr, nullptr };
				modules_.emplace_back(info);
			}
		}
		return false;
	});
	
	btn_ok_ = new wxButton(this, wxID_OK, "&OK");
	btn_ignore_ = new wxButton(this, wxID_IGNORE, "&Ignore");
	btn_cancel_ = new wxButton(this, wxID_CANCEL, "&Cancel");

	wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

	for (auto& module : modules_)
	{
		sizerTop->Add(module.view, 0, wxEXPAND);
	}

	wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

	sizerBottom->Add(btn_ok_, 0, wxALIGN_CENTER | wxALL, 5);
	sizerBottom->Add(btn_ignore_, 0, wxALIGN_CENTER | wxALL, 5);
	sizerBottom->Add(btn_cancel_, 0, wxALIGN_CENTER | wxALL, 5);

	sizerTop->Add(sizerBottom, 0, wxEXPAND);

	SetSizerAndFit(sizerTop);

	Centre();

	SetEscapeId(wxID_CANCEL);

	btn_ok_->SetFocus();
	btn_ok_->SetDefault();
	Bind(ZQDB_NOTIFY_LOG_UPDATE_EVENT, &MyLoginDlg::OnUpdateStatus, this);
	/*Bind(ZQDB_NOTIFY_ENABLE_EVENT, &MyLoginDlg::OnNotifyEvent, this);
	Bind(ZQDB_NOTIFY_DISABLE_EVENT, &MyLoginDlg::OnNotifyEvent, this);
	Bind(ZQDB_NOTIFY_APPEND_EVENT, &MyLoginDlg::OnNotifyEvent, this);
	Bind(ZQDB_NOTIFY_REMOVE_EVENT, &MyLoginDlg::OnNotifyEvent, this);
	Bind(ZQDB_NOTIFY_UPDATE_EVENT, &MyLoginDlg::OnNotifyEvent, this);*/
}

MyLoginDlg::~MyLoginDlg()
{
	s_dlg = nullptr;
}

void MyLoginDlg::OnUpdateStatus(wxStringCommandEvent& event)
{
	CFG_FROM_XML(cfg, event.m_data.c_str(), XUtil::XML_FLAG_JSON_STRING);
	auto userInfo = cfg.get<string>("user", "");
	if ("" == userInfo)
	{
		return;
	}
	auto contentInfo = cfg.get<string>("content", "");
	for (auto& module_ : modules_)
	{
		if (strstr(module_.user,userInfo.c_str()))
		{
			module_.view->UpdateInfo(contentInfo.c_str());
		}
	}
}

void MyLoginDlg::OnSkinInfoChanged()
{
	//SetBackgroundColour(skin_info_ptr_->crPrimary);
}

void MyLoginDlg::OnNotifyUpdate(HZQDB h)
{
	DealUserLogin(h);
}

void MyLoginDlg::OnNotifyAppend(HZQDB h)
{
	DealUserLogin(h);
}

void MyLoginDlg::DealUserLogin(HZQDB h)
{
	if (h->type != ZQDB_HANDLE_TYPE_USER)
		return;

	zqdb::ObjectT<tagUserInfo> user(h);
	if (user->Status == USER_STATUS_LOGGED) {
		bool bAllLogin = true;
		for (auto& module : modules_)
		{
			if (module.user) {
				if (strstr(module.user, user->User)) {
					module.h = h;
				} else {
					bAllLogin = false;
				}
			}
		}
		if (bAllLogin) {
			EndModal(wxID_OK);
		}
	}
}

//void MyLoginDlg::OnNotifyEvent(wxCommandEvent& event)
//{
//	auto h = (HZQDB)event.GetClientData();
//	if (h) {
//		/*auto evt_type = event.GetEventType();
//		if (evt_type == ZQDB_NOTIFY_ENABLE_EVENT)
//		{
//			ctrl_list_model_->UpdateAll();
//		}
//		else if (evt_type == ZQDB_NOTIFY_DISABLE_EVENT)
//		{
//			ctrl_list_model_->ClearAll();
//		}
//		else if (evt_type == ZQDB_NOTIFY_APPEND_EVENT)
//		{
//			
//		}
//		else if (evt_type == ZQDB_NOTIFY_REMOVE_EVENT)
//		{
//			
//		}
//		else if (evt_type == ZQDB_NOTIFY_UPDATE_EVENT)
//		{
//			
//		}*/
//	}
//}

int MyLoginDlg::OnNetMsg(zqdb::Msg& msg)
{
	for (auto& module : modules_)
	{
		if (module.view) {
			int ret = module.view->OnNetMsg(msg);
			if (ret) {
				return ret;
			}
		}
	}
	return 0;
}

void MyLoginDlg::OnOK(wxCommandEvent& event)
{
	for (auto& module : modules_)
	{
		module.user = module.view->StartLogin();
	}
	btn_ok_->Enable(false);
	//btn_cancel_->Enable(false);
	//EndModal(wxID_OK);
}

void MyLoginDlg::OnIgnore(wxCommandEvent& event)
{
	EndModal(wxID_OK);
}

void MyLoginDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(wxID_CANCEL);
}



