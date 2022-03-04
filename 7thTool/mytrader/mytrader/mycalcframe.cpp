#include "myapp.h"
#include "mycalcframe.h"
#include "mylogdlg.h"


///

wxBEGIN_EVENT_TABLE(MyCalcOutputDlg, wxDialog)
	EVT_ACTIVATE(MyCalcOutputDlg::OnActivate)
wxEND_EVENT_TABLE()

MyCalcOutputDlg::MyCalcOutputDlg(wxPoint parentPos, int width) :wxDialog(nullptr, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(width, 150), 0)
{
	parentPos.y -= GetRect().height;
	SetPosition(parentPos);

	//ctrl_text_ = new wxRichTextCtrl(this, wxID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize
	//	, wxTE_READONLY | wxVSCROLL);
	ctrl_text_ = new wxTextCtrl(this, wxID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize
		, wxTE_READONLY | wxTE_MULTILINE | wxNO_BORDER);
}

MyCalcOutputDlg::~MyCalcOutputDlg()
{

}

static MyCalcOutputDlg* g_logdlg = nullptr;

void MyCalcOutputDlg::ShowDlg(wxPoint parentPos, int width)
{
	if (!g_logdlg) {
		g_logdlg = new MyCalcOutputDlg(parentPos, width);
		g_logdlg->Show();
		g_logdlg->Raise();
	}
}

void MyCalcOutputDlg::Clear()
{
	if (!g_logdlg) {
		return;
	}
	g_logdlg->DoClear();
}

void MyCalcOutputDlg::Append(const wxString& text)
{
	if (g_logdlg) {
		if (!wxIsMainThread()) {
			wxGetApp().Post([text] {
				if (g_logdlg) {
					g_logdlg->DoAppend(text);
				}
			});
		}
		else {
			g_logdlg->DoAppend(text);
		}
	}
}

void MyCalcOutputDlg::DoClear()
{
	ctrl_text_->Clear();
}

void MyCalcOutputDlg::DoAppend(const wxString& text)
{
	ctrl_text_->AppendText(text);
}

void MyCalcOutputDlg::OnActivate(wxActivateEvent& event)
{
	if (!event.GetActive()) {
		//这里会收到两次消息，所以要注意
		Show(false);
		wxGetApp().Post(200, [this] {
			if (g_logdlg) {
				//LOG4D("CLOSE LOG DIALOG %p %p", g_logdlg, this);
				g_logdlg = nullptr;
				if (Close(true)) {
					Destroy();
				}
			}
		});
	}
}

static const char *numlockIndicators[] = { "OFF", "NUM" };
static const char *insovrIndicators[] = { "插入", "覆盖" };
static const char *capslockIndicators[] = { "小写", "大写" };

wxBEGIN_EVENT_TABLE(MyCalcFrame, wxFrame)
EVT_MENU(wxID_EXIT, MyCalcFrame::OnExit)
// Menu items with standard IDs forwarded to the editor.
EVT_MENU(wxID_CLEAR, MyCalcFrame::OnEditClear)
EVT_MENU(wxID_CUT, MyCalcFrame::OnEditCut)
EVT_MENU(wxID_COPY, MyCalcFrame::OnEditCopy)
EVT_MENU(wxID_PASTE, MyCalcFrame::OnEditPaste)
//EVT_MENU(wxID_SELECTALL, MyCalcFrame::OnEdit)
EVT_MENU(wxID_REDO, MyCalcFrame::OnEditRedo)
EVT_MENU(wxID_UNDO, MyCalcFrame::OnEditUndo)
//EVT_MENU(wxID_FIND, MyCalcFrame::OnEdit)
//#if wxUSE_CLIPBOARD
EVT_UPDATE_UI(wxID_PASTE, MyCalcFrame::OnUpdateEditPaste)
//#endif // wxUSE_CLIPBOARD
EVT_UPDATE_UI(wxID_REDO, MyCalcFrame::OnUpdateEditRedo)
EVT_UPDATE_UI(wxID_UNDO, MyCalcFrame::OnUpdateEditUndo)
// And all our edit-related menu commands.
//EVT_MENU_RANGE(myID_EDIT_FIRST, myID_EDIT_LAST,
//	MyCalcFrame::OnEdit)
EVT_MENU(ID_CALC_EDIT_SAVE, MyCalcFrame::OnSave)
EVT_MENU(ID_CALC_EDIT_COMPILE, MyCalcFrame::OnCompile)
EVT_MENU(ID_CALC_EDIT_APPLY, MyCalcFrame::OnApply)
//
EVT_CLOSE(MyCalcFrame::OnCloseEvent)
EVT_IDLE(MyCalcFrame::OnIdle)
wxEND_EVENT_TABLE()

MyCalcFrame::MyCalcFrame(const char* xml, size_t xmlflag)
	: wxFrame((wxFrame *)NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(800, 600))
	, all_funcs_{ zqdb::Calc::AllFunc((CALC_TYPE)0)
	, zqdb::Calc::AllFunc((CALC_TYPE)1)
	, zqdb::Calc::AllFunc((CALC_TYPE)2)
	, zqdb::Calc::AllFunc((CALC_TYPE)3)
	, zqdb::Calc::AllFunc((CALC_TYPE)4)
	, zqdb::Calc::AllFunc((CALC_TYPE)5)
	, zqdb::Calc::AllFunc((CALC_TYPE)6)
}
{
	wxClientDC dc(this);

	CFG_FROM_XML(cfg, xml, xmlflag);

#if wxUSE_STATUSBAR
	status_bar_ = CreateStatusBar();
	//wxSize sizeNumLock = dc.GetTextExtent(numlockIndicators[0]);
	//sizeNumLock.IncTo(dc.GetTextExtent(numlockIndicators[1]));

	int widths[Field_Max];
	widths[Field_Text] = -1; // growable
	widths[Field_Info] = 150;
	widths[Field_Size] = 50;
	widths[Field_Line_Endings] = 100;
	widths[Field_Lang] = 50;
	widths[Field_InsOvrIndicator] = dc.GetTextExtent(insovrIndicators[1]).x;
	widths[Field_CapsLockIndicator] = dc.GetTextExtent(capslockIndicators[1]).x;

	status_bar_->SetFieldsCount(Field_Max);
	status_bar_->SetStatusWidths(Field_Max, widths);

	SetStatusText(insovrIndicators[0], Field_InsOvrIndicator);
	//SetStatusText("no selection", 0);
	status_bar_->Bind(wxEVT_LEFT_UP, &MyCalcFrame::OnStatusBarMouseEvent, this);
#endif // wxUSE_STATUSBAR

	// create menu
	auto m_menuBar = new wxMenuBar;
	// File menu
	wxMenu *menuFile = new wxMenu;
	/*for (int i = 0, id = ID_CALC_EDIT_OPEN; i < CALC_TYPE_MAX; i++)
	{
	auto sub_menu = new wxMenu;
	sub_menu->Append(ID_CALC_EDIT_NEW + CALC_MAJOR, _("New"));
	sub_menu->AppendSeparator();
	for (size_t j = 0; j < all_funcs_[i].size(); j++, id++)
	{
	zqdb::Calc::Func func(all_funcs_[i][j]);
	if (func.GetCalcLang() != CALC_LANG_C_CPP) {
	sub_menu->Append(id, func.GetCalcName());
	}
	}
	menuFile->AppendSubMenu(sub_menu, _(ZQDBCalcType2Str((CALC_TYPE)i)));
	}
	menuFile->AppendSeparator();*/
	//menuFile->Append(ID_CALC_EDIT_SAVE, _("&Save\tCtrl+S"));
	menuFile->Append(ID_CALC_EDIT_COMPILE, _("&Compile\tAlt+C"));
	menuFile->Append(ID_CALC_EDIT_APPLY, _("&Save\tCtrl+S"));
	/*menuFile->AppendSeparator();
	menuFile->Append(ID_CALC_EDIT_EXPORT, _("&Export ..\tCtrl+E"));
	menuFile->Append(ID_CALC_EDIT_IMPORT, _("&Import ..\tCtrl+I"));
	menuFile->AppendSeparator();
	menuFile->Append(wxID_PRINT_SETUP, _("Print Set&up .."));
	menuFile->Append(wxID_PREVIEW, _("Print Pre&view\tCtrl+Shift+P"));
	menuFile->Append(wxID_PRINT, _("&Print ..\tCtrl+P"));*/
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT, _("&Quit\tCtrl+Q"));

	// Edit menu
	wxMenu *menuEdit = new wxMenu;
	menuEdit->Append(wxID_UNDO, _("&Undo\tCtrl+Z"));
	menuEdit->Append(wxID_REDO, _("&Redo\tCtrl+Shift+Z"));
	menuEdit->AppendSeparator();
	menuEdit->Append(wxID_CUT, _("Cu&t\tCtrl+X"));
	menuEdit->Append(wxID_COPY, _("&Copy\tCtrl+C"));
	menuEdit->Append(wxID_PASTE, _("&Paste\tCtrl+V"));
	menuEdit->Append(wxID_CLEAR, _("&Delete\tDel"));
	//		menuEdit->AppendSeparator();
	//		menuEdit->Append(wxID_FIND, _("&Find\tCtrl+F"));
	//		menuEdit->Enable(wxID_FIND, false);
	//		menuEdit->Append(myID_FINDNEXT, _("Find &next\tF3"));
	//		menuEdit->Enable(myID_FINDNEXT, false);
	//		menuEdit->Append(myID_REPLACE, _("&Replace\tCtrl+H"));
	//		menuEdit->Enable(myID_REPLACE, false);
	//		menuEdit->Append(myID_REPLACENEXT, _("Replace &again\tShift+F4"));
	//		menuEdit->Enable(myID_REPLACENEXT, false);
	//		menuEdit->AppendSeparator();
	//		menuEdit->Append(myID_BRACEMATCH, _("&Match brace\tCtrl+M"));
	//		menuEdit->Append(myID_GOTO, _("&Goto\tCtrl+G"));
	//		menuEdit->Enable(myID_GOTO, false);
	//		menuEdit->AppendSeparator();
	//		menuEdit->Append(myID_INDENTINC, _("&Indent increase\tTab"));
	//		menuEdit->Append(myID_INDENTRED, _("I&ndent reduce\tShift+Tab"));
	//		menuEdit->AppendSeparator();
	//		menuEdit->Append(wxID_SELECTALL, _("&Select all\tCtrl+A"));
	//		menuEdit->Append(myID_SELECTLINE, _("Select &line\tCtrl+L"));
	//
	//		// highlight submenu
	//		wxMenu *menuHighlight = new wxMenu;
	//		int Nr;
	//		for (Nr = 0; Nr < g_LanguagePrefsSize; Nr++) {
	//		menuHighlight->Append(myID_HIGHLIGHTFIRST + Nr,
	//		g_LanguagePrefs[Nr].name);
	//		}
	//
	//		// charset submenu
	//		wxMenu *menuCharset = new wxMenu;
	//		menuCharset->Append(myID_CHARSETANSI, _("&ANSI (Windows)"));
	//		menuCharset->Append(myID_CHARSETMAC, _("&MAC (Macintosh)"));
	//
	//		// View menu
	//		wxMenu *menuView = new wxMenu;
	//		//menuView->Append(myID_HIGHLIGHTLANG, _("&Highlight language .."), menuHighlight);
	//		//menuView->AppendSeparator();
	//		menuView->AppendCheckItem(myID_FOLDTOGGLE, _("&Toggle current fold\tCtrl+T"));
	//		menuView->AppendCheckItem(myID_OVERTYPE, _("&Overwrite mode\tIns"));
	//		menuView->AppendCheckItem(myID_WRAPMODEON, _("&Wrap mode\tCtrl+U"));
	//		menuView->AppendSeparator();
	//		menuView->AppendCheckItem(myID_DISPLAYEOL, _("Show line &endings"));
	//		menuView->AppendCheckItem(myID_INDENTGUIDE, _("Show &indent guides"));
	//		menuView->AppendCheckItem(myID_LINENUMBER, _("Show line &numbers"));
	//		menuView->AppendCheckItem(myID_LONGLINEON, _("Show &long line marker"));
	//		menuView->AppendCheckItem(myID_WHITESPACE, _("Show white&space"));
	//		menuView->AppendSeparator();
	//		menuView->Append(myID_USECHARSET, _("Use &code page of .."), menuCharset);
	//
	//		// Annotations menu
	//		wxMenu* menuAnnotations = new wxMenu;
	//		menuAnnotations->Append(myID_ANNOTATION_ADD, _("&Add or edit an annotation..."),
	//			_("Add an annotation for the current line"));
	//		menuAnnotations->Append(myID_ANNOTATION_REMOVE, _("&Remove annotation"),
	//			_("Remove the annotation for the current line"));
	//		menuAnnotations->Append(myID_ANNOTATION_CLEAR, _("&Clear all annotations"));
	//
	//		wxMenu* menuAnnotationsStyle = new wxMenu;
	//		menuAnnotationsStyle->AppendRadioItem(myID_ANNOTATION_STYLE_HIDDEN, _("&Hidden"));
	//		menuAnnotationsStyle->AppendRadioItem(myID_ANNOTATION_STYLE_STANDARD, _("&Standard"));
	//		menuAnnotationsStyle->AppendRadioItem(myID_ANNOTATION_STYLE_BOXED, _("&Boxed"));
	//		menuAnnotations->AppendSubMenu(menuAnnotationsStyle, "&Style");
	//
	//		// change case submenu
	//		wxMenu *menuChangeCase = new wxMenu;
	//		menuChangeCase->Append(myID_CHANGEUPPER, _("&Upper case"));
	//		menuChangeCase->Append(myID_CHANGELOWER, _("&Lower case"));
	//
	//		// convert EOL submenu
	//		wxMenu *menuConvertEOL = new wxMenu;
	//		menuConvertEOL->Append(myID_CONVERTCR, _("CR (&Linux)"));
	//		menuConvertEOL->Append(myID_CONVERTCRLF, _("CR+LF (&Windows)"));
	//		menuConvertEOL->Append(myID_CONVERTLF, _("LF (&Macintosh)"));
	//
	//		// Extra menu
	//		wxMenu *menuExtra = new wxMenu;
	//		menuExtra->AppendCheckItem(myID_READONLY, _("&Readonly mode"));
	//		menuExtra->AppendSeparator();
	//		menuExtra->Append(myID_CHANGECASE, _("Change &case to .."), menuChangeCase);
	//		menuExtra->AppendSeparator();
	//		menuExtra->Append(myID_CONVERTEOL, _("Convert line &endings to .."), menuConvertEOL);
	//		menuExtra->AppendCheckItem(myID_MULTIPLE_SELECTIONS, _("Toggle &multiple selections"));
	//		menuExtra->AppendCheckItem(myID_MULTI_PASTE, _("Toggle multi-&paste"));
	//		menuExtra->AppendCheckItem(myID_MULTIPLE_SELECTIONS_TYPING, _("Toggle t&yping on multiple selections"));
	//		menuExtra->AppendSeparator();
	//#if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D
	//		wxMenu* menuTechnology = new wxMenu;
	//		menuTechnology->AppendRadioItem(myID_TECHNOLOGY_DEFAULT, _("&Default"));
	//		menuTechnology->AppendRadioItem(myID_TECHNOLOGY_DIRECTWRITE, _("Direct&Write"));
	//		menuExtra->AppendSubMenu(menuTechnology, _("&Technology"));
	//		menuExtra->AppendSeparator();
	//#endif
	//		menuExtra->AppendCheckItem(myID_CUSTOM_POPUP, _("C&ustom context menu"));

	// construct menu
	m_menuBar->Append(menuFile, _("&File"));
	m_menuBar->Append(menuEdit, _("&Edit"));
	//m_menuBar->Append(menuView, _("&View"));
	//m_menuBar->Append(menuAnnotations, _("&Annotations"));
	//m_menuBar->Append(menuExtra, _("E&xtra"));
	SetMenuBar(m_menuBar);

	//m_menuBar->Check(myID_ANNOTATION_STYLE_BOXED, true);

	// open first page
	ctrl_edit_ = new zqdb::TechEdit(this, wxID_ANY);
	ctrl_edit_->SetFocus();
}

MyCalcFrame::~MyCalcFrame()
{
	wxGetApp().ResetCalcFrame(this);
}

int MyCalcFrame::FilterEvent(wxEvent& event)
{
	// Continue processing the event normally as well.
	return wxEventFilter::Event_Skip;
}

void MyCalcFrame::OnSkinInfoChanged()
{
	Freeze();
	wxIcon icon;
	icon.CopyFromBitmap(skin_info_ptr_->GetBitmap32(wxT("策略")));
	SetIcon(icon);
	Layout();
	Thaw();
}

void MyCalcFrame::OnFuncChanged()
{
	Freeze();
	//ctrl_edit_->SetReadOnly(false);
	ctrl_edit_->EmptyUndoBuffer();
	ctrl_edit_->SetText(GetCalcSource());
	ctrl_edit_->SelectNone();
	if (!IsNew()) {
		ctrl_edit_->SetSavePoint();
	}
	Layout();
	Thaw();
}

// edit events
void MyCalcFrame::OnEditRedo(wxCommandEvent &WXUNUSED(event)) {
	if (!ctrl_edit_->CanRedo()) return;
	ctrl_edit_->Redo();
}

void MyCalcFrame::OnEditUndo(wxCommandEvent &WXUNUSED(event)) {
	if (!ctrl_edit_->CanUndo()) return;
	ctrl_edit_->Undo();
}

void MyCalcFrame::OnEditClear(wxCommandEvent &WXUNUSED(event)) {
	if (ctrl_edit_->GetReadOnly()) return;
	ctrl_edit_->Clear();
}

void MyCalcFrame::OnEditCut(wxCommandEvent &WXUNUSED(event)) {
	if (ctrl_edit_->GetReadOnly() || (ctrl_edit_->GetSelectionEnd() - ctrl_edit_->GetSelectionStart() <= 0)) return;
	ctrl_edit_->Cut();
}

void MyCalcFrame::OnEditCopy(wxCommandEvent &WXUNUSED(event)) {
	if (ctrl_edit_->GetSelectionEnd() - ctrl_edit_->GetSelectionStart() <= 0) return;
	ctrl_edit_->Copy();
}

void MyCalcFrame::OnEditPaste(wxCommandEvent &WXUNUSED(event)) {
	if (!ctrl_edit_->CanPaste()) return;
	ctrl_edit_->Paste();
}

//#if wxUSE_CLIPBOARD

void MyCalcFrame::OnUpdateEditPaste(wxUpdateUIEvent& event)
{
	//wxClipboardLocker lock;
	//event.Enable(wxTheClipboard->IsSupported(wxDF_TEXT));
	event.Enable(ctrl_edit_->CanPaste());
}

//#endif // wxUSE_CLIPBOARD

void MyCalcFrame::OnUpdateEditRedo(wxUpdateUIEvent& event)
{
	event.Enable(ctrl_edit_->CanRedo());
}

void MyCalcFrame::OnUpdateEditUndo(wxUpdateUIEvent& event)
{
	event.Enable(ctrl_edit_->CanUndo());
}

void MyCalcFrame::DoSave()
{
	if (!ctrl_edit_->Modified()) {
		return;
	}
	auto hmodule = GetCalcModule();
	auto type = GetCalcType();
	auto name = wxString2utf(GetCalcName());
	wxString text = ctrl_edit_->GetText();
	auto source = wxString2utf(text);
	ZQDBCalcModuleSaveFunc(hmodule, type, name.c_str(), source.c_str());
	ctrl_edit_->SetModified(false);
	SetStatusText(_("save success"));
}

void MyCalcFrame::SaveIfModified()
{
	if (!ctrl_edit_->Modified()) {
		return;
	}
	if (wxOK != wxMessageBox(wxT("是否保存修改？"), wxT("提示"),
		wxOK | wxCANCEL)) {
		return;
	}
	DoSave();
}

void MyCalcFrame::OnSave(wxCommandEvent &event)
{
	DoSave();
}

void MyCalcFrame::ShowOuptput()
{
	MyCalcOutputDlg::ShowDlg(status_bar_->GetScreenPosition(), GetClientSize().x);
	if (!output_text_.IsEmpty()) {
		MyCalcOutputDlg::Append(output_text_);
	}
}

void MyCalcFrame::ClearOutput()
{
	output_text_.Clear();
	MyCalcOutputDlg::Clear();
	SetStatusText(wxEmptyString);
}

void MyCalcFrame::AppendOutput(const wxString& text, bool status)
{
	output_text_.Append(text);
	MyCalcOutputDlg::Append(text);
	if (status) {
		SetStatusText(text);
	}
}

bool MyCalcFrame::DoCompile(bool tips)
{
	ClearOutput();
	ShowOuptput();
	auto hmodule = GetCalcModule();
	auto type = GetCalcType();
	auto name = wxString2utf(GetCalcName());
	wxString text = ctrl_edit_->GetText();
	auto source = wxString2utf(text);
	int err = ZQDBCalcModuleCompileFunc(hmodule, type, name.c_str(), source.c_str(), [](const char* err, void* data) {
		MyCalcFrame* pT = (MyCalcFrame*)data;
		pT->AppendOutput(err);
	}, this);
	if (!err) {
		if(tips)
			AppendOutput(_("compile success"), true);
		return true;
	}
	else {
		if (tips)
			AppendOutput(_("compile error"), true);
		return false;
	}
}

void MyCalcFrame::OnCompile(wxCommandEvent &event)
{
	DoCompile();
}

void MyCalcFrame::OnApply(wxCommandEvent &event)
{
	if (!DoCompile(false)) {
		AppendOutput(wxT("编译失败，无法保存!"), true);
		//wxMessageBox(wxT("编译失败，无法保存!"), wxT("提示"), wxOK, this);
		return;
	}
	DoSave();
	auto hmodule = GetCalcModule();
	auto type = GetCalcType();
	auto name = wxString2utf(GetCalcName());
	ZQDBCalcModuleApplyFunc(hmodule, type, name.c_str());
	SetStatusText(_("apply success"));
}

void MyCalcFrame::OnExit(wxCommandEvent &WXUNUSED(event)) 
{
	Close();
}

void MyCalcFrame::OnCloseEvent(wxCloseEvent& event)
{
	SaveIfModified();

	event.Skip();
}

void MyCalcFrame::OnIdle(wxIdleEvent& event)
{
	if (ctrl_edit_->Modified()) {
		SetTitle(wxT("* ") + GetCalcName() + wxT(" - ") + _(ZQDBCalcType2Str(GetCalcType())));
	}
	else {
		SetTitle(GetCalcName() + wxT(" - ") + _(ZQDBCalcType2Str(GetCalcType())));
	}

	SetStatusText(wxString::Format(wxT("行 %d : %d 位置 %d")
		, ctrl_edit_->GetCurrentLine(), ctrl_edit_->GetLineCount(), ctrl_edit_->GetCurrentPos()),
		Field_Info);
	SetStatusText(wxString::Format(wxT("%d"), ctrl_edit_->GetTextLength()), Field_Size);
	wxString EOLtype;
	switch (ctrl_edit_->GetEOLMode()) {
	case wxSTC_EOL_CR: {EOLtype = "CR (Unix)"; break; }
	case wxSTC_EOL_CRLF: {EOLtype = "CRLF (Windows)"; break; }
	case wxSTC_EOL_LF: {EOLtype = "CR (Macintosh)"; break; }
	}
	SetStatusText(EOLtype, Field_Line_Endings);
	SetStatusText(wxT("Python"), Field_Lang);
	SetStatusText(insovrIndicators[!ctrl_edit_->Ins()],
		Field_InsOvrIndicator);
	bool caps = (wxGetKeyState(WXK_CAPITAL) && !wxGetKeyState(WXK_SHIFT)) || (!wxGetKeyState(WXK_CAPITAL) && wxGetKeyState(WXK_SHIFT));
	SetStatusText(capslockIndicators[caps],
		Field_CapsLockIndicator);

	event.Skip();
}

void MyCalcFrame::OnStatusBarMouseEvent(wxMouseEvent& event)
{
	wxRect rect;
	status_bar_->GetFieldRect(0, rect);
	if (rect.Contains(event.GetPosition()))
	{
		ShowOuptput();
	}
}

