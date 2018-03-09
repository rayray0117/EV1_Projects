#include "Game/Actor.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/Camera3D.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameSession.hpp"
#include "Game/PlayerInfo.hpp"
#include "Engine/Networking/NetMessage.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Game/PlayerModelReference.hpp"
#include "Engine/Core/BinaryStream.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Networking/NetObjectSystem.hpp"
#include "Engine/Networking/NetConnection.hpp"
#include "Engine/Networking/TCPSession.hpp"
//////////////////////////////////////////////////////////////////////////
Actor::Actor(const Vector3& pos, uint8 playerId)
	:  m_position(pos)
	, m_timeOfBirth(GetCurrentTimeSeconds())
	, m_playerId(playerId)
	, m_velocity(Vector3::ZERO)
	, m_playerMovementState(IDLE)
	, m_pitchDegreesAboutY(0.0f)
	, m_yawDegreesAboutZ(0.0f)
	, m_physicsBox(m_position, PLAYER_BOX_RADIUS, PLAYER_BOX_RADIUS, PLAYER_BOX_HEIGHT_RADIUS)
{
	m_camera = new Camera3D();
	mo_playerInfo = g_gameSession->getPlayerInfo(playerId);
	mo_playerInfo->mo_playerCharacter = this;

	m_characterVBO = g_renderer->CreateAndGetVBOId();
	g_renderer->SetVBO3DData(m_characterVBO, characterModel, NUM_CHAR_MODEL_VERTS, RHIEnum::USAGE_STATIC_DRAW);
}

Actor::~Actor()
{
	if (m_noId != INVALID_NETWORK_ID)
	{
		nos::NetObject* obj = nos::FindNetObject(m_noId);
		nos::UnregisterNetObject(obj);
	}
}

//////////////////////////////////////////////////////////////////////////
void Actor::Update(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	if (mo_playerInfo != g_gameSession->my_player && calcDistanceSquared(Vector2(m_position), Vector2(g_gameSession->getCamera()->m_position)) > MAX_CHUNK_VIS_DIST * MAX_CHUNK_VIS_DIST)
	{
		return;
	}

	HandleInput(deltaSeconds);

	/*/
	applyFirction(deltaSeconds);
	//*/
	//if (!m_isFlying)
	ApplyGravity(deltaSeconds);
	/*/
	clampVelocity();
	//*/
	//predictivePhysics(deltaSeconds);

	SetPosition(m_position + m_velocity);

	CorrectivePhysics();

	//UpdatePlayerDrawArray();

	//*/
	if (g_gameSession->getPlayerInfo(m_playerId) == g_gameSession->my_player)
	{
		m_camera->UpdateCameraFromMouse(deltaSeconds);
		//*/
		m_pitchDegreesAboutY = m_camera->m_degreesAboutY;
		m_yawDegreesAboutZ = m_camera->m_degreesAboutZ;
	}
	m_camera->m_position = getPlayerEyePosition();
	
	UpdatePlayerAvatar(deltaSeconds);
	RaycastCheck();
}

void Actor::Render() const
{
	PROFILE_SCOPE_FUNCTION();
	if (mo_playerInfo != g_gameSession->my_player && calcDistanceSquared(Vector2(m_position), Vector2(g_gameSession->getCamera()->m_position)) > MAX_CHUNK_VIS_DIST * MAX_CHUNK_VIS_DIST)
	{
		return;
	}
	//m_camera->Render();
	drawPlayerAvatar();
}

void Actor::Sync(ActorSync* sync)
{
	PROFILE_SCOPE_FUNCTION();

	if(!m_position.isMostlyEqualTo(sync->position))
 		m_position = sync->position;
	/*/
	if (g_gameSession->getPlayerInfo(m_playerId) == g_gameSession->my_player)
		return;
	//*/
	m_pitchDegreesAboutY = sync->pitchDegreesAboutY;
	m_yawDegreesAboutZ = sync->yawDegreesAboutZ;
	
	Update((float)GetCurrentTimeSeconds() - sync->clientTime);
}

//////////////////////////////////////////////////////////////////////////
const double Actor::getAge() const
{
	double currentTime = GetCurrentTimeSeconds();
	return currentTime - m_timeOfBirth;
}
//////////////////////////////////////////////////////////////////////////
void Actor::AppendCreateInfo(NetMessage *msg, void *ptr)
{
	Actor *actor = (Actor*)ptr;

	// constant
	msg->write(actor->m_playerId);
	msg->sender = g_gameSession->m_session->my_connection;
	// initial state
	msg->write((Vector3)actor->m_position);
}

void* Actor::ProcessCreateInfo(NetMessage *msg, nos::NetObject *nop)
{
	uint8 player_idx;
	msg->read(player_idx);
	Vector3 pos;
	msg->read(pos);

	Actor *actor = new Actor(pos, player_idx);
	g_theGame->m_world->m_actors.push_back(actor);
	actor->m_noId = nop->net_id;
	
	//*/
	ActorSync* sync = new ActorSync();
	nop->current_snapshot = sync;
	nop->last_received_snapshot = new ActorSync();
	//*/


	return actor;
}

void Actor::ProcessDestroyInfo(NetMessage *msg, void *ptr)
{
	Actor *actor = (Actor*)ptr;

	SAFE_DELETE(actor);
	UNUSED(msg);
}
//////////////////////////////////////////////////////////////////////////
void Actor::ProcessSnap(void* last_snapshot, NetMessage* update_msg)
{
	float hostCurrentTime;
	update_msg->read(hostCurrentTime);
	//*/
	ActorSync* ship_sync = (ActorSync*)last_snapshot;
	ship_sync->read(*update_msg);
	if (g_gameSession->isHost())
	{
		PlayerInfo* player =g_gameSession->getPlayerInfo(update_msg->sender->connection_index);
		ship_sync->clientTime = (hostCurrentTime - player->m_clientsJoinedTime) + player->m_hostRecievedTime;
	}
	else
	{
		ship_sync->clientTime = (hostCurrentTime - g_gameSession->m_joinAtHostTime) + g_gameSession->m_localTimeReceivedJoinResponse;
	}
	//*/
	//UNUSED(last_snapshot);
}

void Actor::AppendSnap(NetMessage* update_msg, void* current_snapshot)
{
	ActorSync* ship_sync = (ActorSync*)current_snapshot;

	if (!(g_gameSession->isHost() || g_gameSession->my_player->m_id == ship_sync->playerId))
	{
		update_msg->message_type_index = NET_MSG_INVALID;
		return;
	}


	update_msg->write((float)GetCurrentTimeSeconds());
	update_msg->sender = g_gameSession->m_session->my_connection;
	ship_sync->write(*update_msg);
}

void Actor::ApplySnap(void* local_obj, void* last_snapshot)
{
	PROFILE_SCOPE_FUNCTION();
	//*/
	Actor* ship = (Actor*)local_obj;
	ActorSync* ship_sync = (ActorSync*)last_snapshot;

	if (ship->mo_playerInfo == g_gameSession->my_player)
	{
		return;
	}

	ship->Sync(ship_sync);
	//*/
	//UNUSED(local_obj);
	//UNUSED(last_snapshot);
}

void Actor::GetCurrentSnap(void* current_snapshot, void* local_obj)
{
	PROFILE_SCOPE_FUNCTION();
	//*/
	ActorSync* ship_sync = (ActorSync*)current_snapshot;
	Actor* ship = (Actor*)local_obj;

	/*/
	if (ship->mo_playerInfo == g_gameSession->my_player)
	{
		return;
	}
	//*/
	ship_sync->Sync(ship);
	//*/
	//UNUSED(current_snapshot);
	//UNUSED(local_obj);
}
//////////////////////////////////////////////////////////////////////////
void Actor::moveForward(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction = getForwardXY();
	MovePosition(deltaSeconds, direction, isSprinting);
}

void Actor::moveBackward(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction = -getForwardXY();
	MovePosition(deltaSeconds, direction, isSprinting);
}

void Actor::moveLeft(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction = getLeftXY();
	MovePosition(deltaSeconds, direction, isSprinting);
}

void Actor::moveRight(float deltaSeconds, bool isSprinting /*= false*/)
{
	Vector3 direction = -getLeftXY();
	MovePosition(deltaSeconds, direction, isSprinting);
}

void Actor::jump(float deltaSeconds)
{
	if (m_playerMovementState != FALLING && m_playerMovementState != JUMPING)
	{
		m_velocity = JUMP_VELOCITY * deltaSeconds;
		m_playerMovementState = JUMPING;
	}
}


Vector3 Actor::getForwardXYZ() const
{
	return Vector3(cosDegrees(m_yawDegreesAboutZ)*cosDegrees(m_pitchDegreesAboutY), sinDegrees(m_yawDegreesAboutZ)*cosDegrees(m_pitchDegreesAboutY), -sinDegrees(m_pitchDegreesAboutY));
}

Vector3 Actor::getForwardXY() const
{
	return Vector3(cosDegrees(m_yawDegreesAboutZ), sinDegrees(m_yawDegreesAboutZ), 0.f);
}

Vector3 Actor::getLeftXY() const
{
	Vector3 forward = getForwardXY();
	return Vector3(-forward.y, forward.x, 0.f);
}

void Actor::RaycastCheck()
{
	m_myRaycast = g_theGame->m_world->raycast(getPlayerEyePosition(), getForwardXYZ(), RAYCAST_DIST);
}

void Actor::HandleInput(float deltaSeconds)
{
	bool isSprinting = false;
	m_moving = false;

	PlayerInfo* info = g_gameSession->getPlayerInfo(m_playerId);
	if (info == nullptr) return;

	if (info->m_up == 1)
	{
		moveForward(deltaSeconds, isSprinting);
		m_moving = true;
	}
	if (info->m_down == 1)
	{
		moveBackward(deltaSeconds, isSprinting);
		m_moving = true;
	}
	if (info->m_right == 1)
	{
		moveRight(deltaSeconds, isSprinting);
		m_moving = true;
	}
	if (info->m_left == 1)
	{
		moveLeft(deltaSeconds, isSprinting);
		m_moving = true;
	}
	/*/
	if (wasInputJustPressed(INPUT_X))
	{
		jump(.16f);
	}
	//*/
}

void Actor::SetPosition(const Vector3& pos)
{
	m_position = pos;

	m_physicsBox = AABB3D(m_position, PLAYER_BOX_RADIUS, PLAYER_BOX_RADIUS, PLAYER_BOX_HEIGHT_RADIUS);
}

void Actor::MoveUpOneBlock(BlockInfo& bi)
{
	bi.moveTop();
	IntVector3 blockCoords;
	if (bi.Exists())
		blockCoords = bi.m_chunk->getBlockCoordsForBlockIndex(bi.m_blockIndex);
	else
	{
		blockCoords = IntVector3(0, 0, CHUNK_Z);
	}
	m_position.z = blockCoords.z + PLAYER_BOX_HEIGHT_RADIUS;
	SetPosition(m_position);
}

void Actor::MovePosition(float deltaSeconds, Vector3 direction, bool isSprinting /*= false*/)
{
	float speed = PLAYER_MOVEMENT_SPEED;
	if (isSprinting)
		speed *= CAMERA_SPRINT_MULTIPIER;

	Vector3 pos = m_position + speed * deltaSeconds * direction;
	SetPosition(pos);
}

void Actor::CorrectivePhysics()
{
	if (m_position.z < 0.f)
	{
		m_position.z = 1.f;
		SetPosition(m_position);
	}
	PushPlayerUp();
}

void Actor::ApplyGravity(float deltaSeconds)
{
	m_velocity += GRAVITY_ACCELERATION * deltaSeconds * deltaSeconds;
}

void Actor::PushPlayerUp()
{
	BlockInfo bi;
	bool isInSolid = PlayerFeetAreInSolid(bi);
	if (isInSolid)
	{
		m_velocity.z = 0.0f;
		m_playerMovementState = IDLE;
	}
	while (isInSolid)
	{
		MoveUpOneBlock(bi);
		if (bi.Exists())
			isInSolid = PlayerFeetAreInSolid(bi);
		else
			break;
	}
	m_feetBlockInfo = bi;
}

bool Actor::PlayerFeetAreInSolid(BlockInfo& bi)
{
	if (g_theGame->m_world->isWorldPositionInSolid(getBottomSouthWest(), bi))
		return true;
	if (g_theGame->m_world->isWorldPositionInSolid(getBottomNorthWest(), bi))
		return true;
	if (g_theGame->m_world->isWorldPositionInSolid(getBottomSouthEast(), bi))
		return true;
	if (g_theGame->m_world->isWorldPositionInSolid(getBottomNorthEast(), bi))
		return true;

	return false;
}

const bool Actor::isPlayerStandingOnSolid() const
{
	BlockInfo blockToCheck;
	blockToCheck = g_theGame->m_world->getBlockInfoAtWorldPos(getBottomNorthEast());
	blockToCheck.moveBottom();
	if (blockToCheck.isSolid())
	{
		return true;
	}

	blockToCheck = g_theGame->m_world->getBlockInfoAtWorldPos(getBottomSouthEast());
	blockToCheck.moveBottom();
	if (blockToCheck.isSolid())
	{
		return true;
	}

	blockToCheck = g_theGame->m_world->getBlockInfoAtWorldPos(getBottomNorthWest());
	blockToCheck.moveBottom();
	if (blockToCheck.isSolid())
	{
		return true;
	}

	blockToCheck = g_theGame->m_world->getBlockInfoAtWorldPos(getBottomSouthWest());
	blockToCheck.moveBottom();
	if (blockToCheck.isSolid())
	{
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
void Actor::drawPlayerAvatar() const
{
	g_renderer->BindTexture(g_renderer->CreateOrGetTexture(CHAR_TEXTURE_PATH));

	g_renderer->pushMatrix();

	g_renderer->translate3D(getFeetCenter());
	g_renderer->rotate3D(-90.f, Vector3::ZAXIS);

	// Head
	drawPlayerHead();


	//
	g_renderer->rotate3D(m_bodyDegreesAboutZ, Vector3::ZAXIS);
	g_renderer->drawVBO3D(m_characterVBO, NUM_VERTS_PER_PART, RHIEnum::PRIMATIVE_QUADS, BODY_VERTS_START);

	drawPlayerRLeg();
	drawPlayerLLeg();
	drawPlayerRArm();
	drawPlayerLArm();

	g_renderer->popMatrix();
}

void Actor::UpdatePlayerAvatar(float deltaSeconds)
{
	if (m_bodyDegreesAboutZ > m_yawDegreesAboutZ + ALLOWED_BODY_ROTATION_OFFSET)
		m_bodyDegreesAboutZ = m_yawDegreesAboutZ + ALLOWED_BODY_ROTATION_OFFSET;
	else if (m_bodyDegreesAboutZ < m_yawDegreesAboutZ - ALLOWED_BODY_ROTATION_OFFSET)
		m_bodyDegreesAboutZ = m_yawDegreesAboutZ - ALLOWED_BODY_ROTATION_OFFSET;

	timeInAnimation += deltaSeconds;
//	PlayerInfo* info = g_gameSession->getPlayerInfo(m_playerId);
	if (m_velocity.isMostlyEqualTo(Vector3::ZERO) /*&& !m_moving*/)
	{
		m_armDegreesAboutY = ARM_Y_ANGLE * sin(timeInAnimation) + ARM_Y_ANGLE;
		m_armDegreesAboutX = 0;
		m_legDegreesAboutX = 0;
	}
	else
	{
		m_armDegreesAboutY = 0;
		m_armDegreesAboutX = ARM_X_ANGLE * -sin(timeInAnimation*WALKING_SPEED_ANIM_MULTIPLIER);
		m_legDegreesAboutX = LEG_X_ANGLE * sin(timeInAnimation*WALKING_SPEED_ANIM_MULTIPLIER);
	}
}

void Actor::drawPlayerHead() const
{
	g_renderer->pushMatrix();
	g_renderer->translate3D(Vector3::ZAXIS*PLAYER_EYE_LEVEL_FROM_FEET);
	g_renderer->rotate3D(m_yawDegreesAboutZ, Vector3::ZAXIS);

	float pitch = clamp(m_pitchDegreesAboutY, MAX_HEAD_UP_ANGLE, MAX_HEAD_DOWN_ANGLE);
	g_renderer->rotate3D(-pitch, Vector3::XAXIS);
	g_renderer->drawVBO3D(m_characterVBO, NUM_VERTS_PER_PART, RHIEnum::PRIMATIVE_QUADS, HEAD_VERTS_START);
	g_renderer->popMatrix();
}

void Actor::drawPlayerRLeg() const
{
	g_renderer->pushMatrix();
	g_renderer->translate3D(Vector3(2.f, 0.f, 12.f)*scaleDown); //Referring to PlayerModelReference
	g_renderer->rotate3D(-m_legDegreesAboutX, Vector3::XAXIS);
	g_renderer->drawVBO3D(m_characterVBO, NUM_VERTS_PER_PART, RHIEnum::PRIMATIVE_QUADS, R_LEG_VERTS_START);
	g_renderer->popMatrix();
}

void Actor::drawPlayerLLeg() const
{
	g_renderer->pushMatrix();
	g_renderer->translate3D(Vector3(-2.f, 0.f, 12.f)*scaleDown); //Referring to PlayerModelReference
	g_renderer->rotate3D(m_legDegreesAboutX, Vector3::XAXIS);
	g_renderer->drawVBO3D(m_characterVBO, NUM_VERTS_PER_PART, RHIEnum::PRIMATIVE_QUADS, L_LEG_VERTS_START);
	g_renderer->popMatrix();
}

void Actor::drawPlayerRArm() const
{
	g_renderer->pushMatrix();
	g_renderer->translate3D(Vector3(4.f, 0.f, 24.f)*scaleDown); //Referring to PlayerModelReference
	g_renderer->rotate3D(-m_armDegreesAboutX, Vector3::XAXIS);
	g_renderer->rotate3D(-m_armDegreesAboutY, Vector3::YAXIS);
	g_renderer->drawVBO3D(m_characterVBO, NUM_VERTS_PER_PART, RHIEnum::PRIMATIVE_QUADS, R_ARM_VERTS_START);
	g_renderer->popMatrix();
}

void Actor::drawPlayerLArm() const
{
	g_renderer->pushMatrix();
	g_renderer->translate3D(Vector3(-4.f, 0.f, 24.f)*scaleDown); //Referring to PlayerModelReference
	g_renderer->rotate3D(m_armDegreesAboutX, Vector3::XAXIS);
	g_renderer->rotate3D(m_armDegreesAboutY, Vector3::YAXIS);
	g_renderer->drawVBO3D(m_characterVBO, NUM_VERTS_PER_PART, RHIEnum::PRIMATIVE_QUADS, L_ARM_VERTS_START);
	g_renderer->popMatrix();
}

void ActorSync::write(BinaryStream &stream)
{
	stream.write(position);
	stream.write(pitchDegreesAboutY);
	stream.write(yawDegreesAboutZ);
}

void ActorSync::read(BinaryStream &stream)
{
	stream.read(position);
	stream.read(pitchDegreesAboutY);
	stream.read(yawDegreesAboutZ);
}

void ActorSync::Sync(Actor* actor)
{
	PROFILE_SCOPE_FUNCTION();
	position = actor->m_position;
	pitchDegreesAboutY = actor->m_pitchDegreesAboutY;
	yawDegreesAboutZ = actor->m_yawDegreesAboutZ;
	playerId = actor->m_playerId;
}
