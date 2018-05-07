#pragma once
#include <map>
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/General/DataDrivenDefinition.hpp"
//////////////////////////////////////////////////////////////////////////
struct XMLNode;
//////////////////////////////////////////////////////////////////////////
class TileDefinition : public DataDrivenDefinition<TileDefinition>
{
	friend DataDrivenDefinition;
public:
	~TileDefinition() {};
public:
	bool m_isSolid;
	bool m_isOpaque;
    std::vector<Rgba> m_fillColors;
    std::vector<Rgba> m_glyphColors;
    std::vector<char> m_glyphs;
protected:
	TileDefinition(const XMLNode& element);
};