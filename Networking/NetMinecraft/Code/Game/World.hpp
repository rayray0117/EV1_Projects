#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Game\RaycastResult.hpp"
#include <vector>
#include <deque>
//////////////////////////////////////////////////////////////////////////
class Mesh;
class Actor;
class Chunk;
//////////////////////////////////////////////////////////////////////////
class World
{
	friend class GameSession;
public:
	World();
	~World();
	
	void Update(float deltaSeconds);
	void Render() const;
	//////////////////////////////////////////////////////////////////////////
	Chunk* getChunkAtCoords(const IntVector2& chunkCoords);
	IntVector2 getChunkCoordsForWorldPosition(const Vector3& worldPosition);
	RaycastResult raycast(const Vector3& start, const Vector3& direction, float length);
	bool isWorldPositionInSolid(const Vector3& pos, BlockInfo& bi);
	BlockInfo getBlockInfoAtWorldPos(const Vector3& pos);
	BlockFaces getBlockFaceForDirection(const IntVector3& direction) const;
	void moveBlockInfoToFace(BlockInfo& bi, BlockFaces face);
	void DeactivateAllChunks();

	void placeBlock(BlockInfo& bi, BlockTypes blockType = COBBLESTONE);
	void digBlock(BlockInfo& bi);
	void SetChunkRLE(const IntVector2& coords, std::vector<uchar> rleData);
	bool isChunkRLECorrect(const IntVector2& coords, std::vector<uchar> rleData);
public:
	std::vector<Actor*> m_actors;
	std::map <IntVector2, Chunk*> m_chunks;
	std::map <IntVector2, std::vector<uchar>> m_rleChunks;
	std::map <IntVector2, std::vector<uchar>> m_rleChunksToUpdate;
private:
	void UpdateChunks(float	deltaSeconds);
	void UpdateLighting();
	void UpdateBlockLighting(BlockInfo& bi);
	void UpdateVertexArrays();

	void ManageChunks();
	void DeactivateChunk(Chunk* c);
	void ActivateChunk(const IntVector2& chunkCoords);
	void removeChunkFromMap(Chunk* c);
	void DeactivateFarthestChunk();

	Chunk* getFarthestChunk(const Vector3& camPos, float& farthestDistanceFound);

	bool ActivateClosestChunk();

	bool ActiveClosestChunkToPos(Vector3 camPos);
	bool ActivateMostImportantRLE();

	bool FindClosestInactiveChunk(Vector3 camPos, IntVector2& closestCoords, float& squareDist);
	bool FindClosestInactiveRLE(Vector3 camPos, IntVector2& closestCoords, float& closestSquareDist);

	void DeactivateFarthestChunkOutOfRange();
	void setChunksNeighbors(Chunk* newChunk, const IntVector2& chunkCoords);

	int getIdealLightValue(const BlockInfo& bi) const;
	int getHighestNeighborsLightValue(const BlockInfo& bi) const;
	void dirtyBlockLight(BlockInfo& bi);
	void forceDirtyBlockLight(BlockInfo& bi);
	void setBlockNeighborsChunkDirty(const BlockInfo& bi);
	void setBlockNeighborsLightDirty(const BlockInfo& bi);
	void setInitialLighting(Chunk* newChunk);
	void setInitialSkyBlocks(Chunk* newChunk);
	void setInitialIlluminationBlocks(Chunk* newChunk);
	void initializeSkyDownward(BlockInfo& bi);
	void initializeIlluminationUpward(BlockInfo& bi); // Checks if block has self illumination up till it hits sky. 
	void dirtySkyNeighborsDownward(BlockInfo& bi);
	void setSkyBlockNeighborsLightDirty(const BlockInfo& bi);
	void clearSkyDownward(BlockInfo bi);
	void setSkyDownward(BlockInfo bi);

	void RaycastCheck();
	bool PopulateChunkFromRLEStorage(Chunk* newChunk);
	void SaveOutRLEChunks();
private:
	std::deque<BlockInfo> m_dirtyLightingBlocks;
};

inline IntVector2 World::getChunkCoordsForWorldPosition(const Vector3& worldPosition)
{
	int chunkX = ((int)floor(worldPosition.x)) >> CHUNK_X_BITS;
	int chunkY = ((int)floor(worldPosition.y)) >> CHUNK_Y_BITS;

	return IntVector2(chunkX, chunkY);
}