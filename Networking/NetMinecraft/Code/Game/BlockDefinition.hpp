#pragma once

#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"
//////////////////////////////////////////////////////////////////////////
enum BlockFaces
{
	EAST,
	WEST,
	NORTH,
	SOUTH,
	TOP,
	BOTTOM,
	NUM_BLOCK_FACES
};
//////////////////////////////////////////////////////////////////////////
class BlockDefinition
{
public:
	BlockDefinition();
	~BlockDefinition();

	static BlockDefinition s_blockDefs[NUM_BLOCK_TYPES];
	static void initializeBlockDefs();

public:
	AABB2 m_textureCoords[NUM_BLOCK_FACES];
	bool m_isSolid;
	bool m_isOpaque;
	unsigned char m_solidAndOpaqueBits;
	int m_selfIllumination;
	std::string m_sfx_dig;
	std::string m_sfx_place;
	std::string m_sfx_step;
	int	  m_num_sfx_dig;
	int	  m_num_sfx_place;
	int	  m_num_sfx_step;
};
