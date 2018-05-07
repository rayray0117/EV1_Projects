#include "Game/PawnDefinition.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Game/Behavior.hpp"
#include "Game/FleeBehavior.hpp"
#include "Game/PursueBehavior.hpp"
#include "Game/WanderBehavior.hpp"
//////////////////////////////////////////////////////////////////////////
const std::string ERROR_NO_FACTION = "ERROR_NO_FACTION";
//////////////////////////////////////////////////////////////////////////
PawnDefinition::PawnDefinition(const XMLNode& element)
	: DataDrivenDefinition(element)
{
	ASSERT_OR_DIE(m_name != ERROR_NO_NAME, "ERROR: A Pawn definition name was unassinged");

	m_faction = ParseXMLAttribute(element, "faction", ERROR_NO_FACTION);
	ASSERT_OR_DIE(m_faction != ERROR_NO_FACTION, "ERROR: " + m_name + " has no faction!");

	XMLNode spriteNode = element.getChildNode("Sprite");
	Rgba glyphColor = ParseXMLAttribute(spriteNode, "glyphColor", Rgba(0, 0, 0, 0));
	Rgba fillColor = ParseXMLAttribute(spriteNode, "fillColor", Rgba(0, 0, 0, 0));
	char glyph = ParseXMLAttribute(spriteNode, "glyph", char(0));
	ASSERT_OR_DIE(glyph != 0, "Glyph for feature " + m_name + " was unassigned!");

	m_sprite.m_glyph = glyph;
	m_sprite.m_glyphColor = glyphColor;
	m_sprite.m_fillColor = fillColor;

	// Set stats if there are any.
	XMLNode statsNode = element.getChildNode("Stats");
	int numStats = statsNode.nChildNode();
	for (int i = 0; i < numStats; i++)
	{
		XMLNode statChildNode = statsNode.getChildNode(i);
		m_initialStats.getStatFromNode(statChildNode);
	}

	XMLNode behaviorNode = element.getChildNode("Behaviors");
	int numBehaviors = behaviorNode.nChildNode();
	for (int i = 0; i < numBehaviors; i++)
	{
		XMLNode newBehaviorNode = behaviorNode.getChildNode(i);
		m_behaviors.push_back(Behavior::CreateBehavior(newBehaviorNode));
	}

	XMLNode soundFXNode = element.getChildNode("SoundFX");
	if (!soundFXNode.IsContentEmpty())
	{
		m_tookDamageSound = ParseXMLAttribute(soundFXNode, "tookDamage", m_tookDamageSound);
	}
}

PawnDefinition::~PawnDefinition()
{
	for (Behavior* current : m_behaviors)
	{
		SAFE_DELETE(current);
	}
}