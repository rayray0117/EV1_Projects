#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );



#define C_STRF(x, ...) Stringf(x, __VA_ARGS__).c_str()
