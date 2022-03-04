#pragma once

#include "mybaseframe.h"
#include <techedit.h>

class MyCalcFrame;

class MyCalcOutputDlg : public wxDialog
{
private:
	//wxRichTextCtrl* ctrl_text_ = nullptr;
	wxTextCtrl* ctrl_text_ = nullptr;
	MyCalcOutputDlg(wxPoint parentPos, int width);
	void DoClear();
	void DoAppend(const wxString& text);

public:
	static void ShowDlg(wxPoint parentPos, int width);
	static void Clear();
	static void Append(const wxString& text);

	~MyCalcOutputDlg();

	void OnActivate(wxActivateEvent& event);

	wxDECLARE_EVENT_TABLE();
};

class MyCalcFrame : public wxFrame
	, public zqdb::SkinMap<MyCalcFrame, SkinInfo>
	, public zqdb::FuncContainerMap<MyCalcFrame, zqdb::FuncContainerInfo>
	, public zqdb::NotifyMap<MyCalcFrame>
{
	typedef wxFrame Base;
	typedef zqdb::SkinMap<MyBaseFrame, SkinInfo> SkinBase;
	typedef zqdb::FuncContainerMap<MyCalcFrame, zqdb::FuncContainerInfo> ContainerMap;
	typedef zqdb::NotifyMap<MyCalcFrame> NotifyBase;
protected:
	zqdb::Calc::AllFunc all_funcs_[CALC_TYPE_MAX];
	//void AddFirstPages();
	zqdb::TechEdit *ctrl_edit_;
	wxStatusBar* status_bar_;
	wxString output_text_;
public:
	MyCalcFrame(const char* xml, size_t xmlflag);
	~MyCalcFrame();

	int FilterEvent(wxEvent& event);

	void OnSkinInfoChanged();
	//void OnInfoChanged();
	void OnFuncChanged();

	void ShowOuptput();
	void ClearOutput();
	void AppendOutput(const wxString& text, bool status = false);

protected:
	//
	enum {
		// menu IDs
		myID_PROPERTIES = wxID_HIGHEST,
		myID_EDIT_FIRST,
		myID_INDENTINC = myID_EDIT_FIRST,
		myID_INDENTRED,
		myID_FINDNEXT,
		myID_REPLACE,
		myID_REPLACENEXT,
		myID_BRACEMATCH,
		myID_GOTO,
		myID_DISPLAYEOL,
		myID_INDENTGUIDE,
		myID_LINENUMBER,
		myID_LONGLINEON,
		myID_WHITESPACE,
		myID_FOLDTOGGLE,
		myID_OVERTYPE,
		myID_READONLY,
		myID_WRAPMODEON,
		myID_ANNOTATION_ADD,
		myID_ANNOTATION_REMOVE,
		myID_ANNOTATION_CLEAR,
		myID_ANNOTATION_STYLE_HIDDEN,
		myID_ANNOTATION_STYLE_STANDARD,
		myID_ANNOTATION_STYLE_BOXED,
		myID_CHANGECASE,
		myID_CHANGELOWER,
		myID_CHANGEUPPER,
		myID_HIGHLIGHTLANG,
		myID_HIGHLIGHTFIRST,
		myID_HIGHLIGHTLAST = myID_HIGHLIGHTFIRST + 99,
		myID_CONVERTEOL,
		myID_CONVERTCR,
		myID_CONVERTCRLF,
		myID_CONVERTLF,
		myID_MULTIPLE_SELECTIONS,
		myID_MULTI_PASTE,
		myID_MULTIPLE_SELECTIONS_TYPING,
		myID_TECHNOLOGY_DEFAULT,
		myID_TECHNOLOGY_DIRECTWRITE,
		myID_CUSTOM_POPUP,
		myID_USECHARSET,
		myID_CHARSETANSI,
		myID_CHARSETMAC,
		myID_SELECTLINE,
		myID_EDIT_LAST = myID_SELECTLINE,
		myID_WINDOW_MINIMAL,

		// other IDs
		myID_ABOUTTIMER,

		ID_CALC_EDIT_NEW,
		ID_CALC_EDIT_NEW_MAX = ID_CALC_EDIT_NEW + CALC_TYPE_MAX,
		ID_CALC_EDIT_OPEN,
		ID_CALC_EDIT_OPEN_MAX = ID_CALC_EDIT_OPEN + 1000,
		ID_CALC_EDIT_SAVE,
		ID_CALC_EDIT_COMPILE,
		ID_CALC_EDIT_APPLY,
		ID_CALC_EDIT_EXPORT,
		ID_CALC_EDIT_IMPORT,
	};

	//Status
	enum
	{
		Field_Text,
		Field_Info,
		Field_Size,
		Field_Line_Endings,
		Field_Lang,
		Field_InsOvrIndicator,
		Field_CapsLockIndicator,
		Field_Max
	};

	void Open(HZQDB h);

	// edit
	void OnEditRedo(wxCommandEvent &event);
	void OnEditUndo(wxCommandEvent &event);
	void OnEditClear(wxCommandEvent &event);
	void OnEditCut(wxCommandEvent &event);
	void OnEditCopy(wxCommandEvent &event);
	void OnEditPaste(wxCommandEvent &event);
	//#if wxUSE_CLIPBOARD
	void OnUpdateEditPaste(wxUpdateUIEvent& event);
	//#endif // wxUSE_CLIPBOARD
	void OnUpdateEditRedo(wxUpdateUIEvent& event);
	void OnUpdateEditUndo(wxUpdateUIEvent& event);

	void DoSave();
	void SaveIfModified();
	void OnSave(wxCommandEvent &event);
	bool DoCompile(bool tips = true);
	void OnCompile(wxCommandEvent &event);
	void OnApply(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);

	void OnCloseEvent(wxCloseEvent& event);
	void OnIdle(wxIdleEvent& event);

	void OnStatusBarMouseEvent(wxMouseEvent& event);

	wxDECLARE_EVENT_TABLE();
};

