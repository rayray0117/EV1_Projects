#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "../Math/AABB2.hpp"
//////////////////////////////////////////////////////////////////////////
int ParseXMLAttribute(const XMLNode& element, const char* attributeName, int defaultValue)
{
	return xmltoi(element.getAttribute(attributeName), defaultValue);
}

char ParseXMLAttribute(const XMLNode& element, const char* attributeName, char defaultValue)
{
    std::string valueAsString = xmltoa(element.getAttribute(attributeName));
    if (valueAsString == "")
        return defaultValue;
    return valueAsString[0];
}

bool ParseXMLAttribute(const XMLNode& element, const char* attributeName, bool defaultValue)
{
	std::string valueAsString = xmltoa(element.getAttribute(attributeName));
	if (valueAsString == "")
		return defaultValue;
	else
		return stringToBool(valueAsString);
}

float ParseXMLAttribute(const XMLNode& element, const char* attributeName, float defaultValue)
{
	return (float)xmltof(element.getAttribute(attributeName), defaultValue);
}

Rgba ParseXMLAttribute(const XMLNode& element, const char* attributeName, const Rgba& defaultValue)
{
	
	std::string valueAsString = xmltoa(element.getAttribute(attributeName));
    if(valueAsString == "")
        return defaultValue;


    std::vector<std::string> values;
    splitArgs(valueAsString, values);
    char red = (char)stoi(values[0]);
    char green = (char)stoi(values[1]);
    char blue = (char)stoi(values[2]);
	char alpha = (values.size() == 4) ? (char)stoi(values[3]) : 255;
    Rgba result = Rgba(red, green, blue, alpha);
    return result;
	
}

Vector2 ParseXMLAttribute(const XMLNode& element, const char* attributeName, const Vector2& defaultValue)
{
	std::string valueAsString = xmltoa(element.getAttribute(attributeName));
    if (valueAsString == "")
        return defaultValue;

    std::vector<std::string> values;
    splitArgs(valueAsString, values);
    float x = stof(values[0]);
    float y = stof(values[1]);
    Vector2 result = Vector2(x, y);
    return result;
}

Vector3 ParseXMLAttribute(const XMLNode& element, const char* attributeName, const Vector3& defaultValue)
{
	std::string valueAsString = xmltoa(element.getAttribute(attributeName));
	if (valueAsString == "")
		return defaultValue;

	std::vector<std::string> values;
	splitArgs(valueAsString, values);
	float x = stof(values[0]);
	float y = stof(values[1]);
	float z = stof(values[2]);
	Vector3 result = Vector3(x, y, z);
	return result;
}


IntVector2 ParseXMLAttribute(const XMLNode& element, const char* attributeName, const IntVector2& defaultValue)
{
	std::string valueAsString = xmltoa(element.getAttribute(attributeName));
    if (valueAsString == "")
        return defaultValue;

    std::vector<std::string> values;
    splitArgs(valueAsString, values);
    char x = (char)stoi(values[0]);
    char y = (char)stoi(values[1]);
    IntVector2 result = IntVector2(x, y);
    return result;
}

std::string ParseXMLAttribute(const XMLNode& element, const char* attributeName, const std::string& defaultValue)
{
	return xmltoa(element.getAttribute(attributeName), defaultValue.c_str());
}

void ParseXMLAttribute(const XMLNode& element, const char* attributeName, AABB2& defaultValue)
{
	std::string valueAsString = xmltoa(element.getAttribute(attributeName));
	if (valueAsString == "")
		return;

	std::vector<std::string> values;
	splitArgs(valueAsString, values);
	float min_x = stof(values[0]);
	float min_y = stof(values[1]);
	float max_x = stof(values[2]);
	float max_y = stof(values[3]);
	defaultValue.mins = Vector2(min_x, min_y);
	defaultValue.maxs = Vector2(max_x, max_y);
	return;
}
