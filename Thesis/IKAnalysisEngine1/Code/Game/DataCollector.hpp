#pragma once
#include <fstream>
#include <string>
#include <iostream>
//////////////////////////////////////////////////////////////////////////
class DataCollector
{
public:
	DataCollector(const std::string& filePath);
	~DataCollector();
	
	void WriteData(void* str);
	void WriteData(const std::string& str);
public:
	std::ofstream myfile;
};