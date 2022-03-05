#pragma once

#include "mytechframe.h"

class MyFrame;

// -- frame --

class MyFrame : public MyTechFrame
{
	typedef MyTechFrame Base;
protected:
	wxRibbonButtonBar *data_bar_ = nullptr;
	wxRibbonButtonBar *calc_bar_ = nullptr;
	zqdb::Calc::AllFunc all_strategy_func_;
	wxRibbonButtonBar *strategy_bar_ = nullptr;
	void InnerUpdateStrategy();
	void AddFirstPages();
public:
    MyFrame(const char* xml, size_t xmlflag);
    ~MyFrame();

	void OnSkinInfoChanged();
	void OnInfoChanged();
	void OnNotifyStartListen();
	void OnNotifyStopListen();
	void OnNotifyUpdate(HZQDB h);

protected:
	//
	
	//Navigate
	//Market
	//Filter
	//Sort
	//TechView
	//Data
	void OnDataListen(wxRibbonButtonBarEvent& evt);
	void OnDataRecord(wxRibbonButtonBarEvent& evt);
	void UpdateTestInfo();
	void OnDataTest(wxRibbonButtonBarEvent& evt);
	void OnDataTestDropdown(wxRibbonButtonBarEvent& evt);
	void OnDataTestRecord(wxCommandEvent& evt);
	//Calc
	void OnCalcEdit(wxRibbonButtonBarEvent& evt);
	void OnCalcEditDropdown(wxRibbonButtonBarEvent& evt);
	void OnCalcEditNew(wxCommandEvent& evt);
	void OnCalcEditOpen(wxCommandEvent& evt);
	void OnCalcTradeFlag(wxRibbonButtonBarEvent& evt);
	void OnCalcSetting(wxRibbonPanelEvent& evt);
	//Script
	//Strategy
	void OnStrategy(wxRibbonButtonBarEvent& evt);
	void OnStrategySetting(wxRibbonPanelEvent& evt);

	void OnCloseEvent(wxCloseEvent& evt);
    wxDECLARE_EVENT_TABLE();
};

