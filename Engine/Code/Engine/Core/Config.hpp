#pragma once
#include <map>
#include <vector>

class Config 
{
public:
	Config();
	~Config();

	// Startup Config System - Setup Initial Configs by parsing supplied file.
	bool ConfigSystemStartup(const char *config_file);
	void ConfigSystemShutdown();

	// Setters - feel free to use std::string
	void ConfigSet(const char *name, const char *value);
	void ConfigSet(const char *name, const int value);
	void ConfigSet(const char *name, const float value);
	void ConfigSet(const char *name, const bool value);

	// Easy check for simple on/off flags. 
	bool IsConfigSet(const char *id);
	void ConfigUnset(const char *id);

	// Fetching.  If a config exists and is 
	// convertible to the desried type, 
	// place its converted value in the out variable,
	// and return true;
	// Otherwise, return false.
	bool ConfigGetString(const char *name, std::string& outv);
	bool ConfigGetBool(const char *name, bool& outv);
	bool ConfigGetInt(const char *name, int& outv);
	bool ConfigGetFloat(const char *name, float& outv);

	// [DEBUG FEATURE] List all currently existing configs.
	void ConfigList();

	// Load a config file
	// see sample.config 
	bool ConfigLoadFile(const char *filename);


private:
	bool performCurrentCharacterState(unsigned int& pos, const std::vector<unsigned char>& buffer, std::string& key); // #ToDo: implement error checking
	void convertTextBoolToStringBool(std::string& value); //Will change written bool values to real ones

	const bool isNumber(const std::string& value) const;
private:
	std::map<std::string, std::string> m_configs;
};

