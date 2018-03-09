#include "Game/BlockDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
//////////////////////////////////////////////////////////////////////////
BlockDefinition::BlockDefinition()
{
}

BlockDefinition::~BlockDefinition()
{
}
//////////////////////////////////////////////////////////////////////////
BlockDefinition BlockDefinition::s_blockDefs[NUM_BLOCK_TYPES];
//////////////////////////////////////////////////////////////////////////
void BlockDefinition::initializeBlockDefs()
{
	// Air Block
	s_blockDefs[AIR].m_textureCoords[TOP] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(0, 0);
	s_blockDefs[AIR].m_textureCoords[BOTTOM] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(0, 0);
	s_blockDefs[AIR].m_textureCoords[NORTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(0, 0);
	s_blockDefs[AIR].m_textureCoords[SOUTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(0, 0);
	s_blockDefs[AIR].m_textureCoords[EAST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(0, 0);
	s_blockDefs[AIR].m_textureCoords[WEST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(0, 0);
	s_blockDefs[AIR].m_isSolid = false;
	s_blockDefs[AIR].m_isOpaque = false;
	s_blockDefs[AIR].m_solidAndOpaqueBits = 0;
	s_blockDefs[AIR].m_selfIllumination = 0;
	s_blockDefs[AIR].m_sfx_dig = "";
	s_blockDefs[AIR].m_num_sfx_dig = 0;
	s_blockDefs[AIR].m_sfx_place = "";
	s_blockDefs[AIR].m_num_sfx_place = 0;
	s_blockDefs[AIR].m_sfx_step = "";
	s_blockDefs[AIR].m_num_sfx_step = 0;

	// Grass Block
	s_blockDefs[GRASS].m_textureCoords[TOP] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(9, 8);
	s_blockDefs[GRASS].m_textureCoords[BOTTOM] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(7, 8);
	s_blockDefs[GRASS].m_textureCoords[NORTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(8, 8);
	s_blockDefs[GRASS].m_textureCoords[SOUTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(8, 8);
	s_blockDefs[GRASS].m_textureCoords[EAST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(8, 8);
	s_blockDefs[GRASS].m_textureCoords[WEST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(8, 8);
	s_blockDefs[GRASS].m_isSolid = true;
	s_blockDefs[GRASS].m_isOpaque = true;
	s_blockDefs[GRASS].m_solidAndOpaqueBits = OPAQUE_AND_SOLID_MASK;
	s_blockDefs[GRASS].m_selfIllumination = 0;
	s_blockDefs[GRASS].m_sfx_dig = SFX_GRASS_DIG;
	s_blockDefs[GRASS].m_num_sfx_dig = SFX_GRASS_DIG_NUM;
	s_blockDefs[GRASS].m_sfx_place = SFX_GRASS_DIG;
	s_blockDefs[GRASS].m_num_sfx_place = SFX_GRASS_DIG_NUM;
	s_blockDefs[GRASS].m_sfx_step = SFX_GRASS_STEP;
	s_blockDefs[GRASS].m_num_sfx_step = SFX_GRASS_STEP_NUM;

	// Stone Block
	s_blockDefs[STONE].m_textureCoords[TOP] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(2, 10);
	s_blockDefs[STONE].m_textureCoords[BOTTOM] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(2, 10);
	s_blockDefs[STONE].m_textureCoords[NORTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(2, 10);
	s_blockDefs[STONE].m_textureCoords[SOUTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(2, 10);
	s_blockDefs[STONE].m_textureCoords[EAST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(2, 10);
	s_blockDefs[STONE].m_textureCoords[WEST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(2, 10);
	s_blockDefs[STONE].m_isSolid = true;
	s_blockDefs[STONE].m_isOpaque = true;
	s_blockDefs[STONE].m_solidAndOpaqueBits = OPAQUE_AND_SOLID_MASK;
	s_blockDefs[STONE].m_selfIllumination = 0;
	s_blockDefs[STONE].m_sfx_dig = SFX_STONE_DIG;
	s_blockDefs[STONE].m_num_sfx_dig = SFX_STONE_DIG_NUM;
	s_blockDefs[STONE].m_sfx_place = SFX_STONE_DIG;
	s_blockDefs[STONE].m_num_sfx_place = SFX_STONE_DIG_NUM;
	s_blockDefs[STONE].m_sfx_step = SFX_STONE_STEP;
	s_blockDefs[STONE].m_num_sfx_step = SFX_STONE_STEP_NUM;

	// Dirt Block
	s_blockDefs[DIRT].m_textureCoords[TOP] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(7, 8);
	s_blockDefs[DIRT].m_textureCoords[BOTTOM] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(7, 8);
	s_blockDefs[DIRT].m_textureCoords[NORTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(7, 8);
	s_blockDefs[DIRT].m_textureCoords[SOUTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(7, 8);
	s_blockDefs[DIRT].m_textureCoords[EAST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(7, 8);
	s_blockDefs[DIRT].m_textureCoords[WEST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(7, 8);
	s_blockDefs[DIRT].m_isSolid = true;
	s_blockDefs[DIRT].m_isOpaque = true;
	s_blockDefs[DIRT].m_solidAndOpaqueBits = OPAQUE_AND_SOLID_MASK;
	s_blockDefs[DIRT].m_selfIllumination = 0;
	s_blockDefs[DIRT].m_sfx_dig = SFX_GRASS_DIG;
	s_blockDefs[DIRT].m_num_sfx_dig = SFX_GRASS_DIG_NUM;
	s_blockDefs[DIRT].m_sfx_place = SFX_GRASS_DIG;
	s_blockDefs[DIRT].m_num_sfx_place = SFX_GRASS_DIG_NUM;
	s_blockDefs[DIRT].m_sfx_step = SFX_GRASS_STEP;
	s_blockDefs[DIRT].m_num_sfx_step = SFX_GRASS_STEP_NUM;

	// Sand Block
	s_blockDefs[SAND].m_textureCoords[TOP] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(1, 8);
	s_blockDefs[SAND].m_textureCoords[BOTTOM] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(1, 8);
	s_blockDefs[SAND].m_textureCoords[NORTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(1, 8);
	s_blockDefs[SAND].m_textureCoords[SOUTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(1, 8);
	s_blockDefs[SAND].m_textureCoords[EAST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(1, 8);
	s_blockDefs[SAND].m_textureCoords[WEST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(1, 8);
	s_blockDefs[SAND].m_isSolid = true;
	s_blockDefs[SAND].m_isOpaque = true;
	s_blockDefs[SAND].m_solidAndOpaqueBits = OPAQUE_AND_SOLID_MASK;
	s_blockDefs[SAND].m_selfIllumination = 0;
	s_blockDefs[SAND].m_sfx_dig = SFX_SAND_DIG;
	s_blockDefs[SAND].m_num_sfx_dig = SFX_SAND_DIG_NUM;
	s_blockDefs[SAND].m_sfx_place = SFX_SAND_DIG;
	s_blockDefs[SAND].m_num_sfx_place = SFX_SAND_DIG_NUM;
	s_blockDefs[SAND].m_sfx_step = SFX_SAND_STEP;
	s_blockDefs[SAND].m_num_sfx_step = SFX_SAND_STEP_NUM;

	// Water Block
	s_blockDefs[WATER].m_textureCoords[TOP] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(15, 11);
	s_blockDefs[WATER].m_textureCoords[BOTTOM] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(15, 11);
	s_blockDefs[WATER].m_textureCoords[NORTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(15, 11);
	s_blockDefs[WATER].m_textureCoords[SOUTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(15, 11);
	s_blockDefs[WATER].m_textureCoords[EAST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(15, 11);
	s_blockDefs[WATER].m_textureCoords[WEST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(15, 11);
	s_blockDefs[WATER].m_isSolid = true;
	s_blockDefs[WATER].m_isOpaque = true; //Keep it opaque for now because it looks really weird otherwise.
	s_blockDefs[WATER].m_solidAndOpaqueBits = OPAQUE_AND_SOLID_MASK;
	s_blockDefs[WATER].m_selfIllumination = 0;
	s_blockDefs[WATER].m_sfx_dig = "";
	s_blockDefs[WATER].m_num_sfx_dig = 0;
	s_blockDefs[WATER].m_sfx_place = "";
	s_blockDefs[WATER].m_num_sfx_place = 0;
	s_blockDefs[WATER].m_sfx_step = SFX_SWIM_STEP;
	s_blockDefs[WATER].m_num_sfx_step = SFX_SWIM_STEP_NUM;

	// Cobblestone Block
	s_blockDefs[COBBLESTONE].m_textureCoords[TOP] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(9, 10);
	s_blockDefs[COBBLESTONE].m_textureCoords[BOTTOM] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(9, 10);
	s_blockDefs[COBBLESTONE].m_textureCoords[NORTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(9, 10);
	s_blockDefs[COBBLESTONE].m_textureCoords[SOUTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(9, 10);
	s_blockDefs[COBBLESTONE].m_textureCoords[EAST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(9, 10);
	s_blockDefs[COBBLESTONE].m_textureCoords[WEST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(9, 10);
	s_blockDefs[COBBLESTONE].m_isSolid = true;
	s_blockDefs[COBBLESTONE].m_isOpaque = true;
	s_blockDefs[COBBLESTONE].m_solidAndOpaqueBits = OPAQUE_AND_SOLID_MASK;
	s_blockDefs[COBBLESTONE].m_selfIllumination = 0;
	s_blockDefs[COBBLESTONE].m_sfx_dig = SFX_STONE_DIG;
	s_blockDefs[COBBLESTONE].m_num_sfx_dig = SFX_STONE_DIG_NUM;
	s_blockDefs[COBBLESTONE].m_sfx_place = SFX_STONE_DIG;
	s_blockDefs[COBBLESTONE].m_num_sfx_place = SFX_STONE_DIG_NUM;
	s_blockDefs[COBBLESTONE].m_sfx_step = SFX_STONE_STEP;
	s_blockDefs[COBBLESTONE].m_num_sfx_step = SFX_STONE_STEP_NUM;

	// Glowstone Block
	s_blockDefs[GLOWSTONE].m_textureCoords[TOP] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(4, 11);
	s_blockDefs[GLOWSTONE].m_textureCoords[BOTTOM] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(4, 11);
	s_blockDefs[GLOWSTONE].m_textureCoords[NORTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(4, 11);
	s_blockDefs[GLOWSTONE].m_textureCoords[SOUTH] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(4, 11);
	s_blockDefs[GLOWSTONE].m_textureCoords[EAST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(4, 11);
	s_blockDefs[GLOWSTONE].m_textureCoords[WEST] = g_theGame->m_blockSpriteSheet->getTexCoordsForSpriteCoords(4, 11);
	s_blockDefs[GLOWSTONE].m_isSolid = true;
	s_blockDefs[GLOWSTONE].m_isOpaque = true;
	s_blockDefs[GLOWSTONE].m_solidAndOpaqueBits = OPAQUE_AND_SOLID_MASK;
	s_blockDefs[GLOWSTONE].m_selfIllumination = 12;
	s_blockDefs[GLOWSTONE].m_sfx_dig = SFX_GLASS_DIG;
	s_blockDefs[GLOWSTONE].m_num_sfx_dig = SFX_GLASS_DIG_NUM;
	s_blockDefs[GLOWSTONE].m_sfx_place = SFX_STONE_DIG;
	s_blockDefs[GLOWSTONE].m_num_sfx_place = SFX_STONE_DIG_NUM;
	s_blockDefs[GLOWSTONE].m_sfx_step = SFX_STONE_STEP;
	s_blockDefs[GLOWSTONE].m_num_sfx_step = SFX_STONE_STEP_NUM;
}

