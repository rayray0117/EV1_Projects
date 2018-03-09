#include "Game/DataCollector.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
DataCollector::DataCollector(const std::string& filePath)
{
	DebuggerPrintf(filePath.c_str());
	myfile.open(filePath);
}

DataCollector::~DataCollector()
{
	myfile.close();
}

void DataCollector::WriteData(void* str)
{
	std::string data = (const char*)str;
	//std::string s = *data;
	WriteData(data);
}

void DataCollector::WriteData(const std::string& str)
{
	myfile << str;
}
