#pragma once

#include <tuple>
#include "mybaseframe.h"
#include "mycalcdlg.h"

class MyTechFrame;

//ID定义
enum
{
	//MyTechFrame
	ID_NAVIGATE_BEGIN = wxID_HIGHEST + 1,
	ID_NAVIGATE_HOME,
	ID_NAVIGATE_BACKWARD,
	ID_NAVIGATE_FORWARD,
	ID_NAVIGATE_UP,
	ID_NAVIGATE_DOWN,
	ID_NAVIGATE_FIND,
	ID_NAVIGATE_NEW,
	ID_NAVIGATE_GOTO,
	ID_NAVIGATE_GOTO_MAX = ID_NAVIGATE_GOTO + 100,
	ID_NAVIGATE_END,
	ID_MARKET_BEGIN,
	ID_MARKET_SELF,
	ID_MARKET_SELF_MAX = ID_MARKET_SELF + 8,
	ID_MARKET_MAIN,
	ID_MARKET_ALL,
	ID_MARKET_PRODUCT,
	ID_MARKET_PRODUCT_MAX = ID_MARKET_PRODUCT + 1024,
	ID_MARKET_EXCHANGE,
	ID_MARKET_EXCHANGE_MAX = ID_MARKET_EXCHANGE + 32,
	ID_MARKET_EXCHANGE_PRODUCT,
	ID_MARKET_EXCHANGE_PRODUCT_MAX = ID_MARKET_EXCHANGE_PRODUCT + 1024,
	ID_MARKET_SUBSCRIBE,
	ID_MARKET_ALL_SUBSCRIBE,
	ID_MARKET_NEW_FILTER,
	ID_MARKET_END,
	ID_FILTER,
	ID_FILTER_MAX = ID_FILTER + 32,
	ID_SORT,
	ID_SORT_QUICK,
	ID_SORT_QUICK_MAX = ID_SORT_QUICK + 6,
	ID_SORT_CALC,
	ID_SORT_MAX = ID_SORT + 32,
	ID_TEMPLATE_SAVE,
	ID_TEMPLATE_NEW_WINDOW,
	ID_TEMPLATE_BEGIN,
	ID_TEMPLATE_MAX = ID_TEMPLATE_BEGIN + 16,
	ID_SCRIPT_BEGIN,
	ID_SCRIPT_END = ID_SCRIPT_BEGIN + 100,
	ID_STRATEGY_BEGIN,
	ID_STRATEGY_END = ID_STRATEGY_BEGIN + 100,
	ID_TRADE_BEGIN,
	ID_TRADE_QUICK_BUY_FAK,
	ID_TRADE_QUICK_BUY_FOK,
	ID_TRADE_QUICK_SELL_FAK,
	ID_TRADE_QUICK_SELL_FOK,
	ID_TRADE_QUICK_CLOSE,
	ID_TRADE_POSITION,
	ID_TRADE_ORDER,
	ID_TRADE_TRADE,
	ID_TRADE_ADDUSER,
	ID_TRADE_REMOVEUSER,
	ID_TRADE_END,
	ID_DATA_RECORD,
	ID_DATA_LISTEN,
	ID_DATA_TEST,
	ID_DATA_TEST_RECORD,
	ID_DATA_TEST_RECORD_MAX = ID_DATA_TEST_RECORD + 7,
	ID_DATA_EDIT,
	ID_CALC_EDIT,
	ID_CALC_EDIT_NEW,
	ID_CALC_EDIT_NEW_MAX = ID_CALC_EDIT_NEW + CALC_TYPE_MAX,
	ID_CALC_EDIT_MAJOR,
	ID_CALC_EDIT_MAJOR_MAX = ID_CALC_EDIT_MAJOR + 100,
	ID_CALC_EDIT_MINOR,
	ID_CALC_EDIT_MINOR_MAX = ID_CALC_EDIT_MINOR + 100,
	ID_CALC_EDIT_FILTER,
	ID_CALC_EDIT_FILTER_MAX = ID_CALC_EDIT_FILTER + 32,
	ID_CALC_EDIT_SORT,
	ID_CALC_EDIT_SORT_MAX = ID_CALC_EDIT_SORT + 32,
	ID_CALC_EDIT_SCRIPT,
	ID_CALC_EDIT_SCRIPT_MAX = ID_CALC_EDIT_SCRIPT + 100,
	ID_CALC_EDIT_STRATEGY,
	ID_CALC_EDIT_STRATEGY_MAX = ID_CALC_EDIT_STRATEGY + 100,
	ID_CALC_MONITOR,
	ID_CALC_MONITOR_MAX = ID_CALC_MONITOR + 16,
	ID_CALC_TRADEFLAG,
	ID_CALC_INPUT,
	ID_CALC_INPUT_MAX = ID_CALC_INPUT + 16,
	ID_TECHVIEW_BEGIN,
	ID_TECHVIEW_KLINE,
	ID_TECHVIEW_KLINE_MAX = ID_TECHVIEW_KLINE + CALC_KLINE_TYPE_MAX,
	ID_TECHVIEW_ADD_INDICATOR,
	ID_TECHVIEW_REMOVE_INDICATOR,
	ID_TECHVIEW_CYCLE,
	ID_TECHVIEW_CYCLE_MAX = ID_TECHVIEW_CYCLE + CYC_MAX,
	ID_TECHVIEW_TEMPLATE,
	ID_TECHVIEW_TEMPLATE_MAX = ID_TECHVIEW_TEMPLATE + 100,
	ID_TECHVIEW_MOVE_AUTO,
	ID_TECHVIEW_MOVE_SHIFT,
	ID_TECHVIEW_MOVE_LAST,
	ID_TECHVIEW_MOVE_BACK,
	ID_TECHVIEW_MOVE_FORWARD,
	ID_TECHVIEW_MOVE_FIRST,
	ID_TECHVIEW_DRAWLINE,
	ID_TECHVIEW_DRAWLINE_MAX = ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_TYPE_MAX,
	ID_TECHVIEW_ZOOM_IN,
	ID_TECHVIEW_ZOOM_OUT,
	ID_TECHVIEW_ZOOM_RESET,
	ID_TECHVIEW_END,
	ID_INFO_F10,
	ID_TOOL_CALC,
	ID_HELP_HOME,
	ID_HELP_DISCUSZ,
	ID_HELP_ABOUT,
	ID_CLEAR_SETTINGS,
	ID_CLEAR_DATA,
	//MyUserView
	wxID_BTN_MMX,
	wxID_CMB_USER,
	wxID_CTRL_PRICE_TYPE,
	wxID_CTRL_PRICE,
	wxID_CTRL_VOLUME_TYPE,
	wxID_CTRL_VOLUME,
	wxID_BTN_BUY,
	wxID_BTN_SELL,
	wxID_BTN_CANCEL,
	wxID_BTN_CLOSE,
	wxID_BTN_QUICK_BUY,
	wxID_BTN_QUICK_SELL,
	wxID_BTN_QUICK_BUY_CLOSE,
	wxID_BTN_QUICK_SELL_CLOSE,
	wxID_BTN_CLOSE_ALL,
	wxID_BTN_CANCEL_ALL,
	wxID_BTN_LOCK_TRADE,
};

// -- MyCodeView --

class MyCodeView
	: public zqdb::BaseViewT<MyCodeView>
{
	typedef MyCodeView This;
public:
	typedef zqdb::BaseViewT<MyCodeView> Base;
protected:
	wxTextCtrl* ctrl_text_ = nullptr;
	wxDataViewCtrl* ctrl_list_ = nullptr;
	MyCodeViewListRenderer* ctrl_list_render_ = nullptr;
	wxObjectDataPtr<MyCodeViewListModel> ctrl_list_model_;
	std::pair<wxString, zqdb::Calc::Filter> pr_filter_;
	HZQDB sel_ = nullptr;
public:
	MyCodeView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	virtual ~MyCodeView();

	//bool IsDispOk() const { return Base::IsDispOk(); }
	bool IsShowAll() const;
	bool IsShowFilter() const;
	bool IsShowSearch() const;
	bool IsShowSearch(const wxString& key) const;
	bool IsTextSearching() const;

	int FilterEvent(wxEvent& event);

	HZQDB All();
	void Select(HZQDB h, bool bEnsureVisible = true);
	void Redo();
	void Goto(HZQDB h);
	void ShowKey(const wxString& key);
	void ShowFilter(const wxString& key, const zqdb::Calc::Filter& filter);
	wxString GetKey(zqdb::Calc::Filter& filter);
	void Up();
	void Down();

	int IsSort(MY_CODE_SORT_TYPE* type = nullptr, size_t* secs = nullptr);
	void Sort();
	void SortByZD(MY_CODE_SORT_TYPE type, size_t duration, int sort);
	void SortByField(MDB_FIELD& field, int sort);
	void SortByCalc(const zqdb::Calc::Sort& calc, int sort);

	void OnSkinInfoChanged();
	void OnHandleChanged();

	void OnNotifyEnable(HZQDB h);
	void OnNotifyDisable(HZQDB h);
	void OnNotifyAppend(HZQDB h);
	void OnNotifyUpdate(HZQDB h);

protected:
	//
	void UpdateAll();

	void DoSelect(const wxDataViewItem& item, size_t count = 0);
	bool DoSelect(HZQDB h, wxDataViewItem& item);
	void DoGoto(HZQDB h);
	void DoAction(int key);

	void OnSearch(wxCommandEvent& event);
	void OnSearchResult(wxCommandEvent& event);

	void Activate(const wxDataViewItem& item);
	void OnActivated(wxDataViewEvent &event);
	void OnSelChanged(wxDataViewEvent &event);
	void OnContextMenu(wxDataViewEvent &event);

	wxDECLARE_EVENT_TABLE();
};

// -- MyUserView --

class MyUserView
	: public zqdb::BaseViewT<MyUserView>
	, public zqdb::UserMap<MyUserView>
{
	typedef MyUserView This;
public:
	typedef zqdb::BaseViewT<MyUserView> Base;
	typedef zqdb::UserMap<MyUserView> UserBase;
protected:
	wxSize best_size_;
	wxButton* btn_mmx_ = nullptr;
	bool mmx_ = false;
	//zqdb::Rect rcUser; //用户
	wxComboBox* cmb_user_ = nullptr;
	wxComboBox* cmb_price_type_ = nullptr;
	wxSpinCtrlDouble* ctrl_price_ = nullptr;
	wxComboBox* cmb_volume_type_ = nullptr;
	wxSpinCtrlDouble* ctrl_volume_ = nullptr;
	wxButton* btn_quick_buy_ = nullptr;
	wxButton* btn_quick_sell_ = nullptr;
	wxButton* btn_quick_buy_close_ = nullptr;
	wxButton* btn_quick_sell_close_ = nullptr;
	wxButton* btn_cancel_all_ = nullptr;
	wxButton* btn_close_all_ = nullptr;
	wxStaticText* stc_info_ = nullptr;
	//wxButton* btn_lock_ = nullptr;
	//bool lock_ = false;
	std::shared_ptr<MyModule> my_module_;
	MyMiniView* my_view_ = nullptr; //当前用户视图
public:
	MyUserView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	virtual ~MyUserView();

	bool IsDispOk() const { return Base::IsDispOk() && UserBase::IsOk(); }

	void Goto(HZQDB user);

	void OnSkinInfoChanged();
	void OnHandleChanged();
	void OnUserChanged();
	
	void OnNotifyEnable(HZQDB h);
	void OnNotifyDisable(HZQDB h);
	void OnNotifyAppend(HZQDB h);
	void OnNotifyRemove(HZQDB h);
	void OnNotifyUpdate(HZQDB h);

protected:
	//
	void DoUpdateAll();
	void DoSelect(HZQDB user);
	void DoGoto(HZQDB user);
	void DoShow();
	void DoHide();
	void DoUpatePrice(bool set = false);

	void Draw(wxDC& dc);

	void OnErase(wxEraseEvent &event);
	void OnPaint(wxPaintEvent &event);
	void OnTimer(wxTimerEvent& event);
	void OnBtnMMX(wxCommandEvent& event);
	void OnCmbUserUpdate(wxCommandEvent& event);
	std::tuple<char, double> GetPrice(bool set = false);
	double GetVolume();
	void OnCmbPriceTypeUpdate(wxCommandEvent& event);
	void OnCmbVolumeTypeUpdate(wxCommandEvent& event);
	bool CheckParams();
	void OrderSend(char direction);
	void OnBtnQuickBuy(wxCommandEvent& event);
	void OnBtnQuickSell(wxCommandEvent& event);
	void OrderClose(char direction);
	void OnBtnQuickBuyClose(wxCommandEvent& event);
	void OnBtnQuickSellClose(wxCommandEvent& event);
	void OnBtnCancelAll(wxCommandEvent& event);
	void OnBtnCloseAll(wxCommandEvent& event);
	void OnBtnLockTrade(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

// -- MyStatusBar --

class MyStatusBar
	: public zqdb::BaseViewT<MyStatusBar, wxStatusBar>
	, public zqdb::UserMap<MyStatusBar>
{
	typedef MyStatusBar This;
public:
	typedef zqdb::BaseViewT<MyStatusBar, wxStatusBar> Base;
	typedef zqdb::UserMap<MyStatusBar> UserBase;
protected:
	zqdb::AllExchange allexchange_;
public:
	MyStatusBar(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	virtual ~MyStatusBar();

	bool IsDispOk() const { return Base::IsDispOk() && UserBase::IsOk(); }

	void OnSkinInfoChanged();
	void OnHandleChanged();
	void OnUserChanged();

	void OnNotifyEnable(HZQDB h);
	void OnNotifyDisable(HZQDB h);
	void OnNotifyAppend(HZQDB h);
	void OnNotifyRemove(HZQDB h);
	void OnNotifyUpdate(HZQDB h);

protected:
	//
	void Reset();
	void Update();
	// event handlers
	void OnErase(wxEraseEvent &event);
	void OnPaint(wxPaintEvent &event);
	void OnTimer(wxTimerEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
	wxDECLARE_EVENT_TABLE();
};

// -- frame --

class MyTechFrame : public MyBaseFrame
	//, public zqdb::SkinMap<MyTechFrame, SkinInfo>
	, public zqdb::TechContainerMap<MyTechFrame, zqdb::TechContainerInfo>
	//, public zqdb::UserMap<MyTechFrame>
	, public zqdb::NotifyMap<MyTechFrame>
{
	typedef MyBaseFrame Base;
	//typedef zqdb::SkinMap<MyTechFrame, SkinInfo> SkinBase;
	typedef zqdb::TechContainerMap<MyTechFrame, zqdb::TechContainerInfo> ContainerMap;
	//typedef zqdb::UserMap<MyTechFrame> UserBase;
	typedef zqdb::NotifyMap<MyTechFrame> NotifyBase;
protected:
	zqdb::Calc::AllFunc all_filter_func_;
	size_t cur_filter_func_ = 0;
	zqdb::Calc::AllFunc all_sort_func_;
	size_t cur_sort_func_ = 0;
	zqdb::Calc::AllFunc all_script_func_;
	void AddFirstPages();
	void AddLastPages();
	void InnerUpdateFilter(const zqdb::AllExchange& allexchange);
	void InnerSetCurFilter(size_t cur);
	wxRibbonButtonBar *filter_bar_ = nullptr;
	void InnerUpdateSort();
	void InnerSetCurSort(size_t cur);
	wxRibbonButtonBar *sort_bar_ = nullptr;
	wxRibbonToolBar *tech_bar_ = nullptr;
	void InnerUpdateScript();
	wxRibbonButtonBar *script_bar_ = nullptr;
	//wxTextCtrl* m_logwindow;
	//wxToggleButton* m_togglePanels;
	std::vector<HZQDB> record_list_;
	size_t record_pos_ = 0;
	MyCodeView* code_view_ = nullptr;
	zqdb::TechView *tech_view_ = nullptr;
	//wxString user_; //USER.BROKER
	//topview or rightview
	zqdb::TitleView* title_view_ = nullptr;
	zqdb::MmpView* mmp_view_ = nullptr;
	zqdb::TradeView* trade_view_ = nullptr;
	zqdb::InfoView* info_view_ = nullptr;
	zqdb::TickView* tick_view_ = nullptr;
	MyUserView* user_view_ = nullptr;
	MyScriptPropertyDlg* script_property_dlg_ = nullptr;
	wxScopedPtr<wxPreferencesEditor> calc_pref_editor_;
	MyStatusBar* status_bar_ = nullptr;
public:
	static MyTechFrame* GetFrameByChild(wxWindow* child);
    MyTechFrame(const char* xml, size_t xmlflag);
    ~MyTechFrame();

	int FilterEvent(wxEvent& event);

	virtual void OnSkinInfoChanged();
	virtual void OnInfoChanged();
	virtual void OnHandleChanged();
	virtual void OnUserChanged();
	virtual void OnNotifyConnect() { }
	virtual void OnNotifyDisconnect() { }
	virtual void OnNotifyStartListen() { }
	virtual void OnNotifyStopListen() { }
	virtual void OnNotifyEnable(HZQDB h);
	virtual void OnNotifyDisable(HZQDB h);
	virtual void OnNotifyAppend(HZQDB h);
	virtual void OnNotifyRemove(HZQDB h);
	virtual void OnNotifyUpdate(HZQDB h);

	void ShowTips(const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	void OnStatusBarMouseEvent(wxMouseEvent& event);

protected:
	//
	HZQDB All();
	void Goto(HZQDB h);
	void Goto(size_t pos);
	void Backward();
	void Forward();
	void ShowKey(const wxString& key);
	void ShowFilter(const wxString& key, const zqdb::Calc::Filter& filter);
	void UpdateFilter();

	void GotoUser(HZQDB user);

	int IsSort(MY_CODE_SORT_TYPE* type = nullptr, size_t* secs = nullptr);
	void Sort();
	void SortByZD(MY_CODE_SORT_TYPE type, size_t duration, int sort);
	void SortByField(MDB_FIELD& field, int sort);
	void SortByCalc(const zqdb::Calc::Sort& calc, int sort);

	//void OnNotify(wxCommandEvent& event);

	//Navigate
    void OnNavigateHome(wxRibbonButtonBarEvent& evt);
    void OnNavigateBackward(wxRibbonButtonBarEvent& evt);
    void OnNavigateBackwardDropdown(wxRibbonButtonBarEvent& evt);
	void OnNavigateBackwardUpdateUI(wxUpdateUIEvent& evt);
    void OnNavigateForward(wxRibbonButtonBarEvent& evt);
    void OnNavigateForwardDropdown(wxRibbonButtonBarEvent& evt);
	void OnNavigateForwardUpdateUI(wxUpdateUIEvent& evt);
	void OnNavigateGoto(wxCommandEvent& evt);
    void OnNavigateUp(wxRibbonButtonBarEvent& evt);
    void OnNavigateDown(wxRibbonButtonBarEvent& evt);
    void OnNavigateFind(wxRibbonButtonBarEvent& evt);
	//Market
	void OnMarketAll(wxRibbonButtonBarEvent& evt);
	void OnMarketAllDropdown(wxRibbonButtonBarEvent& evt);
	void OnMarketAllGoto(wxCommandEvent& evt);
	void OnMarketMain(wxRibbonButtonBarEvent& evt);
	void OnMarketMainDropdown(wxRibbonButtonBarEvent& evt);
	void OnMarketExchange(wxRibbonButtonBarEvent& evt);
	void OnExchangeDropdown(wxRibbonButtonBarEvent& evt); 
	void OnProductGoto(wxCommandEvent& evt);
	void OnMarketProduct(wxRibbonButtonBarEvent& evt);
	void OnMarketAllSubscribe(wxRibbonButtonBarEvent& evt);
	void OnMarketSubscribe(wxRibbonButtonBarEvent& evt);
	void OnMarketSubscribeDropdown(wxRibbonButtonBarEvent& evt);
	void OnMarketSubscribeGoto(wxCommandEvent& evt);
	//void OnMarketSelfSel(wxRibbonButtonBarEvent& evt);
	//void OnMarketSelfSelDropdown(wxRibbonButtonBarEvent& evt);
	//Filter
	void OnFilterNew(wxRibbonButtonBarEvent& evt);
	void OnFilter(wxRibbonButtonBarEvent& evt);
	void OnFilterDropdown(wxRibbonButtonBarEvent& evt);
	void OnFilterGoto(wxCommandEvent& evt);
	void OnFilterSetting(wxRibbonPanelEvent& evt);
	//Sort
	void Sort(int id);
	void OnSort(wxRibbonButtonBarEvent& evt);
	void OnSortDropdown(wxRibbonButtonBarEvent& evt);
	void OnSortGoto(wxCommandEvent& evt);
	void OnSortSetting(wxRibbonPanelEvent& evt);
	//TechView
	void SetTechKLine(CALC_KLINE_TYPE type);
	void OnTechKLine(wxRibbonToolBarEvent& evt);
	void SetTechCycle(PERIODTYPE cycle);
	void OnTechCycle(wxRibbonToolBarEvent& evt);
	void OnTechCycleDropdown(wxRibbonToolBarEvent& evt);
	void OnTechCycleGoto(wxCommandEvent& evt);
	void OnTechMove(wxRibbonToolBarEvent& evt);
	void DoDrawline(CALC_DRAWLINE_TYPE type);
	void OnTechDrawline(wxRibbonToolBarEvent& evt);
	void OnTechDrawlineDropdown(wxRibbonToolBarEvent& evt);
	void OnTechDrawlineGoto(wxCommandEvent& evt);
	void OnTechZoom(wxRibbonToolBarEvent& evt);
	void OnTechSetting(wxRibbonPanelEvent& evt);
	//Script
	void RunScript(HZQDB h);
	void OnScript(wxRibbonButtonBarEvent& evt);
	void OnScriptDropdown(wxRibbonButtonBarEvent& evt);
	void OnScriptGoto(wxCommandEvent& evt);
	void OnScriptSetting(wxRibbonPanelEvent& evt);
	//Other
	void OnClearSettings(wxRibbonButtonBarEvent& evt);
	void OnClearData(wxRibbonButtonBarEvent& evt);

	void OnTimer(wxTimerEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

