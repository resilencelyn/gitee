#pragma once
#include "base.h"
#include <atomic>
#include <mutex>
#ifdef WIN32
#include <shared_mutex>
#else
#include <condition_variable>
#endif
#include <wxutil.h>
#include <XUtil/XXml.hpp>
#include <XUtil/XDateTime.hpp>

//template<class T>
//class NotifyImplT : public TaskImplT<T>
//{
//	typedef TaskImplT<T> Base;
//public:
//	using Base::Base;
//
//	void OnNotifyEnable(HZQDB h) {
//		T* pT = static_cast<T*>(this);
//		pT->Broadcast(h);
//	}
//	void OnNotifyDisable(HZQDB h) {}
//	void OnNotifyAppend(HZQDB h) {}
//	void OnNotifyRemove(HZQDB h) {}
//	void OnNotifyUpdate(HZQDB h) {}
//	void OnNotify(HMDB hdb, HMTABLE htb, MDB_NOTIFY_DATA* notify)
//	{
//		const ZQDB_NOTIFY_INF inf = { static_cast<T*>(this)
//		, [](HZQDB h, void* data) { ((T*)data)->Post([data, h]() { ((T*)data)->OnNotifyEnable(h); }); }
//		, [](HZQDB h, void* data) { ((T*)data)->Post([data, h]() { ((T*)data)->OnNotifyDisable(h); }); }
//		, [](HZQDB h, void* data) { ((T*)data)->Post([data, h]() { ((T*)data)->OnNotifyAppend(h); }); }
//		, [](HZQDB h, void* data) { ((T*)data)->Post([data, h]() { ((T*)data)->OnNotifyRemove(h); }); }
//		, [](HZQDB h, void* data) { ((T*)data)->Post([data, h]() { ((T*)data)->OnNotifyUpdate(h); }); }
//		};
//		ZQDBNotifyDispatch(inf, hdb, htb, notify);
//	}
//};

//wxDECLARE_EVENT(ZQDB_NOTIFY_ENABLE_EVENT, wxCommandEvent);
//wxDECLARE_EVENT(ZQDB_NOTIFY_DISABLE_EVENT, wxCommandEvent);
//wxDECLARE_EVENT(ZQDB_NOTIFY_APPEND_EVENT, wxCommandEvent);
//wxDECLARE_EVENT(ZQDB_NOTIFY_REMOVE_EVENT, wxCommandEvent);
//wxDECLARE_EVENT(ZQDB_NOTIFY_UPDATE_EVENT, wxCommandEvent);
//
//template<class T>
//class NotifyImpl : public zqdb::NotifyImpl<T>
//{
//	typedef zqdb::NotifyImpl<T> Base;
//public:
//	using Base::Base;
//
//	void OnNotifyEnable(HZQDB h) {
//		T* pT = static_cast<T*>(this);
//		auto event = new wxCommandEvent(ZQDB_NOTIFY_ENABLE_EVENT);
//		//event->SetString(table);
//		//event->SetInt(type);
//		//event->SetExtraLong(offset);
//		event->SetClientData(h);
//		wxQueueEvent(pT, event);
//	}
//	void OnNotifyDisable(HZQDB h) {
//		T* pT = static_cast<T*>(this);
//		auto event = new wxCommandEvent(ZQDB_NOTIFY_DISABLE_EVENT);
//		//event->SetString(table);
//		//event->SetInt(type);
//		//event->SetExtraLong(offset);
//		event->SetClientData(h);
//		wxQueueEvent(pT, event);
//	}
//	void OnNotifyAppend(HZQDB h) {
//		T* pT = static_cast<T*>(this);
//		auto event = new wxCommandEvent(ZQDB_NOTIFY_APPEND_EVENT);
//		//event->SetString(table);
//		//event->SetInt(type);
//		//event->SetExtraLong(offset);
//		event->SetClientData(h);
//		wxQueueEvent(pT, event);
//	}
//	void OnNotifyRemove(HZQDB h) {
//		T* pT = static_cast<T*>(this);
//		auto event = new wxCommandEvent(ZQDB_NOTIFY_REMOVE_EVENT);
//		//event->SetString(table);
//		//event->SetInt(type);
//		//event->SetExtraLong(offset);
//		event->SetClientData(h);
//		wxQueueEvent(pT, event);
//	}
//	void OnNotifyUpdate(HZQDB h) {
//		T* pT = static_cast<T*>(this);
//		auto event = new wxCommandEvent(ZQDB_NOTIFY_UPDATE_EVENT);
//		//event->SetString(table);
//		//event->SetInt(type);
//		//event->SetExtraLong(offset);
//		event->SetClientData(h);
//		wxQueueEvent(pT, event);
//	}
//};

///

//自定义命令事件说明
//wxCommandEvent
//String -> Key
//Int -> Type
//ExtraLong -> Long
//ClientData -> Ptr
enum CMD_TYPE 
{
	CMD_TYPE_ZQDB, //Key是Code,ClientData是HZQDB
};

wxDECLARE_EVENT(MY_CMD_GOTO_EVENT, wxCommandEvent);

class ItemBase
{
public:
	virtual const char* Key() { return ""; }
	virtual const char* Name() { return ""; }
	virtual const char* Desc() { return ""; }
};

void Broadcast(wxWindow* parent, const std::function<void(wxWindow* child)>& f, bool deep = false);

///
