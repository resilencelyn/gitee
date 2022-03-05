#include "mysettingsdlg.h"
#include "myapp.h"
#include "wx/hyperlink.h"

// ----------------------------------------------------------------------------
// MySettingsDlg implementation
// ----------------------------------------------------------------------------

std::string MySettingsDlg::GetFile(bool test)
{
	auto path_module_dir = boost::dll::this_line_location().remove_leaf();
	auto path_settings_file = path_module_dir;
	path_settings_file.append(APP_NAME).append(test?"test.json":"settings.json");
	auto settings_file = path_settings_file.string();
	return settings_file;
}

bool InnerHealthCheck(const boost::property_tree::ptree& cfg)
{
	//检查需要重新设置的版本号
	auto opt_version = cfg.get<std::string>("version", "");
	if (opt_version.compare("1.7.1.1") < 0) { 
		return false;
	}
	//检查模块
	bool check_module = false;
	auto opt_module = cfg.get_child_optional("module");
	if (opt_module)
	{
		const auto &cfg_module = opt_module.get();
		BOOST_FOREACH(const boost::property_tree::ptree::value_type &cfgi, cfg_module)
		{
			std::string name = cfgi.second.get<std::string>("name", "");
			std::string path = cfgi.second.get<std::string>("path", "");
			check_module = !name.empty() && !path.empty();
		}
	}
	if (!check_module) {
		wxMessageBox(wxT("没有发现模块，请检查设置"));
		return false;
	}
	return true;
}

bool MySettingsDlg::HealthCheck(std::string& settings_file)
{
	settings_file = GetFile();
	boost::property_tree::ptree cfg;
	if (!XUtil::json_from_file(settings_file, cfg)) {
		return false;
	}
	return InnerHealthCheck(cfg);
}

bool MySettingsDlg::HealthCheck(const boost::property_tree::ptree& cfg)
{
	return InnerHealthCheck(cfg);
}

wxBEGIN_EVENT_TABLE(MySettingsDlg, Base)
EVT_BUTTON(wxID_OK, MySettingsDlg::OnOK)
EVT_BUTTON(wxID_CANCEL, MySettingsDlg::OnCancel)
wxEND_EVENT_TABLE()

MySettingsDlg::MySettingsDlg(boost::property_tree::ptree& cfg)
	: Base(nullptr, wxID_ANY, APP_NAME, wxDefaultPosition, wxDefaultSize), cfg_(cfg)
{
	SetIcon(wxICON(mytrader));

	std::string str;
	XUtil::json_to_str(str, cfg);
	ctrl_text_ = new wxRichTextCtrl(this, wxID_EDIT, utf2wxString(str.c_str()), wxDefaultPosition, wxSize(600, 400)
		, wxVSCROLL);
	//ctrl_text_ = new wxTextCtrl(this, wxID_EDIT, utf2wxString(str.c_str()), wxDefaultPosition, wxSize(800,600)
	//	, wxTE_MULTILINE | wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);

	btn_ok_ = new wxButton(this, wxID_OK, _("&OK"));
	btn_cancel_ = new wxButton(this, wxID_CANCEL, _("&Cancel"));

	wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

	sizerTop->Add(
		new wxStaticText(this, wxID_ANY, wxT("欢迎使用" APP_NAME " v" APP_VERSION)),
		0,
		wxALL,
		5
	);

	sizerTop->Add(
		new wxStaticText(this, wxID_ANY,
			wxT(APP_NAME "是一款强大的可定制的量化分析交易平台，第一次运行时需要适当配置以满足您的个性化需求。")
			),
		0, // No vertical stretching
		wxALL,
		5 // Border width
	);

	sizerTop->Add(ctrl_text_, 0, wxEXPAND, 5);

	auto link_mytrader = new wxGenericHyperlinkCtrl(this, wxID_ANY, wxT("www.mytrader.org.cn"), HOME_URL);
	sizerTop->Add(link_mytrader, 0, wxALL, 5);

	wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

	sizerBottom->AddStretchSpacer(1);
	sizerBottom->Add(btn_ok_, 0, wxALIGN_CENTER | wxALL, 5);
	sizerBottom->Add(btn_cancel_, 0, wxALIGN_CENTER | wxALL, 5);
	//sizerBottom->AddStretchSpacer(1);

	sizerTop->Add(sizerBottom, 0, wxEXPAND);

	SetSizerAndFit(sizerTop);

	Centre();

	SetEscapeId(wxID_CANCEL);

	btn_ok_->SetFocus();
	btn_ok_->SetDefault();
}

MySettingsDlg::~MySettingsDlg()
{
}

void MySettingsDlg::OnOK(wxCommandEvent& event)
{
	wxString str = ctrl_text_->GetValue();
	boost::property_tree::ptree cfg;
	if (!XUtil::json_from_str(wxString2utf(str), cfg)) {
		wxMessageBox(wxT("不合法的json字符串"));
		return;
	}
	if (!InnerHealthCheck(cfg)) {
		return;
	}
	cfg_ = cfg;
	EndModal(wxID_OK);
}

void MySettingsDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(wxID_CANCEL);
}

PrefsPageGeneralPanel::PrefsPageGeneralPanel(wxWindow *parent) : wxPanel(parent)
{
	m_useMarkdown = new wxCheckBox(this, wxID_ANY, "Use Markdown syntax");
	m_spellcheck = new wxCheckBox(this, wxID_ANY, "Check spelling");

	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_useMarkdown, wxSizerFlags().Border());
	sizer->Add(m_spellcheck, wxSizerFlags().Border());

	SetSizerAndFit(sizer);

	m_useMarkdown->Bind(wxEVT_CHECKBOX, &PrefsPageGeneralPanel::ChangedUseMarkdown, this);
	m_spellcheck->Bind(wxEVT_CHECKBOX, &PrefsPageGeneralPanel::ChangedSpellcheck, this);
}

bool PrefsPageGeneralPanel::TransferDataToWindow()
{
	//m_settingsCurrent = wxGetApp().GetSettings();
	//m_useMarkdown->SetValue(m_settingsCurrent.m_useMarkdown);
	//m_spellcheck->SetValue(m_settingsCurrent.m_spellcheck);
	return true;
}

bool PrefsPageGeneralPanel::TransferDataFromWindow()
{
	// Called on platforms with modal preferences dialog to save and apply
	// the changes.
	//wxGetApp().UpdateSettings(m_settingsCurrent);
	return true;
}

void PrefsPageGeneralPanel::UpdateSettingsIfNecessary()
{
	// On some platforms (OS X, GNOME), changes to preferences are applied
	// immediately rather than after the OK or Apply button is pressed, so
	// we need to take them into account as soon as they happen. On others
	// (MSW), we need to wait until the changes are accepted by the user by
	// pressing the "OK" button. To reuse the same code for both cases, we
	// always update m_settingsCurrent object under all platforms, but only
	// update the real application settings if necessary here.
	if (wxPreferencesEditor::ShouldApplyChangesImmediately())
	{
		//wxGetApp().UpdateSettings(m_settingsCurrent);
	}
}

void PrefsPageGeneralPanel::ChangedUseMarkdown(wxCommandEvent& e)
{
	//m_settingsCurrent.m_useMarkdown = e.IsChecked();
	UpdateSettingsIfNecessary();
}

void PrefsPageGeneralPanel::ChangedSpellcheck(wxCommandEvent& e)
{
	//m_settingsCurrent.m_spellcheck = e.IsChecked();
	UpdateSettingsIfNecessary();
}

PrefsPageAboutPanel::PrefsPageAboutPanel(wxWindow *parent) : wxPanel(parent)
{
	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	auto stc_ver = new wxStaticText(this, wxID_ANY, wxString::Format(wxT("mytrader v%s"), MYTRADER_VERSION));
	sizer->Add(stc_ver, wxSizerFlags().Center());
	//wxListBox *box = new wxListBox(this, wxID_ANY);
	//box->SetMinSize(wxSize(400, 300));
	//sizer->Add(box, wxSizerFlags(1).Border().Expand());
	//logo
	auto link_mytrader = new wxGenericHyperlinkCtrl(this, wxID_ANY, wxT("www.mytrader.org.cn"), wxT("www.mytrader.org.cn"));
	//qq
	sizer->Add(link_mytrader, wxSizerFlags().Center());
	auto link_mytrader_src = new wxGenericHyperlinkCtrl(this, wxID_ANY, wxT("https://gitee.com/7thTool/mytrader"), wxT("https://gitee.com/7thTool/mytrader"));
	sizer->Add(link_mytrader_src, wxSizerFlags().Center());
	auto link_zqdb_src = new wxGenericHyperlinkCtrl(this, wxID_ANY, wxT("https://gitee.com/7thTool/zqdb"), wxT("https://gitee.com/7thTool/zqdb"));
	sizer->Add(link_zqdb_src, wxSizerFlags().Center());

	SetSizerAndFit(sizer);
}

wxBEGIN_EVENT_TABLE(MyTechDlg, Base)
EVT_BUTTON(wxID_OK, MyTechDlg::OnOK)
EVT_BUTTON(wxID_CANCEL, MyTechDlg::OnCancel)
wxEND_EVENT_TABLE()

MyTechDlg::MyTechDlg(wxWindow *parent) : Base(parent, wxID_ANY, _("Tech"))
{
	auto anysec = wxGetApp().GetTechCycleAnySec();
	auto anymin = wxGetApp().GetTechCycleAnyMin();

	ctrl_cycle_anysec_ = new wxSpinCtrl(this, wxID_ANY,
		wxString::Format(wxT("%zu"), anysec), 
		wxDefaultPosition, wxSize(100,-1), wxSP_ARROW_KEYS,
		5, 55, anysec);
	ctrl_cycle_anymin_ = new wxSpinCtrl(this, wxID_ANY,
		wxString::Format(wxT("%zu"), anymin),
		wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS,
		5, 55, anymin);

	auto btn_ok = new wxButton(this, wxID_OK, _("&OK"));
	auto btn_cancel = new wxButton(this, wxID_CANCEL, _("&Cancel"));

	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	// cycle pane
	wxStaticBox *box_cycle = new wxStaticBox(this, wxID_ANY, "Custom cycle");
	wxSizer *sizer_cycle = new wxStaticBoxSizer(box_cycle, wxVERTICAL);
	wxSizer *sizer_anysec = new wxBoxSizer(wxHORIZONTAL);
	sizer_anysec->Add(new wxStaticText(this, wxID_ANY, _("Custom anysec")), wxSizerFlags(0).Border(wxRIGHT).CentreVertical());
	sizer_anysec->Add(ctrl_cycle_anysec_, wxSizerFlags(1).Border(wxLEFT).CentreVertical());
	sizer_cycle->Add(sizer_anysec, 0, wxALL | wxGROW, 5);
	wxSizer *sizer_anymin = new wxBoxSizer(wxHORIZONTAL);
	sizer_anymin->Add(new wxStaticText(this, wxID_ANY, _("Custom anymin")), wxSizerFlags(0).Border(wxRIGHT).CentreVertical());
	sizer_anymin->Add(ctrl_cycle_anymin_, wxSizerFlags(1).Border(wxLEFT).CentreVertical());
	sizer_cycle->Add(sizer_anymin, 0, wxALL | wxGROW, 5);
	sizer->Add(sizer_cycle, 1, wxEXPAND, 5);

	wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

	sizerBottom->AddStretchSpacer(1);
	sizerBottom->Add(btn_ok, 0, wxALIGN_CENTER | wxALL, 5);
	sizerBottom->Add(btn_cancel, 0, wxALIGN_CENTER | wxALL, 5);
	//sizerBottom->AddStretchSpacer(1);

	sizer->Add(sizerBottom, 0, wxEXPAND);

	SetSizerAndFit(sizer);

	Centre();

	SetEscapeId(wxID_CANCEL);

	btn_ok->SetFocus();
	btn_ok->SetDefault();
}

void MyTechDlg::OnOK(wxCommandEvent& event)
{
	auto anysec = ctrl_cycle_anysec_->GetValue();
	if (anysec != wxGetApp().GetTechCycleAnySec()) {
		wxGetApp().SetTechCycleAnySec(anysec);
		wxGetApp().GetSkinInfo()->UpdateTechCycleExBitmap16(CYC_ANYSEC, anysec);
	}
	auto anymin = ctrl_cycle_anymin_->GetValue();
	if (anymin != wxGetApp().GetTechCycleAnyMin()) {
		wxGetApp().SetTechCycleAnyMin(anymin);
		wxGetApp().GetSkinInfo()->UpdateTechCycleExBitmap16(CYC_ANYMIN, anymin);
	}
	EndModal(wxID_OK);
}

void MyTechDlg::OnCancel(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}

wxBEGIN_EVENT_TABLE(MyAboutDlg, Base)
EVT_BUTTON(wxID_OK, MyAboutDlg::OnOK)
EVT_BUTTON(wxID_CANCEL, MyAboutDlg::OnCancel)
wxEND_EVENT_TABLE()

MyAboutDlg::MyAboutDlg(wxWindow *parent) : Base(parent, wxID_ANY, _("About"))
{
	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	sizer->AddSpacer(5);

	auto stc_logo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxICON(mytrader)),
		wxDefaultPosition, wxDefaultSize);
	//stc_logo->SetScaleMode(wxStaticBitmap::Scale_AspectFit);
	sizer->Add(stc_logo, wxSizerFlags().Center());
	auto stc_ver = new wxStaticText(this, wxID_ANY, wxString::Format(wxT("mytrader v%s"), MYTRADER_VERSION));
	sizer->Add(stc_ver, wxSizerFlags().Center());
	//wxListBox *box = new wxListBox(this, wxID_ANY);
	//box->SetMinSize(wxSize(400, 300));
	//sizer->Add(box, wxSizerFlags(1).Border().Expand());
	auto link_mytrader = new wxGenericHyperlinkCtrl(this, wxID_ANY, wxT("www.mytrader.org.cn"), HOME_URL);
	sizer->Add(link_mytrader, wxSizerFlags().Center());
	sizer->Add(new wxStaticText(this, wxID_ANY, wxT("                                                              ")), wxSizerFlags().Center());
	auto stc_qqq = new wxStaticText(this, wxID_ANY, wxT("QQ群：207652879"));
	sizer->Add(stc_qqq, wxSizerFlags().Center());
	auto stc_email = new wxStaticText(this, wxID_ANY, wxT("邮箱：i7thtool@qq.com"));
	sizer->Add(stc_email, wxSizerFlags().Center());
	auto stc_wx = new wxStaticText(this, wxID_ANY, wxT("微信：zhangzq9527"));
	sizer->Add(stc_wx, wxSizerFlags().Center());
	//help
	//auto link_help_doc = new wxGenericHyperlinkCtrl(this, wxID_ANY, _("help docs"), wxT("https://gitee.com/7thTool/mytrader"));
	//sizer->Add(link_help_doc, wxSizerFlags().Center());
	//src
	//auto link_mytrader_src = new wxGenericHyperlinkCtrl(this, wxID_ANY, wxT("https://gitee.com/7thTool/mytrader"), wxT("https://gitee.com/7thTool/mytrader"));
	//sizer->Add(link_mytrader_src, wxSizerFlags().Center());
	//auto link_zqdb_src = new wxGenericHyperlinkCtrl(this, wxID_ANY, wxT("https://gitee.com/7thTool/zqdb"), wxT("https://gitee.com/7thTool/zqdb"));
	//sizer->Add(link_zqdb_src, wxSizerFlags().Center());

	auto btn_ok = new wxButton(this, wxID_OK, _("&OK"));
	//auto btn_cancel = new wxButton(this, wxID_CANCEL, _("&Cancel"));

	wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

	sizerBottom->AddStretchSpacer(1);
	sizerBottom->Add(btn_ok, 0, wxALIGN_CENTER | wxALL, 5);
	//sizerBottom->Add(btn_cancel, 0, wxALIGN_CENTER | wxALL, 5);
	//sizerBottom->AddStretchSpacer(1);

	sizer->Add(sizerBottom, 0, wxEXPAND);

	SetSizerAndFit(sizer);

	CentreOnScreen();

	//SetEscapeId(wxID_CANCEL);

	btn_ok->SetFocus();
	btn_ok->SetDefault();
}

void MyAboutDlg::OnOK(wxCommandEvent& event)
{
	EndModal(wxID_OK);
}

void MyAboutDlg::OnCancel(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}

///

wxBEGIN_EVENT_TABLE(MyCheckDlg, Base)
	EVT_BUTTON(wxID_OK, MyCheckDlg::OnOK)
	EVT_BUTTON(wxID_CANCEL, MyCheckDlg::OnCancel)
wxEND_EVENT_TABLE()

MyCheckDlg::MyCheckDlg(wxWindow* parent, const wxString& text, const wxString& check_text, bool check_value, const wxString& title, long style)
	: Base(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
	SetIcon(wxICON(mytrader));

	auto stc_bmp = new wxStaticBitmap(this, wxID_ANY, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, wxSize(32, 32)));

	ctrl_text_ = new wxStaticText(this, wxID_ANY, text);
	ctrl_check_ = new wxCheckBox(this, wxID_ANY, check_text);
	ctrl_check_->SetValue(check_value);
	if(style & wxOK) 
		btn_ok_ = new wxButton(this, wxID_OK, _("&OK"));
	if (style & wxCANCEL)
		btn_cancel_ = new wxButton(this, wxID_CANCEL, _("&Cancel"));

	wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *sizerMain = new wxBoxSizer(wxHORIZONTAL);

	sizerMain->Add(stc_bmp, 0, wxALIGN_TOP | wxALL, 5);

	wxBoxSizer *sizerRight = new wxBoxSizer(wxVERTICAL);

	sizerRight->Add(
		ctrl_text_,
		1,
		wxALL, 5
	);

	sizerRight->Add(
		ctrl_check_,
		0, // No vertical stretching
		wxALL, 5
	);

	sizerMain->Add(sizerRight, 1, wxEXPAND | wxALL, 5);

	sizerTop->Add(sizerMain, 1, wxEXPAND | wxALL, 5);

	wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

	sizerBottom->AddStretchSpacer(1);
	if(btn_ok_)
		sizerBottom->Add(btn_ok_, 0, wxALIGN_CENTER | wxALL, 5);
	if (btn_cancel_)
		sizerBottom->Add(btn_cancel_, 0, wxALIGN_CENTER | wxALL, 5);
	//sizerBottom->AddStretchSpacer(1);

	sizerTop->Add(sizerBottom, 0, wxEXPAND);

	SetSizerAndFit(sizerTop);

	Centre();

	SetEscapeId(wxID_CANCEL);

	if ((style & wxCANCEL_DEFAULT) && btn_cancel_) {
		btn_cancel_->SetFocus();
		btn_cancel_->SetDefault();
	}
	else if (btn_ok_) {
		btn_ok_->SetFocus();
		btn_ok_->SetDefault();
	}
}

bool MyCheckDlg::IsCheck()
{
	return ctrl_check_->GetValue();
}

void MyCheckDlg::OnOK(wxCommandEvent& event)
{
	EndModal(wxOK);
}

void MyCheckDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(wxCANCEL);
}

///

wxBEGIN_EVENT_TABLE(MyTestDlg, Base)
EVT_CHOICE(CTRL_ID_BEGIN, MyTestDlg::OnBeginChoice)
EVT_CHOICE(CTRL_ID_END, MyTestDlg::OnEndChoice)
EVT_COMMAND_SCROLL(CTRL_ID_SPEED, MyTestDlg::OnSpeed)
EVT_BUTTON(wxID_OK, MyTestDlg::OnOK)
EVT_BUTTON(wxID_CANCEL, MyTestDlg::OnCancel)
wxEND_EVENT_TABLE()

MyTestDlg::MyTestDlg(wxWindow* parent)
	: Base(parent, wxID_ANY, wxT("超级回测"), wxDefaultPosition, wxDefaultSize)
{
	SetIcon(wxICON(mytrader));

	wxArrayString records;
	for (size_t i = 0, j = ZQDBGetRecordCount(); i < j; i++)
	{
		size_t date = ZQDBGetRecordDate(i);
		if (date == ZQDBIsRecording()) {
			continue;
		}
		records.push_back(wxString::Format(wxT("%zu"), date));
	}

	ctrl_begin_ = new wxChoice(this, CTRL_ID_BEGIN, wxDefaultPosition, wxSize(230,-1), records);
	ctrl_begin_->SetSelection(0);
	ctrl_end_ = new wxChoice(this, CTRL_ID_END, wxDefaultPosition, wxDefaultSize, records);
	ctrl_end_->SetSelection(0);
	ctrl_end_->Disable();
	ctrl_speed_ = new wxSlider(this, CTRL_ID_SPEED, 50, 1, 100);

	btn_ok_ = new wxButton(this, wxID_OK, _("&OK"));
	btn_cancel_ = new wxButton(this, wxID_CANCEL, _("&Cancel"));

	wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

	// 日期选择
	wxSizer *sizer_begin = new wxBoxSizer(wxHORIZONTAL);
	sizer_begin->Add(new wxStaticText(this, wxID_ANY, wxT("开始日期")), wxSizerFlags(0).Border(wxRIGHT).CentreVertical());
	sizer_begin->Add(ctrl_begin_, wxSizerFlags(1).Border(wxLEFT).CentreVertical());
	sizerTop->Add(sizer_begin, 0, wxALL | wxGROW, 5);
	wxSizer *sizer_end = new wxBoxSizer(wxHORIZONTAL);
	sizer_end->Add(new wxStaticText(this, wxID_ANY, wxT("结束日期")), wxSizerFlags(0).Border(wxRIGHT).CentreVertical());
	sizer_end->Add(ctrl_end_, wxSizerFlags(1).Border(wxLEFT).CentreVertical());
	sizerTop->Add(sizer_end, 0, wxALL | wxGROW, 5);

	sizerTop->AddSpacer(5);

	// 速度控制
	wxSizer *sizer_speed = new wxBoxSizer(wxHORIZONTAL);
	sizer_speed->Add(new wxStaticText(this, wxID_ANY, wxT("回测速度")), wxSizerFlags(0).Border(wxRIGHT).CentreVertical());
	sizer_speed->Add(ctrl_speed_, wxSizerFlags(1).Border(wxLEFT).CentreVertical());
	sizerTop->Add(sizer_speed, 0, wxALL | wxGROW, 5);
	
	wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

	sizerBottom->AddStretchSpacer(1);
	sizerBottom->Add(btn_ok_, 0, wxALIGN_CENTER | wxALL, 5);
	sizerBottom->Add(btn_cancel_, 0, wxALIGN_CENTER | wxALL, 5);
	//sizerBottom->AddStretchSpacer(1);

	sizerTop->Add(sizerBottom, 0, wxEXPAND);

	SetSizerAndFit(sizerTop);

	Centre();

	SetEscapeId(wxID_CANCEL);

	btn_ok_->SetFocus();
	btn_ok_->SetDefault();
}

size_t MyTestDlg::GetBegin()
{
	long val = 0;
	ctrl_begin_->GetStringSelection().ToLong(&val);
	return val;
}

size_t MyTestDlg::GetEnd()
{
	long val = 0;
	ctrl_end_->GetStringSelection().ToLong(&val);
	return val;
}

size_t MyTestDlg::GetSpeed()
{
	return ctrl_speed_->GetValue();
}

void MyTestDlg::OnBeginChoice(wxCommandEvent& event)
{
	long sel = event.GetSelection();
	ctrl_end_->SetSelection(sel);
}

void MyTestDlg::OnEndChoice(wxCommandEvent& event)
{
	//long sel = event.GetSelection();
}

void MyTestDlg::OnSpeed(wxScrollEvent& event)
{
	//wxASSERT_MSG(event.GetInt() == m_slider->GetValue(),
	//	"slider value should be the same");

	//wxEventType eventType = event.GetEventType();

	///*
	//This array takes the EXACT order of the declarations in
	//include/wx/event.h
	//(section "wxScrollBar and wxSlider event identifiers")
	//*/
	//static const wxString eventNames[] =
	//{
	//	"wxEVT_SCROLL_TOP",
	//	"wxEVT_SCROLL_BOTTOM",
	//	"wxEVT_SCROLL_LINEUP",
	//	"wxEVT_SCROLL_LINEDOWN",
	//	"wxEVT_SCROLL_PAGEUP",
	//	"wxEVT_SCROLL_PAGEDOWN",
	//	"wxEVT_SCROLL_THUMBTRACK",
	//	"wxEVT_SCROLL_THUMBRELEASE",
	//	"wxEVT_SCROLL_CHANGED"
	//};

	//int index = eventType - wxEVT_SCROLL_TOP;

	///*
	//If this assert is triggered, there is an unknown slider event which
	//should be added to the above eventNames array.
	//*/
	//wxASSERT_MSG(index >= 0 && (size_t)index < WXSIZEOF(eventNames),
	//	"Unknown slider event");


	//static int s_numSliderEvents = 0;

	//wxLogMessage("Slider event #%d: %s (pos = %d, int value = %d)",
	//	s_numSliderEvents++,
	//	eventNames[index],
	//	event.GetPosition(),
	//	event.GetInt());
}

void MyTestDlg::OnOK(wxCommandEvent& event)
{
	EndModal(wxOK);
}

void MyTestDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(wxCANCEL);
}
