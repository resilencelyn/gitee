#include "mymodulemgr.h"
#include "myctp.h"

// -- MyModuleMgr --

void MyModuleMgr::Init()
{
	zqdb::AllModule allmodule;
	for (auto& h : allmodule)
	{
		zqdb::ObjectT<MODULEINFO> module(h);
		if (stricmp(module->Code, "ctp") == 0) {
			modules_.emplace_back(std::make_shared<MyCTPModule>(h));
		}
	}
}

void MyModuleMgr::Term()
{
	modules_.clear();
}

std::shared_ptr<MyModule> MyModuleMgr::GetByHandle(HZQDB h)
{
	HZQDB hmodule = ZQDBGetModule(h);
	if (hmodule) {
		for (auto& m : modules_) {
			if (*m == hmodule) {
				return m;
				break;
			}
		}
	}
	return nullptr;
}

void MyModuleMgr::Broadcast(const std::function<bool(std::shared_ptr<MyModule> module)>& f)
{
	for (auto& m : modules_) {
		if (f(m)) {
			break;
		}
	}
}
