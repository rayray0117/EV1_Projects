#pragma once
#include <vector>
#include <string>
#include "Engine/Core/EngineBase.hpp"
#include "ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////
bool WriteBufferToFile(const std::vector<unsigned char>& buffer,const std::string& filePath);
bool WriteBufferToFileF(const std::vector<float>& buffer, const std::string& filePath);
bool ReadBufferToFile(std::vector<unsigned char>& buffer,const std::string& filePath);
bool ReadBufferToFileF(std::vector<float>& buffer, const std::string& filePath);
//////////////////////////////////////////////////////////////////////////
