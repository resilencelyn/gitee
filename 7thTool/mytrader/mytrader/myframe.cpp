#include "myapp.h"
#include "myframe.h"
#include "mylogdlg.h"
#include "mysettingsdlg.h"
#include <techdlg.h>
#include <techedit.h>

///

wxBEGIN_EVENT_TABLE(MyFrame, Base)
EVT_RIBBONBUTTONBAR_CLICKED(ID_DATA_LISTEN, MyFrame::OnDataListen)
EVT_RIBBONBUTTONBAR_CLICKED(ID_DATA_RECORD, MyFrame::OnDataRecord)
EVT_RIBBONBUTTONBAR_CLICKED(ID_DATA_TEST, MyFrame::OnDataTest)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_DATA_TEST, MyFrame::OnDataTestDropdown)
EVT_MENU_RANGE(ID_DATA_TEST_RECORD, ID_DATA_TEST_RECORD_MAX, MyFrame::OnDataTestRecord)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CALC_EDIT, MyFrame::OnCalcEdit)
EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(ID_CALC_EDIT, MyFrame::OnCalcEditDropdown)
EVT_MENU_RANGE(ID_CALC_EDIT_NEW, ID_CALC_EDIT_NEW_MAX, MyFrame::OnCalcEditNew)
EVT_MENU_RANGE(ID_CALC_EDIT_MAJOR, ID_CALC_EDIT_MAJOR_MAX, MyFrame::OnCalcEditOpen)
EVT_MENU_RANGE(ID_CALC_EDIT_MINOR, ID_CALC_EDIT_MINOR_MAX, MyFrame::OnCalcEditOpen)
EVT_MENU_RANGE(ID_CALC_EDIT_FILTER, ID_CALC_EDIT_FILTER_MAX, MyFrame::OnCalcEditOpen)
EVT_MENU_RANGE(ID_CALC_EDIT_SORT, ID_CALC_EDIT_SORT_MAX, MyFrame::OnCalcEditOpen)
EVT_MENU_RANGE(ID_CALC_EDIT_SCRIPT, ID_CALC_EDIT_SCRIPT_MAX, MyFrame::OnCalcEditOpen)
EVT_MENU_RANGE(ID_CALC_EDIT_STRATEGY, ID_CALC_EDIT_STRATEGY_MAX, MyFrame::OnCalcEditOpen)
EVT_RIBBONBUTTONBAR_CLICKED(ID_CALC_TRADEFLAG, MyFrame::OnCalcTradeFlag)
EVT_CLOSE(MyFrame::OnCloseEvent)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(const char* xml, size_t xmlflag)
    : Base(xml, xmlflag)
	, all_strategy_func_(CALC_STRATEGY)
{
	CFG_FROM_XML(cfg, xml, xmlflag);

	Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyFrame::OnStrategy, this, ID_STRATEGY_BEGIN, ID_STRATEGY_END);
}

MyFrame::~MyFrame()
{
	zqdb::Calc::Filter filter;
	auto key = code_view_->GetKey(filter);
	if (filter) {
		//key = filter.
	}
	wxGetApp().SetFrameLastFilter(key);

	MY_CODE_SORT_TYPE sort_type = SORT_ZDF;
	int sort = IsSort(&sort_type);
	wxGetApp().SetFrameLastSortType(sort_type);
	wxGetApp().SetFrameLastSort(sort);
}

void MyFrame::OnCloseEvent(wxCloseEvent& evt)
{
	evt.Veto();
	int action = wxCANCEL;
	if (ZQDBIsTest()) {
		action = wxOK;
	}
	else {
		if (wxGetApp().GetFrameOnCloseRember()) {
			action = wxGetApp().GetFrameOnCloseAction();
#ifdef _DEBUG
			wxGetApp().SetFrameOnCloseRember(false);
#endif//
		}
		else {
			wxString tips;
			long style = 0;
			if (ZQDBIsTest()) {
				tips = wxT("确定要退出超级回测吗？\n")
					wxT("点击“取消”按钮，超级回测将会隐藏在后台继续运行。");
				style = wxOK | wxCANCEL;
			}
			else {
				tips = wxT("退出程序后将无法收集行情数据，将导致历史行情数据缺失。\n")
					wxT("默认“取消”按钮，程序将会隐藏在后台继续收集行情数据。\n")
					wxT("确定要退出程序吗？");
				style = wxOK | wxCANCEL | wxCANCEL_DEFAULT;
			}
#if 0
			MyCheckDlg dlg(this, tips
				, wxT("下次不再提示"), false
				, wxT("提示"), style);
			action = dlg.ShowModal();
			if (dlg.IsCheck()) {
				wxGetApp().SetFrameOnCloseAction(action);
				wxGetApp().SetFrameOnCloseRember(true);
			}
#else
			wxRichMessageDialog dlg(this, tips, wxT("提示"), wxOK | wxCANCEL | wxICON_INFORMATION);
			//dlg.SetYesNoLabels(m_labels[Btn_Yes]->GetValue(),
			//	m_labels[Btn_No]->GetValue());
			dlg.ShowCheckBox(wxT("下次不再提示"), false);
			//dlg.ShowDetailedText(wxT("Detaile"));
			//dlg.SetFooterText(wxT("Footer"));
			//dlg.SetFooterIcon(wxICON_INFORMATION);
			switch (dlg.ShowModal())
			{
			case wxID_YES:
			case wxID_OK:
				action = wxOK;
				break;
			case wxID_CANCEL:
			case wxID_NO:
			default:
				action = wxCANCEL;
				break;
			}
			if (dlg.IsCheckBoxChecked()) {
				wxGetApp().SetFrameOnCloseAction(action);
				wxGetApp().SetFrameOnCloseRember(true);
			}
#endif//
		}
	}
	if (wxOK == action) {
		//wxGetApp().Post(100, []() {
		wxGetApp().DoExit();
		//});
	}
	else {
		wxGetApp().DoHide();
	}
}

void MyFrame::AddFirstPages()
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
	navigate_bar->AddHybridButton(ID_MARKET_ALL, wxT("全部"), skin_info_ptr_->GetBitmap32(wxT("全部")));
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
	filter_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyFrame::OnFilterSetting, this);
	filter_bar_ = new wxRibbonButtonBar(filter_panel);
	InnerUpdateFilter(allexchange);

	//点击一次降序、二次升序、三次不排序
	auto sort_panel = new wxRibbonPanel(home, wxID_ANY, wxT("排序"), wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	sort_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyFrame::OnSortSetting, this);
	sort_bar_ = new wxRibbonButtonBar(sort_panel);
	InnerUpdateSort();

	wxRibbonPanel *analysis_panel = new wxRibbonPanel(home, wxID_ANY, "技术分析",
		wxNullBitmap, wxDefaultPosition, wxDefaultSize,
		wxRIBBON_PANEL_NO_AUTO_MINIMISE |
		wxRIBBON_PANEL_EXT_BUTTON);
	analysis_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyFrame::OnTechSetting, this);
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

	auto data_panel = new wxRibbonPanel(home, wxID_ANY, wxT("数据"), wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	//data_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyFrame::OnDataSetting, this);
	data_bar_ = new wxRibbonButtonBar(data_panel);
	if (ZQDBIsTest()) {
		data_bar_->AddToggleButton(ID_DATA_TEST, wxT("开始回测"), skin_info_ptr_->GetBitmap32(wxT("回放")));
		//, wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, wxART_OTHER, wxSize(32, 32)));
		//data_bar_->SetButtonTextMinWidth(ID_DATA_TEST, wxT("回放录制的全市场数据"));
		data_bar_->SetButtonMaxSizeClass(ID_DATA_TEST, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		data_bar_->SetButtonMinSizeClass(ID_DATA_TEST, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	}
	else {
		if (ZQDBIsServer()) {
			data_bar_->AddToggleButton(ID_DATA_LISTEN, wxT("云服务"), skin_info_ptr_->GetBitmap32(wxT("服务")));
			data_bar_->ToggleButton(ID_DATA_LISTEN, ZQDBIsListen());
		}
		if (ZQDBIsRecording()) {
			data_bar_->AddToggleButton(ID_DATA_RECORD, wxT("录制中"), skin_info_ptr_->GetBitmap32(wxT("录制")));
			data_bar_->ToggleButton(ID_DATA_RECORD, true);
		}
		else {
			data_bar_->AddToggleButton(ID_DATA_RECORD, wxT("录制"), skin_info_ptr_->GetBitmap32(wxT("录制")));
		}
		//, wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, wxART_OTHER, wxSize(32, 32)));
		//data_bar_->SetButtonTextMinWidth(ID_DATA_RECORD, wxT("开始/停止录制全市场数据"));
		data_bar_->SetButtonMaxSizeClass(ID_DATA_RECORD, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		data_bar_->SetButtonMinSizeClass(ID_DATA_RECORD, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		data_bar_->AddButton(ID_DATA_TEST, wxT("超级回测"), skin_info_ptr_->GetBitmap32(wxT("回放")));
		//, wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, wxART_OTHER, wxSize(32, 32)));
		//data_bar_->SetButtonTextMinWidth(ID_DATA_TEST, wxT("回放录制的全市场数据"));
		data_bar_->SetButtonMaxSizeClass(ID_DATA_TEST, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		data_bar_->SetButtonMinSizeClass(ID_DATA_TEST, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	}
	/*data_bar_->AddHybridButton(ID_DATA_EDIT, wxT("管理"), skin_info_ptr_->GetBitmap32(wxT("公式管理")));
	//, wxArtProvider::GetBitmap(wxART_HELP_BOOK, wxART_OTHER, wxSize(32, 32)));
	//data_bar_->SetButtonTextMinWidth(ID_DATA_EDIT, wxT("管理指标、过滤器、脚本、策略"));
	data_bar_->SetButtonMaxSizeClass(ID_DATA_EDIT, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	data_bar_->SetButtonMinSizeClass(ID_DATA_EDIT, wxRIBBON_BUTTONBAR_BUTTON_LARGE);*/

	auto calc_panel = new wxRibbonPanel(home, wxID_ANY, wxT("计算系统"), wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	calc_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyFrame::OnCalcSetting, this);
	calc_bar_ = new wxRibbonButtonBar(calc_panel);
	/*calc_bar_->AddHybridButton(ID_TRADE_QUICK_BUY_FAK, "快买", skin_info_ptr_->GetBitmap32(wxEmptyString));
	calc_bar_->SetButtonMaxSizeClass(ID_TRADE_QUICK_BUY_FAK, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	calc_bar_->SetButtonMinSizeClass(ID_TRADE_QUICK_BUY_FAK, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	calc_bar_->AddHybridButton(ID_TRADE_QUICK_SELL_FAK, "快卖", skin_info_ptr_->GetBitmap32(wxEmptyString));
	calc_bar_->SetButtonMaxSizeClass(ID_TRADE_QUICK_SELL_FAK, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	calc_bar_->SetButtonMinSizeClass(ID_TRADE_QUICK_SELL_FAK, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	calc_bar_->AddButton(ID_TRADE_QUICK_CLOSE, "快平", skin_info_ptr_->GetBitmap32(wxEmptyString));
	calc_bar_->SetButtonMaxSizeClass(ID_TRADE_QUICK_CLOSE, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	calc_bar_->SetButtonMinSizeClass(ID_TRADE_QUICK_CLOSE, wxRIBBON_BUTTONBAR_BUTTON_LARGE);*/
	calc_bar_->AddHybridButton(ID_CALC_EDIT, wxT("公式管理"), skin_info_ptr_->GetBitmap32(wxT("公式管理")));
		//, wxArtProvider::GetBitmap(wxART_HELP_BOOK, wxART_OTHER, wxSize(32, 32)));
	//calc_bar_->SetButtonTextMinWidth(ID_CALC_EDIT, wxT("管理指标、过滤器、脚本、策略"));
	calc_bar_->SetButtonMaxSizeClass(ID_CALC_EDIT, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	calc_bar_->SetButtonMinSizeClass(ID_CALC_EDIT, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	//calc_bar_->AddHybridButton(ID_CALC_MONITOR, wxT("监控中心"), skin_info_ptr_->GetBitmap32(wxT("监控中心")));
	////calc_bar_->SetButtonTextMinWidth(ID_CALC_MONITOR, wxT("监控全市场市场动态变化"));
	//calc_bar_->SetButtonMaxSizeClass(ID_CALC_MONITOR, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	//calc_bar_->SetButtonMinSizeClass(ID_CALC_MONITOR, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	calc_bar_->AddToggleButton(ID_CALC_TRADEFLAG, wxT("智能交易"), skin_info_ptr_->GetBitmap32(wxT("智能交易")));
		//, wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, wxART_OTHER, wxSize(32, 32)));
	//calc_bar_->SetButtonTextMinWidth(ID_CALC_TRADEFLAG, wxT("允许或禁止算法交易"));
	calc_bar_->SetButtonMaxSizeClass(ID_CALC_TRADEFLAG, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	calc_bar_->SetButtonMinSizeClass(ID_CALC_TRADEFLAG, wxRIBBON_BUTTONBAR_BUTTON_LARGE);

	//脚本
	auto script_panel = new wxRibbonPanel(home, wxID_ANY, wxT("快速脚本"), wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	script_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyFrame::OnScriptSetting, this);
	script_bar_ = new wxRibbonButtonBar(script_panel);
	InnerUpdateScript();

	//策略
	auto strategy_panel = new wxRibbonPanel(home, wxID_ANY, wxT("策略算法"), wxNullBitmap, wxDefaultPosition, wxDefaultSize
		, wxRIBBON_PANEL_NO_AUTO_MINIMISE | wxRIBBON_PANEL_EXT_BUTTON);
	strategy_panel->Bind(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, &MyFrame::OnStrategySetting, this);
	strategy_bar_ = new wxRibbonButtonBar(strategy_panel);
	InnerUpdateStrategy();

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

void MyFrame::InnerUpdateStrategy()
{
	/*strategy_bar->AddHybridButton(ID_STRATEGY_BEGIN, "趋势开仓A", skin_info_ptr_->GetBitmap32(wxEmptyString));
	strategy_bar->SetButtonMaxSizeClass(ID_STRATEGY_BEGIN, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	strategy_bar->SetButtonMinSizeClass(ID_STRATEGY_BEGIN, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	strategy_bar->AddHybridButton(ID_STRATEGY_BEGIN + 1, "策略平仓A", skin_info_ptr_->GetBitmap32(wxEmptyString));
	strategy_bar->SetButtonMaxSizeClass(ID_STRATEGY_BEGIN + 1, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	strategy_bar->SetButtonMinSizeClass(ID_STRATEGY_BEGIN + 1, wxRIBBON_BUTTONBAR_BUTTON_LARGE);*/
	for (size_t i = 0, j = all_strategy_func_.size(); i < j; i++)
	{
		zqdb::Calc::Func func(all_strategy_func_[i]);
		auto name = func.GetCalcName();
		if (strstr(name, "Algo")) {
			strategy_bar_->AddToggleButton(ID_STRATEGY_BEGIN + i, name, skin_info_ptr_->GetBitmap32(wxT("算法")));
		}
		else {
			strategy_bar_->AddToggleButton(ID_STRATEGY_BEGIN + i, name, skin_info_ptr_->GetBitmap32(wxT("策略")));
		}
		strategy_bar_->SetButtonMaxSizeClass(ID_STRATEGY_BEGIN + i, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
		strategy_bar_->SetButtonMinSizeClass(ID_STRATEGY_BEGIN + i, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
	}
}

void MyFrame::OnSkinInfoChanged()
{
	Base::OnSkinInfoChanged();
}

void MyFrame::OnInfoChanged()
{
	if (ZQDBIsTest()) {
		//提示运行策略，然后点击开始回测按钮
		ShowMessage(wxT("请点击上方↑策略列表中的策略按钮运行策略，然后再点击上方↑\"开始回测\"按钮开始回测。"));
	}

	auto sort_type = (MY_CODE_SORT_TYPE)wxGetApp().GetFrameLastSortType();
	auto sort = wxGetApp().GetFrameLastSort();
	if (sort) {
		switch (sort_type)
		{
		case SORT_ZDF:
			SortByZD(sort_type, 0, sort);
			break;
		case SORT_ZDS:
			SortByZD(sort_type, wxGetApp().GetSortQuick(), sort);
			break;
		case SORT_CALC_SORT:
			if (cur_sort_func_ < all_sort_func_.size()) {
				zqdb::Calc::Func func(all_sort_func_[cur_sort_func_]);
				if (wxGetApp().GetSortCalcFunc() == func.GetCalcName()) {
					zqdb::Calc::Sort calc(func.GetCalcName(), nullptr);
					SortByCalc(calc, sort);
				}
			}
			break;
		}
	}
	InnerUpdateSort();
	if (calc_bar_) {
		calc_bar_->ToggleButton(ID_CALC_TRADEFLAG, ZQDBGetCalcTradeFlag());
	}
	m_ribbon->Realize();

	auto h = Get();
	if (!h) {
		auto key = wxGetApp().GetFrameLastFilter();
		if (!key.IsEmpty()) {
			ShowKey(key);
		}
	}
	else {
		Goto(h);
	}
	GotoUser(GetUser());
}

void MyFrame::OnNotifyStartListen()
{
	if(data_bar_)
		data_bar_->ToggleButton(ID_DATA_LISTEN, ZQDBIsListen());
}

void MyFrame::OnNotifyStopListen()
{
	if (data_bar_)
		data_bar_->ToggleButton(ID_DATA_LISTEN, ZQDBIsListen());
}

void MyFrame::OnNotifyUpdate(HZQDB h)
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
		} break;
		case CALC_SORT: {
		} break;
		case CALC_SCRIPT: {
		} break;
		case CALC_STRATEGY: {
			all_strategy_func_.Update();
			InnerUpdateStrategy();
			m_ribbon->Realize();
		} break;
		}
	}

	Base::OnNotifyUpdate(h);
}

void MyFrame::OnDataListen(wxRibbonButtonBarEvent& evt)
{
	if (ZQDBIsListen()) {
		ZQDBStopListen();
	}
	else {
		ZQDBStartListen();
	}
}

void MyFrame::OnDataRecord(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	wxWindowDisabler disableAll;
	if (ZQDBIsRecording()) {
		wxBusyInfo info(wxT("停止录制中，请稍后..."), this);
		ZQDBStopRecord();
	}
	else {
		bool record = false;
		zqdb::AllExchange allexchange;
		if (allexchange.empty()) {
			if (wxNO == wxMessageBox(wxT("没有任何市场可用，建议等所有市场都可用时再录制数据。\n")
				wxT("取消录制吗？"), wxT("提示"), wxYES | wxNO)) {
				record = true;
			}
		}
		else if (ZQDBGetCalcAnyDisabledExchange()) {
			if (wxNO == wxMessageBox(wxT("有市场不可用，建议等所有市场都可用时再录制数据。\n")
				wxT("取消录制吗？"), wxT("提示"), wxYES | wxNO)) {
				record = true;
			}
		}
		else {
			record = true;
		}
		if (record) {
			wxBusyInfo info(wxT("准备录制中，请稍后..."), this);
			ZQDBStartRecord();
		}
	}
	if (data_bar_) {
		data_bar_->ToggleButton(ID_DATA_RECORD, ZQDBIsRecording() ? true : false);
		data_bar_->SetButtonText(ID_DATA_RECORD, ZQDBIsRecording() ? wxT("录制中") : wxT("录制"));
		m_ribbon->Realize();
	}
}

void  MyFrame::UpdateTestInfo()
{
	if (data_bar_) {
		data_bar_->ToggleButton(ID_DATA_TEST, ZQDBIsTesting());
		data_bar_->SetButtonText(ID_DATA_TEST, ZQDBIsTesting()?wxT("回测中"):wxT("继续回测"));
		m_ribbon->Realize();
	}
	SetTitle(wxGetApp().GetAppTitle() + wxString(ZQDBIsTesting() ? wxT("") : wxT("[暂停]")));
}

void MyFrame::OnDataTest(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	if (ZQDBIsTest()) {
		if (ZQDBIsTesting()) {
			ZQDBPauseTesting();
		}
		else {
			ZQDBContinueTesting();
		}
		UpdateTestInfo();
	}
	else {
		auto count = ZQDBGetRecordCount();
		if (!count) {
			wxMessageBox(wxT("请先录制数据!!!"), wxT("提示"), wxOK);
			return;
		}
		else if (count == 1 && ZQDBIsRecording()) {
			wxMessageBox(wxT("请先停止录制!!!"), wxT("提示"), wxOK);
			return;
		}
		MyTestDlg dlg(this);
		if (wxOK == dlg.ShowModal()) {
			auto begin = dlg.GetBegin();
			auto end = dlg.GetEnd();
			auto speed = 1000.0 / dlg.GetSpeed();
			wxGetApp().Test(begin, end, speed);
		}
	}
}

void MyFrame::OnDataTestDropdown(wxRibbonButtonBarEvent& evt)
{
	auto count = ZQDBGetRecordCount();
	if (!count) {
		return;
	}
	wxMenu menu;
	for (size_t i = 0, j = std::min<>((size_t)7, count); i < j; i++)
	{
		menu.Append(ID_DATA_TEST_RECORD + i, wxString::Format("%zu", ZQDBGetRecordDate(i)));
	}
	evt.PopupMenu(&menu);
}

void MyFrame::OnDataTestRecord(wxCommandEvent& evt)
{
	size_t pos = evt.GetId() - ID_DATA_TEST_RECORD;
	size_t date = ZQDBGetRecordDate(pos);
	if (date) {
		wxGetApp().Test(date, 0, 100);
	}
}

void MyFrame::OnCalcEdit(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	zqdb::TechDlg dlg(this);
	dlg.ShowModal();
	//wxGetApp().ShowCalcFrame();
}

void MyFrame::OnCalcEditDropdown(wxRibbonButtonBarEvent& evt)
{
	wxRibbonButtonBar* pBar = evt.GetBar();
	if (pBar) {
		zqdb::Calc::AllFunc all_major_func_(CALC_MAJOR);
		zqdb::Calc::AllFunc all_minor_func_(CALC_MINOR);
		wxMenu menu;
		wxMenu* sub_menu = new wxMenu;
		sub_menu->Append(ID_CALC_EDIT_NEW + CALC_MAJOR, wxT("新建"));
		sub_menu->AppendSeparator();
		for (size_t i = 0, j = 0; i < all_major_func_.size(); i++)
		{
			zqdb::Calc::Func func(all_major_func_[i]);
			if (func.GetCalcLang() != CALC_LANG_C_CPP) {
				auto sub_menu_item = sub_menu->Append(ID_CALC_EDIT_MAJOR + i, func.GetCalcName());
				zqdb::SetMenuItemClientData(sub_menu_item, func);
			}
		}
		menu.AppendSubMenu(sub_menu, wxT("主图指标"));
		sub_menu = new wxMenu;
		sub_menu->Append(ID_CALC_EDIT_NEW + CALC_MINOR, wxT("新建"));
		sub_menu->AppendSeparator();
		for (size_t i = 0, j = 0; i < all_minor_func_.size(); i++)
		{
			zqdb::Calc::Func func(all_minor_func_[i]);
			if (func.GetCalcLang() != CALC_LANG_C_CPP) {
				auto sub_menu_item = sub_menu->Append(ID_CALC_EDIT_MINOR + i, func.GetCalcName());
				zqdb::SetMenuItemClientData(sub_menu_item, func);
			}
		}
		menu.AppendSubMenu(sub_menu, wxT("副图指标"));
		sub_menu = new wxMenu;
		sub_menu->Append(ID_CALC_EDIT_NEW + CALC_FILTER, wxT("新建"));
		sub_menu->AppendSeparator();
		for (size_t i = 0, j = 0; i < all_filter_func_.size(); i++)
		{
			zqdb::Calc::Func func(all_filter_func_[i]);
			if (func.GetCalcLang() != CALC_LANG_C_CPP) {
				auto sub_menu_item = sub_menu->Append(ID_CALC_EDIT_FILTER + i, func.GetCalcName());
				zqdb::SetMenuItemClientData(sub_menu_item, func);
			}
		}
		menu.AppendSubMenu(sub_menu, wxT("筛选算法"));
		sub_menu = new wxMenu;
		sub_menu->Append(ID_CALC_EDIT_NEW + CALC_SORT, wxT("新建"));
		sub_menu->AppendSeparator();
		for (size_t i = 0, j = 0; i < all_sort_func_.size(); i++)
		{
			zqdb::Calc::Func func(all_sort_func_[i]);
			if (func.GetCalcLang() != CALC_LANG_C_CPP) {
				auto sub_menu_item = sub_menu->Append(ID_CALC_EDIT_SORT + i, func.GetCalcName());
				zqdb::SetMenuItemClientData(sub_menu_item, func);
			}
		}
		menu.AppendSubMenu(sub_menu, wxT("排序算法"));
		sub_menu = new wxMenu;
		sub_menu->Append(ID_CALC_EDIT_NEW + CALC_SCRIPT, wxT("新建"));
		sub_menu->AppendSeparator();
		for (size_t i = 0, j = 0; i < all_script_func_.size(); i++)
		{
			zqdb::Calc::Func func(all_script_func_[i]);
			if (func.GetCalcLang() != CALC_LANG_C_CPP) {
				auto sub_menu_item = sub_menu->Append(ID_CALC_EDIT_SCRIPT + i, func.GetCalcName());
				zqdb::SetMenuItemClientData(sub_menu_item, func);
			}
		}
		menu.AppendSubMenu(sub_menu, wxT("快速脚本"));
		sub_menu = new wxMenu;
		sub_menu->Append(ID_CALC_EDIT_NEW + CALC_STRATEGY, wxT("新建"));
		sub_menu->AppendSeparator();
		for (size_t i = 0, j = 0; i < all_strategy_func_.size(); i++)
		{
			zqdb::Calc::Func func(all_strategy_func_[i]);
			if (func.GetCalcLang() != CALC_LANG_C_CPP) {
				auto sub_menu_item = sub_menu->Append(ID_CALC_EDIT_STRATEGY + i, func.GetCalcName());
				zqdb::SetMenuItemClientData(sub_menu_item, func);
			}
		}
		menu.AppendSubMenu(sub_menu, wxT("策略算法"));
		evt.PopupMenu(&menu);
	}
}

void MyFrame::OnCalcEditNew(wxCommandEvent& evt)
{
	CALC_TYPE type = (CALC_TYPE)(evt.GetId() - ID_CALC_EDIT_NEW);
	wxString name;
	switch (type)
	{
	case CALC_MAJOR: {
		name = wxGetTextFromUser("请输入主图指标名称", APP_NAME, wxEmptyString, this);
	} break;
	case CALC_MINOR: {
		name = wxGetTextFromUser("请输入副图指标名称", APP_NAME, wxEmptyString, this);
	} break;
	case CALC_FILTER: {
		name = wxGetTextFromUser("请输入筛选算法名称", APP_NAME, wxEmptyString, this);
	} break;
	case CALC_SORT: {
		name = wxGetTextFromUser("请输入排序算法名称", APP_NAME, wxEmptyString, this);
	} break;
	case CALC_SCRIPT: {
		name = wxGetTextFromUser("请输入快速脚本名称", APP_NAME, wxEmptyString, this);
	} break;
	case CALC_STRATEGY: {
		name = wxGetTextFromUser("请输入策略算法名称", APP_NAME, wxEmptyString, this);
	} break;
	default: {
		return;
	} break;
	}
	if (name.empty()) {
		return;
	}
	wxGetApp().ShowCalcFrame (std::make_shared<zqdb::FuncContainerInfo>(type, name));
}

void MyFrame::OnCalcEditOpen(wxCommandEvent& evt)
{
	zqdb::Calc::Func func;
	if (!zqdb::GetMenuItemClientData(func, evt)) {
		return;
	}
	wxGetApp().ShowCalcFrame(std::make_shared<zqdb::FuncContainerInfo>(func));
}

void MyFrame::OnCalcTradeFlag(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	ZQDBSetCalcTradeFlag(!ZQDBGetCalcTradeFlag());
}

void MyFrame::OnCalcSetting(wxRibbonPanelEvent& evt)
{
	zqdb::TechDlg dlg(this);
	dlg.ShowModal();
}

void MyFrame::OnStrategy(wxRibbonButtonBarEvent& evt)
{
	auto pBar = evt.GetBar();
	size_t pos = evt.GetId() - ID_STRATEGY_BEGIN;
	if (pos < all_strategy_func_.size()) {
		HZQDB h = all_strategy_func_[pos];
		zqdb::Calc::Func func(h);
		std::string name = func.GetCalcName();
		auto strategy = wxGetApp().FindStrategy(name);
		if (!strategy) {
			auto huser = GetUser();
			if (!huser) {
				/*pBar->ToggleButton(evt.GetId(), false);
				wxMessageBox(wxT("没有登录交易账户!!!\n")
					wxT("请登录交易"), wxT("提示"), wxOK | wxCANCEL);
				return;*/
			}
			zqdb::TechDlg dlg(this, CALC_STRATEGY, name.c_str());
			if (wxID_OK == dlg.ShowModal()) {
				HZQDB input = zqdb::TechDlg::GetInputAttr(h);
				size_t target = zqdb::TechDlg::GetTarget(h);
				HZQDB data = Get();
				if (target) {
					//
				}
				strategy = wxGetApp().StartStrategy(h, input, data, huser);
				//Strategy停止时会释放input
				/*if (input) {
					ZQDBCloseCalcInput(input);
				}*/
			}
			else {
				pBar->ToggleButton(evt.GetId(), false);
			}
		}
		else {
			if (!strategy->IsRun()) {
				strategy->Start();
			}
			else {
				if (wxOK == wxMessageBox(wxT("策略正在运行中...\n")
					wxT("确定停止策略吗？"), wxT("提示"), wxOK | wxCANCEL)) {
					wxGetApp().StopStrategy(strategy);
					pBar->ToggleButton(evt.GetId(), strategy->IsRun());
				}
			}
			pBar->ToggleButton(evt.GetId(), strategy->IsRun());
		}
	}
}

void MyFrame::OnStrategySetting(wxRibbonPanelEvent& evt)
{
	zqdb::TechDlg dlg(this, CALC_STRATEGY);
	dlg.ShowModal();
}