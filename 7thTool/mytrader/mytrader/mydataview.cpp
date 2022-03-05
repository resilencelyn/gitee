/////////////////////////////////////////////////////////////////////////////
// Name:        dataview.cpp
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "mydataview.h"

// a simple renderer that wraps each word on a new line
MyCodeViewListRenderer::MyCodeViewListRenderer()
	: wxDataViewCustomRenderer("void*", wxDATAVIEW_CELL_INERT, 0)
{
}

void MyCodeViewListRenderer::OnSkinInfoChanged()
{
	fontName_ = skin_info_ptr_->artProvider->GetFont(wxRIBBON_ART_TAB_LABEL_FONT);
	fontCode_ = skin_info_ptr_->artProvider->GetFont(wxRIBBON_ART_BUTTON_BAR_LABEL_FONT);
	fontPrice_ = skin_info_ptr_->artProvider->GetFont(wxRIBBON_ART_PANEL_LABEL_FONT);

	crName_ = skin_info_ptr_->artProvider->GetColor(wxRIBBON_ART_TAB_LABEL_COLOUR);
	crCode_ = skin_info_ptr_->artProvider->GetColor(wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR);
}

void MyCodeViewListRenderer::RenderBackground(wxDC* dc, const wxRect& rect)
{
	//这里可以改变非选中行背景
	//dc->SetBrush(*wxLIGHT_GREY_BRUSH);
	//dc->DrawRectangle(rect);
}

bool MyCodeViewListRenderer::Render(wxRect rect, wxDC *dc, int state)
{
	if (!val_) {
		return false;
	}

	//选中状态颜色像这样修改
	if (state & wxDATAVIEW_CELL_SELECTED) {
		//dc->SetBrush(*wxLIGHT_GREY_BRUSH);
		//dc->DrawRectangle(rect);
	}

	rect.Deflate(skin_info_ptr_->xySpace);

	//Name
	dc->SetFont(fontName_);
	dc->SetTextForeground(crName_);
	dc->DrawLabel(val_->Name.c_str(), rect, wxALIGN_LEFT | wxALIGN_TOP);
	//Code
	dc->SetFont(fontCode_);
	dc->SetTextForeground(crCode_);
	dc->DrawLabel(val_->Code.c_str(), rect, wxALIGN_LEFT | wxALIGN_BOTTOM);
	//RenderText(m_value, 0, rect, dc, state);

	zqdb::Code code((HZQDB)val_->Data);
	wxString strClose(wxT("——")), strZD(wxT("——")), strZDF(wxT("——"));
	double zd = 0.;
	bool invalid_flag = true;
	if (!ZQDBIsSubscribeMarketDataAll()) {
		if (code.IsSubscribe()) {
			invalid_flag = false;
		}
		else {
			strZDF = wxT("未订阅");
		}
	}
	else {
		invalid_flag = IsZeroValue(code->Volume);
	}
	if (!invalid_flag) {
		auto close = code->Close, yclose = IsInvalidValue(code->YSettle) ? code->YClose : code->YSettle;
		strClose = wxString::Format("%.2f", close);
		zd = close - yclose;
		double zdf = zd / yclose * 100;
		strZD = wxString::Format("%+.2f", zd);
		strZDF = wxString::Format("%+.2f%%", zdf);
	}

	dc->SetFont(fontPrice_);
	dc->SetTextForeground(skin_info_ptr_->GetCtrlColor(zd));
	dc->DrawLabel(strClose, rect, wxALIGN_RIGHT | wxALIGN_TOP);

	dc->SetFont(fontPrice_);
	dc->SetTextForeground(skin_info_ptr_->GetCtrlColor(zd));
	dc->DrawLabel(strZD, rect, wxALIGN_CENTER | wxALIGN_BOTTOM);

	dc->SetFont(fontPrice_);
	dc->SetTextForeground(skin_info_ptr_->GetCtrlColor(zd));
	dc->DrawLabel(strZDF, rect, wxALIGN_RIGHT | wxALIGN_BOTTOM);

	return true;
}

wxSize MyCodeViewListRenderer::GetSize() const
{
	wxSize txtSize;
	txtSize.x = -1;//skin_info_ptr_->xySpace.x + val_->Name.size() * skin_info_ptr_->xyName.x + skin_info_ptr_->xySpace.x;
	txtSize.y = skin_info_ptr_->xySpace.y + skin_info_ptr_->xyText.y + skin_info_ptr_->xySpace.y + skin_info_ptr_->xyText.y + skin_info_ptr_->xySpace.y;
	return txtSize;
}

bool MyCodeViewListRenderer::SetValue(const wxVariant &value)
{
	val_ = (SmartKBItem*)value.GetVoidPtr();
	return true;
}

bool MyCodeViewListRenderer::GetValue(wxVariant &WXUNUSED(value)) const
{
	return true;
}

#if wxUSE_ACCESSIBILITY
wxString MyCodeViewListRenderer::GetAccessibleDescription() const
{
	return val_->Name.c_str();
}
#endif // wxUSE_ACCESSIBILITY

///

// a simple renderer that wraps each word on a new line
MyCodeListCodeRenderer::MyCodeListCodeRenderer()
	: wxDataViewCustomRenderer("void*", wxDATAVIEW_CELL_INERT, 0)
{ 
}

bool MyCodeListCodeRenderer::Render(wxRect rect, wxDC *dc, int state)
{
	//Name
	dc->SetFont(skin_info_ptr_->fontText);
	dc->SetTextForeground(skin_info_ptr_->crCtrlForgnd);
	dc->DrawLabel(code_, rect, wxALIGN_LEFT | wxALIGN_TOP);
	//Code
	dc->SetFont(skin_info_ptr_->fontTitle);
	dc->SetTextForeground(skin_info_ptr_->crCtrlForgnd);
	dc->DrawLabel(name_, rect, wxALIGN_LEFT | wxALIGN_BOTTOM);
	//RenderText(m_value, 0, rect, dc, state);
	return true;
}

wxSize MyCodeListCodeRenderer::GetSize() const
{
	wxSize txtSize;
	txtSize.x = name_.size() * skin_info_ptr_->xyText.x;
	txtSize.y = skin_info_ptr_->xyText.y + skin_info_ptr_->xyTitle.y;
	return txtSize;
}

bool MyCodeListCodeRenderer::SetValue(const wxVariant &value)
{
	wxCSConv utf8cv(wxFONTENCODING_UTF8);

	h_ = (HZQDB)value.GetVoidPtr();
	auto info = (CODEINFO*)ZQDBGetValue(h_);
	name_ = utf8cv.cMB2WX(info->Name);//value.GetString();
	code_ = utf8cv.cMB2WX(info->Code);
	return true;
}

bool MyCodeListCodeRenderer::GetValue(wxVariant &WXUNUSED(value)) const 
{ 
	return true; 
}

#if wxUSE_ACCESSIBILITY
wxString MyCodeListCodeRenderer::GetAccessibleDescription() const
{
	return name_;
}
#endif // wxUSE_ACCESSIBILITY

///

MyCodeListNameRenderer::MyCodeListNameRenderer()
	: wxDataViewCustomRenderer("void*", wxDATAVIEW_CELL_INERT, 0)
{ 

}

bool MyCodeListNameRenderer::Render(wxRect rect, wxDC *dc, int state)
{
	if (!info_ || IsInvalidValue(info_->Close) || IsInvalidValue(info_->YClose)) {
		return false;
	}
	//
	//Price
	//change zdf
	//
	double price = info_->Close;
	double change = info_->Close - info_->YClose;
	double zdf = change / info_->YClose * 100;
	//Price
	wxString strPrice = wxString::Format("%.2f", price);
	dc->SetFont(skin_info_ptr_->fontText);
	dc->SetTextForeground(skin_info_ptr_->GetColor(change));
	dc->DrawLabel(strPrice, rect, wxALIGN_LEFT | wxALIGN_TOP);
	//change
	wxString strChange = wxString::Format("%.2f", change);
	dc->SetFont(skin_info_ptr_->fontTitle);
	dc->SetTextForeground(skin_info_ptr_->GetColor(change));
	dc->DrawLabel(strChange, rect, wxALIGN_LEFT | wxALIGN_BOTTOM);
	//zdf
	wxString strZDF = wxString::Format("%.2f", change);
	dc->SetFont(skin_info_ptr_->fontTitle);
	dc->SetTextForeground(skin_info_ptr_->GetColor(change));
	dc->DrawLabel(strZDF, rect, wxALIGN_RIGHT | wxALIGN_BOTTOM);
	//RenderText(m_value, 0, rect, dc, state);
	return true;
}

wxSize MyCodeListNameRenderer::GetSize() const
{
	wxSize txtSize;
	txtSize.x = -1;
	txtSize.y = skin_info_ptr_->xyText.y + skin_info_ptr_->xyTitle.y;
	return txtSize;
}

bool MyCodeListNameRenderer::SetValue(const wxVariant &value)
{
	wxCSConv utf8cv(wxFONTENCODING_UTF8);

	h_ = (HZQDB)value.GetVoidPtr();
	info_ = (CODEINFO*)ZQDBGetValue(h_);
	return true;
}

bool MyCodeListNameRenderer::GetValue(wxVariant &WXUNUSED(value)) const 
{ 
	return true;
}

#if wxUSE_ACCESSIBILITY
wxString MyCodeListNameRenderer::GetAccessibleDescription() const
{
	return "";
}
#endif // wxUSE_ACCESSIBILITY
