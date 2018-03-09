#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameState.hpp"
#include "Game/Actor.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/ParseUtils.hpp"
#include "Engine/Networking/NetObjectSystem.hpp"
#include "GameSession.hpp"
#include "Chunk.hpp"
#include "Game/PlayerInfo.hpp"
#include "Game/Camera3D.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "Engine/Networking/TCPConnection.hpp"
#include "Engine/Networking/TCPSession.hpp"
//////////////////////////////////////////////////////////////////////////
const float HORIZON_EFFECT_DIST_DIVSOR = 1.f / (MAX_CHUNK_VIS_DIST * MAX_CHUNK_VIS_DIST * 10);
//////////////////////////////////////////////////////////////////////////
World::World()
{
}

World::~World()
{
	DeactivateAllChunks();
	if (g_gameSession != nullptr && g_gameSession->isHost())
		SaveOutRLEChunks();
}
//////////////////////////////////////////////////////////////////////////
void World::Update(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	RaycastCheck();

	UpdateChunks(deltaSeconds);
	ManageChunks();
	UpdateLighting();
	UpdateVertexArrays();


	nos::NetObjectSystemStep();
	/*/
	for (uint i = 0; i < m_actors.size(); ++i)
	{
		if (m_actors[i] == nullptr)
			continue;
		if (g_gameSession->isHost())
		{
			m_actors[i]->Update(deltaSeconds);
		}
	}
	//*/
	/*/
	for (uint i = 0; i < g_gameSession->m_players.size(); ++i)
	{
		if (g_gameSession->m_players[i] == nullptr)
			continue;
		if (g_gameSession->isHost() || g_gameSession->my_player == g_gameSession->m_players[i])
		{
			g_gameSession->m_players[i]->Update(deltaSeconds);
		}
	}
	
	//*/
}

void World::Render() const
{
	PROFILE_SCOPE_FUNCTION();
	//*/
	for (std::map<IntVector2, Chunk*>::const_iterator iterator = m_chunks.begin(); iterator != m_chunks.end(); iterator++)
	{
		//const Chunk* chunk = iterator->second;
		float distSqauredToChunk = 0.f;//calcDistanceSquared(chunk->m_chunkCenter, Vector2(g_gameSession->getMyPlayer()->m_position));
		float heightScale = 1 - clamp(distSqauredToChunk * HORIZON_EFFECT_DIST_DIVSOR, 0.f, 1.f);
		g_renderer->pushMatrix();
		g_renderer->scale3D(Vector3(1.f, 1.f, heightScale));
		iterator->second->Render();
		g_renderer->popMatrix();
	}
	//*/
	/*/
	for (Actor* current : m_actors)
	{
		if (current == nullptr)
			continue;
		current->Render();
	}
	//*/
	for (uint i = 0; i < g_gameSession->m_players.size(); ++i)
	{
		if (g_gameSession->m_players[i] == nullptr)
			continue;
		g_gameSession->m_players[i]->Render();
	}
}
//////////////////////////////////////////////////////////////////////////

Chunk* World::getChunkAtCoords(const IntVector2& chunkCoords)
{
	std::map<IntVector2, Chunk*>::iterator found = m_chunks.find(chunkCoords);
	if (found != m_chunks.end())
	{
		return found->second;
	}
	else
		return nullptr;
}

RaycastResult World::raycast(const Vector3& start, const Vector3& direction, float length)
{
	int numSteps = 200;
	float distPerStep = length / numSteps;

	RaycastResult result;
	IntVector3 lastPos = start;
	result.direction = direction;
	result.lastFraction = 0.f;
	result.fraction = 0.f;

	for (int stepNum = 0; stepNum < numSteps; stepNum++)
	{
		float dist = distPerStep * (float)stepNum;
		Vector3 pos = start + (dist * direction);
		BlockInfo blockInfo;
		if (isWorldPositionInSolid(pos, blockInfo))
		{
			result.didImpact = true;
			result.hitPosition = pos;
			result.blockInfo = blockInfo;
			result.length = dist;
			result.lastFraction = result.fraction;
			result.fraction = dist / length;

			result.face = getBlockFaceForDirection(lastPos - IntVector3(pos));
			return result;
		}

		lastPos = pos;
		result.fraction = dist / length;
	}

	result.fraction = 1.f;
	result.didImpact = false;
	return result;
}

bool World::isWorldPositionInSolid(const Vector3& pos, BlockInfo& bi)
{
	bi = getBlockInfoAtWorldPos(pos);
	if (bi.m_chunk)
	{
		return bi.isSolid();
	}
	else
		return false;
}

BlockInfo World::getBlockInfoAtWorldPos(const Vector3& pos)
{
	IntVector2 p = getChunkCoordsForWorldPosition(pos);

	BlockInfo info;

	Chunk* chunk = getChunkAtCoords(p);
	if (chunk)
	{
		IntVector3 localCoords = chunk->getBlockCoordsForWorldCoords(pos);
		int blockIndex = chunk->getBlockIndexCoords(localCoords);
		info.m_blockIndex = blockIndex;
		info.m_chunk = chunk;
	}
	else
	{
		info.m_blockIndex = 0;
		info.m_chunk = nullptr;
	}
	return info;
}

BlockFaces World::getBlockFaceForDirection(const IntVector3& direction) const
{
	if (direction.x == 1)
		return EAST;
	if (direction.x == -1)
		return WEST;
	if (direction.y == 1)
		return NORTH;
	if (direction.y == -1)
		return SOUTH;
	if (direction.z == 1)
		return TOP;
	if (direction.z == -1)
		return BOTTOM;

	return TOP;
}

void World::moveBlockInfoToFace(BlockInfo& bi, BlockFaces face)
{
	switch (face)
	{
	case EAST: bi.moveEast();
		break;
	case WEST: bi.moveWest();
		break;
	case NORTH: bi.moveNorth();
		break;
	case SOUTH: bi.moveSouth();
		break;
	case TOP: bi.moveTop();
		break;
	case BOTTOM: bi.moveBottom();
		break;
	case NUM_BLOCK_FACES:
		break;
	default:
		break;
	}
}

void World::DeactivateAllChunks()
{
	std::map<IntVector2, Chunk*>::iterator iterator;
	while (!m_chunks.empty())
	{
		iterator = m_chunks.begin();
		DeactivateChunk(iterator->second);
	}
}

void World::placeBlock(BlockInfo& bi, BlockTypes blockType /*= COBBLESTONE*/)
{
	if (!bi.Exists())
		return;

	// Place block
	int blockIndex = bi.m_blockIndex;
	bi.m_chunk->setBlockAtIndex(blockIndex, blockType);

	// Check if block is now sky and set things to dirty
	if (bi.isSky() && bi.isOpaque())
	{
		bi.clearSkyFlag();
		clearSkyDownward(bi.getBlockInfoToBottom());
	}
	setBlockNeighborsChunkDirty(bi);
	forceDirtyBlockLight(bi);

	// Get sound
	std::string placeSfx;
	int numSfxs;
	bi.getSoundPlace(placeSfx, numSfxs);

	// Play sound
	g_theGame->PlayRandomSound(placeSfx, numSfxs);
}

void World::digBlock(BlockInfo& bi)
{
	if (!bi.Exists())
		return;

	// Get sound
	std::string digSfx;
	int numSfxs;
	bi.getSoundDig(digSfx, numSfxs);

	// Remove block
	int blockIndex = bi.m_blockIndex;
	bi.m_chunk->removeBlockAtIndex(blockIndex);

	// Check if block is now sky and set things to dirty
	if (bi.getBlockInfoToTop().isSky())
	{
		bi.setSkyFlag();
		setSkyDownward(bi.getBlockInfoToBottom());
	}
	setBlockNeighborsChunkDirty(bi);
	forceDirtyBlockLight(bi);

	// Play sound
	g_theGame->PlayRandomSound(digSfx, numSfxs);
}

void World::SetChunkRLE(const IntVector2& coords, std::vector<uchar> rleData)
{
	m_rleChunksToUpdate[coords] = rleData;
}

bool World::isChunkRLECorrect(const IntVector2& coords, std::vector<uchar> rleData)
{
	auto found = m_rleChunks.find(coords);
	if (found != m_rleChunks.end())
	{
		return found->second == rleData;
	}
	//If don't have that rle assume they were right and add it to my list. Yes this is dangerous I know but can possible save some net traffic, cpu, and headaches later down the line.
	m_rleChunks[coords] = rleData;
	return true;
}

void World::UpdateChunks(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	for (std::map<IntVector2, Chunk*>::const_iterator iterator = m_chunks.begin(); iterator != m_chunks.end(); iterator++)
	{
		iterator->second->Update(deltaSeconds);
	}
}

void World::UpdateLighting()
{
	PROFILE_SCOPE_FUNCTION();
	while (!m_dirtyLightingBlocks.empty())
	{
		BlockInfo bi = m_dirtyLightingBlocks.front();
		m_dirtyLightingBlocks.pop_front();
		bi.clearLightDirty();
		UpdateBlockLighting(bi);
	}
}

void World::UpdateBlockLighting(BlockInfo& bi)
{
	int idealLightValue = getIdealLightValue(bi);
	int currentLightValue = bi.getLightValue();
	if (idealLightValue == currentLightValue)
		return;

	bi.setLightValue(idealLightValue);


	// Dirty the neighbors
	setBlockNeighborsLightDirty(bi);
}

void World::UpdateVertexArrays()
{
	for (std::map<IntVector2, Chunk*>::const_iterator iterator = m_chunks.begin(); iterator != m_chunks.end(); iterator++)
	{
		iterator->second->UpdateVertexArray();
	}
}

void World::ManageChunks()
{
	PROFILE_SCOPE_FUNCTION();
	if (!m_rleChunksToUpdate.empty())
	{
		for (auto i = m_rleChunksToUpdate.begin(); i != m_rleChunksToUpdate.end(); ++i)
		{
			IntVector2 coords = i->first;
			
			auto found = m_chunks.find(coords);
			if(found != m_chunks.end())
			{
				Chunk* c = found->second;
				c->populateFromRLE(i->second);
				setInitialLighting(c);
			}
			else
			{
				m_rleChunks[coords] = i->second;
			}
		}

		m_rleChunksToUpdate.clear();
	}

	// 1) If at MAX number of Chunks, deactivate farthest.
	if (m_chunks.size() >= MAX_NUM_OF_CHUNKS)
	{
		DeactivateFarthestChunk();
		return;
	}
	// 2) If missing chunk in Range, Activate closest missing one
	if (ActivateClosestChunk())
		return;
	// 3) if above IDEAL number of Chunks, deactivate furtherest out of range chunk
	if (m_chunks.size() > IDEAL_NUM_OF_CHUNKS)
	{
		DeactivateFarthestChunkOutOfRange();
		return;
	}

	if (ActivateMostImportantRLE() && g_gameSession->isHost())
	{
		return;
	}

}

void World::DeactivateChunk(Chunk* c)
{
	if (!c) // Was sent nothing so do nothing.
		return;
	PROFILE_SCOPE_FUNCTION();
	removeChunkFromMap(c);
	if (g_gameSession->isHost() && false)
		c->saveToFile();
	delete c;
}

void World::ActivateChunk(const IntVector2& chunkCoords)
{
	PROFILE_SCOPE_FUNCTION();
	Chunk* newChunk = new Chunk(chunkCoords);
	bool didLoad = PopulateChunkFromRLEStorage(newChunk);

	if (!didLoad && g_gameSession->isHost()) //If we couldn't load from our RLE then check the save files only if we are the host. There is no point for clients to check this because they don't save off a world they don't own.
	{
		didLoad = newChunk->populateFromFile();
		std::vector<uchar> rleBuffer;
		newChunk->compressToRLE(rleBuffer);
	}
	if (!didLoad)
	{
		newChunk->generateStartingBlocks();
		std::vector<uchar> rleBuffer;
		newChunk->compressToRLE(rleBuffer);
	}
	setChunksNeighbors(newChunk, chunkCoords);
	setInitialLighting(newChunk);
	m_chunks[chunkCoords] = newChunk;

	// Activated a chunk so send my guess as to what is was.
	//*/
	if (g_gameSession->isHost()) return;
	NetMessage guess(GAMEMSG_RLE_GUESS);
	guess.sender = g_gameSession->m_session->my_connection;
	guess.write(chunkCoords);
	std::vector<uchar> buf = m_chunks[chunkCoords]->m_rle;
	GameSession::writeRLE(guess, buf);
	g_gameSession->m_session->SendMessageToOthers(guess);
	//*/
}

void World::removeChunkFromMap(Chunk* c)
{
	std::map<IntVector2, Chunk*>::iterator found = m_chunks.find(c->m_chunkCoords);
	if (found != m_chunks.end())
	{
		m_chunks.erase(found);
	}
}

void World::DeactivateFarthestChunk()
{
	const Vector3& camPos = g_gameSession->getCamera()->m_position;
	float farthestDist = 0;
	float squaredDistance;
	Chunk* chunkToDeactivate = getFarthestChunk(camPos, squaredDistance);
	farthestDist = squaredDistance;

	if (g_gameSession->isHost())
	{
		for (uint i = 0; i < g_gameSession->m_players.size(); ++i)
		{
			if (g_gameSession->m_players[i] == nullptr || g_gameSession->m_players[i]->mo_playerCharacter == nullptr) continue;


			Vector3 pos = g_gameSession->m_players[i]->mo_playerCharacter->m_position;
			Chunk* currentChunk = getFarthestChunk(pos, squaredDistance);
			
			if (squaredDistance > farthestDist)
			{
				farthestDist = squaredDistance;
				chunkToDeactivate = currentChunk;
			}
		}
	}



	DeactivateChunk(chunkToDeactivate);
}

Chunk* World::getFarthestChunk(const Vector3& camPos, float& farthestSquaredDistanceFound)
{
	Chunk* chunkToDeactivate = nullptr;
	farthestSquaredDistanceFound = 0;

	for (std::map<IntVector2, Chunk*>::iterator iterator = m_chunks.begin(); iterator != m_chunks.end(); iterator++)
	{
		float squaredDistance = calcDistanceSquared(iterator->second->m_chunkCenter, Vector2(camPos));

		if (farthestSquaredDistanceFound < squaredDistance)
		{
			farthestSquaredDistanceFound = squaredDistance;
			chunkToDeactivate = iterator->second;
		}
	}
	return chunkToDeactivate;
}

void World::DeactivateFarthestChunkOutOfRange()
{
	const Vector3& camPos = g_gameSession->getCamera()->m_position;
	float farthestDist = 0;
	float squaredDistance;
	Chunk* chunkToDeactivate = nullptr;
	Chunk* currentChunk = getFarthestChunk(camPos, squaredDistance);
	if (squaredDistance > MAX_CHUNK_VIS_DIST * MAX_CHUNK_VIS_DIST)
	{
		chunkToDeactivate = currentChunk;
		farthestDist = squaredDistance;
	}

	if (g_gameSession->isHost())
	{
		for (uint i = 0; i < g_gameSession->m_players.size(); ++i)
		{
			if (g_gameSession->m_players[i] == nullptr || g_gameSession->m_players[i]->mo_playerCharacter == nullptr) continue;


			Vector3 pos = g_gameSession->m_players[i]->mo_playerCharacter->m_position;
			currentChunk = getFarthestChunk(pos, squaredDistance);

			if (squaredDistance > farthestDist && squaredDistance > MAX_CHUNK_VIS_DIST * MAX_CHUNK_VIS_DIST)
			{
				farthestDist = squaredDistance;
				chunkToDeactivate = currentChunk;
			}
		}
	}

	DeactivateChunk(chunkToDeactivate);
}

bool World::ActivateClosestChunk()
{
	PROFILE_SCOPE_FUNCTION();
	Vector3 camPos;
	if (g_gameSession->getCamera())
		camPos = g_gameSession->getCamera()->m_position;
	else
		camPos = Vector3::ZERO;

	if (g_gameSession->isHost())
	{
		IntVector2 closestChunkCoords;
		float closestDist = MAX_FLOAT;
		bool foundAnInActiveChunk = false;

		IntVector2 curChunkCoords;
		float curDist;
		if (FindClosestInactiveChunk(camPos, curChunkCoords, curDist))
		{
			foundAnInActiveChunk = true;
			if (curDist < closestDist)
			{
				closestChunkCoords = curChunkCoords;
				closestDist = curDist;
			}
		}

		for (uint i = 0; i < g_gameSession->m_players.size(); ++i)
		{
			if(g_gameSession->m_players[i] == nullptr || g_gameSession->m_players[i]->mo_playerCharacter == nullptr) continue;


			Vector3 pos = g_gameSession->m_players[i]->mo_playerCharacter->m_position;
			if (FindClosestInactiveChunk(pos, curChunkCoords, curDist))
			{
				foundAnInActiveChunk = true;
				if (curDist < closestDist)
				{
					closestChunkCoords = curChunkCoords;
					closestDist = curDist;
				}
			}

			if (foundAnInActiveChunk)
			{
				ActivateChunk(closestChunkCoords);
				return true;
			}
		}
	}

	return ActiveClosestChunkToPos(camPos);
}

bool World::ActiveClosestChunkToPos(Vector3 camPos)
{
	PROFILE_SCOPE_FUNCTION();
	IntVector2 closestChunkCoords;
	float dist;
	bool foundAnInActiveChunk = FindClosestInactiveChunk(camPos, closestChunkCoords, dist);
	if(foundAnInActiveChunk)
		ActivateChunk(closestChunkCoords);

	return foundAnInActiveChunk;
}

bool World::ActivateMostImportantRLE()
{
	if (!g_gameSession->isHost()) return false;
	//*/
	const Vector3& camPos = g_gameSession->getCamera()->m_position;
	float squaredDistance;
	IntVector2 closestChunkCoords;
	bool foundAnInActiveChunk = FindClosestInactiveRLE(camPos, closestChunkCoords, squaredDistance);
	if (foundAnInActiveChunk)
	{
		ActivateChunk(closestChunkCoords);
		Chunk* c = getChunkAtCoords(closestChunkCoords);
		c->UpdateVertexArray();
		DeactivateChunk(c);
	}

	return foundAnInActiveChunk;
	//*/
}

bool World::FindClosestInactiveChunk(Vector3 camPos, IntVector2& closestCoords, float& closestSquareDist)
{
	closestSquareDist = FLT_MAX;
	std::map<IntVector2, Chunk*>::iterator found;
	bool foundAnInactiveChunkInRange = false;



	const IntVector2 camChunkCoords = getChunkCoordsForWorldPosition(camPos);
	for (int y = camChunkCoords.y - CHUNK_VIEW_DIST; y < camChunkCoords.y + CHUNK_VIEW_DIST; ++y)
	{
		for (int x = camChunkCoords.x - CHUNK_VIEW_DIST; x < camChunkCoords.x + CHUNK_VIEW_DIST; ++x)
		{
			IntVector2 possibleChunkCoords(x, y);
			found = m_chunks.find(possibleChunkCoords);

			if (found != m_chunks.end())
				continue;

			Vector2 chunkCenter((float)(x * CHUNK_X + HALF_CHUNK_X), (float)(y * CHUNK_Y + HALF_CHUNK_Y));
			float distance = calcDistanceSquared(Vector2(camPos), chunkCenter);
			if (distance > MAX_CHUNK_VIS_DIST * MAX_CHUNK_VIS_DIST || distance > closestSquareDist)
				continue;

			closestSquareDist = distance;
			closestCoords = possibleChunkCoords;
			foundAnInactiveChunkInRange = true;
		}
	}

	return foundAnInactiveChunkInRange;
}

bool World::FindClosestInactiveRLE(Vector3 camPos, IntVector2& closestCoords, float& closestSquareDist)
{
	closestSquareDist = FLT_MAX;
	bool foundAnInactiveChunkInRange = false;

	const IntVector2 camChunkCoords = getChunkCoordsForWorldPosition(camPos);
	for (int y = camChunkCoords.y - MAX_RLE_VIEW_DIST; y < camChunkCoords.y + MAX_RLE_VIEW_DIST; ++y)
	{
		for (int x = camChunkCoords.x - MAX_RLE_VIEW_DIST; x < camChunkCoords.x + MAX_RLE_VIEW_DIST; ++x)
		{
			IntVector2 possibleChunkCoords(x, y);
			auto found = m_rleChunks.find(possibleChunkCoords);

			if (found != m_rleChunks.end())
				continue;

			Vector2 chunkCenter((float)(x * CHUNK_X + HALF_CHUNK_X), (float)(y * CHUNK_Y + HALF_CHUNK_Y));
			float distance = calcDistanceSquared(Vector2(camPos), chunkCenter);

			closestSquareDist = distance;
			closestCoords = possibleChunkCoords;
			foundAnInactiveChunkInRange = true;
		}
	}

	return foundAnInactiveChunkInRange;
}

void World::setChunksNeighbors(Chunk* newChunk, const IntVector2& chunkCoords)
{
	std::map<IntVector2, Chunk*>::iterator found;
	//North
	found = m_chunks.find(chunkCoords + IntVector2::YAXIS);
	if (found != m_chunks.end())
		newChunk->setNorthNeighbor(found->second);

	//South
	found = m_chunks.find(chunkCoords - IntVector2::YAXIS);
	if (found != m_chunks.end())
		newChunk->setSouthNeighbor(found->second);

	//East
	found = m_chunks.find(chunkCoords + IntVector2::XAXIS);
	if (found != m_chunks.end())
		newChunk->setEastNeighbor(found->second);

	//West
	found = m_chunks.find(chunkCoords - IntVector2::XAXIS);
	if (found != m_chunks.end())
		newChunk->setWestNeighbor(found->second);
}

int World::getIdealLightValue(const BlockInfo& bi) const
{
	// 	const BlockDefinition& blockDef = BlockDefinition::s_blockDefs[bi.getBlockType()];
	// 	if (blockDef.m_selfIllumination > 0)
	// 		return blockDef.m_selfIllumination;
	int selfIlluminationValue = bi.getSelfIlluminationValue();
	if (selfIlluminationValue > 0)
		return selfIlluminationValue;

	if (bi.isOpaque())
		return 0;

	int highestNeighborsLightValue = getHighestNeighborsLightValue(bi);
	int idealLightValue = highestNeighborsLightValue - 1;

	if (bi.isSky() && idealLightValue <= g_skyLightValue)
		return g_skyLightValue;



	if (idealLightValue < 0)
		return 0;
	return idealLightValue;
}

int World::getHighestNeighborsLightValue(const BlockInfo& bi) const
{
	int highestLightValue = bi.getBlockInfoToTop().getLightValue(); // just assume it is the one above me and check against the rest.
	int bottomLightValue = bi.getBlockInfoToBottom().getLightValue();
	int northLightValue = bi.getBlockInfoToNorth().getLightValue();
	int southLightValue = bi.getBlockInfoToSouth().getLightValue();
	int eastLightValue = bi.getBlockInfoToEast().getLightValue();
	int westLightValue = bi.getBlockInfoToWest().getLightValue();

	if (highestLightValue < bottomLightValue)
		highestLightValue = bottomLightValue;
	if (highestLightValue < northLightValue)
		highestLightValue = northLightValue;
	if (highestLightValue < southLightValue)
		highestLightValue = southLightValue;
	if (highestLightValue < eastLightValue)
		highestLightValue = eastLightValue;
	if (highestLightValue < westLightValue)
		highestLightValue = westLightValue;

	return highestLightValue;
}

void World::dirtyBlockLight(BlockInfo& bi) //Why am I setting this as a reference when I'm just making copies before hand any way.
{
	if (!bi.Exists())
		return;
	if (bi.isLightDirty())
		return;
	if (bi.isOpaque())
		return;

	bi.setLightDirty();
	m_dirtyLightingBlocks.push_back(bi);
}

void World::forceDirtyBlockLight(BlockInfo& bi)
{
	bi.setLightDirty();
	m_dirtyLightingBlocks.push_back(bi);
}

void World::setBlockNeighborsChunkDirty(const BlockInfo& bi)
{
	BlockInfo neighbor;
	neighbor = bi.getBlockInfoToEast();
	if (neighbor.m_chunk)
		neighbor.m_chunk->m_isVertexArrayDirty = true;
	neighbor = bi.getBlockInfoToWest();
	if (neighbor.m_chunk)
		neighbor.m_chunk->m_isVertexArrayDirty = true;
	neighbor = bi.getBlockInfoToNorth();
	if (neighbor.m_chunk)
		neighbor.m_chunk->m_isVertexArrayDirty = true;
	neighbor = bi.getBlockInfoToSouth();
	if (neighbor.m_chunk)
		neighbor.m_chunk->m_isVertexArrayDirty = true;
}

void World::setBlockNeighborsLightDirty(const BlockInfo& bi)
{
	BlockInfo topBi = bi.getBlockInfoToTop();
	BlockInfo bottomBi = bi.getBlockInfoToBottom();
	BlockInfo northBi = bi.getBlockInfoToNorth();
	BlockInfo southBi = bi.getBlockInfoToSouth();
	BlockInfo eastBi = bi.getBlockInfoToEast();
	BlockInfo westBi = bi.getBlockInfoToWest();

	dirtyBlockLight(topBi);
	dirtyBlockLight(bottomBi);
	dirtyBlockLight(northBi);
	dirtyBlockLight(southBi);
	dirtyBlockLight(eastBi);
	dirtyBlockLight(westBi);
}

void World::setInitialLighting(Chunk* newChunk)
{
	setInitialSkyBlocks(newChunk);
	setInitialIlluminationBlocks(newChunk);
}

void World::setInitialSkyBlocks(Chunk* newChunk)
{
	BlockInfo bi(newChunk, 0);
	for (int column_XY = 0; column_XY < BLOCKS_PER_LAYER; column_XY++)
	{
		bi.m_chunk = newChunk; // Make sure we didn't leave the chunk. Ex: Not opaque for an entire column
		bi.m_blockIndex = getBlockIndexWithMaxZForBlockIndex(column_XY);
		initializeSkyDownward(bi);
	}
	for (int column_XY = 0; column_XY < BLOCKS_PER_LAYER; column_XY++)
	{
		bi.m_chunk = newChunk; // Make sure we didn't leave the chunk. Ex: Not opaque for an entire column
		bi.m_blockIndex = getBlockIndexWithMaxZForBlockIndex(column_XY);
		dirtySkyNeighborsDownward(bi);
	}
}

void World::setInitialIlluminationBlocks(Chunk* newChunk)
{
	BlockInfo bi(newChunk, 0);
	for (int column_XY = 0; column_XY < BLOCKS_PER_LAYER; column_XY++)
	{
		bi.m_chunk = newChunk; // Make sure we didn't leave the chunk. Ex: Opaque for an entire column
		bi.m_blockIndex = getBlockIndexWithMinZForBlockIndex(column_XY);
		initializeIlluminationUpward(bi);
	}
}

void World::initializeSkyDownward(BlockInfo& bi)
{
	while (!bi.isOpaque() && bi.Exists())
	{
		bi.setSkyFlag();
		bi.setLightValue(g_skyLightValue);
		bi.moveBottom();
	}
}

void World::initializeIlluminationUpward(BlockInfo& bi)
{
	while (!bi.isSky() && bi.Exists())
	{
		if (bi.getSelfIlluminationValue() > 0)
			forceDirtyBlockLight(bi);

		bi.moveTop();
	}
}

void World::dirtySkyNeighborsDownward(BlockInfo& bi)
{
	while (!bi.isOpaque() && bi.Exists())
	{
		setSkyBlockNeighborsLightDirty(bi);
		bi.moveBottom();
	}
}

void World::setSkyBlockNeighborsLightDirty(const BlockInfo& bi)
{
	BlockInfo northBi = bi.getBlockInfoToNorth();
	BlockInfo southBi = bi.getBlockInfoToSouth();
	BlockInfo eastBi = bi.getBlockInfoToEast();
	BlockInfo westBi = bi.getBlockInfoToWest();

	if (!northBi.isSky())
		dirtyBlockLight(northBi);
	if (!southBi.isSky())
		dirtyBlockLight(southBi);
	if (!eastBi.isSky())
		dirtyBlockLight(eastBi);
	if (!westBi.isSky())
		dirtyBlockLight(westBi);
}

void World::clearSkyDownward(BlockInfo bi)
{
	while (!bi.isOpaque() && bi.Exists())
	{
		bi.clearSkyFlag();
		forceDirtyBlockLight(bi);
		bi.moveBottom();
	}
}

void World::setSkyDownward(BlockInfo bi)
{
	while (!bi.isOpaque() && bi.Exists())
	{
		bi.setSkyFlag();
		forceDirtyBlockLight(bi);
		bi.moveBottom();
	}
}

void World::RaycastCheck()
{
	/*/
	if (g_cameraModeP1 == NO_CLIP)
		Camera1RaycastCheck();
	else
		Player1RaycastCheck();

	if (g_cameraModeP2 == NO_CLIP)
		Camera2RaycastCheck();
	else
		Player2RaycastCheck();
	//*/
}

bool World::PopulateChunkFromRLEStorage(Chunk* newChunk)
{
	PROFILE_SCOPE_FUNCTION();
	auto found = m_rleChunks.find(newChunk->m_chunkCoords);
	if (found != m_rleChunks.end())
	{
		newChunk->populateFromRLE(found->second);
		return true;
	}
	return false;
}

void World::SaveOutRLEChunks()
{
	std::map<IntVector2, std::vector<uchar>>::iterator iterator;
	for(auto i = m_rleChunks.begin(); i != m_rleChunks.end(); ++i)
	{
		Chunk::SaveToFile(i->first, i->second);
	}
}
