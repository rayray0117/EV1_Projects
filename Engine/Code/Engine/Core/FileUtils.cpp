#include "Engine/Core/FileUtils.hpp"
//////////////////////////////////////////////////////////////////////////
bool WriteBufferToFile(const std::vector<unsigned char>& buffer, const std::string& filePath)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb");
	if (errorCode != 0)
		return false;
	/*size_t numBytesWritten = */fwrite(buffer.data(), 1, buffer.size(), file);
	fclose(file);
	return true;
}

bool WriteBufferToFileF(const std::vector<float>& buffer, const std::string& filePath)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb");
	if (errorCode != 0)
		return false;
	/*size_t numBytesWritten = */fwrite(buffer.data(), 4, buffer.size(), file);
	fclose(file);
	return true;
}

bool ReadBufferToFile(std::vector<unsigned char>& buffer, const std::string& filePath)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");
	if (errorCode != 0)
		return false;
	fseek(file, 0, SEEK_END);
	size_t numBytes = ftell(file);
	buffer.resize(numBytes);
	fseek(file, 0, SEEK_SET);
	/*size_t numBytesRead = */fread_s(buffer.data(), buffer.size(), 1, numBytes, file);
	fclose(file);
	return true;
}

bool ReadBufferToFileF(std::vector<float>& buffer, const std::string& filePath)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");
	if (errorCode != 0)
		return false;
	fseek(file, 0, SEEK_END);
	size_t numBytes = ftell(file);
	buffer.resize(numBytes);
	fseek(file, 0, SEEK_SET);
	/*size_t numBytesRead = */fread_s(buffer.data(), buffer.size(), 4, numBytes, file);
	fclose(file);
	return true;
}
