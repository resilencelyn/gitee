#include <myctp.h>
#include <zqctp.h>
#include <zqdb.pb.h>
#include<wx/generic/statbmpg.h>
#include <wx/gbsizer.h>

#include <wx/xy/xyplot.h>
#include <wx/xy/xylinerenderer.h>

#include <wx/axis/numberaxis.h>
#include <wx/axis/dateaxis.h>

#include <wx/xy/timeseriesdataset.h>

#include "../zqapp/circle.xpm"

#ifdef _DEBUG

const std::set<wxString> g_names = {
	_("PreDelta")
	,_("CurrDelta")
	,_("TradingDay")
	,_("LoginTime")
	,_("SystemName")
	,_("FrontID")
	,_("SessionID")
	,_("MaxOrderRef")
	,_("InvestorGroupID")
	,_("IdentifiedCardType")
	,_("IdentifiedCardNo")
	,_("IsActive")
	,_("Telephone")
	,_("Address")
	,_("OpenDate")
	,_("Mobile")
	,_("CommModelID")
	,_("MarginModelID")
	,_("PreMortgage")
	,_("PreCredit")
	,_("PreDeposit")
	,_("PreBalance")
	,_("PreMargin")
	,_("InterestBase")
	,_("Interest")
	,_("Deposit")
	,_("Withdraw")
	,_("FrozenMargin")
	,_("FrozenCash")
	,_("FrozenCommission")
	,_("CurrMargin")
	,_("CashIn")
	,_("Commission")
	,_("CloseProfit")
	,_("PositionProfit")
	,_("Balance")
	,_("Available")
	,_("WithdrawQuota")
	,_("Reserve")
	,_("TradingDay")
	,_("SettlementID")
	,_("Credit")
	,_("Mortgage")
	,_("ExchangeMargin")
	,_("DeliveryMargin")
	,_("ExchangeDeliveryMargin")
	,_("ReserveBalance")
	,_("CurrencyID")
	,_("PreFundMortgageIn")
	,_("PreFundMortgageOut")
	,_("FundMortgageAvailable")
	,_("MortgageableFund")
	,_("SpecProductMargin")
	,_("SpecProductFrozenMargin")
	,_("SpecProductCommission")
	,_("SpecProductPositionProfit")
	,_("SpecProductCloseProfit")
	,_("SpecProductPositionProfitByAlg")
	,_("SpecProductExchangeMargin")
	,_("BizType")
	,_("FrozenSwap")
	,_("RemainSwap")
	,_("InstrumentID")
	,_("InvestorRange")
	,_("OpenRatioByMoney")
	,_("OpenRatioByVolume")
	,_("CloseRatioByMoney")
	,_("CloseRatioByVolume")
	,_("CloseTodayRatioByMoney")
	,_("CloseTodayRatioByVolume")
	,_("ExchangeID")
	,_("BizType")
	,_("InvestUnitID")
};

#endif

static MyCTPModule* g_module = nullptr;

static const std::array<const char*, 5> const sz_exchanges = {
	EXCHANGE_CFFEX,EXCHANGE_CZCE,EXCHANGE_DCE,EXCHANGE_SHFE,EXCHANGE_INE
};

// -- MyCTPLoginView -- 

const wxString picName = "";
MyCTPLoginView::MyCTPLoginView(wxWindow* parent, const char* xml, size_t xmlflag):Base(parent, xml, xmlflag)
{
    wxBitmap bitmap(circle_xpm);
    wxGenericStaticBitmap* pTitlePic = new wxGenericStaticBitmap(this, wxID_ANY, bitmap);
    pTitlePic->SetScaleMode(wxStaticBitmapBase::Scale_AspectFit);

    wxStaticText* pTitle = new wxStaticText(this, wxID_ANY, wxT("中文标题"), wxDefaultPosition, wxSize(200, wxDefaultCoord), wxALIGN_CENTER);
    wxFont titleDefaultFont = pTitle->GetFont();
    titleDefaultFont.SetPixelSize(titleDefaultFont.GetPixelSize() * 2);
    pTitle->SetFont(titleDefaultFont);

    wxStaticText* pUserNameLbl = new wxStaticText(this, wxID_ANY, L"用户:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    wxStaticText* pPasswordNameLbl = new wxStaticText(this, wxID_ANY, L"密码:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    wxStaticText* pCodeLbl = new wxStaticText(this, wxID_ANY, L"机构码:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    m_pTip = new wxStaticText(this, wxID_ANY, L"tip......");

    m_pUserNameTxt = new wxTextCtrl(this, wxID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    m_pPasswordNameTxt = new wxTextCtrl(this, wxID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    m_pCodeTxt = new wxTextCtrl(this, wxID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize);

    wxGridBagSizer* gridBagSizer = new wxGridBagSizer(3, 3);
    gridBagSizer->Add(new wxStaticText(), wxGBPosition(0, 3), wxGBSpan(1, 1), wxTILE | wxRIGHT, 10);

    gridBagSizer->Add(pTitlePic, wxGBPosition(0, 0), wxGBSpan(6, 1), wxSHAPED | wxALIGN_CENTER | wxALL, 5);

    gridBagSizer->Add(pUserNameLbl, wxGBPosition(2, 1), wxGBSpan(1, 1), wxALIGN_RIGHT);
    gridBagSizer->Add(pPasswordNameLbl, wxGBPosition(3, 1), wxGBSpan(1, 1), wxALIGN_RIGHT);
    gridBagSizer->Add(pCodeLbl, wxGBPosition(4, 1), wxGBSpan(1, 1), wxALIGN_RIGHT);

    gridBagSizer->Add(pTitle, wxGBPosition(0, 2), wxGBSpan(2, 1), wxEXPAND);
    gridBagSizer->Add(m_pUserNameTxt, wxGBPosition(2, 2), wxGBSpan(1, 1), wxEXPAND);
    gridBagSizer->Add(m_pPasswordNameTxt, wxGBPosition(3, 2), wxGBSpan(1, 1), wxEXPAND);
    gridBagSizer->Add(m_pCodeTxt, wxGBPosition(4, 2), wxGBSpan(1, 1), wxEXPAND);
    gridBagSizer->Add(m_pTip, wxGBPosition(5, 2), wxGBSpan(1, 1));

    gridBagSizer->AddGrowableCol(2);
    gridBagSizer->Fit(this);
    gridBagSizer->SetSizeHints(this);
    SetSizerAndFit(gridBagSizer);
#if 1
	m_pUserNameTxt->SetValue("xxx");
	m_pPasswordNameTxt->SetValue("xxx");
	m_pCodeTxt->SetValue("0000000000000000");
#endif//
}

void MyCTPLoginView::UpdateInfo(const char* const pInfo)
{
    m_pTip->SetLabelText(pInfo);
}

void MyCTPLoginView::EnableUI(bool enable)
{
    m_pUserNameTxt->Enable(enable);
    m_pPasswordNameTxt->Enable(enable);
    m_pCodeTxt->Enable(enable);
}

const char* MyCTPLoginView::StartLogin()
{
    EnableUI(false);
	//发送登录，这里写死了，实际上应该点击登录按钮根据用户填的参数来登录
	{
		zqdb::Msg msg(NET_MSG_REQUEST_DO);
		msg.SetReqID(com::zqdb::proto::msg::MSG_REQUEST_DO_TD_USER_LOGIN);
		auto user = wxString2utf(m_pUserNameTxt->GetValue());
		auto pwd = wxString2utf(m_pPasswordNameTxt->GetValue());
		auto auth_code = wxString2utf(m_pCodeTxt->GetValue());
#if 1
		userid_ = user + ".broker";
		msg.SetParam("User", user.c_str());
		com::zqdb::proto::msg::ReqLogin req;
		req.set_broker("1010");
		req.set_user(user);
		req.set_password(pwd);
		req.set_code(auth_code);
		req.set_appid("client_mytrader_1.0.0");
		req.set_tdaddress("tcp://125.71.232.79:41407");
		req.set_mdaddress("tcp://125.71.232.79:41415");
#endif//
		auto str = req.SerializeAsString();
		msg.SetData(str.data(), str.size());
		ZQDBRequest(*g_module, msg, nullptr, 0);
	}
	return userid_.c_str();
}

void MyCTPLoginView::CancelLogin()
{

}

int MyCTPLoginView::OnNetMsg(zqdb::Msg& msg)
{
	auto type = msg.GetMsgType();
	auto reqtype = msg.GetReqType();
	int ret = 0;
	switch (type)
	{
	case NET_MSG_NOTIFY: {
		switch (reqtype)
		{
		case com::zqdb::proto::msg::MSG_NOTIFY_TD_USER_LOGIN: {
			//收到登录通知，登录成功还是失败参看errorcode，提示参看errormsg
			auto errorcode = msg.GetParamAsInt(STR_ZQDB_MSG_ERROR_CODE);
			auto errormsg = msg.GetParam(STR_ZQDB_MSG_ERROR_MESSAGE);
			ret = 1;
		} break;
		}
	} break;
	case NET_MSG_REQUEST_DO: {
		//
	} break;
	case NET_MSG_REQUEST_QUERY: {
		//
	} break;
	case NET_MSG_RESPONSE: {
		switch (reqtype)
		{
		case com::zqdb::proto::msg::MSG_REQUEST_DO_TD_USER_LOGIN: {
			//发送登录请求成功
			ret = 1;
		} break;
		}
	} break;
	default:
		break;
	}
	return ret;
}

// -- MyCTPUserInfoView -- 

MyCTPUserInfoView::MyCTPUserInfoView(wxWindow *parent, const char* xml, size_t xmlflag) :Base(parent, xml, xmlflag)
{
	ctrl_user_list_ = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition,
		FromDIP(wxSize(240, 160)));
	ctrl_user_list_model_ = new zqdb::HZQDBModel(*g_module, "./myctp/userinfo.json", XUtil::XML_FLAG_JSON_FILE);
	ctrl_user_list_->AssociateModel(ctrl_user_list_model_.get());
	ctrl_user_list_->AppendTextColumn(wxT("名称"), 0, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE);
	ctrl_user_list_->AppendTextColumn(wxT("数值"), 1, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE);

	ctrl_account_list_ = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition,
		FromDIP(wxSize(240, 80)));
	ctrl_account_list_model_ = new zqdb::HZQDBListModel(*g_module, "./myctp/accountlist.json", XUtil::XML_FLAG_JSON_FILE);
	ctrl_account_list_->AssociateModel(ctrl_account_list_model_.get());
	auto& ctrl_account_col_infos = ctrl_account_list_model_->GetColInfo();
	for (size_t i = 0, j = ctrl_account_col_infos.size(); i < j; i++)
	{
		auto& col_info = ctrl_account_col_infos[i];
		ctrl_account_list_->AppendTextColumn(col_info.name, i, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE);
	}

	ctrl_investor_list_ = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition,
		FromDIP(wxSize(240, 80)));
	ctrl_investor_list_model_ = new zqdb::HZQDBListModel(*g_module, "./myctp/investorlist.json", XUtil::XML_FLAG_JSON_FILE);
	ctrl_investor_list_->AssociateModel(ctrl_investor_list_model_.get());
	auto& ctrl_investor_col_infos = ctrl_investor_list_model_->GetColInfo();
	for (size_t i = 0, j = ctrl_investor_col_infos.size(); i < j; i++)
	{
		auto& col_info = ctrl_investor_col_infos[i];
		ctrl_investor_list_->AppendTextColumn(col_info.name, i, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE);
	}

	wxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	topSizer->Add(ctrl_user_list_, 0, wxEXPAND);

	wxSizer* sizer_user_v = new wxBoxSizer(wxVERTICAL);
	sizer_user_v->Add(ctrl_account_list_, 0, wxEXPAND);
	sizer_user_v->Add(ctrl_investor_list_, 0, wxEXPAND);
	topSizer->Add(sizer_user_v, 1, wxEXPAND);

	SetSizer(topSizer);
}

void MyCTPUserInfoView::OnSkinInfoChanged()
{
	Base::OnSkinInfoChanged();
}

void MyCTPUserInfoView::OnUserChanged() 
{
	ctrl_user_list_model_->SetHandle(user_);
	zqdb::AllUserInvestor allinvestor(user_, *g_module);
	ctrl_investor_list_model_->Show(allinvestor);
	zqdb::AllUserAccount allaccount(user_, *g_module);
	ctrl_account_list_model_->Show(allaccount);
}


// -- MyCTPAccountStatView -- 

MyCTPAccountStatView::MyCTPAccountStatView(wxWindow *parent, const char* xml, size_t xmlflag) :Base(parent, xml, xmlflag)
{
	chart_ = new wxChartPanel(this);
#ifdef wxUSE_GRAPHICS_CONTEXT
	chart_->SetAntialias(true);
#endif

	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	topSizer->Add(chart_, 1, wxEXPAND);

	SetSizer(topSizer);

	RefreshAll();
}

void MyCTPAccountStatView::RefreshAll()
{
	uint32_t date = 0, time = g_module->GetTradeTime(&date, nullptr);
	if (!date || !time) {
		return;
	}

	auto now_time = std::chrono::steady_clock::now();
	if ((now_time - last_refresh_time_) < std::chrono::milliseconds(3000)) {
		return;
	}
	last_refresh_time_ = now_time;

	zqdb::AllUserAccount allaccount(user_, *g_module);
	zqdb::ObjectT<tagCTPAccountInfo> account(allaccount[0]); 
	double data_max = account->Balance;
	double data_min = account->Balance;
	auto now_stat_info = g_module->GetAccountStatInfo(account);
	auto max_time_point = 60;//ZQDBGetMaxTimePoint(trade_time, trade_time_count);
	if (datas_.empty()) {
		datas_.reserve(max_time_point + 10);
		times_.reserve(max_time_point + 10);
#if 1
		for (size_t i = max_time_point; i > 0; i--)
		{
			datas_.emplace_back(account->Balance);
			auto ttime = XUtil::PrevMinute(XUtil::GetHour(time), XUtil::GetMinute(time), i);
			times_.emplace_back(XUtil::make_time_t(XUtil::GetYear(date), XUtil::GetMonth(date), XUtil::GetDay(date),
				XUtil::GetHour(ttime), XUtil::GetMinute(ttime), XUtil::GetSecond(ttime)));
		}
#else
		auto NowTimePoint = ZQDBGetTimePointByHHMMSS(time, trade_time, trade_time_count);
		time = ZQDBGetHHMMSSByTimePoint(0, trade_time, trade_time_count);
		for (size_t i = max_time_point - (NowTimePoint + 1); i > 0; i--)
		{
			datas_.emplace_back(account->Balance);
			auto ttime = XUtil::PrevMinute(XUtil::GetHour(time), XUtil::GetMinute(time), i);
			times_.emplace_back(XUtil::make_time_t(XUtil::GetYear(date), XUtil::GetMonth(date), XUtil::GetDay(date),
				XUtil::GetHour(ttime), XUtil::GetMinute(ttime), XUtil::GetSecond(ttime)));
		}
		bool crossday = ZQDBIsTradeTimeCrossDay(trade_time, trade_time_count);
		for (size_t i = 0, j = NowTimePoint + 1; i < j; i++)
		{
			datas_.emplace_back(account->Balance);
			if (crossday && !ZQDBIsTimePointCrossDay(i, trade_time, trade_time_count)) {
				date = XUtil::PrevDay(tradeday, 1);
			}
			else {
				date = tradeday;
			}
			auto ttime = ZQDBGetHHMMSSByTimePoint(i, trade_time, trade_time_count);
			times_.emplace_back(XUtil::make_time_t(XUtil::GetYear(date), XUtil::GetMonth(date), XUtil::GetDay(date),
				XUtil::GetHour(ttime), XUtil::GetMinute(ttime), XUtil::GetSecond(ttime)));
		}
#endif//
	}
	else {
		bool update = false;
		size_t i = 0;
		for (; i < now_stat_info.size();)
		{
			auto& one = now_stat_info[i];
			if (one.Time > times_.back()) {
				struct tm now_tm = *localtime(&one.Time);
				struct tm old_tm = *localtime(&times_.back());
				auto now_time = XUtil::MakeTime(now_tm.tm_hour, now_tm.tm_min, 0);
				auto old_time = XUtil::MakeTime(old_tm.tm_hour, old_tm.tm_min, 0);
				if (now_time > old_time) {
					break;
				}
				else if (!IsEqualFloat(one.Balance, datas_.back())) {
					datas_.back() = one.Balance;
					update = true;
					i++;
					break;
				}
			}
			i++;
		}
		if (i >= now_stat_info.size() && !update) {
			return;
		}
		for (; i < now_stat_info.size(); i++)
		{
			auto& one = now_stat_info[i];
			datas_.emplace_back(one.Balance);
			times_.emplace_back(one.Time);
		}
		if (datas_.size() > max_time_point) {
			datas_.erase(datas_.begin(), datas_.begin() + datas_.size() - max_time_point);
			times_.erase(times_.begin(), times_.begin() + times_.size() - max_time_point);
		}
		for (auto one : datas_)
		{
			if (data_max < one) {
				data_max = one;
			}
			if (data_min > one) {
				data_min = one;
			}
		}
	}
	TimeSeriesDataset *dataset = new TimeSeriesDataset(datas_.data(), times_.data(), datas_.size());
	if (dataset) {
		XYPlot *plot = new XYPlot();

		dataset->SetRenderer(new XYLineRenderer());

		plot->AddDataset(dataset);

		// add left number and bottom date axes
		NumberAxis *leftAxis = new NumberAxis(AXIS_LEFT);
		DateAxis *bottomAxis = new DateAxis(AXIS_BOTTOM);

		double delta = data_max - data_min;
		if (IsZeroValue(delta)) {
			leftAxis->SetFixedBounds(data_min * 0.95, data_max * 1.05);
		}
		else {
			leftAxis->SetFixedBounds(data_min - delta * 0.075, data_max + delta * 0.075);
		}

		// setup window
		//bottomAxis->SetWindow(0, 10);
		//bottomAxis->SetUseWindow(true);

		bottomAxis->SetVerticalLabelText(true);
		bottomAxis->SetDateFormat(wxT("%H:%M"));

		// add axes to first plot
		plot->AddAxis(leftAxis);
		plot->AddAxis(bottomAxis);

		// link axes and dataset
		plot->LinkDataVerticalAxis(0, 0);
		plot->LinkDataHorizontalAxis(0, 0);

		// and finally create chart
		Chart *chart = new Chart(plot, wxEmptyString);

		//chart->SetScrolledAxis(bottomAxis);

		chart_->SetChart(chart);
	}
}

void MyCTPAccountStatView::OnSkinInfoChanged()
{
	Base::OnSkinInfoChanged();
}

void MyCTPAccountStatView::OnUserChanged()
{
}
//
//void MyCTPAccountStatView::OnTimer()
//{
//
//}

// -- MyCTPUserOrderView -- 

wxBEGIN_EVENT_TABLE(MyCTPUserOrderView, Base)
EVT_SHOW(MyCTPUserOrderView::OnShowEvent)
wxEND_EVENT_TABLE()

MyCTPUserOrderView::MyCTPUserOrderView(wxWindow *parent, const char* xml, size_t xmlflag) :Base(parent, xml, xmlflag)
{
	ctrl_list_ = new wxDataViewCtrl(this, wxID_ANY);
	ctrl_list_model_ = new zqdb::HZQDBListModel(*g_module, "./myctp/orderlist.json", XUtil::XML_FLAG_JSON_FILE);
	ctrl_list_->AssociateModel(ctrl_list_model_.get());
	auto htb = ZQDBFindTableEx(*g_module, STR_ZQDB_TABLE_ORDER);
	auto& col_infos = ctrl_list_model_->GetColInfo();
	for (size_t i = 0, j = col_infos.size(); i < j; i++)
	{
		auto& col_info = col_infos[i];
		ZQDBNormalizeField(htb, const_cast<MDB_FIELD*>(&col_info.field), 1);
		ctrl_list_->AppendTextColumn(col_info.name, i, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE
			, MDBFieldTypeIsNumber(col_info.field.type) ? wxALIGN_RIGHT : wxALIGN_NOT);
	}

	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	topSizer->Add(ctrl_list_, 1, wxEXPAND);

	SetSizer(topSizer);

	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &MyCTPUserOrderView::OnSelChanged, this);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MyCTPUserOrderView::OnActivated, this);
}

void MyCTPUserOrderView::ShowAll()
{
	wxASSERT(IsDispOk());
	zqdb::AllUserOrder all(user_, *g_module);
	ctrl_list_model_->Show(all);
}

void MyCTPUserOrderView::ClearAll()
{
	ctrl_list_model_->Clear();
}

void MyCTPUserOrderView::RefreshAll()
{
	ctrl_list_->Refresh();
}

void MyCTPUserOrderView::OnShowEvent(wxShowEvent& evt)
{
	if (evt.IsShown() && IsDispOk()) {
		ShowAll();
	}
}

void MyCTPUserOrderView::OnSelChanged(wxDataViewEvent &event)
{
	auto h = ctrl_list_model_->GetData(event.GetItem());
	if (h) {
		zqdb::ObjectT<tagOrderInfo> order(h);
		HZQDB hs[3] = { 0 };
		ZQDBGetCodeByTradeCode(order->Code, hs, 3);
		if (hs[0]) {
			zqdb::GetApp().Goto(hs[0], wxGetTopLevelParent(this));
		}
	}
}

void MyCTPUserOrderView::OnActivated(wxDataViewEvent &event)
{
	auto h = ctrl_list_model_->GetData(event.GetItem());
	if (h) {
		if (wxOK == wxMessageBox(wxT("确定撤销委托吗？"), wxT("提示"), wxOK | wxCANCEL | wxCENTRE)) {
			zqdb::GetApp().OrderCancel(user_, h);
		}
	}
}

// -- MyCTPUserTradeView -- 

wxBEGIN_EVENT_TABLE(MyCTPUserTradeView, Base)
EVT_SHOW(MyCTPUserTradeView::OnShowEvent)
wxEND_EVENT_TABLE()

MyCTPUserTradeView::MyCTPUserTradeView(wxWindow *parent, const char* xml, size_t xmlflag) :Base(parent, xml, xmlflag)
{
	ctrl_list_ = new wxDataViewCtrl(this, wxID_ANY);
	ctrl_list_model_ = new zqdb::HZQDBListModel(*g_module, "./myctp/tradelist.json", XUtil::XML_FLAG_JSON_FILE);
	ctrl_list_->AssociateModel(ctrl_list_model_.get());
	auto htb = ZQDBFindTableEx(*g_module, STR_ZQDB_TABLE_TRADE);
	auto& col_infos = ctrl_list_model_->GetColInfo();
	for (size_t i = 0, j = col_infos.size(); i < j; i++)
	{
		auto& col_info = col_infos[i];
		ZQDBNormalizeField(htb, const_cast<MDB_FIELD*>(&col_info.field), 1);
		ctrl_list_->AppendTextColumn(col_info.name, i, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE
			, MDBFieldTypeIsNumber(col_info.field.type) ? wxALIGN_RIGHT : wxALIGN_NOT);
	}

	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	topSizer->Add(ctrl_list_, 1, wxEXPAND);

	SetSizer(topSizer);

	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &MyCTPUserTradeView::OnSelChanged, this);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MyCTPUserTradeView::OnActivated, this);
}

void MyCTPUserTradeView::ShowAll()
{
	wxASSERT(IsDispOk());
	zqdb::AllUserTrade all(user_, *g_module);
	ctrl_list_model_->Show(all);
}

void MyCTPUserTradeView::ClearAll()
{
	ctrl_list_model_->Clear();
}

void MyCTPUserTradeView::RefreshAll()
{
	ctrl_list_->Refresh();
}

void MyCTPUserTradeView::OnShowEvent(wxShowEvent& evt)
{
	if (evt.IsShown() && IsDispOk()) {
		ShowAll();
	}
}

void MyCTPUserTradeView::OnSelChanged(wxDataViewEvent &event)
{
	auto h = ctrl_list_model_->GetData(event.GetItem());
	if (h) {
		zqdb::ObjectT<tagTradeInfo> trade(h);
		HZQDB hs[3] = { 0 };
		ZQDBGetCodeByTradeCode(trade->Code, hs, 3);
		if (hs[0]) {
			zqdb::GetApp().Goto(hs[0], wxGetTopLevelParent(this));
		}
	}
}

void MyCTPUserTradeView::OnActivated(wxDataViewEvent &event)
{
	OnSelChanged(event);
}

// -- MyCTPUserPositionView -- 

wxBEGIN_EVENT_TABLE(MyCTPUserPositionView, Base)
EVT_SHOW(MyCTPUserPositionView::OnShowEvent)
wxEND_EVENT_TABLE()

MyCTPUserPositionView::MyCTPUserPositionView(wxWindow *parent, const char* xml, size_t xmlflag):Base(parent, xml, xmlflag)
{
	ctrl_list_ = new wxDataViewCtrl(this, wxID_ANY);
	ctrl_list_model_ = new zqdb::HZQDBListModel(*g_module, "./myctp/positionlist.json", XUtil::XML_FLAG_JSON_FILE);
	ctrl_list_->AssociateModel(ctrl_list_model_.get());
	auto htb = ZQDBFindTableEx(*g_module, STR_ZQDB_TABLE_POSITION);
	auto& col_infos = ctrl_list_model_->GetColInfo();
	for (size_t i = 0, j = col_infos.size(); i < j; i++)
	{
		auto& col_info = col_infos[i];
		ZQDBNormalizeField(htb, const_cast<MDB_FIELD*>(&col_info.field), 1);
		ctrl_list_->AppendTextColumn(col_info.name, i, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE
			, MDBFieldTypeIsNumber(col_info.field.type) ? wxALIGN_RIGHT : wxALIGN_NOT);
	}

	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	topSizer->Add(ctrl_list_, 1, wxEXPAND);

	SetSizer(topSizer);

	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &MyCTPUserPositionView::OnSelChanged, this);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MyCTPUserPositionView::OnActivated, this);
}

void MyCTPUserPositionView::ShowAll()
{
	wxASSERT(IsDispOk());
	zqdb::AllUserPosition all(user_, *g_module);
	ctrl_list_model_->Show(all);
}

void MyCTPUserPositionView::ClearAll()
{
	ctrl_list_model_->Clear();
}

void MyCTPUserPositionView::RefreshAll()
{
	ctrl_list_->Refresh();
}

void MyCTPUserPositionView::OnShowEvent(wxShowEvent& evt)
{
	if (evt.IsShown() && IsDispOk()) {
		ShowAll();
	}
}

void MyCTPUserPositionView::OnSelChanged(wxDataViewEvent &event)
{
	auto h = ctrl_list_model_->GetData(event.GetItem());
	if (h) {
		zqdb::ObjectT<tagPositionInfo> position(h);
		HZQDB hs[3] = { 0 };
		ZQDBGetCodeByTradeCode(position->Code, hs, 3);
		if (hs[0]) {
			zqdb::GetApp().Goto(hs[0], wxGetTopLevelParent(this));
		}
	}
}

void MyCTPUserPositionView::OnActivated(wxDataViewEvent &event)
{
	auto h = ctrl_list_model_->GetData(event.GetItem());
	if (h) {
		zqdb::ObjectT<tagPositionInfo> position(h);
		HZQDB hs[3] = { 0 };
		ZQDBGetCodeByTradeCode(position->Code, hs, 3);
		if (hs[0]) {
			zqdb::Code code(hs[0]);
			if (wxOK == wxMessageBox(wxT("确定平仓吗？"), wxT("提示"), wxOK | wxCANCEL | wxCENTRE)) {
				zqdb::GetApp().CloseOrder(user_, h, ORDER_LIMIT, position->Volume - position->FrozenVolume, code->Close);
			}
		} else {
			if (wxOK == wxMessageBox(wxT("确定平仓吗？"), wxT("提示"), wxOK | wxCANCEL | wxCENTRE)) {
				zqdb::GetApp().CloseOrder(user_, h, ORDER_MARKET, position->Volume - position->FrozenVolume, 0);
			}
		}
	}
}

// -- MyCTPUserCommissionRateView -- 

wxBEGIN_EVENT_TABLE(MyCTPUserCommissionRateView, Base)
EVT_SHOW(MyCTPUserCommissionRateView::OnShowEvent)
wxEND_EVENT_TABLE()

MyCTPUserCommissionRateView::MyCTPUserCommissionRateView(wxWindow *parent, const char* xml, size_t xmlflag) :Base(parent, xml, xmlflag)
{
	ctrl_list_ = new wxDataViewCtrl(this, wxID_ANY);
	ctrl_list_model_ = new zqdb::HMTABLEListModel("./myctp/commissionratelist.json", XUtil::XML_FLAG_JSON_FILE);
	ctrl_list_->AssociateModel(ctrl_list_model_.get());
	htb_ = ZQDBGetMTable(ZQDBFindTableEx(*g_module, STR_CTP_TABLE_COMMISSIONRATE), &hdb_);
	ASSERT(hdb_ && htb_);
	auto& col_infos = ctrl_list_model_->GetColInfo();
	for (size_t i = 0, j = col_infos.size(); i < j; i++)
	{
		auto& col_info = col_infos[i];
		MDBTableNormalizeField(hdb_, htb_, const_cast<MDB_FIELD*>(&col_info.field), 1);
		ctrl_list_->AppendTextColumn(col_info.name, i, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE
			, MDBFieldTypeIsNumber(col_info.field.type) ? wxALIGN_RIGHT : wxALIGN_NOT);
	}

	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	topSizer->Add(ctrl_list_, 1, wxEXPAND);

	SetSizer(topSizer);
}

void MyCTPUserCommissionRateView::ShowAll()
{
	wxASSERT(IsDispOk());
	ctrl_list_model_->Show(hdb_, htb_);
}

void MyCTPUserCommissionRateView::ClearAll()
{
	ctrl_list_model_->Clear();
}

void MyCTPUserCommissionRateView::OnShowEvent(wxShowEvent& evt)
{
	if (evt.IsShown() && IsDispOk()) {
		ShowAll();
	}
}

// -- MyCTPMiniView -- 

wxBEGIN_EVENT_TABLE(MyCTPMiniView, Base)
EVT_TIMER(wxID_ANY, MyCTPMiniView::OnTimer)
EVT_SHOW(MyCTPMiniView::OnShowEvent)
wxEND_EVENT_TABLE()

// DISPATCH_ON_TYPE() macro is an ugly way to write the "same" code for
// different wxBookCtrlBase-derived classes without duplicating code and
// without using templates, it expands into "before <xxx> after" where "xxx"
// part is control class-specific
#if wxUSE_NOTEBOOK
#define CASE_NOTEBOOK(x) case Type_Notebook: x; break;
#else
#define CASE_NOTEBOOK(x)
#endif

#if wxUSE_LISTBOOK
#define CASE_LISTBOOK(x) case Type_Listbook: x; break;
#else
#define CASE_LISTBOOK(x)
#endif

#if wxUSE_CHOICEBOOK
#define CASE_CHOICEBOOK(x) case Type_Choicebook: x; break;
#else
#define CASE_CHOICEBOOK(x)
#endif

#if wxUSE_TREEBOOK
#define CASE_TREEBOOK(x) case Type_Treebook: x; break;
#else
#define CASE_TREEBOOK(x)
#endif

#if wxUSE_TOOLBOOK
#define CASE_TOOLBOOK(x) case Type_Toolbook: x; break;
#else
#define CASE_TOOLBOOK(x)
#endif

#if wxUSE_AUI
#define CASE_AUINOTEBOOK(x) case Type_AuiNotebook: x; break;
#else
#define CASE_AUINOTEBOOK(x)
#endif

#define CASE_SIMPLEBOOK(x) case Type_Simplebook: x; break;

#define DISPATCH_ON_TYPE(before, nb, lb, cb, tb, toolb, aui, sb, after)       \
    switch ( m_type )                                                         \
    {                                                                         \
        CASE_NOTEBOOK(before nb after)                                        \
        CASE_LISTBOOK(before lb after)                                        \
        CASE_CHOICEBOOK(before cb after)                                      \
        CASE_TREEBOOK(before tb after)                                        \
        CASE_TOOLBOOK(before toolb after)                                     \
        CASE_AUINOTEBOOK(before aui after)                                    \
        CASE_SIMPLEBOOK(before sb after)                                      \
                                                                              \
        default:                                                              \
            wxFAIL_MSG( "unknown book control type" );                   \
    }

MyCTPMiniView::MyCTPMiniView(wxWindow *parent, const char* xml, size_t xmlflag) :Base(parent, xml, xmlflag)
{
#if wxUSE_NOTEBOOK
	m_type = Type_Notebook;
#elif wxUSE_CHOICEBOOK
	m_type = Type_Choicebook;
#elif wxUSE_LISTBOOK
	m_type = Type_Listbook;
#elif wxUSE_TREEBOOK
	m_type = Type_Treebook;
#elif wxUSE_TOOLBOOK
	m_type = Type_Toolbook;
#elif wxUSE_AUI
	m_type = Type_Aui;
#else
	m_type = Type_Simplebook;
#endif
	m_type = Type_Listbook;

	m_orient = ID_ORIENT_DEFAULT;
	m_chkShowImages = true;
	m_fixedWidth = false;
	m_multi = false;
	m_noPageTheme = false;
	m_buttonBar = false;
	m_horzLayout = false;

	m_bookCtrl = nullptr;
	int flags;
	switch (m_orient)
	{
	case ID_ORIENT_TOP:
		flags = wxBK_TOP;
		break;

	case ID_ORIENT_BOTTOM:
		flags = wxBK_BOTTOM;
		break;

	case ID_ORIENT_LEFT:
		flags = wxBK_LEFT;
		break;

	case ID_ORIENT_RIGHT:
		flags = wxBK_RIGHT;
		break;

	default:
		flags = wxBK_DEFAULT;
	}

#if wxUSE_NOTEBOOK
	if (m_fixedWidth && m_type == Type_Notebook)
		flags |= wxNB_FIXEDWIDTH;
	if (m_multi && m_type == Type_Notebook)
		flags |= wxNB_MULTILINE;
	if (m_noPageTheme && m_type == Type_Notebook)
		flags |= wxNB_NOPAGETHEME;
#endif
#if wxUSE_TOOLBOOK
	if (m_buttonBar && m_type == Type_Toolbook)
		flags |= wxTBK_BUTTONBAR;
	if (m_horzLayout && m_type == Type_Toolbook)
		flags |= wxTBK_HORZ_LAYOUT;
#endif

	if (!m_bookCtrl) {
		DISPATCH_ON_TYPE(m_bookCtrl = new,
			wxNotebook,
			wxListbook,
			wxChoicebook,
			wxTreebook,
			wxToolbook,
			wxAuiNotebook,
			wxSimplebook,
			(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags));
	}

	// wxToolbook doesn't work without icons so always use them for it.
	/*if (m_chkShowImages || m_type == Type_Toolbook)
	{
		m_bookCtrl->SetImageList(m_imageList);
	}*/

	account_stat_view_ = new MyCTPAccountStatView(m_bookCtrl);
	info_view_ = new MyCTPUserInfoView(m_bookCtrl);
	order_view_ = new MyCTPUserOrderView(m_bookCtrl);
	trade_view_ = new MyCTPUserTradeView(m_bookCtrl);
	position_view_ = new MyCTPUserPositionView(m_bookCtrl);
	commission_rate_view_ = new MyCTPUserCommissionRateView(m_bookCtrl);
	m_bookCtrl->AddPage(account_stat_view_, wxT("资金图"), true);
	m_bookCtrl->AddPage(info_view_, wxT("账户"), false);
	m_bookCtrl->AddPage(order_view_, wxT("委托"), false);
	m_bookCtrl->AddPage(trade_view_, wxT("成交"), false);
	m_bookCtrl->AddPage(position_view_, wxT("持仓"), false);
	m_bookCtrl->AddPage(commission_rate_view_, wxT("佣金费率"), false);

	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	topSizer->Add(m_bookCtrl, 1, wxEXPAND);

	SetSizer(topSizer);
}

void MyCTPMiniView::OnSkinInfoChanged()
{
	Base::OnSkinInfoChanged();
	if (account_stat_view_) {
		account_stat_view_->SetSkinInfo(GetSkinInfo());
	}
	if (info_view_) {
		info_view_->SetSkinInfo(GetSkinInfo());
	}
	if (order_view_) {
		order_view_->SetSkinInfo(GetSkinInfo());
	}
	if (trade_view_) {
		trade_view_->SetSkinInfo(GetSkinInfo());
	}
	if (position_view_) {
		position_view_->SetSkinInfo(GetSkinInfo());
	}
	if (commission_rate_view_) {
		commission_rate_view_->SetSkinInfo(GetSkinInfo());
	}
}

void MyCTPMiniView::OnHandleChanged()
{
	Base::OnHandleChanged();
	if (account_stat_view_) {
		account_stat_view_->SetHandle(h_);
	}
	if (info_view_) {
		info_view_->SetHandle(h_);
	}
	if (order_view_) {
		order_view_->SetHandle(h_);
	}
	if (trade_view_) {
		trade_view_->SetHandle(h_);
	}
	if (position_view_) {
		position_view_->SetHandle(h_);
	}
	if (commission_rate_view_) {
		commission_rate_view_->SetHandle(h_);
	}
}

void MyCTPMiniView::OnUserChanged()
{
	Base::OnUserChanged();
	if (account_stat_view_) {
		account_stat_view_->SetUser(user_);
	}
	if (info_view_) {
		info_view_->SetUser(user_);
	}
	if (order_view_) {
		order_view_->SetUser(user_);
	}
	if (trade_view_) {
		trade_view_->SetUser(user_);
	}
	if (position_view_) {
		position_view_->SetUser(user_);
	}
	if (commission_rate_view_) {
		commission_rate_view_->SetUser(user_);
	}
}

void MyCTPMiniView::OnNotifyEnable(HZQDB h)
{
	switch (h->type)
	{
	case ZQDB_HANDLE_TYPE_TABLE_DATA: {
		if (h->type == ZQDB_HANDLE_TYPE_MODULE && h == *g_module) {
			auto book_page = m_bookCtrl->GetCurrentPage();
			if (book_page == order_view_) {
				order_view_->ShowAll();
			}
			else if (book_page == trade_view_) {
				trade_view_->ShowAll();
			}
			else if (book_page == position_view_) {
				position_view_->ShowAll();
			}
			else if (book_page == commission_rate_view_) {
				commission_rate_view_->ShowAll();
			}
		}
	} break;
	default:
		break;
	}
}

void MyCTPMiniView::OnNotifyDisable(HZQDB h)
{
	switch (h->type)
	{
	case ZQDB_HANDLE_TYPE_TABLE_DATA: {
		if (h->type == ZQDB_HANDLE_TYPE_MODULE && h == *g_module) {
			auto book_page = m_bookCtrl->GetCurrentPage();
			if (book_page == order_view_) {
				order_view_->ClearAll();
			}
			else if (book_page == trade_view_) {
				trade_view_->ClearAll();
			}
			else if (book_page == position_view_) {
				position_view_->ClearAll();
			}
			else if (book_page == commission_rate_view_) {
				commission_rate_view_->ClearAll();
			}
		}
	} break;
	default:
		break;
	}
}

void MyCTPMiniView::OnNotifyAppend(HZQDB h)
{
	switch (h->type)
	{
	case ZQDB_HANDLE_TYPE_TABLE_DATA: {
		if (ZQDBGetModule(h) != *g_module) {
			break;
		}
		auto book_page = m_bookCtrl->GetCurrentPage();
		if (book_page == order_view_ && ZQDBIsDataOfTable(h, STR_ZQDB_TABLE_ORDER)) {
			order_view_->ShowAll();
		}
		else if (book_page == trade_view_ && ZQDBIsDataOfTable(h, STR_ZQDB_TABLE_TRADE)) {
			trade_view_->ShowAll();
		}
		else if (book_page == position_view_ && ZQDBIsDataOfTable(h, STR_ZQDB_TABLE_POSITION)) {
			position_view_->ShowAll();
		}
	} break;
	default:
		break;
	}
}

void MyCTPMiniView::OnNotifyUpdate(HZQDB h)
{
	switch (h->type)
	{
	case ZQDB_HANDLE_TYPE_TABLE_DATA: {
		if (ZQDBGetModule(h) != *g_module) {
			break;
		}
		auto book_page = m_bookCtrl->GetCurrentPage();
		if (book_page == order_view_ && ZQDBIsDataOfTable(h, STR_ZQDB_TABLE_ORDER)) {
			order_view_->RefreshAll();
		}
		else if (book_page == trade_view_ && ZQDBIsDataOfTable(h, STR_ZQDB_TABLE_TRADE)) {
			trade_view_->RefreshAll();
		}
		else if (book_page == position_view_ && ZQDBIsDataOfTable(h, STR_ZQDB_TABLE_POSITION)) {
			position_view_->RefreshAll();
		}
	} break;
	default:
		break;
	}
}

void MyCTPMiniView::ShowView(wxWindow* page)
{
	//切换显示视图
	auto old_page = m_bookCtrl->GetCurrentPage();
	if (old_page != page) {
		for (size_t i = 0, j = m_bookCtrl->GetPageCount(); i < j; i++)
		{
			if (m_bookCtrl->GetPage(i) == page)
			{
				m_bookCtrl->SetSelection(i);
				break;
			}
		}
	}
}

void MyCTPMiniView::OnTimer(wxTimerEvent& event)
{
	auto book_page = m_bookCtrl->GetCurrentPage();
	if (book_page == account_stat_view_) {
		account_stat_view_->RefreshAll();
	}
}

void MyCTPMiniView::OnShowEvent(wxShowEvent& evt)
{
	
}

// -- MyCTPModule --

MyCTPModule::MyCTPModule(HZQDB h) :Base(h)
{
	g_module = this; 
	RefreshBaseInfo();
}

MyCTPModule::~MyCTPModule()
{
	g_module = nullptr;
}

void MyCTPModule::RefreshBaseInfo()
{
	for (auto sz_exchange : sz_exchanges)
	{
		zqdb::Exchange exchange(sz_exchange);
		if (exchange) {
			auto tmax_time_point = ZQDBGetMaxTimePoint(exchange->TradeTime, exchange->TradeTimeCount);
			if (tmax_time_point > max_time_point_) {
				max_time_point_ = tmax_time_point;
			}
		}
	}
}

void MyCTPModule::ClearBaseInfo()
{
	max_time_point_ = 0;
}

uint32_t MyCTPModule::GetTradeTime(uint32_t* date, uint32_t* tradeday)
{
	for (auto sz_exchange : sz_exchanges)
	{
		zqdb::Exchange exchange(sz_exchange);
		if (exchange) {
			auto time = ZQDBGetExchangeTime(exchange, date, tradeday);
			if (time) {
				return time;
			}
		}
	}
	return 0;
}

wxString MyCTPModule::GetUserInfo(HZQDB huser)
{
	if (!huser) {
		return wxEmptyString;
	}
	//静态权益=上日结算-出金金额+入金金额 -> account.PreBalance(昨日总资产或者叫总权益) - account.Withdraw + account.Deposit
	//动态权益=期货结算准备金(Balance=静态权益+ 平仓盈亏+ 持仓盈亏- 手续费 -> 静态权益+ account.CloseProfit + account.PositionProfit - account.Commission)
	//可用资金(动态权益-占用保证金- 冻结保证金- 冻结手续费- 交割保证金) -> account.Available
	//风险度=当前保证金总额/期货结算准备金
	zqdb::ObjectT<tagCTPUserInfo> user(huser);
	double Static = 0;//PreBalance - Withdraw + Deposit;
	double Dynamic = 0;//CloseProfit + PositionProfit - Commission;
	double Available = 0;//;
	double Risk = 0;//CurrMargin / Balance;
	zqdb::AllUserAccount allaccount(huser, *g_module);
	for (auto haccount : allaccount)
	{
		zqdb::ObjectT<tagCTPAccountInfo> account(haccount);
		if (strcmp(account->User, user->User) == 0 && strcmp(account->Broker, user->Broker) == 0) {
			auto tBalance = account->Balance;
			auto tAvailable = account->Available;
			auto tCurrMargin = account->CurrMargin;
			Static += account->PreBalance - account->Withdraw + account->Deposit;
			Dynamic += tBalance; //Static + account->CloseProfit + account->PositionProfit - account->Commission;
			Available += tAvailable;
			Risk = tCurrMargin / tBalance;
			break;
		}
	}
	return wxString::Format(wxT("总资产:%.2f 可用:%.2f 当日盈亏:%.2f 风险度:%.2f%%"), Dynamic, Available, Dynamic - Static, Risk * 100);
}

const std::vector<AccountStatInfo>& MyCTPModule::GetAccountStatInfo(HZQDB account)
{
	auto it = account_stat_infos_.find(account);
	if(it != account_stat_infos_.end()) {
		return it->second;
	}
	static std::vector<AccountStatInfo> empty_data;
	return empty_data;
}

MyLoginView* MyCTPModule::NewLoginView(wxWindow* parent, const char* xml, size_t xmlflag)
{
	return new MyCTPLoginView(parent, xml, xmlflag);
}

MyMiniView* MyCTPModule::NewMiniView(wxWindow* parent, const char* xml, size_t xmlflag) 
{
	/*if (view == wxT("账户")) {
		return new MyCTPUserInfoView(parent, xml, xmlflag);
	}
	else if (view == wxT("持仓")) {
		return new MyCTPUserPositionView(parent, xml, xmlflag);
	}*/
	return new MyCTPMiniView(parent, xml, xmlflag);
}

int MyCTPModule::ReqModifyPassword(HZQDB huser, const char* old_pwd, const char* new_pwd, HNMSG* rsp, size_t timeout, size_t flags)
{
	zqdb::ObjectT<tagUserInfo> user(huser);
	zqdb::Msg msg(NET_MSG_REQUEST_DO);
	msg.SetReqID(com::zqdb::proto::msg::MSG_REQUEST_DO_TD_USER_MODIFY_PASSWORD);
	com::zqdb::proto::msg::ReqModifyPassword req;
	req.set_broker(user->Broker);
	//req.set_investor(user->)
	req.set_user(user->User);
	req.set_oldpassword(old_pwd);
	req.set_newpassword(new_pwd);
	auto str = req.SerializeAsString();
	msg.SetData(str.data(), str.size());
	return ZQDBRequest(user, msg, rsp, timeout);
}

int MyCTPModule::ReqModifyAccountPassword(HZQDB haccount, const char* old_pwd, const char* new_pwd, HNMSG* rsp, size_t timeout, size_t flags)
{
	zqdb::ObjectT<tagCTPAccountInfo> account(haccount);
	zqdb::Msg msg(NET_MSG_REQUEST_DO);
	msg.SetReqID(com::zqdb::proto::msg::MSG_REQUEST_DO_TD_USER_MODIFY_ACCOUNT_PASSWORD);
	com::zqdb::proto::msg::ReqModifyAccountPassword req;
	req.set_broker(account->Broker);
	//req.set_investor(user->)
	req.set_user(account->User);
	req.set_account(account->Account);
	req.set_currency(account->CurrencyID);
	req.set_oldpassword(old_pwd);
	req.set_newpassword(new_pwd);
	auto str = req.SerializeAsString();
	msg.SetData(str.data(), str.size());
	return ZQDBRequest(account, msg, rsp, timeout);
}

int MyCTPModule::OrderSend(HZQDB huser, HZQDB hcode, char direction, char offset, char type, double volume, double price, HNMSG* rsp, size_t timeout, size_t flags)
{
	zqdb::Code code(hcode);
	zqdb::ObjectT<tagUserInfo> user(huser);
	zqdb::Msg msg(NET_MSG_REQUEST_DO);
	msg.SetReqID(com::zqdb::proto::msg::MSG_REQUEST_DO_TD_ORDER_INSERT);
	//msg.SetParam("str", "1234567");
	//msg.SetParam("i64", "1234567");
	//msg.SetParam("f", "1234567.89");
	com::zqdb::proto::msg::ReqOrderInsert req;
	req.set_broker(user->Broker);
	//req.set_investor(user->)
	req.set_user(user->User);
	req.set_exchange(code->Exchange);
	req.set_code(code->TradeCode);
	req.set_price(price);
	req.set_volume(volume);
	req.set_direction(direction); //DIRECTION_LONG
	req.set_offset(offset); //OFFSET_OPEN
	req.set_type(type); //ORDER_LIMIT
	auto str = req.SerializeAsString();
	msg.SetData(str.data(), str.size());
	return ZQDBRequest(user, msg, rsp, timeout);
	/*if (rsp) {
		zqdb::Msg rsp_msg(rsp,zqdb::Msg::AutoDelete);
		auto errorcode = msg.GetParamAsInt(STR_ZQDB_MSG_ERROR_CODE, 0);
		auto errormsg = msg.GetParam(STR_ZQDB_MSG_ERROR_MESSAGE,"");
		if (!errorcode) {
			strcpy(orderid, rsp_msg.GetParam(STR_MDB_FIELD_INDEX(ZQDB, ORDER, ORDER), ""));
		}
		return 0;
	}*/
#ifdef _DEBUG
	com::zqdb::proto::msg::TestMessage test_msg;
	test_msg.set_msg(NET_MSG_REQUEST_DO);
	test_msg.set_type(com::zqdb::proto::msg::MSG_REQUEST_DO_TD_ORDER_INSERT);
	test_msg.set_id(ZQDBReqID());
	(*test_msg.mutable_param())["str"] = "1234567";
	(*test_msg.mutable_param())["u32"] = "1234567";
	(*test_msg.mutable_param())["i32"] = "1234567";
	(*test_msg.mutable_param())["u64"] = "1234567";
	(*test_msg.mutable_param())["i64"] = "1234567";
	(*test_msg.mutable_param())["f32"] = "1234567.89";
	(*test_msg.mutable_param())["f64"] = "1234567.89";
	auto test_msg_str = test_msg.SerializeAsString();
	double test_f64 = std::stod((*test_msg.mutable_param())["f64"]);

	com::zqdb::proto::msg::VarMessage var_msg;
	var_msg.set_msg(NET_MSG_REQUEST_DO);
	var_msg.set_type(com::zqdb::proto::msg::MSG_REQUEST_DO_TD_ORDER_INSERT);
	var_msg.set_id(ZQDBReqID());
	com::zqdb::proto::msg::Variant var_val;
	var_val.set_str("1234567");
	(*var_msg.mutable_param())["str"] = var_val;
	var_val.set_u32(1234567);
	(*var_msg.mutable_param())["u32"] = var_val;
	var_val.set_i32(1234567);
	(*var_msg.mutable_param())["i32"] = var_val;
	var_val.set_u64(1234567);
	(*var_msg.mutable_param())["u64"] = var_val;
	var_val.set_i64(1234567);
	(*var_msg.mutable_param())["i64"] = var_val;
	var_val.set_f32(1234567.89);
	(*var_msg.mutable_param())["f32"] = var_val;
	var_val.set_f64(1234567.89);
	(*var_msg.mutable_param())["f64"] = var_val;
	auto var_msg_str = var_msg.SerializeAsString();
	double var_f64 = (*var_msg.mutable_param())["f64"].f64();
#endif//
	return 0;
}

int MyCTPModule::OrderCancel(HZQDB huser, HZQDB horder, HNMSG* rsp, size_t timeout, size_t flags)
{
	zqdb::ObjectT<tagOrderInfo> order(horder);
	zqdb::Msg msg(NET_MSG_REQUEST_DO);
	msg.SetReqID(com::zqdb::proto::msg::MSG_REQUEST_DO_TD_ORDER_CANCEL);
	com::zqdb::proto::msg::ReqOrderCancel req;
	req.set_broker(order->Broker);
	req.set_user(order->User);
	req.set_exchange(order->Exchange);
	req.set_code(order->Code);
	req.set_orderid(order->Order);
	auto str = req.SerializeAsString();
	msg.SetData(str.data(), str.size());
	return ZQDBRequest(huser, msg, rsp, timeout);
}

int MyCTPModule::OrderClose(HZQDB huser, HZQDB hposition, char type, double volume, double price, HNMSG* rsp, size_t timeout, size_t flags)
{
	zqdb::ObjectT<tagPositionInfo> position(hposition);
	zqdb::ObjectT<tagUserInfo> user(huser);
	zqdb::Msg msg(NET_MSG_REQUEST_DO);
	msg.SetReqID(com::zqdb::proto::msg::MSG_REQUEST_DO_TD_ORDER_INSERT);
	com::zqdb::proto::msg::ReqOrderInsert req;
	req.set_broker(user->Broker);
	//req.set_investor(user->)
	req.set_user(user->User);
	req.set_exchange(position->Exchange);
	req.set_code(position->Code);
	req.set_price(price);
	req.set_volume(volume);
	switch (position->Direction)
	{
	case DIRECTION_LONG: {
		req.set_direction(DIRECTION_SHORT);
	} break;
	case DIRECTION_SHORT: {
		req.set_direction(DIRECTION_LONG);
	} break;
	default: {
		ASSERT(0);
	} break;
	}
	req.set_offset(OFFSET_CLOSE); //OFFSET_OPEN
	req.set_type(type); //ORDER_LIMIT
	auto str = req.SerializeAsString();
	msg.SetData(str.data(), str.size());
	return ZQDBRequest(huser, msg, rsp, timeout);
}

int MyCTPModule::ReqQryMaxOrderVolume(HZQDB huser, HZQDB hcode, char direction, char offset, char type, HNMSG* rsp, size_t timeout, size_t flags) 
{ 
	zqdb::Code code(hcode);
	zqdb::ObjectT<tagUserInfo> user(huser);
	zqdb::Msg msg(NET_MSG_REQUEST_QUERY);
	msg.SetReqID(com::zqdb::proto::msg::MSG_REQUEST_QUERY_TD_ORDER_MAX_ORDER_VOLUME);
	com::zqdb::proto::msg::ReqOrderInsert req;
	req.set_broker(user->Broker);
	//req.set_investor(user->)
	req.set_user(user->User);
	req.set_exchange(code->Exchange);
	req.set_code(code->TradeCode);
	req.set_direction(direction); //DIRECTION_LONG
	req.set_offset(offset); //OFFSET_OPEN
	req.set_type(type); //ORDER_LIMIT
	auto str = req.SerializeAsString();
	msg.SetData(str.data(), str.size());
	return ZQDBRequest(user, msg, rsp, timeout);
}

void MyCTPModule::OnTimer()
{
	bool refresh = IsBaseInfoOk();//account_stat_infos_.empty();
	if (refresh) {
		uint32_t date = 0, time = GetTradeTime(&date, nullptr);
		if(!date || !time) {
			return;
		}
		auto now_t = XUtil::make_time_t(XUtil::GetYear(date), XUtil::GetMonth(date), XUtil::GetDay(date),
			XUtil::GetHour(time), XUtil::GetMinute(time), XUtil::GetSecond(time));//::time(nullptr);
		
		zqdb::AllModuleAccount allaccount(*g_module);
		for (auto haccount : allaccount)
		{
			auto& account_stat_infos = account_stat_infos_[haccount];
			bool add = false;
			if (account_stat_infos.empty()) {
				account_stat_infos.reserve(max_time_point_);
				add = true;
			} else {
				/*auto now_tp = std::chrono::system_clock::from_time_t(now_t);
				auto old_tp = std::chrono::system_clock::from_time_t(account_stat_infos.back().Time);
				auto duration = now_tp - old_tp;
				if (duration > std::chrono::seconds(60)) {*/
				struct tm now_tm = *localtime(&now_t);
				struct tm old_tm = *localtime(&account_stat_infos.back().Time);
				if(XUtil::MakeTime(now_tm.tm_hour, now_tm.tm_min, 0) > XUtil::MakeTime(old_tm.tm_hour, old_tm.tm_min, 0)) {
					add = true;
				}
			}
			if (add) {
				account_stat_infos.emplace_back(AccountStatInfo{});
			}
			AccountStatInfo& stat_info = account_stat_infos.back();
			zqdb::ObjectT<tagCTPAccountInfo> account(haccount);
			double Static = account->PreBalance - account->Withdraw + account->Deposit;
			stat_info.Time = now_t;
			stat_info.Balance = account->Balance;
			stat_info.Available = account->Available;
			stat_info.Profit = account->Balance - Static;
			if (account_stat_infos.size() > max_time_point_) {
				account_stat_infos.erase(account_stat_infos.begin()
					, account_stat_infos.begin() + (account_stat_infos.size() - max_time_point_));
			}
		}
	}
}

void MyCTPModule::OnNotifyEnable(HZQDB h)
{
	if (h == this->h_) {
		RefreshBaseInfo();
	}
}

void MyCTPModule::OnNotifyDisable(HZQDB h)
{
	if (h == this->h_) {
		ClearBaseInfo();
	}
}

void MyCTPModule::OnNotifyAppend(HZQDB h)
{

}

void MyCTPModule::OnNotifyUpdate(HZQDB h)
{

}

int MyCTPModule::OnNetMsg(zqdb::Msg& msg)
{
	int ret = 0;
	auto type = msg.GetMsgType();
	auto reqtype = msg.GetReqType();
	switch (type)
	{
	case NET_MSG_NOTIFY: {
		switch (reqtype)
		{
		case com::zqdb::proto::msg::MSG_NOTIFY_TD_USER_LOGIN: {
			//收到登录通知，登录成功还是失败参看errorcode，提示参看errormsg
			auto user = msg.GetParam(STR_MDB_FIELD_INDEX(ZQDB, USER, USER));
			auto errorcode = msg.GetParamAsInt(STR_ZQDB_MSG_ERROR_CODE);
			auto errormsg = msg.GetParam(STR_ZQDB_MSG_ERROR_MESSAGE);
			if (errorcode) {
				//wxLogError(wxT("%s 登录失败:[%zd]%s"), user, errorcode, errormsg);
			}
			else {
				//wxLogInfo(wxT("%s 登录成功"), user);
			}
			ret = 1;
		} break;
		}
	} break;
	case NET_MSG_REQUEST_DO: {
		//
	} break;
	case NET_MSG_REQUEST_QUERY: {
		//
	} break;
	case NET_MSG_RESPONSE: {
		switch (reqtype)
		{
		case com::zqdb::proto::msg::MSG_REQUEST_DO_TD_USER_LOGIN: {
			//发送登录请求成功
			ret = 1;
		} break;
		}
	} break;
	default:
		break;
	}
	return ret;
	return 0;
}
