#pragma once

#include <mymodule.h>
#include "mymodule.h"

// -- MyModuleMgr --

class MyModuleMgr
{
public:
	static MyModuleMgr& Inst() {
		static MyModuleMgr _inst;
		return _inst;
	}
protected:
	std::vector<std::shared_ptr<MyModule>> modules_;
public:
    void Init();
	void Term();

	std::shared_ptr<MyModule> GetByHandle(HZQDB h);
	void Broadcast(const std::function<bool(std::shared_ptr<MyModule> module)>& f);
};

