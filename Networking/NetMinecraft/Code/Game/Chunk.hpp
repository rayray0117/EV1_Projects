#pragma once
#include "Engine/Renderer/Vertex3D.hpp"
#include "Game/Block.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
struct BlockInfo;
//////////////////////////////////////////////////////////////////////////
class Chunk
{
public:
	Chunk(IntVector2 chunkCoords);
	~Chunk();

	void Update(float deltaSeconds);
	void Render() const;

	int getBlockIndexCoords(const IntVector3& blockCoords) const;
	//int getBlockIndexWithMinZForBlockIndex(int blockIndex) const; //Gets 
	//int getBlockIndexWithMaxZForBlockIndex(int blockIndexXY) const;
	IntVector3 getBlockCoordsForBlockIndex(int index) const;
	IntVector3 getBlockWorldCoordsForBlockIndex(int index) const;
	IntVector3 getBlockCoordsForWorldCoords(const Vector3& pos);

	void removeBlockAtIndex(int index);
	void setBlockAtIndexsFace(int index, int blockFace, unsigned char blockType = STONE);
	void setBlockAtIndex(int index, BlockTypes blockType = STONE);


	void UpdateVertexArray();

	void saveToFile();
	static void SaveToFile(const IntVector2& coords, std::vector<uchar>& buffer);
	void compressToRLE(std::vector<unsigned char>&);
	bool populateFromFile();
	void populateFromRLE(const std::vector<unsigned char>& buffer);

	void generateStartingBlocks();
	void generateStartingBlocks_Flat();
	void generateStartingBlocks_Custom();
	void generateStartingBlocks_Perlin();

	void setRangeOfBlocksToBlockType(int startingBlockIndex, unsigned char blockType, int numberOfBlocks);
	unsigned char getBlockTypeForBlockCoords(const IntVector3& blockCoords) const;

	void setNorthNeighbor(Chunk* northChunk);
	void setSouthNeighbor(Chunk* southChunk);
	void setEastNeighbor(Chunk* eastChunk);
	void setWestNeighbor(Chunk* westChunk);
	Vector3 m_worldCorners[8];
	std::vector<unsigned char> m_rle;
public:
	unsigned int m_vbo;
	int m_vertexArraySize;
	//std::vector<Vertex3D> m_vertexArray;
	bool m_isVertexArrayDirty;
	IntVector2 m_chunkCoords;
	Vector2 m_chunkCenter;
	Block m_blocks[BLOCKS_PER_CHUNK];
	IntVector3 m_worldMins;
	Chunk* m_northChunk;
	Chunk* m_eastChunk;
	Chunk* m_southChunk;
	Chunk* m_westChunk;

private:
	void rebuildVertexArray();

	void RLEUpdate();

	void populateVertexArray(std::vector<Vertex3D>& out_vertexArray);

	bool doBuildThisFace(int blockFace, const IntVector3& blockCoords) const;
	bool doBuildThisFace_Top(const BlockInfo& bi) const;
	bool doBuildThisFace_Bottom(const BlockInfo& bi) const;
	bool doBuildThisFace_North(const BlockInfo& bi) const;
	bool doBuildThisFace_South(const BlockInfo& bi) const;
	bool doBuildThisFace_East(const BlockInfo& bi) const;
	bool doBuildThisFace_West(const BlockInfo& bi) const;

	void setBlocksWithHeightVarience(int* columnHeights);
	void setWorldCorners();

public:
};


//////////////////////////////////////////////////////////////////////////
inline int Chunk::getBlockIndexCoords(const IntVector3& blockCoords) const
{
	//ASSERT_OR_DIE in bounds
	return blockCoords.x | (blockCoords.y << CHUNK_X_BITS) | (blockCoords.z << (CHUNK_X_BITS + CHUNK_Y_BITS));/*blockCoords.x + (blockCoords.y*CHUNK_X) + (blockCoords.z * BLOCKS_PER_LAYER);*/
}

inline IntVector3 Chunk::getBlockCoordsForBlockIndex(int index) const
{
	// 	int z = index / BLOCKS_PER_LAYER;
	// 	int y = index % BLOCKS_PER_LAYER / CHUNK_X;
	// 	int x = index % CHUNK_X;
	int x = index&X_MASK_BITS;
	int y = (index&Y_MASK_BITS) >> CHUNK_X_BITS;
	int z = (index&Z_MASK_BITS) >> (CHUNK_X_BITS + CHUNK_Y_BITS);
	return IntVector3(x, y, z);
}

inline IntVector3 Chunk::getBlockWorldCoordsForBlockIndex(int index) const
{
	// 	int z = index / BLOCKS_PER_LAYER;
	// 	int y = index % BLOCKS_PER_LAYER / CHUNK_X;
	// 	int x = index % CHUNK_X;
	int x = index&X_MASK_BITS;
	int y = (index&Y_MASK_BITS) >> CHUNK_X_BITS;
	int z = (index&Z_MASK_BITS) >> (CHUNK_X_BITS + CHUNK_Y_BITS);
	x += m_chunkCoords.x * CHUNK_X;
	y += m_chunkCoords.y * CHUNK_Y;
	return IntVector3(x, y, z);
}
