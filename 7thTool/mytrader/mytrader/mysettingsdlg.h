#pragma once

#include "utility.h"

#include "wx/richtext/richtextctrl.h"
//#include "wx/richtext/richtextstyles.h"
//#include "wx/richtext/richtextxml.h"
//#include "wx/richtext/richtexthtml.h"
//#include "wx/richtext/richtextformatdlg.h"
//#include "wx/richtext/richtextsymboldlg.h"
//#include "wx/richtext/richtextstyledlg.h"
//#include "wx/richtext/richtextprint.h"
//#include "wx/richtext/richtextimagedlg.h"

class MySettingsDlg : public wxDialog
{
	typedef wxDialog Base;
private:
	wxRichTextCtrl* ctrl_text_ = nullptr;
	//wxTextCtrl* ctrl_text_ = nullptr;
	wxButton* btn_ok_ = nullptr;
	wxButton* btn_cancel_ = nullptr;
public:
	static std::string GetFile(bool test = false);
	static bool HealthCheck(std::string& settings_file);
	static bool HealthCheck(const boost::property_tree::ptree& cfg);
	MySettingsDlg(boost::property_tree::ptree& cfg);
	virtual ~MySettingsDlg();
protected:
	boost::property_tree::ptree& cfg_;
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

class PrefsPageGeneralPanel : public wxPanel
{
public:
	PrefsPageGeneralPanel(wxWindow *parent);

	virtual bool TransferDataToWindow() wxOVERRIDE;
	virtual bool TransferDataFromWindow() wxOVERRIDE;

private:
	void UpdateSettingsIfNecessary();
	void ChangedUseMarkdown(wxCommandEvent& e);
	void ChangedSpellcheck(wxCommandEvent& e);

	wxCheckBox *m_useMarkdown;
	wxCheckBox *m_spellcheck;

	// Settings corresponding to the current values in this dialog.
	//MySettings m_settingsCurrent;
};

class PrefsPageGeneral : public wxStockPreferencesPage
{
public:
	PrefsPageGeneral() : wxStockPreferencesPage(Kind_General) {}
	virtual wxWindow *CreateWindow(wxWindow *parent) wxOVERRIDE
	{
		return new PrefsPageGeneralPanel(parent);
	}
};


class PrefsPageAboutPanel : public wxPanel
{
public:
	PrefsPageAboutPanel(wxWindow *parent);
};

class PrefsPageAbout : public wxPreferencesPage
{
public:
	virtual wxString GetName() const wxOVERRIDE { return _("About"); }
	virtual wxBitmap GetLargeIcon() const wxOVERRIDE
	{
		return wxArtProvider::GetBitmap(wxART_HELP, wxART_TOOLBAR);
	}
	virtual wxWindow *CreateWindow(wxWindow *parent) wxOVERRIDE
	{
		return new PrefsPageAboutPanel(parent);
	}
};

class MyTechDlg : public wxDialog
{
	typedef wxDialog Base;
protected:
	wxSpinCtrl *ctrl_cycle_anysec_;
	wxSpinCtrl *ctrl_cycle_anymin_;
public:
	MyTechDlg(wxWindow *parent);

protected:
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

class MyAboutDlg : public wxDialog
{
	typedef wxDialog Base;
public:
	MyAboutDlg(wxWindow *parent);
protected:
	//
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

class MyCheckDlg : public wxDialog
{
	typedef wxDialog Base;
private:
	wxStaticText* ctrl_text_ = nullptr;
	wxCheckBox* ctrl_check_ = nullptr;
	wxButton* btn_ok_ = nullptr;
	wxButton* btn_cancel_ = nullptr;
public:
	MyCheckDlg(wxWindow* parent, const wxString& text, const wxString& check_text, bool check_value, const wxString& title, long style = wxOK|wxCANCEL);

	bool IsCheck();
protected:
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

class MyTestDlg : public wxDialog
{
	typedef wxDialog Base;
protected:
	enum {
		CTRL_ID_BEGIN = wxID_HIGHEST + 1,
		CTRL_ID_END,
		CTRL_ID_SPEED,
	};
	wxChoice *ctrl_begin_ = nullptr;
	wxChoice *ctrl_end_ = nullptr;
	wxSlider *ctrl_speed_ = nullptr;
	wxButton* btn_ok_ = nullptr;
	wxButton* btn_cancel_ = nullptr;
public:
	MyTestDlg(wxWindow *parent);

	size_t GetBegin();
	size_t GetEnd();
	size_t GetSpeed();

protected:
	void OnBeginChoice(wxCommandEvent& event);
	void OnEndChoice(wxCommandEvent& event);

	void OnSpeed(wxScrollEvent& event);

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};
