#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Networking/Net.hpp"
#include "RaycastResult.hpp"
#include "Engine/Math/AABB3D.hpp"
#include "Game/GameSession.hpp"
//////////////////////////////////////////////////////////////////////////
class Camera3D;
class PlayerInfo;
class NetMessage;
class Actor;
class BinaryStream;
//////////////////////////////////////////////////////////////////////////
enum PlayerMovementState
{
	IDLE,
	WALKING,
	RUNNING,
	JUMPING,
	FALLING,
	NUM_PLAYER_MOVEMENT_STATES
};
//////////////////////////////////////////////////////////////////////////
const float PLAYER_BOX_HEIGHT = 2;//1.86f;
const float PLAYER_BOX_WIDTH = 0.75f;//0.60f;
const float PLAYER_BOX_RADIUS = PLAYER_BOX_WIDTH / 2.0f;
const float PLAYER_BOX_HEIGHT_RADIUS = PLAYER_BOX_HEIGHT / 2.0f;
const float PLAYER_EYE_LEVEL_FROM_CENTER =/* 0.75;*/ 0.69f;
const float PLAYER_EYE_LEVEL_FROM_FEET = /*1.75;*/  1.62f;
//////////////////////////////////////////////////////////////////////////
struct ActorSync
{
	Vector3 position;
	float pitchDegreesAboutY;
	float yawDegreesAboutZ;
	float clientTime;
	uint8 playerId;

	void write(BinaryStream &stream);
	void read(BinaryStream &stream);
	void Sync(Actor* ship);
};
//////////////////////////////////////////////////////////////////////////
class Actor
{
	friend class GameSession; // So Game Session has access to portions of Actors
public:
	Actor(const Vector3& pos, uint8 playerId);
	virtual ~Actor();

	virtual void Update(float deltaSeconds);
	virtual void Render() const;
	void Sync(ActorSync* ship);
	const double getAge() const;
	void HandleInput(float deltaSeconds);
	//virtual const bool isDead() const = 0;


	static void  AppendCreateInfo(NetMessage *msg, void *ptr);
	static void* ProcessCreateInfo(NetMessage *msg, nos::NetObject *nop);
	static void  ProcessDestroyInfo(NetMessage *msg, void *ptr);
	//*/
	static void  ProcessSnap(void* last_snapshot, NetMessage* update_msg);
	static void  AppendSnap(NetMessage* update_msg, void* current_snapshot);
	static void  ApplySnap(void* local_obj, void* last_snapshot);
	static void  GetCurrentSnap(void* current_snapshot, void* local_obj);
	//*/


	void moveForward(float deltaSeconds, bool isSprinting = false);
	void moveBackward(float deltaSeconds, bool isSprinting = false);
	void moveLeft(float deltaSeconds, bool isSprinting = false);
	void moveRight(float deltaSeconds, bool isSprinting = false);
	void jump(float deltaSeconds);
	Vector3 getForwardXYZ() const;
	Vector3 getForwardXY() const;
	Vector3 getLeftXY() const;

	void RaycastCheck();
public:
	Vector3 m_position;
	uint8 m_playerId;
	uint16 m_noId = INVALID_NETWORK_ID;
	float m_pitchDegreesAboutY;
	float m_yawDegreesAboutZ;
	RaycastResult m_myRaycast;
	PlayerMovementState m_playerMovementState;
	Camera3D* m_camera;
	PlayerInfo* mo_playerInfo;
protected:
	Vector3 m_velocity;
	double	m_timeOfBirth;
	uint    m_hp;
	AABB3D m_physicsBox;
	BlockInfo m_feetBlockInfo;
	uint m_characterVBO;
	bool m_moving = false;
protected:

	void SetPosition(const Vector3& pos);
	void MoveUpOneBlock(BlockInfo& bi);
	void MovePosition(float deltaSeconds, Vector3 direction, bool isSprinting = false);

	void CorrectivePhysics();
	//void predictivePhysics(float deltaSeconds);
	void ApplyGravity(float deltaSeconds);
	//void applyAcceleration(float magnitude, Vector3 direction, float deltaSeconds);
	void PushPlayerUp();
	bool PlayerFeetAreInSolid(BlockInfo& bi);
	const bool isPlayerStandingOnSolid() const;


	const Vector3 getFeetCenter() const;
	const Vector3 getBottomNorthEast() const;
	const Vector3 getBottomSouthEast() const;
	const Vector3 getBottomNorthWest() const;
	const Vector3 getBottomSouthWest() const;
	const Vector3 getTopNorthEast() const;
	const Vector3 getTopSouthEast() const;
	const Vector3 getTopNorthWest() const;
	const Vector3 getTopSouthWest() const;
	const Vector3 getPlayerEyePosition() const;

protected:
	void drawPlayerAvatar() const;
	void UpdatePlayerAvatar(float deltaSeconds);
	std::vector<Vertex3D> m_playerDrawArray;
	void drawPlayerHead() const;
	void drawPlayerRLeg() const;
	void drawPlayerLLeg() const;
	void drawPlayerRArm() const;
	void drawPlayerLArm() const;

	float m_bodyDegreesAboutZ;

	float m_armDegreesAboutX;
	float m_armDegreesAboutY;
	float m_legDegreesAboutX;
	float timeInAnimation;
};

inline const Vector3 Actor::getFeetCenter() const
{
	Vector3 result = m_position;
	result.z -= PLAYER_BOX_HEIGHT_RADIUS;
	return result;
}

inline const Vector3 Actor::getBottomNorthEast() const
{
	float x = m_physicsBox.maxs.x;
	float y = m_physicsBox.maxs.y;
	float z = m_physicsBox.mins.z;
	return Vector3(x, y, z);
}
inline const Vector3 Actor::getBottomSouthEast() const
{
	float x = m_physicsBox.maxs.x;
	float y = m_physicsBox.mins.y;
	float z = m_physicsBox.mins.z;
	return Vector3(x, y, z);
}
inline const Vector3 Actor::getBottomNorthWest() const
{
	float x = m_physicsBox.mins.x;
	float y = m_physicsBox.maxs.y;
	float z = m_physicsBox.mins.z;
	return Vector3(x, y, z);
}
inline const Vector3 Actor::getBottomSouthWest() const
{
	return m_physicsBox.mins;
}
inline const Vector3 Actor::getTopNorthEast() const
{
	return m_physicsBox.maxs;
}
inline const Vector3 Actor::getTopSouthEast() const
{
	float x = m_physicsBox.maxs.x;
	float y = m_physicsBox.mins.y;
	float z = m_physicsBox.maxs.z;
	return Vector3(x, y, z);
}
inline const Vector3 Actor::getTopNorthWest() const
{
	float x = m_physicsBox.mins.x;
	float y = m_physicsBox.maxs.y;
	float z = m_physicsBox.maxs.z;
	return Vector3(x, y, z);
}
inline const Vector3 Actor::getTopSouthWest() const
{
	float x = m_physicsBox.mins.x;
	float y = m_physicsBox.mins.y;
	float z = m_physicsBox.maxs.z;
	return Vector3(x, y, z);
}
/*/
inline const Vector3 Actor::getPlayerPosition() const
{
	return m_position;
}
//*/

inline const Vector3 Actor::getPlayerEyePosition() const
{
	return m_position + (Vector3::ZAXIS * PLAYER_EYE_LEVEL_FROM_CENTER);
}