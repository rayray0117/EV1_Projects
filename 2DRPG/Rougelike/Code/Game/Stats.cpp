#include "Game/Stats.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "ThirdParty/XMLParser/XMLParser.hpp"
//////////////////////////////////////////////////////////////////////////
// Helper function prototypes
StatName stringToStatName(const std::string& str);
//////////////////////////////////////////////////////////////////////////
void Stats::getStatFromNode(const XMLNode& node)
{
	StatName name = stringToStatName(node.getName());

	m_stats[name] = ParseXMLAttribute(node, "value", 0);
	ASSERT_RECOVERABLE(m_stats[name] != 0, "WARNING: " + (std::string)node.getName() + " wasn't actually set to a value!");
}
//////////////////////////////////////////////////////////////////////////
// Helper functions
StatName stringToStatName(const std::string& str)
{
	if (str == "Health")
		return STAT_HEALTH;
	if (str == "Stamina")
		return STAT_STAMINA;
	if (str == "Movement")
		return STAT_MOVEMENT;
	if (str == "Speed")
		return STAT_SPEED;
	if (str == "Attack")
		return STAT_ATTACK;
	if (str == "Defense")
		return STAT_DEFENSE;
	if (str == "Magic")
		return STAT_MAGIC;
	if (str == "Sight")
		return STAT_SIGHT;

	ERROR_AND_DIE("ERROR: " + str + " is not a valid stat. Check spelling!");
}

