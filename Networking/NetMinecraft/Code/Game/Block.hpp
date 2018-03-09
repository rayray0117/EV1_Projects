#pragma once
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Game\GameCommon.hpp"
#include "Game\BlockDefinition.hpp"
//////////////////////////////////////////////////////////////////////////
class Block
{
public:
	unsigned char m_type;
private:
	unsigned char m_lightingAndFlags;

public:
	unsigned char getBlockType() const;
	void setLightValue(unsigned int newValue);
	unsigned int getLightValue() const;
	void changeType(unsigned char newType);
	void clearLightDirtyFlag();
	void setLightDirtyFlag();
	bool getLightDirtyFlag() const;
	bool isOpaque() const;
	bool isSolid() const;
	bool isSky() const;
	void setSkyFlag();
	void clearSkyFlag();
};
//////////////////////////////////////////////////////////////////////////
inline unsigned char Block::getBlockType() const
{
	return m_type;
}

inline void Block::setLightValue(unsigned int newValue)
{
	ASSERT_OR_DIE(newValue <= LIGHT_MASK, "Passed in an invalid value");
	m_lightingAndFlags &= ~LIGHT_MASK;
	m_lightingAndFlags |= newValue;
}

inline unsigned int Block::getLightValue() const
{
	return m_lightingAndFlags & LIGHT_MASK;
}

inline void Block::changeType(unsigned char newType)
{
	m_lightingAndFlags &= ~OPAQUE_AND_SOLID_MASK;
	const BlockDefinition& blockDef = BlockDefinition::s_blockDefs[newType];
	m_lightingAndFlags |= blockDef.m_solidAndOpaqueBits;
	m_type = newType;
}

inline void Block::clearLightDirtyFlag()
{
	m_lightingAndFlags &= ~LIGHT_DIRTY_MASK;
}

inline void Block::setLightDirtyFlag()
{
	m_lightingAndFlags |= LIGHT_DIRTY_MASK;
}

inline bool Block::getLightDirtyFlag() const
{
	int b = m_lightingAndFlags & LIGHT_DIRTY_MASK;
	return b == LIGHT_DIRTY_MASK;
}

inline bool Block::isOpaque() const
{
	int b = m_lightingAndFlags & OPAQUE_MASK;
	return b == OPAQUE_MASK;
}

inline bool Block::isSolid() const
{
	int b = m_lightingAndFlags & SOLID_MASK;
	return b == SOLID_MASK;
}

inline bool Block::isSky() const
{
	int b = m_lightingAndFlags & IS_SKY_MASK;
	return b == IS_SKY_MASK;
}

inline void Block::setSkyFlag()
{
	m_lightingAndFlags |= IS_SKY_MASK;
}

inline void Block::clearSkyFlag()
{
	m_lightingAndFlags &= ~IS_SKY_MASK;
}