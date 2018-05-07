#pragma once
#include "Engine/Core/Singleton.hpp"
#include <map>
//////////////////////////////////////////////////////////////////////////
typedef void(*universal_func_cb)(void*);
//////////////////////////////////////////////////////////////////////////
class UniversalFunctionDatabase : public Singleton<UniversalFunctionDatabase>
{
public:
// 	UniversalFunctionDatabase();
// 	~UniversalFunctionDatabase();
	
	void Add(const std::string& name, universal_func_cb func);
	void Remove(const std::string& name);
	void CallF(const std::string& name, void*);

private:
	std::map<std::string, universal_func_cb> m_database;

private:
	bool FindF(const std::string& name, universal_func_cb& func);
};
//////////////////////////////////////////////////////////////////////////
#define UF UniversalFunctionDatabase::GetInstance()