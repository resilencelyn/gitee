#pragma once

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/textctrl.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"
#include "wx/sizer.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/dcbuffer.h"
#include "wx/colordlg.h"
#include "wx/artprov.h"
#include "wx/combobox.h"
#include "wx/tglbtn.h"
#include "wx/wrapsizer.h"

#include <view.h>
#include <XUtil/XThread.hpp>
#include "mydatamodel.h"
#include "mydataview.h"
#include "mymodulemgr.h"

class MyStrategyFrame;

// -- MyStrategyCodeView --

class MyStrategyCodeView
	: public zqdb::BaseViewT<MyStrategyCodeView>
{
	typedef MyStrategyCodeView This;
public:
	typedef zqdb::BaseViewT<MyStrategyCodeView> Base;
protected:
	wxTextCtrl* ctrl_text_ = nullptr;
	wxDataViewCtrl* ctrl_list_ = nullptr;
	MyCodeViewListRenderer* ctrl_list_render_ = nullptr;
	wxObjectDataPtr<MyCodeViewListModel> ctrl_list_model_;
public:
	MyStrategyCodeView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	virtual ~MyStrategyCodeView();

	//bool IsDispOk() const { return Base::IsDispOk(); }
	bool IsShowAll() const;

	int FilterEvent(wxEvent& event);

	void Select(HZQDB h);
	void Goto(HZQDB h);
	void Goto(const wxString& key);
	void Up();
	void Down();

	void OnSkinInfoChanged();
	void OnHandleChanged();

	void OnNotifyEnable(HZQDB h);
	void OnNotifyDisable(HZQDB h);
	void OnNotifyUpdate(HZQDB h);

protected:
	//
	void DoSelect(const wxDataViewItem& item);
	bool DoSelect(HZQDB h, wxDataViewItem& item);
	void DoGoto(HZQDB h);
	void DoAction(int key);

	void OnSearch(wxCommandEvent& event);
	void OnSearchResult(wxCommandEvent& event);

	void Activate(const wxDataViewItem& item);
	void OnActivated(wxDataViewEvent &event);

	wxDECLARE_EVENT_TABLE();
};

// -- MyStrategyUserView --

class MyStrategyUserView
	: public zqdb::BaseViewT<MyStrategyUserView>
	, public zqdb::UserMap<MyStrategyUserView>
{
	typedef MyStrategyUserView This;
public:
	typedef zqdb::BaseViewT<MyStrategyUserView> Base;
	typedef zqdb::UserMap<MyStrategyUserView> UserBase;
protected:
	wxSize best_size_;
	enum IDS
	{
		wxID_BTN_MMX = wxID_HIGHEST + 1,
		wxID_BTN_BUY,
		wxID_BTN_SELL,
		wxID_BTN_CLOSE,
	};
	wxButton* btn_mmx_ = nullptr;
	wxComboBox* cmb_user_ = nullptr;
	wxButton* btn_buy_ = nullptr;
	wxButton* btn_sell_ = nullptr;
	wxButton* btn_close_ = nullptr;
	wxStaticText* stc_info_ = nullptr;
	//zqdb::Rect rcUser; //用户
	MyMiniView* my_view_ = nullptr; //当前用户视图
public:
	MyStrategyUserView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
	virtual ~MyStrategyUserView();

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
	void DoGoto(HZQDB user);

	void Draw(wxDC& dc);

	void OnErase(wxEraseEvent &event);
	void OnPaint(wxPaintEvent &event);
	void OnBtnMMX(wxCommandEvent& event);
	void OnBtnBuy(wxCommandEvent& event);
	void OnBtnSell(wxCommandEvent& event);
	void OnBtnClose(wxCommandEvent& event);
	void OnCmbUserUpdate(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

// -- frame --

class MyStrategyFrame : public wxFrame
	, public zqdb::SkinMap<MyStrategyFrame, SkinInfo>
	, public zqdb::TechContainerMap<MyStrategyFrame, zqdb::TechContainerInfo>
	//, public zqdb::UserMap<MyStrategyFrame>
	, public zqdb::NotifyMap<MyStrategyFrame>
{
	typedef wxFrame Base;
	typedef zqdb::SkinMap<MyStrategyFrame, SkinInfo> SkinBase;
	typedef zqdb::TechContainerMap<MyStrategyFrame, zqdb::TechContainerInfo> ContainerMap;
	//typedef zqdb::UserMap<MyStrategyFrame> UserBase;
	typedef zqdb::NotifyMap<MyStrategyFrame> NotifyBase;
protected:
	wxRibbonBar* m_ribbon;
	void AddStartPage();
	size_t AddExchangePage(HZQDB h, size_t product_id);
	void AddFuture();
	void AddOption();
	void AddCalcPage();
	void AddNewPage();
	//wxTextCtrl* m_logwindow;
	//wxToggleButton* m_togglePanels;

	std::vector<HZQDB> record_list_;
	size_t record_pos_ = 0;
	MyStrategyCodeView* code_view_ = nullptr;
	zqdb::TechView *tech_view_ = nullptr;
	//wxString user_; //USER.BROKER
	//topview or rightview
	zqdb::TitleView* title_view_ = nullptr;
	zqdb::MmpView* mmp_view_ = nullptr;
	zqdb::TradeView* trade_view_ = nullptr;
	zqdb::InfoView* info_view_ = nullptr;
	zqdb::TickView* tick_view_ = nullptr;
	MyStrategyUserView* user_view_ = nullptr;
	wxTimer timer_;
public:
	static MyStrategyFrame* GetFrameByChild(wxWindow* child);
    MyStrategyFrame(const char* xml, size_t xmlflag);
    ~MyStrategyFrame();

	int FilterEvent(wxEvent& event);

	void OnSkinInfoChanged();
	void OnInfoChanged();
	void OnHandleChanged();
	void OnUserChanged();
	void OnNotifyEnable(HZQDB h);
	void OnNotifyDisable(HZQDB h);
	void OnNotifyAppend(HZQDB h);
	void OnNotifyRemove(HZQDB h);
	void OnNotifyUpdate(HZQDB h);
	void OnStatusBarMouseEvent(wxMouseEvent& event);

protected:
	//
	void Goto(HZQDB h);
	void Goto(const wxString& key);
	void Goto(size_t pos);
	void Backward();
	void Forward();

	void UpdateStatus();

	//void OnNotify(wxCommandEvent& event);

	//RibbonBar ID定义
    enum
    {
		ID_NAVIGATE_BEGIN = wxID_HIGHEST + 1,
		ID_NAVIGATE_HOME,
		ID_NAVIGATE_BACKWARD,
		ID_NAVIGATE_FORWARD,
		ID_NAVIGATE_UP,
		ID_NAVIGATE_DOWN,
		ID_NAVIGATE_FIND,
		ID_NAVIGATE_GOTO,
		ID_NAVIGATE_GOTO_MAX = ID_NAVIGATE_GOTO + 100,
		ID_NAVIGATE_END,
		ID_MARKET_BEGIN,
        ID_MARKET_SELF,
		ID_MARKET_SELF_MAX = ID_MARKET_SELF + 8,
		ID_MARKET_MAIN,
        ID_MARKET_ALL,
		ID_MARKET_EXCHANGE,
		ID_MARKET_EXCHANGE_MAX = ID_MARKET_EXCHANGE + 32,
        ID_MARKET_PRODUCT,
		ID_MARKET_PRODUCT_MAX = ID_MARKET_PRODUCT +10240,
		ID_MARKET_END,
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
		ID_TECHVIEW_BEGIN,
		ID_TECHVIEW_ADD_INDICATOR,
		ID_TECHVIEW_REMOVE_INDICATOR,
		ID_TECHVIEW_CYCLE,
		ID_TECHVIEW_CYCLE_MAX = ID_TECHVIEW_CYCLE + CYC_MAX,
		ID_TECHVIEW_TEMPLATE,
		ID_TECHVIEW_TEMPLATE_MAX = ID_TECHVIEW_TEMPLATE + 100,
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
    };

	void OnPageChanging(wxRibbonBarEvent& evt);
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
	void OnMarketMain(wxRibbonButtonBarEvent& evt);
	void OnMarketExchange(wxRibbonButtonBarEvent& evt);
	void OnExchangeDropdown(wxRibbonButtonBarEvent& evt); 
	void OnProductGoto(wxCommandEvent& evt);
	void OnMarketProduct(wxRibbonButtonBarEvent& evt);

    void OnSizeEvent(wxSizeEvent& evt);
	void OnCloseEvent(wxCloseEvent& evt);
	void OnTimer(wxTimerEvent& evt);

    wxDECLARE_EVENT_TABLE();
};

