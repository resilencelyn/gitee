#ifndef _MY_LOG_DLG_
#define _MY_LOG_DLG_

#include "myapp.h"
#include "mylog.h"
#include "wx/dataview.h"

class MyLogListModel
	: public wxDataViewVirtualListModel
{
public:
	enum
	{
		Col_Time,
		Col_Thread,
		Col_Level,
		Col_Msg,
		Col_Max
	};

	MyLogListModel(const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING);

	void Append(const wxLogRecord& record);

	// implementation of base class virtuals to define model

	virtual unsigned int GetColumnCount() const wxOVERRIDE
	{
		return Col_Max;
	}

	virtual wxString GetColumnType(unsigned int col) const wxOVERRIDE
	{
		return "string";
	}

	virtual void GetValueByRow(wxVariant &variant,
		unsigned int row, unsigned int col) const wxOVERRIDE;
	virtual bool GetAttrByRow(unsigned int row, unsigned int col,
		wxDataViewItemAttr &attr) const wxOVERRIDE;
	virtual bool SetValueByRow(const wxVariant &variant,
		unsigned int row, unsigned int col) wxOVERRIDE;
};

class MyLogDlg : public wxDialog
{
private:
	wxDataViewCtrl* ctrl_list_ = nullptr;
	wxObjectDataPtr<MyLogListModel> ctrl_list_model_;

	//wxString GetLevel(const wxString& msg)
	//{
	//    wxLogLevelValues level = wxLOG_Message;
	//    switch (level)
	//    {
	//    case wxLOG_Info:
	//    case wxLOG_Message:
	//    case wxLOG_Status:
	//    case wxLOG_Debug:
	//    case wxLOG_Trace:
	//    case wxLOG_Progress:
	//    case wxLOG_User:
	//        return wxART_INFORMATION;
	//    case wxLOG_Warning:
	//        return wxART_WARNING; 
	//    case wxLOG_Error:
	//    case wxLOG_FatalError:
	//        return wxART_ERROR;
	//    default:
	//        return wxART_INFORMATION;
	//    }
	//}
	//

	MyLogDlg(wxPoint parentPos, int width);

public:
	static void ShowDlg(wxPoint parentPos, int width);
	static void AppendLog(const wxLogRecord& record);

	~MyLogDlg();

	void Append(const wxLogRecord& record);

    void OnActivate(wxActivateEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#endif