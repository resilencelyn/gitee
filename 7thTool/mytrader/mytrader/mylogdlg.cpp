#include <wx/artprov.h>
#include <wx/imaglist.h>
#include "mylogdlg.h"
#include "mylog.h"

// ----------------------------------------------------------------------------
// MyLogListModel
// ----------------------------------------------------------------------------

MyLogListModel::MyLogListModel(const char* xml, size_t xmlflag) :
	wxDataViewVirtualListModel()
{
	Reset(mylog::GetRecentLogCount());
}

void MyLogListModel::Append(const wxLogRecord& record)
{
	Reset(mylog::GetRecentLogCount());
}

void MyLogListModel::GetValueByRow(wxVariant &variant,
	unsigned int row, unsigned int col) const
{
	auto& record = mylog::GetRecentLog(row);
	switch (col)
	{
	case Col_Level: {
		switch (record.level)
		{
		case wxLOG_FatalError:
		case wxLOG_Error:
			variant = ("Error");
			break;
		case wxLOG_Warning:
			variant = ("Warn ");
			break;
		case wxLOG_Message:
		case wxLOG_Status:
		case wxLOG_Info:
			variant = ("Info ");
			break;
		case wxLOG_Debug:
		case wxLOG_Trace:
		default:
			variant = ("Debug");
			break;
		}
	} break;
	case Col_Msg: {
		variant = record.msg;
	} break;
	case Col_Time: {
		variant = wxDateTime(record.info.timestamp).Format("%H:%M:%S");
	} break;
	case Col_Thread: {
		variant = wxString::Format(wxT("%09zu"), (size_t)record.info.threadId);
	} break;
	default: {
		variant = wxT("");
	} break;
	}
}

bool MyLogListModel::GetAttrByRow(unsigned int row, unsigned int col,
	wxDataViewItemAttr &attr) const
{
	auto& record = mylog::GetRecentLog(row);
	switch (record.level)
	{
	case wxLOG_FatalError:
	case wxLOG_Error:
		//attr.SetColour(wxColour(*wxWHITE));
		attr.SetBackgroundColour(wxColour(*wxRED));
		break;
	case wxLOG_Warning:
		//attr.SetColour(wxColour(*wxWHITE));
		attr.SetBackgroundColour(wxColour(*wxYELLOW));
		break;
	case wxLOG_Message:
	case wxLOG_Status:
	case wxLOG_Info:
		//attr.SetColour(wxColour(*wxBLUE));
		break;
	case wxLOG_Debug:
	case wxLOG_Trace:
	default:
		break;
	}
	switch (col)
	{
	case Col_Level:
		/*if (row < m_toggleColValues.size())
		{
			if (m_toggleColValues[row])
			{
				attr.SetColour(wxColour(*wxLIGHT_GREY));
				attr.SetStrikethrough(true);
				return true;
			}
		}
		return false;*/
		break;

	case Col_Msg:
		/*if (!(row % 2))
			return false;
		attr.SetColour(*wxYELLOW);
		attr.SetBackgroundColour(*wxLIGHT_GREY);*/
		break;

	case Col_Time:
		/*if (row < m_toggleColValues.size())
		{
			if (m_toggleColValues[row])
			{
				attr.SetColour(wxColour(*wxLIGHT_GREY));
				attr.SetStrikethrough(true);
				return true;
			}
		}
		wxFALLTHROUGH;*/
		break;

	case Col_Thread:
		/*// do what the labels defined in GetValueByRow() hint at
		switch (row % 5)
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
		}*/
		break;

	case Col_Max:
		wxFAIL_MSG("invalid column");
	}
	return true;
}

bool MyLogListModel::SetValueByRow(const wxVariant &variant,
	unsigned int row, unsigned int col)
{
	return true;
}

wxBEGIN_EVENT_TABLE(MyLogDlg, wxDialog)
EVT_ACTIVATE(MyLogDlg::OnActivate)
wxEND_EVENT_TABLE()

MyLogDlg::MyLogDlg(wxPoint parentPos, int width) :wxDialog(nullptr, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(width, 256), 0)
{
    parentPos.y -= GetRect().height;
    SetPosition(parentPos);

	ctrl_list_ = new wxDataViewCtrl(this, wxID_ANY);
	ctrl_list_model_ = new MyLogListModel();
	ctrl_list_->AssociateModel(ctrl_list_model_.get());
	ctrl_list_->AppendTextColumn(wxT("时间"), 0, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_NOT);
	ctrl_list_->AppendTextColumn(wxT("线程"), 1, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_RIGHT);
	ctrl_list_->AppendTextColumn(wxT("级别"), 2, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_NOT);
	ctrl_list_->AppendTextColumn(wxT("消息"), 3, wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_DEFAULT, wxALIGN_NOT);
	ctrl_list_->EnsureVisible(ctrl_list_model_->GetItem(ctrl_list_model_->GetCount() - 1));
    //// prepare the imagelist
    //static const int ICON_SIZE = 16;
    //wxImageList* imageList = new wxImageList(ICON_SIZE, ICON_SIZE);

    //static wxString const icons[] =
    //{
    //    wxART_ERROR,
    //    wxART_WARNING,
    //    wxART_INFORMATION
    //};

    //for (size_t icon = 0; icon < WXSIZEOF(icons); icon++)
    //{
    //    wxBitmap bmp = wxArtProvider::GetBitmap(icons[icon], wxART_MESSAGE_BOX, wxSize(ICON_SIZE, ICON_SIZE));
    //    imageList->Add(bmp);
    //}

    //m_listctrl->SetImageList(imageList, wxIMAGE_LIST_SMALL);

    //size_t count = m_messages.size();
    //for (size_t n = 0; n < count; n++)
    //{
    //    int image = 2;
    //    wxString msg = m_messages[n];
    //    msg.Replace(wxT("\n"), wxT(" "));
    //    m_listctrl->InsertItem(n, msg, image);
    //}

    //m_listctrl->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
}

MyLogDlg::~MyLogDlg()
{

}

static MyLogDlg* g_logdlg = nullptr;

void MyLogDlg::ShowDlg(wxPoint parentPos, int width)
{
	if (!g_logdlg) {
		g_logdlg = new MyLogDlg(parentPos, width);
		g_logdlg->Show();
		g_logdlg->Raise();
	}
}

void MyLogDlg::AppendLog(const wxLogRecord& record)
{
	if (g_logdlg) {
		if (!wxIsMainThread()) {
			wxGetApp().Post([record] {
				if (g_logdlg) {
					g_logdlg->Append(record);
				}
			});
		}
		else {
			g_logdlg->Append(record);
		}
	}
}

void MyLogDlg::Append(const wxLogRecord& record)
{
	ctrl_list_model_->Append(record);
	ctrl_list_->EnsureVisible(ctrl_list_model_->GetItem(ctrl_list_model_->GetCount() - 1));
	ctrl_list_->Refresh();
}

void MyLogDlg::OnActivate(wxActivateEvent& event)
{
    if (!event.GetActive()) {
		//这里会收到两次消息，所以要注意
		Show(false);
		wxGetApp().Post(200, [this] {
			if (g_logdlg) {
				//LOG4D("CLOSE LOG DIALOG %p %p", g_logdlg, this);
				g_logdlg = nullptr;
				if (Close(true)) {
					Destroy();
				}
			}
		});
    }
}
