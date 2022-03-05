#include "settings.h"
#include <techdlg.h>

#include "transparent.xpm"
#include "transparent16.xpm"
//#include "zoom_in.xpm"
//#include "zoom_out.xpm"


SkinInfo::SkinInfo()
{
	bmpTransparent16 = wxBitmap(transparent16_xpm);
	bmpTransparent32 = wxBitmap(transparent_xpm);
	for (int cycle = CYC_MIN; cycle < CYC_MAX; cycle++)
	{
		wxString str = ZQDBCycle2Str((PERIODTYPE)cycle);
		bmp16_[str] = wxBitmap(16, 16);
	}
	boost::system::error_code ec;
	for (boost::filesystem::directory_iterator it("./res", ec); it != boost::filesystem::directory_iterator(); ++it) {
		const auto& path = it->path();
		if (boost::filesystem::is_regular(path, ec)) {
			wxBitmap bmp;
			if (path.extension() == ".bmp") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_BMP);
			}
			else if (path.extension() == ".ico") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_ICO);
			}
			else if (path.extension() == ".cur") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_CUR);
			}
			else if (path.extension() == ".xbm") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_XBM);
			}
			else if (path.extension() == ".xpm") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_XPM);
			}
			else if (path.extension() == ".tif" || path.extension() == ".tiff") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_TIFF);
			}
			else if (path.extension() == ".gif") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_GIF);
			}
			else if (path.extension() == ".png") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_PNG);
			}
			else if (path.extension() == ".jpg" || path.extension() == ".jpeg") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_JPEG);
			}
			else if (path.extension() == ".pnm") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_PNM);
			}
			else if (path.extension() == ".pcx") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_PCX);
			}
			else if (path.extension() == ".pict") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_PICT);
			}
			else if (path.extension() == ".icon") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_ICON);
			}
			else if (path.extension() == ".ani") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_ANI);
			}
			else if (path.extension() == ".iff") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_IFF);
			}
			else if (path.extension() == ".tga") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_TGA);
			}
			else if (path.extension() == ".maccursor") {
				bmp = wxBitmap(path.wstring().c_str(), wxBITMAP_TYPE_MACCURSOR);
			}
			if (bmp.IsOk()) {
				if (path.stem() == wxT("Ïòµ¼")) {
					bmpWizard = bmp;
				}
				else {
					wxSize sz = bmp.GetSize();
					//if (sz.x == sz.y) {
					if (sz.x == 16) {
						bmp16_[path.stem().wstring().c_str()] = std::move(bmp);
					}
					else if (sz.x == 32) {
						bmp32_[path.stem().wstring().c_str()] = std::move(bmp);
					}
					//}
				}
			}
		}
	}
}

SkinInfo::~SkinInfo()
{
}

// const wxColour& SkinInfo::GetUpDownColor(int nUpDown)
// {
// 	if (nUpDown < 0) {
// 		return crFalling;
// 	}
// 	else if (nUpDown > 0) {
// 		return crRising;
// 	}
// 	return crText;
// }

// const wxColour& SkinInfo::GetUpDownLineColor(int nUpDown)
// {
// 	if (nUpDown < 0) {
// 		return crFallingLine;
// 	}
// 	else if (nUpDown > 0) {
// 		return crRisingLine;
// 	}
// 	return crLine;
// }

// const wxPen& SkinInfo::GetUpDownPen(int nUpDown)
// {
// 	if (nUpDown < 0) {
// 		return penFalling;
// 	}
// 	else if (nUpDown > 0) {
// 		return penRising;
// 	}
// 	return pen;
// }

// const wxPen& SkinInfo::GetUpDownLinePen(int nUpDown)
// {
// 	if (nUpDown < 0) {
// 		return penFalling;
// 	}
// 	else if (nUpDown > 0) {
// 		return penRising;
// 	}
// 	return pen;
// }

// const wxBrush& SkinInfo::GetUpDownBrush(int nUpDown)
// {
// 	if (nUpDown < 0) {
// 		return brushFalling;
// 	}
// 	else if (nUpDown > 0) {
// 		return brushRising;
// 	}
// 	return brush;
// }

// const wxBrush& SkinInfo::GetUpDownLineBrush(int nUpDown)
// {
// 	if (nUpDown < 0) {
// 		return brushFalling;
// 	}
// 	else if (nUpDown > 0) {
// 		return brushRising;
// 	}
// 	return brush;
// }

void SkinInfo::TextToBitmap16(wxBitmap& bmp, const wxString& text, wxFont font, wxColour color)
{
	// use a memory DC to draw directly onto the bitmap
	//wxBitmap bmp1(32,32);
	wxMemoryDC memDc(bmp);

	auto szBmp = bmp.GetSize();
	const wxRect rcBmp(0, 0, szBmp.x, szBmp.y);

	memDc.SetBackground(*wxRED_BRUSH);
	//memDc.SetBackgroundMode(wxTRANSPARENT);
	memDc.Clear();

	//memDc.SetBrush(*wxTRANSPARENT_BRUSH);
	//memDc.DrawRectangle(rcBmp);

	memDc.SetFont(font);
	memDc.SetTextForeground(color);
	memDc.DrawLabel(text, rcBmp, wxALIGN_CENTER);

	memDc.SetFont(wxNullFont);
	memDc.SetBrush(wxNullBrush);
	memDc.SelectObject(wxNullBitmap);

	/*wxMemoryDC memDc2(bmp);
	memDc2.StretchBlit(0, 0, 16, 16, &memDc, 0, 0, 32, 32);
	memDc2.SelectObject(wxNullBitmap);*/

	wxMask *mask = new wxMask(bmp, *wxRED);
	bmp.SetMask(mask);
	//wxImage image = bmp.ConvertToImage();
	//image.InitAlpha();
	//return wxBitmap(image);
	//return bmp;
}

void SkinInfo::Save(const wxBitmap& bmp)
{
#if wxUSE_FILEDLG
	wxImage image = bmp.ConvertToImage();

	wxString savefilename = wxFileSelector("Save Image",
		wxEmptyString,
		wxEmptyString,
		wxEmptyString,
		"BMP files (*.bmp)|*.bmp|"
#if wxUSE_LIBPNG
		"PNG files (*.png)|*.png|"
#endif
#if wxUSE_LIBJPEG
		"JPEG files (*.jpg)|*.jpg|"
#endif
#if wxUSE_GIF
		"GIF files (*.gif)|*.gif|"
#endif
#if wxUSE_LIBTIFF
		"TIFF files (*.tif)|*.tif|"
#endif
#if wxUSE_PCX
		"PCX files (*.pcx)|*.pcx|"
#endif
#if wxUSE_XPM
		"X PixMap files (*.xpm)|*.xpm|"
#endif
		"ICO files (*.ico)|*.ico|"
		"CUR files (*.cur)|*.cur",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
		nullptr);

	if (savefilename.empty())
		return;

	wxString extension;
	wxFileName::SplitPath(savefilename, NULL, NULL, &extension);

	bool saved = false;
	if (extension == "bmp")
	{
		static const int bppvalues[] =
		{
			wxBMP_1BPP,
			wxBMP_1BPP_BW,
			wxBMP_4BPP,
			wxBMP_8BPP,
			wxBMP_8BPP_GREY,
			wxBMP_8BPP_RED,
#if wxUSE_PALETTE
			wxBMP_8BPP_PALETTE,
#endif // wxUSE_PALETTE
			wxBMP_24BPP
		};

		const wxString bppchoices[] =
		{
			"1 bpp color",
			"1 bpp B&W",
			"4 bpp color",
			"8 bpp color",
			"8 bpp greyscale",
			"8 bpp red",
#if wxUSE_PALETTE
			"8 bpp own palette",
#endif // wxUSE_PALETTE
			"24 bpp"
		};

		int bppselection = wxGetSingleChoiceIndex("Set BMP BPP",
			"Image sample: save file",
			WXSIZEOF(bppchoices),
			bppchoices,
			nullptr);
		if (bppselection != -1)
		{
			int format = bppvalues[bppselection];
			image.SetOption(wxIMAGE_OPTION_BMP_FORMAT, format);
#if wxUSE_PALETTE
			if (format == wxBMP_8BPP_PALETTE)
			{
				unsigned char *cmap = new unsigned char[256];
				for (int i = 0; i < 256; i++)
					cmap[i] = (unsigned char)i;
				image.SetPalette(wxPalette(256, cmap, cmap, cmap));

				delete[] cmap;
			}
#endif // wxUSE_PALETTE
		}
	}
#if wxUSE_LIBPNG
	else if (extension == "png")
	{
		static const int pngvalues[] =
		{
			wxPNG_TYPE_COLOUR,
			wxPNG_TYPE_COLOUR,
			wxPNG_TYPE_GREY,
			wxPNG_TYPE_GREY,
			wxPNG_TYPE_GREY_RED,
			wxPNG_TYPE_GREY_RED,
		};

		const wxString pngchoices[] =
		{
			"Colour 8bpp",
			"Colour 16bpp",
			"Grey 8bpp",
			"Grey 16bpp",
			"Grey red 8bpp",
			"Grey red 16bpp",
		};

		int sel = wxGetSingleChoiceIndex("Set PNG format",
			"Image sample: save file",
			WXSIZEOF(pngchoices),
			pngchoices,
			nullptr);
		if (sel != -1)
		{
			image.SetOption(wxIMAGE_OPTION_PNG_FORMAT, pngvalues[sel]);
			image.SetOption(wxIMAGE_OPTION_PNG_BITDEPTH, sel % 2 ? 16 : 8);

			// these values are taken from OptiPNG with -o3 switch
			const wxString compressionChoices[] =
			{
				"compression = 9, memory = 8, strategy = 0, filter = 0",
				"compression = 9, memory = 9, strategy = 0, filter = 0",
				"compression = 9, memory = 8, strategy = 1, filter = 0",
				"compression = 9, memory = 9, strategy = 1, filter = 0",
				"compression = 1, memory = 8, strategy = 2, filter = 0",
				"compression = 1, memory = 9, strategy = 2, filter = 0",
				"compression = 9, memory = 8, strategy = 0, filter = 5",
				"compression = 9, memory = 9, strategy = 0, filter = 5",
				"compression = 9, memory = 8, strategy = 1, filter = 5",
				"compression = 9, memory = 9, strategy = 1, filter = 5",
				"compression = 1, memory = 8, strategy = 2, filter = 5",
				"compression = 1, memory = 9, strategy = 2, filter = 5",
			};

			sel = wxGetSingleChoiceIndex("Select compression option (Cancel to use default)\n",
				"PNG Compression Options",
				WXSIZEOF(compressionChoices),
				compressionChoices,
				nullptr);
			if (sel != -1)
			{
				const int zc[] = { 9, 9, 9, 9, 1, 1, 9, 9, 9, 9, 1, 1 };
				const int zm[] = { 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9 };
				const int zs[] = { 0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2 };
				const int f[] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
					0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8 };

				image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL, zc[sel]);
				image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL, zm[sel]);
				image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY, zs[sel]);
				image.SetOption(wxIMAGE_OPTION_PNG_FILTER, f[sel]);
				image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE, 1048576); // 1 MB
			}
		}
	}
#endif // wxUSE_LIBPNG
	else if (extension == "cur")
	{
		image.Rescale(32, 32);
		image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
		image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
		// This shows how you can save an image with explicitly
		// specified image format:
		saved = image.SaveFile(savefilename, wxBITMAP_TYPE_CUR);
	}

	if (!saved)
	{
		// This one guesses image format from filename extension
		// (it may fail if the extension is not recognized):
		image.SaveFile(savefilename);
	}
#endif // wxUSE_FILEDLG
}

const wxBitmap& SkinInfo::GetBitmap16(const wxString& name)
{
	auto it = bmp16_.find(name);
	if (it != bmp16_.end()) {
		return it->second;
	}
	return bmpTransparent16;
}

void SkinInfo::SetBitmap16(const wxString& name, const wxBitmap& bmp)
{
	bmp16_[name] = bmp;
}

void SkinInfo::UpdateTechCycleBitmap16()
{
	auto win = wxGetActiveWindow();
	auto afont = GetArtFont(wxRIBBON_ART_TAB_LABEL_FONT);
	//auto color = GetArtColor(wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR);
	wxColor color(112, 130, 163);
	wxFontInfo info(wxWindow::ToDIP(12, win)/*afont.GetPointSize()*/); info.Family(afont.GetFamily()).AntiAliased();
	wxFont font(info);
	for (int cycle = CYC_MIN; cycle < CYC_MAX; cycle++)
	{
		switch (cycle)
		{
		case CYC_ANYSEC:
		case CYC_ANYMIN:
			break;
		default: {
			wxString str = ZQDBCycle2Str((PERIODTYPE)cycle);
			TextToBitmap16(bmp16_[str], str, font, color);
		} break;
		}
	}
}

void SkinInfo::UpdateTechCycleExBitmap16(PERIODTYPE cycle, size_t cycleex)
{
	auto afont = GetArtFont(wxRIBBON_ART_TAB_LABEL_FONT);
	//auto color = GetArtColor(wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR);
	wxColor color(112, 130, 163);
	wxFontInfo info(afont.GetPointSize()); info.Family(afont.GetFamily()).AntiAliased();
	wxFont font(info);
	wxString str = ZQDBCycle2Str((PERIODTYPE)cycle);
	switch (cycle)
	{
	case CYC_ANYSEC: {
		TextToBitmap16(bmp16_[str], wxString::Format(_("S%zu"), cycleex), font, color);
	} break;
	case CYC_ANYMIN: {
		if (cycleex % 60 == 0) {
			TextToBitmap16(bmp16_[str], wxString::Format(_("H%zu"), cycleex / 60), font, color);
		}
		else {
			TextToBitmap16(bmp16_[str], wxString::Format(_("M%zu"), cycleex), font, color);
		}
	} break;
	}
}

const wxBitmap& SkinInfo::GetBitmap32(const wxString& name)
{
	auto it = bmp32_.find(name);
	if (it != bmp32_.end()) {
		return it->second;
	}
	return bmpTransparent32;
}

ConfigInfo::ConfigInfo()
{
	
}

void ConfigInfo::Init()
{
	auto config_ = wxConfigBase::Get();
}

void ConfigInfo::SetFrameOnCloseAction(int action)
{
	auto config_ = wxConfigBase::Get();
	config_->Write(CONFIG_FRAME_ON_CLOSE_ACTION, action);
}

int ConfigInfo::GetFrameOnCloseAction()
{
	auto config_ = wxConfigBase::Get();
	return config_->ReadLong(CONFIG_FRAME_ON_CLOSE_ACTION, 0);
}

void ConfigInfo::SetFrameOnCloseRember(bool rember)
{
	auto config_ = wxConfigBase::Get();
	config_->Write(CONFIG_FRAME_ON_CLOSE_REMBER, rember);
}

bool ConfigInfo::GetFrameOnCloseRember()
{
	auto config_ = wxConfigBase::Get();
	return config_->ReadBool(CONFIG_FRAME_ON_CLOSE_REMBER, false);
}

void ConfigInfo::SetFrameLastFilter(const wxString& filter)
{
	auto config_ = wxConfigBase::Get();
	config_->Write(CONFIG_FRAME_LAST_FILTER, filter);
}

wxString ConfigInfo::GetFrameLastFilter()
{
	auto config_ = wxConfigBase::Get();
	return config_->Read(CONFIG_FRAME_LAST_FILTER);
}

void ConfigInfo::SetFrameLastSortType(int type)
{
	auto config_ = wxConfigBase::Get();
	config_->Write(CONFIG_FRAME_LAST_SORT_TYPE, type);
}

int ConfigInfo::GetFrameLastSortType()
{
	auto config_ = wxConfigBase::Get();
	return config_->ReadLong(CONFIG_FRAME_LAST_SORT_TYPE, 0);
}

void ConfigInfo::SetFrameLastSort(int sort)
{
	auto config_ = wxConfigBase::Get();
	config_->Write(CONFIG_FRAME_LAST_SORT, sort);
}

int ConfigInfo::GetFrameLastSort()
{
	auto config_ = wxConfigBase::Get();
	return config_->ReadLong(CONFIG_FRAME_LAST_SORT, 0);
}

wxString ConfigInfo::GetFilterSubscribe()
{
	auto config_ = wxConfigBase::Get();
	return config_->Read(CONFIG_FILTER_SUBSCRIBE);
}

void ConfigInfo::SetFilterSubscribe(const wxString& sub)
{
	auto config_ = wxConfigBase::Get();
	config_->Write(CONFIG_FILTER_SUBSCRIBE, sub);
}

void ConfigInfo::SetFilterCalcFunc(const wxString& name)
{
	zqdb::SetFilterCalcFunc(name);
}

wxString ConfigInfo::GetFilterCalcFunc()
{
	return zqdb::GetFilterCalcFunc();
}

size_t ConfigInfo::GetSortQuick()
{
	auto config_ = wxConfigBase::Get();
	return config_->ReadLong(CONFIG_SORT_QUICK, 60);
}

void ConfigInfo::SetSortQuick(size_t size_t)
{
	auto config_ = wxConfigBase::Get();
	config_->Write(CONFIG_SORT_QUICK, (int)size_t);
}

void ConfigInfo::SetSortCalcFunc(const wxString& name)
{
	zqdb::SetSortCalcFunc(name);
}

wxString ConfigInfo::GetSortCalcFunc()
{
	return zqdb::GetSortCalcFunc();
}

void ConfigInfo::SetTechKScale(size_t scale)
{
	zqdb::SetTechKScale(scale);
}

size_t ConfigInfo::GetTechKScale()
{
	return zqdb::GetTechKScale();
}

void ConfigInfo::SetTechKType(CALC_KLINE_TYPE type)
{
	zqdb::SetTechKType(type);
}

CALC_KLINE_TYPE ConfigInfo::GetTechKType()
{
	return zqdb::GetTechKType();
}

void ConfigInfo::SetTechCycleCur(PERIODTYPE cycle)
{
	zqdb::SetTechCycleCur(cycle);
}

size_t ConfigInfo::GetTechCycleCur()
{
	return zqdb::GetTechCycleCur();
}

void ConfigInfo::SetTechCycleAnySec(int sec)
{
	zqdb::SetTechCycleAnySec(sec);
}

size_t ConfigInfo::GetTechCycleAnySec()
{
	return zqdb::GetTechCycleAnySec();
}

void ConfigInfo::SetTechCycleAnyMin(int min)
{
	zqdb::SetTechCycleAnyMin(min);
}

size_t ConfigInfo::GetTechCycleAnyMin()
{
	return zqdb::GetTechCycleAnyMin();
}