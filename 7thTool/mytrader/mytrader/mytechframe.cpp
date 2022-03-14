#include "myapp.h"
#include "mytechframe.h"
#include "mylogdlg.h"
#include "mysettingsdlg.h"
#include <techdlg.h>

MyCodeView::MyCodeView(wxWindow* parent, const char* xml, size_t xmlflag) : Base(parent, xml, xmlflag)
{
	wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

	ctrl_text_ = new wxTextCtrl(this, wxID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize
		, /*wxNO_BORDER | */wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
	sizerTop->Add(ctrl_text_, 0, wxEXPAND);

	//sizerTop->AddSpacer(2);

	ctrl_list_ = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition,
		FromDIP(wxSize(230, 300)), wxNO_BORDER | wxDV_NO_HEADER | wxDV_VARIABLE_LINE_HEIGHT);
	ctrl_list_->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);

	ctrl_list_model_ = new MyCodeViewListModel;
	ctrl_list_->AssociateModel(ctrl_list_model_.get());

	ctrl_list_render_ = new MyCodeViewListRenderer(this);
	auto code_col = new wxDataViewColumn("Code", ctrl_list_render_, MyCodeViewListModel::Col_Code
		, 80, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	ctrl_list_->AppendColumn(code_col);
	//ctrl_list_->AppendTextColumn("Code",
	//	SmartKBListModel::Col_Code);

	ctrl_list_model_->ShowAll();

	sizerTop->Add(ctrl_list_, 1, wxEXPAND);

	SetSizer(sizerTop);

	Bind(SMARTKB_SEARCH_RESULT_EVENT, &MyCodeView::OnSearchResult, this);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MyCodeView::OnActivated, this);
	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &MyCodeView::OnSelChanged, this);
	Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &MyCodeView::OnContextMenu, this);

}

MyCodeView::~MyCodeView()
{

}

bool MyCodeView::IsShowAll() const
{
	if (pr_filter_.second) {
		return false;
	}
	return ctrl_list_model_->IsShowAll();
}

bool MyCodeView::IsShowFilter() const
{
	if (pr_filter_.second) {
		return true;
	}
	return ctrl_list_model_->IsShowFilter();
}

bool MyCodeView::IsShowSearch() const
{
	if (IsShowAll()) {
		return false;
	}
	else if (IsShowFilter()) {
		return false;
	}
	else {
		return true;
	}
}

bool MyCodeView::IsShowSearch(const wxString& key) const 
{ 
	if (IsShowAll()) {
		return false;
	}
	else if (IsShowFilter()) {
		return false;
	}
	else {
		return key == ctrl_text_->GetValue();
	}
}

bool MyCodeView::IsTextSearching() const
{
	return ctrl_text_->FindFocus() == ctrl_text_;
}

int MyCodeView::FilterEvent(wxEvent& event)
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

void MyCodeView::DoSelect(const wxDataViewItem& item, size_t try_count)
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
		if (try_count > 1) {
			return;
		}
		wxGetApp().Post(500, [this, item, try_count]() {
			DoSelect(item, try_count + 1);
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

bool MyCodeView::DoSelect(HZQDB h, wxDataViewItem& item)
{
	auto pos = ctrl_list_model_->FindResult(h);
	if (pos >= 0) {
		item = ctrl_list_model_->GetItem(pos);
		DoSelect(item);
		return true;
	}
	return false;
}

void MyCodeView::DoGoto(HZQDB h)
{
	wxDataViewItem item;
	if(DoSelect(h, item)) {
		ctrl_list_->SetFocus();
		Activate(item);
	}
}

HZQDB MyCodeView::All()
{
	return ctrl_list_model_->All();
}

void MyCodeView::Select(HZQDB h, bool bEnsureVisible)
{
	if (bEnsureVisible) {
		wxDataViewItem item;
		DoSelect(h, item);
	}
	else {
		auto pos = ctrl_list_model_->FindResult(h);
		auto item = ctrl_list_model_->GetItem(pos);
		if (item != ctrl_list_->GetSelection()) {
			ctrl_list_->Select(item);
		}
	}
}

void MyCodeView::Redo()
{
	if (IsShowAll()) {
		ShowKey(wxT(SMARTKB_KEY_ALL));
	}
	else if (IsShowFilter()) {
		ShowFilter(pr_filter_.first, pr_filter_.second);
	}
	else {
		ShowKey(ctrl_text_->GetValue());
	}
}

void MyCodeView::Goto(HZQDB h)
{
	if (!ctrl_list_model_->IsShowAll()) {
		ShowKey(wxT(SMARTKB_KEY_ALL));
		wxGetApp().Post(200, [this,h]() {
			Goto(h);
		});
	}
	else {
		DoGoto(h);
	}
}

void MyCodeView::ShowKey(const wxString& key)
{
	pr_filter_.first.Clear();
	pr_filter_.second.Close();
	if (key == wxT(SMARTKB_KEY_ALL)) {
		ctrl_text_->Clear();
		//ctrl_list_model_->ShowAll();
	}
	else {
		//比如筛选”主力“
		ctrl_text_->SetLabel(key);
	}
	ctrl_list_->SetFocus();
}

void MyCodeView::ShowFilter(const wxString& key, const zqdb::Calc::Filter& filter)
{
	pr_filter_.first = key;
	pr_filter_.second = filter;
	ctrl_text_->SetLabel(key);
}

wxString MyCodeView::GetKey(zqdb::Calc::Filter& filter)
{ 
	if (IsShowFilter()) {
		filter = pr_filter_.second;
		return pr_filter_.first;
	}
	return ctrl_text_->GetLabel();
}

void MyCodeView::Up()
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

void MyCodeView::Down()
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

int MyCodeView::IsSort(MY_CODE_SORT_TYPE* type, size_t* secs)
{
	return ctrl_list_model_->IsSort(type, secs);
}

void MyCodeView::Sort()
{
	if (IsTextSearching()) {
		return;
	}

	if (ctrl_list_->GetSortingColumn()) {
		ctrl_list_model_->Resort();
	}

	if (ctrl_list_model_->IsSort()) {
		ctrl_list_model_->Sort();
		Select(sel_, false);
		ctrl_list_->Refresh();
	}
}

void MyCodeView::SortByZD(MY_CODE_SORT_TYPE type, size_t duration, int sort)
{
	wxDataViewColumn* const col = ctrl_list_->GetColumn(0);
	if (sort)
		col->SetSortOrder(sort < 0 ? true /* ascending */ : false);
	else
		col->UnsetAsSortKey();

	ctrl_list_model_->SortByZD(type, duration, sort);
	SmartKBItem smkbi;
	if (ctrl_list_model_->GetResult(0, smkbi)) {
		DoGoto((HZQDB)smkbi.Data);
	}
	ctrl_list_->Refresh();
}

void MyCodeView::SortByField(MDB_FIELD& field, int sort)
{
	ctrl_list_model_->SortByField(field, sort);
	SmartKBItem smkbi;
	if (ctrl_list_model_->GetResult(0, smkbi)) {
		DoGoto((HZQDB)smkbi.Data);
	}
	ctrl_list_->Refresh();
}

void MyCodeView::SortByCalc(const zqdb::Calc::Sort& calc, int sort)
{
	ctrl_list_model_->SortByCalc(calc, sort);
	SmartKBItem smkbi;
	if (ctrl_list_model_->GetResult(0, smkbi)) {
		DoGoto((HZQDB)smkbi.Data);
	}
	ctrl_list_->Refresh();
}

void MyCodeView::OnSkinInfoChanged()
{
	//Base::OnSkinInfoChanged();
	auto skin_info_ptr = std::static_pointer_cast<SkinInfo>(skin_info_ptr_);
	ctrl_list_->SetBackgroundColour(skin_info_ptr->crCtrlBkgnd);
	ctrl_list_->SetForegroundColour(skin_info_ptr->crCtrlForgnd);
	ctrl_list_render_->SetSkinInfo(skin_info_ptr);
	wxSize cellSize = ctrl_list_render_->GetSize();
	ctrl_list_->SetScrollRate(0, cellSize.y);
}

void MyCodeView::OnHandleChanged()
{
	Base::OnHandleChanged();
}

void MyCodeView::UpdateAll()
{
	thread_local zqdb::TaskID task_id;
	if (task_id) {
		wxGetApp().Cancel(task_id);
		task_id.reset();
	}
	task_id = wxGetApp().Post(100, [this] {
		task_id.reset();
		ctrl_list_model_->ClearAll();
		ctrl_list_model_->UpdateAll();
		if (IsShowFilter()) {
			ctrl_list_model_->Search(this, pr_filter_.second);
		}
		else {
			ctrl_list_model_->Search(this, ctrl_text_->GetLabel());
		}
	});
}

void  MyCodeView::OnNotifyEnable(HZQDB h)
{
	if (h->type == ZQDB_HANDLE_TYPE_MODULE) {
		UpdateAll();
	}
}

void MyCodeView::OnNotifyDisable(HZQDB h)
{
	if (h->type == ZQDB_HANDLE_TYPE_MODULE) {
		ctrl_list_model_->ClearAll();
	}
}

void MyCodeView::OnNotifyAppend(HZQDB h)
{
	/*if (h->type == ZQDB_HANDLE_TYPE_EXCHANGE) {
		zqdb::Exchange exchange(h);
		LOG4D("UpdateAll %s", exchange->Exchange);
		UpdateAll();
	}*/
}

void MyCodeView::OnNotifyUpdate(HZQDB h)
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

void MyCodeView::DoAction(int key)
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

wxBEGIN_EVENT_TABLE(MyCodeView, Base)
//EVT_ERASE_BACKGROUND(MyCodeView::OnErase)
//EVT_PAINT(MyCodeView::OnPaint)
EVT_TEXT(wxID_EDIT, MyCodeView::OnSearch)
wxEND_EVENT_TABLE()

void MyCodeView::OnSearch(wxCommandEvent& event)
{
	if (pr_filter_.second) {
		ctrl_list_model_->Search(this, pr_filter_.second);
		ctrl_list_->SetFocus();
	}
	else {
		ctrl_list_model_->Search(this, event.GetString().Trim().MakeUpper());
	}
}

void MyCodeView::OnSearchResult(wxCommandEvent& event)
{
	bool text_searching = IsTextSearching();
	ctrl_list_model_->ShowResult();
	if (!text_searching) {
		ctrl_list_model_->Sort();
	}
	if (ctrl_list_model_->GetCount()) {
		if (h_ && ctrl_list_model_->IsShowAll()) {
			Select(h_);
		}
		else {
			if (text_searching) {
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

void MyCodeView::Activate(const wxDataViewItem& item)
{
	SmartKBItem smkbi;
	if (ctrl_list_model_->GetResult(item, smkbi)) {
		HZQDB h = (HZQDB)smkbi.Data;
		if (!ZQDBIsSubscribeMarketDataAll()) {
			if (!ZQDBIsSubscribeMarketData(h)) {
				//自动订阅
				ZQDBSubscribeMarketData(h);
				//wxGetApp().AddSubscribe(h);
			}
		}
		sel_ = h;
		MyTechFrame::GetFrameByChild(this)->Set(sel_);
	}
	else {
		sel_ = nullptr;
	}
	Refresh();
}

void MyCodeView::OnActivated(wxDataViewEvent &event)
{
	Activate(event.GetItem());
}

void MyCodeView::OnSelChanged(wxDataViewEvent &event)
{
	SmartKBItem smkbi;
	if (ctrl_list_model_->GetResult(event.GetItem(), smkbi)) {
		sel_ = (HZQDB)smkbi.Data;
	}
	else {
		sel_ = nullptr;
	}
}

void MyCodeView::OnContextMenu(wxDataViewEvent &event)
{
	enum {
		ID_FUNC_ADD_SUBSCRIBE = 1,
		ID_FUNC_REMOVE_SUBSCRIBE,
		ID_FUNC_SUBSCRIBE,
		ID_FUNC_UNSUBSCRIBE,
		ID_FUNC_REFRESH,
	};
	wxMenu menu;
	HZQDB h = nullptr;
	auto sel = ctrl_list_->GetSelection();
	if (sel) {
		SmartKBItem smkbi;
		if (ctrl_list_model_->GetResult(sel, smkbi)) {
			h = (HZQDB)smkbi.Data;
		}
		if (ZQDBGetFlags(h) & ZQDB_CODE_FLAG_SUBSCRIBE) {
			menu.Append(ID_FUNC_REMOVE_SUBSCRIBE, wxT("删除" SMARTKB_KEY_SUBSCRIBE));
		}
		else {
			menu.Append(ID_FUNC_ADD_SUBSCRIBE, wxT("添加" SMARTKB_KEY_SUBSCRIBE));
		}
		//if (!ZQDBIsSubscribeMarketDataAll()) {
			if (ZQDBIsSubscribeMarketData(h)) {
				menu.Append(ID_FUNC_UNSUBSCRIBE, wxT("取消订阅"));
			}
			else {
				menu.Append(ID_FUNC_SUBSCRIBE, wxT("订阅"));
			}
		//}
		menu.AppendSeparator();
	}
	menu.Append(ID_FUNC_REFRESH, wxT("刷新"));
	int id = ctrl_list_->GetPopupMenuSelectionFromUser(menu, event.GetPosition());
	if (id == wxID_NONE) {
		return;
	}
	switch (id)
	{
	case ID_FUNC_ADD_SUBSCRIBE: {
		wxGetApp().AddSubscribe(h);
		Refresh();
	} break;
	case ID_FUNC_REMOVE_SUBSCRIBE: {
		wxGetApp().RemoveSubscribe(h);
		bool need_redo = IsShowSearch(wxT(SMARTKB_KEY_SUBSCRIBE));
		if (!need_redo) {
			if (!ZQDBIsSubscribeMarketData(h)) {
				if (IsShowSearch(wxT(SMARTKB_KEY_ALL_SUBSCRIBE))) {
					need_redo = true;
				}
			}
		}
		if (need_redo) {
			Redo();
		}
		Refresh();
	} break;
	case ID_FUNC_SUBSCRIBE: {
		ZQDBSubscribeMarketData(h);
		Refresh();
	} break;
	case ID_FUNC_UNSUBSCRIBE: {
		ZQDBUnSubscribeMarketData(h);
		if (IsShowSearch(wxT(SMARTKB_KEY_ALL_SUBSCRIBE))) {
			Redo();
		}
		Refresh();
	} break;
	case ID_FUNC_REFRESH: {
		//Redo();
		Refresh();
	} break;
	default: {
	} break;
	}
}

///

MyUserView::MyUserView(wxWindow* parent, const char* xml, size_t xmlflag) : Base(parent, xml, xmlflag)
{
	btn_mmx_ = new wxButton(this, wxID_BTN_MMX, wxT("▼"), wxDefaultPosition, wxSize(30,-1));
	cmb_user_ = new wxComboBox(this, wxID_CMB_USER,
		wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0, nullptr, wxCB_READONLY);
	const std::array<wxString,6> price_type = {
		_("Limit Price")
		, _("Upper Price")
		, _("Lower Price")
		, _("Market Price")
		, wxT("FAK")
		, wxT("FOK")
	};
	cmb_price_type_ = new wxComboBox(this, wxID_CTRL_PRICE_TYPE,
		price_type[0], wxDefaultPosition, FromDIP(wxSize(50, -1)), price_type.size(), price_type.data(), wxCB_READONLY);
	ctrl_price_ = new wxSpinCtrlDouble(this, wxID_CTRL_PRICE, wxEmptyString, wxDefaultPosition, wxSize(120, -1));
	ctrl_price_->SetDigits(2);
	const std::array<wxString,1> volume_type = {
		_("Number Volume")
		//, _("Number Amount")
	};
	cmb_volume_type_ = new wxComboBox(this, wxID_CTRL_VOLUME_TYPE, volume_type[0], wxDefaultPosition, FromDIP(wxSize(50, -1)), volume_type.size(), volume_type.data(), wxCB_READONLY);
	ctrl_volume_ = new wxSpinCtrlDouble(this, wxID_CTRL_VOLUME, wxT("1"), wxDefaultPosition, wxSize(120, -1));
	btn_quick_buy_ = new wxButton(this, wxID_BTN_QUICK_BUY, _("Quick Buy"), wxDefaultPosition, wxSize(60, -1));
	btn_quick_sell_ = new wxButton(this, wxID_BTN_QUICK_SELL, _("Quick Sell"), wxDefaultPosition, wxSize(60, -1));
	btn_quick_buy_close_ = new wxButton(this, wxID_BTN_QUICK_BUY_CLOSE, _("Quick Buy Close"), wxDefaultPosition, wxSize(60, -1));
	btn_quick_sell_close_ = new wxButton(this, wxID_BTN_QUICK_SELL_CLOSE, _("Quick Sell Close"), wxDefaultPosition, wxSize(60, -1));
#if wxUSE_STATLINE
	auto sl_buy = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	auto statline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
#endif // wxUSE_STATLINE
	btn_cancel_all_ = new wxButton(this, wxID_BTN_CANCEL_ALL, _("Cancel All Order"), wxDefaultPosition, wxSize(60, -1));
	btn_close_all_ = new wxButton(this, wxID_BTN_CLOSE_ALL, _("Close All Position"), wxDefaultPosition, wxSize(60, -1));
	stc_info_ = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT | wxALIGN_BOTTOM);
	//btn_lock_ = new wxButton(this, wxID_BTN_LOCK_TRADE, _("Lock Trade"), wxDefaultPosition, wxSize(35, -1));

	//SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	//SetBackgroundColour(btn_mmx_->GetBackgroundColour());

	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	wxSizer* sizer_user_h = new wxBoxSizer(wxHORIZONTAL);
	sizer_user_h->Add(btn_mmx_, 0, wxEXPAND);
	sizer_user_h->Add(cmb_user_, 0, wxUP | wxDOWN, 1);

	sizer_user_h->AddSpacer(5);
	sizer_user_h->Add(cmb_price_type_, 0, wxEXPAND | wxUP | wxDOWN, 1);
	sizer_user_h->Add(ctrl_price_, 0, wxEXPAND | wxUP | wxDOWN, 1);
	sizer_user_h->AddSpacer(5);
	sizer_user_h->Add(cmb_volume_type_, 0, wxUP | wxDOWN, 1);
	sizer_user_h->Add(ctrl_volume_, 0, wxUP | wxDOWN, 1);
	sizer_user_h->AddSpacer(5);
	sizer_user_h->Add(btn_quick_buy_, 0, wxEXPAND);
	sizer_user_h->AddSpacer(5);
	sizer_user_h->Add(btn_quick_sell_close_, 0, wxEXPAND);
	sizer_user_h->AddSpacer(5);
#if wxUSE_STATLINE
	sizer_user_h->Add(sl_buy, 0, wxEXPAND | wxUP | wxDOWN, 2);
#endif // wxUSE_STATLINE
	sizer_user_h->AddSpacer(5);
	sizer_user_h->Add(btn_quick_sell_, 0, wxEXPAND);
	sizer_user_h->AddSpacer(5);
	sizer_user_h->Add(btn_quick_buy_close_, 0, wxEXPAND);
	sizer_user_h->AddSpacer(5);
#if wxUSE_STATLINE
	sizer_user_h->Add(statline, 0, wxEXPAND | wxUP | wxDOWN, 2);
#endif // wxUSE_STATLINE
	sizer_user_h->AddSpacer(5);
	sizer_user_h->Add(btn_cancel_all_, 0, wxEXPAND);
	sizer_user_h->AddSpacer(5);
	sizer_user_h->Add(btn_close_all_, 0, wxEXPAND);

	sizer_user_h->AddStretchSpacer();
	/*wxSizer* sizer_stc_info = new wxBoxSizer(wxVERTICAL);
	sizer_stc_info->AddStretchSpacer();
	sizer_stc_info->Add(stc_info_, 0, wxEXPAND);
	sizer_stc_info->AddStretchSpacer();*/
	sizer_user_h->Add(stc_info_, 1, wxALIGN_CENTER_VERTICAL);
	
	sizer_user_h->AddSpacer(5);
	//sizer_user_h->Add(btn_lock_, 0, wxEXPAND);

	topSizer->Add(sizer_user_h, 0, wxEXPAND);

	SetSizer(topSizer);

	DoUpdateAll();
}

MyUserView::~MyUserView()
{

}

void MyUserView::Goto(HZQDB user)
{
	DoSelect(user);
}

void MyUserView::OnSkinInfoChanged()
{
	Base::OnSkinInfoChanged();

	auto skin_info_ptr = std::static_pointer_cast<SkinInfo>(skin_info_ptr_);
	SetBackgroundColour(skin_info_ptr->crPrimary);
	SetForegroundColour(skin_info_ptr->crTertiary);
	btn_mmx_->SetBackgroundColour(skin_info_ptr->crPrimary);
	btn_mmx_->SetForegroundColour(skin_info_ptr_->GetArtColor(wxRIBBON_ART_TAB_LABEL_COLOUR));
	//cmb_user_->SetBackgroundColour(skin_info_ptr->crPrimary);
	//cmb_user_->SetForegroundColour(skin_info_ptr->crCtrlText); 
	//cmb_price_type_->SetBackgroundColour(skin_info_ptr->crPrimary);
	//cmb_price_type_->SetForegroundColour(skin_info_ptr_->GetArtColor(wxRIBBON_ART_TAB_LABEL_COLOUR));
	//ctrl_price_->SetBackgroundColour(skin_info_ptr->crPrimary);
	//cmb_volume_type_->SetBackgroundColour(skin_info_ptr->crPrimary);
	//cmb_volume_type_->SetForegroundColour(skin_info_ptr_->GetArtColor(wxRIBBON_ART_TAB_LABEL_COLOUR));
	//ctrl_volume_->SetBackgroundColour(skin_info_ptr->crPrimary);
	btn_quick_buy_->SetBackgroundColour(skin_info_ptr->crPrimary);
	btn_quick_buy_->SetForegroundColour(skin_info_ptr->crCtrlRising);
	btn_quick_sell_->SetBackgroundColour(skin_info_ptr->crPrimary);
	btn_quick_sell_->SetForegroundColour(skin_info_ptr->crCtrlFalling);
	btn_quick_buy_close_->SetBackgroundColour(skin_info_ptr->crPrimary);
	btn_quick_buy_close_->SetForegroundColour(skin_info_ptr->crCtrlRising);
	btn_quick_sell_close_->SetBackgroundColour(skin_info_ptr->crPrimary);
	btn_quick_sell_close_->SetForegroundColour(skin_info_ptr->crCtrlFalling);
	btn_cancel_all_->SetBackgroundColour(skin_info_ptr->crPrimary);
	btn_cancel_all_->SetForegroundColour(skin_info_ptr_->GetArtColor(wxRIBBON_ART_TAB_LABEL_COLOUR));
	btn_close_all_->SetBackgroundColour(skin_info_ptr->crPrimary);
	btn_close_all_->SetForegroundColour(skin_info_ptr_->GetArtColor(wxRIBBON_ART_TAB_LABEL_COLOUR));
	stc_info_->SetForegroundColour(skin_info_ptr_->GetArtColor(wxRIBBON_ART_TAB_LABEL_COLOUR));
	//btn_lock_->SetBackgroundColour(skin_info_ptr->crPrimary);
	//btn_lock_->SetForegroundColour(skin_info_ptr_->GetArtColor(wxRIBBON_ART_TAB_LABEL_COLOUR));
	if (my_view_) {
		my_view_->SetSkinInfo(skin_info_ptr_);
	}
}

void MyUserView::OnHandleChanged()
{
	Base::OnHandleChanged();
	DoUpatePrice(true);
	if (my_view_) {
		my_view_->SetHandle(h_);
	}
}

void MyUserView::OnUserChanged()
{
	UserBase::OnUserChanged();

	if (my_module_) {
		stc_info_->SetLabel(my_module_->GetUserInfo(user_));
	}
	else {
		stc_info_->SetLabel(wxEmptyString);
	}

	if (my_view_) {
		my_view_->SetUser(user_);
	}

	if (mmx_) {
		DoShow();
	}
}

void MyUserView::OnNotifyEnable(HZQDB h)
{
	if (h->type == ZQDB_HANDLE_TYPE_MODULE) {
		DoUpdateAll();
		DoSelect(GetUser());
	}
	if (my_view_) {
		my_view_->OnNotifyEnable(h);
	}
}

void MyUserView::OnNotifyDisable(HZQDB h)
{
	if (h->type == ZQDB_HANDLE_TYPE_MODULE) {
		DoUpdateAll();
		DoSelect(GetUser());
	}
	if (my_view_) {
		my_view_->OnNotifyDisable(h);
	}
}

void MyUserView::OnNotifyAppend(HZQDB h) 
{
	switch (h->type)
	{
	case ZQDB_HANDLE_TYPE_MODULE:
	case ZQDB_HANDLE_TYPE_USER: {
		DoUpdateAll();
		DoSelect(GetUser());
	} break;
	default: {
		//
	} break;
	}
	if (my_view_) {
		my_view_->OnNotifyAppend(h);
	}
}

void MyUserView::OnNotifyRemove(HZQDB h) 
{
	if (my_view_) {
		my_view_->OnNotifyRemove(h);
	}
}

void MyUserView::OnNotifyUpdate(HZQDB h)
{
	bool relayout = false;
	if (my_module_) {
		bool update_user_info = false;
		if (h == user_) {
			update_user_info = true;
		}
		else if (ZQDBGetModule(h) == *my_module_) {
			if (ZQDBIsDataOfTable(h, STR_ZQDB_TABLE_ACCOUNT)) {
				update_user_info = true;
			}
		}
		if (update_user_info) {
			auto info = my_module_->GetUserInfo(user_);
			if (info.Length() != stc_info_->GetLabel().Length()) {
				relayout = true;
			}
			stc_info_->SetLabel(info);
		}
	}
	if (h == h_) {
		DoUpatePrice();
	}
	if (my_view_) {
		my_view_->OnNotifyUpdate(h);
	}
	if (relayout) {
		this->Layout();
	}
}

void MyUserView::DoUpdateAll()
{
	cmb_user_->Clear();
	size_t user_count = ZQDBGetCalcUserCount();
	if (!user_count) {
		//
	}
	else {
		for (size_t i = 0; i < user_count; i++)
		{
			auto h = ZQDBGetCalcUser(i);
			zqdb::ObjectT<tagUserInfo> user(h);
			cmb_user_->Append(user->User, (void*)h);
		}
	}
}

void MyUserView::DoSelect(HZQDB user)
{
	size_t user_count = ZQDBGetCalcUserCount();
	if (user_count <= 0) {
		return;
	}
	int sel = -1;
	if (user) {
		for (size_t i = 0; i < user_count; i++)
		{
			if (ZQDBGetCalcUser(i) == user) {
				sel = i;
				break;
			}
		}
	}
	if (sel < 0) {
		sel = 0;
		user = ZQDBGetCalcUser(sel);
	}
	cmb_user_->Select(sel);
	DoGoto(user);
}

void MyUserView::DoGoto(HZQDB user)
{
	if (user == user_) {
		return;
	}

	Freeze();

	HZQDB hModule = ZQDBGetModule(user);
	if (hModule != ZQDBGetModule(user_)) {
		if (mmx_) {
			auto sizer = GetSizer();
			my_module_.reset();
			if (my_view_) {
				sizer->Remove(1);
				delete my_view_;
				my_view_ = nullptr;
			}
		}
		MyModule::Broadcast([this, hModule](std::shared_ptr<MyModule> module)->bool {
			if (hModule == *module) {
				my_module_ = module;
				return true;
			}
			return false;
		});
	}

	MyTechFrame::GetFrameByChild(this)->SetUser(user);

	Layout();
	Thaw();
}

void MyUserView::DoShow()
{
	bool relayout = false;
	if (!my_view_) {
		auto sizer = GetSizer();
		if (my_module_) {
			//显示user模块视图
			my_view_ = my_module_->NewMiniView(this);
		}
		if (my_view_) {
			my_view_->SetSkinInfo(GetSkinInfo());
			my_view_->SetHandle(h_);
			my_view_->SetUser(user_);
			sizer->Add(my_view_, 1, wxEXPAND);
			relayout = true;
		}
	}

	mmx_ = true;
	btn_mmx_->SetLabel(wxT("▼"));
	if (relayout) {
		GetParent()->Layout();
	}
}

void MyUserView::DoHide()
{
	auto sizer = GetSizer();
	if (sizer->GetItemCount() < 2) {
		ASSERT(!my_view_);
		return;
	}
	ASSERT(my_view_);

	mmx_ = false;
	btn_mmx_->SetLabel(wxT("▲"));
	sizer->Remove(1);
	delete my_view_;
	my_view_ = nullptr;
	GetParent()->Layout();
}

void MyUserView::DoUpatePrice(bool set)
{
	char type;
	double price;
	std::tie(type, price) = GetPrice(set);
	if (set) {
		zqdb::Code code(h_);
		if (code->Upper > code->Lower && !IsInvalidValue(code->Upper)) {
			ctrl_price_->SetRange(code->Lower, code->Upper);
		}
		else {
			ctrl_price_->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::infinity());
		}
		ctrl_price_->SetIncrement(code->PriceTick);
		ctrl_price_->SetValue(price);
	}
	else {
		switch (type)
		{
		case ORDER_LIMIT: {
		} break;
		case ORDER_MARKET:
		case ORDER_FAK:
		case ORDER_FOK: {
			ctrl_price_->SetValue(price);
		} break;
		}
	}
}

void MyUserView::Draw(wxDC& dc)
{
}

wxBEGIN_EVENT_TABLE(MyUserView, Base)
//EVT_ERASE_BACKGROUND(MyUserView::OnErase)
//EVT_PAINT(MyUserView::OnPaint)
EVT_TIMER(wxID_ANY, MyUserView::OnTimer)
EVT_BUTTON(wxID_BTN_MMX, MyUserView::OnBtnMMX)
EVT_COMBOBOX(wxID_CMB_USER, MyUserView::OnCmbUserUpdate)
EVT_COMBOBOX(wxID_CTRL_PRICE_TYPE, MyUserView::OnCmbPriceTypeUpdate)
EVT_COMBOBOX(wxID_CTRL_VOLUME_TYPE, MyUserView::OnCmbVolumeTypeUpdate)
EVT_BUTTON(wxID_BTN_QUICK_BUY, MyUserView::OnBtnQuickBuy)
EVT_BUTTON(wxID_BTN_QUICK_SELL, MyUserView::OnBtnQuickSell)
EVT_BUTTON(wxID_BTN_QUICK_BUY_CLOSE, MyUserView::OnBtnQuickBuyClose)
EVT_BUTTON(wxID_BTN_QUICK_SELL_CLOSE, MyUserView::OnBtnQuickSellClose)
EVT_BUTTON(wxID_BTN_CANCEL_ALL, MyUserView::OnBtnCancelAll)
EVT_BUTTON(wxID_BTN_CLOSE_ALL, MyUserView::OnBtnCloseAll)
EVT_BUTTON(wxID_BTN_LOCK_TRADE, MyUserView::OnBtnLockTrade)
wxEND_EVENT_TABLE()

void MyUserView::OnErase(wxEraseEvent &event)
{

}

void MyUserView::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	dc.Clear();
	if (IsDispOk()) {
		Draw(dc);
	}
}

void MyUserView::OnTimer(wxTimerEvent& event)
{
	if (my_view_) {
		my_view_->GetEventHandler()->ProcessEvent(event);
	}
}

void MyUserView::OnBtnMMX(wxCommandEvent& event)
{
	if (mmx_) {
		//收起
		DoHide();
	} else {
		//展开
		DoShow();
	}
}

void MyUserView::OnCmbUserUpdate(wxCommandEvent& event)
{
	auto user = (HZQDB)cmb_user_->GetClientData(event.GetSelection());
	if (user_ != user) {
		DoGoto(user);
	}
}

std::tuple<char, double> MyUserView::GetPrice(bool set)
{
	ASSERT(h_);
	zqdb::Code code(h_);
	char type = ORDER_LIMIT;
	double price = 0;
	switch (cmb_price_type_->GetSelection())
	{
	case 0: {//Limit Price
		price = set ? code->Close : ctrl_price_->GetValue();
	} break;
	case 1: {//Upper Price
		price = set ? code->Upper : ctrl_price_->GetValue();
	} break;
	case 2: {//Lower Price
		price = set ? code->Lower : ctrl_price_->GetValue();
	} break; 
	case 3: {//Market Price
		type = ORDER_MARKET;
		price = code->Close;
	} break;
	case 4: {//FAK
		type = ORDER_FAK;
		price = code->Close;
	} break;
	case 5: {//FOK
		type = ORDER_FOK;
		price = code->Close;
	} break;
	}
	return std::make_tuple(type, price);
}

double MyUserView::GetVolume()
{
	switch (cmb_volume_type_->GetSelection())
	{
	case 0: {//Number Volume
		return ctrl_volume_->GetValue();
	} break;
	case 1: {//Number Amount
		
	} break;
	}
	return 0.;
}

void MyUserView::OnCmbPriceTypeUpdate(wxCommandEvent& event)
{
	if (!h_) {
		return;
	}
	char type;
	double price;
	std::tie(type, price) = GetPrice(true);
	ctrl_price_->SetValue(price);
	switch (type)
	{
	case ORDER_LIMIT: {
		ctrl_price_->Enable(true);
	} break;
	case ORDER_MARKET:
	case ORDER_FAK:
	case ORDER_FOK: {
		ctrl_price_->Enable(false);
	} break;
	}
}

void MyUserView::OnCmbVolumeTypeUpdate(wxCommandEvent& event)
{
	
}

bool MyUserView::CheckParams()
{
	if (!h_) {
		wxMessageBox(wxT("当前代码为空!!!"));
		return false;
	}
	if (!user_) {
		wxMessageBox(wxT("当前用户为空!!!"));
		return false;
	}
	if (!my_module_) {
		wxMessageBox(wxT("当前用户不支持交易!!!"));
		return false;
	}
	return true;
}

void MyUserView::OrderSend(char direction)
{
	if (!CheckParams()) {
		return;
	}
	char type;
	double price;
	std::tie(type, price) = GetPrice();
	double volume = GetVolume();
#if 1
	wxGetApp().SendOrder(user_, h_, direction, OFFSET_OPEN, type, volume, price);
#else
	wxString strTips;
	HNMSG rsp = nullptr;
	int err = my_module_->OrderSend(user_, h_, direction, OFFSET_OPEN, type, volume, price, &rsp);
	if (rsp) {
		zqdb::Msg rsp_msg(rsp, zqdb::Msg::AutoDelete);
		auto errorcode = rsp_msg.GetParamAsInt(STR_ZQDB_MSG_ERROR_CODE, 0);
		auto errormsg = utf2wxString(rsp_msg.GetParam(STR_ZQDB_MSG_ERROR_MESSAGE, ""));
		if (!errorcode) {
			strTips = wxString::Format(wxT("提交订单成功。\n")
				wxT("订单ID：%s"), rsp_msg.GetParam(STR_MDB_FIELD_INDEX(ZQDB, ORDER, ORDER), ""));
		}
		else {
			strTips = wxString::Format(wxT("提交订单失败!!!\n")
				wxT("错误原因：[%zd]%s"), errorcode, errormsg);
		}
	}
	else {
		strTips = wxString::Format(wxT("提交订单失败!!!"));
	}
	wxMessageBox(strTips);
#endif
}

void MyUserView::OnBtnQuickBuy(wxCommandEvent& event)
{
	OrderSend(DIRECTION_LONG);
}

void MyUserView::OnBtnQuickSell(wxCommandEvent& event)
{
	OrderSend(DIRECTION_SHORT);
}

void MyUserView::OrderClose(char direction)
{
	if (!CheckParams()) {
		return;
	}
	char type;
	double price;
	std::tie(type, price) = GetPrice();
	double volume = GetVolume();
	wxString strTips;
	zqdb::AllTableData allposition(STR_ZQDB_TABLE_POSITION, *my_module_);
	for (auto h : allposition)
	{
		zqdb::ObjectT<tagPositionInfo> position(h);
		zqdb::Code code(h_);
		if (position->Direction == direction && strcmp(code->TradeCode, position->Code) == 0 && strcmp(code->Exchange, position->Exchange) == 0) {
			if (!strTips.IsEmpty()) {
				strTips += wxT("\n");
			}
			HNMSG rsp = nullptr;
			int err = my_module_->OrderClose(user_, h, type, volume, price, &rsp);
			if (rsp) {
				zqdb::Msg rsp_msg(rsp, zqdb::Msg::AutoDelete);
				auto errorcode = rsp_msg.GetParamAsInt(STR_ZQDB_MSG_ERROR_CODE, 0);
				auto errormsg = utf2wxString(rsp_msg.GetParam(STR_ZQDB_MSG_ERROR_MESSAGE, ""));
				if (!errorcode) {
					strTips += wxString::Format(wxT("提交订单成功。\n")
						wxT("订单ID：%s"), rsp_msg.GetParam(STR_MDB_FIELD_INDEX(ZQDB, ORDER, ORDER), ""));
				}
				else {
					strTips += wxString::Format(wxT("提交订单失败!!!\n")
						wxT("错误原因：[%zd]%s"), errorcode, errormsg);
				}
			}
			else {
				strTips += wxString::Format(wxT("提交订单失败!!!"));
			}
			break;
		}
	}
	if (strTips.IsEmpty()) {
		wxMessageBox(wxT("无仓可平!!!"));
	}
	else {
		wxMessageBox(strTips);
	}
}

void MyUserView::OnBtnQuickBuyClose(wxCommandEvent& event)
{
	OrderClose(DIRECTION_SHORT);
}

void MyUserView::OnBtnQuickSellClose(wxCommandEvent& event)
{
	OrderClose(DIRECTION_LONG);
}

void MyUserView::OnBtnCancelAll(wxCommandEvent& event)
{
	if (!CheckParams()) {
		return;
	}
	wxString strTips;
	zqdb::AllTableData allorder(STR_ZQDB_TABLE_ORDER, *my_module_);
	for (auto h : allorder)
	{
		zqdb::ObjectT<tagOrderInfo> order(h);
		if (ZQDBOrderIsFinal(order->Status)) {
			continue;
		}
		zqdb::Code code(h_);
		if (!strTips.IsEmpty()) {
			strTips += wxT("\n");
		}
		HNMSG rsp = nullptr;
		int err = my_module_->OrderCancel(user_, h, &rsp);
		if (rsp) {
			zqdb::Msg rsp_msg(rsp, zqdb::Msg::AutoDelete);
			auto errorcode = rsp_msg.GetParamAsInt(STR_ZQDB_MSG_ERROR_CODE, 0);
			auto errormsg = utf2wxString(rsp_msg.GetParam(STR_ZQDB_MSG_ERROR_MESSAGE, ""));
			if (!errorcode) {
				strTips += wxString::Format(wxT("订单%s撤销成功。"), order->Order);
			}
			else {
				strTips += wxString::Format(wxT("订单%s撤销失败，错误原因：[%zd]%s"), order->Order, errorcode, errormsg);
			}
		}
		else {
			strTips += wxString::Format(wxT("提交撤单失败!!!"));
		}
	}
	if (strTips.IsEmpty()) {
		wxMessageBox(wxT("无单可撤!!!"));
	}
	else {
		wxMessageBox(strTips);
	}
}

void MyUserView::OnBtnCloseAll(wxCommandEvent& event)
{
	if (!CheckParams()) {
		return;
	}
	wxString strTips;
	zqdb::AllTableData allposition(STR_ZQDB_TABLE_POSITION, *my_module_);
	for (auto h : allposition)
	{
		if (!strTips.IsEmpty()) {
			strTips += wxT("\n");
		}
		zqdb::ObjectT<tagPositionInfo> position(h);
		zqdb::Code code(h_);
		wxString strTips;
		HNMSG rsp = nullptr;
		int err = my_module_->OrderClose(user_, h, ORDER_MARKET, position->Volume - position->FrozenVolume, 0, &rsp);
		if (rsp) {
			zqdb::Msg rsp_msg(rsp, zqdb::Msg::AutoDelete);
			auto errorcode = rsp_msg.GetParamAsInt(STR_ZQDB_MSG_ERROR_CODE, 0);
			auto errormsg = utf2wxString(rsp_msg.GetParam(STR_ZQDB_MSG_ERROR_MESSAGE, ""));
			if (!errorcode) {
				strTips += wxString::Format(wxT("提交订单成功。\n")
					wxT("订单ID：%s"), rsp_msg.GetParam(STR_MDB_FIELD_INDEX(ZQDB, ORDER, ORDER), ""));
			}
			else {
				strTips += wxString::Format(wxT("提交订单失败!!!\n")
					wxT("错误原因：[%zd]%s"), errorcode, errormsg);
			}
		}
		else {
			strTips += wxString::Format(wxT("提交订单失败!!!"));
		}
	}
	if (strTips.IsEmpty()) {
		wxMessageBox(wxT("无仓可平!!!"));
	}
	else {
		wxMessageBox(strTips);
	}
}

void MyUserView::OnBtnLockTrade(wxCommandEvent& event)
{
	/*lock_ = !lock_;
	btn_lock_->SetLabel(lock_ ? _("Unlock Trade") : _("Lock Trade"));
	if (mmx_) {
		//收起
		DoHide();
	}
	bool show = !lock_;
	btn_mmx_->Show(show);
	cmb_user_->Show(show);
	cmb_price_type_->Show(show);
	ctrl_price_->Show(show);
	cmb_volume_type_->Show(show);
	ctrl_volume_->Show(show);
	btn_quick_buy_->Show(show);
	btn_quick_sell_->Show(show);
	btn_quick_buy_close_->Show(show);
	btn_quick_sell_close_->Show(show);
	btn_cancel_all_->Show(show);
	btn_close_all_->Show(show);
	stc_info_->Show(show);*/
}

///

//static const char *numlockIndicators[] = { "OFF", "NUM" };
//static const char *capslockIndicators[] = { "", "CAPS" };

static const std::map<char, wxString> connectStatusMap =
{
	{ STATUS_OFFLINE,wxString(L"离线") },
	{ STATUS_CONNECT,wxString(L"连接中") },
	{ STATUS_CONNECTED,wxString(L"已连接") },
	{ STATUS_LOGGED,wxString(L"已登录") },
};

static const std::map<char, wxString> businessStatusMap =
{
	{ XMT_Status_BeforeTrading,wxString(L"开盘前") },
	{ XMT_Status_NoTrading,wxString(L"非交易") },
	{ XMT_Status_Continous,wxString(L"连续交易") },
	{ XMT_Status_AuctionOrdering,wxString(L"集合竞价报单") },
	{ XMT_Status_AuctionBalance,wxString(L"集合竞价价格平衡") },
	{ XMT_Status_AuctionMatch,wxString(L"集合竞价撮合") },
	{ XMT_Status_Closed,wxString(L"收盘") },
};

MyStatusBar::MyStatusBar(wxWindow *parent, const char* xml, size_t xmlflag) : Base(parent, xml, xmlflag)
{
	// compute the size needed for num lock indicator pane
	/*wxClientDC dc(this);
	wxSize sizeNumLock = dc.GetTextExtent(numlockIndicators[0]);
	sizeNumLock.IncTo(dc.GetTextExtent(numlockIndicators[1]));

	int widths[Field_Max];
	widths[Field_Text] = -1; // growable
	widths[Field_Checkbox] = 150;
	widths[Field_Bitmap] = BITMAP_SIZE_X;
	widths[Field_NumLockIndicator] = sizeNumLock.x;
	widths[Field_Clock] = 100;
	widths[Field_CapsLockIndicator] = dc.GetTextExtent(capslockIndicators[1]).x;

	SetFieldsCount(Field_Max);
	SetStatusWidths(Field_Max, widths);*/


	//SetMinHeight(wxMax(m_statbmp->GetBestSize().GetHeight(),
	//	m_checkbox->GetBestSize().GetHeight()));

	SetDoubleBuffered(true);
}

MyStatusBar::~MyStatusBar()
{
}

void MyStatusBar::OnSkinInfoChanged()
{
	Base::OnSkinInfoChanged();
	auto skin_info_ptr = std::static_pointer_cast<SkinInfo>(skin_info_ptr_);
	SetBackgroundColour(skin_info_ptr->crPrimary);
	SetForegroundColour(skin_info_ptr_->GetArtColor(wxRIBBON_ART_PANEL_LABEL_COLOUR));
	Reset();
}

void MyStatusBar::OnHandleChanged()
{

}

void MyStatusBar::OnUserChanged()
{

}

void MyStatusBar::OnNotifyEnable(HZQDB h)
{

}

void MyStatusBar::OnNotifyDisable(HZQDB h)
{

}

void MyStatusBar::OnNotifyAppend(HZQDB h)
{
	if (h->type == ZQDB_HANDLE_TYPE_EXCHANGE) {
		Reset();
	}
}

void MyStatusBar::OnNotifyRemove(HZQDB h)
{

}

void MyStatusBar::OnNotifyUpdate(HZQDB h)
{

}

void MyStatusBar::Reset()
{
	//auto font = GetFont();
	wxClientDC dc(this);
	wxSize szExchange = dc.GetTextExtent(wxT("CFFEX 14:45:06"));

	allexchange_.Update();
	std::vector<int> widths = { -1 };
	for (size_t i = 0; i < allexchange_.size(); i++)
	{
		//zqdb::Exchange exchange(allexchange_[i]);
		//wxSize szExchange = dc.GetTextExtent(wxString::Format(wxT("%s 14:45:06"), exchange->Exchange));
		widths.emplace_back(szExchange.x);
	}
	SetFieldsCount(widths.size(), widths.data());
	Update();
}

void MyStatusBar::Update()
{
	size_t i = 0;
	for (; i < allexchange_.size(); i++)
	{
		auto h = allexchange_[i];
		zqdb::Exchange exchange(h);
		wxString name = utf2wxString(exchange->Exchange);

		//auto iFindConnectStatus = connectStatusMap.find(ZQDBGetStatus(exchange->Exchange));
		//wxString connectStaus = iFindConnectStatus != connectStatusMap.end() ? iFindConnectStatus->second : wxString(L"未知");

		//auto iFindBusinessStatus = businessStatusMap.find(exchange->Status);
		//wxString businessStatus = iFindBusinessStatus != businessStatusMap.end() ? iFindBusinessStatus->second : wxString(L"未知");

		wxString strStatus;
		auto status = ZQDBGetStatus(exchange->Exchange);
		if (ZQDBIsTest() || status == STATUS_LOGGED) {
			uint32_t Time = exchange.GetNowTime();
			strStatus = wxString::Format(L"%02d:%02d:%02d", XUtil::GetHour(Time), XUtil::GetMinute(Time), XUtil::GetSecond(Time));
		}
		else {
			switch (status)
			{
			case STATUS_OFFLINE: {
				strStatus = wxT("离线");
			} break;
			case STATUS_CONNECT: {
				strStatus = wxT("连接中");
			} break;
			case STATUS_CONNECTED: {
				strStatus = wxT("已连接");
			} break;
			case STATUS_LOGGED: {
				strStatus = wxT("正常");
			} break;
			default: {
				strStatus = wxT("未知");
			} break;
			}
		}
		SetStatusText(name + wxT(" ") + strStatus, i + 1);
	}
}

wxBEGIN_EVENT_TABLE(MyStatusBar, Base)
	//EVT_ERASE_BACKGROUND(MyStatusBar::OnErase)
	//EVT_PAINT(MyStatusBar::OnPaint)
	//EVT_SIZE(MyStatusBar::OnSize)
	EVT_TIMER(wxID_ANY, MyStatusBar::OnTimer)
	//EVT_IDLE(MyStatusBar::OnIdle)
wxEND_EVENT_TABLE()

void MyStatusBar::OnErase(wxEraseEvent &event)
{

}

void MyStatusBar::OnPaint(wxPaintEvent &event)
{
	wxBufferedPaintDC dc(this);
	dc.Clear();
}

void MyStatusBar::OnTimer(wxTimerEvent& event)
{
	Update();
}

void MyStatusBar::OnSize(wxSizeEvent& event)
{
	/*#if wxUSE_CHECKBOX
	if (!m_checkbox)
	return;
	#endif

	wxRect rect;
	if (!GetFieldRect(Field_Checkbox, rect))
	{
	event.Skip();
	return;
	}

	#if wxUSE_CHECKBOX
	wxRect rectCheck = rect;
	rectCheck.Deflate(2);
	m_checkbox->SetSize(rectCheck);
	#endif

	GetFieldRect(Field_Bitmap, rect);
	wxSize size = m_statbmp->GetSize();

	m_statbmp->Move(rect.x + (rect.width - size.x) / 2,
	rect.y + (rect.height - size.y) / 2);
	*/
	event.Skip();
}

void MyStatusBar::OnIdle(wxIdleEvent& event)
{
	//SetStatusText(numlockIndicators[wxGetKeyState(WXK_NUMLOCK)],
	//	Field_NumLockIndicator);
	//SetStatusText(capslockIndicators[wxGetKeyState(WXK_CAPITAL)],
	//	Field_CapsLockIndicator);

	event.Skip();
}

///

wxBEGIN_EVENT_TABLE(MyTechFrame, Base)
//Navigate
//EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_HOME, MyTechFrame::OnNavigateHome)
EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_BACKWARD, MyTechFrame::OnNavigateBackward)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_NAVIGATE_BACKWARD, MyTechFrame::OnNavigateBackwardDropdown)
EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_FORWARD, MyTechFrame::OnNavigateForward)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_NAVIGATE_FORWARD, MyTechFrame::OnNavigateForwardDropdown)
EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_ALL, MyTechFrame::OnMarketAll)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_MARKET_ALL, MyTechFrame::OnMarketAllDropdown)
EVT_UPDATE_UI(ID_NAVIGATE_BACKWARD, MyTechFrame::OnNavigateBackwardUpdateUI)
EVT_UPDATE_UI(ID_NAVIGATE_FORWARD, MyTechFrame::OnNavigateForwardUpdateUI)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_UP, MyTechFrame::OnNavigateUp)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_DOWN, MyTechFrame::OnNavigateDown)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_NAVIGATE_FIND, MyTechFrame::OnNavigateFind)
EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_MAIN, MyTechFrame::OnMarketMain)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_MARKET_MAIN, MyTechFrame::OnMarketMainDropdown)
//Market
//EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_SELF, MyTechFrame::OnMarket)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_MAIN, MyTechFrame::OnMarketMain)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_MARKET_ALL, MyTechFrame::OnMarketAll)
////Trade
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_BUY_FAK, MyTechFrame::OnTradeBuyFAK)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_BUY_FOK, MyTechFrame::OnTradeBuyFOK)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_SELL_FAK, MyTechFrame::OnTradeSellFAK)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_SELL_FOK, MyTechFrame::OnTradeSellFOK)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_QUICK_CLOSE, MyTechFrame::OnTradeClose)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_POSITION, MyTechFrame::OnTradePosition)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_ORDER, MyTechFrame::OnTradeOrder)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_TRADE, MyTechFrame::OnTradeTrade)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_ADDUSER, MyTechFrame::OnTradeAddUser)
//EVT_RIBBONBUTTONBAR_CLICKED(ID_TRADE_REMOVEUSER, MyTechFrame::OnTradeRemoveUser)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CLEAR_SETTINGS, MyTechFrame::MyTechFrame::OnClearSettings)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CLEAR_DATA, MyTechFrame::MyTechFrame::OnClearData)

EVT_TIMER(wxID_ANY, MyTechFrame::OnTimer)
wxEND_EVENT_TABLE()
//
//wxBEGIN_EVENT_TABLE(MyTechFrame::EventHandler, wxEvtHandler)
//EVT_CHAR(MyTechFrame::EventHandler::OnChar)
//wxEND_EVENT_TABLE()
//
//MyTechFrame::EventHandler::EventHandler(MyTechFrame* frame)
//{
//	wxWindowList children = frame->GetChildren();
//	for (wxWindowListNode *i = children.GetFirst(); i; i = i->GetNext()) {
//		SetChildNextHandler(i->GetData());
//	}
//}
//
//void MyTechFrame::EventHandler::SetChildNextHandler(wxWindow* child)
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
//void MyTechFrame::EventHandler::OnChar(wxKeyEvent &event)
//{
//	event.Skip();
//}

MyTechFrame* MyTechFrame::GetFrameByChild(wxWindow* child)
{
	if (child) {
		wxWindow* parent = nullptr;
		while (parent = child->GetParent()) {
			child = parent;
		}
		//return wxDynamicCast(child, MyTechFrame);
		return wxStaticCast(child, MyTechFrame);
	}
	return nullptr;
}

MyTechFrame::MyTechFrame(const char* xml, size_t xmlflag)
    : Base(xml, xmlflag)
	, all_filter_func_(CALC_FILTER)
	, all_sort_func_(CALC_SORT)
	, all_script_func_(CALC_SCRIPT)
{
#ifdef _DEBUG
	auto cwd = boost::filesystem::current_path();
#endif//
	CFG_FROM_XML(cfg, xml, xmlflag);

	auto filter_calc_func = wxGetApp().GetFilterCalcFunc();
	for (size_t i = 0; i < all_filter_func_.size(); i++)
	{
		zqdb::Calc::Func func(all_filter_func_[i]);
		wxString name = func.GetCalcName();
		if (name == filter_calc_func) {
			cur_filter_func_ = i;
			break;
		}
	}
	auto sort_calc_func = wxGetApp().GetSortCalcFunc();
	for (size_t i = 0; i < all_sort_func_.size(); i++)
	{
		zqdb::Calc::Func func(all_sort_func_[i]);
		wxString name = func.GetCalcName();
		if (name == sort_calc_func) {
			cur_sort_func_ = i;
			break;
		}
	}

	//CreateStatusBar();
	status_bar_ = new MyStatusBar(this);
	SetStatusBar(status_bar_);
	//SetStatusText("no selection", 0);
	status_bar_->Bind(wxEVT_LEFT_UP, &MyTechFrame::OnStatusBarMouseEvent, this);

    m_ribbon->Realize();

    //m_logwindow = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
    //    wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY |
    //    wxTE_LEFT | wxTE_BESTWRAP | wxBORDER_NONE);
	//m_logwindow->Show(false);

	code_view_ = new MyCodeView(this);

	user_view_ = new MyUserView(this);

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
		auto kscale = wxGetApp().GetTechKScale();
		cfg_tv.put("kscale", kscale);
		auto ktype = wxGetApp().GetTechKType();
		cfg_tv.put("ktype", ktype);
		CALC_STICK_TYPE kvtype = CALC_STICK_LINE;
		if(ktype == CALC_KLINE_K) {
			kvtype = CALC_STICK_BAR;
		}
		cfg_tv.put("kvtype", kvtype);
		auto cycle = wxGetApp().GetTechCycleCur();
		size_t cycleex = 0;
		switch (cycle)
		{
		case CYC_ANYSEC: {
			cycleex = wxGetApp().GetTechCycleAnySec();
		} break;
		case CYC_ANYMIN: {
			cycleex = wxGetApp().GetTechCycleAnyMin();
		} break;
		}
		cfg_tv.put("cycle", cycle);
		cfg_tv.put("cycleex", cycleex);
		auto tv = new zqdb::TechView(this, (const char*)&cfg_tv, XUtil::XML_FLAG_PTREE);
		tech_view_ = tv;
		//tech_view_->SetScrollbars(10, 10, 100, 240);
	}

	//m_togglePanels = new wxToggleButton(this, wxID_ANY, "&Toggle panels");
	//m_togglePanels->SetValue(true);
	//m_togglePanels->Show(false);

	wxSizer *s = new wxBoxSizer(wxVERTICAL);

	s->Add(m_ribbon, 0, wxEXPAND);

#if wxUSE_INFOBAR
	s->Add(infobar_, wxSizerFlags().Expand());
#endif//

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
		auto opt_infoview = cfg_rightview.get_child_optional("infoview");
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
		if (opt_infoview) {
			if (add_spacer) {
				sizer_rightview->AddSpacer(1);
			}
			add_spacer = true;
			info_view_ = new zqdb::InfoView(this, (const char*)&opt_infoview.get(), XUtil::XML_FLAG_PTREE);
			sizer_rightview->Add(info_view_, 0, wxEXPAND);
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

	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyTechFrame::OnMarketExchange, this, ID_MARKET_EXCHANGE, ID_MARKET_EXCHANGE_MAX);
	Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MyTechFrame::OnExchangeDropdown, this, ID_MARKET_EXCHANGE, ID_MARKET_EXCHANGE_MAX);
	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyTechFrame::OnMarketProduct, this, ID_MARKET_PRODUCT, ID_MARKET_PRODUCT_MAX);
	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyTechFrame::OnMarketAllSubscribe, this, ID_MARKET_ALL_SUBSCRIBE);
	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyTechFrame::OnMarketSubscribe, this, ID_MARKET_SUBSCRIBE);
	Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MyTechFrame::OnMarketSubscribeDropdown, this, ID_MARKET_SUBSCRIBE);
	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyTechFrame::OnFilter, this, ID_FILTER, ID_FILTER_MAX);
	Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MyTechFrame::OnFilterDropdown, this, ID_FILTER, ID_FILTER_MAX);
	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyTechFrame::OnSort, this, ID_SORT, ID_SORT_MAX);
	Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MyTechFrame::OnSortDropdown, this, ID_SORT, ID_SORT_MAX);
	Bind(wxEVT_RIBBONTOOLBAR_CLICKED, &MyTechFrame::OnTechKLine, this, ID_TECHVIEW_KLINE, ID_TECHVIEW_KLINE_MAX);
	Bind(wxEVT_RIBBONTOOLBAR_CLICKED, &MyTechFrame::OnTechCycle, this, ID_TECHVIEW_CYCLE, ID_TECHVIEW_CYCLE_MAX);
	Bind(wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED, &MyTechFrame::OnTechCycleDropdown, this, ID_TECHVIEW_CYCLE, ID_TECHVIEW_CYCLE_MAX);
	Bind(wxEVT_RIBBONTOOLBAR_CLICKED, &MyTechFrame::OnTechMove, this, ID_TECHVIEW_MOVE_AUTO, ID_TECHVIEW_MOVE_SHIFT);
	Bind(wxEVT_RIBBONTOOLBAR_CLICKED, &MyTechFrame::OnTechDrawline, this, ID_TECHVIEW_DRAWLINE, ID_TECHVIEW_DRAWLINE_MAX);
	Bind(wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED, &MyTechFrame::OnTechDrawlineDropdown, this, ID_TECHVIEW_DRAWLINE, ID_TECHVIEW_DRAWLINE_MAX);
	Bind(wxEVT_RIBBONTOOLBAR_CLICKED, &MyTechFrame::OnTechZoom, this, ID_TECHVIEW_ZOOM_IN, ID_TECHVIEW_ZOOM_RESET);
	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyTechFrame::OnScript, this, ID_SCRIPT_BEGIN, ID_SCRIPT_END);
	Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MyTechFrame::OnScriptDropdown, this, ID_SCRIPT_BEGIN, ID_SCRIPT_END);

	Bind(wxEVT_MENU, &MyTechFrame::OnNavigateGoto, this, ID_NAVIGATE_GOTO, ID_NAVIGATE_GOTO_MAX);
	Bind(wxEVT_MENU, &MyTechFrame::OnMarketAllGoto, this, ID_MARKET_EXCHANGE, ID_MARKET_EXCHANGE_MAX);
	Bind(wxEVT_MENU, &MyTechFrame::OnMarketAllGoto, this, ID_MARKET_EXCHANGE_PRODUCT, ID_MARKET_EXCHANGE_PRODUCT_MAX);
	Bind(wxEVT_MENU, &MyTechFrame::OnProductGoto, this, ID_MARKET_PRODUCT, ID_MARKET_PRODUCT_MAX);
	Bind(wxEVT_MENU, &MyTechFrame::OnMarketSubscribeGoto, this, ID_MARKET_ALL_SUBSCRIBE);
	Bind(wxEVT_MENU, &MyTechFrame::OnFilterGoto, this, ID_FILTER, ID_FILTER_MAX);
	Bind(wxEVT_MENU, &MyTechFrame::OnSortGoto, this, ID_SORT, ID_SORT_MAX);
	Bind(wxEVT_MENU, &MyTechFrame::OnTechCycleGoto, this, ID_TECHVIEW_CYCLE, ID_TECHVIEW_CYCLE_MAX);
	Bind(wxEVT_MENU, &MyTechFrame::OnTechDrawlineGoto, this, ID_TECHVIEW_DRAWLINE, ID_TECHVIEW_DRAWLINE_MAX);
	Bind(wxEVT_MENU, &MyTechFrame::OnScriptGoto, this, ID_SCRIPT_BEGIN, ID_SCRIPT_END);

	/*Bind(ZQDB_NOTIFY_ENABLE_EVENT, &MyTechFrame::OnNotify, this);
	Bind(ZQDB_NOTIFY_DISABLE_EVENT, &MyTechFrame::OnNotify, this);
	Bind(ZQDB_NOTIFY_APPEND_EVENT, &MyTechFrame::OnNotify, this);
	Bind(ZQDB_NOTIFY_REMOVE_EVENT, &MyTechFrame::OnNotify, this);
	Bind(ZQDB_NOTIFY_UPDATE_EVENT, &MyTechFrame::OnNotify, this);*/

	// test that event handlers pushed on top of MDI children do work (this
	// used to be broken, see #11225)
	//PushEventHandler(new EventHandler(this));
}

MyTechFrame::~MyTechFrame()
{
	//PopEventHandler(true);

	wxGetApp().ResetFrame(this);
}

void MyTechFrame::AddFirstPages()
{
	zqdb::AllExchange allexchange;

	wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("开始"));

	auto navigate_panel = new wxRibbonPanel(home, wxID_ANY, wxT("导航"));
	wxRibbonButtonBar *navigate_bar = new wxRibbonButtonBar(navigate_panel);
	navigate_bar->AddHybridButton(ID_NAVIGATE_BACKWARD, wxT("返回"), skin_info_ptr_->GetBitmap32(wxT("返回")));
	navigate_bar->SetButtonTextMinWidth(ID_NAVIGATE_BACKWARD, wxT("上一个"));
	navigate_bar->SetButtonMaxSizeClass(ID_NAVIGATE_BACKWARD, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	navigate_bar->SetButtonMinSizeClass(ID_NAVIGATE_BACKWARD, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	navigate_bar->AddHybridButton(ID_NAVIGATE_FORWARD, wxT("前进"), skin_info_ptr_->GetBitmap32(wxT("前进")));
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
	navigate_bar->AddHybridButton(ID_MARKET_ALL, wxT(SMARTKB_KEY_ALL), skin_info_ptr_->GetBitmap32(wxT("全部")));
		//, wxArtProvider::GetBitmap(wxART_GO_HOME, wxART_OTHER, wxSize(32, 32)));
	navigate_bar->SetButtonMaxSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	navigate_bar->SetButtonMinSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	//navigate_bar->AddButton(ID_NAVIGATE_NEW, "新窗口", skin_info_ptr_->GetBitmap32(wxEmptyString));
	//navigate_bar->SetButtonMaxSizeClass(ID_NAVIGATE_NEW, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	//navigate_bar->SetButtonMinSizeClass(ID_NAVIGATE_NEW, wxRIBBON_BUTTONBAR_BUTTON_LARGE);

	/*auto fast_access_panel = new wxRibbonPanel(home, wxID_ANY, "快速访问");
	wxRibbonButtonBar* fast_access_bar = new wxRibbonButtonBar(fast_access_panel);
	fast_access_bar->AddButton(ID_MARKET_ALL, "全部", skin_info_ptr_->GetBitmap32(wxEmptyString));
	fast_access_bar->SetButtonMaxSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_MARKET_MAIN, "主力", skin_info_ptr_->GetBitmap32(wxEmptyString));
	fast_access_bar->SetButtonMaxSizeClass(ID_MARKET_MAIN, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_MARKET_MAIN, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_MARKET_SELF, "自选", skin_info_ptr_->GetBitmap32(wxEmptyString), wxEmptyString);
	fast_access_bar->SetButtonMaxSizeClass(ID_MARKET_SELF, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_MARKET_SELF, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_TRADE_POSITION, "持仓", skin_info_ptr_->GetBitmap32(wxEmptyString));
	fast_access_bar->SetButtonMaxSizeClass(ID_TRADE_POSITION, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_TRADE_POSITION, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_TRADE_ORDER, "委托", skin_info_ptr_->GetBitmap32(wxEmptyString));
	fast_access_bar->SetButtonMaxSizeClass(ID_TRADE_ORDER, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_TRADE_ORDER, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->AddButton(ID_TRADE_TRADE, "成交", skin_info_ptr_->GetBitmap32(wxEmptyString));
	fast_access_bar->SetButtonMaxSizeClass(ID_TRADE_TRADE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	fast_access_bar->SetButtonMinSizeClass(ID_TRADE_TRADE, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);*/

	//auto market_panel = new wxRibbonPanel(home, wxID_ANY, "快速导航");
	//wxRibbonButtonBar* market_bar = new wxRibbonButtonBar(market_panel);
	//int product_id = ID_MARKET_PRODUCT;
	//for (size_t i = 0; i < allexchange.size(); i++)
	//{
	//	zqdb::Exchange exchange(allexchange[i]);
	//	//auto item = market_bar->AddButton(ID_MARKET_EXCHANGE + i, utf2wxString(exchange->Exchange), skin_info_ptr_->GetBitmap32(wxEmptyString));
	//	//market_bar->SetItemClientData(item, allexchange[i]);
	//	//market_bar->SetButtonMaxSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	//	//market_bar->SetButtonMinSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	//	/*auto market_panel = new wxRibbonPanel(home, ID_MARKET_EXCHANGE + i, utf2wxString(exchange->Exchange));
	//	market_panel->SetToolTip(utf2wxString(exchange->Name));
	//	wxRibbonButtonBar* market_bar = new wxRibbonButtonBar(market_panel);*/
	//	////这里可以显示最活跃品种
	//	//zqdb::AllProduct allproduct(allexchange[i]);
	//	///*std::sort(allproduct.begin(), allproduct.end(), [](HZQDB x, HZQDB y) {
	//	//	zqdb::Product xproduct(x);
	//	//	zqdb::Product yproduct(y);
	//	//	return strcmp(xproduct->Name, yproduct->Name) < 0;
	//	//});*/
	//	//for (size_t k = 0; k < allproduct.size() && k < 2; k++)
	//	//{
	//	//	zqdb::Product product(allproduct[k]);
	//	//	auto item = market_bar->AddButton(product_id, utf2wxString(product->Name), skin_info_ptr_->GetBitmap32(wxEmptyString));
	//	//	market_bar->SetItemClientData(item, allproduct[k]);
	//	//	market_bar->SetButtonMaxSizeClass(product_id, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	//	//	market_bar->SetButtonMinSizeClass(product_id, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	//	//	++product_id;
	//	//}
	//	//这里显示市场下拉菜单，可以查看所有品种
	//	auto item = market_bar->AddHybridButton(ID_MARKET_EXCHANGE + i
	//		, utf2wxString(exchange->Exchange), skin_info_ptr_->GetBitmap32(wxEmptyString));
	//	market_bar->SetItemClientData(item, allexchange[i]);
	//	market_bar->SetButtonMaxSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	//	market_bar->SetButtonMinSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	//}
	//ASSERT(product_id < ID_MARKET_PRODUCT_MAX);

	auto filter_panel = new wxRibbonPanel(home, wxID_ANY, wxT("筛选"), wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	filter_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyTechFrame::OnFilterSetting, this);
	filter_bar_ = new wxRibbonButtonBar(filter_panel);
	InnerUpdateFilter(allexchange);

	//点击一次降序、二次升序、三次不排序
	auto sort_panel = new wxRibbonPanel(home, wxID_ANY, wxT("排序"), wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	sort_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyTechFrame::OnSortSetting, this);
	sort_bar_ = new wxRibbonButtonBar(sort_panel);
	InnerUpdateSort();

	wxRibbonPanel *analysis_panel = new wxRibbonPanel(home, wxID_ANY, "技术分析",
		wxNullBitmap, wxDefaultPosition, wxDefaultSize,
		wxRIBBON_PANEL_NO_AUTO_MINIMISE |
		wxRIBBON_PANEL_EXT_BUTTON);
	analysis_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyTechFrame::OnTechSetting, this);
	tech_bar_ = new wxRibbonToolBar(analysis_panel);
	//线型
	tech_bar_->AddToggleTool(ID_TECHVIEW_KLINE + CALC_KLINE_K, skin_info_ptr_->GetBitmap16(wxT("K线")));
	tech_bar_->AddToggleTool(ID_TECHVIEW_KLINE + CALC_KLINE_BAR, skin_info_ptr_->GetBitmap16(wxT("美国线")));
	tech_bar_->AddToggleTool(ID_TECHVIEW_KLINE + CALC_KLINE_TREND, skin_info_ptr_->GetBitmap16(wxT("趋势线")));
	tech_bar_->AddSeparator();
	tech_bar_->ToggleTool(ID_TECHVIEW_KLINE + wxGetApp().GetTechKType(), true);
	//周期
	tech_bar_->AddToggleTool(ID_TECHVIEW_CYCLE + CYC_TICK, skin_info_ptr_->GetBitmap16(wxT("走势")));
	for (int cycle = CYC_MIN; cycle < CYC_MAX; cycle++)
	{
		bool add = cycle == wxGetApp().GetTechCycleCur();
		if (!add) {
			switch (cycle)
			{
			case CYC_TICK:
			case CYC_ANYSEC:
			case CYC_15MIN:
			case CYC_30MIN:
			case CYC_ANYMIN:
			case CYC_WEEK:
			case CYC_MONTH:
			case CYC_SEASON:
			case CYC_YEAR:
				break;
			default: {
				add = true;
			} break;
			}
		}
		if (add) {
			wxString str = ZQDBCycle2Str((PERIODTYPE)cycle);
			tech_bar_->AddToggleTool(ID_TECHVIEW_CYCLE + cycle, skin_info_ptr_->GetBitmap16(str));
		}
	}
	tech_bar_->AddDropdownTool(ID_TECHVIEW_CYCLE + CYC_MAX, skin_info_ptr_->GetBitmap16(wxT("更多"))); //all
	tech_bar_->AddSeparator();
	tech_bar_->ToggleTool(ID_TECHVIEW_CYCLE + wxGetApp().GetTechCycleCur(), true);
	//划线
	tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE, skin_info_ptr_->GetBitmap16(wxT("光标"))); //恢复
	tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_DDLINE, skin_info_ptr_->GetBitmap16(wxT("直线")));
	tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_DOTDOT, skin_info_ptr_->GetBitmap16(wxT("线段")));
	tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_PLINE, skin_info_ptr_->GetBitmap16(wxT("平行线")));
	//tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_GUNSLINE, skin_info_ptr_->GetBitmap16(wxT("通道线")));
	tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_CYCLELINE, skin_info_ptr_->GetBitmap16(wxT("周期线")));
	tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_FABCYCLELINE, skin_info_ptr_->GetBitmap16(wxT("斐波那契周期线")));
	tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_GOLDLINE, skin_info_ptr_->GetBitmap16(wxT("黄金分割线")));
	//tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_TEXT, skin_info_ptr_->GetBitmap16(wxT("文字")));
	//tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_UPARROW, skin_info_ptr_->GetBitmap16(wxT("上箭头")));
	//tech_bar_->AddTool(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_DOWNARROW, skin_info_ptr_->GetBitmap16(wxT("下箭头")));
	tech_bar_->AddDropdownTool(ID_TECHVIEW_DRAWLINE_MAX, skin_info_ptr_->GetBitmap16(wxT("更多"))); //All
	tech_bar_->AddSeparator();
	//缩放
	tech_bar_->AddTool(ID_TECHVIEW_ZOOM_IN, skin_info_ptr_->GetBitmap16(wxT("缩小")));
	tech_bar_->AddTool(ID_TECHVIEW_ZOOM_OUT, skin_info_ptr_->GetBitmap16(wxT("放大")));
	tech_bar_->AddSeparator();
	//移动
	//tech_bar_->AddToggleTool(ID_TECHVIEW_MOVE_AUTO, skin_info_ptr_->GetBitmap16(wxT("自动滚动")));
	//tech_bar_->AddToggleTool(ID_TECHVIEW_MOVE_SHIFT, skin_info_ptr_->GetBitmap16(wxT("转移图表")));
	//tech_bar_->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_GOTO_FIRST, wxART_OTHER, wxSize(16, 15)));
	//tech_bar_->AddToggleTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_OTHER, wxSize(16, 15)));
	//tech_bar_->AddTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_OTHER, wxSize(16, 15)));
	//tech_bar_->AddToggleTool(wxID_ANY, wxArtProvider::GetBitmap(wxART_GOTO_LAST, wxART_OTHER, wxSize(16, 15)));
	tech_bar_->AddSeparator();
	/*//加减指标
	tech_bar_->AddDropdownTool(wxID_ADD, wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize(16, 15)));
	tech_bar_->AddDropdownTool(wxID_REMOVE, wxArtProvider::GetBitmap(wxART_MINUS, wxART_OTHER, wxSize(16, 15)));
	tech_bar_->AddSeparator();*/
	/*//技术模板
	tech_bar_->AddHybridTool(wxID_NEW, wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(16, 15)));
	tech_bar_->AddTool(wxID_OPEN, wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 15)), "Open something");
	tech_bar_->AddTool(wxID_SAVE, wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, wxSize(16, 15)), "Save something");
	tech_bar_->AddTool(wxID_SAVEAS, wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_OTHER, wxSize(16, 15)), "Save something as ...");
	tech_bar_->EnableTool(wxID_OPEN, false);
	tech_bar_->EnableTool(wxID_SAVE, false);
	tech_bar_->EnableTool(wxID_SAVEAS, false);
	tech_bar_->AddSeparator();*/
	//
	tech_bar_->SetRows(3, 3);

	/*auto template_panel = new wxRibbonPanel(home, wxID_ANY, "模板");
	wxRibbonButtonBar* template_bar = new wxRibbonButtonBar(template_panel);
	template_bar->AddButton(ID_TEMPLATE_SAVE, "保存", skin_info_ptr_->GetBitmap32(wxEmptyString));
	template_bar->SetButtonMaxSizeClass(ID_TEMPLATE_SAVE, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	template_bar->SetButtonMinSizeClass(ID_TEMPLATE_SAVE, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	template_bar->AddButton(ID_TEMPLATE_NEW_WINDOW, "新窗口", skin_info_ptr_->GetBitmap32(wxEmptyString));
	template_bar->SetButtonMaxSizeClass(ID_TEMPLATE_NEW_WINDOW, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	template_bar->SetButtonMinSizeClass(ID_TEMPLATE_NEW_WINDOW, wxRIBBON_BUTTONBAR_BUTTON_LARGE);*/

	//脚本
	auto script_panel = new wxRibbonPanel(home, wxID_ANY, wxT("快速脚本"), wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	script_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyTechFrame::OnScriptSetting, this);
	script_bar_ = new wxRibbonButtonBar(script_panel);
	InnerUpdateScript();

	/*auto other_panel = new wxRibbonPanel(home, wxID_ANY, "其他", wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	wxRibbonButtonBar* other_bar = new wxRibbonButtonBar(other_panel);
	other_bar->AddButton(ID_CLEAR_SETTINGS, "重新设置", skin_info_ptr_->GetBitmap32(wxEmptyString));
	other_bar->SetButtonMaxSizeClass(ID_CLEAR_SETTINGS, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	other_bar->SetButtonMinSizeClass(ID_CLEAR_SETTINGS, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	other_bar->AddButton(ID_CLEAR_DATA, "清理数据", skin_info_ptr_->GetBitmap32(wxEmptyString));
	other_bar->SetButtonMaxSizeClass(ID_CLEAR_DATA, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	other_bar->SetButtonMinSizeClass(ID_CLEAR_DATA, wxRIBBON_BUTTONBAR_BUTTON_LARGE);*/
	//other_bar->AddButton(ID_HELP_ABOUT, "关于", skin_info_ptr_->GetBitmap32(wxEmptyString));
	//other_bar->SetButtonMaxSizeClass(ID_HELP_ABOUT, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
	//other_bar->SetButtonMinSizeClass(ID_HELP_ABOUT, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
}

void MyTechFrame::AddLastPages()
{
	wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("新窗口"));
	last_pages_[scheme] = [this](wxRibbonBarEvent& evt) {
		wxGetApp().Goto(nullptr);
	};
}

void MyTechFrame::InnerUpdateFilter(const zqdb::AllExchange& allexchange)
{
	filter_bar_->ClearButtons();
	filter_bar_->AddButton(ID_MARKET_ALL_SUBSCRIBE, wxT(SMARTKB_KEY_ALL_SUBSCRIBE), skin_info_ptr_->GetBitmap32(wxT("全部")));
	//, wxArtProvider::GetBitmap(wxART_GO_HOME, wxART_OTHER, wxSize(32, 32)));
	filter_bar_->SetButtonMaxSizeClass(ID_MARKET_ALL_SUBSCRIBE, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	filter_bar_->SetButtonMinSizeClass(ID_MARKET_ALL_SUBSCRIBE, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	filter_bar_->AddHybridButton(ID_MARKET_SUBSCRIBE, wxT(SMARTKB_KEY_SUBSCRIBE), skin_info_ptr_->GetBitmap32(wxT(SMARTKB_KEY_SUBSCRIBE)));
	//, wxArtProvider::GetBitmap(wxART_GO_HOME, wxART_OTHER, wxSize(32, 32)));
	filter_bar_->SetButtonMaxSizeClass(ID_MARKET_SUBSCRIBE, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	filter_bar_->SetButtonMinSizeClass(ID_MARKET_SUBSCRIBE, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	//filter_bar_->AddHybridButton(ID_MARKET_ALL, "市场", skin_info_ptr_->GetBitmap32(wxEmptyString));
	//filter_bar_->SetButtonMaxSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	//filter_bar_->SetButtonMinSizeClass(ID_MARKET_ALL, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	for (size_t i = 0; i < allexchange.size(); i++)
	{
		zqdb::Exchange exchange(allexchange[i]);
		//这里显示市场下拉菜单，可以查看所有品种
		auto name = utf2wxString(exchange->Name);
		auto item = filter_bar_->AddHybridButton(ID_MARKET_EXCHANGE + i, name, skin_info_ptr_->GetBitmap32(name));
		filter_bar_->SetItemClientData(item, allexchange[i]);
		filter_bar_->SetButtonMaxSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		filter_bar_->SetButtonMinSizeClass(ID_MARKET_EXCHANGE + i, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	}
	filter_bar_->AddButton(ID_MARKET_MAIN, wxT(SMARTKB_KEY_MAIN), skin_info_ptr_->GetBitmap32(wxT("主力")));
	filter_bar_->SetButtonMaxSizeClass(ID_MARKET_MAIN, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	filter_bar_->SetButtonMinSizeClass(ID_MARKET_MAIN, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	/*filter_bar_->AddHybridButton(ID_MARKET_NEW_FILTER, "更多"
	, wxArtProvider::GetBitmap(wxART_PLUS, wxART_OTHER, wxSize(32, 32)));
	filter_bar_->SetButtonMaxSizeClass(ID_MARKET_NEW_FILTER, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	filter_bar_->SetButtonMinSizeClass(ID_MARKET_NEW_FILTER, wxRIBBON_BUTTONBAR_BUTTON_LARGE);*/
	if (!all_filter_func_.empty()) {
		ASSERT(cur_filter_func_ < all_filter_func_.size());
		zqdb::Calc::Func func(all_filter_func_[cur_filter_func_]);
		if (all_filter_func_.size() <= 1) {
			filter_bar_->AddButton(ID_FILTER + cur_filter_func_, func.GetCalcName(), skin_info_ptr_->GetBitmap32(wxT("筛选")));
		}
		else {
			filter_bar_->AddHybridButton(ID_FILTER + cur_filter_func_, func.GetCalcName(), skin_info_ptr_->GetBitmap32(wxT("筛选")));
		}
		filter_bar_->SetButtonMaxSizeClass(ID_FILTER + cur_filter_func_, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		filter_bar_->SetButtonMinSizeClass(ID_FILTER + cur_filter_func_, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	}
}

void MyTechFrame::InnerSetCurFilter(size_t cur)
{
	ASSERT(cur_filter_func_ < all_filter_func_.size());
	filter_bar_->DeleteButton(ID_FILTER + cur_filter_func_);
	cur_filter_func_ = cur;
	ASSERT(cur_filter_func_ < all_filter_func_.size());
	zqdb::Calc::Func func(all_filter_func_[cur_filter_func_]);
	wxString name = func.GetCalcName();
	filter_bar_->AddHybridButton(ID_FILTER + cur_filter_func_, name, skin_info_ptr_->GetBitmap32(wxT("筛选")));
	filter_bar_->SetButtonMaxSizeClass(ID_FILTER + cur_filter_func_, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	filter_bar_->SetButtonMinSizeClass(ID_FILTER + cur_filter_func_, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	wxGetApp().SetSortCalcFunc(name);
}

void MyTechFrame::InnerUpdateSort()
{
	sort_bar_->ClearButtons();
	sort_bar_->AddButton(ID_SORT, wxT("涨跌幅"), skin_info_ptr_->GetBitmap32(wxT("涨跌幅")));
	//, wxArtProvider::GetBitmap(wxART_GO_UP, wxART_OTHER, wxSize(32, 32)));
	sort_bar_->SetButtonMaxSizeClass(ID_SORT, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	sort_bar_->SetButtonMinSizeClass(ID_SORT, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	sort_bar_->AddHybridButton(ID_SORT_QUICK
		, wxString::Format(wxT("%zu分钟涨跌"), wxGetApp().GetSortQuick() / 60), skin_info_ptr_->GetBitmap32(wxT("涨跌速")));
	//, wxArtProvider::GetBitmap(wxART_GO_UP, wxART_OTHER, wxSize(32, 32)));
	sort_bar_->SetButtonMaxSizeClass(ID_SORT_QUICK, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	sort_bar_->SetButtonMinSizeClass(ID_SORT_QUICK, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	if (!all_sort_func_.empty()) {
		ASSERT(cur_sort_func_ < all_sort_func_.size());
		zqdb::Calc::Func func(all_sort_func_[cur_sort_func_]); 
		if (all_sort_func_.size() <= 1) {
			sort_bar_->AddButton(ID_SORT_CALC + cur_sort_func_, func.GetCalcName(), skin_info_ptr_->GetBitmap32(wxT("排序")));
		}
		else {
			sort_bar_->AddHybridButton(ID_SORT_CALC + cur_sort_func_, func.GetCalcName(), skin_info_ptr_->GetBitmap32(wxT("排序")));
		}
		sort_bar_->SetButtonMaxSizeClass(ID_SORT_CALC + cur_sort_func_, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		sort_bar_->SetButtonMinSizeClass(ID_SORT_CALC + cur_sort_func_, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	}
	MY_CODE_SORT_TYPE type;
	size_t secs = 0;
	int sort = IsSort(&type, &secs);
	if (sort) {
		switch (type)
		{
		case SORT_ZDF:
			sort_bar_->SetButtonText(ID_SORT, sort > 0 ? wxT("涨跌幅▼") : wxT("涨跌幅▲"));
			break;
		case SORT_ZDS:
			sort_bar_->SetButtonText(ID_SORT_QUICK
				, wxString::Format(wxT("%zu分钟涨跌%s"), secs / 60, sort > 0 ? wxT("▼") : wxT("▲")));
			break;
		case SORT_CALC_SORT: {
			zqdb::Calc::Func func(all_sort_func_[cur_sort_func_]);
			wxString name = func.GetCalcName();
			sort_bar_->SetButtonText(ID_SORT_CALC + cur_sort_func_, sort > 0 ? name + wxT("▼") : name + wxT("▲"));
		} break;
		}
	}
}

void MyTechFrame::InnerSetCurSort(size_t cur)
{
	ASSERT(cur_sort_func_ < all_sort_func_.size());
	sort_bar_->DeleteButton(ID_SORT_CALC + cur_sort_func_);
	cur_sort_func_ = cur;
	ASSERT(cur_sort_func_ < all_sort_func_.size());
	zqdb::Calc::Func func(all_sort_func_[cur_sort_func_]);
	wxString name = func.GetCalcName();
	sort_bar_->AddHybridButton(ID_SORT_CALC + cur_sort_func_, name, skin_info_ptr_->GetBitmap32(wxT("指标排序")));
	sort_bar_->SetButtonMaxSizeClass(ID_SORT_CALC + cur_sort_func_, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	sort_bar_->SetButtonMinSizeClass(ID_SORT_CALC + cur_sort_func_, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	wxGetApp().SetSortCalcFunc(name);
}

void MyTechFrame::InnerUpdateScript()
{
	script_bar_->ClearButtons();
	/*script_bar->AddHybridButton(ID_SCRIPT_BEGIN, "快买", skin_info_ptr_->GetBitmap32(wxEmptyString));
	script_bar->SetButtonMaxSizeClass(ID_SCRIPT_BEGIN, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	script_bar->SetButtonMinSizeClass(ID_SCRIPT_BEGIN, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	script_bar->AddHybridButton(ID_SCRIPT_BEGIN + 1, "快卖", skin_info_ptr_->GetBitmap32(wxEmptyString));
	script_bar->SetButtonMaxSizeClass(ID_SCRIPT_BEGIN + 1, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	script_bar->SetButtonMinSizeClass(ID_SCRIPT_BEGIN + 1, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	script_bar->AddHybridButton(ID_SCRIPT_BEGIN + 2, "快平", skin_info_ptr_->GetBitmap32(wxEmptyString));
	script_bar->SetButtonMaxSizeClass(ID_SCRIPT_BEGIN + 2, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	script_bar->SetButtonMinSizeClass(ID_SCRIPT_BEGIN + 2, wxRIBBON_BUTTONBAR_BUTTON_LARGE);*/
	for (size_t i = 0, j = all_script_func_.size(); i < j; i++)
	{
		zqdb::Calc::Func func(all_script_func_[i]);
		script_bar_->AddHybridButton(ID_SCRIPT_BEGIN + i, func.GetCalcName(), skin_info_ptr_->GetBitmap32(wxT("脚本")));
		script_bar_->SetButtonMaxSizeClass(ID_SCRIPT_BEGIN + i, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		script_bar_->SetButtonMinSizeClass(ID_SCRIPT_BEGIN + i, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	}
}

int MyTechFrame::FilterEvent(wxEvent& event)
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

void MyTechFrame::OnSkinInfoChanged()
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
	m_ribbon->ClearPages();
	if (skin_info_ptr_->artProvider) {
		m_ribbon->SetArtProvider(skin_info_ptr_->artProvider->Clone());
	}
	else {
		ASSERT(0);
		m_ribbon->GetArtProvider()->SetColourScheme(skin_info_ptr_->crPrimary, skin_info_ptr_->crSecondary, skin_info_ptr_->crTertiary);
		skin_info_ptr_->artProvider = m_ribbon->GetArtProvider()->Clone();
		/*skin_info_ptr_->fontName = skin_info_ptr_->GetArtFont(wxRIBBON_ART_TAB_LABEL_FONT);
		skin_info_ptr_->fontText = skin_info_ptr_->GetArtFont(wxRIBBON_ART_TAB_LABEL_FONT);
		skin_info_ptr_->crRising = skin_info_ptr_->crCtrlRising;
		skin_info_ptr_->crFalling = skin_info_ptr_->crCtrlFalling;*/
		skin_info_ptr_->UpdateTechCycleBitmap16();
		skin_info_ptr_->UpdateTechCycleExBitmap16(CYC_ANYSEC, wxGetApp().GetTechCycleAnySec());
		skin_info_ptr_->UpdateTechCycleExBitmap16(CYC_ANYMIN, wxGetApp().GetTechCycleAnyMin());
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
	if (status_bar_) {
		status_bar_->SetSkinInfo(skin_info_ptr_);
	}
	//
	UpdateAllPages();
	//
	Layout();
	Thaw();
}

void MyTechFrame::OnInfoChanged()
{
	Goto(Get());
	GotoUser(GetUser());
}

void MyTechFrame::OnHandleChanged()
{
	Freeze();
	auto h = Get();
	ASSERT(h && h->type == ZQDB_HANDLE_TYPE_CODE);
	zqdb::Code code(h);
	wxCSConv utf8cv(wxFONTENCODING_UTF8);
	auto frame = wxGetApp().GetFrame();
	if (frame && frame != this) {
		wxString title = wxString::Format(wxT("%s %s")
			, utf8cv.cMB2WX(code->Name).data()
			, utf8cv.cMB2WX(code->Code).data()
		);
		SetTitle(title);
	}
	if (code_view_) {
		code_view_->SetHandle(h);
	}
	if (tech_view_) {
		//切换代码时，先移除所有划线，后期需要做提示是否保存划线
		auto major = tech_view_->GetMajor();
		if (major) {
			major->RemoveAllDrawline();
		}
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

void MyTechFrame::OnUserChanged()
{
	if (tech_view_) {
		tech_view_->SetUser(GetUser());
	}
	if (user_view_) {
		user_view_->SetUser(GetUser());
	}
	if (trade_view_) {
		trade_view_->SetUser(GetUser());
	}
}

HZQDB MyTechFrame::All()
{
	return code_view_->All();
}

void MyTechFrame::Goto(HZQDB h)
{
	code_view_->Goto(h);
}

void MyTechFrame::Goto(size_t pos)
{
	if (pos < record_list_.size()) {
		record_pos_ = pos;
		auto record = record_list_[record_pos_];
		code_view_->Goto(record);
	}
}

void MyTechFrame::Backward()
{
	if (record_pos_ > 0 && !record_list_.empty()) {
		--record_pos_;
		auto record = record_list_[record_pos_];
		code_view_->Goto(record);
	}
}

void MyTechFrame::Forward()
{
	if ((record_pos_ + 1) < record_list_.size()) {
		++record_pos_;
		auto record = record_list_[record_pos_];
		code_view_->Goto(record);
	}
}

void MyTechFrame::ShowKey(const wxString& key)
{
	code_view_->ShowKey(key);
}

void MyTechFrame::ShowFilter(const wxString& key, const zqdb::Calc::Filter& filter)
{
	code_view_->ShowFilter(key, filter);
}

void MyTechFrame::UpdateFilter()
{
	zqdb::AllExchange allexchange;
	InnerUpdateFilter(allexchange);
	m_ribbon->Realize();
}

void MyTechFrame::GotoUser(HZQDB user)
{
	user_view_->Goto(user);
}

int MyTechFrame::IsSort(MY_CODE_SORT_TYPE* type, size_t* secs)
{
	return code_view_->IsSort(type, secs);
}

void MyTechFrame::Sort()
{
	code_view_->Sort();
}

void MyTechFrame::SortByZD(MY_CODE_SORT_TYPE type, size_t duration, int sort)
{
	return code_view_->SortByZD(type, duration, sort);
}

void MyTechFrame::SortByField(MDB_FIELD& field, int sort)
{
	return code_view_->SortByField(field, sort);
}

void MyTechFrame::SortByCalc(const zqdb::Calc::Sort& calc, int sort)
{
	return code_view_->SortByCalc(calc, sort);
}

void MyTechFrame::OnNotifyEnable(HZQDB h)
{
	if (h->type == ZQDB_HANDLE_TYPE_MODULE) {
		UpdateFilter();
	}
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
	if (status_bar_) {
		status_bar_->OnNotifyEnable(h);
	}
}

void MyTechFrame::OnNotifyDisable(HZQDB h)
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
	if (status_bar_) {
		status_bar_->OnNotifyDisable(h);
	}
}

void MyTechFrame::OnNotifyAppend(HZQDB h)
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
	if (status_bar_) {
		status_bar_->OnNotifyAppend(h);
	}
}

void MyTechFrame::OnNotifyRemove(HZQDB h)
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
	if (status_bar_) {
		status_bar_->OnNotifyRemove(h);
	}
}

void MyTechFrame::OnNotifyUpdate(HZQDB h)
{
	if (h->type == ZQDB_HANDLE_TYPE_CALCFUNC) {
		zqdb::Calc::Func func(h);
		switch (func.GetCalcType())
		{
		case CALC_MAJOR: {

		} break;
		case CALC_MINOR: {

		} break;
		case CALC_FILTER: {
			all_filter_func_.Update();
			zqdb::AllExchange allexchange;
			InnerUpdateFilter(allexchange);
			m_ribbon->Realize();
		} break;
		case CALC_SORT: {
			all_sort_func_.Update();
			InnerUpdateSort();
			m_ribbon->Realize();
		} break;
		case CALC_SCRIPT: {
			all_script_func_.Update();
			InnerUpdateScript();
			m_ribbon->Realize();
			
		} break;
		case CALC_STRATEGY: {

		} break;
		}
	}

	zqdb::AllExchange allexchange;
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
	if (status_bar_) {
		status_bar_->OnNotifyUpdate(h);
	}
}

void MyTechFrame::ShowTips(const char* xml, size_t xmlflag)
{
	SetStatusText(wxGetApp().GetLastTips());
}

void MyTechFrame::OnStatusBarMouseEvent(wxMouseEvent& event)
{
	wxRect rect;
	GetStatusBar()->GetFieldRect(0, rect);
	if (rect.Contains(event.GetPosition()))
	{
		MyLogDlg::ShowDlg(GetStatusBar()->GetScreenPosition(), rect.width);
	}
}

//void MyTechFrame::OnNotify(wxCommandEvent& event)
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

void MyTechFrame::OnNavigateHome(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
}

void MyTechFrame::OnNavigateBackward(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	Backward();
}

void MyTechFrame::OnNavigateBackwardDropdown(wxRibbonButtonBarEvent& evt)
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

void MyTechFrame::OnNavigateBackwardUpdateUI(wxUpdateUIEvent& evt)
{
	evt.Enable(record_pos_ > 0);
}

void MyTechFrame::OnNavigateForward(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	Forward();
}

void MyTechFrame::OnNavigateForwardDropdown(wxRibbonButtonBarEvent& evt)
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

void MyTechFrame::OnNavigateForwardUpdateUI(wxUpdateUIEvent& evt)
{
	evt.Enable((record_pos_ + 1) < record_list_.size());
}

void MyTechFrame::OnNavigateGoto(wxCommandEvent& evt)
{
	auto id = evt.GetId();
	if (id >= ID_NAVIGATE_GOTO && id < ID_NAVIGATE_GOTO_MAX)
	{
		auto pos = id - ID_NAVIGATE_GOTO;
		Goto(pos);
	}
}

void MyTechFrame::OnNavigateUp(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	//Up();
}

void MyTechFrame::OnNavigateDown(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	//Down();
}

void MyTechFrame::OnNavigateFind(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	
}

void MyTechFrame::OnMarketAll(wxRibbonButtonBarEvent& evt)
{
	ShowKey(wxT(SMARTKB_KEY_ALL));
}

void MyTechFrame::OnMarketAllDropdown(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		wxMenu menu;
		wxGetApp().ClearMenuMap();
		size_t exchange_id = ID_MARKET_EXCHANGE;
		size_t product_id = ID_MARKET_EXCHANGE_PRODUCT;
		zqdb::AllExchange allexchange;
		for (size_t i = 0; i < allexchange.size(); i++)
		{
			wxMenu *submenu = new wxMenu;
			zqdb::Exchange exchange(allexchange[i]);
			auto menu_item = submenu->Append(exchange_id, utf2wxString(exchange->Name));
			wxGetApp().SetMenuData(exchange_id++, allexchange[i]);
			submenu->AppendSeparator();
			zqdb::AllProduct allproduct(allexchange[i]);
			/*std::sort(allproduct.begin(), allproduct.end(), [](HZQDB x, HZQDB y) {
			zqdb::Product xproduct(x);
			zqdb::Product yproduct(y);
			return strcmp(xproduct->Name, yproduct->Name) < 0;
			});*/
			for (size_t j = 0; j < allproduct.size(); j++)
			{
				zqdb::Product product(allproduct[j]);
				submenu->Append(product_id, utf2wxString(product->Name));
				wxGetApp().SetMenuData(product_id++, allproduct[j]);
			}
			menu.AppendSubMenu(submenu, utf2wxString(exchange->Name));
		}
		evt.PopupMenu(&menu);
	}
}

void MyTechFrame::OnMarketAllGoto(wxCommandEvent& evt)
{
	auto id = evt.GetId();
	if ((id >= ID_MARKET_EXCHANGE && id < ID_MARKET_EXCHANGE_MAX)
		|| (id >= ID_MARKET_EXCHANGE_PRODUCT && id < ID_MARKET_EXCHANGE_PRODUCT_MAX)) {
		HZQDB h = (HZQDB)wxGetApp().GetMenuData(id);
		if (h) {
			Goto(h);
		}
	}
}

void MyTechFrame::OnMarketMain(wxRibbonButtonBarEvent& evt)
{
	ShowKey(wxT(SMARTKB_KEY_MAIN));
}

void MyTechFrame::OnMarketMainDropdown(wxRibbonButtonBarEvent& evt)
{
	OnFilterDropdown(evt);
}

void MyTechFrame::OnMarketExchange(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		HZQDB h = static_cast<HZQDB>(pBar->GetItemClientData(evt.GetButton()));
		zqdb::Exchange exchange(h);
		ShowFilter(utf2wxString(exchange->Name), zqdb::Calc::Container(h));
	}
}

void MyTechFrame::OnExchangeDropdown(wxRibbonButtonBarEvent& evt)
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

void MyTechFrame::OnProductGoto(wxCommandEvent& evt)
{
	auto id = evt.GetId();
	if (id >= ID_MARKET_PRODUCT && id < ID_MARKET_PRODUCT_MAX)
	{
		HZQDB h = (HZQDB)wxGetApp().GetMenuData(id);
		if (h) {
			zqdb::Product product(h);
			ShowFilter(utf2wxString(product->Name), zqdb::Calc::Container(h));
		}
	}
}

void MyTechFrame::OnMarketProduct(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		HZQDB h = static_cast<HZQDB>(pBar->GetItemClientData(evt.GetButton()));
		Goto(h);
	}
}

void MyTechFrame::OnMarketAllSubscribe(wxRibbonButtonBarEvent& evt)
{
	ShowKey(wxT(SMARTKB_KEY_ALL_SUBSCRIBE));
}

void MyTechFrame::OnMarketSubscribe(wxRibbonButtonBarEvent& evt)
{
	ShowKey(wxT(SMARTKB_KEY_SUBSCRIBE));
}

void MyTechFrame::OnMarketSubscribeDropdown(wxRibbonButtonBarEvent& evt)
{
	/*wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		wxMenu menu;
		wxGetApp().ClearMenuMap();
		menu.Append(ID_MARKET_ALL_SUBSCRIBE, wxT(SMARTKB_KEY_ALL_SUBSCRIBE));
		evt.PopupMenu(&menu);
	}*/
}

void MyTechFrame::OnMarketSubscribeGoto(wxCommandEvent& evt)
{
	//ShowKey(wxT(SMARTKB_KEY_SUBSCRIBE));
}
//
//void MyTechFrame::OnMarketSelfSel(wxRibbonButtonBarEvent& evt)
//{
//	ShowKey(wxT(SMARTKB_KEY_SELFSEL));
//}
//
//void MyTechFrame::OnMarketSelfSelDropdown(wxRibbonButtonBarEvent& evt)
//{
//}

void MyTechFrame::OnFilterNew(wxRibbonButtonBarEvent& evt)
{

}

void MyTechFrame::OnFilter(wxRibbonButtonBarEvent& evt)
{
	size_t pos = evt.GetId() - ID_FILTER;
	if (pos < all_filter_func_.size()) {
		zqdb::Calc::Func func(all_filter_func_[pos]);
		wxRibbonButtonBar* pBar = evt.GetBar();
		if (pBar) {
			zqdb::Calc::Filter filter(func.GetCalcName(), All(), nullptr);
			ShowFilter(utf2wxString(filter.GetCalcName()), filter);
		}
	}
}

void MyTechFrame::OnFilterDropdown(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		wxMenu menu;
		wxGetApp().ClearMenuMap();
		for (size_t i = 0, j = all_filter_func_.size(); i < j; i++)
		{
			zqdb::Calc::Func func(all_filter_func_[i]);
			if (i == cur_filter_func_) {
				continue;
			}
			else {
				auto menu_item = menu.Append(ID_FILTER + i, utf2wxString(func.GetCalcName()));
			}
			wxGetApp().SetMenuData(ID_FILTER + i, all_filter_func_[i]);
		}
		evt.PopupMenu(&menu);
	}
}

void MyTechFrame::OnFilterGoto(wxCommandEvent& evt)
{
	size_t pos = evt.GetId() - ID_FILTER;
	if (pos < all_filter_func_.size()) {
		InnerSetCurFilter(pos);
		m_ribbon->Realize();
		zqdb::Calc::Func func(all_filter_func_[pos]);
		zqdb::Calc::Filter filter(func.GetCalcName(), All(), nullptr);
		ShowFilter(utf2wxString(filter.GetCalcName()), filter);
	}
}

void MyTechFrame::OnFilterSetting(wxRibbonPanelEvent& evt)
{
	if (cur_filter_func_ < all_filter_func_.size()) {
		zqdb::Calc::Func func(all_filter_func_[cur_filter_func_]);
		zqdb::TechDlg dlg(this, CALC_FILTER, func.GetCalcName());
		dlg.ShowModal();
	}
}

void MyTechFrame::Sort(int id)
{
	MY_CODE_SORT_TYPE old_type = SORT_ZDF;
	size_t old_secs = 0;
	int old_sort = IsSort(&old_type, &old_secs);

	MY_CODE_SORT_TYPE type = SORT_ZDF;
	int sort = 0;
	if (id < ID_SORT_CALC) {
		switch (id)
		{
		case ID_SORT: {
		} break;
		default: {
			type = SORT_ZDS;
		} break;
		}
	}
	else {
		type = SORT_CALC_SORT;
	}

	if (id < ID_SORT_CALC) {
		size_t secs = 0;
		switch (type)
		{
		case SORT_ZDF:
			break;
		case SORT_ZDS:
			secs = wxGetApp().GetSortQuick();
			break;
		}
		if (type != old_type) {
			sort = 0;
		}
		else {
			if (secs != old_secs) {
				sort = 0;
			}
			else {
				sort = old_sort;
			}
		}
		switch (sort)
		{
		case -1:
			sort = 0;
			break;
		case 0:
			sort = 1;
			break;
		case 1:
			sort = -1;
			break;
		}
		SortByZD(type, secs, sort);
	}
	else {
		size_t pos = id - ID_SORT_CALC;
		if (type != old_type) {
			sort = 0;
		}
		else if (pos != cur_sort_func_) {
			sort = 0;
			InnerSetCurSort(pos);
		}
		else {
			sort = old_sort;
		}
		zqdb::Calc::Func func(all_sort_func_[pos]);
		switch (sort)
		{
		case -1:
			sort = 0;
			break;
		case 0:
			sort = 1;
			break;
		case 1:
			sort = -1;
			break;
		}
		zqdb::Calc::Sort calc(func.GetCalcName(), nullptr);
		SortByCalc(calc, sort);
	}
	InnerUpdateSort();
	m_ribbon->Realize();
}

void MyTechFrame::OnSort(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		Sort(evt.GetId());
	}
}

void MyTechFrame::OnSortDropdown(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		wxMenu menu;
		switch (evt.GetId()) 
		{
		case ID_SORT: {
			//
		} break;
		case ID_SORT_QUICK: {
			menu.Append(ID_SORT_QUICK + 1, wxT("1分钟涨跌"));
			menu.Append(ID_SORT_QUICK + 2, wxT("2分钟涨跌"));
			menu.Append(ID_SORT_QUICK + 3, wxT("3分钟涨跌"));
			menu.Append(ID_SORT_QUICK + 4, wxT("4分钟涨跌"));
			menu.Append(ID_SORT_QUICK + 5, wxT("5分钟涨跌"));
		} break;
		default: {
			wxGetApp().ClearMenuMap();
			for (size_t i = 0, j = all_sort_func_.size(); i < j; i++)
			{
				zqdb::Calc::Func func(all_sort_func_[i]);
				if (i == cur_sort_func_) {
					continue;
				}
				else {
					auto menu_item = menu.Append(ID_SORT_CALC + i, utf2wxString(func.GetCalcName()));
				}
				wxGetApp().SetMenuData(ID_SORT_CALC + i, all_sort_func_[i]);
			}
		} break;
		}
		evt.PopupMenu(&menu);
	}
}

void MyTechFrame::OnSortGoto(wxCommandEvent& evt)
{
	if (evt.GetId() > ID_SORT_QUICK && evt.GetId() <= ID_SORT_QUICK_MAX) {
		size_t secs = 0;
		switch (evt.GetId()) 
		{
		case ID_SORT_QUICK + 1: {
			secs = 60;
		} break;
		case ID_SORT_QUICK + 2: {
			secs = 120;
		} break;
		case ID_SORT_QUICK + 3: {
			secs = 180;
		} break;
		case ID_SORT_QUICK + 4: {
			secs = 240;
		} break;
		case ID_SORT_QUICK + 5: {
			secs = 300;
		} break;
		default: {
			wxASSERT(0);
		} break;
		}
		wxGetApp().SetSortQuick(secs);
	}
	Sort(evt.GetId());
}

void MyTechFrame::OnSortSetting(wxRibbonPanelEvent& evt)
{
	if (cur_sort_func_ < all_sort_func_.size()) {
		zqdb::Calc::Func func(all_sort_func_[cur_sort_func_]);
		zqdb::TechDlg dlg(this, CALC_SORT, func.GetCalcName());
		dlg.ShowModal();
	}
}

void MyTechFrame::SetTechKLine(CALC_KLINE_TYPE type)
{
	if (!tech_view_) {
		return;
	}
	if (!tech_bar_) {
		return;
	}
	for (size_t i = CALC_KLINE_K; i < CALC_KLINE_TYPE_MAX; i++) 
	{
		if (type == (CALC_KLINE_TYPE)i) {
			tech_bar_->ToggleTool(ID_TECHVIEW_KLINE + i, true);
		}
		else {
			tech_bar_->ToggleTool(ID_TECHVIEW_KLINE + i, false);
		}
	}
	switch (type)
	{
	case CALC_KLINE_TREND: {
		tech_view_->SetKType(CALC_KLINE_TREND, CALC_STICK_LINE);
	} break;
	case CALC_KLINE_BAR: {
		tech_view_->SetKType(CALC_KLINE_BAR, CALC_STICK_LINE);
	} break;
	default: {
		tech_view_->SetKType(CALC_KLINE_K, CALC_STICK_BAR);
	} break;
	}
	wxGetApp().SetTechKType(type);
}

void MyTechFrame::OnTechKLine(wxRibbonToolBarEvent& evt)
{
	CALC_KLINE_TYPE type = (CALC_KLINE_TYPE)(evt.GetId() - ID_TECHVIEW_KLINE);
	switch (type)
	{
	/*case CALC_KLINE_TREND: {
		tech_bar_->ToggleTool(ID_TECHVIEW_KLINE + CALC_KLINE_TREND, false);
		wxMessageBox(wxT("暂不支持该功能，敬请期待..."));
		//tech_view_->SetKType(CALC_KLINE_TREND, CALC_STICK_LINE);
	} break;*/
	default: {
		SetTechKLine(type);
	} break;
	}
}

void MyTechFrame::SetTechCycle(PERIODTYPE cycle)
{
	if (!tech_view_) {
		return;
	}
	if (!tech_bar_) {
		return;
	}
	auto old_cycle = tech_view_->GetCycle();
	if (old_cycle == cycle) {
		tech_bar_->ToggleTool(ID_TECHVIEW_CYCLE + cycle, false);
	}
	else if (cycle == CYC_TICK) {
		tech_bar_->ToggleTool(ID_TECHVIEW_CYCLE + cycle, false);
		wxMessageBox(wxT("暂不支持该功能，敬请期待..."));
	}
	else {
		//切换周期时，先移除所有划线，后期需要做提示是否保存划线
		auto major = tech_view_->GetMajor();
		if (major) {
			major->RemoveAllDrawline();
		}
		if (tech_bar_->FindById(ID_TECHVIEW_CYCLE + old_cycle)) {
			tech_bar_->ToggleTool(ID_TECHVIEW_CYCLE + old_cycle, false);
		}
		else {
			//
		}
		if (tech_bar_->FindById(ID_TECHVIEW_CYCLE + cycle)) {
			tech_bar_->ToggleTool(ID_TECHVIEW_CYCLE + cycle, true);
		}
		else {
			//
		}
		size_t cycleex = 0;
		switch (cycle)
		{
		case CYC_ANYSEC: {
			cycleex = wxGetApp().GetTechCycleAnySec();
		} break;
		case CYC_ANYMIN: {
			cycleex = wxGetApp().GetTechCycleAnyMin();
		} break;
		}
		tech_view_->SetCycle(cycle, cycleex);
		wxGetApp().SetTechCycleCur(cycle);
	}
}

void MyTechFrame::OnTechCycle(wxRibbonToolBarEvent& evt)
{
	SetTechCycle((PERIODTYPE)(evt.GetId() - ID_TECHVIEW_CYCLE));
}

void MyTechFrame::OnTechCycleDropdown(wxRibbonToolBarEvent& evt)
{
	if (!tech_view_) {
		return;
	}
	wxRibbonToolBar* pBar = evt.GetBar();
	if (!pBar) {
		return;
	}

	wxMenu menu;
	wxGetApp().ClearMenuMap();
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_TICK, wxT("走势"));
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_5SEC, wxT("5秒"));
	size_t anysec = wxGetApp().GetTechCycleAnySec();
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_ANYSEC, wxString::Format(wxT("%zu秒"), anysec));
	size_t anymin = wxGetApp().GetTechCycleAnyMin();
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_1MIN, wxT("1分"));
	size_t min = 1, premin = 1;
	for (size_t cycle = CYC_5MIN; cycle < CYC_DAY; cycle++)
	{
		premin = min;
		switch (cycle)
		{
		case CYC_5MIN: {
			min = 5;
		} break;
		case CYC_15MIN: {
			min = 15;
		} break;
		case CYC_30MIN: {
			min = 30;
		} break;
		case CYC_60MIN: {
			min = 60;
		} break;
		}
		if (anymin > premin && anymin < min) {
			if (anymin % 60 == 0) {
				menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_ANYMIN, wxString::Format(wxT("%zu时"), anymin / 60));
			}
			else {
				menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_ANYMIN, wxString::Format(wxT("%zu分"), anymin));
			}
		}
		switch (cycle)
		{
		case CYC_5MIN: {
			menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_5MIN, wxT("5分"));
		} break;
		case CYC_15MIN: {
			menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_15MIN, wxT("15分"));
		} break;
		case CYC_30MIN: {
			menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_30MIN, wxT("30分"));
		} break;
		case CYC_60MIN: {
			menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_60MIN, wxT("60分"));
		} break;
		}
	}
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_DAY, wxT("日"));
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_WEEK, wxT("周"));
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_MONTH, wxT("月"));
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_SEASON, wxT("季"));
	menu.AppendCheckItem(ID_TECHVIEW_CYCLE + CYC_YEAR, wxT("年"));
	menu.Check(ID_TECHVIEW_CYCLE + (int)tech_view_->GetCycle(), true);
	evt.PopupMenu(&menu);
}

void MyTechFrame::OnTechCycleGoto(wxCommandEvent& evt)
{
	auto cycle = (PERIODTYPE)(evt.GetId() - ID_TECHVIEW_CYCLE);
	bool relayout = !tech_bar_->FindById(ID_TECHVIEW_CYCLE + cycle);
	if (relayout) {
		for (int i = cycle - 1; i > CYC_TICK; i--)
		{
			auto pos = tech_bar_->GetToolPos(ID_TECHVIEW_CYCLE + i);
			if (pos >= 0) {
				wxString str = ZQDBCycle2Str(cycle);
				tech_bar_->InsertToggleTool(pos, ID_TECHVIEW_CYCLE + cycle, skin_info_ptr_->GetBitmap16(str));
				break;
			}
		}
	}
	SetTechCycle(cycle);
	if (relayout) {
		m_ribbon->Realize();
	}
}

void MyTechFrame::OnTechMove(wxRibbonToolBarEvent& evt)
{
	wxMessageBox(wxT("暂不支持该功能，敬请期待..."));
}

void MyTechFrame::DoDrawline(CALC_DRAWLINE_TYPE type)
{
	if (!tech_view_) {
		return;
	}
	auto major = tech_view_->GetMajor();
	if (!major) {
		return;
	}
	size_t pos = 0;
	auto obj = major->IsInDrawline(&pos);
	if (obj) {
		//画线中就先取消画线
		major->RemoveIndicator(pos);
		major->Refresh();
	}
	auto name = ZQDBGetDrawlineName(type);
	if (!name || !name[0]) {
		return;
	}
	char xml[1024] = { 0 };
	sprintf(xml, R"({"name":"%s"})", name);
	major->AppendDrawline(name, xml, XUtil::XML_FLAG_JSON_STRING);
}

void MyTechFrame::OnTechDrawline(wxRibbonToolBarEvent& evt)
{
	DoDrawline((CALC_DRAWLINE_TYPE)(evt.GetId() - ID_TECHVIEW_DRAWLINE));
}

void MyTechFrame::OnTechDrawlineDropdown(wxRibbonToolBarEvent& evt)
{
	if (!tech_view_) {
		return;
	}
	wxRibbonToolBar* pBar = evt.GetBar();
	if (!pBar) {
		return;
	}
	wxMenu menu;
	wxGetApp().ClearMenuMap();
	menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_DDLINE, wxT("直线"));
	menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_DOTDOT, wxT("线段"));
	menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_PLINE, wxT("平行线"));
	//menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_GUNSLINE, wxT("通道线"));
	menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_CYCLELINE, wxT("周期线"));
	menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_FABCYCLELINE, wxT("斐波那契周期线"));
	menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_GOLDLINE, wxT("黄金分割线"));
	//menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_TEXT, wxT("文字"));
	//menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_UPARROW, wxT("上箭头"));
	//menu.AppendCheckItem(ID_TECHVIEW_DRAWLINE + CALC_DRAWLINE_DOWNARROW, wxT("下箭头"));
	evt.PopupMenu(&menu);
}

void MyTechFrame::OnTechDrawlineGoto(wxCommandEvent& evt)
{
	DoDrawline((CALC_DRAWLINE_TYPE)(evt.GetId() - ID_TECHVIEW_DRAWLINE));
}

void MyTechFrame::OnTechZoom(wxRibbonToolBarEvent& evt)
{
	if (!tech_view_) {
		return;
	}
	switch (evt.GetId())
	{
	case ID_TECHVIEW_ZOOM_IN: {
		tech_view_->Scale(-1);
	} break;
	case ID_TECHVIEW_ZOOM_OUT: {
		tech_view_->Scale(1);
	} break;
	}
}

void MyTechFrame::OnTechSetting(wxRibbonPanelEvent& evt)
{
	zqdb::TechDlg dlg(this);
	dlg.ShowModal();
}

void MyTechFrame::RunScript(HZQDB h)
{
	auto huser = GetUser();
	if (!huser) {
		/*wxMessageBox(wxT("没有登录交易账户!!!\n")
			wxT("请登录交易"), wxT("提示"), wxOK | wxCANCEL);
		return;*/
	}
	zqdb::Calc::Func func(h);
	HZQDB input = zqdb::TechDlg::GetInputAttr(h);
	size_t target = zqdb::TechDlg::GetTarget(h);
	HZQDB data = Get();
	if (target) {
		//
	}
	zqdb::Calc::Calculator script(func.GetCalcName(), CALC_SCRIPT, data, input, huser);
	if (input) {
		ZQDBCloseCalcInput(input);
	}
}

void MyTechFrame::OnScript(wxRibbonButtonBarEvent& evt)
{
	//直接执行
	size_t pos = evt.GetId() - ID_SCRIPT_BEGIN;
	if (pos < all_script_func_.size()) {
		RunScript(all_script_func_[pos]);
	}
}

void MyTechFrame::OnScriptDropdown(wxRibbonButtonBarEvent& evt)
{
	//zqdb::Calc::AllFunc allscript(CALC_SCRIPT);
	//size_t i = evt.GetId() - ID_SCRIPT_BEGIN;
	wxMenu menu;
	menu.Append(evt.GetId(), wxT("设置"));
	evt.PopupMenu(&menu);
}

void MyTechFrame::OnScriptGoto(wxCommandEvent& evt)
{
	/*if (script_property_dlg_)
	{
	delete script_property_dlg_;
	script_property_dlg_ = NULL;
	}
	script_property_dlg_ = new MyScriptPropertyDlg(this);
	wxPoint pt = wxGetMousePosition();
	script_property_dlg_->Position(pt, wxSize(0, 0));
	script_property_dlg_->Show();*/
	size_t pos = evt.GetId() - ID_SCRIPT_BEGIN;
	if (pos < all_script_func_.size()) {
		zqdb::Calc::Func func(all_script_func_[pos]);
		zqdb::TechDlg dlg(this, CALC_SCRIPT, func.GetCalcName());
		if (wxID_OK == dlg.ShowModal()) {
			//
		}
	}
}

void MyTechFrame::OnScriptSetting(wxRibbonPanelEvent& evt)
{
	zqdb::TechDlg dlg(this, CALC_SCRIPT);
	dlg.ShowModal();
}

void MyTechFrame::OnClearSettings(wxRibbonButtonBarEvent& evt)
{
	wxGetApp().Resettings();
}

void MyTechFrame::OnClearData(wxRibbonButtonBarEvent& evt)
{
	wxGetApp().ClearData();
}

void MyTechFrame::OnTimer(wxTimerEvent& evt)
{
	Sort();
	if (user_view_) {
		user_view_->GetEventHandler()->ProcessEvent(evt);
	}
	if (status_bar_) {
		status_bar_->GetEventHandler()->ProcessEvent(evt);
	}
	evt.Skip();
}

