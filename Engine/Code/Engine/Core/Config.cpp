#include "Engine/Core/Config.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <cctype>
#include "Engine/Core/ParseUtils.hpp"
//////////////////////////////////////////////////////////////////////////
Config::Config()
{

}

Config::~Config()
{
	ConfigSystemShutdown();
}
//////////////////////////////////////////////////////////////////////////
bool Config::ConfigSystemStartup(const char *config_file)
{
	return ConfigLoadFile(config_file);
}

void Config::ConfigSystemShutdown()
{

}

void Config::ConfigSet(const char *name, const char *value)
{
	m_configs[name] = value;
}

void Config::ConfigSet(const char *name, const int value)
{
	m_configs[name] = std::to_string(value);
}

void Config::ConfigSet(const char *name, const float value)
{
	m_configs[name] = std::to_string(value);
}

void Config::ConfigSet(const char *name, const bool value)
{
	if (value)
	{
		m_configs[name] = "1";
	}
	else
	{
		m_configs[name] = "0";
	}
}

bool Config::ConfigGetString(const char *name, std::string& outv)
{
	std::map<std::string, std::string>::iterator found = m_configs.find(name);
	if (found != m_configs.end())
	{
		outv = found->second;
		return true;
	}

	return false;
}

bool Config::ConfigGetBool(const char *name, bool& outv)
{
	std::string value;
	if (!ConfigGetString(name, value))
	{
		return false;
	}
	
	// Convert it's held value to a bool
	if (value == "1")
	{
		outv = true;
		return true;
	}
	if (value == "0")
	{
		outv = false;
		return true;
	}

	return false;
}

bool Config::ConfigGetInt(const char *name, int& outv)
{
	std::string value;
	if (!ConfigGetString(name, value))
	{
		return false;
	}

	if (isNumber(value))
	{
		float num = std::stof(value, 0);
		outv = (int)num;
		return true;
	}
	
	return false;
}

bool Config::ConfigGetFloat(const char *name, float& outv)
{
	std::string value;
	if (!ConfigGetString(name, value))
	{
		return false;
	}

	if (isNumber(value))
	{
		float num = std::stof(value, 0);
		outv = num;
		return true;
	}

	return false;
}

void Config::ConfigList()
{
	DebuggerPrintf("\n Config List: \n");
	for (std::map<std::string, std::string>::const_iterator iterator = m_configs.begin(); iterator != m_configs.end(); iterator++)
	{
		std::string key = iterator->first;
		std::string value = iterator->second;
		DebuggerPrintf("   %s = %s \n", key.c_str(), value.c_str());
	}
}

bool Config::ConfigLoadFile(const char *filename)
{
	bool success = true;

	std::vector<unsigned char> buffer;
	success &= ReadBufferToFile(buffer, filename);

	if (!success)
		return false;

	std::string key;
	//std::string value;

	unsigned int bufferIndex = 0;

	for (; bufferIndex < buffer.size(); /*++bufferIndex*/)
	{
		success &= performCurrentCharacterState(bufferIndex, buffer, key);
	}

	return success;
}

////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////////////////
bool Config::performCurrentCharacterState(unsigned int& pos, const std::vector<unsigned char>& buffer, std::string& key)
{
	if (!key.empty())
	{
		skipEqualsSign(pos, buffer);
		std::string value = getValueFromBuffer(pos, buffer);
		convertTextBoolToStringBool(value);
		m_configs[key] = value;

		key.clear();
		return true;
	}

	const unsigned char c = buffer[pos];

	if (c == '#')
	{
		skipLine(pos, buffer);
		return true;
	}

	if (c == '+')
	{
		++pos;
		key = getKeyFromBuffer(pos, buffer);
		
		m_configs[key] = "1";
		key.clear();
		return true;
	}
	if (c == '-')
	{
		++pos;
		key = getKeyFromBuffer(pos, buffer);

		m_configs[key] = "0";
		key.clear();
		return true;
	}

	if (isWhiteSpace(c))
	{
		skipWhiteSpace(pos, buffer);
		return true;
	}

	if (pos < buffer.size())
	{
		key = getKeyFromBuffer(pos, buffer);
		return true;
	}

	return true; // Don't know how you got here but it's all good
}

void Config::convertTextBoolToStringBool(std::string& value)
{
	if (value == "false" || value == "FALSE" || value == "False")
	{
		value = "0";
		return;
	}

	if (value == "true" || value == "TRUE" || value == "True")
	{
		value = "1";
		return;
	}
}

const bool Config::isNumber(const std::string& value) const  
{
	if (value.empty())
		return false;

	std::vector<std::string> values;
	splitByChar(value, values, '.');
	if (values.size() > 2)
		return false;

	for (uint i = 0; i < values.size(); ++i)
	{
		std::string::const_iterator it = values[i].begin();
		while (it != values[i].end() && std::isdigit(*it))
			++it;
		if (it != values[i].end())
			return false;
	}

	return true;
}
