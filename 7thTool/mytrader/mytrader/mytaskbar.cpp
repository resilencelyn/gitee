/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.cpp
// Purpose:     wxTaskBarIcon demo
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "mytaskbar.h"
#include "myapp.h"
#include "myframe.h"

// ============================================================================
// implementation
// ============================================================================
//
//MyDialog::~MyDialog()
//{
//    delete m_taskBarIcon;
//}
//
//void MyDialog::OnAbout(wxCommandEvent& WXUNUSED(event))
//{
//    static const char * const title = "About wxWidgets Taskbar Sample";
//    static const char * const message
//        = "wxWidgets sample showing wxTaskBarIcon class\n"
//          "\n"
//          "(C) 1997 Julian Smart\n"
//          "(C) 2007 Vadim Zeitlin";
//
//#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
//    m_taskBarIcon->ShowBalloon(title, message, 15000, wxICON_INFORMATION);
//#else // !__WXMSW__
//    wxMessageBox(message, title, wxICON_INFORMATION|wxOK, this);
//#endif // __WXMSW__/!__WXMSW__
//}
//
//void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
//{
//    Show(false);
//}
//
//void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
//{
//    Close(true);
//}
//
//void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
//{
//    Destroy();
//}


// ----------------------------------------------------------------------------
// MyTaskBarIcon implementation
// ----------------------------------------------------------------------------

enum
{
    PU_INFO = 10001,
	PU_ABOUT, 
	PU_RESTORE,
	PU_PREFERENCE,
	PU_CLEARDATA,
	PU_RESETTINGS,
	PU_REPORTCONTEXT,
	PU_RESTART,
    PU_EXIT,
    PU_CHECKMARK,
    PU_SUB1,
    PU_SUB2,
    PU_SUBMAIN
};


wxBEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
	EVT_MENU(PU_ABOUT, MyTaskBarIcon::OnAbout)
	EVT_MENU(PU_PREFERENCE, MyTaskBarIcon::OnPreference)
	EVT_MENU(PU_CLEARDATA, MyTaskBarIcon::OnClearData)
	EVT_MENU(PU_RESETTINGS, MyTaskBarIcon::OnResettings)
	EVT_MENU(PU_REPORTCONTEXT, MyTaskBarIcon::OnReportContext)
	EVT_MENU(PU_RESTART, MyTaskBarIcon::OnRestart)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
    //EVT_MENU(PU_NEW_ICON,MyTaskBarIcon::OnMenuSetNewIcon)
    EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
    EVT_MENU(PU_SUB1, MyTaskBarIcon::OnMenuSub)
    EVT_MENU(PU_SUB2, MyTaskBarIcon::OnMenuSub)
wxEND_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
	wxGetApp().ShowFrame();
}

void MyTaskBarIcon::OnAbout(wxCommandEvent&)
{
	wxGetApp().ShowAbout();
}

void MyTaskBarIcon::OnPreference(wxCommandEvent&)
{
	wxGetApp().ShowPreferencesEditor(nullptr);
}

void MyTaskBarIcon::OnClearData(wxCommandEvent&)
{
	wxGetApp().ClearData();
}

void MyTaskBarIcon::OnResettings(wxCommandEvent&)
{
	wxGetApp().Resettings();
}

void MyTaskBarIcon::OnReportContext(wxCommandEvent&)
{
	// example of manually generated report, this could be also
	// used in wxApp::OnAssert()
	wxGetApp().GenerateReport(wxDebugReport::Context_Current);
}

void MyTaskBarIcon::OnRestart(wxCommandEvent&)
{
	wxGetApp().Restart();
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
	if (wxGetApp().AnyModalDlg()) {
		wxMessageBox(wxT("主界面有对话框未关闭!"), wxT("提示"),wxOK);
		return;
	}
    wxGetApp().Exit();
}

static bool check = true;

void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
{
    check = !check;
}

void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
{
    event.Check(check);
}

void MyTaskBarIcon::OnMenuSetNewIcon(wxCommandEvent&)
{
    /*wxIcon icon(smile_xpm);

    if (!SetIcon(icon, "wxTaskBarIcon Sample - a different icon"))
        wxMessageBox("Could not set new icon.");*/
}

void MyTaskBarIcon::OnMenuSub(wxCommandEvent&)
{
    wxMessageBox("You clicked on a submenu!");
}

// Overridables
wxMenu *MyTaskBarIcon::CreatePopupMenu()
{
	wxMenu *menu = new wxMenu;
	wxString strAppStatus = wxGetApp().GetAppStatus();
	menu->Append(PU_INFO, strAppStatus);
	menu->Enable(PU_INFO, false);
	menu->AppendSeparator();
	menu->Append(PU_ABOUT, _("About"));
	if (wxGetApp().IsRunFlag()) {
		menu->AppendSeparator();
		menu->Append(PU_RESTORE, wxT("打开主窗口"));
	}
	if (!ZQDBIsTest()) {
		menu->AppendSeparator();
		//menu->Append(PU_PREFERENCE, "设置");
		menu->Append(PU_CLEARDATA, "清理数据");
		menu->Append(PU_RESETTINGS, "重新设置");
		//menu->Append(PU_REPORTCONTEXT, "报告上下文");
		menu->AppendSeparator();
		menu->Append(PU_RESTART, wxT("重启"));
	}
	/*menu->AppendSeparator();
	menu->Append(PU_NEW_ICON, "&Set New Icon");
	menu->AppendSeparator();
	wxMenu *submenu = new wxMenu;
	submenu->Append(PU_SUB1, "监控");
	submenu->Append(PU_SUB2, "策略");
	menu->Append(PU_SUBMAIN, "新建", submenu);*/
	//menu->AppendSeparator();
	//menu->AppendCheckItem(PU_CHECKMARK, "Test &check mark");
    /* OSX has built-in quit menu for the dock menu, but not for the status item */
#ifdef __WXOSX__
    if ( OSXIsStatusItem() )
#endif
    {
        menu->AppendSeparator();
        menu->Append(PU_EXIT, wxT("退出"));
    }
    return menu;
}

void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	if (wxGetApp().IsRunFlag()) {
		wxGetApp().ShowFrame();
	}
	else {
		wxMessageBox(wxT("正在初始化..."), wxT("提示"), wxOK);
	}
}
