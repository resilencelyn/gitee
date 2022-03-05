#pragma once

#include <mymodule.h>

// -- MyCTPLoginView -- 

class MyCTPLoginView : public MyLoginView
{
	typedef MyLoginView Base;
private:
	std::string userid_;
public:
	MyCTPLoginView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	virtual void UpdateInfo(const char* const pInfo)override;
	virtual const char* StartLogin()override;
	virtual void CancelLogin()override;

	void EnableUI(bool enable);

	int OnNetMsg(zqdb::Msg& msg);
private:
	wxStaticText* m_pTip = nullptr;
	wxTextCtrl* m_pUserNameTxt = nullptr;
	wxTextCtrl* m_pPasswordNameTxt = nullptr;
	wxTextCtrl* m_pCodeTxt = nullptr;
};

// -- MyCTPUserInfoView -- 

class MyCTPUserInfoView : public MyUserViewT<MyCTPUserInfoView>
{
	typedef MyUserViewT<MyCTPUserInfoView> Base;
private:
	wxDataViewCtrl* ctrl_user_list_ = nullptr;
	wxObjectDataPtr<zqdb::HZQDBModel> ctrl_user_list_model_;
	wxDataViewCtrl* ctrl_account_list_ = nullptr;
	wxObjectDataPtr<zqdb::HZQDBListModel> ctrl_account_list_model_;
	wxDataViewCtrl* ctrl_investor_list_ = nullptr;
	wxObjectDataPtr<zqdb::HZQDBListModel> ctrl_investor_list_model_;
public:
	MyCTPUserInfoView(wxWindow *parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	void OnSkinInfoChanged();
	void OnUserChanged();
};

// -- MyCTPAccountStatView -- 

struct AccountStatInfo
{
	time_t Time;
	double Balance;
	double Available;
	double Profit;
};

class MyCTPAccountStatView : public MyUserViewT<MyCTPAccountStatView>
{
	typedef MyUserViewT<MyCTPAccountStatView> Base;
private:
	wxChartPanel* chart_ = nullptr;
	std::chrono::steady_clock::time_point last_refresh_time_;
	std::vector<double> datas_;
	std::vector<time_t> times_;
public:
	MyCTPAccountStatView(wxWindow *parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	void RefreshAll();

	void OnSkinInfoChanged();
	void OnUserChanged();

	//void OnTimer(wxTimerEvent& event);
};

// -- MyCTPUserOrderView -- 

class MyCTPUserOrderView : public MyUserViewT<MyCTPUserOrderView>
{
	typedef MyUserViewT<MyCTPUserOrderView> Base;
private:
	wxDataViewCtrl* ctrl_list_ = nullptr;
	wxObjectDataPtr<zqdb::HZQDBListModel> ctrl_list_model_;
public:
	MyCTPUserOrderView(wxWindow *parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	void ShowAll();
	void ClearAll();
	void RefreshAll();

protected:
	void OnShowEvent(wxShowEvent& evt);
	void OnSelChanged(wxDataViewEvent &event);
	void OnActivated(wxDataViewEvent &event);
	wxDECLARE_EVENT_TABLE();
};

// -- MyCTPUserTradeView -- 

class MyCTPUserTradeView : public MyUserViewT<MyCTPUserTradeView>
{
	typedef MyUserViewT<MyCTPUserTradeView> Base;
private:
	wxDataViewCtrl* ctrl_list_ = nullptr;
	wxObjectDataPtr<zqdb::HZQDBListModel> ctrl_list_model_;
public:
	MyCTPUserTradeView(wxWindow *parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	void ShowAll();
	void ClearAll();
	void RefreshAll();

protected:
	void OnShowEvent(wxShowEvent& evt);
	void OnSelChanged(wxDataViewEvent &event);
	void OnActivated(wxDataViewEvent &event);
	wxDECLARE_EVENT_TABLE();
};

// -- MyCTPUserPositionView -- 

class MyCTPUserPositionView : public MyUserViewT<MyCTPUserPositionView>
{
	typedef MyUserViewT<MyCTPUserPositionView> Base;
private:
	wxDataViewCtrl* ctrl_list_ = nullptr;
	wxObjectDataPtr<zqdb::HZQDBListModel> ctrl_list_model_;
public:
	MyCTPUserPositionView(wxWindow *parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	void ShowAll();
	void ClearAll();
	void RefreshAll();

protected:
	void OnShowEvent(wxShowEvent& evt);
	void OnSelChanged(wxDataViewEvent &event);
	void OnActivated(wxDataViewEvent &event);
	wxDECLARE_EVENT_TABLE();
};

// -- MyCTPUserPositionView -- 

class MyCTPUserCommissionRateView : public MyUserViewT<MyCTPUserCommissionRateView>
{
	typedef MyUserViewT<MyCTPUserCommissionRateView> Base;
private:
	wxDataViewCtrl* ctrl_list_ = nullptr;
	wxObjectDataPtr<zqdb::HMTABLEListModel> ctrl_list_model_;
	HMDB hdb_ = nullptr;
	HMTABLE htb_ = nullptr;
public:
	MyCTPUserCommissionRateView(wxWindow *parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	void ShowAll();
	void ClearAll();

protected:
	void OnShowEvent(wxShowEvent& evt);
	wxDECLARE_EVENT_TABLE();
};

// -- MyCTPMiniView -- 

class MyCTPMiniView : public MyMiniView
{
	typedef MyMiniView Base;
private:
	enum ID_COMMANDS
	{
		// these should be in the same order as Type_XXX elements above
		ID_BOOK_NOTEBOOK = wxID_HIGHEST,
		ID_BOOK_LISTBOOK,
		ID_BOOK_CHOICEBOOK,
		ID_BOOK_TREEBOOK,
		ID_BOOK_TOOLBOOK,
		ID_BOOK_AUINOTEBOOK,
		ID_BOOK_SIMPLEBOOK,
		ID_BOOK_MAX,

		ID_ORIENT_DEFAULT,
		ID_ORIENT_TOP,
		ID_ORIENT_BOTTOM,
		ID_ORIENT_LEFT,
		ID_ORIENT_RIGHT,
		ID_ORIENT_MAX,
		ID_SHOW_IMAGES,
		ID_FIXEDWIDTH,
		ID_MULTI,
		ID_NOPAGETHEME,
		ID_BUTTONBAR,
		ID_HORZ_LAYOUT,
		ID_ADD_PAGE,
		ID_ADD_PAGE_NO_SELECT,
		ID_INSERT_PAGE,
		ID_DELETE_CUR_PAGE,
		ID_DELETE_LAST_PAGE,
		ID_NEXT_PAGE,
		ID_ADD_PAGE_BEFORE,
		ID_ADD_SUB_PAGE,
		ID_CHANGE_SELECTION,
		ID_SET_SELECTION,
		ID_GET_PAGE_SIZE,
		ID_SET_PAGE_SIZE,

#if wxUSE_HELP
		ID_CONTEXT_HELP,
#endif // wxUSE_HELP
		ID_HITTEST
	};
	// Sample setup
	enum BookType
	{
		Type_Notebook,
		Type_Listbook,
		Type_Choicebook,
		Type_Treebook,
		Type_Toolbook,
		Type_AuiNotebook,
		Type_Simplebook,
		Type_Max
	} m_type;
	int m_orient;
	bool m_chkShowImages;
	bool m_fixedWidth;
	bool m_multi;
	bool m_noPageTheme;
	bool m_buttonBar;
	bool m_horzLayout;
	wxBookCtrlBase *m_bookCtrl; 
	MyCTPAccountStatView* account_stat_view_ = nullptr;
	MyCTPUserInfoView* info_view_ = nullptr;
	MyCTPUserOrderView* order_view_ = nullptr;
	MyCTPUserTradeView* trade_view_ = nullptr;
	MyCTPUserPositionView* position_view_ = nullptr;
	MyCTPUserCommissionRateView* commission_rate_view_ = nullptr;
public:
	MyCTPMiniView(wxWindow *parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	void OnSkinInfoChanged();
	void OnHandleChanged();
	void OnUserChanged();

	void OnNotifyEnable(HZQDB h);
	void OnNotifyDisable(HZQDB h);
	void OnNotifyAppend(HZQDB h);
	void OnNotifyUpdate(HZQDB h);
protected:
	//void DoGoto(const wxString& view);
	void ShowView(wxWindow* page);

	void OnTimer(wxTimerEvent& event);
	void OnShowEvent(wxShowEvent& evt);
	wxDECLARE_EVENT_TABLE();
};

// -- MyCTPModule --

class MyCTPModule : public MyModule
{
	typedef MyModule Base;
private:	
	//uint32_t tradeday_ = 0;	
	//uint8_t trade_time_count_ = 0; // 交易时段数量
	//uint16_t trade_time_[4][2] = { 0 }; // 交易时段描述[最多只是4个交易时段，每个时段包括开始、结束时间，精确到分，HH * 60 + MM格式]
	size_t max_time_point_ = 0;
	std::map<HZQDB, std::vector<AccountStatInfo>> account_stat_infos_;
	void RefreshBaseInfo();
	void ClearBaseInfo();
	inline bool IsBaseInfoOk() { return max_time_point_ != 0; }
public:
	MyCTPModule(HZQDB h);
	virtual ~MyCTPModule();

	//size_t GetMaxTimePoint() { return max_time_point_; }

	uint32_t GetTradeTime(uint32_t* date = nullptr, uint32_t* tradeday = nullptr);

	wxString GetUserInfo(HZQDB user);
	const std::vector<AccountStatInfo>& GetAccountStatInfo(HZQDB account);

	MyLoginView* NewLoginView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	MyMiniView* NewMiniView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	int ReqModifyPassword(HZQDB user, const char* old_pwd, const char* new_pwd, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0);
	int ReqModifyAccountPassword(HZQDB account, const char* old_pwd, const char* new_pwd, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0);

	int OrderSend(HZQDB user, HZQDB code, char direction, char offset, char type, double volume, double price, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0);
	int OrderCancel(HZQDB user, HZQDB order, HNMSG* rsp = nullptr, size_t timeout = 0, size_t flags = 0);
	int OrderClose(HZQDB user, HZQDB position, char type, double volume, double price, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0);

	int ReqQryMaxOrderVolume(HZQDB user, HZQDB code, char direction, char offset, char type, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0);

	void OnTimer();

	void OnNotifyEnable(HZQDB h);
	void OnNotifyDisable(HZQDB h);
	void OnNotifyAppend(HZQDB h);
	void OnNotifyUpdate(HZQDB h);

	//void OnNetStatus(HNNODE h, NET_NODE_STATUS status);
	int OnNetMsg(zqdb::Msg& msg);
};

