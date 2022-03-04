#include "mymodule.h"

std::vector<std::shared_ptr<MyModule>> g_modules;
/*
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
*/

void MyModule::AddModule(std::shared_ptr<MyModule> module)
{
	g_modules.emplace_back(module);
}

void MyModule::RemoveAllModule()
{
	g_modules.clear();
}

std::shared_ptr<MyModule> MyModule::GetByHandle(HZQDB h)
{
	HZQDB hmodule = ZQDBGetModule(h);
	if (hmodule) {
		for (auto& m : g_modules) {
			if (*m == hmodule) {
				return m;
				break;
			}
		}
	}
	return nullptr;
}

void MyModule::Broadcast(const std::function<bool(std::shared_ptr<MyModule> module)>& f)
{
	for (auto& m : g_modules) {
		if (f(m)) {
			break;
		}
	}
}
