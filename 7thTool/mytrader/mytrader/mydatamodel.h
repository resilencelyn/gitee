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
	std::string type_ = ""; //Ĭ��ȫ��SmartKB
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

	//�������������б�
	void ClearAll();
	void UpdateAll();
	//����
	bool Search(wxEvtHandler* notify, const wxString& strKey, int flag = 0);
	//֪ͨ��������
	virtual void NotifyResult();
	//��ʾ���
	virtual void ShowResult();
	//��ȡ����
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
//������ʾ����������Ϣ
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
	int sort = 0; //-1����0������1����
private:
	union {
		size_t duration; //ZDF����ʱ�䣬����5��������
		MDB_FIELD field = { 0 }; //Normalize����field
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
	std::string type_ = ""; //Ĭ��ȫ��SmartKB
	zqdb::Calc::StdContainer all_; //type��Ӧfilter
	wxEvtHandler* notify_ = nullptr; 
	wxString key_; //�ؼ���
	zqdb::Calc::Filter filter_; //��ǰ������
	std::vector<SmartKBItem> results_;
	SmartKBItemSort sort_; //������
public:
	enum
	{
		Col_Code,
		Col_Max
	};

	MyCodeViewListModel();

	//����ȫ��filter
	inline HZQDB All() { return all_; }
	//�������������б�
	void ClearAll();
	void UpdateAll();
	//�Ƿ���ʾ����
	bool IsShowAll();
	//��ʾ����
	void ShowAll();
	//�Ƿ���ʾ������
	bool IsShowFilter();
	//��ʾɸѡ
	void ShowFilter();
	//����
	bool Search(wxEvtHandler* notify, const wxString& strKey, int flag = 0);
	void Search(wxEvtHandler* notify, const zqdb::Calc::Filter& filter);
	//֪ͨ��������
	virtual void NotifyResult();
	//��ʾ���
	virtual void ShowResult();
	//����
	int IsSort(MY_CODE_SORT_TYPE* type = nullptr);
	void Sort(bool force = false);
	void SortByZD(MY_CODE_SORT_TYPE type, size_t duration, int sort);
	void SortByField(MDB_FIELD& field, int sort);
	void SortByCalc(const zqdb::Calc::Sort& calc, int sort);
	//��ȡ����
	bool GetResult(const wxDataViewItem& item, SmartKBItem& smkbi);
	bool GetResult(const size_t row, SmartKBItem& smkbi);
	//��������,��������λ�ã�-1��ʾû���ҵ�
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
