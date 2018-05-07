#include "Game/FeatureDefinition.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
FeatureDefinition::FeatureDefinition(const XMLNode& element)
	:DataDrivenDefinition(element)
{
	m_isSolid = ParseXMLAttribute(element, "isSolid", false);
	m_isExit = ParseXMLAttribute(element, "exit", m_isExit);

	Rgba glyphColor = ParseXMLAttribute(element, "glyphColor", Rgba(0, 0, 0, 0));
	Rgba fillColor = ParseXMLAttribute(element, "fillColor", Rgba(0, 0, 0, 0));
	char glyph = ParseXMLAttribute(element, "glyph", char(0));
	ASSERT_OR_DIE(glyph != 0, "Glyph for feature " + m_name + " was unassigned!");

	m_sprite.m_glyph = glyph;
	m_sprite.m_glyphColor = glyphColor;
	m_sprite.m_fillColor = fillColor;
}
