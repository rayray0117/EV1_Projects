#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Chunk.hpp"
//////////////////////////////////////////////////////////////////////////
struct BlockInfo
{
	Chunk* m_chunk;
	int m_blockIndex;

	BlockInfo() :m_chunk(nullptr), m_blockIndex(0) {}
	BlockInfo(Chunk* chunk, int blockIndex) :m_chunk(chunk), m_blockIndex(blockIndex) {}

	const bool Exists() const;

	unsigned char getBlockType() const;
	void clearLightDirty();
	void setLightDirty();
	bool isLightDirty() const;
	bool isOpaque() const;
	bool isSolid() const;
	bool isSky() const;
	void setSkyFlag();
	void clearSkyFlag();

	unsigned int getLightValue() const;
	void setLightValue(unsigned int newValue);
	int getSelfIlluminationValue() const;

	void getSoundPlace(std::string& soundPath, int& numberOfSounds) const;
	void getSoundDig(std::string& soundPath, int& numberOfSounds) const;
	void getSoundStep(std::string& soundPath, int& numberOfSounds) const;

	// Directions
	void moveEast();
	void moveWest();
	void moveNorth();
	void moveSouth();
	void moveTop();
	void moveBottom();

	BlockInfo getBlockInfoToEast() const;
	BlockInfo getBlockInfoToWest() const;
	BlockInfo getBlockInfoToNorth() const;
	BlockInfo getBlockInfoToSouth() const;
	BlockInfo getBlockInfoToTop() const;
	BlockInfo getBlockInfoToBottom() const;
};

//////////////////////////////////////////////////////////////////////////
inline const bool BlockInfo::Exists() const
{
	if (m_chunk)
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
inline unsigned char BlockInfo::getBlockType() const
{
	if (m_chunk == nullptr)
		return AIR;

	return m_chunk->m_blocks[m_blockIndex].getBlockType();
}

inline void BlockInfo::clearLightDirty()
{
	if (m_chunk == nullptr)
		return;

	m_chunk->m_blocks[m_blockIndex].clearLightDirtyFlag();
}
inline void BlockInfo::setLightDirty()
{
	if (m_chunk == nullptr)
		return;

	m_chunk->m_blocks[m_blockIndex].setLightDirtyFlag();
}
inline bool BlockInfo::isLightDirty() const
{
	if (m_chunk == nullptr)
		return true;

	return m_chunk->m_blocks[m_blockIndex].getLightDirtyFlag();
}
inline bool BlockInfo::isOpaque() const
{
	if (m_chunk == nullptr)
		return true;

	bool result = m_chunk->m_blocks[m_blockIndex].isOpaque();
	return result;
}
inline bool BlockInfo::isSolid() const
{
	if (!m_chunk)
		return true;
	if (m_blockIndex > BLOCKS_PER_CHUNK)
		return false;
	if (m_blockIndex < 0)
		return false;

	return m_chunk->m_blocks[m_blockIndex].isSolid();
}
inline bool BlockInfo::isSky() const
{
	if (!m_chunk)
		return false;

	return m_chunk->m_blocks[m_blockIndex].isSky();
}

inline void BlockInfo::setSkyFlag()
{
	if (!m_chunk)
		return;

	m_chunk->m_blocks[m_blockIndex].setSkyFlag();
}

inline void BlockInfo::clearSkyFlag()
{
	if (!m_chunk)
		return;

	m_chunk->m_blocks[m_blockIndex].clearSkyFlag();
}
//////////////////////////////////////////////////////////////////////////
inline unsigned int BlockInfo::getLightValue() const
{
	if (!m_chunk)
		return 0;

	return m_chunk->m_blocks[m_blockIndex].getLightValue();
}
inline void BlockInfo::setLightValue(unsigned int newValue)
{
	if (!m_chunk)
		return;

	m_chunk->m_blocks[m_blockIndex].setLightValue(newValue);
	m_chunk->m_isVertexArrayDirty = true;
}

inline int BlockInfo::getSelfIlluminationValue() const
{
	const BlockDefinition& blockDef = BlockDefinition::s_blockDefs[getBlockType()];
	return blockDef.m_selfIllumination;
}
//////////////////////////////////////////////////////////////////////////
inline void BlockInfo::getSoundPlace(std::string& soundPath, int& numberOfSounds) const
{
	const BlockDefinition& blockDef = BlockDefinition::s_blockDefs[getBlockType()];
	soundPath = blockDef.m_sfx_place;
	numberOfSounds = blockDef.m_num_sfx_place;
}
inline void BlockInfo::getSoundDig(std::string& soundPath, int& numberOfSounds) const
{
	const BlockDefinition& blockDef = BlockDefinition::s_blockDefs[getBlockType()];
	soundPath = blockDef.m_sfx_dig;
	numberOfSounds = blockDef.m_num_sfx_dig;
}
inline void BlockInfo::getSoundStep(std::string& soundPath, int& numberOfSounds) const
{
	const BlockDefinition& blockDef = BlockDefinition::s_blockDefs[getBlockType()];
	soundPath = blockDef.m_sfx_step;
	numberOfSounds = blockDef.m_num_sfx_step;
}
//////////////////////////////////////////////////////////////////////////
inline void BlockInfo::moveEast()
{
	if (m_chunk == nullptr)
		return;

	if ((m_blockIndex & X_MASK_BITS) == X_MASK_BITS)
	{
		m_chunk = m_chunk->m_eastChunk;
		m_blockIndex &= ~X_MASK_BITS;
	}
	else
	{
		++m_blockIndex;
	}
}
inline void BlockInfo::moveWest()
{
	if (m_chunk == nullptr)
		return;

	if ((m_blockIndex & X_MASK_BITS) == 0)
	{
		m_chunk = m_chunk->m_westChunk;
		m_blockIndex |= X_MASK_BITS;
	}
	else
	{
		--m_blockIndex;
	}
}
inline void BlockInfo::moveNorth()
{
	if (m_chunk == nullptr)
		return;

	if ((m_blockIndex & Y_MASK_BITS) == Y_MASK_BITS)
	{
		m_chunk = m_chunk->m_northChunk;
		m_blockIndex &= ~Y_MASK_BITS;
	}
	else
	{
		m_blockIndex += CHUNK_X;
	}
}
inline void BlockInfo::moveSouth()
{
	if (m_chunk == nullptr)
		return;

	if ((m_blockIndex & Y_MASK_BITS) == 0)
	{
		m_chunk = m_chunk->m_southChunk;
		m_blockIndex |= Y_MASK_BITS;
	}
	else
	{
		m_blockIndex -= CHUNK_X;
	}
}
inline void BlockInfo::moveTop()
{
	if (m_chunk == nullptr)
		return;

	if ((m_blockIndex & Z_MASK_BITS) == Z_MASK_BITS)
	{
		m_chunk = nullptr;
	}
	else
	{
		m_blockIndex += BLOCKS_PER_LAYER;
	}
}
inline void BlockInfo::moveBottom()
{
	if (m_chunk == nullptr)
		return;

	if ((m_blockIndex & Z_MASK_BITS) == 0)
	{
		m_chunk = nullptr;
	}
	else
	{
		m_blockIndex -= BLOCKS_PER_LAYER;
	}
}

//////////////////////////////////////////////////////////////////////////
inline BlockInfo BlockInfo::getBlockInfoToEast() const
{
	BlockInfo bi;
	if ((m_blockIndex & X_MASK_BITS) == X_MASK_BITS)
	{
		bi.m_chunk = m_chunk->m_eastChunk;
		bi.m_blockIndex = (m_blockIndex & ~X_MASK_BITS);
	}
	else
	{
		bi.m_chunk = m_chunk;
		bi.m_blockIndex = m_blockIndex + 1;
	}
	return bi;
}
inline BlockInfo BlockInfo::getBlockInfoToWest() const
{
	BlockInfo bi;
	if ((m_blockIndex & X_MASK_BITS) == 0)
	{
		bi.m_chunk = m_chunk->m_westChunk;
		bi.m_blockIndex = (m_blockIndex | X_MASK_BITS);
	}
	else
	{
		bi.m_chunk = m_chunk;
		bi.m_blockIndex = m_blockIndex - 1;
	}
	return bi;
}
inline BlockInfo BlockInfo::getBlockInfoToNorth() const
{
	BlockInfo bi;
	if ((m_blockIndex & Y_MASK_BITS) == Y_MASK_BITS)
	{
		bi.m_chunk = m_chunk->m_northChunk;
		bi.m_blockIndex = (m_blockIndex & ~Y_MASK_BITS);
	}
	else
	{
		bi.m_chunk = m_chunk;
		bi.m_blockIndex = m_blockIndex + CHUNK_X;
	}
	return bi;
}
inline BlockInfo BlockInfo::getBlockInfoToSouth() const
{
	BlockInfo bi;
	if ((m_blockIndex & Y_MASK_BITS) == 0)
	{
		bi.m_chunk = m_chunk->m_southChunk;
		bi.m_blockIndex = (m_blockIndex | Y_MASK_BITS);
	}
	else
	{
		bi.m_chunk = m_chunk;
		bi.m_blockIndex = m_blockIndex - CHUNK_X;
	}
	return bi;
}
inline BlockInfo BlockInfo::getBlockInfoToTop() const
{
	BlockInfo bi;
	if ((m_blockIndex & Z_MASK_BITS) == Z_MASK_BITS)
	{
		bi.m_chunk = nullptr;
	}
	else
	{
		bi.m_chunk = m_chunk;
		bi.m_blockIndex = m_blockIndex + BLOCKS_PER_LAYER;
	}
	return bi;
}
inline BlockInfo BlockInfo::getBlockInfoToBottom() const
{
	BlockInfo bi;
	if ((m_blockIndex & Z_MASK_BITS) == 0)
	{
		bi.m_chunk = nullptr;
	}
	else
	{
		bi.m_chunk = m_chunk;
		bi.m_blockIndex = m_blockIndex - BLOCKS_PER_LAYER;
	}
	return bi;
}