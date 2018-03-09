#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include <string>
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
class AABB2;
//////////////////////////////////////////////////////////////////////////
int ParseXMLAttribute(const XMLNode& element, const char* attributeName, int defaultValue);
char ParseXMLAttribute(const XMLNode& element, const char* attributeName, char defaultValue);
bool ParseXMLAttribute(const XMLNode& element, const char* attributeName, bool defaultValue);
float ParseXMLAttribute(const XMLNode& element, const char* attributeName, float defaultValue);
Rgba ParseXMLAttribute(const XMLNode& element, const char* attributeName, const Rgba& defaultValue);
Vector2 ParseXMLAttribute(const XMLNode& element, const char* attributeName, const Vector2& defaultValue);
Vector3 ParseXMLAttribute(const XMLNode& element, const char* attributeName, const Vector3& defaultValue);
IntVector2 ParseXMLAttribute(const XMLNode& element, const char* attributeName, const IntVector2& defaultValue);
std::string ParseXMLAttribute(const XMLNode& element, const char* attributeName, const std::string& defaultValue);
void ParseXMLAttribute(const XMLNode& element, const char* attributeName, AABB2& defaultValue);