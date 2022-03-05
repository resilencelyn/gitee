#pragma once

#include <view.h>

//// -- MyBaseView -- 
//
//template<class T>
//class MyBaseViewT : public zqdb::BaseViewT<T>
//{
//	typedef zqdb::BaseViewT<T> Base;
//public:
//	using Base::Base;
//
//	virtual void OnSkinInfoChanged() {}
//	virtual void OnHandleChanged() {}
//	virtual void OnNotifyEnable(HZQDB h) { }
//	virtual void OnNotifyDisable(HZQDB h) { }
//	virtual void OnNotifyAppend(HZQDB h) { }
//	virtual void OnNotifyRemove(HZQDB h) { }
//	virtual void OnNotifyUpdate(HZQDB h) { }
//};

// -- MyLoginView -- 

class MyLoginView : public zqdb::BaseViewT<MyLoginView>
{
	typedef zqdb::BaseViewT<MyLoginView> Base;
public:
	using Base::Base;

	virtual void OnSkinInfoChanged() {}
	virtual void OnHandleChanged() {}
	virtual void OnNotifyEnable(HZQDB h) { }
	virtual void OnNotifyDisable(HZQDB h) { }
	virtual void OnNotifyAppend(HZQDB h) { }
	virtual void OnNotifyRemove(HZQDB h) { }
	virtual void OnNotifyUpdate(HZQDB h) { }
	virtual void OnNetStatus(HNNODE h, NET_NODE_STATUS status) { }
	virtual int OnNetMsg(zqdb::Msg& msg) { return 0; }

	//返回用户名 USER.MODULE，空表示不登录该模块
	virtual const char* StartLogin() { return nullptr; }
	virtual void CancelLogin() { }
	virtual void UpdateInfo(const char* const pInfo) {};
};

// -- MyUserViewT -- 

template<class T>
class MyUserViewT
	: public zqdb::BaseViewT<T>
	, public zqdb::UserMap<T>
{
	typedef zqdb::BaseViewT<T> Base;
	typedef zqdb::UserMap<T> UserBase;
public:
	using Base::Base;

	bool IsDispOk() const { return Base::IsDispOk() && UserBase::IsOk(); }

	void OnSkinInfoChanged() {}
	/*void OnHandleChanged() {}
	void OnNotifyEnable(HZQDB h) { }
	void OnNotifyDisable(HZQDB h) { }
	void OnNotifyAppend(HZQDB h) { }
	void OnNotifyRemove(HZQDB h) { }
	void OnNotifyUpdate(HZQDB h) { }
	void OnUserChanged() { }*/
};

// -- MyMiniView -- 

class MyMiniView
	: public MyUserViewT<MyMiniView>
{
	typedef MyUserViewT<MyMiniView> Base;
public:
	using Base::Base;

	virtual void OnSkinInfoChanged() {}
	virtual void OnHandleChanged() {}
	virtual void OnNotifyEnable(HZQDB h) { }
	virtual void OnNotifyDisable(HZQDB h) { }
	virtual void OnNotifyAppend(HZQDB h) { }
	virtual void OnNotifyRemove(HZQDB h) { }
	virtual void OnNotifyUpdate(HZQDB h) { }
	virtual void OnUserChanged() { }
};

// -- MyMainView -- 

class MyMainView : public zqdb::BaseViewT<MyMainView>
{
	typedef zqdb::BaseViewT<MyMainView> Base;
public:
	using Base::Base;

	virtual void OnSkinInfoChanged() {}
	virtual void OnHandleChanged() {}
	virtual void OnNotifyEnable(HZQDB h) { }
	virtual void OnNotifyDisable(HZQDB h) { }
	virtual void OnNotifyAppend(HZQDB h) { }
	virtual void OnNotifyRemove(HZQDB h) { }
	virtual void OnNotifyUpdate(HZQDB h) { }
};

// -- MyModule --

enum {
	ORDER_SEND_FLAG_NONE = 0,
	ORDER_SEND_FLAG_SHOWTIPS = 0X01,
};

class MyModule 
	: public zqdb::ObjectT<MODULEINFO>
	, public zqdb::NotifyMap<MyModule>
{
	typedef zqdb::ObjectT<MODULEINFO> Base;
	typedef zqdb::NotifyMap<MyModule> NotifyBase;
public:
	static void AddModule(std::shared_ptr<MyModule> module);
	//static void RemoveModule(std::shared_ptr<MyModule> module);
	static void RemoveAllModule();
	static std::shared_ptr<MyModule> GetByHandle(HZQDB h);
	static void Broadcast(const std::function<bool(std::shared_ptr<MyModule> module)>& f);
public:
	MyModule(HZQDB h):Base(h) {}
	virtual ~MyModule() {}

	virtual wxString GetUserInfo(HZQDB user) { return wxEmptyString; }

	virtual MyLoginView* NewLoginView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING) { return nullptr; }
	virtual MyMiniView* NewMiniView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING) { return nullptr; }
	//virtual wxRibbonPage* NewMainPage(wxRibbonBar* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING) { return nullptr; }
	virtual MyMainView* NewMainView(wxWindow* parent, const char* xml = nullptr, size_t xmlflag = XUtil::XML_FLAG_JSON_STRING) { return nullptr; }

	virtual int ReqModifyPassword(HZQDB user, const char* old_pwd, const char* new_pwd, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0) { return 0; }
	virtual int ReqModifyAccountPassword(HZQDB account, const char* old_pwd, const char* new_pwd, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0) { return 0; }

	virtual int OrderSend(HZQDB user, HZQDB code, char direction, char offset, char type, double volume, double price, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0) { return 0; }
		//, int slippage, double stoploss, double takeprofit,
		//const char* comment, int magic, unsigned long date, unsigned long time);
	virtual int OrderCancel(HZQDB user, HZQDB order, HNMSG* rsp = nullptr, size_t timeout = 0, size_t flags = 0) { return 0; }
	virtual int OrderClose(HZQDB user, HZQDB position, char type, double volume, double price, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0) { return 0; }
	
	virtual int ReqQryMaxOrderVolume(HZQDB user, HZQDB code, char direction, char offset, char type, HNMSG* rsp, size_t timeout = 3000, size_t flags = 0) { return 0; }

	virtual void OnTimer() {}

	virtual void OnNotifyEnable(HZQDB h) { }
	virtual void OnNotifyDisable(HZQDB h) { }
	virtual void OnNotifyAppend(HZQDB h) { }
	virtual void OnNotifyRemove(HZQDB h) { }
	virtual void OnNotifyUpdate(HZQDB h) { }

	virtual void OnNetStatus(HNNODE h, NET_NODE_STATUS status) { }
	virtual int OnNetMsg(zqdb::Msg& msg) { return 0; }
};
