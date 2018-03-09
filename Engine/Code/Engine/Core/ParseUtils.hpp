#pragma once
#include <vector>
#include <string>
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class Vector3;
//////////////////////////////////////////////////////////////////////////
void skipLine(unsigned int& pos, const std::vector<unsigned char>& buffer);

void skipWord(unsigned int& pos, const std::vector<unsigned char>& buffer);

void skipWhiteSpace(unsigned int& pos, const std::vector<unsigned char>& buffer);
void skipWhiteSpace(unsigned int& pos, const std::string& buffer);
const bool isWhiteSpace(unsigned char c);

bool isCStyleCommentEnd(uchar current, uchar next);
bool isCStyleCommentBegin(uchar current, uchar next);
bool isCPPStyleComment(uchar current, uchar next);
void skipCStyleComment(unsigned int& pos, const std::vector<unsigned char>& buffer);

void skipEqualsSign(unsigned int& pos, const std::vector<unsigned char>& buffer);

std::string getKeyFromBuffer(unsigned int& pos, const std::vector<unsigned char>& buffer);
std::string getValueFromBuffer(unsigned int& pos, const std::vector<unsigned char>& buffer);
std::string getValueFromBuffer(unsigned int& pos, const std::string&  buffer);

const bool isEndline(unsigned char c);

std::string getQuote(unsigned int& pos, const std::vector<unsigned char>& buffer);
std::string getQuote(unsigned int& pos, const std::string& buffer);
std::string getQuoteWithQuote(unsigned int& pos, const std::vector<unsigned char>& buffer);


void splitCommandAndArgs(const std::string& buffer, std::string& cmd, std::vector<std::string>& args);
void splitArgs(const std::string& buffer, std::vector<std::string>& args);
void splitByChar(const std::string& buffer, std::vector<std::string>& out, uchar character);

std::string getLine(unsigned int& pos, const std::vector<unsigned char>& buffer);

bool stringToBool(std::string str);
const std::string boolToString(bool value);
const std::string bytesToString(size_t bytes, uint precision = 2);
const std::string timeToString(double time);
const std::string vectorToString(const Vector3& value);