#include "myapp.h"
#include "mybaseframe.h"
#include "mylogdlg.h"
#include "mysettingsdlg.h"
#include "wx/sysopt.h"
#include "wx/display.h"

///

wxBEGIN_EVENT_TABLE(MyBaseFrame, wxFrame)
EVT_RIBBONBAR_PAGE_CHANGING(wxID_ANY, MyBaseFrame::OnPageChanging)
EVT_RIBBONBAR_HELP_CLICK(wxID_ANY, MyBaseFrame::OnHelpClicked)
EVT_SIZE(MyBaseFrame::OnSizeEvent)
EVT_CLOSE(MyBaseFrame::OnCloseEvent)
EVT_TIMER(wxID_ANY, MyBaseFrame::OnTimer)
wxEND_EVENT_TABLE()

#include "transparent.xpm"
//#include "zoom_in.xpm"
//#include "zoom_out.xpm"

MyBaseFrame::MyBaseFrame(const char* xml, size_t xmlflag)
    : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(1280, 768), wxDEFAULT_FRAME_STYLE)
{
	//wxConfigBase *pConfig = wxConfigBase::Get();

	SetIcon(wxICON(mytrader));
	//SetSize(pConfig->ReadLong(wxT("frame/width"), 800), pConfig->ReadLong(wxT("frame/height"), 600));
	
	/*auto frame = wxGetApp().GetFrame();
	wxDisplay display(frame ? wxDisplay::GetFromWindow(frame) : 0);
	SetSize(display.GetClientArea());*/

    m_ribbon = new wxRibbonBar(this,-1,wxDefaultPosition, wxDefaultSize, wxRIBBON_BAR_FLOW_HORIZONTAL
		| wxRIBBON_BAR_SHOW_PAGE_LABELS
		| wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS
		| wxRIBBON_BAR_SHOW_PANEL_MINIMISE_BUTTONS
		| wxRIBBON_BAR_ALWAYS_SHOW_TABS
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
		//AddStartPage();
		/*size_t product_id = ID_MARKET_PRODUCT;
		zqdb::AllExchange allexchange;
		for (size_t i = 0; i < allexchange.size(); i++)
		{
			product_id = AddExchangePage(allexchange[i], product_id);
		}
		assert(product_id < ID_MARKET_PRODUCT_MAX);
		Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MyBaseFrame::OnMarketProduct, this, ID_MARKET_PRODUCT, ID_MARKET_PRODUCT_MAX);*/
		//AddFuture();
		//AddOption();
		//AddCalcPage();
		//AddNewPage();
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

    //m_ribbon->Realize();

#if wxUSE_INFOBAR
	infobar_ = new wxInfoBar(this);
#endif
}

MyBaseFrame::~MyBaseFrame()
{
	
}

void MyBaseFrame::AddFrame(MyBaseFrame* frame)
{
	RemoveLastPages();
	auto page = new wxRibbonPage(m_ribbon, wxID_ANY, frame->GetTitle());
	page->SetClientData(frame);
	frames_[frame] = page;
	AddLastPages();
	m_ribbon->Realize();
	Layout();
}

void MyBaseFrame::RemoveFrame(MyBaseFrame* frame)
{
	auto it = frames_.find(frame);
	if (it != frames_.end()) {
		m_ribbon->DeletePage(m_ribbon->GetPageNumber(it->second));
		frames_.erase(it);
		m_ribbon->Realize();
		Layout();
	}
}

void MyBaseFrame::ActivateFrame(MyBaseFrame* frame)
{
	auto it = frames_.find(frame);
	if (it != frames_.end()) {
		m_ribbon->SetActivePage(it->second);
	}
}

void MyBaseFrame::UpdateAllPages()
{
	m_ribbon->ClearPages();
	AddFirstPages();
	for (auto& pr : frames_)
	{
		auto page = new wxRibbonPage(m_ribbon, wxID_ANY, pr.first->GetTitle());
		page->SetClientData(pr.first);
		pr.second = page;
	}
	AddLastPages();
	m_ribbon->Realize();
	Layout();
}

void MyBaseFrame::ShowMessage(const wxString& msg, int flags)
{
#if wxUSE_INFOBAR
	infobar_->ShowMessage(msg, flags);
#endif
}

//
//size_t MyBaseFrame::AddExchangePage(HZQDB h, size_t product_id)
//{
//	zqdb::Exchange exchange(h);
//	auto strExchange = utf2wxString(exchange->Exchange);
//	auto strName = wxT(" ") + utf2wxString(exchange->Name) + wxT(" ");
//	wxRibbonPage* home = new wxRibbonPage(m_ribbon, wxID_ANY, strExchange);
//
//	auto market_panel = new wxRibbonPanel(home, ID_MARKET_EXCHANGE + 0, strName);
//	wxRibbonButtonBar* market_bar = new wxRibbonButtonBar(market_panel);
//	zqdb::AllProduct allproduct(h);
//	/*std::sort(allproduct.begin(), allproduct.end(), [](HZQDB x, HZQDB y) {
//		zqdb::Product xproduct(x);
//		zqdb::Product yproduct(y);
//		return strcmp(xproduct->Name, yproduct->Name) < 0;
//	});*/
//	for (size_t k = 0; k < allproduct.size(); k++)
//	{
//		zqdb::Product product(allproduct[k]);
//		auto item = market_bar->AddButton(product_id, utf2wxString(product->Name), wxBitmap(transparent_xpm));
//		market_bar->SetItemClientData(item, allproduct[k]);
//		market_bar->SetButtonMaxSizeClass(product_id, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
//		market_bar->SetButtonMinSizeClass(product_id, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
//		++product_id;
//	}
//
//	return product_id;
//}

void MyBaseFrame::AddLastPages()
{
	wxRibbonPage* scheme = new wxRibbonPage(m_ribbon, wxID_ANY, "帮助");
	last_pages_[scheme] = std::bind(&MyBaseFrame::OnHelpClicked, this, std::placeholders::_1);
}

void MyBaseFrame::RemoveLastPages()
{
	for (auto& pr : last_pages_)
	{
		m_ribbon->DeletePage(m_ribbon->GetPageNumber(pr.first));
	}
	last_pages_.clear();
}

int MyBaseFrame::FilterEvent(wxEvent& event)
{
	// Continue processing the event normally as well.
	return wxEventFilter::Event_Skip;
}

void MyBaseFrame::OnSkinInfoChanged()
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
	auto statusbar = GetStatusBar();
	if (statusbar) {
		statusbar->SetBackgroundColour(skin_info_ptr_->crPrimary);
		statusbar->SetForegroundColour(skin_info_ptr_->crTertiary);
	}
	Layout();
	Thaw();
}

void MyBaseFrame::OnPageChanging(wxRibbonBarEvent& evt)
{
	evt.Veto();
	auto page = evt.GetPage();
	if (page) {
		auto it = last_pages_.find(page);
		if (it != last_pages_.end()) {
			it->second(evt);
		}
		else {
			auto frame = (MyBaseFrame*)page->GetClientData();
			if (frame) {
				wxGetApp().ShowFrame(frame);
			}
			else {
				wxMessageBox(wxT("敬请期待"), evt.GetPage()->GetLabelText(), wxICON_INFORMATION | wxOK, this);
			}
		}
	}
}

void MyBaseFrame::OnHelpClicked(wxRibbonBarEvent& evt)
{
	/*wxString filename = wxGetTextFromUser(
		"Enter the URL",
		"exec sample",
		s_url,
		this
	);
	if (filename.empty())
		return;
	s_url = filename;*/
	if (!wxLaunchDefaultBrowser(HOME_URL)) {
		wxLogError("Failed to open URL \"%s\"", HOME_URL);
	}
}

// This shows how to hide ribbon dynamically if there is not enough space.
void MyBaseFrame::OnSizeEvent(wxSizeEvent& evt)
{
   /* if ( evt.GetSize().GetWidth() < 200 )
        m_ribbon->Hide();
    else
        m_ribbon->Show();*/

    evt.Skip();
}

void MyBaseFrame::OnCloseEvent(wxCloseEvent& evt)
{
	evt.Skip();
}

void MyBaseFrame::OnTimer(wxTimerEvent& evt)
{
	bool change = false;
	for (auto& pr : frames_)
	{
		auto title = pr.first->GetTitle();
		if (title != pr.second->GetLabel()) {
			change = true;
			pr.second->SetLabel(title);
		}
	}
	if (change) {
		m_ribbon->Realize();
	}
}

