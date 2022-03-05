#pragma once

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/textctrl.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"
#include "wx/sizer.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/dcbuffer.h"
#include "wx/colordlg.h"
#include "wx/artprov.h"
#include "wx/combobox.h"
#include "wx/tglbtn.h"
#include "wx/wrapsizer.h"

#include <view.h>
#include <XUtil/XThread.hpp>
#include "settings.h"
#include "mydatamodel.h"
#include "mydataview.h"
#include "mymodulemgr.h"

class MyBaseFrame;

// -- frame --

class MyBaseFrame : public wxFrame
	, public zqdb::SkinMap<MyBaseFrame, SkinInfo>
{
	typedef wxFrame Base;
	typedef zqdb::SkinMap<MyBaseFrame, SkinInfo> SkinBase;
protected:
	//RibbonBar ID∂®“Â
	enum
	{
		ID_PAGE_BEGIN = wxID_HIGHEST + 1,
		ID_PAGE_START,
		ID_PAGE_CALC,
		ID_PAGE_NEW,
		ID_PAGE_NEW_PAGE,
		ID_PAGE_NEW_PAGE_MAX = ID_PAGE_NEW_PAGE + 16,
		ID_PAGE_HELP,
		ID_PAGE_END
	};
	wxRibbonBar* m_ribbon; 
	std::map<MyBaseFrame*, wxRibbonPage*> frames_;
	//size_t AddExchangePage(HZQDB h, size_t product_id);
	std::map<wxRibbonPage*, std::function<void(wxRibbonBarEvent&)>> last_pages_;
	virtual void AddFirstPages() { }
	virtual void AddLastPages();
	void RemoveLastPages();
	//wxTextCtrl* m_logwindow;
	//wxToggleButton* m_togglePanels;
#if wxUSE_INFOBAR
	wxInfoBar* infobar_ = nullptr;
#endif//
public:
    MyBaseFrame(const char* xml, size_t xmlflag);
    ~MyBaseFrame();

	void AddFrame(MyBaseFrame* frame);
	void RemoveFrame(MyBaseFrame* frame);
	void ActivateFrame(MyBaseFrame* frame);
	void UpdateAllPages();

	// show the info bar with the given message and optionally an icon
	void ShowMessage(const wxString& msg,
		int flags = wxICON_INFORMATION);

	virtual int FilterEvent(wxEvent& event);

	virtual void OnSkinInfoChanged();

protected:
	//
	void OnPageChanging(wxRibbonBarEvent& evt); 
	void OnHelpClicked(wxRibbonBarEvent& evt);
	
    void OnSizeEvent(wxSizeEvent& evt);
	void OnCloseEvent(wxCloseEvent& evt);

	void OnTimer(wxTimerEvent& evt);
    wxDECLARE_EVENT_TABLE();
};

