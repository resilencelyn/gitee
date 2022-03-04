#pragma once

#include "myapp.h"
#include "myprops.h"
#include "wx/popupwin.h"

// -- MyScriptPropertyDlg --

class MyScriptPropertyDlg : public wxPopupWindow
{
	wxScrolledWindow *m_panel;
	wxPropertyGrid *m_grid;
	wxBoxSizer *m_sizer;
public:
	MyScriptPropertyDlg(wxWindow *parent);

	//void OnCollapse(wxPropertyGridEvent& event);
	//void OnExpand(wxPropertyGridEvent& event);

	void Fit() wxOVERRIDE;

	wxDECLARE_ABSTRACT_CLASS(MyScriptPropertyDlg);
	wxDECLARE_EVENT_TABLE();
};

class MyCalcDlg : public wxDialog
{
	typedef wxDialog Base;
private:
	HZQDB h_;
	wxPropertyGrid* pg_input_ = nullptr;
	wxRadioBox *radio_target_ = nullptr;
	wxSizer *sizer_target_ = nullptr;
	wxButton* btn_ok_ = nullptr;
	wxButton* btn_cancel_ = nullptr;
public:
	static HZQDB GetInputAttr(HZQDB h);
	static size_t GetTarget(HZQDB h);

	MyCalcDlg(wxWindow *parent, HZQDB h);
	virtual ~MyCalcDlg();

protected:
	//
	void OnPropertyGridChange(wxPropertyGridEvent& event);
	//void OnPropertyGridChanging(wxPropertyGridEvent& event);
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};


