/////////////////////////////////////////////////////////////////////////////
// Name:        mymodels.cpp
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "mydatamodel.h"
#include "zqstr.hpp"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#include "null.xpm"
#include "wx_small.xpm"

// ----------------------------------------------------------------------------
// MyListModel
// ----------------------------------------------------------------------------

static int my_sort_reverse( int *v1, int *v2 )
{
   return *v2-*v1;
}

static int my_sort( int *v1, int *v2 )
{
   return *v1-*v2;
}

#define INITIAL_NUMBER_OF_ITEMS 10000

MyListModel::MyListModel() :
        wxDataViewVirtualListModel( INITIAL_NUMBER_OF_ITEMS )
{
    // the first 100 items are really stored in this model;
    // all the others are synthesized on request
    static const unsigned NUMBER_REAL_ITEMS = 100;

    m_toggleColValues.reserve(NUMBER_REAL_ITEMS);
    m_textColValues.reserve(NUMBER_REAL_ITEMS);
    m_toggleColValues.push_back(false);
    m_textColValues.push_back("first row with long label to test ellipsization");
    for (unsigned int i = 1; i < NUMBER_REAL_ITEMS; i++)
    {
        m_toggleColValues.push_back(false);
        m_textColValues.push_back(wxString::Format("real row %d", i));
    }

    m_iconColValues.assign(NUMBER_REAL_ITEMS, "test");

    m_icon[0] = wxIcon( null_xpm );
    m_icon[1] = wxIcon( wx_small_xpm );
}

void MyListModel::Prepend( const wxString &text )
{
    m_toggleColValues.insert( m_toggleColValues.begin(), 0 );
    m_textColValues.Insert( text, 0 );
    RowPrepended();
}

void MyListModel::DeleteItem( const wxDataViewItem &item )
{
    unsigned int row = GetRow( item );

    if (row >= m_toggleColValues.size())
        return;

    m_toggleColValues.erase( m_toggleColValues.begin()+row );

    if (row >= m_textColValues.GetCount())
        return;

    m_textColValues.RemoveAt( row );
    RowDeleted( row );
}

void MyListModel::DeleteItems( const wxDataViewItemArray &items )
{
    unsigned i;
    wxArrayInt rows;
    for (i = 0; i < items.GetCount(); i++)
    {
        unsigned int row = GetRow( items[i] );
        if (row < m_textColValues.GetCount())
        {
            wxASSERT(row < m_toggleColValues.size());
            rows.Add( row );
        }
    }

    if (rows.GetCount() == 0)
    {
        // none of the selected items were in the range of the items
        // which we store... for simplicity, don't allow removing them
        wxLogError( "Cannot remove rows with an index greater than %u", unsigned(m_textColValues.GetCount()) );
        return;
    }

    // Sort in descending order so that the last
    // row will be deleted first. Otherwise the
    // remaining indeces would all be wrong.
    rows.Sort( my_sort_reverse );
    for (i = 0; i < rows.GetCount(); i++)
    {
        m_toggleColValues.erase( m_toggleColValues.begin()+rows[i] );
        m_textColValues.RemoveAt( rows[i] );
    }

    // This is just to test if wxDataViewCtrl can
    // cope with removing rows not sorted in
    // descending order
    rows.Sort( my_sort );
    RowsDeleted( rows );
}

void MyListModel::AddMany()
{
    Reset( GetCount()+1000 );
}

void MyListModel::GetValueByRow( wxVariant &variant,
                                 unsigned int row, unsigned int col ) const
{
    switch ( col )
    {
        case Col_Toggle:
            if (row >= m_toggleColValues.size())
                variant = false;
            else
                variant = m_toggleColValues[row];
            break;

        case Col_EditableText:
            if (row >= m_textColValues.GetCount())
                variant = wxString::Format( "virtual row %d", row );
            else
                variant = m_textColValues[ row ];
            break;

        case Col_IconText:
            {
                wxString text;
                if ( row >= m_iconColValues.GetCount() )
                    text = "virtual icon";
                else
                    text = m_iconColValues[row];

                variant << wxDataViewIconText(text, m_icon[row % 2]);
            }
            break;

        case Col_Date:
            variant = wxDateTime(1, wxDateTime::Jan, 2000).Add(wxTimeSpan(row));
            break;

        case Col_TextWithAttr:
            {
                static const char *labels[5] =
                {
                    // These strings will look wrong without wxUSE_MARKUP, but
                    // it's just a sample, so we don't care.
                    "<span color=\"#87ceeb\">light</span> and "
                        "<span color=\"#000080\">dark</span> blue",
                    "<big>growing green</big>",
                    "<i>emphatic &amp; red</i>",
                    "<b>bold &amp;&amp; cyan</b>",
                    "<small><tt>dull default</tt></small>",
                };

                variant = labels[row % 5];
            }
            break;

        case Col_Custom:
            {
                IntToStringMap::const_iterator it = m_customColValues.find(row);
                if ( it != m_customColValues.end() )
                    variant = it->second;
                else
                    variant = wxString::Format("%d", row % 100);
            }
            break;

        case Col_Max:
            wxFAIL_MSG( "invalid column" );
    }
}

bool MyListModel::GetAttrByRow( unsigned int row, unsigned int col,
                                wxDataViewItemAttr &attr ) const
{
    switch ( col )
    {
        case Col_Toggle:
            return false;

        case Col_EditableText:
        case Col_Date:
            if (row < m_toggleColValues.size())
            {
                if (m_toggleColValues[row])
                {
                    attr.SetColour( wxColour( *wxLIGHT_GREY ) );
                    attr.SetStrikethrough( true );
                    return true;
                }
            }
            return false;

        case Col_IconText:
            if ( !(row % 2) )
                return false;
            attr.SetColour(*wxYELLOW);
            attr.SetBackgroundColour(*wxLIGHT_GREY);
            break;

        case Col_TextWithAttr:
            if (row < m_toggleColValues.size())
            {
                if (m_toggleColValues[row])
                {
                    attr.SetColour( wxColour( *wxLIGHT_GREY ) );
                    attr.SetStrikethrough( true );
                    return true;
                }
            }
            wxFALLTHROUGH;

        case Col_Custom:
            // do what the labels defined in GetValueByRow() hint at
            switch ( row % 5 )
            {
                case 0:
                    attr.SetColour(*wxBLUE);
                    break;

                case 1:
                    attr.SetColour(*wxGREEN);
                    break;

                case 2:
                    attr.SetColour(*wxRED);
                    break;

                case 3:
                    attr.SetColour(*wxCYAN);
                    attr.SetBold(true);
                    break;

                case 4:
                    return false;
            }
            break;

        case Col_Max:
            wxFAIL_MSG( "invalid column" );
    }

    return true;
}

bool MyListModel::SetValueByRow( const wxVariant &variant,
                                 unsigned int row, unsigned int col )
{
    switch ( col )
    {
        case Col_Toggle:
            if (row >= m_toggleColValues.size())
                return false;

            m_toggleColValues[row] = variant.GetBool();
            return true;

        case Col_EditableText:
        case Col_IconText:
            if (row >= m_textColValues.GetCount())
            {
                // the item is not in the range of the items
                // which we store... for simplicity, don't allow editing it
                wxLogError( "Cannot edit rows with an index greater than %d",
                            m_textColValues.GetCount() );
                return false;
            }

            if ( col == Col_EditableText )
            {
                m_textColValues[row] = variant.GetString();
            }
            else // col == Col_IconText
            {
                wxDataViewIconText iconText;
                iconText << variant;
                m_iconColValues[row] = iconText.GetText();
            }
            return true;

        case Col_Date:
        case Col_TextWithAttr:
            wxLogError("Cannot edit the column %d", col);
            break;

        case Col_Custom:
            m_customColValues[row] = variant.GetString();
            break;

        case Col_Max:
            wxFAIL_MSG( "invalid column" );
    }

    return false;
}

// ----------------------------------------------------------------------------
// SmartKBListModel
// ----------------------------------------------------------------------------

wxDEFINE_EVENT(SMARTKB_SEARCH_RESULT_EVENT, wxCommandEvent);

SmartKBListModel::SmartKBListModel() :
	wxDataViewVirtualListModel()
{
	UpdateAll();
}

void SmartKBListModel::ClearAll()
{
	SmartKB::Clear(type_);
}

void SmartKBListModel::UpdateAll()
{
	zqdb::AllExchange allexchange;
	for (auto h : allexchange)
	{
		zqdb::Exchange exchange(h);
		SmartKBItem one;
		one.Code = zqdb::utf8_to_unicode(exchange->Exchange);
		one.Name = zqdb::utf8_to_unicode(exchange->Name);
		one.Type = 0;
		one.Data = h;
		Add(type_, &one, 1);
		zqdb::AllProduct allproduct(h);
		for (auto h : allproduct)
		{
			zqdb::Product product(h);
			SmartKBItem one;
			one.Code = zqdb::utf8_to_unicode(product->Product);
			one.Name = zqdb::utf8_to_unicode(product->Name);
			one.Type = 0;
			one.Data = h;
			Add(type_, &one, 1);
			zqdb::AllCode allcode(h);
			for (auto h : allcode)
			{
				zqdb::Code code(h);
				SmartKBItem one;
				one.Code = zqdb::utf8_to_unicode(code->Code);
				one.Name = zqdb::utf8_to_unicode(code->Name);
				one.Type = 0;
				one.Data = h;
				Add(type_, &one, 1);
			}
		}
	}
	Select(type_);
}

bool SmartKBListModel::Search(wxEvtHandler* notify, const wxString& strKey, int flag)
{
	notify_ = notify;
	return Input(strKey.c_str(), flag);
}

void SmartKBListModel::NotifyResult()
{
	notify_->QueueEvent(new wxCommandEvent(SMARTKB_SEARCH_RESULT_EVENT));
}

void SmartKBListModel::ShowResult()
{
	results_.clear();
	GetInputResults(results_);
	Reset(results_.size());
}

bool SmartKBListModel::SmartKBListModel::GetResult(const wxDataViewItem& item, SmartKBItem& smkbi)
{
	auto row = GetRow(item);
	if (row >= 0 && row < results_.size()) {
		smkbi = results_[row];
		return true;
	}
	return false;
}

void SmartKBListModel::GetValueByRow(wxVariant &variant,
	unsigned int row, unsigned int col) const
{
	switch (col)
	{
	case Col_Code:
	{
		variant = results_[row].Code;
	}
	break;

	case Col_Name:
		variant = results_[row].Name;
		break;

	case Col_Desc:
	{
		static const char *labels[5] =
		{
			// These strings will look wrong without wxUSE_MARKUP, but
			// it's just a sample, so we don't care.
			"<span color=\"#87ceeb\">light</span> and "
			"<span color=\"#000080\">dark</span> blue",
			"<big>growing green</big>",
			"<i>emphatic &amp; red</i>",
			"<b>bold &amp;&amp; cyan</b>",
			"<small><tt>dull default</tt></small>",
		};

		variant = labels[row % 5];
	}
	break;
	case Col_Max:
		wxFAIL_MSG("invalid column");
	}
}

bool SmartKBListModel::GetAttrByRow(unsigned int row, unsigned int col,
	wxDataViewItemAttr &attr) const
{
	auto skin_info_ptr = wxGetApp().GetSkinInfo();
	switch (col)
	{
	case Col_Code:
		attr.SetColour(skin_info_ptr->crCtrlForgnd);
		attr.SetBackgroundColour(skin_info_ptr->crCtrlBkgnd);
		//attr.SetStrikethrough(true);
		return true;
	case Col_Name:
		attr.SetColour(skin_info_ptr->crCtrlForgnd);
		attr.SetBackgroundColour(skin_info_ptr->crCtrlBkgnd);
		return true;
		break;
	case Col_Desc:
		if (!(row % 2))
			return false;
		attr.SetColour(wxColour(*wxLIGHT_GREY));
		attr.SetStrikethrough(true);
		return true;
		break;
	case Col_Max:
		wxFAIL_MSG("invalid column");
	}
	return true;
}

bool SmartKBListModel::SetValueByRow(const wxVariant &variant,
	unsigned int row, unsigned int col)
{
	switch (col)
	{
	case Col_Code:
		return true;
	case Col_Name:
		return true;
		break;
	case Col_Desc:
		return true;
		break;
	case Col_Max:
		break;
	}

	return false;
}

// ----------------------------------------------------------------------------
// MyCodeViewListModel
// ----------------------------------------------------------------------------

SmartKBItemSort::SmartKBItemSort()
{

}

SmartKBItemSort::SmartKBItemSort(MY_CODE_SORT_TYPE type, size_t secs, int sort) :type(type), sort(sort), secs(secs)
{

}

SmartKBItemSort::SmartKBItemSort(const MDB_FIELD& field, int sort) : type(SORT_FIELD), sort(sort), field(field)
{

}

SmartKBItemSort::SmartKBItemSort(const zqdb::Calc::Sort& calc, int sort) : type(SORT_CALC_SORT), sort(sort), calc_sort(calc)
{

}

bool SmartKBItemSort::operator() (const SmartKBItem& x, const SmartKBItem& y) const {
	switch (type)
	{
	case SORT_ZDF: {
		zqdb::Code xcode((HZQDB)x.Data), ycode((HZQDB)y.Data);
		double xzdf = 0, yzdf = 0;
		if (!IsZeroValue(xcode->Volume)) {
			auto xclose = xcode->Close, xyclose = IsInvalidValue(xcode->YClose) ? xcode->YSettle : xcode->YClose;
			xzdf = (xclose - xyclose) / xyclose;
		}
		if (!IsZeroValue(ycode->Volume)) {
			auto yclose = ycode->Close, yyclose = IsInvalidValue(ycode->YClose) ? ycode->YSettle : ycode->YClose;
			yzdf = (yclose - yyclose) / yyclose;
		}
		return xzdf < yzdf;
	} break;
	case SORT_ZDS: {
		return x.ValueF < y.ValueF;
	} break;
	case SORT_FIELD: {
		ASSERT(MDBFieldIsNormalized(field) && sort != 0);
		if (field.index == MDB_FIELD_INDEX(ZQDB, CODE, CODE)) {
			return x.Code < y.Code;
		}
		else if (field.index == MDB_FIELD_INDEX(ZQDB, CODE, NAME)) {
			return x.Name < y.Name;
		}
		else {
			zqdb::Code xcode((HZQDB)x.Data), ycode((HZQDB)y.Data);
			auto xfield = field, yfield = field;
			return xcode.GetFieldAsDouble(xfield) < ycode.GetFieldAsDouble(yfield);
		}
	} break;
	case SORT_CALC_SORT: {
		return calc_sort.Calc((HZQDB)x.Data, (HZQDB)y.Data);
	} break;
	}
	ASSERT(0);
	return false;
}

MyCodeViewListModel::MyCodeViewListModel() :
	wxDataViewVirtualListModel()
{
	all_ = zqdb::Calc::StdContainer();
	UpdateAll();
}

void MyCodeViewListModel::ClearAll()
{
	//filter_.Close();
	SmartKB::Clear(type_);
	//all_.Close();
}

void MyCodeViewListModel::UpdateAll()
{
	if (0) {
		//
	}
	else {
		/*zqdb::AllExchange allexchange;
		for (auto h : allexchange)
		{
			zqdb::AllProduct allproduct(h);
			for (auto h : allproduct)
			{
				zqdb::AllCode allcode(h);
				for (auto h : allcode)
				{
					zqdb::Code code(h);
					SmartKBItem one;
					one.Code = zqdb::utf8_to_unicode(code->Code);
					one.Name = zqdb::utf8_to_unicode(code->Name);
					one.Type = 0;
					one.Data = h;
					Add(type_, &one, 1);
				}
			}
		}*/
		for (size_t i = 0, j = all_.size(); i < j; i++)
		{
			zqdb::Code code(all_[i]);
			SmartKBItem one;
			one.Code = zqdb::utf8_to_unicode(code->Code);
			one.Name = zqdb::utf8_to_unicode(code->Name);
			one.Type = SMARTKB_ITEM_TYPE_CODE;
			one.Data = all_[i];
			Add(type_, &one, 1);
		}
	}
	Select(type_);
}

bool MyCodeViewListModel::IsShowAll()
{
	if(!filter_)
		return key_.IsEmpty();
	return false;
}

void MyCodeViewListModel::ShowAll()
{
	results_.clear();
	GetInputs(results_);
	Reset(results_.size());
}

bool MyCodeViewListModel::IsShowFilter()
{
	return filter_.IsOpen();
}

void MyCodeViewListModel::ShowFilter()
{
	ASSERT(filter_);
	results_.clear();
	for (size_t i = 0, j = filter_.size(); i < j; i++)
	{
		zqdb::Code code(filter_[i]);
		SmartKBItem one;
		one.Code = zqdb::utf8_to_unicode(code->Code);
		one.Name = zqdb::utf8_to_unicode(code->Name);
		one.Type = SMARTKB_ITEM_TYPE_CODE;
		one.Data = filter_[i];
		results_.emplace_back(one);
	}
	Reset(results_.size());
}

bool MyCodeViewListModel::Search(wxEvtHandler* notify, const wxString& strKey, int flag)
{
	notify_ = notify;
	key_ = strKey;
	filter_.Close();
	return Input(strKey.c_str(), flag);
}

void MyCodeViewListModel::Search(wxEvtHandler* notify, const zqdb::Calc::Filter& filter)
{
	ASSERT(filter);
	notify_ = notify;
	key_ = wxT("__filter__");
	filter_ = filter;
	NotifyResult();
}

void MyCodeViewListModel::NotifyResult()
{
	notify_->QueueEvent(new wxCommandEvent(SMARTKB_SEARCH_RESULT_EVENT));
}

void MyCodeViewListModel::ShowResult()
{
	if (IsShowFilter()) {
		ShowFilter();
	}
	else if (IsShowAll()) {
		ShowAll();
	}
	else {
		results_.clear();
		GetInputResults(results_);
		Reset(results_.size());
	}
}

int MyCodeViewListModel::IsSort(MY_CODE_SORT_TYPE* type, size_t* secs)
{
	if (type) {
		*type = sort_.type;
	}
	if (secs) {
		*secs = sort_.secs;
	}
	return sort_.sort;
}

void MyCodeViewListModel::Sort(bool force)
{
	switch (sort_.type)
	{
	case SORT_ZDS: {
		uint32_t date = 0, time = 0;
		date = XUtil::NowDateTime(&time);
		uint32_t prev_date = date, prev_time = time;
		for (auto& result : results_) {
			zqdb::Code code((HZQDB)result.Data);
			double price = code->Close;
			auto cmp = ZQDBCmpTradingTime(code, date, time);
			if (cmp < 0) {
				//
			}
			else {
				if (cmp > 0) {
					uint32_t close_date = 0, close_time = 0;
					close_time = code.GetCloseTime(&close_date);
					prev_date = close_date, prev_time = close_time;
					XUtil::PrevDateTime(prev_date, prev_time, sort_.secs);
				}
				else {
					XUtil::PrevDateTime(prev_date, prev_time, sort_.secs);
				}
				double yclose = IsInvalidValue(code->YClose) ? code->YSettle : code->YClose;
				price = ZQDBGetPriceByTickTime(code, prev_date, prev_time, code->YClose);
			}
			if (IsInvalidValue(price) || IsZeroValue(price)) {
				result.Value = 0;
				result.ValueF = 0;
			}
			else {
				result.Value = code->Close - price;
				result.ValueF = result.Value / price;
			}
		}
	} break;
	default: {
		
	} break;
	}
	
	if (sort_.sort > 0) {
		std::sort(results_.rbegin(), results_.rend(), sort_);
	}
	else if(sort_.sort < 0) {
		std::sort(results_.begin(), results_.end(), sort_);
	}
	else if(force) {
		ShowResult();
	}
}

void MyCodeViewListModel::SortByZD(MY_CODE_SORT_TYPE type, size_t secs, int sort)
{
	sort_ = SmartKBItemSort(type, secs, sort);
	Sort(true);
}

void MyCodeViewListModel::SortByField(MDB_FIELD& field, int sort)
{
	sort_ = SmartKBItemSort(field, sort);
	Sort(true);
}

void MyCodeViewListModel::SortByCalc(const zqdb::Calc::Sort& calc, int sort)
{
	sort_ = SmartKBItemSort(calc, sort);
	Sort(true);
}

bool MyCodeViewListModel::GetResult(const wxDataViewItem& item, SmartKBItem& smkbi)
{
	auto row = GetRow(item);
	if (row >= 0 && row < results_.size()) {
		smkbi = results_[row];
		return true;
	}
	return false;
}

bool MyCodeViewListModel::GetResult(const size_t row, SmartKBItem& smkbi)
{
	if (row >= 0 && row < results_.size()) {
		smkbi = results_[row];
		return true;
	}
	return false;
}

int MyCodeViewListModel::FindResult(HZQDB h)
{
	if (results_.empty()) {
		return -1;
	}
	if (h) {
		switch (h->type)
		{
		case ZQDB_HANDLE_TYPE_EXCHANGE: {
			for (size_t i = 0; i < results_.size(); i++)
			{
				zqdb::Code code((HZQDB)results_[i].Data);
				if (code.GetExchange() == h) {
					return i;
					break;
				}
			}
		} break;
		case ZQDB_HANDLE_TYPE_PRODUCT: {
			for (size_t i = 0; i < results_.size(); i++)
			{
				zqdb::Code code((HZQDB)results_[i].Data);
				if (code.GetProduct() == h) {
					return i;
					break;
				}
			}
		} break;
		case ZQDB_HANDLE_TYPE_CODE: {
			for (size_t i = 0; i < results_.size(); i++)
			{
				if ((HZQDB)results_[i].Data == h) {
					return i;
					break;
				}
			}
		} break;
		default:
			break;
		}
	}
	else {
		return 0;
	}
	return -1;
}

void MyCodeViewListModel::GetValueByRow(wxVariant &variant,
	unsigned int row, unsigned int col) const
{
	switch (col)
	{
	case Col_Code:
	{
		variant = (void*)&results_[row];
	}
	break;
	case Col_Max:
		wxFAIL_MSG("invalid column");
	}
}

bool MyCodeViewListModel::GetAttrByRow(unsigned int row, unsigned int col,
	wxDataViewItemAttr &attr) const
{
	switch (col)
	{
	case Col_Code:
		//attr.SetColour(skin_info_ptr->crCtrlForgnd);
		//attr.SetBackgroundColour(skin_info_ptr->crCtrlBkgnd);
		//attr.SetStrikethrough(true);
		return true;
	case Col_Max:
		wxFAIL_MSG("invalid column");
	}
	return true;
}

bool MyCodeViewListModel::SetValueByRow(const wxVariant &variant,
	unsigned int row, unsigned int col)
{
	switch (col)
	{
	case Col_Code:
		return true;
		break;
	case Col_Max:
		break;
	}

	return false;
}

// ----------------------------------------------------------------------------
// MyCodeListModel
// ----------------------------------------------------------------------------

MyCodeListModel::MyCodeListModel() :
	wxDataViewVirtualListModel()
{
	
}

void MyCodeListModel::Show(const std::vector<HZQDB>& h_list)
{
	code_items_.resize(h_list.size());
	for (size_t i = 0, j = code_items_.size(); i < j; ++i)
	{
		code_items_[i] = h_list[i];
	}
	Reset(code_items_.size());
}

HZQDB MyCodeListModel::GetData(const wxDataViewItem& item)
{
	auto row = GetRow(item);
	if (row >= 0 && row < code_items_.size()) {
		return code_items_[row];
	}
	return nullptr;
}

void MyCodeListModel::GetValueByRow(wxVariant &variant,
	unsigned int row, unsigned int col) const
{
	switch (col)
	{
	case Col_Code:
	{
		variant = code_items_[row];
	}
	break;

	case Col_Name:
		variant = code_items_[row];
		break;

	case Col_Desc:
	{
		static const char *labels[5] =
		{
			// These strings will look wrong without wxUSE_MARKUP, but
			// it's just a sample, so we don't care.
			"<span color=\"#87ceeb\">light</span> and "
			"<span color=\"#000080\">dark</span> blue",
			"<big>growing green</big>",
			"<i>emphatic &amp; red</i>",
			"<b>bold &amp;&amp; cyan</b>",
			"<small><tt>dull default</tt></small>",
		};

		variant = labels[row % 5];
	}
	break;
	case Col_Max:
		wxFAIL_MSG("invalid column");
	}
}

bool MyCodeListModel::GetAttrByRow(unsigned int row, unsigned int col,
	wxDataViewItemAttr &attr) const
{
	auto skin_info_ptr = wxGetApp().GetSkinInfo();
	switch (col)
	{
	case Col_Code:
		attr.SetColour(skin_info_ptr->crCtrlForgnd);
		attr.SetBackgroundColour(skin_info_ptr->crCtrlBkgnd);
		return true;
	case Col_Name:
		attr.SetColour(skin_info_ptr->crCtrlForgnd);
		attr.SetBackgroundColour(skin_info_ptr->crCtrlBkgnd);
		return true;
		break;
	case Col_Desc:
		if (!(row % 2))
			return false;
		attr.SetColour(wxColour(*wxLIGHT_GREY));
		attr.SetStrikethrough(true);
		return true;
		break;
	case Col_Max:
		wxFAIL_MSG("invalid column");
	}
	return true;
}

bool MyCodeListModel::SetValueByRow(const wxVariant &variant,
	unsigned int row, unsigned int col)
{
	switch (col)
	{
	case Col_Code:
		return true;
	case Col_Name:
		return true;
		break;
	case Col_Desc:
		return true;
		break;
	case Col_Max:
		break;
	}

	return false;
}

//// ----------------------------------------------------------------------------
//// MyHZQDBListModel
//// ----------------------------------------------------------------------------
//
//MyHZQDBListModel::MyHZQDBListModel(const char* xml, size_t xmlflag) : Base()
//{
//	CFG_FROM_XML(cfg, xml, xmlflag);
//	auto opt_col = cfg.get_child_optional("col");
//	ASSERT(opt_col);
//	auto& cfg_col = opt_col.get();
//	BOOST_FOREACH(const boost::property_tree::ptree::value_type &module_pr, cfg_col)
//	{
//		auto& col_items = all_col_items_[module_pr.first];
//		BOOST_FOREACH(const boost::property_tree::ptree::value_type &col_pr, module_pr.second)
//		{
//			auto& col_one = col_pr.second;
//			ColInfo col_item;
//			strncpy(col_item.field.name, col_one.get<std::string>("id", "").c_str(), MAX_FIELD_NAME);
//			col_item.name = utf2wxString(col_one.get<std::string>("name", "").c_str());
//			col_item.precision = col_one.get<int>("precision", 0);
//			col_items.emplace_back(col_item);
//		}
//	}
//}
//
//void MyHZQDBListModel::Select(HZQDB user, ZQDB_HANDLE_TYPE type)
//{
//	zqdb::ObjectT<tagModuleInfo> module(ZQDBGetParent(user));
//	col_items_ = all_col_items_[module->Name];
//	for (auto& item : col_items_)
//	{
//		module.NormalizeField(item.field, type);
//	}
//}

