#include "Engine/General/UniversalFunction.hpp"
//////////////////////////////////////////////////////////////////////////
void UniversalFunctionDatabase::Add(const std::string& name, universal_func_cb func)
{
	m_database[name] = func;
}

void UniversalFunctionDatabase::Remove(const std::string& name)
{
	m_database.erase(name);
}

void UniversalFunctionDatabase::CallF(const std::string& name, void* input)
{
	universal_func_cb funcToCall;
	if (FindF(name, funcToCall))
		funcToCall(input);
}

bool UniversalFunctionDatabase::FindF(const std::string& name, universal_func_cb& func)
{
	auto found = m_database.find(name);
	if (found != m_database.end())
	{
		func = found->second;
		return true;
	}

	return false;
}

