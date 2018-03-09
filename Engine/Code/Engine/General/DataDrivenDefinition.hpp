#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/XMLParseHelper.h"

#include <string>
#include <map>
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
const std::string ERROR_NO_NAME = "ERROR_NO_NAME";
//////////////////////////////////////////////////////////////////////////
template<typename T>
class DataDrivenDefinition
{
public:
	DataDrivenDefinition(const XMLNode& element);
	virtual ~DataDrivenDefinition();

	static void ParseFromFile(const std::string& filePath);

	static T* getDefinition(const std::string& name);

	//These functions only used for creating definitions in code. Only use if you know what you are doing.
	DataDrivenDefinition(const std::string& name);
	static T* AddNewDefinition(const std::string& name);
public:
	static std::map<std::string, T*> s_registry;

	std::string m_name;
};
//////////////////////////////////////////////////////////////////////////
template<typename T>
std::map<std::string, T*> DataDrivenDefinition<T>::s_registry;
//////////////////////////////////////////////////////////////////////////
template<typename T>
DataDrivenDefinition<T>::DataDrivenDefinition(const XMLNode& element)
{
	m_name = ParseXMLAttribute(element, "name", ERROR_NO_NAME);
}

template<typename T>
DataDrivenDefinition<T>::DataDrivenDefinition(const std::string& name)
{
	m_name = name;
}

template<typename T>
DataDrivenDefinition<T>::~DataDrivenDefinition(){}

template<typename T>
T* DataDrivenDefinition<T>::getDefinition(const std::string& name)
{
	auto found = DataDrivenDefinition<T>::s_registry.find(name);
	if (found != DataDrivenDefinition<T>::s_registry.end())
	{
		return found->second;
	}
	else
		return nullptr;
}

#include "ThirdParty/XMLParser/XMLParser.hpp"
//////////////////////////////////////////////////////////////////////////
template<typename T>
void DataDrivenDefinition<T>::ParseFromFile(const std::string& filePath)
{
	XMLNode root = XMLNode::openFileHelper(filePath.c_str());
	uint numDefNodes = root.nChildNode();
	for (uint i = 0; i < numDefNodes; i++)
	{
		XMLNode childElement = root.getChildNode(i);
		T* newDef = new T(childElement);
		s_registry[newDef->m_name] = newDef;
	}
}

template<typename T>
T* DataDrivenDefinition<T>::AddNewDefinition(const std::string& name)
{
	T* newDef = new T(name);
	s_registry[newDef->m_name] = newDef;
	return newDef;
}