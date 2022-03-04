#include "mycalcdlg.h"


wxPGProperty* GetRealRoot(wxPropertyGrid *grid)
{
	wxPGProperty *property = grid->GetRoot();
	return property ? grid->GetFirstChild(property) : NULL;
}

void GetColumnWidths(wxClientDC &dc, wxPropertyGrid *grid, wxPGProperty *root, int width[3])
{
	wxPropertyGridPageState *state = grid->GetState();

	width[0] =
		width[1] =
		width[2] = 0;
	int minWidths[3] = { state->GetColumnMinWidth(0),
		state->GetColumnMinWidth(1),
		state->GetColumnMinWidth(2) };
	unsigned ii;
	for (ii = 0; ii < root->GetChildCount(); ++ii)
	{
		wxPGProperty* p = root->Item(ii);

		width[0] = wxMax(width[0], state->GetColumnFullWidth(dc, p, 0));
		width[1] = wxMax(width[1], state->GetColumnFullWidth(dc, p, 1));
		width[2] = wxMax(width[2], state->GetColumnFullWidth(dc, p, 2));
	}
	for (ii = 0; ii < root->GetChildCount(); ++ii)
	{
		wxPGProperty* p = root->Item(ii);
		if (p->IsExpanded())
		{
			int w[3];
			GetColumnWidths(dc, grid, p, w);
			width[0] = wxMax(width[0], w[0]);
			width[1] = wxMax(width[1], w[1]);
			width[2] = wxMax(width[2], w[2]);
		}
	}

	width[0] = wxMax(width[0], minWidths[0]);
	width[1] = wxMax(width[1], minWidths[1]);
	width[2] = wxMax(width[2], minWidths[2]);
}

void GetColumnWidths(wxPropertyGrid *grid, wxPGProperty *root, int width[3])
{
	wxClientDC dc(grid);
	dc.SetFont(grid->GetFont());
	GetColumnWidths(dc, grid, root, width);
}

void SetMinSize(wxPropertyGrid *grid)
{
	wxPGProperty *p = GetRealRoot(grid);
	wxPGProperty *first = grid->wxPropertyGridInterface::GetFirst(wxPG_ITERATE_ALL);
	wxPGProperty *last = grid->GetLastItem(wxPG_ITERATE_DEFAULT);
	wxRect rect = grid->GetPropertyRect(first, last);
	int height = rect.height + 2 * grid->GetVerticalSpacing();

	// add some height when the root item is collapsed,
	// this is needed to prevent the vertical scroll from showing
	if (!grid->IsPropertyExpanded(p))
		height += 2 * grid->GetVerticalSpacing();

	int width[3];
	GetColumnWidths(grid, grid->GetRoot(), width);
	rect.width = width[0] + width[1] + width[2];

	int minWidth = (wxSystemSettings::GetMetric(wxSYS_SCREEN_X, grid->GetParent()) * 3) / 2;
	int minHeight = (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y, grid->GetParent()) * 3) / 2;

	wxSize size(wxMin(minWidth, rect.width + grid->GetMarginWidth()), wxMin(minHeight, height));
	grid->SetMinSize(size);

	int proportions[3];
	proportions[0] = wxRound((double)width[0] / size.x*100.0);
	proportions[1] = wxRound((double)width[1] / size.x*100.0);
	proportions[2] = wxMax(100 - proportions[0] - proportions[1], 0);
	grid->SetColumnProportion(0, proportions[0]);
	grid->SetColumnProportion(1, proportions[1]);
	grid->SetColumnProportion(2, proportions[2]);
	grid->ResetColumnSizes(true);
}

wxIMPLEMENT_CLASS(MyScriptPropertyDlg, wxPopupWindow);
wxBEGIN_EVENT_TABLE(MyScriptPropertyDlg, wxPopupWindow)
//EVT_PG_ITEM_COLLAPSED(ID_POPUPGRID, MyScriptPropertyDlg::OnCollapse)
//EVT_PG_ITEM_EXPANDED(ID_POPUPGRID, MyScriptPropertyDlg::OnExpand)
wxEND_EVENT_TABLE()

MyScriptPropertyDlg::MyScriptPropertyDlg(wxWindow *parent) : wxPopupWindow(parent, wxBORDER_NONE | wxWANTS_CHARS)
{
	m_panel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
	m_grid = new wxPropertyGrid(m_panel, wxID_ANY, wxDefaultPosition, wxSize(400, 400), wxPG_SPLITTER_AUTO_CENTER);
	m_grid->SetColumnCount(3);

	wxPGProperty *prop = m_grid->Append(new wxStringProperty("test_name", wxPG_LABEL, "test_value"));
	m_grid->SetPropertyAttribute(prop, wxPG_ATTR_UNITS, "type");
	wxPGProperty *prop1 = m_grid->AppendIn(prop, new wxStringProperty("sub_name1", wxPG_LABEL, "sub_value1"));

	m_grid->AppendIn(prop1, new wxSystemColourProperty("Cell Colour", wxPG_LABEL, m_grid->GetGrid()->GetCellBackgroundColour()));
	wxPGProperty *prop2 = m_grid->AppendIn(prop, new wxStringProperty("sub_name2", wxPG_LABEL, "sub_value2"));
	m_grid->AppendIn(prop2, new wxStringProperty("sub_name21", wxPG_LABEL, "sub_value21"));

	wxArrayDouble arrdbl;
	arrdbl.Add(-1.0); arrdbl.Add(-0.5); arrdbl.Add(0.0); arrdbl.Add(0.5); arrdbl.Add(1.0);
	//m_grid->AppendIn(prop, new wxArrayDoubleProperty("ArrayDoubleProperty", wxPG_LABEL, arrdbl));
	m_grid->AppendIn(prop, new wxFontProperty("Font", wxPG_LABEL));
	m_grid->AppendIn(prop2, new wxStringProperty("sub_name22", wxPG_LABEL, "sub_value22"));
	m_grid->AppendIn(prop2, new wxStringProperty("sub_name23", wxPG_LABEL, "sub_value23"));
	prop2->SetExpanded(false);

	::SetMinSize(m_grid);

	m_sizer = new wxBoxSizer(wxVERTICAL);
	m_sizer->Add(m_grid, wxSizerFlags(0).Expand().Border(wxALL, 0));
	m_panel->SetAutoLayout(true);
	m_panel->SetSizer(m_sizer);
	m_sizer->Fit(m_panel);
	m_sizer->Fit(this);
}
//
//void MyScriptPropertyDlg::OnCollapse(wxPropertyGridEvent& WXUNUSED(event))
//{
//	wxLogMessage("OnCollapse");
//	Fit();
//}
//
//void MyScriptPropertyDlg::OnExpand(wxPropertyGridEvent& WXUNUSED(event))
//{
//	wxLogMessage("OnExpand");
//	Fit();
//}

void MyScriptPropertyDlg::Fit()
{
	::SetMinSize(m_grid);
	m_sizer->Fit(m_panel);
	wxPoint pos = GetScreenPosition();
	wxSize size = m_panel->GetScreenRect().GetSize();
	SetSize(pos.x, pos.y, size.x, size.y);
}


// ----------------------------------------------------------------------------
// MyCalcDlg implementation
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyCalcDlg, Base)
EVT_PG_CHANGED(wxID_ANY, MyCalcDlg::OnPropertyGridChange)
//EVT_PG_CHANGING(wxID_ANY, MyCalcDlg::OnPropertyGridChanging)
EVT_BUTTON(wxID_OK, MyCalcDlg::OnOK)
EVT_BUTTON(wxID_CANCEL, MyCalcDlg::OnCancel)
wxEND_EVENT_TABLE()

HZQDB MyCalcDlg::GetInputAttr(HZQDB h)
{
	HZQDB input = nullptr;

	wxConfigBase *pConfig = wxConfigBase::Get();

	zqdb::Calc::Func func(h);

	size_t input_count = func.GetInputAttrCount();
	for (size_t i = 0; i < input_count; i++)
	{
		wxString value;
		if (pConfig->Read(wxString::Format(wxT("calc/%d/%s/input/%s"), func.GetCalcType(), func.GetCalcName(), func.GetInputAttrName(i)), &value)) {
			ZQDBSetCalcInputAttrAsStr(input, func.GetInputAttrName(i), wxString2utf(value).c_str());
		}
	}

	return input;
}

size_t MyCalcDlg::GetTarget(HZQDB h)
{
	int target = 0;

	wxConfigBase *pConfig = wxConfigBase::Get();

	zqdb::Calc::Func func(h);

	if (pConfig->Read(wxString::Format(wxT("calc/%d/%s/target"), func.GetCalcType(), func.GetCalcName()), &target)) {
		return target;
	}

	return 0;
}

MyCalcDlg::MyCalcDlg(wxWindow *parent, HZQDB h)
	:Base(parent, wxID_ANY, ZQDBGetCalcName(h), wxDefaultPosition, wxDefaultSize), h_(h)
{
	wxConfigBase *pConfig = wxConfigBase::Get();

	zqdb::Calc::Func func(h_);

	pg_input_ = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxSize(400, 100),
		wxPG_SPLITTER_AUTO_CENTER |
		wxPG_BOLD_MODIFIED);

	//pg_->Append(new wxStringProperty(wxT("名称"), wxPG_LABEL, func.GetCalcName()));
	//pg_->DisableProperty("名称");
	//pg_->Append(new wxIntProperty(wxT("类型"), wxPG_LABEL, func.GetCalcType()));
	//pg_->DisableProperty("类型");

	//pg_->Append(new wxPropertyCategory("输入", wxPG_LABEL));
	size_t input_count = func.GetInputAttrCount();
	for (size_t i = 0; i < input_count; i++)
	{
		wxString name(func.GetInputAttrName(i));
		wxString nameupr = name.Upper();
		wxString value;
		if (pConfig->Read(wxString::Format(wxT("calc/%d/%s/input/%s"), func.GetCalcType(), func.GetCalcName(), func.GetInputAttrName(i)), &value)) {
			//
		}
		else {
			value = func.GetInputAttrValue(i);
		}
		if (nameupr.Find("DIRECTION_TYPE") >= 0) {
			//
			// wxEnumProperty does not store strings or even list of strings
			// ( so that's why they are static in function ).
			static const wxString enum_DIRECTION_labels[] = { "LONG", "SHORT", "NET" };

			// this value array would be optional if values matched string indexes
			static long enum_DIRECTION_values[] = { DIRECTION_LONG, DIRECTION_SHORT, DIRECTION_NET };

			// note that the initial value (the last argument) is the actual value,
			// not index or anything like that. Thus, our value selects "Another Item".
			pg_input_->Append(new wxEnumProperty(name, wxPG_LABEL,
				wxArrayString(WXSIZEOF(enum_DIRECTION_labels), enum_DIRECTION_labels),
				wxArrayInt(enum_DIRECTION_values, enum_DIRECTION_values + WXSIZEOF(enum_DIRECTION_values)), DIRECTION_LONG));
		}
		else if (nameupr.Find("ORDER_TYPE") >= 0) {
			//
			// wxEnumProperty does not store strings or even list of strings
			// ( so that's why they are static in function ).
			static const wxString enum_ORDER_labels[] = { "LIMIT", "MARKET", "FAK", "FOK" };

			// this value array would be optional if values matched string indexes
			static long enum_ORDER_values[] = { ORDER_LIMIT, ORDER_MARKET, ORDER_FAK, ORDER_FOK };

			// note that the initial value (the last argument) is the actual value,
			// not index or anything like that. Thus, our value selects "Another Item".
			pg_input_->Append(new wxEnumProperty(name, wxPG_LABEL,
				wxArrayString(WXSIZEOF(enum_ORDER_labels), enum_ORDER_labels),
				wxArrayInt(enum_ORDER_values, enum_ORDER_values + WXSIZEOF(enum_ORDER_values)), ORDER_LIMIT));
		}
		else {
			pg_input_->Append(new wxStringProperty(name, wxPG_LABEL, value));
		}
	}
	/*pg_->Append(new wxStringProperty("String Property", wxPG_LABEL));
	pg_->Append(new wxIntProperty("Int Property", wxPG_LABEL));
	pg_->Append(new wxBoolProperty("Bool Property", wxPG_LABEL));*/

	/*pg_->Append(new wxPropertyCategory("目标", wxPG_LABEL));
	static const wxString enum_prop_labels[] = { "当前代码",
	"当前代码列表", "全市场所有代码" };
	static long enum_prop_values[] = { 0, 1, 2 };
	pg_->Append(new wxEnumProperty("Target", wxPG_LABEL,
	wxArrayString(WXSIZEOF(enum_prop_labels), enum_prop_labels),
	wxArrayInt(enum_prop_values, enum_prop_values + WXSIZEOF(enum_prop_values)), 0));*/

	//::SetMinSize(pg_input_);

	const wxString targets[] = {
		wxT("当前代码")
		, wxT("当前代码列表")
		, wxT("全市场所有代码")
	};
	radio_target_ = new wxRadioBox(this, wxID_ANY,
		wxT("目标"),
		wxDefaultPosition, wxDefaultSize,
		3, targets);
	int target = 0;
	if (pConfig->Read(wxString::Format(wxT("calc/%d/%s/target"), func.GetCalcType(), func.GetCalcName()), &target)) {
		//
	}
	radio_target_->SetSelection(target);

	btn_ok_ = new wxButton(this, wxID_OK, _("&OK"));
	btn_cancel_ = new wxButton(this, wxID_CANCEL, _("&Cancel"));

	wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

	// input pane
	wxStaticBox *boxInput = new wxStaticBox(this, wxID_ANY, wxT("输入"));
	wxSizer *sizerInput = new wxStaticBoxSizer(boxInput, wxVERTICAL);

	sizerInput->Add(pg_input_, wxALIGN_CENTER | wxALL, 5);
	//sizerInput->AddSpacer(2);

	sizerTop->Add(sizerInput,
		wxSizerFlags(0).Expand().Border((wxALL & ~wxTop), 5));

	sizer_target_ = new wxBoxSizer(wxHORIZONTAL);
	sizer_target_->Add(radio_target_, wxSizerFlags(1).Expand());
	sizerTop->Add(sizer_target_, wxSizerFlags(0).Expand().Border((wxALL & ~wxBOTTOM), 5));

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

MyCalcDlg::~MyCalcDlg()
{
}

void MyCalcDlg::OnPropertyGridChange(wxPropertyGridEvent &event)
{
	wxPGProperty* p = event.GetProperty();
	if (p)
	{
		auto name = p->GetName();
		auto value = p->GetValueAsString();
		//auto config = wxConfigBase::Get();
		//config->Write(wxString::Format("calc/%s/%s", ));
	}
}

//void MyCalcDlg::OnPropertyGridChanging(wxPropertyGridEvent &event)
//{
//	wxPGProperty* p = event.GetProperty();
//}

void MyCalcDlg::OnOK(wxCommandEvent& event)
{
	auto pConfig = wxConfigBase::Get();

	zqdb::Calc::Func func(h_);

	size_t input_count = func.GetInputAttrCount();
	for (size_t i = 0; i < input_count; i++)
	{
		wxPGProperty* p = pg_input_->GetProperty(func.GetInputAttrName(i)); 
		if (!p) {
			continue;
		}
		auto name = p->GetName();
		auto new_value = p->GetValueAsString();
		wxString value;
		if (pConfig->Read(wxString::Format(wxT("calc/%d/%s/input/%s"), func.GetCalcType(), func.GetCalcName(), func.GetInputAttrName(i)), &value)) {
			//
		}
		else {
			value = func.GetInputAttrValue(i);
		}
		if (new_value != value) {
			pConfig->Write(wxString::Format(wxT("calc/%d/%s/input/%s"), func.GetCalcType(), func.GetCalcName(), func.GetInputAttrName(i)), new_value);
		}
	}

	int new_target = radio_target_->GetSelection();
	int target = 0;
	if (pConfig->Read(wxString::Format(wxT("calc/%d/%s/target"), func.GetCalcType(), func.GetCalcName()), &target)) {
		//
	}
	if (new_target != target) {
		pConfig->Write(wxString::Format(wxT("calc/%d/%s/target"), func.GetCalcType(), func.GetCalcName()), new_target);
	}

	EndModal(wxID_OK);
}

void MyCalcDlg::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(wxID_CANCEL);
}
