///////////////////////////////////////////////////////////////////////////////
// Name:        ribbondemo.cpp
// Purpose:     wxRibbon: Ribbon user interface - sample/test program
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// Copyright:   (C) Copyright 2009, Peter Cawley
// Licence:     wxWindows Library Licence
///////////////////////////////////////////////////////////////////////////////

#include "myapp.h" 
#include "mylog.h"
#include "mysettingsdlg.h"
#include "myctp.h"
#include "mylogindlg.h"
#include "mytaskbar.h"
#include "mysmartkbdlg.h"
#include "myframe.h"
#include "mycalcframe.h"
#include "mystrategyframe.h"
#include <bz.h>
#include <zqdb.pb.h>

// this is a definition so can't be in a header
wxDEFINE_EVENT(MY_EVENT, wxCommandEvent);
wxDEFINE_EVENT(ZQDB_NOTIFY_LOG_UPDATE_EVENT, wxStringCommandEvent);

static wxString FormatTips(const char* xml, size_t xmlflag)
{
	CFG_FROM_XML(cfg, xml, xmlflag);
	//std::string s = u8"连接中...";
	std::string utf_content = cfg.get<std::string>("content", "");
	//std::wstring ws = zqdb::utf8_to_unicode(s);
	wxString content = utf2wxString(utf_content.c_str());
	switch (cfg.get<int>("type", 0))
	{
	case ZQDB_LOG_TIP_TYPE_MODULE: {
		return wxString::Format(wxT("%s %s"), cfg.get<std::string>("module", "").c_str(), content.wx_str());
	} break;
	case ZQDB_LOG_TIP_TYPE_USER: {
		return wxString::Format(wxT("%s %s %s"), cfg.get<std::string>("module", "").c_str(), cfg.get<std::string>("user", "").c_str(), content.wx_str());
	} break;
	case ZQDB_LOG_TIP_TYPE_CALC: {
		return wxString::Format(wxT("%s %s"), cfg.get<std::string>("calc", "").c_str(), content.wx_str());
	} break;
	default: {
		return content;
	} break;
	}
}

extern "C" {

void CB_LOG(int level, char const* const format, void const* const args)
{
	if (wxGetApp().IsExitFlag()) {
		return;
	}
	const char* logstr = nullptr;
	if (args) {
		//BZLogPrintF(format, args);
		char buf[4096] = { 0 };
		try {
			BZSLogF(buf, 4096, format, args);
			logstr = buf;
		}
		catch (...) {
			wxLogWarning("BZSLogF exception level=%d", level);
			return;
		}
	}
	else {
		logstr = format;
	}
	switch (level)
	{
	case 0: {
		//
	} break;
	case 1: {
		wxLogError(utf2wxString(logstr));
	} break;
	case 2: {
		wxLogWarning(utf2wxString(logstr));
	} break;
	case 3: {
		wxLogInfo(utf2wxString(logstr));
	} break;
	case 4: {
		wxLogDebug(utf2wxString(logstr));
	} break;
	case ZQDB_LOG_LEVEL_TIP: {
		try {
			CFG_FROM_XML(cfg, logstr, XUtil::XML_FLAG_JSON_STRING);
			wxLogInfo(FormatTips((const char*)&cfg, XUtil::XML_FLAG_PTREE));
			wxGetApp().Post([cfg = std::move(cfg)]() {
				wxGetApp().ShowTips((const char*)&cfg, XUtil::XML_FLAG_PTREE);
			});
		}
		catch (const std::exception& ex) {
			wxLogWarning(wxT("process tips[%s] exception:%s"), utf2wxString(logstr).wx_str(), ex.what());
		}
		catch (...) {
			wxLogWarning(wxT("process tips[%s] exception: unknown"), utf2wxString(logstr));
		}
	} break;
	default:
		break;
	}
}

void CB_STATUS(HNNODE h, NET_NODE_STATUS status)
{
	if (wxGetApp().IsExitFlag()) {
		return;
	}
	wxGetApp().HandleNetStatus(h, status);
}

int CB_MSG(HNMSG hmsg, size_t* flags)
{
	if (wxGetApp().IsExitFlag()) {
		return 0;
	}
	return wxGetApp().HandleNetMsg(hmsg, flags);
}

void CB_NOTIFY(HZQDB h, ZQDB_NOTIFY_TYPE notify)
{
	if (wxGetApp().IsExitFlag()) {
		return;
	}
	wxGetApp().HandleNotify(h, notify);
}

int CB_ORDER_SEND(HZQDB user, HZQDB code, char direction, char offset, char type, double volume, double price, HNMSG* rsp, size_t timeout)
{
	if (wxGetApp().IsExitFlag()) {
		return 0;
	}
	return wxGetApp().OrderSend(user, code, direction, offset, type, volume, price, rsp, timeout
		, ZQDBGetCalcTradeFlag() ? 0 : ORDER_SEND_FLAG_SHOWTIPS
	);
}

int CB_ORDER_CANCEL(HZQDB user, HZQDB order, HNMSG* rsp, size_t timeout)
{
	if (wxGetApp().IsExitFlag()) {
		return 0;
	}
	return wxGetApp().OrderCancel(user, order, rsp, timeout
		, ZQDBGetCalcTradeFlag() ? 0 : ORDER_SEND_FLAG_SHOWTIPS
	);
}

int CB_ORDER_CLOSE(HZQDB user, HZQDB position, char type, double volume, double price, HNMSG* rsp, size_t timeout)
{
	if (wxGetApp().IsExitFlag()) {
		return 0;
	}
	return wxGetApp().OrderClose(user, position, type, volume, price, rsp, timeout
		, ZQDBGetCalcTradeFlag() ? 0 : ORDER_SEND_FLAG_SHOWTIPS
	);
}

}

// -- implementations --

MyEventFilter::MyEventFilter()
{
	wxEvtHandler::AddFilter((MyEventFilter*)this);
}

MyEventFilter::~MyEventFilter()
{
	wxEvtHandler::RemoveFilter(this);
}

int MyEventFilter::FilterEvent(wxEvent& event)
{
	const wxEventType t = event.GetEventType(); 
	auto obj = event.GetEventObject();
	if (obj) {
		if (obj->IsKindOf(wxCLASSINFO(wxWindow))) {
			auto wnd = wxDynamicCast(obj, wxWindow);
			if (wnd) {
				auto topWnd = wnd;
				while (wnd = wnd->GetParent())
				{
					topWnd = wnd;
				}
				ASSERT(topWnd);
				if (topWnd->IsKindOf(wxCLASSINFO(wxFrame))) {
					auto frame = wxDynamicCast(topWnd, wxFrame);
					if (wxGetApp().FindTechFrame(frame)) {
						int ret = ((MyFrame*)frame)->FilterEvent(event);
						if (ret != Event_Skip) {
							return ret;
						}
					}
				}
				else if (topWnd->IsKindOf(wxCLASSINFO(wxDialog))) {
					auto dlg = wxDynamicCast(topWnd, wxDialog);
					if (dlg == wxGetApp().GetSmartKBDlg()) {
						int ret = ((MySmartKBDlg*)dlg)->FilterEvent(event);
						if (ret != Event_Skip) {
							return ret;
						}
					}
				}
			}
		}
	}
	// Continue processing the event normally as well.
	return Event_Skip;
}

wxIMPLEMENT_APP(MyApp);

#include "transparent.xpm"

MyApp::MyApp() : timer_(this)
{
	// call this to tell the library to call our OnFatalException()
	wxHandleFatalExceptions();

	BZLogSet(&CB_LOG);
}

#if !defined(mytraderpro) && !defined(mytraderd)
void MyApp::LoadSkinInfo()
{
	wxScreenDC  dc;

	auto skin_info_ptr = std::make_shared<SkinInfo>();

	auto artProvider = new wxRibbonDefaultArtProvider();
	skin_info_ptr->artProvider = artProvider;
	if (skin_info_ptr->artProvider) {
		skin_info_ptr->artProvider->GetColourScheme(&skin_info_ptr->crPrimary
			, &skin_info_ptr->crSecondary, &skin_info_ptr->crTertiary);
		skin_info_ptr->crViewBkgnd = skin_info_ptr->artProvider->GetColour(wxRIBBON_ART_PAGE_BACKGROUND_COLOUR);		//视图色
		//skin_info_ptr->crCtrlBkgnd = skin_info_ptr->artProvider->GetColour(wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR);		//控件色
		skin_info_ptr->crCtrlForgnd.Set(234, 242, 251);
		skin_info_ptr->crViewForgnd = skin_info_ptr->crTertiary;		//视图文字
		skin_info_ptr->crCtrlForgnd = skin_info_ptr->crTertiary;		//视图文字
	}
	else {
		skin_info_ptr->crPrimary.Set(194, 216, 241);
		skin_info_ptr->crSecondary.Set(255, 223, 114);
		skin_info_ptr->crTertiary.Set(0, 0, 0);
		skin_info_ptr->crViewBkgnd.Set(201, 217, 237);
		skin_info_ptr->crCtrlForgnd.Set(234, 242, 251);
		skin_info_ptr->crViewForgnd = skin_info_ptr->crTertiary;		//视图文字
		skin_info_ptr->crCtrlForgnd = skin_info_ptr->crTertiary;		//视图文字
	}

	skin_info_ptr->curDragLeftRight = wxCursor(wxCURSOR_SIZEWE);
	skin_info_ptr->curDragUpDown = wxCursor(wxCURSOR_SIZENS);
	skin_info_ptr->curDrawLine = wxCursor(wxCURSOR_PENCIL);
	
	skin_info_ptr->crBackgnd.Set(7, 7, 7);		//背景
	skin_info_ptr->crTabSelBackgnd.Set(64, 0, 0);	//标签选中背景
	skin_info_ptr->crRptTitleBakcgnd.Set(27, 27, 27);//报表标题背景
	skin_info_ptr->crRptSelBackgnd.Set(38, 38, 38);	//报表选中背景

	skin_info_ptr->crTitle.Set(128, 128, 128);			//标题
	skin_info_ptr->crText.Set(224, 224, 224);			//文字
	skin_info_ptr->crRising.Set(255, 90, 90);			//上涨
	skin_info_ptr->crFalling.Set(59, 235, 104);		//下跌
	skin_info_ptr->crCommodityCode.Set(80, 248, 255);	//代码
	skin_info_ptr->crCommodityName.Set(252, 252, 84);	//名称
	skin_info_ptr->crAmount.Set(252, 252, 84);			//价
	skin_info_ptr->crVolume.Set(252, 252, 84);			//量
	skin_info_ptr->crTabSel.Set(84, 252, 252);			//标签选中

	skin_info_ptr->crLine.Set(255, 255, 255);			//线
	skin_info_ptr->crAverageLine.Set(252, 252, 84);	//均线
	skin_info_ptr->crDrawLine.Set(255, 0, 255);		//画线
	skin_info_ptr->crXYLine.Set(60, 60, 60);			//X、Y分隔线
	skin_info_ptr->crXText.Set(125, 125, 125);			//X坐标文字
	skin_info_ptr->crYText.Set(125, 125, 125);//QCOLOR_DEF_YAXIS_VALUE;			//Y坐标文字
	skin_info_ptr->crCrossCursor.Set(224, 224, 224);	//十字游标	
	skin_info_ptr->crRptLine.Set(81, 81, 81);		//报表线
	skin_info_ptr->crRisingLine.Set(255, 52, 52);		//上涨线
	skin_info_ptr->crFallingLine.Set(84, 252, 252);	//下跌线
	skin_info_ptr->crILine[0].Set(252, 252, 252);
	skin_info_ptr->crILine[1].Set(252, 252, 84);
	skin_info_ptr->crILine[2].Set(252, 84, 252);
	skin_info_ptr->crILine[3].Set(84, 232, 61);
	skin_info_ptr->crILine[4].Set(255, 109, 0);
	skin_info_ptr->crILine[5].Set(185, 120, 234);
	skin_info_ptr->crILine[6].Set(107, 142, 212);
	skin_info_ptr->crILine[7].Set(164, 164, 137);
	skin_info_ptr->crRefline.Set(60, 60, 60);

	skin_info_ptr->penTitle = wxPen(skin_info_ptr->crTitle, 1);			//和Title颜色一致的线
	skin_info_ptr->penLine = wxPen(skin_info_ptr->crLine, 1);			//线
	skin_info_ptr->penRisingLine = wxPen(skin_info_ptr->crRisingLine, 1);	//上涨线
	skin_info_ptr->penFallingLine = wxPen(skin_info_ptr->crFallingLine, 1);	//下跌线
	skin_info_ptr->penAverageLine = wxPen(skin_info_ptr->crAverageLine, 1);	//均线
	skin_info_ptr->penDrawLine = wxPen(skin_info_ptr->crDrawLine, 1);		//画线
	skin_info_ptr->penXYLine = wxPen(skin_info_ptr->crXYLine, 1);		//X、Y分隔线
	//skin_info_ptr->penCrossCursor = wxPen(skin_info_ptr->crCrossCursor, 1);	//十字游标	
	skin_info_ptr->penCrossCursor = wxPen(wxColour(80, 80, 80), 0);
	skin_info_ptr->penYCrossCursor = wxPen(wxColour(60, 60, 60), 0);
	skin_info_ptr->penRptLine = wxPen(skin_info_ptr->crRptLine, 1);		//报表线
	skin_info_ptr->penILine[0] = wxPen(skin_info_ptr->crILine[0], 1);		//指标线
	skin_info_ptr->penILine[1] = wxPen(skin_info_ptr->crILine[1], 1);		//指标线
	skin_info_ptr->penILine[2] = wxPen(skin_info_ptr->crILine[2], 1);		//指标线
	skin_info_ptr->penILine[3] = wxPen(skin_info_ptr->crILine[3], 1);		//指标线
	skin_info_ptr->penILine[4] = wxPen(skin_info_ptr->crILine[4], 1);		//指标线
	skin_info_ptr->penILine[5] = wxPen(skin_info_ptr->crILine[5], 1);		//指标线
	skin_info_ptr->penILine[6] = wxPen(skin_info_ptr->crILine[6], 1);		//指标线
	skin_info_ptr->penILine[7] = wxPen(skin_info_ptr->crILine[7], 1);		//指标线
	skin_info_ptr->penRefline = wxPen(skin_info_ptr->crRefline, 0, wxPENSTYLE_DOT);		//参考线

	skin_info_ptr->brush = wxBrush(skin_info_ptr->crLine);
	skin_info_ptr->brushRising = wxBrush(skin_info_ptr->crRisingLine);
	skin_info_ptr->brushFalling = wxBrush(skin_info_ptr->crFallingLine);
	skin_info_ptr->brushDrawLine = wxBrush(skin_info_ptr->crDrawLine);
	skin_info_ptr->brushCrossCursor = wxBrush(wxColour(38, 38, 38));
	skin_info_ptr->brushNull = *wxTRANSPARENT_BRUSH;

	//skin_info_ptr->bmpNull = wxBitmap(transparent_xpm);

	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	skin_info_ptr->fontTitle = wxFont(10, font.GetFamily(), font.GetStyle(), font.GetWeight(), font.GetUnderlined()
		, font.GetFaceName());
	skin_info_ptr->fontText = wxFont(9, font.GetFamily(), font.GetStyle(), wxFONTWEIGHT_BOLD, font.GetUnderlined()
		, font.GetFaceName());
	skin_info_ptr->fontRptTitle = wxFont(12, font.GetFamily(), font.GetStyle(), font.GetWeight(), font.GetUnderlined()
		, wxT("宋体"));
	skin_info_ptr->fontRptText = wxFont(12, font.GetFamily(), font.GetStyle(), font.GetWeight(), font.GetUnderlined()
		, wxT("宋体"));
	skin_info_ptr->fontXText = wxFont(12, font.GetFamily(), font.GetStyle(), font.GetWeight(), font.GetUnderlined()
		, wxT("宋体"));
	skin_info_ptr->fontYText = wxFont(12, font.GetFamily(), font.GetStyle(), font.GetWeight(), font.GetUnderlined()
		, wxT("宋体"));

	wxString strText = wxT("高速行情交易系统");
	dc.SetFont(skin_info_ptr->fontTitle);
	skin_info_ptr->xyTitle = dc.GetTextExtent(strText);
	skin_info_ptr->xyTitle.x /= strText.Length();
	dc.SetFont(skin_info_ptr->fontText);
	skin_info_ptr->xyText = dc.GetTextExtent(strText);
	skin_info_ptr->xyText.x /= strText.Length();
	dc.SetFont(skin_info_ptr->fontRptTitle);
	skin_info_ptr->xyRptTitle = dc.GetTextExtent(strText);
	skin_info_ptr->xyRptTitle.x /= strText.Length();
	dc.SetFont(skin_info_ptr->fontRptText);
	skin_info_ptr->xyRptText = dc.GetTextExtent(strText);
	skin_info_ptr->xyRptText.x /= strText.Length();
	dc.SetFont(skin_info_ptr->fontXText);
	skin_info_ptr->xyXText = dc.GetTextExtent(strText);
	skin_info_ptr->xyXText.x /= strText.Length();
	dc.SetFont(skin_info_ptr->fontYText);
	skin_info_ptr->xyYText = dc.GetTextExtent(strText);
	skin_info_ptr->xyYText.x /= strText.Length();
	skin_info_ptr->xySpace.x = 2;
	skin_info_ptr->xySpace.y = 3;

	skin_info_ptr->xyTabCtrl.x = 18;
	skin_info_ptr->xyTabCtrl.y = 18;
	skin_info_ptr->xyScrollBar.x = 18;
	skin_info_ptr->xyScrollBar.y = 18;
	skin_info_ptr->xyWndIndicator.x = 100;
	skin_info_ptr->xyWndIndicator.y = 100;
	skin_info_ptr->xyInfoIndicator.x = skin_info_ptr->xyXText.x * 5;
	skin_info_ptr->xyInfoIndicator.y = 24;
	skin_info_ptr->xyCoordinate = wxWindow::FromDIP(wxSize(18, 18), dc.GetWindow());

	skin_info_ptr->nBarWidth[0] = 1;
	skin_info_ptr->nBarWidth[1] = 1;
	skin_info_ptr->nBarWidth[2] = 1;
	skin_info_ptr->nBarWidth[3] = 1;
	skin_info_ptr->nBarWidth[4] = 3;
	skin_info_ptr->nBarWidth[5] = 5;
	skin_info_ptr->nBarWidth[6] = 7;
	skin_info_ptr->nBarWidth[7] = 9;
	skin_info_ptr->nBarWidth[8] = 12;
	skin_info_ptr->nBarSpace[0] = 0;
	skin_info_ptr->nBarSpace[1] = 1;
	skin_info_ptr->nBarSpace[2] = 2;
	skin_info_ptr->nBarSpace[3] = 3;
	skin_info_ptr->nBarSpace[4] = 3;
	skin_info_ptr->nBarSpace[5] = 3;
	skin_info_ptr->nBarSpace[6] = 3;
	skin_info_ptr->nBarSpace[7] = 3;
	skin_info_ptr->nBarSpace[8] = 3;
	skin_info_ptr->nBarShift = 100;

	skin_info_ptr->crCtrlText.Set(0, 0, 0); //平
	skin_info_ptr->crCtrlRising.Set(217, 36, 48/*255, 62, 62*/); //涨
	skin_info_ptr->crCtrlFalling.Set(63, 148, 42/*84, 252, 84*/); //跌

#ifdef _DEBUG
	/*auto bmp = skin_info_ptr->TextToBitmap16(wxT("5S"), wxFontInfo(10).Bold(), skin_info_ptr->crCtrlText);
	skin_info_ptr->SetBitmap16(wxT("5秒"), bmp);
	//skin_info_ptr->Save(bmp);
	bmp.SaveFile(wxT("D:/5.png"), wxBITMAP_TYPE_PNG);*/
#endif
	skin_info_ptr->UpdateTechCycleBitmap16();
	skin_info_ptr->UpdateTechCycleExBitmap16(CYC_ANYSEC, wxGetApp().GetTechCycleAnySec());
	skin_info_ptr->UpdateTechCycleExBitmap16(CYC_ANYMIN, wxGetApp().GetTechCycleAnyMin());

	SetSkinInfo(skin_info_ptr);
}
#endif

class MySplashScreen : public wxSplashScreen
{
public:
	using wxSplashScreen::wxSplashScreen;

	virtual int FilterEvent(wxEvent& event) wxOVERRIDE
	{
		const wxEventType t = event.GetEventType();
		auto obj = event.GetEventObject();
		if (obj) {
			if (obj->IsKindOf(wxCLASSINFO(wxWindow))) {
				auto wnd = wxDynamicCast(obj, wxWindow);
				if (wnd == this || wnd == m_window) {
					return wxSplashScreen::FilterEvent(event);
				}
			}
		}
		return Event_Skip;
	}
};

static const wxColour SplashScreenColor = *wxBLACK;
static const wxColour SplashScreenBkColor = *wxWHITE;

// Draws artwork onto our splashscreen at runtime
void MyApp::DecorateSplashScreen(wxBitmap& bmp, bool about)
{
	// use a memory DC to draw directly onto the bitmap
	wxMemoryDC memDc(bmp);

	auto szBmp = bmp.GetSize();
	const wxRect rcBmp(0, 0, szBmp.x, szBmp.y);

	if (about) {
		// draw an orange box (with black outline) at the bottom of the splashscreen.
		// this box will be 10% of the height of the bitmap, and be at the bottom.
		const wxRect bannerRect(wxPoint(0, (szBmp.y / 10) * 2), wxPoint(szBmp.x, (szBmp.y / 10) * 8));
		memDc.SetPen(*wxBLACK_PEN);
		wxDCBrushChanger bc(memDc, wxBrush(SplashScreenBkColor));
		memDc.DrawRectangle(bannerRect);
		//memDc.GradientFillLinear(bannerRect, SplashScreenBkColor, SplashScreenBkColor);
		memDc.DrawLine(bannerRect.GetTopLeft(), bannerRect.GetTopRight());
	}
	else {
		// draw an orange box (with black outline) at the bottom of the splashscreen.
		// this box will be 10% of the height of the bitmap, and be at the bottom.
		const wxRect bannerRect(wxPoint(0, (szBmp.y / 10) * 9), wxPoint(szBmp.x, szBmp.y));
		memDc.SetPen(*wxBLACK_PEN);
		wxDCBrushChanger bc(memDc, wxBrush(SplashScreenBkColor));
		memDc.DrawRectangle(bannerRect);
		//memDc.GradientFillLinear(bannerRect, SplashScreenBkColor, SplashScreenBkColor);
		memDc.DrawLine(bannerRect.GetTopLeft(), bannerRect.GetTopRight());

		// dynamically get the wxWidgets version to display
		wxString description = wxString::Format("" APP_NAME " %s", MYTRADER_VERSION);
		// create a copyright notice that uses the year that this file was compiled
		wxString year(__DATE__);
		wxString copyrightLabel = wxString::Format("%s%s " APP_NAME ". %s",
			wxString::FromUTF8("\xc2\xa9"), year.Mid(year.Length() - 4),
			"All rights reserved.");

		// draw the (white) labels inside of our orange box (at the bottom of the splashscreen)
		//wxDCFontChanger(memDc, wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT)); 
		memDc.SetFont(wxFontInfo(12));
		memDc.SetTextForeground(SplashScreenColor);
		// draw the "wxWidget" label on the left side, vertically centered.
		// note that we deflate the banner rect a little bit horizontally
		// so that the text has some padding to its left.
		memDc.DrawLabel(description, bannerRect.Deflate(5, 0), wxALIGN_CENTRE_VERTICAL | wxALIGN_LEFT);

		// draw the copyright label on the right side
		//memDc.SetFont(wxFontInfo(8));
		memDc.DrawLabel(copyrightLabel, bannerRect.Deflate(5, 0), wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT);

		// draw tips
		memDc.DrawLabel(wxT("just a moment, please..."), rcBmp.Deflate(5, 0), wxALIGN_BOTTOM | wxALIGN_LEFT);
	}
}

#if !defined(mytraderpro) && !defined(mytraderd)

bool g_wait = false;
bool g_clear_settings = false;
bool g_clear_data = false;
bool g_test = false;

#if wxUSE_CMDLINE_PARSER

#include <wx/cmdline.h>

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (parser.Found(OPTION_WAIT)) {
		g_wait = true;
	}
	if (parser.Found(OPTION_CLEAR_SETTINGS)) {
		g_clear_settings = true;
	}
	if (parser.Found(OPTION_CLEAR_DATA)) {
		g_clear_data = true;
	}
	if (parser.Found(OPTION_TEST)) {
		g_test = true;
	}
	return Base::OnCmdLineParsed(parser);
}

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	Base::OnInitCmdLine(parser);
	parser.AddLongSwitch(OPTION_WAIT);
	parser.AddLongSwitch(OPTION_CLEAR_SETTINGS);
	parser.AddLongSwitch(OPTION_CLEAR_DATA);
	parser.AddLongSwitch(OPTION_TEST);
}

#endif

bool MyApp::OnInit()
{
    if(!wxApp::OnInit())
        return false;

#if wxUSE_IMAGE
	wxInitAllImageHandlers();
#endif
//#if wxUSE_LIBPNG
//	wxImage::AddHandler(new wxPNGHandler);
//#endif

	//std::locale::global(std::locale(""));

	mylog::Init();

	SetVendorName("7thtool");
	//SetAppName(APP_NAME); // not needed, it's the default value

	SettingsBase::Init();

	// uncomment this to force writing back of the defaults for all values
	// if they're not present in the config - this can give the user an idea
	// of all possible settings for this program
	//pConfig->SetRecordDefaults();

	if (g_clear_data) {
#ifdef _DEBUG
		wxMessageBox("ClearData");
#endif
		DoClearData();
	}

	if (g_clear_settings) {
		boost::system::error_code ec;
		boost::filesystem::remove(MySettingsDlg::GetFile(), ec);
	}

	std::string app_path = XUtil::get_program_path().string();
	char sha[BZ_SHA_DIGEST_LENGTH] = { 0 };
	if (g_test) {
		std::string app_test = app_path + " --test";
		BZSHA1(app_test.data(), app_test.size(), sha);
	} else {
		BZSHA1(app_path.data(), app_path.size(), sha);
	}
	char sha_hex[BZ_SHA_DIGEST_LENGTH * 2 + 1] = { 0 };
	BZToHex(sha, BZ_SHA_DIGEST_LENGTH, sha_hex);
	snglinst_checker_ = std::make_shared<wxSingleInstanceChecker>(sha_hex);
	if (snglinst_checker_->IsAnotherRunning()) {
		if (g_wait) {
#ifdef _DEBUG
			wxMessageBox("IsAnotherRunning");
#endif
			//snglinst_checker_.reset();
			/*do {
				wxSleep(1);
				snglinst_checker_ = std::make_shared<wxSingleInstanceChecker>(sha_hex);
			} while (snglinst_checker_->IsAnotherRunning());*/
			wxSleep(5);
			auto cmd = wxString::Format(wxT(APP_NAME " --wait"));
			auto pid = wxExecute(cmd);
			FinalRelease();
			return false;
		}
		else {
			wxMessageBox(wxT(APP_NAME "在运行中..."), wxT("提示"));
			FinalRelease();
			return false;
		}
	}

	//这里保证只有一个mytrader或则mytrader test在运行

	busy_info_ = std::make_shared<wxBusyInfo>(g_test ? wxT("准备超级回测中，请稍后...") : wxT("启动中，请稍后..."));

	LoadSkinInfo();

	std::string settings_file;
	if (g_test) {
		settings_file = MySettingsDlg::GetFile(g_test);
	}
	else {
		if (!MySettingsDlg::HealthCheck(settings_file)) {
			busy_info_.reset();
			//先清理数据
			DoClearData();
			//再重新设置
			auto path_module_dir = boost::dll::this_line_location().remove_leaf();
			auto path_desc_file = path_module_dir;
			path_desc_file.append(APP_NAME).append("desc.json");
			auto desc_file = path_desc_file.string();
			CFG_FROM_XML(cfg, desc_file.c_str(), XUtil::XML_FLAG_JSON_FILE);
			MySettingsDlg dlg(cfg);
			if (wxID_OK != dlg.ShowModal()) {
				FinalRelease();
				return false;
			}
			XUtil::json_to_file(settings_file, cfg);
			busy_info_ = std::make_shared<wxBusyInfo>(g_test ? wxT("准备超级回测中，请稍后...") : wxT("启动中，请稍后..."));
		}
	}

	CFG_FROM_XML(cfg, settings_file.c_str(), XUtil::XML_FLAG_JSON_FILE);

	if (g_test) {

	}
	else {
		//根据版本号做兼容性处理
		bool version_is_too_low = false;
		auto opt_version = cfg.get<std::string>("version", "");
		if (opt_version.compare("1.7.1.1") < 0) {
			version_is_too_low = true;
		}
		if (version_is_too_low) {
			//清理数据
			DoClearData();

			//修改版本配置
			cfg.put("version", APP_VERSION);
			XUtil::json_to_file(settings_file, cfg);
		}
	}
	/*if (ZQDBFindData((const char*)settings_file.c_str(), XUtil::XML_FLAG_JSON_FILE)) {
		if (wxOK == wxMessageBox(wxT("以下情况会导致明细、K线等数据缺失：\n")
			wxT("1、程序交易时段关闭过；\n")
			wxT("2、程序运行时崩溃过；\n")
			wxT("3、程序重新设置过。\n")
			wxT("如果您遇到了上述情况，建议选择【确定】清理数据。\n")
			wxT("如果您没有数据缺失问题，请选择默认【取消】正常启动程序。\n")
			wxT("需要清理数据吗？"), wxT("提示"), wxOK | wxCANCEL | wxCANCEL_DEFAULT, nullptr)) {
			ZQDBClearData((const char*)settings_file.c_str(), XUtil::XML_FLAG_JSON_FILE);
		}
	}*/

#if 0
	wxBitmap bitmap("./res/splash.png", wxBITMAP_TYPE_PNG);
	if (bitmap.IsOk()) {
		// we can even draw dynamic artwork onto our splashscreen
		DecorateSplashScreen(bitmap);
		//skin_info_ptr_->Save(bitmap);
		// show the splashscreen
		new wxSplashScreen(bitmap,
			wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
			6000, nullptr, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxSIMPLE_BORDER | wxSTAY_ON_TOP);
	}
#endif//

	MatchCH::Init(cfg.get<std::string>("pinyin").c_str());

	ZQDB_CALC_INF zqdb_inf = { XUtil::XML_FLAG_JSON_FILE, (const char*)settings_file.c_str(), &CB_STATUS, &CB_MSG, &CB_NOTIFY, &CB_ORDER_SEND, &CB_ORDER_CANCEL, &CB_ORDER_CLOSE };
	ZQCalcStart(&zqdb_inf); 

	if (!wxTaskBarIcon::IsAvailable())
	{
		wxMessageBox
		(
			"There appears to be no system tray support in your current environment. This sample may not behave as expected.",
			"Warning",
			wxOK | wxICON_EXCLAMATION
		);
	}

	taskbaricon_ = new MyTaskBarIcon();

	// we should be able to show up to 128 characters on Windows
	if (!taskbaricon_->SetIcon(wxICON(mytrader), GetAppTitle()))
	{
		wxLogError("Could not set icon.");
	}

#if defined(__WXOSX__) && wxOSX_USE_COCOA
	dockicon_ = new MyTaskBarIcon(wxTBI_DOCK);
	if (!dockicon_->SetIcon(wxICON(sample)))
	{
		wxLogError("Could not set icon.");
	}
#endif

	Bind(wxEVT_TIMER, &MyApp::OnTimer, this);
	timer_.Start(1, true);

    return true;
}

void MyApp::FinalRelease()
{
	busy_info_.reset();

	google::protobuf::ShutdownProtobufLibrary();

	// clean up: Set() returns the active config object as Get() does, but unlike
	// Get() it doesn't try to create one if there is none (definitely not what
	// we want here!)
	delete wxConfigBase::Set((wxConfigBase *)NULL);

	mylog::Term();
}

void MyApp::DoRestart(const wxString& args)
{
	auto cmd = wxString::Format(wxT(APP_NAME " %s"), args);
	auto pid = wxExecute(cmd);
}

#endif

void MyApp::ShowBusyInfo(const wxString& msg)
{
	if (busy_info_) {
		//busy_info_ = std::make_shared<wxBusyInfo>(msg);
		busy_info_->UpdateText(msg);
		//busy_info_->UpdateLabel(msg);
	}
	else {
		/*if (taskbaricon_) {
			taskbaricon_->ShowBalloon(wxEmptyString, msg);
		}*/
	}
}

void MyApp::Run()
{
	ZQDBRun();

	//第一次需要调用一下，这样才会创建计算模块
	ZQDBCalcUpdate(nullptr, ZQDB_NOTIFY_NONE);

	mylog::SetAsync();

	//ZQDBPrevTradeDay(EXCHANGE_CFFEX,20210808, 3);

	zqdb::AllModule allmodule;
	for (auto& h : allmodule)
	{
		zqdb::ObjectT<MODULEINFO> module(h);
		if (stricmp(module->Code, "ctp") == 0) {
			MyModule::AddModule(std::make_shared<MyCTPModule>(h));
		}
	}

	subscribes_ = wxSplit(GetFilterSubscribe(), wxT(','));
	LoadSubscribe();

	busy_info_.reset();

	//Post([this] {

	/*//先登录
	MyLoginDlg dlg;
	if (wxID_OK != dlg.ShowModal())
	{
	Exit();
	return;
	}*/

	// Create the main window
	/*smartkbdlg_ = new MySmartKBDlg();
	smartkbdlg_->SetSkinInfo(skin_info_ptr_);
	smartkbdlg_->Show(false);*/

	// Create the main frame window
	Goto(nullptr);

#ifdef _DEBUG
	const char* pysrc = R"(
import numpy
z1 = numpy.zeros((5,6), dtype=float)
z2 = numpy.zeros((4,3), dtype=float)
fill1(z1)
fill2(z2)
print(z1)
print(z2)
config = { 'name':'test', 'value':12312 }
i = 0
def init():
    # 策略初始化运行
    global i
    i = i + 1
    print(config['test'])
def handle(np_bar):
    global i
    i = i + 1
    print(i)
    print(np_bar)
)";
	//zqdb::Calc::AllModule allmodule;
	//HZQDB h = allmodule.back();
	//ZQDBCalcModuleCompileFunc(h, pysrc, [](const char* err, void* data) {
	//}, this);
	/*//auto result = std::async(//std::launch::async|std::launch::deferred,
	std::thread result(
	[h, pysrc] {
	ZQDBUpdateCalc();
	char err[1024] = { 0 };
	auto errlen = ZQDBCalcModuleCompileFunc(h, pysrc, err, 1024);
	});
	result.detach();*/
#endif//
	//});
}

void MyApp::FinalExit()
{
	timer_.Stop();
	TaskBase::Clear();
	StopAllStrategy();
	MyModule::RemoveAllModule();
	mylog::SetAsync(false);
	//bool bRPC = ZQDBIsRPC();
	ZQCalcStop();
	FinalRelease();
}

int MyApp::OnExit()
{
	FinalExit();
	return wxApp::OnExit();
}

#ifndef _DEBUG

void MyApp::OnFatalException()
{
	GenerateReport(wxDebugReport::Context_Exception);
}

#endif//

void MyApp::GenerateReport(wxDebugReport::Context ctx)
{
	wxDebugReportCompress *report = new wxDebugReportCompress;

	// add all standard files: currently this means just a minidump and an
	// XML file with system info and stack trace
	report->AddAll(ctx);

	//report->AddFile()
	
	// calling Show() is not mandatory, but is more polite
	if (wxDebugReportPreviewStd().Show(*report)) {
		if (report->Process()) {
#ifdef WIN32
			wxExecute(wxT("explorer.exe /select,") + report->GetCompressedFileName());
#else
			wxLaunchDefaultBrowser(report->GetCompressedFileName());
#endif//
			//wxLogMessage("Report generated in \"%s\".",
			//	report->GetCompressedFileName());
		}
		else {
			wxLaunchDefaultBrowser(report->GetDirectory());
		}
		report->Reset();
	}
	//else: user cancelled the report

	delete report;
}

void MyApp::Exit()
{
#ifndef _DEBUG
	if (wxOK == wxMessageBox(wxT("退出程序后将无法收集行情数据，将导致历史行情数据缺失。\n")
		wxT("确定要退出程序吗？"), wxT("提示"), wxOK | wxCANCEL, nullptr))
#endif
	{
		DoExit();
	}
}

void MyApp::DoExit()
{
	exit_flag_ = true;
	DoHide();
	if (smartkbdlg_) {
		delete smartkbdlg_;
		wxASSERT(smartkbdlg_ == nullptr);
	}
	if (taskbaricon_) {
		taskbaricon_->Destroy();
		taskbaricon_ = nullptr;
	}
}

wxString MyApp::GetAppMode()
{
	switch (ZQDBGetAppType())
	{
	case ZQDB_APP_STANDALONE: {
		return wxT("单机");
	} break;
	case ZQDB_APP_SOURCE: {
		return wxT("源服务");
	} break;
	case ZQDB_APP_MIDDLE: {
		return wxT("中继服务");
	} break;
	case ZQDB_APP_CLIENT: {
		return wxT("超级终端");
	} break;
	}
	return wxEmptyString;
}

wxString MyApp::GetAppTitle()
{
	wxString strAppTitle = GetAppName() + wxT(" ") + GetAppMode();
	if (ZQDBIsTest()) {
		strAppTitle += wxT(" - ");
		strAppTitle += wxT("超级回测");
	}
	return strAppTitle;
}

wxString MyApp::GetAppStatus()
{
	wxString strAppStatus = GetAppTitle();
	if (ZQDBIsTest()) {
		//
	}
	else {
		switch (ZQDBGetAppType())
		{
		case ZQDB_APP_STANDALONE: {
		} break;
		case ZQDB_APP_SOURCE: {
			auto count = ZQDBGetPeerCount();
			if (count) {
				strAppStatus += wxT(" - ");
				strAppStatus += wxString::Format(wxT("连接数[%zu]"), count);
			}
		} break;
		case ZQDB_APP_MIDDLE: {
			strAppStatus += wxT(" - ");
			strAppStatus += NETIsConnect(ZQDBGetCliNode()) ? wxT("在线") : wxT("连接中");
			auto count = ZQDBGetPeerCount();
			if (count) {
				strAppStatus += wxT(" - ");
				strAppStatus += wxString::Format(wxT("连接数[%zu]"), count);
			}
		} break;
		case ZQDB_APP_CLIENT: {
			strAppStatus += wxT(" - ");
			strAppStatus += NETIsConnect(ZQDBGetCliNode()) ? wxT("在线") : wxT("连接中");
		} break;
		}
	}
	return strAppStatus;
}

wxDialog* MyApp::AnyModalDlg()
{
	if (!modaldlgs_.empty()) {
		return modaldlgs_.back();
	}
	return nullptr;
}

bool MyApp::RemoveModalDlg(wxDialog* dlg)
{
	auto it = std::find(modaldlgs_.begin(), modaldlgs_.end(), dlg);
	if (it != modaldlgs_.end()) {
		modaldlgs_.erase(it);
		return true;
	}
	return false;
}

void MyApp::AddModalDlg(wxDialog* dlg)
{
	modaldlgs_.push_back(dlg);
}

void MyApp::DoHide()
{
	auto calc_frames = calc_frames_;
	for (auto frame : calc_frames)
	{
		delete frame;
	}
	wxASSERT(calc_frames_.empty());
	auto frames = tech_frames_;
	for (auto frame : frames) {
		delete frame;
	}
	wxASSERT(tech_frames_.empty());
	if (frame_) {
		delete frame_;
		frame_ = nullptr;
	}
}

wxFrame* MyApp::GetFrame()
{ 
	return frame_; 
}

bool MyApp::FindTechFrame(wxFrame * frame)
{
	if (frame == frame_) {
		return true;
	}
	auto it = std::find(tech_frames_.begin(), tech_frames_.end(), frame);
	if (it != tech_frames_.end()) {
		return true;
	}
	return false;
}

void MyApp::ShowFrame(wxFrame* frame)
{
	if (!frame) {
		frame = GetFrame();
	}
	if (!frame) {
		Goto(nullptr);
	} else {
		if (frame->IsIconized()) {
			frame->Restore();
		}
		//if (!frame->IsVisible()) {
		frame->Show();
		//}
		frame->Raise();
	}
}

void MyApp::ResetFrame(wxFrame * frame)
{
	frame_->RemoveFrame((MyBaseFrame*)frame);
	auto it = std::find(tech_frames_.begin(), tech_frames_.end(), frame);
	if (it != tech_frames_.end()) {
		tech_frames_.erase(it);
	}
}

void MyApp::Goto(HZQDB h, wxWindow* top)
{
	MyTechFrame* frame = (MyTechFrame*)top;
	if (!frame) {
		if (!frame_) {
			frame_ = new MyFrame("./" APP_NAME "/techfrm.json", XUtil::XML_FLAG_JSON_FILE);
			frame_->SetTitle(GetAppTitle());
			frame_->SetSkinInfo(skin_info_ptr_);
			auto info_ptr = std::make_shared<zqdb::TechContainerInfo>();
			info_ptr->Set(h);
			frame_->SetInfo(info_ptr);
			frame_->Maximize();
			frame = frame_;
		}
		else {
			auto techframe = new MyTechFrame("./" APP_NAME "/techfrm.json", XUtil::XML_FLAG_JSON_FILE);
			techframe->SetSkinInfo(skin_info_ptr_);
			auto info_ptr = std::make_shared<zqdb::TechContainerInfo>();
			info_ptr->Set(h);
			techframe->SetInfo(info_ptr);
			frame_->AddFrame(techframe);
			tech_frames_.emplace_back(techframe);
			frame = techframe;
		}
		frame->SetStatusText(last_tips_);
	}
	else {
		ASSERT(FindTechFrame(frame));
		frame->Set(h);
	}
	ShowFrame(frame);
}

void MyApp::ResetCalcFrame(MyCalcFrame* frame)
{
	//calc_frame_ = nullptr;
	auto it = std::find(calc_frames_.begin(), calc_frames_.end(), frame);
	if (it != calc_frames_.end()) {
		calc_frames_.erase(it);
	}
}

/*void MyApp::ShowCalcFrame()
{
	if (!calc_frame_) {
		calc_frame_ = new MyCalcFrame("./" APP_NAME "/techfrm.json", XUtil::XML_FLAG_JSON_FILE);
		//calc_frame_->SetSkinInfo(skin_info_ptr_);
		calc_frame_->SetInfo(info_ptr);
	}
	if (calc_frame_->IsIconized()) {
		calc_frame_->Restore();
	}
	//if (!calc_frame_->IsVisible()) {
		calc_frame_->Show();
	//}
	calc_frame_->Raise();
}*/

void MyApp::ShowCalcFrame(std::shared_ptr<zqdb::FuncContainerInfo> info_ptr)
{
	auto calc_frame = new MyCalcFrame(nullptr, XUtil::XML_FLAG_JSON_STRING);
	calc_frame->SetSkinInfo(skin_info_ptr_);
	calc_frame->SetInfo(info_ptr);
	calc_frames_.emplace_back(calc_frame);

	calc_frame->Show();
	calc_frame->Raise();
	calc_frame->CenterOnScreen();
}

void MyApp::ResetStrategyFrame(MyStrategyFrame * frame)
{
	auto it = std::find(strategy_frames_.begin(), strategy_frames_.end(), frame);
	if (it != strategy_frames_.end()) {
		strategy_frames_.erase(it);
	}
}

void MyApp::ShowPreferencesEditor(wxWindow* parent, size_t flags)
{
	if (!parent) {
		parent = frame_;
	}
	if (!m_prefEditor)
	{
		m_prefEditor.reset(new wxPreferencesEditor);
		m_prefEditor->AddPage(new PrefsPageGeneral());
		m_prefEditor->AddPage(new PrefsPageAbout());
	}

	m_prefEditor->Show(parent);
}

void MyApp::DismissPreferencesEditor()
{
	if (m_prefEditor)
		m_prefEditor->Dismiss();
}

void MyApp::Resettings(bool showMsg)
{
	if (!showMsg || wxOK == wxMessageBox(wxT("重新设置将会重启程序并弹出第一次运行时设置对话框。\n")
		wxT("确定要重新设置吗？"), wxT("提示"), wxOK | wxCANCEL, nullptr)) {
		//Post(50, [this] {
			DoExit();
			DoRestart(wxT("--" OPTION_WAIT " --" OPTION_CLEAR_DATA " --" OPTION_CLEAR_SETTINGS));
		//});
	}
}

void MyApp::DoClearData()
{
	//清理数据
	ZQDBClearData((const char*)MySettingsDlg::GetFile().c_str(), XUtil::XML_FLAG_JSON_FILE);
	//清理录制数据
	ZQDBClearRecord();
}

void MyApp::ClearData(bool showMsg)
{
	if (!showMsg || ZQDBIsRPC() || wxOK == wxMessageBox(wxT("清理数据需要关闭并重启程序。\n")
		wxT("确定要清理数据吗？"), wxT("提示"), wxOK | wxCANCEL, nullptr)) {
		//Post(50, [this] {
		DoExit();
		DoRestart(wxT("--" OPTION_WAIT " --" OPTION_CLEAR_DATA));
		//});
	}
}

void MyApp::Restart(bool showMsg)
{
	if (!showMsg || wxOK == wxMessageBox(wxT("确定要重启程序吗？"), wxT("提示"), wxOK | wxCANCEL, nullptr)) {
		//Post(50, [this] {
		DoExit();
		DoRestart(wxT("--" OPTION_WAIT));
		//});
	}
}

void MyApp::Test(size_t begin, size_t end, size_t speed)
{
	wxASSERT(!ZQDBIsTest());
	//if (wxOK == wxMessageBox(wxString::Format("确定开始超级回测吗？"), wxT("提示"), wxOK | wxCANCEL, nullptr)) {
		try {
			boost::system::error_code ec;
			boost::filesystem::path test_file(MySettingsDlg::GetFile(true));
			if (boost::filesystem::exists(test_file)) {
				boost::filesystem::remove(test_file);
			}
			boost::filesystem::copy(MySettingsDlg::GetFile(), test_file, ec);

			CFG_FROM_XML(cfg, test_file.string().c_str(), XUtil::XML_FLAG_JSON_FILE);
			cfg.put("test.begin", begin);
			cfg.put("test.end", end);
			cfg.put("test.speed", speed);
			cfg.put("mdb.name", "test.mdb");
			XUtil::json_to_file(test_file, cfg);

			//Post(50, [this] {
			auto cmd = wxString::Format(wxT(APP_NAME " --test"));
			auto pid = wxExecute(cmd);
			//});
			return;
		} catch (const std::exception& ex) {
		}
		wxMessageBox(wxT("启动超级回测失败!!!"), wxT("提示"), wxOK);
	//}
}

void MyApp::ShowAbout(wxWindow* parent)
{
	MyAboutDlg dlg(parent);
	dlg.ShowModal();
	return;
	wxBitmap bitmap("./res/splash.png", wxBITMAP_TYPE_PNG);
	if (bitmap.IsOk()) {
		// we can even draw dynamic artwork onto our splashscreen
		//DecorateSplashScreen(bitmap, true);
		//skin_info_ptr_->Save(bitmap);
		// show the splashscreen
		wxSplashScreen *splash = new MySplashScreen(bitmap,
			wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_NO_TIMEOUT,
			6000, parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			/*wxSIMPLE_BORDER | */wxSTAY_ON_TOP);

		wxWindow *win = splash->GetSplashWindow();

		wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
		sizer->AddStretchSpacer(1);

		auto stc_ver = new wxStaticText(win, wxID_ANY, wxString::Format(wxT("" APP_NAME " v%s"), MYTRADER_VERSION));
		stc_ver->SetForegroundColour(SplashScreenColor);
		stc_ver->SetBackgroundColour(SplashScreenBkColor);
		sizer->Add(stc_ver, wxSizerFlags().Center());
		//wxListBox *box = new wxListBox(this, wxID_ANY);
		//box->SetMinSize(wxSize(400, 300));
		//sizer->Add(box, wxSizerFlags(1).Border().Expand());
		//logo
		auto link_mytrader = new wxGenericHyperlinkCtrl(win, wxID_ANY, wxT("www.mytrader.org.cn"), wxT("www.mytrader.org.cn"));
		link_mytrader->SetBackgroundColour(SplashScreenBkColor);
		sizer->Add(link_mytrader, wxSizerFlags().Center());
		//qq
		//help
		auto link_help_doc = new wxGenericHyperlinkCtrl(win, wxID_ANY, _("help docs"), wxT("https://gitee.com/7thTool/mytrader"));
		link_help_doc->SetBackgroundColour(SplashScreenBkColor);
		sizer->Add(link_help_doc, wxSizerFlags().Center());
		//src
		auto link_mytrader_src = new wxGenericHyperlinkCtrl(win, wxID_ANY, wxT("https://gitee.com/7thTool/mytrader"), wxT("https://gitee.com/7thTool/mytrader"));
		link_mytrader_src->SetBackgroundColour(SplashScreenBkColor);
		sizer->Add(link_mytrader_src, wxSizerFlags().Center());
		auto link_zqdb_src = new wxGenericHyperlinkCtrl(win, wxID_ANY, wxT("https://gitee.com/7thTool/zqdb"), wxT("https://gitee.com/7thTool/zqdb"));
		link_zqdb_src->SetBackgroundColour(SplashScreenBkColor);
		sizer->Add(link_zqdb_src, wxSizerFlags().Center());

		sizer->AddStretchSpacer(1);

		//auto btn_ok = new wxButton(win, wxID_OK, _("&OK"));
		//auto btn_cancel = new wxButton(this, wxID_CANCEL, _("&Cancel"));

		//wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

		//sizerBottom->AddStretchSpacer(1);
		//sizerBottom->Add(btn_ok, 0, wxALIGN_CENTER | wxALL, 5);
		//sizerBottom->Add(btn_cancel, 0, wxALIGN_CENTER | wxALL, 5);
		//sizerBottom->AddStretchSpacer(1);

		//sizer->Add(sizerBottom, 0, wxEXPAND);

		win->SetSizer(sizer);
		win->Layout();

		/*wxStaticText *text = new wxStaticText(win,
			wxID_EXIT,
			"click somewhere\non this image",
			wxPoint(13, 11)
		);
		text->SetBackgroundColour(*wxWHITE);
		text->SetForegroundColour(*wxBLACK);
		wxFont font = text->GetFont();
		font.SetFractionalPointSize(2.0*font.GetFractionalPointSize() / 3.0);
		text->SetFont(font);*/
	}
}

void MyApp::LoadSubscribe()
{
	for (auto it = subscribes_.begin(); it != subscribes_.end();)
	{
		const auto& sub = *it;
		if (!sub.empty()) {
			auto h = ZQDBGetCode(wxString2utf(sub).c_str(), nullptr);
			if (h) {
				ZQDBSetFlags(h, ZQDBGetFlags(h) | ZQDB_CODE_FLAG_SUBSCRIBE);
				if (!ZQDBIsSubscribeMarketDataAll()) {
					ZQDBSubscribeMarketData(h);
				}
				it = subscribes_.erase(it);
			}
			else {
				++it;
			}
		}
		else {
			++it;
		}
	}
}

void MyApp::SaveSubscribe()
{
	std::ostringstream ss;
	zqdb::AllCode allcode;
	for (auto h : allcode)
	{
		if (ZQDBGetFlags(h) & ZQDB_CODE_FLAG_SUBSCRIBE) {
			zqdb::Code code(h);
			char szCode[260] = { 0 };
			ZQDBCodeCombine(code->Exchange, code->Code, szCode);
			ss << szCode << ",";
		}
	}
	ss.seekp(-1, ss.cur);
	SetFilterSubscribe(utf2wxString(ss.str().c_str()));
}

bool MyApp::IsSubscribe(HZQDB h)
{
	if (ZQDBGetFlags(h) & ZQDB_CODE_FLAG_SUBSCRIBE) {
		return true;
	}
	return false;
}

void MyApp::AddSubscribe(HZQDB h, bool save)
{
	ZQDBSetFlags(h, ZQDBGetFlags(h) | ZQDB_CODE_FLAG_SUBSCRIBE);
	if (!ZQDBIsSubscribeMarketDataAll()) {
		if (!ZQDBIsSubscribeMarketData(h)) {
			ZQDBSubscribeMarketData(h);
		}
	}
	if (save) {
		SaveSubscribe();
	}
}

void MyApp::RemoveSubscribe(HZQDB h, bool save)
{
	ZQDBSetFlags(h, ZQDBGetFlags(h) & ~ZQDB_CODE_FLAG_SUBSCRIBE);
	if (!ZQDBIsSubscribeMarketDataAll()) {
		if (ZQDBIsSubscribeMarketData(h)) {
			ZQDBUnSubscribeMarketData(h);
		}
	}
	if (save) {
		SaveSubscribe();
	}
}

void MyApp::OnTimer(wxTimerEvent& evt)
{
	if (!run_flag_) {
		if (!ZQDBIsOk()) {
			DoExit();
		}
		else {
			if (ZQDBIsTest()) {
				run_flag_ = true;
			}
			else {
				if (ZQDBIsRPC()) {
					run_flag_ = ZQDBIsConnect();
				}
				else {
					run_flag_ = true;
				}
			}
			if (run_flag_) {
				Run();
			}
			else {
				if (evt.GetInterval() < 1000) {
					//first
					if (ZQDBIsRPC()) {
						ShowBusyInfo(wxT("连接中，请稍后..."));
					}
				}
			}
		}
	}
	else {
		run_flag_++;

		if (ZQDBIsServer()) {
			//每天20：30服务暂停服务至少1分钟
			if (ZQDBIsListen()) {
				uint32_t date = 0, time = 0;
				date = XUtil::NowDateTime(&time);
				if (time > 203000 && time < 203100) {
					ZQDBStopListen();
				}
			}
		}
		if (suspend_flag_) {
			if (ZQDBIsServer()) {
				if (ZQDBIsListen()) {
					ASSERT(0);
					suspend_flag_ = 0;
				}
				else {
					//每天【20：31-21：00】之间服务断开超过60秒且所有市场都可用就自动重启服务
					uint32_t date = 0, time = 0;
					date = XUtil::NowDateTime(&time);
					if (time > 203100 && time < 210000) {
						if (!ZQDBGetCalcAnyDisabledExchange()) {
							if ((run_flag_ - suspend_flag_) > 60) {
								ZQDBStartListen();
							}
						}
					}
				}
			} else {
				//每天【20：30-21：00】之间连接断开超过30秒就需要重新启动程序
				if ((run_flag_ - suspend_flag_) > 30) {
					uint32_t date = 0, time = 0;
					date = XUtil::NowDateTime(&time);
					if (time > 203000 && time < 210000) {
						Restart(false);
						return;
					}
				}
			}
		}

		ZQDBCalcUpdateTimer();
		MyModule::Broadcast([](std::shared_ptr<MyModule> module)->bool {
			module->OnTimer();
			return false;
		});
		if (frame_) {
			auto title = GetAppStatus();
			if (title != frame_->GetTitle()) {
				frame_->SetTitle(title);
			}
			frame_->GetEventHandler()->ProcessEvent(evt);
		}
		for (auto frame : tech_frames_) {
			frame->GetEventHandler()->ProcessEvent(evt);
		}
	}
	//
	timer_.Start(1000, true);
}

void MyApp::OnNotifyConnect()
{
	suspend_flag_ = 0; //清空连接断开标志
	MyModule::Broadcast([](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyConnect();
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyConnect();
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyConnect();
	}
	if (frame_) {
		frame_->OnNotifyConnect();
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyConnect();
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyConnect();
	}
}

void MyApp::OnNotifyDisconnect()
{
	if (suspend_flag_) {
		return;//屏蔽重复通知
	}
	suspend_flag_ = run_flag_; //记录连接断开标志
	MyModule::Broadcast([](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyDisconnect();
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyDisconnect();
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyDisconnect();
	}
	if (frame_) {
		frame_->OnNotifyDisconnect();
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyDisconnect();
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyDisconnect();
	}
}

void MyApp::OnNotifyStartListen()
{
	suspend_flag_ = 0; //清空停止服务标志
	MyModule::Broadcast([](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyStartListen();
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyStartListen();
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyStartListen();
	}
	if (frame_) {
		frame_->OnNotifyStartListen();
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyStartListen();
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyStartListen();
	}
}

void MyApp::OnNotifyStopListen()
{
	if (suspend_flag_) {
		return;//屏蔽重复通知
	}
	suspend_flag_ = run_flag_; //记录停止服务标志	
	MyModule::Broadcast([](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyStopListen();
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyStopListen();
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyStopListen();
	}
	if (frame_) {
		frame_->OnNotifyStopListen();
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyStopListen();
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyStopListen();
	}
}


void MyApp::OnNotifyEnable(HZQDB h) 
{
	//加载订阅列表
	LoadSubscribe();
	//策略
	if (!ZQDBGetCalcAnyDisabledModule()) {
		//ReStartAllStrategy();
	}
	MyModule::Broadcast([h](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyEnable(h);
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyEnable(h);
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyEnable(h);
	}
	if (frame_) {
		frame_->OnNotifyEnable(h);
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyEnable(h);
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyEnable(h);
	}
}
void MyApp::OnNotifyDisable(HZQDB h)
{
	MyModule::Broadcast([h](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyDisable(h);
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyDisable(h);
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyDisable(h);
	}
	if (frame_) {
		frame_->OnNotifyDisable(h);
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyDisable(h);
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyDisable(h);
	}
}
void MyApp::OnNotifyAppend(HZQDB h)
{
	MyModule::Broadcast([h](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyAppend(h);
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyAppend(h);
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyAppend(h);
	}
	if (frame_) {
		frame_->OnNotifyAppend(h);
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyAppend(h);
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyAppend(h);
	}
}
void MyApp::OnNotifyRemove(HZQDB h)
{
	MyModule::Broadcast([h](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyRemove(h);
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyRemove(h);
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyRemove(h);
	}
	if (frame_) {
		frame_->OnNotifyRemove(h);
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyRemove(h);
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyRemove(h);
	}
}
void MyApp::OnNotifyUpdate(HZQDB h)
{
	MyModule::Broadcast([h](std::shared_ptr<MyModule> module)->bool {
		module->OnNotifyUpdate(h);
		return false;
	});
	if (MyLoginDlg::Inst()) {
		MyLoginDlg::Inst()->OnNotifyUpdate(h);
	}
	if (smartkbdlg_) {
		smartkbdlg_->OnNotifyUpdate(h);
	}
	if (frame_) {
		frame_->OnNotifyUpdate(h);
	}
	for (auto frame : tech_frames_) {
		frame->OnNotifyUpdate(h);
	}
	for (auto frame : calc_frames_) {
		frame->OnNotifyUpdate(h);
	}
}

void MyApp::OnPreNotify(HZQDB h, ZQDB_NOTIFY_TYPE notify)
{
	switch (notify)
	{
	case ZQDB_NOTIFY_ENABLE: {
		switch (h->type)
		{
		case ZQDB_HANDLE_TYPE_MODULE: {
		} break;
		default: {
			//
		} break;
		}
	} break;
	case ZQDB_NOTIFY_DISABLE: {
		switch (h->type)
		{
		case ZQDB_HANDLE_TYPE_MODULE: {
			
		} break;
		default: {
			//
		} break;
		}
	} break;
	case ZQDB_NOTIFY_APPEND: {
		
	} break;
	case ZQDB_NOTIFY_REMOVE: {

	} break;
	case ZQDB_NOTIFY_UPDATE: {

	} break;
	default:
		break;
	}
}

void MyApp::HandleNotify(HZQDB h, ZQDB_NOTIFY_TYPE notify)
{
	//处理前先引用资源
	switch (h->type)
	{
	case ZQDB_HANDLE_TYPE_CALCFUNC: {
		ZQDBCalcRef(h); //计算模块通知对象需要增加引用计数，以防止对象失效
	} break;
	}
	Post([this, h, notify]() {
		OnPreNotify(h, notify);
		ZQDBCalcUpdate(h, notify);
		OnNotify(h, notify);
		//处理后再资源释放
		switch (h->type)
		{
		case ZQDB_HANDLE_TYPE_CALCFUNC: {
			ZQDBCloseCalcFunc(h);
		} break;
		}
	});
}

void MyApp::OnNetStatus(HNNODE h, NET_NODE_STATUS status)
{
	NotifyBase::OnNetStatus(h, status);
}

int MyApp::OnNetMsg(zqdb::Msg& msg)
{
	int ret = 0;
	MyModule::Broadcast([&ret, &msg](std::shared_ptr<MyModule> module)->bool {
		ret = module->OnNetMsg(msg);
		if (ret) {
			return true;
		}
		return false;
	});
	if (ret) {
		return ret;
	}
	/*if (MyLoginDlg::Inst()) {
		ret = MyLoginDlg::Inst()->OnNetMsg(msg);
		if (ret) {
			return ret;
		}
	}*/
	return 0;
}

void MyApp::HandleNetStatus(HNNODE h, NET_NODE_STATUS status)
{
	//Post(std::bind(&MyApp::OnNetStatus, this, h, status));
	if (ZQDBIsServer()) {
		if (h == ZQDBGetSrvNode()) {
			switch (status)
			{
			case NET_NODE_STATUS_DISCONNECT: {
				Post(std::bind(&MyApp::OnNotifyStopListen, this));
			} break;
			case NET_NODE_STATUS_CONNECTED: {
				Post(std::bind(&MyApp::OnNotifyStartListen, this));
			} break;
			default:
				break;
			}
		}
	}
	else {
		if (h == ZQDBGetCliNode()) {
			switch (status)
			{
			case NET_NODE_STATUS_DISCONNECT: {
				Post(std::bind(&MyApp::OnNotifyDisconnect, this));
			} break;
			case NET_NODE_STATUS_CONNECTED: {
				Post(std::bind(&MyApp::OnNotifyConnect, this));
			} break;
			default:
				break;
			}
		}
	}
}

int MyApp::HandleNetMsg(HNMSG hmsg, size_t* flags)
{
	zqdb::Msg msg(hmsg, zqdb::Msg::Ref);
	Post(std::bind(&MyApp::OnNetMsg, this, msg));
	return 0;
}

int MyApp::OrderSend(HZQDB user, HZQDB code, char direction, char offset, char type, double volume, double price, HNMSG* rsp, size_t timeout, size_t flags)
{
	auto m = MyModule::GetByHandle(user);
	if (m) {
		return m->OrderSend(user, code, direction, offset, type, volume, price, rsp, timeout, flags);
	}
	return -1;
}

int MyApp::OrderCancel(HZQDB user, HZQDB order, HNMSG* rsp, size_t timeout, size_t flags)
{
	auto m = MyModule::GetByHandle(user);
	if (m) {
		return m->OrderCancel(user, order, rsp, timeout, flags);
	}
	return -1;
}

int MyApp::OrderClose(HZQDB user, HZQDB position, char type, double volume, double price, HNMSG* rsp, size_t timeout, size_t flags)
{
	auto m = MyModule::GetByHandle(user);
	if (m) {
		return m->OrderClose(user, position, type, volume, price, rsp, timeout, flags);
	}
	return -1;
}

void MyApp::ShowTips(const char* xml, size_t xmlflag)
{
	last_tips_ = FormatTips(xml, xmlflag);
	/*if (MyLoginDlg::Inst())
	{
		auto event = new wxStringCommandEvent(logstr, ZQDB_NOTIFY_LOG_UPDATE_EVENT);
		wxQueueEvent(MyLoginDlg::Inst()->GetEventHandler(), event);
	}*/
	if (frame_) {
		frame_->ShowTips(xml, xmlflag);
	}
	for (auto frame : tech_frames_) {
		frame->ShowTips(xml, xmlflag);
	}
}

