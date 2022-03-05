#include "myapp.h"
#include "mystrategyframe.h"
#include "mylogdlg.h"

MyStrategyCodeView::MyStrategyCodeView(wxWindow* parent, const char* xml, size_t xmlflag) : Base(parent, xml, xmlflag)
{
	wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

	ctrl_text_ = new wxTextCtrl(this, wxID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize
		, /*wxNO_BORDER | */wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
	sizerTop->Add(ctrl_text_, 0, wxEXPAND);

	//sizerTop->AddSpacer(2);

	ctrl_list_ = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition,
		wxSize(240, 300), wxNO_BORDER | wxDV_NO_HEADER | wxDV_VARIABLE_LINE_HEIGHT);
	ctrl_list_->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

	ctrl_list_model_ = new MyCodeViewListModel;
	ctrl_list_->AssociateModel(ctrl_list_model_.get());

	ctrl_list_render_ = new MyCodeViewListRenderer();
	auto code_col = new wxDataViewColumn("Code", ctrl_list_render_, MyCodeViewListModel::Col_Code
		, FromDIP(220), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
	ctrl_list_->AppendColumn(code_col);
	//ctrl_list_->AppendTextColumn("Code",
	//	SmartKBListModel::Col_Code);

	ctrl_list_model_->ShowAll();

	sizerTop->Add(ctrl_list_, 1, wxEXPAND);

	SetSizer(sizerTop);

	Bind(SMARTKB_SEARCH_RESULT_EVENT, &MyStrategyCodeView::OnSearchResult, this);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MyStrategyCodeView::OnActivated, this);
}

MyStrategyCodeView::~MyStrategyCodeView()
{

}

bool MyStrategyCodeView::IsShowAll() const
{
	return ctrl_list_model_->IsShowAll();
}

int MyStrategyCodeView::FilterEvent(wxEvent& event)
{
	const wxEventType t = event.GetEventType();
	auto obj = event.GetEventObject();
	if (obj) {
		if (t == wxEVT_KEY_DOWN) {
			if (obj->IsKindOf(wxCLASSINFO(wxTextCtrl))) {
				auto key_event = wxDynamicCast(&event, wxKeyEvent);
				auto key_code = key_event->GetKeyCode();
				if (obj == ctrl_text_) {
					switch (key_code)
					{
					case WXK_UP:
					case WXK_DOWN:
					case WXK_RETURN: {
						DoAction(key_code);
					} break;
					default:
						break;
					}
				}
			}
			else {
				auto key_event = wxDynamicCast(&event, wxKeyEvent);
				auto key_code = key_event->GetKeyCode();
				if (key_code >= 0 && key_code <= 255 && isalnum(key_code)) {
					ctrl_text_->SetLabel(wxString::Format(wxT("%c"), key_code));
					ctrl_text_->SetSelection(1, 1);
					ctrl_text_->SetFocus();
					return wxEventFilter::Event_Ignore;
				}
			}
		}
	}
	// Continue processing the event normally as well.
	return wxEventFilter::Event_Skip;
}

void MyStrategyCodeView::DoSelect(const wxDataViewItem& item)
{
	auto pos = ctrl_list_model_->GetRow(item);
	ctrl_list_->Select(item);
	ctrl_list_->EnsureVisible(item);
	/*auto lines = ctrl_list_->GetScrollLines(wxVERTICAL);
	auto offset = pos - lines;
	ctrl_list_->ScrollLines(offset);*/
	int count = ctrl_list_model_->GetCount();
	int range = ctrl_list_->GetScrollRange(wxVERTICAL);
	if (range <= 1) {
		wxGetApp().Post(500, [this, &item]() {
			DoSelect(item);
		});
		/*auto count_per_page = ctrl_list_->GetCountPerPage();
		auto real_count_per_page = count_per_page / 2; //这里代码显示占用了两行位置
		if (pos + real_count_per_page >= count) {
			ctrl_list_->EnsureVisible(ctrl_list_model_->GetItem(count - 1));
		}
		else {
			ctrl_list_->EnsureVisible(ctrl_list_model_->GetItem(pos + real_count_per_page));
		}*/
		//ctrl_list_->Refresh();
		//ctrl_list_->Update();
		//range = ctrl_list_->GetScrollRange(wxVERTICAL);
	}
	else {
		//range = ctrl_list_render_->GetSize().y * count;
		int scrollPos = pos * range / count;
		ctrl_list_->Scroll(0, scrollPos);
	}
	/*auto rc = ctrl_list_->GetItemRect(item);
	if (rc.IsEmpty()) {
	}
	else if (rc.y < 0) {
		ctrl_list_->LineUp();
	}*/
}

bool MyStrategyCodeView::DoSelect(HZQDB h, wxDataViewItem& item)
{
	auto pos = ctrl_list_model_->FindResult(h);
	if (pos >= 0) {
		item = ctrl_list_model_->GetItem(pos);
		DoSelect(item);
		return true;
	}
	return false;
}

void MyStrategyCodeView::DoGoto(HZQDB h)
{
	wxDataViewItem item;
	if(DoSelect(h, item)) {
		ctrl_list_->SetFocus();
		Activate(item);
	}
}

void MyStrategyCodeView::Select(HZQDB h)
{
	wxDataViewItem item;
	DoSelect(h, item);
}

void MyStrategyCodeView::Goto(HZQDB h)
{
	if (!ctrl_list_model_->IsShowAll()) {
		Goto(wxT("全部"));
		wxGetApp().Post(200, [this,h]() {
			Goto(h);
		});
	}
	else {
		DoGoto(h);
	}
}

void MyStrategyCodeView::Goto(const wxString& key)
{
	if (key == wxT("全部")) {
		ctrl_text_->Clear();
		//ctrl_list_model_->ShowAll();
	}
	else {
		//比如筛选”主力“
		ctrl_text_->SetLabel(key);
	}
	ctrl_list_->SetFocus();
}

void MyStrategyCodeView::Up()
{
	/*auto pos = GetCurItemPos();
	if (pos <= 0) {
		pos = GetItemCount() - 1;
	}
	else {
		pos = (pos - 1) % GetItemCount();
	}
	SetCurItemPos(pos);*/
}

void MyStrategyCodeView::Down()
{
	/*auto pos = GetCurItemPos();
	if (pos >= (GetItemCount() - 1)) {
		pos = 0;
	}
	else {
		pos = (pos + 1) % GetItemCount();
	}
	SetCurItemPos(pos);*/
}

void MyStrategyCodeView::OnSkinInfoChanged()
{
	//Base::OnSkinInfoChanged();
	auto skin_info_ptr = std::static_pointer_cast<SkinInfo>(skin_info_ptr_);
	ctrl_list_->SetBackgroundColour(skin_info_ptr->crCtrlBkgnd);
	ctrl_list_->SetForegroundColour(skin_info_ptr->crCtrlForgnd);
	ctrl_list_render_->SetSkinInfo(skin_info_ptr);
	wxSize cellSize = ctrl_list_render_->GetSize();
	ctrl_list_->SetScrollRate(0, cellSize.y);
}

void MyStrategyCodeView::OnHandleChanged()
{
	ASSERT(0); //这里暂时没有使用h_
	Base::OnHandleChanged();
}

void  MyStrategyCodeView::OnNotifyEnable(HZQDB h)
{
	ctrl_list_model_->UpdateAll();
}

void MyStrategyCodeView::OnNotifyDisable(HZQDB h)
{
	ctrl_list_model_->ClearAll();
}

void MyStrategyCodeView::OnNotifyUpdate(HZQDB h)
{
	if (h->type == ZQDB_HANDLE_TYPE_CODE) {
		auto pos = ctrl_list_model_->FindResult(h);
		auto item = ctrl_list_model_->GetItem(pos);
		auto rect = ctrl_list_->GetItemRect(item);
		ctrl_list_->RefreshRect(rect);
		/*auto count_per_page = ctrl_list_->GetCountPerPage();
		if (pos < count_per_page) {
		}*/
	}
}

void MyStrategyCodeView::DoAction(int key)
{
	switch (key)
	{
	case WXK_UP: {
		auto sel = ctrl_list_->GetSelection();
		if (sel) {
			auto sel_row = ctrl_list_model_->GetRow(sel);
			if (sel_row <= 0) {
				return;
			}
			auto item = ctrl_list_model_->GetItem(sel_row - 1);
			ctrl_list_->Select(item);
			ctrl_list_->EnsureVisible(item);
		}
		else {
			if (ctrl_list_model_->GetCount()) {
				ctrl_list_->Select(ctrl_list_model_->GetItem(0));
			}
		}
	} break;
	case WXK_DOWN: {
		auto sel = ctrl_list_->GetSelection();
		if (sel) {
			auto sel_row = ctrl_list_model_->GetRow(sel);
			if (sel_row >= (ctrl_list_model_->GetCount() - 1)) {
				return;
			}
			auto item = ctrl_list_model_->GetItem(sel_row + 1);
			ctrl_list_->Select(item);
			ctrl_list_->EnsureVisible(item);
		}
		else {
			if (ctrl_list_model_->GetCount()) {
				ctrl_list_->Select(ctrl_list_model_->GetItem(0));
			}
		}
	} break;
	case WXK_RETURN: {
		auto sel = ctrl_list_->GetSelection();
		Activate(sel);
	} break;
	default:
		break;
	}
}

wxBEGIN_EVENT_TABLE(MyStrategyCodeView, Base)
//EVT_ERASE_BACKGROUND(MyStrategyCodeView::OnErase)
//EVT_PAINT(MyStrategyCodeView::OnPaint)
EVT_TEXT(wxID_EDIT, MyStrategyCodeView::OnSearch)
wxEND_EVENT_TABLE()

void MyStrategyCodeView::OnSearch(wxCommandEvent& event)
{
	ctrl_list_model_->Search(this, event.GetString().Trim().MakeUpper());
}

void MyStrategyCodeView::OnSearchResult(wxCommandEvent& event)
{
	ctrl_list_model_->ShowResult();
	if (ctrl_list_model_->GetCount()) {
		if (ctrl_list_model_->IsShowAll()) {
			//DoGoto(h_);
		}
		else {
			if (ctrl_text_->FindFocus() == ctrl_text_) {
				SmartKBItem smkbi;
				if (ctrl_list_model_->GetResult(0, smkbi)) {
					Select((HZQDB)smkbi.Data);
				}
			}
			else {
				SmartKBItem smkbi;
				if (ctrl_list_model_->GetResult(0, smkbi)) {
					DoGoto((HZQDB)smkbi.Data);
				}
			}
		}
	}
}

void MyStrategyCodeView::Activate(const wxDataViewItem& item)
{
	SmartKBItem smkbi;
	if (ctrl_list_model_->GetResult(item, smkbi)) {
		MyStrategyFrame::GetFrameByChild(this)->Set((HZQDB)smkbi.Data);
	}
}

void MyStrategyCodeView::OnActivated(wxDataViewEvent &event)
{
	Activate(event.GetItem());
}

///

MyStrategyUserView::MyStrategyUserView(wxWindow* parent, const char* xml, size_t xmlflag) : Base(parent, xml, xmlflag)
{
	btn_mmx_ = new wxButton(this, wxID_BTN_MMX, "▼", wxDefaultPosition, wxSize(40,-1));
	cmb_user_ = new wxComboBox(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0, nullptr, wxCB_READONLY);
	btn_buy_ = new wxButton(this, wxID_BTN_BUY, "快买", wxDefaultPosition, wxSize(60, -1));
	btn_sell_ = new wxButton(this, wxID_BTN_SELL, "快卖", wxDefaultPosition, wxSize(60, -1));
	btn_close_ = new wxButton(this, wxID_BTN_CLOSE, "快平", wxDefaultPosition, wxSize(60, -1));
	stc_info_ = new wxStaticText(this, wxID_ANY, wxEmptyString);

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetBackgroundColour(btn_mmx_->GetBackgroundColour());

	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	wxSizer* sizer_user_h = new wxBoxSizer(wxHORIZONTAL);
	sizer_user_h->Add(btn_mmx_, 0, wxEXPAND);
	sizer_user_h->Add(cmb_user_, 0, wxEXPAND);
	sizer_user_h->Add(btn_buy_, 0, wxEXPAND);
	sizer_user_h->Add(btn_sell_, 0, wxEXPAND);
	sizer_user_h->Add(btn_close_, 0, wxEXPAND);
	sizer_user_h->Add(stc_info_, 1, wxEXPAND);
	topSizer->Add(sizer_user_h, 0, wxEXPAND);

	SetSizer(topSizer);

	DoGoto((HZQDB)cmb_user_->GetClientData(0));
}

MyStrategyUserView::~MyStrategyUserView()
{

}

void MyStrategyUserView::OnSkinInfoChanged()
{
	if (my_view_) {
		my_view_->SetSkinInfo(skin_info_ptr_);
	}
	//Base::OnSkinInfoChanged();
}

void MyStrategyUserView::OnHandleChanged()
{
	if (my_view_) {
		my_view_->SetHandle(h_);
	}
	Base::OnHandleChanged();
}

void MyStrategyUserView::OnUserChanged()
{
	if (my_view_) {
		my_view_->SetUser(user_);
	}
	MyStrategyFrame::GetFrameByChild(this)->SetUser(user_);
	UserBase::OnUserChanged();
}

void MyStrategyUserView::OnNotifyEnable(HZQDB h)
{
	if (my_view_) {
		my_view_->OnNotifyEnable(h);
	}
}

void MyStrategyUserView::OnNotifyDisable(HZQDB h)
{
	if (my_view_) {
		my_view_->OnNotifyDisable(h);
	}
}

void MyStrategyUserView::OnNotifyAppend(HZQDB h) 
{
	if (my_view_) {
		my_view_->OnNotifyAppend(h);
	}
}

void MyStrategyUserView::OnNotifyRemove(HZQDB h) 
{
	if (my_view_) {
		my_view_->OnNotifyRemove(h);
	}
}

void MyStrategyUserView::OnNotifyUpdate(HZQDB h)
{
	if (my_view_) {
		my_view_->OnNotifyUpdate(h);
	}
}

void MyStrategyUserView::DoGoto(HZQDB user)
{
	Freeze();
	HZQDB hModule = ZQDBGetParent(user);
	if (hModule != ZQDBGetParent(user_)) {
		auto old_my_view = my_view_;
		my_view_ = nullptr;
		//显示user模块视图
		MyModule::Broadcast([this,hModule](std::shared_ptr<MyModule> module)->bool {
			if(hModule == *module) {
				my_view_ = module->NewMiniView(this);
				return true;
			}
			return false;
		});
		if (old_my_view) {
			GetSizer()->Replace(old_my_view, my_view_);
		}
		else if(my_view_) {
			GetSizer()->Add(my_view_, 1, wxEXPAND);
		}
		/*position_list_ = new wxDataViewCtrl(m_bookCtrl, wxID_ANY);
		position_list_model_ = new MyHZQDBListModel("./mytrader/positionlist.json", XUtil::XML_FLAG_JSON_FILE);
		position_list_->AssociateModel(position_list_model_.get());
		if (user_) {
		position_list_model_->Select(user_, ZQDB_HANDLE_TYPE_POSITION);
		auto& position_col_info = position_list_model_->GetColInfo();
		for (size_t i = 0, j = position_col_info.size(); i < j; i++)
		{
		auto& col_info = position_col_info[i];
		position_list_->AppendTextColumn(col_info.name, i);
		}
		}
		m_bookCtrl->AddPage(position_list_, wxT("持仓"), false);

		order_list_ = new wxDataViewCtrl(m_bookCtrl, wxID_ANY);
		order_list_model_ = new MyHZQDBListModel("./mytrader/orderlist.json", XUtil::XML_FLAG_JSON_FILE);
		order_list_->AssociateModel(order_list_model_.get());
		if (user_) {
		order_list_model_->Select(user_, ZQDB_HANDLE_TYPE_ORDER);
		auto& order_col_info = order_list_model_->GetColInfo();
		for (size_t i = 0, j = order_col_info.size(); i < j; i++)
		{
		auto& col_info = order_col_info[i];
		order_list_->AppendTextColumn(col_info.name, i);
		}
		}
		m_bookCtrl->AddPage(order_list_, wxT("委托"), true);
		//m_bookCtrl->AddPage(new wxTextCtrl(m_bookCtrl, wxID_ANY, wxEmptyString,
		//	wxDefaultPosition, wxDefaultSize,
		//	wxTE_MULTILINE | wxTE_READONLY), wxT("委托"), false);

		trade_list_ = new wxDataViewCtrl(m_bookCtrl, wxID_ANY);
		trade_list_model_ = new MyHZQDBListModel("./mytrader/tradelist.json", XUtil::XML_FLAG_JSON_FILE);
		trade_list_->AssociateModel(trade_list_model_.get());
		if (user_) {
		trade_list_model_->Select(user_, ZQDB_HANDLE_TYPE_TRADE);
		auto& trade_col_info = trade_list_model_->GetColInfo();
		for (size_t i = 0, j = trade_col_info.size(); i < j; i++)
		{
		auto& col_info = trade_col_info[i];
		trade_list_->AppendTextColumn(col_info.name, i);
		}
		}
		m_bookCtrl->AddPage(trade_list_, wxT("成交"), false);*/
	}

	//ShowView(position_list_);
	Layout();
	Thaw();

	SetUser(user);
}

void MyStrategyUserView::Draw(wxDC& dc)
{
}

wxBEGIN_EVENT_TABLE(MyStrategyUserView, Base)
EVT_ERASE_BACKGROUND(MyStrategyUserView::OnErase)
EVT_PAINT(MyStrategyUserView::OnPaint)
EVT_BUTTON(wxID_BTN_MMX, MyStrategyUserView::OnBtnMMX)
EVT_COMBOBOX(wxID_ANY, MyStrategyUserView::OnCmbUserUpdate)
wxEND_EVENT_TABLE()

void MyStrategyUserView::OnErase(wxEraseEvent &event)
{

}

void MyStrategyUserView::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	dc.Clear();
	if (IsDispOk()) {
		Draw(dc);
	}
}

void MyStrategyUserView::OnBtnMMX(wxCommandEvent& event)
{
	//Freeze();
	auto topSizer = GetSizer();
	if (wxT("▼") == btn_mmx_->GetLabel()) {
		//展开
		btn_mmx_->SetLabel(wxT("▲"));
		topSizer->Hide(1);
	}
	else {
		//收起
		btn_mmx_->SetLabel(wxT("▼"));
		topSizer->Show(1, true);
	}
	//topSizer->Fit(this);
	//topSizer->SetSizeHints(this);
	GetParent()->Layout();
	//Layout();
	//Thaw();
}

void MyStrategyUserView::OnBtnBuy(wxCommandEvent& event)
{
	if (!h_ || !user_) {
		return;
	}
	auto m = MyModule::GetByHandle(user_);
	if (m) {
		//m->OrderSend(user_, h_, DIRECTION_LONG, OFFSET_OPEN, ORDER_MARKET, 1, 0);
	}
}

void MyStrategyUserView::OnBtnSell(wxCommandEvent& event)
{
	if (!h_ || !user_) {
		return;
	}
	auto m = MyModule::GetByHandle(user_);
	if (m) {
		//m->OrderSend(user_, h_, DIRECTION_SHORT, OFFSET_OPEN, ORDER_MARKET, 1, 0);
	}
}

void MyStrategyUserView::OnBtnClose(wxCommandEvent& event)
{
	if (!h_ || !user_) {
		return;
	}
	auto m = MyModule::GetByHandle(user_);
	if (m) {
		zqdb::AllTableData allorder(STR_ZQDB_TABLE_ORDER, *m);
		for (auto h : allorder)
		{
			zqdb::ObjectT<tagPositionInfo> position(h);
			if (ZQDBGetCode(position->Code, position->Exchange) == h_) {
				//m->OrderClose(user_, h, ORDER_MARKET, 1, 0);
			}
		}
	}
}

void MyStrategyUserView::OnCmbUserUpdate(wxCommandEvent& event)
{
	auto user = (HZQDB)cmb_user_->GetClientData(event.GetSelection());
	if (user_ != user) {
		DoGoto(user);
	}
}

///

wxBEGIN_EVENT_TABLE(MyStrategyFrame, wxFrame)
EVT_RIBBONBAR_PAGE_CHANGING(wxID_ANY, MyStrategyFrame::OnPageChanging)
//Navigate
//EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_HOME, MyStrategyFrame::OnNavigateHome)
EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_BACKWARD, MyStrategyFrame::OnNavigateBackward)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_NAVIGATE_BACKWARD, MyStrategyFrame::OnNavigateBackwardDropdown)
EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_FORWARD, MyStrategyFrame::OnNavigateForward)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_NAVIGATE_FORWARD, MyStrategyFrame::OnNavigateForwardDropdown)
EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_ALL, MyStrategyFrame::MyStrategyFrame::OnMarketAll)
EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_MAIN, MyStrategyFrame::MyStrategyFrame::OnMarketMain)
EVT_UPDATE_UI(ID_NAVIGATE_BACKWARD, MyStrategyFrame::OnNavigateBackwardUpdateUI)
EVT_UPDATE_UI(ID_NAVIGATE_FORWARD, MyStrategyFrame::OnNavigateForwardUpdateUI)

//EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_UP, MyStrategyFrame::OnNavigateUp)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_DOWN, MyStrategyFrame::OnNavigateDown)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_FIND, MyStrategyFrame::OnNavigateFind)
//Market
//EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_SELF, MyStrategyFrame::OnMarket)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_MAIN, MyStrategyFrame::OnMarketMain)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_ALL, MyStrategyFrame::OnMarketAll)
////Trade
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_BUY_FAK, MyStrategyFrame::OnTradeBuyFAK)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_BUY_FOK, MyStrategyFrame::OnTradeBuyFOK)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_SELL_FAK, MyStrategyFrame::OnTradeSellFAK)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_SELL_FOK, MyStrategyFrame::OnTradeSellFOK)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_CLOSE, MyStrategyFrame::OnTradeClose)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_POSITION, MyStrategyFrame::OnTradePosition)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_ORDER, MyStrategyFrame::OnTradeOrder)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_TRADE, MyStrategyFrame::OnTradeTrade)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_ADDUSER, MyStrategyFrame::OnTradeAddUser)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_REMOVEUSER, MyStrategyFrame::OnTradeRemoveUser)
EVT_SIZE(MyStrategyFrame::OnSizeEvent)
EVT_CLOSE(MyStrategyFrame::OnCloseEvent)
EVT_TIMER(wxID_ANY, MyStrategyFrame::OnTimer)
wxEND_EVENT_TABLE()
//
//wxBEGIN_EVENT_TABLE(MyStrategyFrame::EventHandler, wxEvtHandler)
//EVT_CHAR(MyStrategyFrame::EventHandler::OnChar)
//wxEND_EVENT_TABLE()
//
//MyStrategyFrame::EventHandler::EventHandler(MyStrategyFrame* frame)
//{
//	wxWindowList children = frame->GetChildren();
//	for (wxWindowListNode *i = children.GetFirst(); i; i = i->GetNext()) {
//		SetChildNextHandler(i->GetData());
//	}
//}
//
//void MyStrategyFrame::EventHandler::SetChildNextHandler(wxWindow* child)
//{
//	if (child) {
//		child->SetNextHandler(this);
//		wxWindowList children = child->GetChildren();
//		for (wxWindowListNode *i = children.GetFirst(); i; i = i->GetNext()) {
//			SetChildNextHandler(i->GetData());
//		}
//	}
//}
//
//void MyStrategyFrame::EventHandler::OnChar(wxKeyEvent &event)
//{
//	event.Skip();
//}

MyStrategyFrame* MyStrategyFrame::GetFrameByChild(wxWindow* child)
{
	if (child) {
		wxWindow* parent = nullptr;
		while (parent = child->GetParent()) {
			child = parent;
		}
		//return wxDynamicCast(child, MyStrategyFrame);
		return wxStaticCast(child, MyStrategyFrame);
	}
	return nullptr;
}

#include "transparent.xpm"
//#include "zoom_in.xpm"
//#include "zoom_out.xpm"

MyStrategyFrame::MyStrategyFrame(const char* xml, size_t xmlflag)
    : wxFrame(NULL, wxID_ANY, "wxRibbon Sample Application", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE)
	, timer_(this)
{
	CFG_FROM_XML(cfg, xml, xmlflag);

	wxCSConv utf8cv(wxFONTENCODING_UTF8);

	/*
	zqdb::AllUser alluser;
	for (size_t i = 0; i < alluser.size(); i++)
	{
		user_ = alluser[0];
		break;
	}*/
	user_view_ = new MyStrategyUserView(this);

#if wxUSE_STATUSBAR
	CreateStatusBar();
	GetStatusBar()->Bind(wxEVT_LEFT_UP, &MyStrategyFrame::OnStatusBarMouseEvent, this);
	UpdateStatus();

	//SetStatusText("no selection", 0);
#endif // wxUSE_STATUSBAR

	//开始
	//后退 上一个 | 搜索  市场 品种 ... | 模板1 模板2 2图 3图 4图  | 快买 持仓 | 官网
	//后退 下一个 | 主力 市场 ... 品种 ... |   最前 前移 后移 最后 放大 缩小  | 快卖 委托       | 关于
	//后退 前进进 | 自选                  | 周期  ... 划线 ... | 快平 成交 | 帮助
	//导航         行情               | 分析                  | 交易                | 其他
    m_ribbon = new wxRibbonBar(this,-1,wxDefaultPosition, wxDefaultSize, wxRIBBON_BAR_FLOW_HORIZONTAL
                                | wxRIBBON_BAR_SHOW_PAGE_LABELS
                                | wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS
                                | wxRIBBON_BAR_SHOW_TOGGLE_BUTTON
                                | wxRIBBON_BAR_SHOW_HELP_BUTTON
                                );

	{
		/*int metric_vals[wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE - wxRIBBON_ART_TAB_SEPARATION_SIZE + 1] = { 0 };
		for (size_t id = wxRIBBON_ART_TAB_SEPARATION_SIZE; id <= wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE; id++)
		{
			metric_vals[id - wxRIBBON_ART_TAB_SEPARATION_SIZE] = m_ribbon->GetArtProvider()->GetMetric(id);
		}
		for (size_t id = wxRIBBON_ART_TAB_SEPARATION_SIZE; id <= wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE; id++)
		{
			if (metric_vals[id - wxRIBBON_ART_TAB_SEPARATION_SIZE] < 10) {
				metric_vals[id - wxRIBBON_ART_TAB_SEPARATION_SIZE] = 10;
			}
			m_ribbon->GetArtProvider()->SetMetric(id, metric_vals[id - wxRIBBON_ART_TAB_SEPARATION_SIZE]);
		}*/
		AddStartPage();
		/*size_t product_id = ID_MARKET_PRODUCT;
		zqdb::AllExchange allexchange;
		for (size_t i = 0; i < allexchange.size(); i++)
		{
			product_id = AddExchangePage(allexchange[i], product_id);
		}
		assert(product_id < ID_MARKET_PRODUCT_MAX);
		Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyStrategyFrame::OnMarketProduct, this, ID_MARKET_PRODUCT, ID_MARKET_PRODUCT_MAX);*/
		//AddFuture();
		//AddOption();
		AddCalcPage();
		AddNewPage();
		//wxRibbonPanel *analysis_panel = new wxRibbonPanel(home, wxID_ANY, "分析",
		//	wxNullBitmap, wxDefaultPosition, wxDefaultSize,
		//	wxRIBBON_PANEL_NO_AUTO_MINIMISE |
		//	wxRIBBON_PANEL_EXT_BUTTON);
		//wxRibbonToolBar *analysis_bar = new wxRibbonToolBar(analysis_panel, ID_MAIN_TOOLBAR);
		////加减指标，技术模板，移动，缩放
		////周期，划线
		////加减指标
		//analysis_bar->AddDropdownTool(wxID_ADD, wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize(16, 15)));
		//analysis_bar->AddDropdownTool(wxID_REMOVE, wxArtProvider::GetBitmap(wxART_MINUS, wxART_OTHER, wxSize(16, 15)));
		//analysis_bar->AddSeparator();
		////周期
		//analysis_bar->AddToggleTool(wxID_JUSTIFY_LEFT, align_left_xpm);
		//analysis_bar->AddToggleTool(wxID_JUSTIFY_CENTER, align_center_xpm);
		//analysis_bar->AddToggleTool(wxID_JUSTIFY_RIGHT, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddSeparator();
		////技术模板
		//analysis_bar->AddHybridTool(wxID_NEW, wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(16, 15)));
		//analysis_bar->AddTool(wxID_OPEN, wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 15)), "Open something");
		//analysis_bar->AddTool(wxID_SAVE, wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, wxSize(16, 15)), "Save something");
		//analysis_bar->AddTool(wxID_SAVEAS, wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_OTHER, wxSize(16, 15)), "Save something as ...");
		//analysis_bar->EnableTool(wxID_OPEN, false);
		//analysis_bar->EnableTool(wxID_SAVE, false);
		//analysis_bar->EnableTool(wxID_SAVEAS, false);
		//analysis_bar->AddSeparator();
		////移动
		//analysis_bar->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_GOTO_FIRST, wxART_OTHER, wxSize(16, 15)));
		//analysis_bar->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_OTHER, wxSize(16, 15)));
		//analysis_bar->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_OTHER, wxSize(16, 15)));
		//analysis_bar->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_GOTO_LAST, wxART_OTHER, wxSize(16, 15)));
		//analysis_bar->AddSeparator();
		////划线
		//analysis_bar->AddToggleTool(wxID_JUSTIFY_LEFT, align_left_xpm);
		//analysis_bar->AddToggleTool(wxID_JUSTIFY_CENTER, align_center_xpm);
		//analysis_bar->AddToggleTool(wxID_JUSTIFY_RIGHT, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddToggleTool(ID_SMALL_BUTTON_1, align_right_xpm);
		//analysis_bar->AddSeparator();
		////缩放
		//analysis_bar->AddTool(wxID_ZOOM_IN, zoom_in_xpm);
		//analysis_bar->AddTool(wxID_ZOOM_OUT, zoom_out_xpm);
		//analysis_bar->AddSeparator();
		///*analysis_bar->AddHybridTool(ID_POSITION_LEFT, position_left_xpm,
		//	"Align ribbonbar vertically\non the left\nfor demonstration purposes");
		//analysis_bar->AddHybridTool(ID_POSITION_TOP, position_top_xpm,
		//	"Align the ribbonbar horizontally\nat the top\nfor demonstration purposes");
		//analysis_bar->AddSeparator();
		//analysis_bar->AddHybridTool(wxID_PRINT, wxArtProvider::GetBitmap(wxART_PRINT, wxART_OTHER, wxSize(16, 15)),
		//	"This is the Print button tooltip\ndemonstrating a tooltip");*/
		//analysis_bar->SetRows(2, 2);

		/*auto trade_panel = new wxRibbonPanel(home, wxID_ANY, "交易");
		wxRibbonButtonBar* trade_bar = new wxRibbonButtonBar(trade_panel);
		trade_bar->AddButton(ID_TRADE_QUICK_BUY_FAK, "快买FAK", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_BUY_FAK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_BUY_FAK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_QUICK_SELL_FAK, "快卖FAK", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_SELL_FAK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_SELL_FAK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_QUICK_CLOSE, "快平", wxBitmap(transparent_xpm));
		trade_bar->SetButtonTextMinWidth(ID_TRADE_QUICK_CLOSE, wxT("上一个"));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_CLOSE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_CLOSE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_QUICK_BUY_FOK, "快买FOK", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_BUY_FOK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_BUY_FOK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_QUICK_SELL_FOK, "快卖FOK", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_SELL_FOK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_SELL_FOK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_POSITION, "持仓", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_POSITION, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_POSITION, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_ORDER, "委托", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_ORDER, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_ORDER, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_TRADE, "成交", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_TRADE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_TRADE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);*/
		
		//wxRibbonPanel *sizer_panel = new wxRibbonPanel(home, wxID_ANY, "Panel with Sizer",
		//	wxNullBitmap, wxDefaultPosition, wxDefaultSize,
		//	wxRIBBON_PANEL_DEFAULT_STYLE);

		//wxArrayString as;
		//as.Add("Item 1 using a box sizer now");
		//as.Add("Item 2 using a box sizer now");
		//wxComboBox* sizer_panelcombo = new wxComboBox(sizer_panel, wxID_ANY,
		//	wxEmptyString,
		//	wxDefaultPosition, wxDefaultSize,
		//	as, wxCB_READONLY);

		//wxComboBox* sizer_panelcombo2 = new wxComboBox(sizer_panel, wxID_ANY,
		//	wxEmptyString,
		//	wxDefaultPosition, wxDefaultSize,
		//	as, wxCB_READONLY);

		//sizer_panelcombo->Select(0);
		//sizer_panelcombo2->Select(1);
		//sizer_panelcombo->SetMinSize(wxSize(150, 24));
		//sizer_panelcombo2->SetMinSize(wxSize(150, 24));

		//wxRibbonButtonBar* bar = new wxRibbonButtonBar(sizer_panel, wxID_ANY);
		//bar->AddButton(ID_BUTTON_XX, "xx", ribbon_xpm);
		//bar->AddButton(ID_BUTTON_XY, "xy", ribbon_xpm);
		//// This prevents ribbon buttons in panels with sizer from collapsing.
		//bar->SetButtonMinSizeClass(ID_BUTTON_XX, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		//bar->SetButtonMinSizeClass(ID_BUTTON_XY, wxRIBBON_BUTTONBAR_BUTTON_LARGE);

		//wxSizer* sizer_panelsizer_h = new wxBoxSizer(wxHORIZONTAL);
		//wxSizer* sizer_panelsizer_v = new wxBoxSizer(wxVERTICAL);
		//sizer_panelsizer_v->AddStretchSpacer(1);
		//sizer_panelsizer_v->Add(sizer_panelcombo, 0, wxALL | wxEXPAND, 2);
		//sizer_panelsizer_v->Add(sizer_panelcombo2, 0, wxALL | wxEXPAND, 2);
		//sizer_panelsizer_v->AddStretchSpacer(1);
		//sizer_panelsizer_h->Add(bar, 0, wxEXPAND);
		//sizer_panelsizer_h->Add(sizer_panelsizer_v, 0);
		//sizer_panel->SetSizer(sizer_panelsizer_h);

		//wxFont label_font(wxFontInfo(8).Light());
		//m_bitmap_creation_dc.SetFont(label_font);
	}
	/*{
		wxRibbonPage* market_page = new wxRibbonPage(m_ribbon, wxID_ANY, "行情");
		auto market_panel = new wxRibbonPanel(market_page, wxID_ANY, "行情");
		wxRibbonButtonBar* market_bar = new wxRibbonButtonBar(market_panel);
		market_bar->AddButton(ID_MARKET_SELF, "自选", wxBitmap(transparent_xpm), wxEmptyString);
		market_bar->SetButtonMaxSizeClass(ID_MARKET_SELF, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		market_bar->SetButtonMinSizeClass(ID_MARKET_SELF, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		market_bar->AddHybridButton(ID_MARKET_MAIN, "主力", wxBitmap(transparent_xpm));
		market_bar->SetButtonMaxSizeClass(ID_MARKET_MAIN, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		market_bar->SetButtonMinSizeClass(ID_MARKET_MAIN, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		market_bar->AddHybridButton(ID_MARKET_ALL, "全部", wxBitmap(transparent_xpm));
		market_bar->SetButtonMaxSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		market_bar->SetButtonMinSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		zqdb::AllExchange allexchange;
		for (size_t i = 0; i < allexchange.size(); i++)
		{
			zqdb::Exchange exchange(allexchange[i]);
			market_bar->AddHybridButton(ID_MARKET_EXCHANGE + i
				, utf8cv.cMB2WX(exchange->Name), wxBitmap(transparent_xpm));
			market_bar->SetButtonMaxSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
			market_bar->SetButtonMinSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		}
	}*/
	/*{
		wxRibbonPage* page = new wxRibbonPage(m_ribbon, wxID_ANY, "分析", empty_xpm);
		wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Button bar manipulation", ribbon_xpm);
		m_mutable_button_bar = new wxRibbonButtonBar(panel, wxID_ANY);
		m_mutable_button_bar->AddButton(ID_PLUS_MINUS, "+/-",
			wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize(24, 24)));
		m_plus_minus_state = false;
		m_mutable_button_bar->AddButton(ID_CHANGE_LABEL, "short", ribbon_xpm);
		m_mutable_button_bar->SetButtonTextMinWidth(ID_CHANGE_LABEL, "some long text");
		m_change_label_state = false;

		panel = new wxRibbonPanel(page, wxID_ANY, "Always medium buttons", ribbon_xpm);
		wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
		bar->AddButton(ID_SMALL_BUTTON_1, "Button 1", ribbon_xpm);
		bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_1, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		bar->AddButton(ID_SMALL_BUTTON_2, "Button 2", ribbon_xpm);
		bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_2, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		bar->AddButton(ID_SMALL_BUTTON_3, "Button 3", ribbon_xpm);
		bar->AddButton(ID_SMALL_BUTTON_4, "Button 4", ribbon_xpm);
		bar->AddButton(ID_SMALL_BUTTON_5, "Button 5", ribbon_xpm);
		bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_5, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		bar->AddButton(ID_SMALL_BUTTON_6, "Button 6", ribbon_xpm);
		bar->SetButtonMaxSizeClass(ID_SMALL_BUTTON_6, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	}*/
	/*{
		wxRibbonPage* trade_page = new wxRibbonPage(m_ribbon, wxID_ANY, "交易");
		auto trade_panel = new wxRibbonPanel(trade_page, wxID_ANY, "交易");
		wxRibbonButtonBar* trade_bar = new wxRibbonButtonBar(trade_panel);
		trade_bar->AddButton(ID_TRADE_QUICK_BUY_FAK, "快买FAK", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_BUY_FAK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_BUY_FAK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_QUICK_SELL_FAK, "快卖FAK", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_SELL_FAK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_SELL_FAK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_QUICK_CLOSE, "快平", wxBitmap(transparent_xpm));
		trade_bar->SetButtonTextMinWidth(ID_TRADE_QUICK_CLOSE, wxT("上一个"));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_CLOSE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_CLOSE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_QUICK_BUY_FOK, "快买FOK", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_BUY_FOK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_BUY_FOK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_QUICK_SELL_FOK, "快卖FOK", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_QUICK_SELL_FOK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_QUICK_SELL_FOK, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_POSITION, "持仓", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_POSITION, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_POSITION, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_ORDER, "委托", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_ORDER, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_ORDER, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->AddButton(ID_TRADE_TRADE, "成交", wxBitmap(transparent_xpm));
		trade_bar->SetButtonMaxSizeClass(ID_TRADE_TRADE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		trade_bar->SetButtonMinSizeClass(ID_TRADE_TRADE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	}*/
	{
        /*wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, "帮助");
        m_ribbon->GetArtProvider()->GetColourScheme(&m_default_primary,
            &m_default_secondary, &m_default_tertiary);
        wxRibbonPanel *provider_panel = new wxRibbonPanel(scheme, wxID_ANY,
            "Art", wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        wxRibbonButtonBar *provider_bar = new wxRibbonButtonBar(provider_panel, wxID_ANY);
        provider_bar->AddButton(ID_DEFAULT_PROVIDER, "Default Provider",
            wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(32, 32)));
        provider_bar->AddButton(ID_AUI_PROVIDER, "AUI Provider", aui_style_xpm);
        provider_bar->AddButton(ID_MSW_PROVIDER, "MSW Provider", msw_style_xpm);
        wxRibbonPanel *primary_panel = new wxRibbonPanel(scheme, wxID_ANY,
            "Primary Colour", colours_xpm);
        m_primary_gallery = PopulateColoursPanel(primary_panel,
            m_default_primary, ID_PRIMARY_COLOUR);
        wxRibbonPanel *secondary_panel = new wxRibbonPanel(scheme, wxID_ANY,
            "Secondary Colour", colours_xpm);
        m_secondary_gallery = PopulateColoursPanel(secondary_panel,
            m_default_secondary, ID_SECONDARY_COLOUR);*/
    }
    //new wxRibbonPage(m_ribbon, wxID_ANY, "Empty Page", empty_xpm);
    //new wxRibbonPage(m_ribbon, wxID_ANY, "Highlight Page", empty_xpm);
    //m_ribbon->AddPageHighlight(m_ribbon->GetPageCount()-1);

    m_ribbon->Realize();

    //m_logwindow = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
    //    wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY |
    //    wxTE_LEFT | wxTE_BESTWRAP | wxBORDER_NONE);
	//m_logwindow->Show(false);

	code_view_ = new MyStrategyCodeView(this);

	/*auto opt_htv = cfg.get_child_optional("htmlview");
	if (opt_htv) {
	auto& cfg_htv = opt_htv.get();
	auto htv = new wxHtmlWindow(this);
	//htv->SetRelatedFrame(this, _("wxHTML Demo: '%s'"));
	#if wxUSE_STATUSBAR
	//htv->SetRelatedStatusBar(1);
	#endif // wxUSE_STATUSBAR
	htv->LoadPage(cfg_htv.get<std::string>("url"));
	}
	else */ {
		auto opt_tv = cfg.get_child_optional("techview");
		ASSERT(opt_tv);
		auto& cfg_tv = opt_tv.get();
		auto tv = new zqdb::TechView(this, (const char*)&cfg_tv, XUtil::XML_FLAG_PTREE);
		tv->SetSkinInfo(wxGetApp().GetSkinInfo());
		tech_view_ = tv;
		//tech_view_->SetScrollbars(10, 10, 100, 240);
	}

	//m_togglePanels = new wxToggleButton(this, wxID_ANY, "&Toggle panels");
	//m_togglePanels->SetValue(true);
	//m_togglePanels->Show(false);

	wxSizer *s = new wxBoxSizer(wxVERTICAL);

	s->Add(m_ribbon, 0, wxEXPAND);

	wxSizer *sh = new wxBoxSizer(wxHORIZONTAL);

	//sh->Add(m_logwindow, 0, wxEXPAND);
	sh->Add(code_view_, 0, wxEXPAND);
	sh->AddSpacer(1);

	/*auto opt_topview = cfg.get_child_optional("topview");
	if (opt_topview) {
		auto& cfg_topview = opt_topview.get();
		auto *sizer_topview = new wxBoxSizer(wxHORIZONTAL);
		auto opt_tradeview = cfg_topview.get_child_optional("tradeview");
		auto opt_infoview = cfg_topview.get_child_optional("infoview");
		if (opt_tradeview) {
			trade_view_ = new TradeView(this, (const char*)&opt_tradeview.get(), XUtil::XML_FLAG_PTREE);
			sizer_topview->Add(trade_view_, 0, wxEXPAND);
		}
		if (opt_infoview) {
			info_view_ = new InfoView(this, (const char*)&opt_infoview.get(), XUtil::XML_FLAG_PTREE);
			sizer_topview->Add(info_view_, 1, wxEXPAND);
		}
		else {
			info_view_ = new InfoView(this);
			sizer_topview->Add(info_view_, 1, wxEXPAND);
		}
		tech_sizer_->Add(sizer_topview, 0, wxEXPAND);
	}*/

	sh->Add(tech_view_, 1, wxEXPAND);

	//右侧视图
	/*sh->AddSplitter(1);
	//sh->AddSpacer(200);
	sh->Add(
	new wxButton(this, wxID_CANCEL, "Cancel"),
	1,
	wxEXPAND);*/
	auto opt_rightview = cfg.get_child_optional("rightview");
	if (opt_rightview) {
		bool add_spacer = false;
		auto& cfg_rightview = opt_rightview.get();
		auto *sizer_rightview = new wxBoxSizer(wxVERTICAL);
		auto opt_titleview = cfg_rightview.get_child_optional("titleview");
		auto opt_mmpview = cfg_rightview.get_child_optional("mmpview");
		auto opt_tradeview = cfg_rightview.get_child_optional("tradeview");
		auto opt_tickview = cfg_rightview.get_child_optional("tickview");
		if (opt_titleview) {
			if (add_spacer) {
				sizer_rightview->AddSpacer(1);
			}
			add_spacer = true;
			title_view_ = new zqdb::TitleView(this, (const char*)&opt_titleview.get(), XUtil::XML_FLAG_PTREE);
			sizer_rightview->Add(title_view_, 0, wxEXPAND);
		}
		if (opt_mmpview) {
			if (add_spacer) {
				sizer_rightview->AddSpacer(1);
			}
			add_spacer = true;
			mmp_view_ = new zqdb::MmpView(this, (const char*)&opt_mmpview.get(), XUtil::XML_FLAG_PTREE);
			sizer_rightview->Add(mmp_view_, 0, wxEXPAND);
		}
		if (opt_tradeview) {
			if (add_spacer) {
				sizer_rightview->AddSpacer(1);
			}
			add_spacer = true;
			trade_view_ = new zqdb::TradeView(this, (const char*)&opt_tradeview.get(), XUtil::XML_FLAG_PTREE);
			sizer_rightview->Add(trade_view_, 0, wxEXPAND);
		}
		if (opt_tickview) {
			if (add_spacer) {
				sizer_rightview->AddSpacer(1);
			}
			add_spacer = true;
			tick_view_ = new zqdb::TickView(this, (const char*)&opt_tickview.get(), XUtil::XML_FLAG_PTREE);
			sizer_rightview->Add(tick_view_, 1, wxEXPAND);
		}
		else {
			if (add_spacer) {
				sizer_rightview->AddSpacer(1);
			}
			add_spacer = true;
			tick_view_ = new zqdb::TickView(this);
			sizer_rightview->Add(tick_view_, 1, wxEXPAND);
		}
		sh->AddSpacer(1);
		sh->Add(sizer_rightview, 0, wxEXPAND);
	}

	s->Add(sh, 1, wxEXPAND);

	s->Add(user_view_, 0, wxEXPAND);

	SetSizer(s);

	Bind(wxEVT_MENU, &MyStrategyFrame::OnNavigateGoto, this, ID_NAVIGATE_GOTO, ID_NAVIGATE_GOTO_MAX);
	//Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MyStrategyFrame::OnMarketDropdown, this, ID_MARKET_BEGIN, ID_MARKET_END);
	Bind(wxEVT_MENU, &MyStrategyFrame::OnProductGoto, this, ID_MARKET_PRODUCT, ID_MARKET_PRODUCT_MAX);

	/*Bind(ZQDB_NOTIFY_ENABLE_EVENT, &MyStrategyFrame::OnNotify, this);
	Bind(ZQDB_NOTIFY_DISABLE_EVENT, &MyStrategyFrame::OnNotify, this);
	Bind(ZQDB_NOTIFY_APPEND_EVENT, &MyStrategyFrame::OnNotify, this);
	Bind(ZQDB_NOTIFY_REMOVE_EVENT, &MyStrategyFrame::OnNotify, this);
	Bind(ZQDB_NOTIFY_UPDATE_EVENT, &MyStrategyFrame::OnNotify, this);*/

	// test that event handlers pushed on top of MDI children do work (this
	// used to be broken, see #11225)
	//PushEventHandler(new EventHandler(this));
	timer_.Start(1000);
}

MyStrategyFrame::~MyStrategyFrame()
{
	//PopEventHandler(true);

	wxGetApp().ResetStrategyFrame(this);
}

void MyStrategyFrame::AddStartPage()
{
	zqdb::AllExchange allexchange;

	wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, "开始");

	auto navigate_panel = new wxRibbonPanel(home, wxID_ANY, "导航");
	wxRibbonButtonBar *navigate_bar = new wxRibbonButtonBar(navigate_panel);
	navigate_bar->AddHybridButton(ID_NAVIGATE_BACKWARD, "返回"
		, wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_OTHER, wxSize(24, 24)));
	navigate_bar->SetButtonTextMinWidth(ID_NAVIGATE_BACKWARD, wxT("上一个"));
	navigate_bar->SetButtonMaxSizeClass(ID_NAVIGATE_BACKWARD, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	navigate_bar->SetButtonMinSizeClass(ID_NAVIGATE_BACKWARD, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	navigate_bar->AddHybridButton(ID_NAVIGATE_FORWARD, "前进"
		, wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_OTHER, wxSize(24, 24)));
	navigate_bar->SetButtonTextMinWidth(ID_NAVIGATE_FORWARD, wxT("上一个"));
	navigate_bar->SetButtonMaxSizeClass(ID_NAVIGATE_FORWARD, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	navigate_bar->SetButtonMinSizeClass(ID_NAVIGATE_FORWARD, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	/*navigate_bar->AddHybridButton(ID_NAVIGATE_FIND, "搜索…"
	, wxArtProvider::GetBitmap(wxART_FIND, wxART_OTHER, wxSize(16, 16))
	, "键盘精灵搜索…,（ALT+V）快捷键键盘精灵搜索将创建新窗口…");
	navigate_bar->SetButtonTextMinWidth(ID_NAVIGATE_FIND, wxT("上一个"));
	navigate_bar->SetButtonMaxSizeClass(ID_NAVIGATE_FIND, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	navigate_bar->SetButtonMinSizeClass(ID_NAVIGATE_FIND, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	navigate_bar->AddButton(ID_NAVIGATE_UP, "上一个"
	, wxArtProvider::GetBitmap(wxART_GO_UP, wxART_OTHER, wxSize(16, 16))
	, "上一个代码");
	navigate_bar->SetButtonMaxSizeClass(ID_NAVIGATE_UP, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	navigate_bar->SetButtonMinSizeClass(ID_NAVIGATE_UP, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	navigate_bar->AddButton(ID_NAVIGATE_DOWN, "下一个"
	, wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_OTHER, wxSize(16, 16))
	, "下一个代码");
	navigate_bar->SetButtonMaxSizeClass(ID_NAVIGATE_DOWN, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	navigate_bar->SetButtonMinSizeClass(ID_NAVIGATE_DOWN, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);*/

	auto fast_access_panel = new wxRibbonPanel(home, wxID_ANY, "快速访问");
	wxRibbonButtonBar* fast_access_bar = new wxRibbonButtonBar(fast_access_panel);
	fast_access_bar->AddButton(ID_MARKET_ALL, "全部", wxBitmap(transparent_xpm));
	fast_access_bar->SetButtonMaxSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_MARKET_MAIN, "主力", wxBitmap(transparent_xpm));
	fast_access_bar->SetButtonMaxSizeClass(ID_MARKET_MAIN, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_MARKET_MAIN, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_MARKET_SELF, "自选", wxBitmap(transparent_xpm), wxEmptyString);
	fast_access_bar->SetButtonMaxSizeClass(ID_MARKET_SELF, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_MARKET_SELF, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	/*fast_access_bar->AddButton(ID_TRADE_POSITION, "持仓", wxBitmap(transparent_xpm));
	fast_access_bar->SetButtonMaxSizeClass(ID_TRADE_POSITION, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_TRADE_POSITION, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_TRADE_ORDER, "委托", wxBitmap(transparent_xpm));
	fast_access_bar->SetButtonMaxSizeClass(ID_TRADE_ORDER, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_TRADE_ORDER, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_TRADE_TRADE, "成交", wxBitmap(transparent_xpm));
	fast_access_bar->SetButtonMaxSizeClass(ID_TRADE_TRADE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_TRADE_TRADE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);*/
	//添加其他筛选脚本

	//auto sort_panel = new wxRibbonPanel(home, wxID_ANY, "排序");

	//auto market_panel = new wxRibbonPanel(home, wxID_ANY, "行情");
	//wxRibbonButtonBar* market_bar = new wxRibbonButtonBar(market_panel);
	int product_id = ID_MARKET_PRODUCT;
	for (size_t i = 0; i < allexchange.size(); i++)
	{
		zqdb::Exchange exchange(allexchange[i]);
		//auto item = market_bar->AddButton(ID_MARKET_EXCHANGE + i, utf2wxString(exchange->Exchange), wxBitmap(transparent_xpm));
		//market_bar->SetItemClientData(item, allexchange[i]);
		//market_bar->SetButtonMaxSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		//market_bar->SetButtonMinSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		auto market_panel = new wxRibbonPanel(home, ID_MARKET_EXCHANGE + i, utf2wxString(exchange->Exchange));
		market_panel->SetToolTip(utf2wxString(exchange->Name));
		wxRibbonButtonBar* market_bar = new wxRibbonButtonBar(market_panel);
		//这里可以显示最活跃品种
		zqdb::AllProduct allproduct(allexchange[i]);
		/*std::sort(allproduct.begin(), allproduct.end(), [](HZQDB x, HZQDB y) {
			zqdb::Product xproduct(x);
			zqdb::Product yproduct(y);
			return strcmp(xproduct->Name, yproduct->Name) < 0;
		});*/
		for (size_t k = 0; k < allproduct.size() && k < 11; k++)
		{
			zqdb::Product product(allproduct[k]);
			auto item = market_bar->AddButton(product_id, utf2wxString(product->Name), wxBitmap(transparent_xpm));
			market_bar->SetItemClientData(item, allproduct[k]);
			market_bar->SetButtonMaxSizeClass(product_id, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
			market_bar->SetButtonMinSizeClass(product_id, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
			++product_id;
		}
		//这里显示市场下拉菜单，可以查看所有品种
		auto item = market_bar->AddDropdownButton(ID_MARKET_EXCHANGE + i
			, wxT("更多..."), wxBitmap(transparent_xpm));
		market_bar->SetItemClientData(item, allexchange[i]);
		market_bar->SetButtonMaxSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		market_bar->SetButtonMinSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	}
	ASSERT(product_id < ID_MARKET_PRODUCT_MAX);

	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyStrategyFrame::OnMarketExchange, this, ID_MARKET_EXCHANGE, ID_MARKET_EXCHANGE_MAX);
	Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MyStrategyFrame::OnExchangeDropdown, this, ID_MARKET_EXCHANGE, ID_MARKET_EXCHANGE_MAX);
	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyStrategyFrame::OnMarketProduct, this, ID_MARKET_PRODUCT, product_id);

	//auto script_panel = new wxRibbonPanel(home, wxID_ANY, "脚本");
}

size_t MyStrategyFrame::AddExchangePage(HZQDB h, size_t product_id)
{
	zqdb::Exchange exchange(h);
	auto strExchange = utf2wxString(exchange->Exchange);
	auto strName = wxT(" ") + utf2wxString(exchange->Name) + wxT(" ");
	wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, strExchange);

	auto market_panel = new wxRibbonPanel(home, ID_MARKET_EXCHANGE + 0, strName);
	wxRibbonButtonBar* market_bar = new wxRibbonButtonBar(market_panel);
	zqdb::AllProduct allproduct(h);
	/*std::sort(allproduct.begin(), allproduct.end(), [](HZQDB x, HZQDB y) {
		zqdb::Product xproduct(x);
		zqdb::Product yproduct(y);
		return strcmp(xproduct->Name, yproduct->Name) < 0;
	});*/
	for (size_t k = 0; k < allproduct.size(); k++)
	{
		zqdb::Product product(allproduct[k]);
		auto item = market_bar->AddButton(product_id, utf2wxString(product->Name), wxBitmap(transparent_xpm));
		market_bar->SetItemClientData(item, allproduct[k]);
		market_bar->SetButtonMaxSizeClass(product_id, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		market_bar->SetButtonMinSizeClass(product_id, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
		++product_id;
	}

	return product_id;
}

void MyStrategyFrame::AddFuture()
{
	wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, "期货");
}

void MyStrategyFrame::AddOption()
{
	wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, "期权");
}

void MyStrategyFrame::AddCalcPage()
{
	wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, "公式系统");
}

void MyStrategyFrame::AddNewPage()
{
	//wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, "新建");
}

int MyStrategyFrame::FilterEvent(wxEvent& event)
{
	if (code_view_) {
		int filter = code_view_->FilterEvent(event);
		if (wxEventFilter::Event_Skip != filter) {
			return filter;
		}
	}

	//const wxEventType t = event.GetEventType();
	//auto obj = event.GetEventObject();
	//if (obj == tech_view_) {
	//	if (t == wxEVT_MOUSEWHEEL) {
	//		auto mouse_event = wxDynamicCast(&event, wxMouseEvent);
	//		if (!tech_view_->IsAnyOperator()) {
	//			auto rotation = mouse_event->GetWheelRotation();
	//			auto delta = mouse_event->GetWheelDelta();
	//			auto inverted = mouse_event->IsWheelInverted();
	//			auto axis = mouse_event->GetWheelAxis();
	//			if (axis == wxMOUSE_WHEEL_VERTICAL) {
	//				if (rotation < 0) {
	//					Up();
	//					return wxEventFilter::Event_Ignore;
	//				}
	//				else if (rotation > 0) {
	//					Down();
	//					return wxEventFilter::Event_Ignore;
	//				}
	//			}
	//		}
	//	}
	//	else if (t == wxEVT_CHAR) {
	//		if (obj->IsKindOf(wxCLASSINFO(wxTextCtrlBase))) {
	//			
	//		}
	//		else {
	//			auto key_event = wxDynamicCast(&event, wxKeyEvent);
	//			auto key_code = key_event->GetKeyCode();
	//			switch (key_code)
	//			{
	//			/*case WXK_ESCAPE: {
	//				auto tv = (TechView*)GetParent();
	//				if (IsShowCrossCursor(disp_info_ptr_->show_cross_cursor)) {
	//					tv->HideCurrent();
	//					event.Skip();
	//				}
	//			} break;
	//			case WXK_HOME: {
	//				auto tv = (TechView*)GetParent();
	//				auto nBarCount = GetBarCount();
	//				tv->MoveTo(std::max<int>(0, nBarCount - (disp_info_ptr_->MaxCount - disp_info_ptr_->Shift)));
	//				if (IsShowCrossCursor(disp_info_ptr_->show_cross_cursor)) {
	//					tv->MoveCurrentTo(nBarCount - 1);
	//					event.Skip();
	//				}
	//			} break;
	//			case WXK_END: {
	//				auto tv = (TechView*)GetParent();
	//				tv->MoveTo(0);
	//				if (IsShowCrossCursor(disp_info_ptr_->show_cross_cursor)) {
	//					tv->MoveCurrentTo(0);
	//					event.Skip();
	//				}
	//			} break;*/
	//			case WXK_PAGEUP: {
	//				Up();
	//				return wxEventFilter::Event_Ignore;
	//			} break;
	//			case WXK_PAGEDOWN: {
	//				Down();
	//				return wxEventFilter::Event_Ignore;
	//			} break;
	//			//case WXK_LEFT: {
	//			//	auto tv = (TechView*)GetParent();
	//			//	if (!IsShowCrossCursor(disp_info_ptr_->show_cross_cursor)) {
	//			//		tv->MoveCurrentTo(disp_info_ptr_->Start);
	//			//	}
	//			//	else {
	//			//		/*if (disp_info_ptr_->Start <= 0 && disp_info_ptr_->Current <= 0) {
	//			//		RequestMore();
	//			//		}*/
	//			//		tv->MoveCurrent(-1);
	//			//	}
	//			//	event.Skip();
	//			//} break;
	//			//case WXK_RIGHT: {
	//			//	auto tv = (TechView*)GetParent();
	//			//	if (!IsShowCrossCursor(disp_info_ptr_->show_cross_cursor)) {
	//			//		tv->MoveCurrentTo(disp_info_ptr_->Start + disp_info_ptr_->Count - 1);
	//			//	}
	//			//	else {
	//			//		tv->MoveCurrent(1);
	//			//	}
	//			//	event.Skip();
	//			//} break;
	//			//case WXK_UP: {
	//			//	auto tv = (TechView*)GetParent();
	//			//	tv->Scale(1);
	//			//	tv->UpdateCurrent();
	//			//	event.Skip();
	//			//} break;
	//			//case WXK_DOWN: {
	//			//	auto tv = (TechView*)GetParent();
	//			//	tv->Scale(-1);
	//			//	tv->UpdateCurrent();
	//			//	event.Skip();
	//			//} break;
	//			default: {
	//				/*if (std::isalnum(key_code)) {
	//					auto dlg = wxGetApp().GetSmartKBDlg();
	//					if (dlg) {
	//						dlg->ShowFor(this, key_code);
	//						return wxEventFilter::Event_Ignore;
	//					}
	//				}*/
	//			} break;
	//			}
	//		}
	//	}
	//}
	// Continue processing the event normally as well.
	return wxEventFilter::Event_Skip;
}

void MyStrategyFrame::OnSkinInfoChanged()
{
	Freeze();
	//普通显示信息变化
	SetBackgroundColour(skin_info_ptr_->crXYLine);
	//SetBackgroundColour(skin_info_ptr_->crPrimary);
	SetForegroundColour(skin_info_ptr_->crTertiary);
	//wxColour primary, secondary, tertiary;
	//m_ribbon->SetBackgroundColour();
	/*m_ribbon->GetArtProvider()->GetColourScheme(&primary, &secondary, &tertiary);
#ifdef _DEBUG
	int r, g, b;
	r = primary.Red();
	g = primary.Green();
	b = primary.Blue();
	r = secondary.Red();
	g = secondary.Green();
	b = secondary.Blue();
	r = tertiary.Red();
	g = tertiary.Green();
	b = tertiary.Blue();
#endif*/
	if (skin_info_ptr_->artProvider) {
		m_ribbon->SetArtProvider(skin_info_ptr_->artProvider->Clone());
	}
	else {
		m_ribbon->GetArtProvider()->SetColourScheme(skin_info_ptr_->crPrimary, skin_info_ptr_->crSecondary, skin_info_ptr_->crTertiary);
		skin_info_ptr_->artProvider = m_ribbon->GetArtProvider()->Clone();
	}
	//
	if (code_view_) {
		code_view_->SetSkinInfo(skin_info_ptr_);
	}
	if (tech_view_) {
		tech_view_->SetSkinInfo(skin_info_ptr_);
	}
	if (title_view_) {
		title_view_->SetSkinInfo(skin_info_ptr_);
	}
	if (mmp_view_) {   
		mmp_view_->SetSkinInfo(skin_info_ptr_);
	}
	if (trade_view_) {
		trade_view_->SetSkinInfo(skin_info_ptr_);
	}
	if (info_view_) {
		info_view_->SetSkinInfo(skin_info_ptr_);
	}
	if (tick_view_) {
		tick_view_->SetSkinInfo(skin_info_ptr_);
	}
	if (user_view_) {
		user_view_->SetSkinInfo(skin_info_ptr_);
	}
	//m_togglePanels->SetBackgroundColour(skin_info_ptr_->crXYLine);
	/*Broadcast(this, [this](wxWindow* child) {
		if (child != tech_view_ && child != m_ribbon) {
			Broadcast(child, [this](wxWindow* child) {
				child->SetBackgroundColour(skin_info_ptr_->crBackgnd);
			}, true);
			child->SetBackgroundColour(skin_info_ptr_->crXYLine);
		}
	});*/
	auto statusbar = GetStatusBar();
	if (statusbar) {
		statusbar->SetBackgroundColour(skin_info_ptr_->crPrimary);
		statusbar->SetForegroundColour(skin_info_ptr_->crTertiary);
	}
	Layout();
	Thaw();
}

void MyStrategyFrame::OnInfoChanged()
{
	Goto(Get());
}

void MyStrategyFrame::OnHandleChanged()
{
	Freeze();
	auto h = Get();
	ASSERT(h && h->type == ZQDB_HANDLE_TYPE_CODE);
	zqdb::Code code(h);
	wxCSConv utf8cv(wxFONTENCODING_UTF8);
	wxString title = wxString::Format(wxT("%s %s")
		, utf8cv.cMB2WX(code->Name).data()
		, utf8cv.cMB2WX(code->Code).data()
	);
	SetTitle(title);
	if (tech_view_) {
		tech_view_->SetHandle(h);
	}
	if (title_view_) {
		title_view_->SetHandle(h);
	}
	if (mmp_view_) {
		mmp_view_->SetHandle(h);
	}
	if (trade_view_) {
		trade_view_->SetHandle(h);
	}
	if (info_view_) {
		info_view_->SetHandle(h);
	}
	if (tick_view_) {
		tick_view_->SetHandle(h);
	}
	if (user_view_) {
		user_view_->SetHandle(h);
	}

	//前进后退时已经设置过了record_pos_，这里不用再记录了
	if (record_list_.empty() || record_list_[record_pos_] != h) {
		size_t i = 0, j = record_list_.size();
		for (; i < j; ++i)
		{
			if (record_list_[i] == h) {
				auto record = record_list_[i];
				size_t k = i;
				for (; (k + 1) < j; ++k) {
					record_list_[k] = record_list_[k + 1];
				}
				record_pos_ = k;
				record_list_[k] = record;
				break;
			}
		}
		if (i >= j) {
			record_pos_ = record_list_.size();
			record_list_.emplace_back(h);
		}
	}
	Layout();
	Thaw();
}

void MyStrategyFrame::OnUserChanged()
{
	if (trade_view_) {
		trade_view_->SetUser(GetUser());
	}
}

void MyStrategyFrame::Goto(HZQDB h)
{
	code_view_->Goto(h);
}

void MyStrategyFrame::Goto(const wxString& key)
{
	code_view_->Goto(key);
}

void MyStrategyFrame::Goto(size_t pos)
{
	if (pos < record_list_.size()) {
		record_pos_ = pos;
		auto record = record_list_[record_pos_];
		code_view_->Goto(record);
	}
}

void MyStrategyFrame::Backward()
{
	if (record_pos_ > 0 && !record_list_.empty()) {
		--record_pos_;
		auto record = record_list_[record_pos_];
		code_view_->Goto(record);
	}
}

void MyStrategyFrame::Forward()
{
	if ((record_pos_ + 1) < record_list_.size()) {
		++record_pos_;
		auto record = record_list_[record_pos_];
		code_view_->Goto(record);
	}
}

void MyStrategyFrame::UpdateStatus()
{
#if wxUSE_STATUSBAR
	static const std::map<char, wxString> connectStatusMap =
	{
		{STATUS_OFFLINE,wxString(L"离线")},
		{STATUS_CONNECT,wxString(L"连接中")},
		{STATUS_CONNECTED,wxString(L"已连接")},
		{STATUS_LOGGED,wxString(L"已登录")},
	};

	static const std::map<char, wxString> businessStatusMap =
	{
		{XMT_Status_BeforeTrading,wxString(L"开盘前")},
		{XMT_Status_NoTrading,wxString(L"非交易")},
		{XMT_Status_Continous,wxString(L"连续交易")},
		{XMT_Status_AuctionOrdering,wxString(L"集合竞价报单")},
		{XMT_Status_AuctionBalance,wxString(L"集合竞价价格平衡")},
		{XMT_Status_AuctionMatch,wxString(L"集合竞价撮合")},
		{XMT_Status_Closed,wxString(L"收盘")},
	};

	auto statusbar = GetStatusBar();
	if (statusbar)
	{
		wxCSConv utf8cv(wxFONTENCODING_UTF8);
		const int scale = 8;
		std::vector<wxString> statusInfos;
		std::vector<int> widths = { -1 };
		zqdb::AllExchange allexchange;
		for (size_t i = 0; i < allexchange.size(); i++)
		{
			zqdb::Exchange exchange(allexchange[i]);
			wxString name = utf8cv.cMB2WX(exchange->Exchange);

			auto iFindBusinessStatus = businessStatusMap.find(exchange->Status);
			wxString businessStatus = iFindBusinessStatus != businessStatusMap.end() ? iFindBusinessStatus->second : wxString(L"未知");

			auto iFindConnectStatus = connectStatusMap.find(ZQDBGetStatus(exchange->Exchange));
			wxString connectStaus = iFindConnectStatus != connectStatusMap.end() ? iFindConnectStatus->second : wxString(L"未知");

			wxString time;// = wxString::Format(L"%02d:%02d:%02d", XUtil::GetHour(exchange->Time), XUtil::GetMinute(exchange->Time), XUtil::GetSecond(exchange->Time));
			statusInfos.push_back(wxString::Format(L"%ls %ls %ls %ls", name.wc_str(), connectStaus.wc_str(), businessStatus.wc_str(), time.wc_str()));
			widths.push_back(statusInfos.back().size()* scale);
		}

		statusbar->SetFieldsCount(widths.size(), widths.data());
		for (size_t i = 0; i < statusInfos.size(); i++)
		{
			auto res = statusInfos[i];
			statusbar->SetStatusText(statusInfos[i], i + 1);
		}
	}
	return;
#endif // wxUSE_STATUSBAR
}

void MyStrategyFrame::OnNotifyEnable(HZQDB h)
{
	if (code_view_) {
		code_view_->OnNotifyEnable(h);
	}
	if (tech_view_) {
		tech_view_->OnNotifyEnable(h);
	}
	if (title_view_) {
		title_view_->OnNotifyEnable(h);
	}
	if (mmp_view_) {
		mmp_view_->OnNotifyEnable(h);
	}
	if (trade_view_) {
		trade_view_->OnNotifyEnable(h);
	}
	if (info_view_) {
		info_view_->OnNotifyEnable(h);
	}
	if (tick_view_) {
		tick_view_->OnNotifyEnable(h);
	}
	if (user_view_) {
		user_view_->OnNotifyEnable(h);
	}
}

void MyStrategyFrame::OnNotifyDisable(HZQDB h)
{
	if (code_view_) {
		code_view_->OnNotifyDisable(h);
	}
	if (tech_view_) {
		tech_view_->OnNotifyDisable(h);
	}
	if (title_view_) {
		title_view_->OnNotifyDisable(h);
	}
	if (mmp_view_) {
		mmp_view_->OnNotifyDisable(h);
	}
	if (trade_view_) {
		trade_view_->OnNotifyDisable(h);
	}
	if (info_view_) {
		info_view_->OnNotifyDisable(h);
	}
	if (tick_view_) {
		tick_view_->OnNotifyDisable(h);
	}
	if (user_view_) {
		user_view_->OnNotifyDisable(h);
	}
}

void MyStrategyFrame::OnNotifyAppend(HZQDB h)
{
	if (code_view_) {
		code_view_->OnNotifyAppend(h);
	}
	if (tech_view_) {
		tech_view_->OnNotifyAppend(h);
	}
	if (title_view_) {
		title_view_->OnNotifyAppend(h);
	}
	if (mmp_view_) {
		mmp_view_->OnNotifyAppend(h);
	}
	if (trade_view_) {
		trade_view_->OnNotifyAppend(h);
	}
	if (info_view_) {
		info_view_->OnNotifyAppend(h);
	}
	if (tick_view_) {
		tick_view_->OnNotifyAppend(h);
	}
	if (user_view_) {
		user_view_->OnNotifyAppend(h);
	}
}

void MyStrategyFrame::OnNotifyRemove(HZQDB h)
{
	if (code_view_) {
		code_view_->OnNotifyRemove(h);
	}
	if (tech_view_) {
		tech_view_->OnNotifyRemove(h);
	}
	if (title_view_) {
		title_view_->OnNotifyRemove(h);
	}
	if (mmp_view_) {
		mmp_view_->OnNotifyRemove(h);
	}
	if (trade_view_) {
		trade_view_->OnNotifyRemove(h);
	}
	if (info_view_) {
		info_view_->OnNotifyRemove(h);
	}
	if (tick_view_) {
		tick_view_->OnNotifyRemove(h);
	}
	if (user_view_) {
		user_view_->OnNotifyRemove(h);
	}
}

void MyStrategyFrame::OnNotifyUpdate(HZQDB h)
{
	if (code_view_) {
		code_view_->OnNotifyUpdate(h);
	}
	if (tech_view_) {
		tech_view_->OnNotifyUpdate(h);
	}
	if (title_view_) {
		title_view_->OnNotifyUpdate(h);
	}
	if (mmp_view_) {
		mmp_view_->OnNotifyUpdate(h);
	}
	if (trade_view_) {
		trade_view_->OnNotifyUpdate(h);
	}
	if (info_view_) {
		info_view_->OnNotifyUpdate(h);
	}
	if (tick_view_) {
		tick_view_->OnNotifyUpdate(h);
	}
	if (user_view_) {
		user_view_->OnNotifyUpdate(h);
	}
}

void MyStrategyFrame::OnStatusBarMouseEvent(wxMouseEvent& event)
{
	/*wxRect rect;
	GetStatusBar()->GetFieldRect(0, rect);
	if (rect.Contains(event.GetPosition()))
	{
		MyLogDlg* p_my_log_dlg = new MyLogDlg(GetStatusBar()->GetScreenPosition(), rect.width);
		p_my_log_dlg->Show();
	}*/
}

//void MyStrategyFrame::OnNotify(wxCommandEvent& event)
//{
//	HZQDB h = (HZQDB)event.GetClientData();
//	if (h) {
//		if (event.GetEventType() == ZQDB_NOTIFY_ENABLE_EVENT)
//		{
//			OnNotifyEnable(h);
//		}
//		else if (event.GetEventType() == ZQDB_NOTIFY_DISABLE_EVENT)
//		{
//			OnNotifyDisable(h);
//		}
//		else if (event.GetEventType() == ZQDB_NOTIFY_APPEND_EVENT)
//		{
//			OnNotifyAppend(h);
//		}
//		else if (event.GetEventType() == ZQDB_NOTIFY_REMOVE_EVENT)
//		{
//			OnNotifyRemove(h);
//		}
//		else if (event.GetEventType() == ZQDB_NOTIFY_UPDATE_EVENT)
//		{
//			OnNotifyUpdate(h);
//		}
//		//tv->OnNotify(event.GetString(), event.GetInt(), event.GetExtraLong(), h);
//		/*auto calc_data_ptr = tv->GetCalcData();
//		if (calc_data_ptr && calc_data_ptr->GetBase() == code) {
//			tv->UpdateCalcData();
//		}*/
//	}
//}

void MyStrategyFrame::OnPageChanging(wxRibbonBarEvent& evt)
{
	//阻止切换，创建新的对应Frame
	evt.Veto();
}

void MyStrategyFrame::OnNavigateHome(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
}

void MyStrategyFrame::OnNavigateBackward(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	Backward();
}

void MyStrategyFrame::OnNavigateBackwardDropdown(wxRibbonButtonBarEvent& evt)
{
	//wxCommandEvent;
	//auto infos = m_history.BackwardInfo();
	//GenerateDropdownEventMenuImpl(evt, infos, false);
	if (record_pos_ > 0) {
		wxMenu menu;
		for(size_t i = 0; i < record_pos_; i++)
		{
			zqdb::Code code(record_list_[i]);
			menu.Append(ID_NAVIGATE_GOTO + i, utf2wxString(code->Name));
		}
		evt.PopupMenu(&menu);
	}
}

void MyStrategyFrame::OnNavigateBackwardUpdateUI(wxUpdateUIEvent& evt)
{
	evt.Enable(record_pos_ > 0);
}

void MyStrategyFrame::OnNavigateForward(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	Forward();
}

void MyStrategyFrame::OnNavigateForwardDropdown(wxRibbonButtonBarEvent& evt)
{
	if ((record_pos_ + 1) < record_list_.size()) {
		wxMenu menu;
		for (size_t i = record_pos_ + 1; i < record_list_.size(); i++)
		{
			zqdb::Code code(record_list_[i]);
			menu.Append(ID_NAVIGATE_GOTO + i, utf2wxString(code->Name));
		}
		evt.PopupMenu(&menu);
	}
}

void MyStrategyFrame::OnNavigateForwardUpdateUI(wxUpdateUIEvent& evt)
{
	evt.Enable((record_pos_ + 1) < record_list_.size());
}

void MyStrategyFrame::OnNavigateGoto(wxCommandEvent& evt)
{
	auto id = evt.GetId();
	if (id >= ID_NAVIGATE_GOTO && id < ID_NAVIGATE_GOTO_MAX)
	{
		auto pos = id - ID_NAVIGATE_GOTO;
		Goto(pos);
	}
}

void MyStrategyFrame::OnNavigateUp(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	//Up();
}

void MyStrategyFrame::OnNavigateDown(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	//Down();
}

void MyStrategyFrame::OnNavigateFind(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	
}

void MyStrategyFrame::OnMarketAll(wxRibbonButtonBarEvent& evt)
{
	Goto(wxT("全部"));
}

void MyStrategyFrame::OnMarketMain(wxRibbonButtonBarEvent& evt)
{
	Goto(wxT("主力"));
}

void MyStrategyFrame::OnMarketExchange(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		HZQDB h = static_cast<HZQDB>(pBar->GetItemClientData(evt.GetButton()));
		Goto(h);
	}
}

void MyStrategyFrame::OnExchangeDropdown(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		HZQDB h = static_cast<HZQDB>(pBar->GetItemClientData(evt.GetButton()));
		zqdb::AllProduct allproduct(h);
		/*std::sort(allproduct.begin(), allproduct.end(), [](HZQDB x, HZQDB y) {
			zqdb::Product xproduct(x);
			zqdb::Product yproduct(y);
			return strcmp(xproduct->Name, yproduct->Name) < 0;
		});*/
		wxMenu menu;
		wxGetApp().ClearMenuMap();
		for (size_t k = 0; k < allproduct.size(); k++)
		{
			zqdb::Product product(allproduct[k]);
			auto menu_item = menu.Append(ID_MARKET_PRODUCT + k, utf2wxString(product->Name));
			wxGetApp().SetMenuData(ID_MARKET_PRODUCT + k, allproduct[k]);
		}
		evt.PopupMenu(&menu);
	}
}

void MyStrategyFrame::OnProductGoto(wxCommandEvent& evt)
{
	auto id = evt.GetId();
	if (id >= ID_MARKET_PRODUCT && id < ID_MARKET_PRODUCT_MAX)
	{
		HZQDB h = (HZQDB)wxGetApp().GetMenuData(id);
		if (h) {
			Goto(h);
		}
	}
}

void MyStrategyFrame::OnMarketProduct(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		HZQDB h = static_cast<HZQDB>(pBar->GetItemClientData(evt.GetButton()));
		Goto(h);
	}
}

// This shows how to hide ribbon dynamically if there is not enough space.
void MyStrategyFrame::OnSizeEvent(wxSizeEvent& evt)
{
    if ( evt.GetSize().GetWidth() < 200 )
        m_ribbon->Hide();
    else
        m_ribbon->Show();

    evt.Skip();
}

void MyStrategyFrame::OnCloseEvent(wxCloseEvent& evt)
{
	if (info_ptr_ && info_ptr_->IsReadOnly()) {
		//
	}
	else {
		evt.Skip();
	}
}

void MyStrategyFrame::OnTimer(wxTimerEvent& evt)
{
	UpdateStatus();
}

