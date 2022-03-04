#pragma once

#include "utility.h"
#include <techview.h>

class SkinInfo : public zqdb::SkinInfo
{
public:
	SkinInfo();
	~SkinInfo();

	//鼠标
	
	//颜色
	wxColour crPrimary;			//主色调（背景色调）
	wxColour crSecondary;		//次色调（高亮色调）
	wxColour crTertiary;		//第三色调（文字色调）

	wxColour crViewBkgnd;		//视图色（二级窗口）
	wxColour crViewForgnd;		//视图文字
	wxColour crCtrlBkgnd;		//控件色（三级窗口）
	wxColour crCtrlForgnd;		//视图文字

	wxColour crCtrlText; //平
	wxColour crCtrlRising; //涨
	wxColour crCtrlFalling; //跌

	inline wxColour GetCtrlColor(double val) {
		if (val > 0.) {
			return crCtrlRising;
		}
		else if (val < 0.) {
			return crCtrlFalling;
		}
		return crCtrlText;
	}
	
	//画笔

	//画刷

	//字体

	//字体高宽

	//窗体高宽（相对于水平或者垂直布局）

	//图片
	wxBitmap bmpWizard;
	static void TextToBitmap16(wxBitmap& bmp, const wxString& text, wxFont font, wxColour color);
	void Save(const wxBitmap& bmp);
	wxBitmap bmpTransparent16; //透明图标
	wxBitmap bmpTransparent32; //透明图标
	std::map<wxString, wxBitmap> bmp16_;
	std::map<wxString, wxBitmap> bmp32_;
	const wxBitmap& GetBitmap16(const wxString& name);
	void SetBitmap16(const wxString& name, const wxBitmap& bmp);
	void UpdateTechCycleBitmap16();
	void UpdateTechCycleExBitmap16(PERIODTYPE cycle, size_t cycleex);
	const wxBitmap& GetBitmap32(const wxString& name);
};

#define CONFIG_FRAME_ON_CLOSE_ACTION wxT("frame/close_action")
#define CONFIG_FRAME_ON_CLOSE_REMBER wxT("frame/close_rember")
#define CONFIG_FRAME_LAST_FILTER wxT("frame/last_filter")
#define CONFIG_FRAME_LAST_SORT_TYPE wxT("frame/last_sort_type")
#define CONFIG_FRAME_LAST_SORT wxT("frame/last_sort")

#define CONFIG_FILTER_SUBSCRIBE wxT("filter/subscribe")

#define CONFIG_SORT_QUICK wxT("sort/quick")

class ConfigInfo
{
public:
	ConfigInfo();

	void Init();

	void SetFrameOnCloseAction(int action);
	int GetFrameOnCloseAction();
	void SetFrameOnCloseRember(bool rember);
	bool GetFrameOnCloseRember();
	void SetFrameLastFilter(const wxString& filter);
	wxString GetFrameLastFilter();
	void SetFrameLastSortType(int type);
	int GetFrameLastSortType();
	void SetFrameLastSort(int sort);
	int GetFrameLastSort();

	wxString GetFilterSubscribe();
	void SetFilterSubscribe(const wxString& sub);

	void SetFilterCalcFunc(const wxString& name);
	wxString GetFilterCalcFunc();

	void SetSortQuick(size_t duration);
	size_t GetSortQuick();
	void SetSortCalcFunc(const wxString& name);
	wxString GetSortCalcFunc();

	void SetTechKScale(size_t scale);
	size_t GetTechKScale();
	void SetTechKType(CALC_KLINE_TYPE type);
	CALC_KLINE_TYPE GetTechKType();
	void SetTechCycleCur(PERIODTYPE cycle);
	size_t GetTechCycleCur();
	void SetTechCycleAnySec(int sec);
	size_t GetTechCycleAnySec();
	void SetTechCycleAnyMin(int min);
	size_t GetTechCycleAnyMin();
};

template<class T>
class SettingsMap 
	: public ConfigInfo
	, public zqdb::SkinMap<T,SkinInfo>
{
	typedef ConfigInfo Base;
	typedef zqdb::SkinMap<T, SkinInfo> SkinBase;
protected:
	wxLanguage lang_ = wxLANGUAGE_CHINESE_SIMPLIFIED;  // language specified by user
	wxLocale locale_;  // locale we'll be using
public:

	void Init()
	{
		Base::Init();

		T* pT = static_cast<T*>(this);

		// don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
		// false just because it failed to load wxstd catalog
		if (!locale_.Init(lang_, wxLOCALE_DONT_LOAD_DEFAULT)) {
			wxLogWarning(_("This language is not supported by the system."));
			// continue nevertheless
		}

		//wxLocale::AddCatalogLookupPathPrefix(wxT("."));
		wxLocale::AddCatalogLookupPathPrefix(wxGetCwd() + wxFILE_SEP_PATH + wxT("locale"));
		//wxLocale::AddCatalogLookupPathPrefix(pT->GetAppDir() + wxFILE_SEP_PATH + wxT("locale"));

		// Initialize the catalogs we'll be using
		const wxLanguageInfo* pInfo = wxLocale::GetLanguageInfo(lang_);
		if (!locale_.AddCatalog(pT->GetAppName())) {
			wxLogError(_("Couldn't find/load the '%s' catalog for locale '%s'."),
				pT->GetAppName(), pInfo ? pInfo->GetLocaleName() : _("unknown"));
		}

		//add zqdb.mo
		locale_.AddCatalog("zqdb");

		// Now try to add wxstd.mo so that loading "NOTEXIST.ING" file will produce
		// a localized error message:
		locale_.AddCatalog("wxstd");
		// NOTE: it's not an error if we couldn't find it!

		// this catalog is installed in standard location on Linux systems and
		// shows that you may make use of the standard message catalogs as well
		//
		// if it's not installed on your system, it is just silently ignored
#ifdef __LINUX__
		{
			wxLogNull noLog;
			locale_.AddCatalog("fileutils");
		}
#endif

		wxLogInfo(_("Language: %s\nSystem locale name: %s\nCanonical locale name: %s\n"), locale_.GetLocale(), locale_.GetSysName(), locale_.GetCanonicalName());
	}

	void Clear()
	{
		SkinBase::Clear();
	}
	inline bool IsDispOk() const {
		return SkinBase::IsDispOk();
	}

	void OnLangChanged() 
	{

	}
	void SetLang(wxLanguage lang) {
		T* pT = static_cast<T*>(this);
		if (lang_ != lang) {
			lang_ = lang;
			pT->OnLangChanged();
		}
	}
};
