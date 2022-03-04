/////////////////////////////////////////////////////////////////////////////
// Name:        mymodels.h
// Purpose:     wxDataViewCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Bo Yang
// Created:     06/01/06
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "myapp.h"
#include "smartkb.h"

#include "wx/dataview.h"
#include "wx/hashmap.h"
#include <wx/vector.h>

WX_DECLARE_HASH_MAP(unsigned, wxString, wxIntegerHash, wxIntegerEqual,
                    IntToStringMap);

// ----------------------------------------------------------------------------
// MyListModel
// ----------------------------------------------------------------------------

class MyListModel: public wxDataViewVirtualListModel
{
public:
    enum
    {
        Col_Toggle,
        Col_EditableText,
        Col_IconText,
        Col_Date,
        Col_TextWithAttr,
        Col_Custom,
        Col_Max
    };

    MyListModel();

    // helper methods to change the model

    void Prepend( const wxString &text );
    void DeleteItem( const wxDataViewItem &item );
    void DeleteItems( const wxDataViewItemArray &items );
    void AddMany();


    // implementation of base class virtuals to define model

    virtual unsigned int GetColumnCount() const wxOVERRIDE
    {
        return Col_Max;
    }

    virtual wxString GetColumnType( unsigned int col ) const wxOVERRIDE
    {
        if (col == Col_Toggle)
            return "bool";

        if (col == Col_IconText)
            return "wxDataViewIconText";

        return "string";
    }

    virtual void GetValueByRow( wxVariant &variant,
                                unsigned int row, unsigned int col ) const wxOVERRIDE;
    virtual bool GetAttrByRow( unsigned int row, unsigned int col,
                               wxDataViewItemAttr &attr ) const wxOVERRIDE;
    virtual bool SetValueByRow( const wxVariant &variant,
                                unsigned int row, unsigned int col ) wxOVERRIDE;

private:
    wxVector<bool>   m_toggleColValues;
    wxArrayString    m_textColValues;
    wxArrayString    m_iconColValues;
    IntToStringMap   m_customColValues;
    wxIcon           m_icon[2];
};

// ----------------------------------------------------------------------------
// SmartKBListModel
// ----------------------------------------------------------------------------

wxDECLARE_EVENT(SMARTKB_SEARCH_RESULT_EVENT, wxCommandEvent);

class SmartKBListModel : public wxDataViewVirtualListModel, public SmartKB
{
private:
	std::string type_ = ""; //默认全局SmartKB
	wxEvtHandler* notify_ = nullptr;
	std::vector<SmartKBItem> results_;
public:
	enum
	{
		Col_Code,
		Col_Name,
		Col_Desc,
		Col_Max
	};

	SmartKBListModel();

	//更新所有数据列表
	void ClearAll();
	void UpdateAll();
	//搜索
	bool Search(wxEvtHandler* notify, const wxString& strKey, int flag = 0);
	//通知搜索结束
	virtual void NotifyResult();
	//显示结果
	virtual void ShowResult();
	//获取数据
	bool GetResult(const wxDataViewItem& item, SmartKBItem& smkbi);

	// implementation of base class virtuals to define model

	virtual unsigned int GetColumnCount() const wxOVERRIDE
	{
		return Col_Max;
	}

	virtual wxString GetColumnType(unsigned int col) const wxOVERRIDE
	{
		/*if (col == Col_Toggle)
			return "bool";

		if (col == Col_IconText)
			return "wxDataViewIconText";*/

		return "string";
	}

	virtual void GetValueByRow(wxVariant &variant,
		unsigned int row, unsigned int col) const wxOVERRIDE;
	virtual bool GetAttrByRow(unsigned int row, unsigned int col,
		wxDataViewItemAttr &attr) const wxOVERRIDE;
	virtual bool SetValueByRow(const wxVariant &variant,
		unsigned int row, unsigned int col) wxOVERRIDE;
};

///
//可以显示代码行情信息
///

enum MY_CODE_SORT_TYPE {
	SORT_ZDF,
	SORT_ZDS,
	SORT_FIELD,
	SORT_CALC_SORT,
};

struct SmartKBItemSort
{
public:
	MY_CODE_SORT_TYPE type = SORT_ZDF;
	int sort = 0; //-1升序，0不排序，1降序
private:
	union {
		size_t duration; //ZDF排序时间，比如5分钟排序
		MDB_FIELD field = { 0 }; //Normalize过的field
	};
	zqdb::Calc::Sort calc_sort; //
public:
	SmartKBItemSort();
	SmartKBItemSort(MY_CODE_SORT_TYPE type, size_t duration, int sort);
	SmartKBItemSort(const MDB_FIELD& field, int sort);
	SmartKBItemSort(const zqdb::Calc::Sort& calc, int sort);

	bool operator() (const SmartKBItem& x, const SmartKBItem& y) const;
};

class MyCodeViewListModel : public wxDataViewVirtualListModel, public SmartKB
{
private:
	std::string type_ = ""; //默认全局SmartKB
	zqdb::Calc::StdContainer all_; //type对应filter
	wxEvtHandler* notify_ = nullptr; 
	wxString key_; //关键字
	zqdb::Calc::Filter filter_; //当前过滤器
	std::vector<SmartKBItem> results_;
	SmartKBItemSort sort_; //排序函数
public:
	enum
	{
		Col_Code,
		Col_Max
	};

	MyCodeViewListModel();

	//返回全局filter
	inline HZQDB All() { return all_; }
	//更新所有数据列表
	void ClearAll();
	void UpdateAll();
	//是否显示所有
	bool IsShowAll();
	//显示所有
	void ShowAll();
	//是否显示过滤器
	bool IsShowFilter();
	//显示筛选
	void ShowFilter();
	//搜索
	bool Search(wxEvtHandler* notify, const wxString& strKey, int flag = 0);
	void Search(wxEvtHandler* notify, const zqdb::Calc::Filter& filter);
	//通知搜索结束
	virtual void NotifyResult();
	//显示结果
	virtual void ShowResult();
	//排序
	int IsSort(MY_CODE_SORT_TYPE* type = nullptr);
	void Sort(bool force = false);
	void SortByZD(MY_CODE_SORT_TYPE type, size_t duration, int sort);
	void SortByField(MDB_FIELD& field, int sort);
	void SortByCalc(const zqdb::Calc::Sort& calc, int sort);
	//获取数据
	bool GetResult(const wxDataViewItem& item, SmartKBItem& smkbi);
	bool GetResult(const size_t row, SmartKBItem& smkbi);
	//查找数据,返回数据位置，-1表示没有找到
	int FindResult(HZQDB h);

	// implementation of base class virtuals to define model

	virtual unsigned int GetColumnCount() const wxOVERRIDE
	{
		return Col_Max;
	}

	virtual wxString GetColumnType(unsigned int col) const wxOVERRIDE
	{
		return "void*";
	}

	virtual void GetValueByRow(wxVariant &variant,
		unsigned int row, unsigned int col) const wxOVERRIDE;
	virtual bool GetAttrByRow(unsigned int row, unsigned int col,
		wxDataViewItemAttr &attr) const wxOVERRIDE;
	virtual bool SetValueByRow(const wxVariant &variant,
		unsigned int row, unsigned int col) wxOVERRIDE;
};

class MyCodeListModel : public wxDataViewVirtualListModel
{
public:
	enum
	{
		Col_Code,
		Col_Name,
		Col_Desc,
		Col_Max
	};
private:
	std::vector<HZQDB> code_items_;
public:
	MyCodeListModel();

	void Show(const std::vector<HZQDB>& h_list);
	HZQDB GetData(const wxDataViewItem& item);

	// implementation of base class virtuals to define model

	virtual unsigned int GetColumnCount() const wxOVERRIDE
	{
		return Col_Max;
	}

	virtual wxString GetColumnType(unsigned int col) const wxOVERRIDE
	{
		if (col == Col_Code)
		return "void*";

		if (col == Col_Name)
		return "void*";

		return "string";
	}

	virtual void GetValueByRow(wxVariant &variant,
		unsigned int row, unsigned int col) const wxOVERRIDE;
	virtual bool GetAttrByRow(unsigned int row, unsigned int col,
		wxDataViewItemAttr &attr) const wxOVERRIDE;
	virtual bool SetValueByRow(const wxVariant &variant,
		unsigned int row, unsigned int col) wxOVERRIDE;
};
//
//class MyHZQDBListModel : public zqdb::HZQDBListModel
//{
//	typedef zqdb::HZQDBListModel Base;
//private:
//	std::map<std::string, std::vector<ColInfo>> all_col_items_;
//public:
//	MyHZQDBListModel(const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);
//
//	void Select(HZQDB user, ZQDB_HANDLE_TYPE type);
//};
