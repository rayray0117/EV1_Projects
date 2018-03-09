#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../Math/Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
void skipLine(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	while ((pos < buffer.size()) && !isEndline(buffer[pos]))
	{
		++pos;
	}
	while ((pos < buffer.size()) && isEndline(buffer[pos]))
	{
		++pos; //Don't stay on the endline;
	}
}

void skipWord(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	while ((pos < buffer.size()) && !isWhiteSpace(buffer[pos]))
	{
		++pos;
	}
	skipWhiteSpace(pos, buffer);
}

void skipWhiteSpace(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	while ((pos < buffer.size()) && isWhiteSpace(buffer[pos]))
	{
		++pos;
	}
}

void skipWhiteSpace(unsigned int& pos, const std::string& buffer)
{
	while ((pos < buffer.size()) && isWhiteSpace(buffer[pos]))
	{
		++pos;
	}
}

const bool isWhiteSpace(unsigned char c)
{
	if (c == ' ')
		return true;

	return isEndline(c);
}

bool isCStyleCommentEnd(uchar current, uchar next)
{
	return (current == '*' && next == '/');
}

bool isCStyleCommentBegin(uchar current, uchar next)
{
	return (current == '/' && next == '*');
}

bool isCPPStyleComment(uchar current, uchar next)
{
	return (current == '/' && next == '/');
}

void skipCStyleComment(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	++pos;
	++pos;
	while (pos < buffer.size() && (pos + 1) < buffer.size() && !isCStyleCommentEnd(buffer[pos], buffer[pos + 1]))
	{
		++pos;
	}
	++pos; //Don't stay on the end comment;
	++pos;
}

std::string getQuote(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	std::string result;
	++pos; //Get off of our initial "

	while (buffer[pos] != '"')
	{
		result += buffer[pos];
		++pos;
	}
	++pos; //Get off of the end "

	return result;
}

std::string getQuote(unsigned int& pos, const std::string& buffer)
{
	std::string result;
	++pos; //Get off of our initial "

	while (buffer[pos] != '"')
	{
		result += buffer[pos];
		++pos;
	}
	++pos; //Get off of the end "

	return result;
}

std::string getQuoteWithQuote(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	std::string result;
	result += buffer[pos];
	++pos; //Get off of our initial "

	while (buffer[pos] != '"')
	{
		if (buffer[pos] == 92 && buffer[pos+1] == '\"') // if \ //
		{
			result += buffer[pos];
			++pos;
		}

		result += buffer[pos];
		++pos;
	}
	result += buffer[pos];
	++pos; //Get off of the end "

	return result;
}

std::string getKeyFromBuffer(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	if (buffer[pos] == '"')
		return getQuote(pos, buffer);
	else
	{
		std::string key;
		while ((pos < buffer.size()) && (!isWhiteSpace(buffer[pos]) && buffer[pos] != '='))
		{
			key += buffer[pos];
			++pos;
		}
		return key;
	}
}

std::string getValueFromBuffer(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	skipWhiteSpace(pos, buffer);
	if (pos >= buffer.size())
	{
		std::string value;
		return value;
	}
	if (buffer[pos] == '"')
		return getQuote(pos, buffer);
	else
	{
		std::string value;
		while ((pos < buffer.size()) && !isWhiteSpace(buffer[pos]))
		{
			value += buffer[pos];
			++pos;
		}
		return value;
	}
}

std::string getValueFromBuffer(unsigned int& pos, const std::string& buffer)
{
	skipWhiteSpace(pos, buffer);
	if (buffer[pos] == '"')
		return getQuote(pos, buffer);
	else
	{
		std::string value;
		while ((pos < buffer.size()) && !isWhiteSpace(buffer[pos]))
		{
			value += buffer[pos];
			++pos;
		}
		return value;
	}
}

void skipEqualsSign(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	skipWhiteSpace(pos, buffer);
	if (buffer[pos] == '=')
	{
		++pos;
	}
}

const bool isEndline(unsigned char c) 
{
	if (c == '\r')
		return true;
	if (c == '\n')
		return true;

	return false;
}

void splitCommandAndArgs(const std::string& buffer, std::string& cmd, std::vector<std::string>& args)
{
	unsigned int pos = 0;
	while (pos < buffer.size() && !isWhiteSpace(buffer[pos]))
	{
		cmd += buffer[pos];
		++pos;
	}
	while (pos < buffer.size())
	{
		args.push_back(getValueFromBuffer(pos, buffer));
	}
}

void splitArgs(const std::string& buffer, std::vector<std::string>& args)
{
	unsigned int pos = 0;
	while (pos < buffer.size())
	{
		args.push_back(getValueFromBuffer(pos, buffer));
	}
}

void splitByChar(const std::string& buffer, std::vector<std::string>& out, uchar character)
{
	// Extract the command part from the arguments part.
	std::string line = buffer;
	size_t first_space = line.find(character);
	
	if (first_space == line.npos)
	{
		out.push_back(buffer); //Didn't find anything that need to be seperated out so return the original
		return;
	}
	
	while (first_space != line.npos)
	{
		out.push_back(line.substr(0, first_space));
		line = line.substr(first_space + 1U);
		first_space = line.find(character);
	}

	out.push_back(line);
}

std::string getLine(unsigned int& pos, const std::vector<unsigned char>& buffer)
{
	std::string line;
	while ((pos < buffer.size()) && !isEndline(buffer[pos]))
	{
		line += buffer[pos];
		++pos;
	}
	while ((pos < buffer.size()) && isEndline(buffer[pos]))
	{
		++pos; //Don't stay on the endline;
	}


	return line;
}

bool stringToBool(std::string str)
{
	if (str == "false" || str == "FALSE" || str == "False" || str == "0")
	{
		return false;
	}

	if (str == "true" || str == "TRUE" || str == "True" || str == "1")
	{
		return true;
	}

	ERROR_AND_DIE(str + " is cannot be converted to a bool");
}

const std::string boolToString(bool value)
{
	if (value)
		return std::string("Yes");
	else
		return std::string("No");
}

const std::string bytesToString(size_t bytes, uint precision)
{
	double outNumber = bytes;
	const char* measurement;

	if (bytes >= 2 GiB)
	{
		measurement = "GiB";
		outNumber *= M_GiB_I;
	}
	else if (bytes >= 2 MiB)
	{
		measurement = "MiB";
		outNumber *= M_MiB_I;
	}
	else if (bytes >= 2 KiB)
	{
		measurement = "KiB";
		outNumber *= M_KiB_I;
	}
	else
	{
		measurement = "B";
	}

	return Stringf("%.*f %s", precision, outNumber, measurement);
}

const std::string timeToString(double time)
{
	if(time >= 1.0)
		return Stringf("%.4f %s", time, "s");
	
	time *= 1000.0;

	if(time >= 1.0)
		return Stringf("%.4f %s", time, "ms");

	time *= 1000.0;
	return Stringf("%.0f %s", time, "us");
}
	
const std::string vectorToString(const Vector3& value)
{
	return Stringf("%f %f %f", value.x, value.y, value.z);
}
