#include "Game/ItemDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
//Helper function prototypes
EquipSlot stringToEquipSlot(std::string str);
//////////////////////////////////////////////////////////////////////////
ItemDefinition::ItemDefinition(const XMLNode& element)
	:DataDrivenDefinition(element)
{
	m_name = ParseXMLAttribute(element, "name", (std::string)"ERROR: UNNAMED ITEM!");
	ASSERT_OR_DIE(m_name != ERROR_NO_NAME, "ERROR: A Item name was unassinged");

	std::string eqslot = ParseXMLAttribute(element, "slot", (std::string)"none"); //If not specified make it none
	m_equipSlot = stringToEquipSlot(eqslot);

	// Set stats if there are any.
	XMLNode statsNode = element.getChildNode("Stats");
	int numStats = statsNode.nChildNode();
	for (int i = 0; i < numStats; i++)
	{
		XMLNode statChildNode = statsNode.getChildNode(i);
		m_stats.getStatFromNode(statChildNode);
	}
}

//////////////////////////////////////////////////////////////////////////
// Actual Helper functions
EquipSlot stringToEquipSlot(std::string str)
{
	if (str == "none")
		return EQSLOT_NONE;

	if (str == "armor")
		return EQSLOT_ARMOR;
	if (str == "movement")
		return EQSLOT_MOVEMENT;
	if (str == "weapon")
		return EQSLOT_WEAPON;

	ERROR_AND_DIE("ERROR: " + str + " is not a valid equip slot name. Check spelling and make it all lower case!");
}

