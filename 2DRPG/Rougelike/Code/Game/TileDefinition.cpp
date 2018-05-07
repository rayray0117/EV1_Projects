#include "Game/TileDefinition.hpp"
#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
//////////////////////////////////////////////////////////////////////////
TileDefinition::TileDefinition(const XMLNode& element)
	:DataDrivenDefinition(element)
{
	ASSERT_OR_DIE(m_name != ERROR_NO_NAME, "ERROR: A Tile name was unassinged");

	m_isSolid = ParseXMLAttribute(element, "isSolid", false);
	m_isOpaque = ParseXMLAttribute(element, "isOpaque", m_isSolid);

    int numFillColors = element.nChildNode("FillColor");
    for (int i = 0; i < numFillColors; i++)
    {
        XMLNode fillColorNode = element.getChildNode("FillColor", i);
        Rgba fillColor = ParseXMLAttribute(fillColorNode, "color", Rgba(0, 0, 0, 0));
        m_fillColors.push_back(fillColor);
    }
    int numGlyphColors = element.nChildNode("GlyphColor");
    for (int i = 0; i < numGlyphColors; i++)
    {
        XMLNode glyphColorNode = element.getChildNode("GlyphColor", i);
        Rgba glyphColor = ParseXMLAttribute(glyphColorNode, "color", Rgba(0,0,0,0));
        m_glyphColors.push_back(glyphColor);
    }

    int numGlyphs = element.nChildNode("Glyph");
    for (int i = 0; i < numGlyphs; i++)
    {
        XMLNode glyphNode = element.getChildNode("Glyph", i);
        char glyph = ParseXMLAttribute(glyphNode, "glyph", (char)0);
        if (glyph == 0)
        {
            ERROR_AND_DIE("ERROR: A Tile glyph was unassigned!");

        }
        m_glyphs.push_back(glyph);
    }
}