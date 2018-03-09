#include "Game/Chunk.hpp"
#include "Game/BlockDefinition.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Noise.hpp"
#include "Game/BlockInfo.hpp"
#include "Engine/RHI/RHITypes.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Game/World.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Game/GameSession.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "Engine/Networking/TCPConnection.hpp"
#include "Engine/Networking/TCPSession.hpp"
//////////////////////////////////////////////////////////////////////////
Chunk::Chunk(IntVector2 chunkCoords)
	: m_chunkCoords(chunkCoords)
	, m_isVertexArrayDirty(false)
	, m_chunkCenter(Vector2((float)(m_chunkCoords.x * CHUNK_X + HALF_CHUNK_X), (float)(m_chunkCoords.y * CHUNK_Y + HALF_CHUNK_Y)))
	, m_northChunk(nullptr)
	, m_southChunk(nullptr)
	, m_westChunk(nullptr)
	, m_eastChunk(nullptr)
	, m_vbo(0)
	, m_vertexArraySize(0)
	, m_worldMins(m_chunkCoords.x * CHUNK_X, m_chunkCoords.y * CHUNK_Y, 0)
{
	setWorldCorners();
}

Chunk::~Chunk()
{
	if (m_northChunk)
	{
		m_northChunk->m_southChunk = nullptr;
		m_northChunk->m_isVertexArrayDirty = true;
	}
	if (m_southChunk)
	{
		m_southChunk->m_northChunk = nullptr;
		m_southChunk->m_isVertexArrayDirty = true;
	}
	if (m_eastChunk)
	{
		m_eastChunk->m_westChunk = nullptr;
		m_eastChunk->m_isVertexArrayDirty = true;
	}
	if (m_westChunk)
	{
		m_westChunk->m_eastChunk = nullptr;
		m_westChunk->m_isVertexArrayDirty = true;
	}

	g_renderer->DeleteVBO(m_vbo);
}
//////////////////////////////////////////////////////////////////////////
void Chunk::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void Chunk::UpdateVertexArray()
{
	if (m_isVertexArrayDirty)
	{
		rebuildVertexArray();
		//RLEUpdate();
	}
}

void Chunk::Render() const
{
	g_renderer->pushMatrix();
	g_renderer->translate3D((float)(m_chunkCoords.x * CHUNK_X), (float)(m_chunkCoords.y * CHUNK_Y), 0.f);
	g_renderer->drawVBO3D(m_vbo, m_vertexArraySize, RHIEnum::PRIMATIVE_QUADS);
	g_renderer->popMatrix();
}
//////////////////////////////////////////////////////////////////////////
IntVector3 Chunk::getBlockCoordsForWorldCoords(const Vector3& pos)
{
	IntVector3 p = Vector3(pos.x - (CHUNK_X*m_chunkCoords.x), pos.y - (CHUNK_Y*m_chunkCoords.y), pos.z);
	return p;
}

void Chunk::removeBlockAtIndex(int index)
{
	m_blocks[index].changeType(AIR);
	std::vector<uchar> trash;
	compressToRLE(trash);
	m_isVertexArrayDirty = true;
}

void Chunk::setBlockAtIndexsFace(int index, int face, unsigned char type /*= STONE*/)
{
	IntVector3 blockCoords = getBlockCoordsForBlockIndex(index);
	switch (face)
	{
	case TOP:
		if (/*index & Z_MASK_BITS == Z_MASK_BITS*/ blockCoords.z != CHUNK_Z - 1) //blockCoords.z != CHUNK_Z - 1
		{
			blockCoords += IntVector3::ZAXIS;
		}
		else
			return;
		break;
	case BOTTOM:
		if (blockCoords.z != 0)
		{
			blockCoords -= IntVector3::ZAXIS;
		}
		else
			return;
		break;
	case NORTH:
		if (/*index & Y_MASK_BITS == Y_MASK_BITS*/ blockCoords.y != CHUNK_Y - 1) //blockCoords.y != CHUNK_Y - 1
		{
			blockCoords += IntVector3::YAXIS;
		}
		else if (m_northChunk)
		{
			return; // TODO
		}
		else
			return;
		break;
	case SOUTH:
		if (blockCoords.y != 0)
		{
			blockCoords -= IntVector3::YAXIS;
		}
		else if (m_southChunk)
		{
			return; // TODO
		}
		else
			return;
		break;
	case EAST:
		if (/*index & X_MASK_BITS == X_MASK_BITS*/ blockCoords.x != CHUNK_X - 1) //blockCoords.x != CHUNK_X - 1
		{
			blockCoords += IntVector3::XAXIS;
		}
		else if (m_eastChunk)
		{
			return; // TODO
		}
		else
			return;
		break;
	case WEST:
		if (/*index & X_MASK_BITS == X_MASK_BITS*/ blockCoords.x != 0) //blockCoords.x != 0
		{
			blockCoords -= IntVector3::XAXIS;
		}
		else if (m_westChunk)
		{
			return; // TODO
		}
		else
			return;
		break;
	default: return;
		break;
	}

	int blockToChangeIndex = getBlockIndexCoords(blockCoords);
	m_blocks[blockToChangeIndex].changeType(type);
	m_isVertexArrayDirty = true;
}

void Chunk::setBlockAtIndex(int index, BlockTypes blockType /*= STONE*/)
{
	m_blocks[index].changeType(blockType);
	std::vector<uchar> trash;
	compressToRLE(trash);
	m_isVertexArrayDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void Chunk::rebuildVertexArray()
{
	std::vector<Vertex3D> vertexArray;
	populateVertexArray(vertexArray);

	if (m_vbo == 0)
	{
		m_vbo = g_renderer->CreateAndGetVBOId();
	}

	m_vertexArraySize = vertexArray.size();
	g_renderer->SetVBO3DData(m_vbo, vertexArray.data(), m_vertexArraySize, RHIEnum::USAGE_STATIC_DRAW);


	m_isVertexArrayDirty = false;
}


void Chunk::RLEUpdate()
{
	g_theGame->m_world->m_rleChunks[m_chunkCoords] = m_rle;
}

void Chunk::populateVertexArray(std::vector<Vertex3D>& out_vertexArray)
{
	out_vertexArray.clear();
	out_vertexArray.reserve(NUM_VERTICES_TO_RESERVE);
	for (int index = 0; index < BLOCKS_PER_CHUNK; index++)
	{
		if (m_blocks[index].m_type == AIR)
			continue;


		Block& block = m_blocks[index];
		BlockDefinition& blockDef = BlockDefinition::s_blockDefs[block.m_type];
		IntVector3 blockCoords = getBlockCoordsForBlockIndex(index);
		Vector3 offset((float)blockCoords.x, (float)blockCoords.y, (float)blockCoords.z);

		AABB2 textureCoords;
		BlockInfo blockInfo(this, index);
		unsigned char lightValue;
		Rgba faceLighting;

		//Bottom (-Z)
		BlockInfo faceBlockInfo = blockInfo.getBlockInfoToBottom();
		if (doBuildThisFace_Bottom(faceBlockInfo))
		{
			lightValue = getByteBrightnessForBlockLighting((unsigned char)faceBlockInfo.getLightValue());
			faceLighting = Rgba(lightValue, lightValue, lightValue);

			textureCoords = blockDef.m_textureCoords[BOTTOM];
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 1.f, 0.f) + offset, Vector2(textureCoords.mins.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 1.f, 0.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 0.f, 0.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 0.f, 0.f) + offset, Vector2(textureCoords.mins.x, textureCoords.maxs.y), faceLighting));
		}

		//Top (+Z)
		faceBlockInfo = blockInfo.getBlockInfoToTop();
		if (doBuildThisFace_Top(faceBlockInfo))
		{
			if (faceBlockInfo.Exists())
				lightValue = getByteBrightnessForBlockLighting((unsigned char)faceBlockInfo.getLightValue());
			else
				lightValue = getByteBrightnessForBlockLighting((unsigned char)g_skyLightValue);
			faceLighting = Rgba(lightValue, lightValue, lightValue);

			textureCoords = blockDef.m_textureCoords[TOP];
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 0.f, 1.f) + offset, Vector2(textureCoords.mins.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 0.f, 1.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 1.f, 1.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 1.f, 1.f) + offset, Vector2(textureCoords.mins.x, textureCoords.mins.y), faceLighting));
		}

		// East (+X)
		faceBlockInfo = blockInfo.getBlockInfoToEast();
		if (doBuildThisFace_East(faceBlockInfo))
		{
			lightValue = getByteBrightnessForBlockLighting((unsigned char)faceBlockInfo.getLightValue());
			faceLighting = Rgba(lightValue, lightValue, lightValue);

			textureCoords = blockDef.m_textureCoords[EAST];
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 0.f, 0.f) + offset, Vector2(textureCoords.mins.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 1.f, 0.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 1.f, 1.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 0.f, 1.f) + offset, Vector2(textureCoords.mins.x, textureCoords.mins.y), faceLighting));
		}

		// West (-X)
		faceBlockInfo = blockInfo.getBlockInfoToWest();
		if (doBuildThisFace_West(faceBlockInfo))
		{
			lightValue = getByteBrightnessForBlockLighting((unsigned char)faceBlockInfo.getLightValue());
			faceLighting = Rgba(lightValue, lightValue, lightValue);

			textureCoords = blockDef.m_textureCoords[WEST];
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 0.f, 1.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 1.f, 1.f) + offset, Vector2(textureCoords.mins.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 1.f, 0.f) + offset, Vector2(textureCoords.mins.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 0.f, 0.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.maxs.y), faceLighting));
		}

		// South (-Y)
		faceBlockInfo = blockInfo.getBlockInfoToSouth();
		if (doBuildThisFace_South(faceBlockInfo))
		{
			lightValue = getByteBrightnessForBlockLighting((unsigned char)faceBlockInfo.getLightValue());
			faceLighting = Rgba(lightValue, lightValue, lightValue);

			textureCoords = blockDef.m_textureCoords[SOUTH];
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 0.f, 0.f) + offset, Vector2(textureCoords.mins.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 0.f, 0.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 0.f, 1.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 0.f, 1.f) + offset, Vector2(textureCoords.mins.x, textureCoords.mins.y), faceLighting));
		}

		// North (+Y)
		faceBlockInfo = blockInfo.getBlockInfoToNorth();
		if (doBuildThisFace_North(faceBlockInfo))
		{
			lightValue = getByteBrightnessForBlockLighting((unsigned char)faceBlockInfo.getLightValue());
			faceLighting = Rgba(lightValue, lightValue, lightValue);

			textureCoords = blockDef.m_textureCoords[NORTH];
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 1.f, 1.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 1.f, 1.f) + offset, Vector2(textureCoords.mins.x, textureCoords.mins.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(1.f, 1.f, 0.f) + offset, Vector2(textureCoords.mins.x, textureCoords.maxs.y), faceLighting));
			out_vertexArray.push_back(Vertex3D(Vector3(0.f, 1.f, 0.f) + offset, Vector2(textureCoords.maxs.x, textureCoords.maxs.y), faceLighting));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// checking block face stuff.
bool Chunk::doBuildThisFace(int blockFace, const IntVector3& blockCoords) const
{
	switch (blockFace)
	{
	case TOP:
		if (blockCoords.z != CHUNK_Z - 1)
		{
			return getBlockTypeForBlockCoords(blockCoords + IntVector3::ZAXIS) == AIR;
		}
		else
		{
			return true;
		}
		break;
	case BOTTOM:
		if (blockCoords.z != 0)
		{
			return getBlockTypeForBlockCoords(blockCoords - IntVector3::ZAXIS) == AIR;
		}
		else
		{
			return false;
		}
		break;
	case NORTH:
		if (blockCoords.y != CHUNK_Y - 1)
		{
			return getBlockTypeForBlockCoords(blockCoords + IntVector3::YAXIS) == AIR;
		}
		else
		{
			if (m_northChunk)
				return m_northChunk->getBlockTypeForBlockCoords(IntVector3(blockCoords.x, 0, blockCoords.z)) == AIR;
			return false;
		}
		break;
	case SOUTH:
		if (blockCoords.y != 0)
		{
			return getBlockTypeForBlockCoords(blockCoords - IntVector3::YAXIS) == AIR;
		}
		else
		{
			if (m_southChunk)
				return m_southChunk->getBlockTypeForBlockCoords(IntVector3(blockCoords.x, CHUNK_Y - 1, blockCoords.z)) == AIR;
			return false;
		}
		break;
	case EAST:
		if (blockCoords.x != CHUNK_X - 1)
		{
			return getBlockTypeForBlockCoords(blockCoords + IntVector3::XAXIS) == AIR;
		}
		else
		{
			if (m_eastChunk)
				return m_eastChunk->getBlockTypeForBlockCoords(IntVector3(0, blockCoords.y, blockCoords.z)) == AIR;
			return false;
		}
		break;
	case WEST:
		if (blockCoords.x != 0)
		{
			return getBlockTypeForBlockCoords(blockCoords - IntVector3::XAXIS) == AIR;
		}
		else
		{
			if (m_westChunk)
				return m_westChunk->getBlockTypeForBlockCoords(IntVector3(CHUNK_X - 1, blockCoords.y, blockCoords.z)) == AIR;
			return false;
		}
		break;
	default: return true;
		break;
	}
}

bool Chunk::doBuildThisFace_Top(const BlockInfo& bi) const
{
	if (bi.m_chunk == nullptr)
		return true;
	else
		return !bi.isOpaque();
}

bool Chunk::doBuildThisFace_Bottom(const BlockInfo& bi) const
{
	if (bi.m_chunk == nullptr)
		return false;
	else
		return !bi.isOpaque();
}

bool Chunk::doBuildThisFace_North(const BlockInfo& bi) const
{
	if (bi.m_chunk == nullptr)
		return false;
	else
		return !bi.isOpaque();
}

bool Chunk::doBuildThisFace_South(const BlockInfo& bi) const
{
	if (bi.m_chunk == nullptr)
		return false;
	else
		return !bi.isOpaque();
}

bool Chunk::doBuildThisFace_East(const BlockInfo& bi) const
{
	if (bi.m_chunk == nullptr)
		return false;
	else
		return !bi.isOpaque();
}

bool Chunk::doBuildThisFace_West(const BlockInfo& bi) const
{
	if (bi.m_chunk == nullptr)
		return false;
	else
		return !bi.isOpaque();
}

unsigned char Chunk::getBlockTypeForBlockCoords(const IntVector3& blockCoords) const
{
	int blockIndex = getBlockIndexCoords(blockCoords);
	return m_blocks[blockIndex].m_type;
}


//////////////////////////////////////////////////////////////////////////
void Chunk::saveToFile()
{
	std::vector<unsigned char> buffer;
	buffer.reserve(10000);
	compressToRLE(buffer);

	std::string savePath = std::string(g_savePath) + "Chunks/" + Stringf("Chunk_at_(%i,%i)", m_chunkCoords.x, m_chunkCoords.y);
	WriteBufferToFile(buffer, savePath);

	return;
}

void Chunk::SaveToFile(const IntVector2& coords, std::vector<uchar>& buffer)
{
	std::string savePath = std::string(g_savePath) + "Chunks/" + Stringf("Chunk_at_(%i,%i)", coords.x, coords.y);
	WriteBufferToFile(buffer, savePath);

	return;
}

void Chunk::compressToRLE(std::vector<unsigned char>& buffer)
{
	buffer.push_back(VERSION);
	buffer.push_back(CHUNK_X_BITS);
	buffer.push_back(CHUNK_Y_BITS);
	buffer.push_back(CHUNK_Z_BITS);
	unsigned char count = (unsigned char)1;
	unsigned char blockType = m_blocks[0].m_type;
	for (int blockIndex = 1; blockIndex < BLOCKS_PER_CHUNK; ++blockIndex)
	{
		if (m_blocks[blockIndex].m_type != blockType || count == 255)
		{
			buffer.push_back(blockType);
			buffer.push_back(count);
			count = (unsigned char)1;
			blockType = m_blocks[blockIndex].m_type;
		}
		else
		{
			++count;
		}
	}
	buffer.push_back(blockType);
	buffer.push_back(count);
	m_rle = buffer;
	RLEUpdate();
}

bool Chunk::populateFromFile()
{
	std::string savePath = std::string(g_savePath) + "Chunks/" + Stringf("Chunk_at_(%i,%i)", m_chunkCoords.x, m_chunkCoords.y);
	std::vector<unsigned char> buffer;
	if (ReadBufferToFile(buffer, savePath))
	{
		if (buffer[0] != VERSION || buffer[1] != CHUNK_X_BITS || buffer[2] != CHUNK_Y_BITS || buffer[3] != CHUNK_Z_BITS)
			return false;


		populateFromRLE(buffer);

		return true;
	}
	return false;
}

void Chunk::populateFromRLE(const std::vector<unsigned char>& buffer)
{
	PROFILE_SCOPE_FUNCTION();
	unsigned char blockType;
	int numberOfBlocks;
	int startingBlockIndex = 0;
	for (unsigned int bufferBlockTypeIndex = 4; bufferBlockTypeIndex < buffer.size(); bufferBlockTypeIndex += 2)
	{
		unsigned int bufferNumberOfBlocksIndex = bufferBlockTypeIndex + 1;
		blockType = buffer[bufferBlockTypeIndex];
		numberOfBlocks = buffer[bufferNumberOfBlocksIndex];
		setRangeOfBlocksToBlockType(startingBlockIndex, blockType, numberOfBlocks);
		startingBlockIndex += numberOfBlocks;
	}
	m_rle = buffer;

	m_isVertexArrayDirty = true;
}

//////////////////////////////////////////////////////////////////////////
void Chunk::generateStartingBlocks()
{
	PROFILE_SCOPE_FUNCTION();
	switch (g_BlockGenerationType)
	{
	case FLAT:
		generateStartingBlocks_Flat();
		break;
	case CUSTOM:
		generateStartingBlocks_Custom();
		break;
	case PERLIN:
		generateStartingBlocks_Perlin();
		break;
	}

	m_isVertexArrayDirty = true;
}

void Chunk::generateStartingBlocks_Flat()
{
	for (int z = 0; z < CHUNK_Z; z++)
	{
		for (int y = 0; y < CHUNK_Y; y++)
		{
			for (int x = 0; x < CHUNK_X; x++)
			{
				int blockIndex = getBlockIndexCoords(IntVector3(x, y, z));
				Block& block = m_blocks[blockIndex];
				if (z < 0)
					block.changeType(STONE);
				else if (z < 0)
					block.changeType(DIRT);
				else if (z < 1)
					block.changeType(GRASS);
				else
				{
					block.changeType(AIR);
				}
				block.clearLightDirtyFlag();
				block.clearSkyFlag();
				block.setLightValue(0);
			}
		}
	}
}

void Chunk::generateStartingBlocks_Custom()
{
	int columnHeights[BLOCKS_PER_LAYER];
	for (int columnY = 0; columnY < CHUNK_Y; ++columnY)
	{
		for (int columnX = 0; columnX < CHUNK_X; columnX++)
		{
			int columnIndex = getBlockIndexCoords(IntVector3(columnX, columnY, 0));
			int columnVariance = (int)(5.f * sinf(.1f * columnX + m_chunkCoords.x));
			columnHeights[columnIndex] = SEA_LEVEL + columnVariance;
		}
	}

	setBlocksWithHeightVarience(columnHeights);
}

void Chunk::generateStartingBlocks_Perlin()
{
	int columnHeights[BLOCKS_PER_LAYER];
	for (int columnY = 0; columnY < CHUNK_Y; columnY++)
	{
		for (int columnX = 0; columnX < CHUNK_X; columnX++)
		{
			int columnIndex = getBlockIndexCoords(IntVector3(columnX, columnY, 0));
			IntVector2 worldPos = (IntVector2(m_chunkCoords.x * CHUNK_X + columnX, columnY + m_chunkCoords.y * CHUNK_Y));
			float perlinNoiseGridCellSize = 80.f;
			int numOctaves = 7;
			float baseAmplitude = 14.f;
			float octavePersistence = .5f;
			int columnVariance = (int)(baseAmplitude * Compute2dPerlinNoise((float)worldPos.x, (float)worldPos.y, perlinNoiseGridCellSize, numOctaves, octavePersistence, 2.0f, false, g_Seed));
			columnHeights[columnIndex] = SEA_LEVEL + columnVariance;
		}
	}

	setBlocksWithHeightVarience(columnHeights);

}


void Chunk::setBlocksWithHeightVarience(int* columnHeights)
{
	for (int blockIndex = 0; blockIndex < BLOCKS_PER_CHUNK; blockIndex++)
	{
		IntVector3 blockCoords = getBlockCoordsForBlockIndex(blockIndex);
		Block& block = m_blocks[blockIndex];

		//int columnIndex = getBlockIndexCoords(IntVector3(blockCoords.x, blockCoords.y, 0)); // #ToDo: Get rid of this with some bit shifty stuff maybe.
		int columnIndex = getBlockIndexWithMinZForBlockIndex(blockIndex);
		int groundHeight = columnHeights[columnIndex];
		int airHeight = groundHeight + 1;
		int stoneHeight = groundHeight - 7;

		if (blockCoords.z >= airHeight)
		{
			block.changeType(AIR);
		}
		else if (blockCoords.z < stoneHeight)
		{
			block.changeType(STONE);

		}
		else if (blockCoords.z == groundHeight)
		{
			block.changeType(GRASS);
		}
		else
		{
			block.changeType(DIRT);
		}

		if ((block.m_type == DIRT || block.m_type == GRASS) && blockCoords.z == SEA_LEVEL)
		{
			block.changeType(SAND);
		}
		else if (block.m_type == AIR && blockCoords.z <= SEA_LEVEL)
		{
			block.changeType(WATER);
		}

		//Make sure it doesn't default to stupid stuff;
		block.setLightValue(0);
		block.clearSkyFlag();
		block.clearLightDirtyFlag();
	}
}

//////////////////////////////////////////////////////////////////////////
void Chunk::setWorldCorners()
{
	m_worldCorners[0] = Vector3((float)m_worldMins.x, (float)m_worldMins.y, 0.f);
	m_worldCorners[1] = Vector3((float)(m_worldMins.x + CHUNK_X), (float)m_worldMins.y, 0.f);
	m_worldCorners[2] = Vector3((float)m_worldMins.x, (float)(m_worldMins.y + CHUNK_Y), 0.f);
	m_worldCorners[3] = Vector3((float)(m_worldMins.x + CHUNK_X), (float)(m_worldMins.y + CHUNK_Y), 0.f);
	m_worldCorners[4] = Vector3((float)m_worldMins.x, (float)m_worldMins.y, (float)CHUNK_Z);
	m_worldCorners[5] = Vector3((float)(m_worldMins.x + CHUNK_X), (float)m_worldMins.y, (float)CHUNK_Z);
	m_worldCorners[6] = Vector3((float)m_worldMins.x, (float)m_worldMins.y, (float)CHUNK_Z);
	m_worldCorners[7] = Vector3((float)(m_worldMins.x + CHUNK_X), (float)(m_worldMins.y + CHUNK_Y), (float)CHUNK_Z);
}

//////////////////////////////////////////////////////////////////////////
void Chunk::setRangeOfBlocksToBlockType(int startingBlockIndex, unsigned char blockType, int numberOfBlocks)
{
	ASSERT_OR_DIE((startingBlockIndex + numberOfBlocks) <= BLOCKS_PER_CHUNK, "Out of range! Tried to put more blocks the chunk than available based on that start position.");
	for (int index = startingBlockIndex; index < (startingBlockIndex + numberOfBlocks); index++)
	{
		m_blocks[index].changeType(blockType);

		//Make sure it doesn't default to stupid stuff. Might want to decide whether I'd actually use this outside of populateFromFile;
		m_blocks[index].setLightValue(0);
		m_blocks[index].clearSkyFlag();
		m_blocks[index].clearLightDirtyFlag();
	}
}

void Chunk::setNorthNeighbor(Chunk* northChunk)
{
	m_northChunk = northChunk;
	if (m_northChunk)
	{
		m_northChunk->m_southChunk = this;
		m_northChunk->m_isVertexArrayDirty = true;
		m_isVertexArrayDirty = true;
	}
}

void Chunk::setSouthNeighbor(Chunk* southChunk)
{
	m_southChunk = southChunk;
	if (m_southChunk)
	{
		m_southChunk->m_northChunk = this;
		m_southChunk->m_isVertexArrayDirty = true;
		m_isVertexArrayDirty = true;
	}
}

void Chunk::setEastNeighbor(Chunk* eastChunk)
{
	m_eastChunk = eastChunk;
	if (m_eastChunk)
	{
		m_eastChunk->m_westChunk = this;
		m_eastChunk->m_isVertexArrayDirty = true;
		m_isVertexArrayDirty = true;
	}
}

void Chunk::setWestNeighbor(Chunk* westChunk)
{
	m_westChunk = westChunk;
	if (m_westChunk)
	{
		m_westChunk->m_eastChunk = this;
		m_westChunk->m_isVertexArrayDirty = true;
		m_isVertexArrayDirty = true;
	}
}
