#pragma once
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Math/AABB2.hpp"
//////////////////////////////////////////////////////////////////////////
const float ALLOWED_BODY_ROTATION_OFFSET = 20.f;
const float ARM_X_ANGLE = 45.f;
const float ARM_Y_ANGLE = 5.f;
const float LEG_X_ANGLE = 45.f;
const float WALKING_SPEED_ANIM_MULTIPLIER = 5.f;
const float MAX_HEAD_UP_ANGLE = -40.f;
const float MAX_HEAD_DOWN_ANGLE = 25.f;
//////////////////////////////////////////////////////////////////////////
const std::string CHAR_TEXTURE_PATH = /*"Data/Images/TestCharTexture.jpg";*/"Data/Images/CharText.png";
constexpr int NUM_CHAR_MODEL_VERTS = 144;
constexpr int NUM_VERTS_PER_PART = 24;
constexpr int HEAD_VERTS_START = 0;
constexpr int BODY_VERTS_START = NUM_VERTS_PER_PART;
constexpr int R_LEG_VERTS_START = NUM_VERTS_PER_PART * 2;
constexpr int L_LEG_VERTS_START = NUM_VERTS_PER_PART * 3;
constexpr int R_ARM_VERTS_START = NUM_VERTS_PER_PART * 4;
constexpr int L_ARM_VERTS_START = NUM_VERTS_PER_PART * 5;
//////////////////////////////////////////////////////////////////////////
constexpr float scaleDown = 1.89f / 32.f;
//////////////////////////////////////////////////////////////////////////
Vector3 headVerts[8] = {
	Vector3(-4.0000f,  4.0000f,-4.0000f),
	Vector3(4.0000f,  4.0000f,-4.0000f),
	Vector3(-4.0000f,  4.0000f, 4.0000f),
	Vector3(4.0000f,  4.0000f, 4.0000f),
	Vector3(-4.0000f, -4.0000f, 4.0000f),
	Vector3(4.0000f, -4.0000f, 4.0000f),
	Vector3(-4.0000f, -4.0000f,-4.0000f),
	Vector3(4.0000f, -4.0000f,-4.0000f),
};

AABB2 headUVs[6] = {
	AABB2(0.1250f, 0.0000f, 0.2500f, 0.1250f), // Head Top
	AABB2(0.2500f, 0.0000f, 0.3750f, 0.1250f), // Head Bottom
	AABB2(0.0000f, 0.1250f, 0.1250f, 0.2500f), // Head Right
	AABB2(0.1250f, 0.1250f, 0.2500f, 0.2500f), // Head Front
	AABB2(0.2500f, 0.1250f, 0.3750f, 0.2500f), // Head Left
	AABB2(0.3750f, 0.1250f, 0.5000f, 0.2500f), // Head Back
};
//////////////////////////////////////////////////////////////////////////
Vector3 torsoVerts[8] = {
	Vector3(-4.0000f,  2.0000f, 12.0000f),
	Vector3(4.0000f,  2.0000f, 12.0000f),
	Vector3(-4.0000f,  2.0000f, 24.0000f),
	Vector3(4.0000f,  2.0000f, 24.0000f),
	Vector3(-4.0000f, -2.0000f, 24.0000f),
	Vector3(4.0000f, -2.0000f, 24.0000f),
	Vector3(-4.0000f, -2.0000f, 12.0000f),
	Vector3(4.0000f, -2.0000f, 12.0000f)
};

AABB2 torsoUVs[6] = {
	AABB2(0.3125f, 0.2500f, 0.4375f, 0.3125f), // Torso Top
	AABB2(0.4375f, 0.2500f, 0.5625f, 0.3125f), // Torso Bottom
	AABB2(0.2500f, 0.3125f, 0.3125f, 0.5000f), // Torso Right
	AABB2(0.3125f, 0.3125f, 0.4375f, 0.5000f), // Torso Front
	AABB2(0.5625f, 0.3125f, 0.6250f, 0.5000f), // Torso Left
	AABB2(0.4375f, 0.3125f, 0.5625f, 0.5000f), // Torso Back
};
//////////////////////////////////////////////////////////////////////////
// Move x -2, move z + 12
Vector3 l_LegVerts[8] = {
	Vector3(-2.0000f,  2.0000f,-12.0000f),
	Vector3(2.0000f,  2.0000f,-12.0000f),
	Vector3(-2.0000f,  2.0000f,  0.0000f),
	Vector3(2.0000f,  2.0000f,  0.0000f),
	Vector3(-2.0000f, -2.0000f,  0.0000f),
	Vector3(2.0000f, -2.0000f,  0.0000f),
	Vector3(-2.0000f, -2.0000f,-12.0000f),
	Vector3(2.0000f, -2.0000f,-12.0000f)
};

AABB2 r_LegUVs[6] = {
	AABB2(0.0625f, 0.2500f, 0.1250f, 0.3125f), // Right Leg Top
	AABB2(0.1250f, 0.2500f, 0.1875f, 0.3125f), // Right Leg Bottom
	AABB2(0.0000f, 0.3125f, 0.0625f, 0.5000f), // Right Leg Right
	AABB2(0.0625f, 0.3125f, 0.1250f, 0.5000f), // Right Leg Front
	AABB2(0.1250f, 0.3125f, 0.1875f, 0.5000f), // Right Leg Left
	AABB2(0.1875f, 0.3125f, 0.2500f, 0.5000f), // Right Leg Back
};
//////////////////////////////////////////////////////////////////////////
// move x + 2, move z +12
Vector3 r_LegVerts[8] = {
	Vector3(-2.0000f, 2.0000f,-12.0000f),
	Vector3(2.0000f, 2.0000f,-12.0000f),
	Vector3(-2.0000f, 2.0000f,  0.0000f),
	Vector3(2.0000f, 2.0000f,  0.0000f),
	Vector3(-2.0000f,-2.0000f,  0.0000f),
	Vector3(2.0000f,-2.0000f,  0.0000f),
	Vector3(-2.0000f,-2.0000f,-12.0000f),
	Vector3(2.0000f,-2.0000f,-12.0000f),
};

AABB2 l_LegUVs[6] = {
	AABB2(0.3125f, 0.7500f, 0.3750f, 0.8125f), // Left Leg Top
	AABB2(0.3750f, 0.7500f, 0.4375f, 0.8125f), // Left Leg Bottom
	AABB2(0.2500f, 0.8125f, 0.3125f, 1.0000f), // Left Leg Right
	AABB2(0.3125f, 0.8125f, 0.3750f, 1.0000f), // Left Leg Front
	AABB2(0.3750f, 0.8125f, 0.4375f, 1.0000f), // Left Leg Left
	AABB2(0.4375f, 0.8125f, 0.5000f, 1.0000f), // Left Leg Back
};
//////////////////////////////////////////////////////////////////////////
// Move x-4, move z + 24
Vector3 l_ArmVerts[8] = {
	Vector3(-4.0000f,  2.0000f,-12.0000f),
	Vector3(0.0000f,  2.0000f,-12.0000f),
	Vector3(-4.0000f,  2.0000f,  0.0000f),
	Vector3(0.0000f,  2.0000f,  0.0000f),
	Vector3(-4.0000f, -2.0000f,  0.0000f),
	Vector3(0.0000f, -2.0000f,  0.0000f),
	Vector3(-4.0000f, -2.0000f,-12.0000f),
	Vector3(0.0000f, -2.0000f,-12.0000f),
};

AABB2 r_ArmUVs[6] = {
	AABB2(0.6875f, 0.2500f, 0.7500f, 0.3125f), // Right Arm Top
	AABB2(0.7500f, 0.2500f, 0.8125f, 0.3125f), // Right Arm Bottom
	AABB2(0.6250f, 0.3125f, 0.6875f, 0.5000f), // Right Arm Right
	AABB2(0.6875f, 0.3125f, 0.7500f, 0.5000f), // Right Arm Front
	AABB2(0.7500f, 0.3125f, 0.8125f, 0.5000f), // Right Arm Left
	AABB2(0.8125f, 0.3125f, 0.8750f, 0.5000f), // Right Arm Back
};

//////////////////////////////////////////////////////////////////////////
// Move x +4, move z +24
Vector3 r_ArmVerts[8] = {
	Vector3(0.0000f,  2.0000f, -12.0000f),
	Vector3(4.0000f,  2.0000f, -12.0000f),
	Vector3(0.0000f,  2.0000f,   0.0000f),
	Vector3(4.0000f,  2.0000f,   0.0000f),
	Vector3(0.0000f, -2.0000f,   0.0000f),
	Vector3(4.0000f, -2.0000f,   0.0000f),
	Vector3(0.0000f, -2.0000f, -12.0000f),
	Vector3(4.0000f, -2.0000f, -12.0000f),
};

AABB2 l_ArmUVs[6] = {
	AABB2(0.5625f, 0.7500f, 0.6250f, 0.8125f), // Left Arm Top
	AABB2(0.6250f, 0.7500f, 0.6875f, 0.8125f), // Left Arm Bottom
	AABB2(0.5000f, 0.8125f, 0.5625f, 1.0000f), // Left Arm Right
	AABB2(0.5625f, 0.8125f, 0.6250f, 1.0000f), // Left Arm Front
	AABB2(0.6250f, 0.8125f, 0.6875f, 1.0000f), // Left Arm Left
	AABB2(0.6875f, 0.8125f, 0.7500f, 1.0000f), // Left Arm Back
};
//////////////////////////////////////////////////////////////////////////
Vertex3D characterModel[] = {
	//Head North
	Vertex3D(headVerts[2] * scaleDown, Vector2(headUVs[3].maxs.x, headUVs[3].mins.y)),
	Vertex3D(headVerts[3] * scaleDown, Vector2(headUVs[3].mins.x, headUVs[3].mins.y)),
	Vertex3D(headVerts[1] * scaleDown, Vector2(headUVs[3].mins.x, headUVs[3].maxs.y)),
	Vertex3D(headVerts[0] * scaleDown, Vector2(headUVs[3].maxs.x, headUVs[3].maxs.y)),
	// Head Top
	Vertex3D(headVerts[4] * scaleDown, Vector2(headUVs[0].maxs.x, headUVs[0].mins.y)),
	Vertex3D(headVerts[5] * scaleDown, Vector2(headUVs[0].mins.x, headUVs[0].mins.y)),
	Vertex3D(headVerts[3] * scaleDown, Vector2(headUVs[0].mins.x, headUVs[0].maxs.y)),
	Vertex3D(headVerts[2] * scaleDown, Vector2(headUVs[0].maxs.x, headUVs[0].maxs.y)),
	// Head South
	Vertex3D(headVerts[6] * scaleDown, Vector2(headUVs[5].mins.x, headUVs[5].maxs.y)),
	Vertex3D(headVerts[7] * scaleDown, Vector2(headUVs[5].maxs.x, headUVs[5].maxs.y)),
	Vertex3D(headVerts[5] * scaleDown, Vector2(headUVs[5].maxs.x, headUVs[5].mins.y)),
	Vertex3D(headVerts[4] * scaleDown, Vector2(headUVs[5].mins.x, headUVs[5].mins.y)),
	// Head Bottom
	Vertex3D(headVerts[0] * scaleDown, Vector2(headUVs[1].maxs.x, headUVs[1].mins.y)),
	Vertex3D(headVerts[1] * scaleDown, Vector2(headUVs[1].mins.x, headUVs[1].mins.y)),
	Vertex3D(headVerts[7] * scaleDown, Vector2(headUVs[1].mins.x, headUVs[1].maxs.y)),
	Vertex3D(headVerts[6] * scaleDown, Vector2(headUVs[1].maxs.x, headUVs[1].maxs.y)),
	// Head East
	Vertex3D(headVerts[3] * scaleDown, Vector2(headUVs[2].maxs.x, headUVs[2].mins.y)),
	Vertex3D(headVerts[5] * scaleDown, Vector2(headUVs[2].mins.x, headUVs[2].mins.y)),
	Vertex3D(headVerts[7] * scaleDown, Vector2(headUVs[2].mins.x, headUVs[2].maxs.y)),
	Vertex3D(headVerts[1] * scaleDown, Vector2(headUVs[2].maxs.x, headUVs[2].maxs.y)),
	// Head West
	Vertex3D(headVerts[4] * scaleDown, Vector2(headUVs[4].maxs.x, headUVs[4].mins.y)),
	Vertex3D(headVerts[2] * scaleDown, Vector2(headUVs[4].mins.x, headUVs[4].mins.y)),
	Vertex3D(headVerts[0] * scaleDown, Vector2(headUVs[4].mins.x, headUVs[4].maxs.y)),
	Vertex3D(headVerts[6] * scaleDown, Vector2(headUVs[4].maxs.x, headUVs[4].maxs.y)),

	// Body
	//Body North
	Vertex3D(torsoVerts[2] * scaleDown, Vector2(torsoUVs[3].maxs.x, torsoUVs[3].mins.y)),
	Vertex3D(torsoVerts[3] * scaleDown, Vector2(torsoUVs[3].mins.x, torsoUVs[3].mins.y)),
	Vertex3D(torsoVerts[1] * scaleDown, Vector2(torsoUVs[3].mins.x, torsoUVs[3].maxs.y)),
	Vertex3D(torsoVerts[0] * scaleDown, Vector2(torsoUVs[3].maxs.x, torsoUVs[3].maxs.y)),
	// Body Top
	Vertex3D(torsoVerts[4] * scaleDown, Vector2(torsoUVs[0].maxs.x, torsoUVs[0].mins.y)),
	Vertex3D(torsoVerts[5] * scaleDown, Vector2(torsoUVs[0].mins.x, torsoUVs[0].mins.y)),
	Vertex3D(torsoVerts[3] * scaleDown, Vector2(torsoUVs[0].mins.x, torsoUVs[0].maxs.y)),
	Vertex3D(torsoVerts[2] * scaleDown, Vector2(torsoUVs[0].maxs.x, torsoUVs[0].maxs.y)),
	// Body South
	Vertex3D(torsoVerts[6] * scaleDown, Vector2(torsoUVs[5].mins.x, torsoUVs[5].maxs.y)),
	Vertex3D(torsoVerts[7] * scaleDown, Vector2(torsoUVs[5].maxs.x, torsoUVs[5].maxs.y)),
	Vertex3D(torsoVerts[5] * scaleDown, Vector2(torsoUVs[5].maxs.x, torsoUVs[5].mins.y)),
	Vertex3D(torsoVerts[4] * scaleDown, Vector2(torsoUVs[5].mins.x, torsoUVs[5].mins.y)),
	// Body Bottom
	Vertex3D(torsoVerts[0] * scaleDown, Vector2(torsoUVs[1].maxs.x, torsoUVs[1].mins.y)),
	Vertex3D(torsoVerts[1] * scaleDown, Vector2(torsoUVs[1].mins.x, torsoUVs[1].mins.y)),
	Vertex3D(torsoVerts[7] * scaleDown, Vector2(torsoUVs[1].mins.x, torsoUVs[1].maxs.y)),
	Vertex3D(torsoVerts[6] * scaleDown, Vector2(torsoUVs[1].maxs.x, torsoUVs[1].maxs.y)),
	// Body East
	Vertex3D(torsoVerts[3] * scaleDown, Vector2(torsoUVs[2].maxs.x, torsoUVs[2].mins.y)),
	Vertex3D(torsoVerts[5] * scaleDown, Vector2(torsoUVs[2].mins.x, torsoUVs[2].mins.y)),
	Vertex3D(torsoVerts[7] * scaleDown, Vector2(torsoUVs[2].mins.x, torsoUVs[2].maxs.y)),
	Vertex3D(torsoVerts[1] * scaleDown, Vector2(torsoUVs[2].maxs.x, torsoUVs[2].maxs.y)),
	// Body West
	Vertex3D(torsoVerts[4] * scaleDown, Vector2(torsoUVs[4].maxs.x, torsoUVs[4].mins.y)),
	Vertex3D(torsoVerts[2] * scaleDown, Vector2(torsoUVs[4].mins.x, torsoUVs[4].mins.y)),
	Vertex3D(torsoVerts[0] * scaleDown, Vector2(torsoUVs[4].mins.x, torsoUVs[4].maxs.y)),
	Vertex3D(torsoVerts[6] * scaleDown, Vector2(torsoUVs[4].maxs.x, torsoUVs[4].maxs.y)),

	// r_Leg
	//North
	Vertex3D(r_LegVerts[2] * scaleDown, Vector2(r_LegUVs[3].maxs.x, r_LegUVs[3].mins.y)),
	Vertex3D(r_LegVerts[3] * scaleDown, Vector2(r_LegUVs[3].mins.x, r_LegUVs[3].mins.y)),
	Vertex3D(r_LegVerts[1] * scaleDown, Vector2(r_LegUVs[3].mins.x, r_LegUVs[3].maxs.y)),
	Vertex3D(r_LegVerts[0] * scaleDown, Vector2(r_LegUVs[3].maxs.x, r_LegUVs[3].maxs.y)),
	//Top
	Vertex3D(r_LegVerts[4] * scaleDown, Vector2(r_LegUVs[0].maxs.x, r_LegUVs[0].mins.y)),
	Vertex3D(r_LegVerts[5] * scaleDown, Vector2(r_LegUVs[0].mins.x, r_LegUVs[0].mins.y)),
	Vertex3D(r_LegVerts[3] * scaleDown, Vector2(r_LegUVs[0].mins.x, r_LegUVs[0].maxs.y)),
	Vertex3D(r_LegVerts[2] * scaleDown, Vector2(r_LegUVs[0].maxs.x, r_LegUVs[0].maxs.y)),
	//South
	Vertex3D(r_LegVerts[6] * scaleDown, Vector2(r_LegUVs[5].mins.x, r_LegUVs[5].maxs.y)),
	Vertex3D(r_LegVerts[7] * scaleDown, Vector2(r_LegUVs[5].maxs.x, r_LegUVs[5].maxs.y)),
	Vertex3D(r_LegVerts[5] * scaleDown, Vector2(r_LegUVs[5].maxs.x, r_LegUVs[5].mins.y)),
	Vertex3D(r_LegVerts[4] * scaleDown, Vector2(r_LegUVs[5].mins.x, r_LegUVs[5].mins.y)),
	//Bottom
	Vertex3D(r_LegVerts[0] * scaleDown, Vector2(r_LegUVs[1].maxs.x, r_LegUVs[1].mins.y)),
	Vertex3D(r_LegVerts[1] * scaleDown, Vector2(r_LegUVs[1].mins.x, r_LegUVs[1].mins.y)),
	Vertex3D(r_LegVerts[7] * scaleDown, Vector2(r_LegUVs[1].mins.x, r_LegUVs[1].maxs.y)),
	Vertex3D(r_LegVerts[6] * scaleDown, Vector2(r_LegUVs[1].maxs.x, r_LegUVs[1].maxs.y)),
	//East
	Vertex3D(r_LegVerts[3] * scaleDown, Vector2(r_LegUVs[2].maxs.x, r_LegUVs[2].mins.y)),
	Vertex3D(r_LegVerts[5] * scaleDown, Vector2(r_LegUVs[2].mins.x, r_LegUVs[2].mins.y)),
	Vertex3D(r_LegVerts[7] * scaleDown, Vector2(r_LegUVs[2].mins.x, r_LegUVs[2].maxs.y)),
	Vertex3D(r_LegVerts[1] * scaleDown, Vector2(r_LegUVs[2].maxs.x, r_LegUVs[2].maxs.y)),
	//West
	Vertex3D(r_LegVerts[4] * scaleDown, Vector2(r_LegUVs[4].maxs.x, r_LegUVs[4].mins.y)),
	Vertex3D(r_LegVerts[2] * scaleDown, Vector2(r_LegUVs[4].mins.x, r_LegUVs[4].mins.y)),
	Vertex3D(r_LegVerts[0] * scaleDown, Vector2(r_LegUVs[4].mins.x, r_LegUVs[4].maxs.y)),
	Vertex3D(r_LegVerts[6] * scaleDown, Vector2(r_LegUVs[4].maxs.x, r_LegUVs[4].maxs.y)),

	// l_Leg
	//North
	Vertex3D(l_LegVerts[2] * scaleDown, Vector2(l_LegUVs[3].maxs.x, l_LegUVs[3].mins.y)),
	Vertex3D(l_LegVerts[3] * scaleDown, Vector2(l_LegUVs[3].mins.x, l_LegUVs[3].mins.y)),
	Vertex3D(l_LegVerts[1] * scaleDown, Vector2(l_LegUVs[3].mins.x, l_LegUVs[3].maxs.y)),
	Vertex3D(l_LegVerts[0] * scaleDown, Vector2(l_LegUVs[3].maxs.x, l_LegUVs[3].maxs.y)),
	//Top
	Vertex3D(l_LegVerts[4] * scaleDown, Vector2(l_LegUVs[0].maxs.x, l_LegUVs[0].mins.y)),
	Vertex3D(l_LegVerts[5] * scaleDown, Vector2(l_LegUVs[0].mins.x, l_LegUVs[0].mins.y)),
	Vertex3D(l_LegVerts[3] * scaleDown, Vector2(l_LegUVs[0].mins.x, l_LegUVs[0].maxs.y)),
	Vertex3D(l_LegVerts[2] * scaleDown, Vector2(l_LegUVs[0].maxs.x, l_LegUVs[0].maxs.y)),
	//South
	Vertex3D(l_LegVerts[6] * scaleDown, Vector2(l_LegUVs[5].mins.x, l_LegUVs[5].maxs.y)),
	Vertex3D(l_LegVerts[7] * scaleDown, Vector2(l_LegUVs[5].maxs.x, l_LegUVs[5].maxs.y)),
	Vertex3D(l_LegVerts[5] * scaleDown, Vector2(l_LegUVs[5].maxs.x, l_LegUVs[5].mins.y)),
	Vertex3D(l_LegVerts[4] * scaleDown, Vector2(l_LegUVs[5].mins.x, l_LegUVs[5].mins.y)),
	//Bottom
	Vertex3D(l_LegVerts[0] * scaleDown, Vector2(l_LegUVs[1].maxs.x, l_LegUVs[1].mins.y)),
	Vertex3D(l_LegVerts[1] * scaleDown, Vector2(l_LegUVs[1].mins.x, l_LegUVs[1].mins.y)),
	Vertex3D(l_LegVerts[7] * scaleDown, Vector2(l_LegUVs[1].mins.x, l_LegUVs[1].maxs.y)),
	Vertex3D(l_LegVerts[6] * scaleDown, Vector2(l_LegUVs[1].maxs.x, l_LegUVs[1].maxs.y)),
	//East
	Vertex3D(l_LegVerts[3] * scaleDown, Vector2(l_LegUVs[2].maxs.x, l_LegUVs[2].mins.y)),
	Vertex3D(l_LegVerts[5] * scaleDown, Vector2(l_LegUVs[2].mins.x, l_LegUVs[2].mins.y)),
	Vertex3D(l_LegVerts[7] * scaleDown, Vector2(l_LegUVs[2].mins.x, l_LegUVs[2].maxs.y)),
	Vertex3D(l_LegVerts[1] * scaleDown, Vector2(l_LegUVs[2].maxs.x, l_LegUVs[2].maxs.y)),
	//West
	Vertex3D(l_LegVerts[4] * scaleDown, Vector2(l_LegUVs[4].maxs.x, l_LegUVs[4].mins.y)),
	Vertex3D(l_LegVerts[2] * scaleDown, Vector2(l_LegUVs[4].mins.x, l_LegUVs[4].mins.y)),
	Vertex3D(l_LegVerts[0] * scaleDown, Vector2(l_LegUVs[4].mins.x, l_LegUVs[4].maxs.y)),
	Vertex3D(l_LegVerts[6] * scaleDown, Vector2(l_LegUVs[4].maxs.x, l_LegUVs[4].maxs.y)),

	// r_Arm
	//North
	Vertex3D(r_ArmVerts[2] * scaleDown, Vector2(r_ArmUVs[3].maxs.x, r_ArmUVs[3].mins.y)),
	Vertex3D(r_ArmVerts[3] * scaleDown, Vector2(r_ArmUVs[3].mins.x, r_ArmUVs[3].mins.y)),
	Vertex3D(r_ArmVerts[1] * scaleDown, Vector2(r_ArmUVs[3].mins.x, r_ArmUVs[3].maxs.y)),
	Vertex3D(r_ArmVerts[0] * scaleDown, Vector2(r_ArmUVs[3].maxs.x, r_ArmUVs[3].maxs.y)),
	//Top
	Vertex3D(r_ArmVerts[4] * scaleDown, Vector2(r_ArmUVs[0].maxs.x, r_ArmUVs[0].mins.y)),
	Vertex3D(r_ArmVerts[5] * scaleDown, Vector2(r_ArmUVs[0].mins.x, r_ArmUVs[0].mins.y)),
	Vertex3D(r_ArmVerts[3] * scaleDown, Vector2(r_ArmUVs[0].mins.x, r_ArmUVs[0].maxs.y)),
	Vertex3D(r_ArmVerts[2] * scaleDown, Vector2(r_ArmUVs[0].maxs.x, r_ArmUVs[0].maxs.y)),
	//South
	Vertex3D(r_ArmVerts[6] * scaleDown, Vector2(r_ArmUVs[5].mins.x, r_ArmUVs[5].maxs.y)),
	Vertex3D(r_ArmVerts[7] * scaleDown, Vector2(r_ArmUVs[5].maxs.x, r_ArmUVs[5].maxs.y)),
	Vertex3D(r_ArmVerts[5] * scaleDown, Vector2(r_ArmUVs[5].maxs.x, r_ArmUVs[5].mins.y)),
	Vertex3D(r_ArmVerts[4] * scaleDown, Vector2(r_ArmUVs[5].mins.x, r_ArmUVs[5].mins.y)),
	//Bottom
	Vertex3D(r_ArmVerts[0] * scaleDown, Vector2(r_ArmUVs[1].maxs.x, r_ArmUVs[1].mins.y)),
	Vertex3D(r_ArmVerts[1] * scaleDown, Vector2(r_ArmUVs[1].mins.x, r_ArmUVs[1].mins.y)),
	Vertex3D(r_ArmVerts[7] * scaleDown, Vector2(r_ArmUVs[1].mins.x, r_ArmUVs[1].maxs.y)),
	Vertex3D(r_ArmVerts[6] * scaleDown, Vector2(r_ArmUVs[1].maxs.x, r_ArmUVs[1].maxs.y)),
	//East
	Vertex3D(r_ArmVerts[3] * scaleDown, Vector2(r_ArmUVs[2].maxs.x, r_ArmUVs[2].mins.y)),
	Vertex3D(r_ArmVerts[5] * scaleDown, Vector2(r_ArmUVs[2].mins.x, r_ArmUVs[2].mins.y)),
	Vertex3D(r_ArmVerts[7] * scaleDown, Vector2(r_ArmUVs[2].mins.x, r_ArmUVs[2].maxs.y)),
	Vertex3D(r_ArmVerts[1] * scaleDown, Vector2(r_ArmUVs[2].maxs.x, r_ArmUVs[2].maxs.y)),
	//West
	Vertex3D(r_ArmVerts[4] * scaleDown, Vector2(r_ArmUVs[4].maxs.x, r_ArmUVs[4].mins.y)),
	Vertex3D(r_ArmVerts[2] * scaleDown, Vector2(r_ArmUVs[4].mins.x, r_ArmUVs[4].mins.y)),
	Vertex3D(r_ArmVerts[0] * scaleDown, Vector2(r_ArmUVs[4].mins.x, r_ArmUVs[4].maxs.y)),
	Vertex3D(r_ArmVerts[6] * scaleDown, Vector2(r_ArmUVs[4].maxs.x, r_ArmUVs[4].maxs.y)),

	// l_Arm
	//North
	Vertex3D(l_ArmVerts[2] * scaleDown, Vector2(l_ArmUVs[3].maxs.x, l_ArmUVs[3].mins.y)),
	Vertex3D(l_ArmVerts[3] * scaleDown, Vector2(l_ArmUVs[3].mins.x, l_ArmUVs[3].mins.y)),
	Vertex3D(l_ArmVerts[1] * scaleDown, Vector2(l_ArmUVs[3].mins.x, l_ArmUVs[3].maxs.y)),
	Vertex3D(l_ArmVerts[0] * scaleDown, Vector2(l_ArmUVs[3].maxs.x, l_ArmUVs[3].maxs.y)),
	//Top
	Vertex3D(l_ArmVerts[4] * scaleDown, Vector2(l_ArmUVs[0].maxs.x, l_ArmUVs[0].mins.y)),
	Vertex3D(l_ArmVerts[5] * scaleDown, Vector2(l_ArmUVs[0].mins.x, l_ArmUVs[0].mins.y)),
	Vertex3D(l_ArmVerts[3] * scaleDown, Vector2(l_ArmUVs[0].mins.x, l_ArmUVs[0].maxs.y)),
	Vertex3D(l_ArmVerts[2] * scaleDown, Vector2(l_ArmUVs[0].maxs.x, l_ArmUVs[0].maxs.y)),
	//South
	Vertex3D(l_ArmVerts[6] * scaleDown, Vector2(l_ArmUVs[5].mins.x, l_ArmUVs[5].maxs.y)),
	Vertex3D(l_ArmVerts[7] * scaleDown, Vector2(l_ArmUVs[5].maxs.x, l_ArmUVs[5].maxs.y)),
	Vertex3D(l_ArmVerts[5] * scaleDown, Vector2(l_ArmUVs[5].maxs.x, l_ArmUVs[5].mins.y)),
	Vertex3D(l_ArmVerts[4] * scaleDown, Vector2(l_ArmUVs[5].mins.x, l_ArmUVs[5].mins.y)),
	//Bottom
	Vertex3D(l_ArmVerts[0] * scaleDown, Vector2(l_ArmUVs[1].maxs.x, l_ArmUVs[1].mins.y)),
	Vertex3D(l_ArmVerts[1] * scaleDown, Vector2(l_ArmUVs[1].mins.x, l_ArmUVs[1].mins.y)),
	Vertex3D(l_ArmVerts[7] * scaleDown, Vector2(l_ArmUVs[1].mins.x, l_ArmUVs[1].maxs.y)),
	Vertex3D(l_ArmVerts[6] * scaleDown, Vector2(l_ArmUVs[1].maxs.x, l_ArmUVs[1].maxs.y)),
	//East
	Vertex3D(l_ArmVerts[3] * scaleDown, Vector2(l_ArmUVs[2].maxs.x, l_ArmUVs[2].mins.y)),
	Vertex3D(l_ArmVerts[5] * scaleDown, Vector2(l_ArmUVs[2].mins.x, l_ArmUVs[2].mins.y)),
	Vertex3D(l_ArmVerts[7] * scaleDown, Vector2(l_ArmUVs[2].mins.x, l_ArmUVs[2].maxs.y)),
	Vertex3D(l_ArmVerts[1] * scaleDown, Vector2(l_ArmUVs[2].maxs.x, l_ArmUVs[2].maxs.y)),
	//West
	Vertex3D(l_ArmVerts[4] * scaleDown, Vector2(l_ArmUVs[4].maxs.x, l_ArmUVs[4].mins.y)),
	Vertex3D(l_ArmVerts[2] * scaleDown, Vector2(l_ArmUVs[4].mins.x, l_ArmUVs[4].mins.y)),
	Vertex3D(l_ArmVerts[0] * scaleDown, Vector2(l_ArmUVs[4].mins.x, l_ArmUVs[4].maxs.y)),
	Vertex3D(l_ArmVerts[6] * scaleDown, Vector2(l_ArmUVs[4].maxs.x, l_ArmUVs[4].maxs.y)),
};
//////////////////////////////////////////////////////////////////////////
// Chicken
Vector3 c_head[8] = {
	Vector3(-2.0000f, -6.0000f, 10.0000f),
	Vector3(2.0000f, -6.0000f, 10.0000f),
	Vector3(-2.0000f, -6.0000f, 16.0000f),
	Vector3(2.0000f, -6.0000f, 16.0000f),
	Vector3(-2.0000f, -3.0000f, 16.0000f),
	Vector3(2.0000f, -3.0000f, 16.0000f),
	Vector3(-2.0000f, -3.0000f, 10.0000f),
	Vector3(2.0000f, -3.0000f, 10.0000f),
};

Vector2 c_headUVs[24] = {
	Vector2(0.0467f, 0.7189f),
	Vector2(0.1092f, 0.7189f),
	Vector2(0.1092f, 0.9068f),
	Vector2(0.0467f, 0.9068f),
	Vector2(0.0466f, 0.9068f),
	Vector2(0.1092f, 0.9068f),
	Vector2(0.1096f, 0.9992f),
	Vector2(0.0470f, 0.9992f),
	Vector2(0.2188f, 0.9068f),
	Vector2(0.1563f, 0.9068f),
	Vector2(0.1563f, 0.7189f),
	Vector2(0.2188f, 0.7189f),
	Vector2(0.1096f, 0.9068f),
	Vector2(0.1722f, 0.9068f),
	Vector2(0.1718f, 0.9992f),
	Vector2(0.1091f, 0.9992f),
	Vector2(0.1093f, 0.7186f),
	Vector2(0.1562f, 0.7186f),
	Vector2(0.1562f, 0.9065f),
	Vector2(0.1093f, 0.9065f),
	Vector2(0.0000f, 0.7189f),
	Vector2(0.0468f, 0.7189f),
	Vector2(0.0468f, 0.9068f),
	Vector2(0.0000f, 0.9068f),
};

Vector3 c_beak[8] = {
	Vector3(-2.0000f, -8.0000f, 12.0000f),
	Vector3(2.0000f, -8.0000f, 12.0000f),
	Vector3(-2.0000f, -8.0000f, 14.0000f),
	Vector3(2.0000f, -8.0000f, 14.0000f),
	Vector3(-2.0000f, -6.0000f, 14.0000f),
	Vector3(2.0000f, -6.0000f, 14.0000f),
	Vector3(-2.0000f, -6.0000f, 12.0000f),
	Vector3(2.0000f, -6.0000f, 12.0000f),
};

Vector2 c_beakUVs[20] = {
	Vector2(0.2499f, 0.8746f),
	Vector2(0.3125f, 0.8747f),
	Vector2(0.3125f, 0.9373f),
	Vector2(0.2499f, 0.9373f),
	Vector2(0.3126f, 1.0000f),
	Vector2(0.2500f, 1.0000f),
	Vector2(0.4062f, 0.9374f),
	Vector2(0.3437f, 0.9374f),
	Vector2(0.3437f, 0.8747f),
	Vector2(0.4062f, 0.8747f),
	Vector2(0.3128f, 0.9377f),
	Vector2(0.3754f, 0.9377f),
	Vector2(0.3754f, 1.0000f),
	Vector2(0.3128f, 1.0000f),
	Vector2(0.3437f, 0.8748f),
	Vector2(0.3437f, 0.9373f),
	Vector2(0.2188f, 0.8746f),
	Vector2(0.2500f, 0.8746f),
	Vector2(0.2500f, 0.9371f),
	Vector2(0.2188f, 0.9371f),
};

Vector3 c_waddle[8] = {
	Vector3(-1.0000f, -7.0000f, 10.0000f),
	Vector3(1.0000f, -7.0000f, 10.0000f),
	Vector3(-1.0000f, -7.0000f, 12.0000f),
	Vector3(1.0000f, -7.0000f, 12.0000f),
	Vector3(-1.0000f, -6.0000f, 12.0000f),
	Vector3(1.0000f, -6.0000f, 12.0000f),
	Vector3(-1.0000f, -6.0000f, 10.0000f),
	Vector3(1.0000f, -6.0000f, 10.0000f),
};

Vector2 c_waddleUVs[22] = {
	Vector2(0.2502f, 0.7501f),
	Vector2(0.2811f, 0.7500f),
	Vector2(0.2811f, 0.8124f),
	Vector2(0.2502f, 0.8124f),
	Vector2(0.2502f, 0.8128f),
	Vector2(0.2810f, 0.8128f),
	Vector2(0.2810f, 0.8751f),
	Vector2(0.2502f, 0.8751f),
	Vector2(0.3438f, 0.8122f),
	Vector2(0.3126f, 0.8122f),
	Vector2(0.3126f, 0.7498f),
	Vector2(0.3438f, 0.7498f),
	Vector2(0.2811f, 0.8127f),
	Vector2(0.3125f, 0.8127f),
	Vector2(0.3125f, 0.8749f),
	Vector2(0.2811f, 0.8749f),
	Vector2(0.3124f, 0.7499f),
	Vector2(0.3124f, 0.8123f),
	Vector2(0.2188f, 0.7502f),
	Vector2(0.2502f, 0.7502f),
	Vector2(0.2502f, 0.8126f),
	Vector2(0.2188f, 0.8126f),
};

Vector3 c_body[8] = {
	Vector3(-3.0000f, -4.0000f,  6.0000f),
	Vector3(3.0000f, -4.0000f,  6.0000f),
	Vector3(-3.0000f, -4.0000f, 12.0000f),
	Vector3(3.0000f, -4.0000f, 12.0000f),
	Vector3(-3.0000f,  4.0000f, 12.0000f),
	Vector3(3.0000f,  4.0000f, 12.0000f),
	Vector3(-3.0000f,  4.0000f,  6.0000f),
	Vector3(3.0000f,  4.0000f,  6.0000f),
};

Vector2 c_bodyUVs[24] = {
	Vector2(0.0935f, 0.5300f),
	Vector2(0.1875f, 0.5300f),
	Vector2(0.1875f, 0.7181f),
	Vector2(0.0935f, 0.7181f),
	Vector2(0.2816f, 0.2822f),
	Vector2(0.3758f, 0.2822f),
	Vector2(0.3758f, 0.5300f),
	Vector2(0.2816f, 0.5300f),
	Vector2(0.2816f, 0.7181f),
	Vector2(0.1875f, 0.7181f),
	Vector2(0.1875f, 0.5300f),
	Vector2(0.2816f, 0.5300f),
	Vector2(0.0935f, 0.2822f),
	Vector2(0.1875f, 0.2822f),
	Vector2(0.1875f, 0.5300f),
	Vector2(0.0935f, 0.5300f),
	Vector2(0.1875f, 0.2822f),
	Vector2(0.2816f, 0.2822f),
	Vector2(0.2816f, 0.5300f),
	Vector2(0.1875f, 0.5300f),
	Vector2(0.0000f, 0.2822f),
	Vector2(0.0935f, 0.2822f),
	Vector2(0.0935f, 0.5300f),
	Vector2(0.0000f, 0.5300f),
};
//////////////////////////////////////////////////////////////////////////
Vector3 c_l_wing[8] = {
	Vector3(3.0000f, -3.0000f,  8.0000f),
	Vector3(4.0000f, -3.0000f,  8.0000f),
	Vector3(3.0000f, -3.0000f, 12.0000f),
	Vector3(4.0000f, -3.0000f, 12.0000f),
	Vector3(3.0000f,  3.0000f, 12.0000f),
	Vector3(4.0000f,  3.0000f, 12.0000f),
	Vector3(3.0000f,  3.0000f,  8.0000f),
	Vector3(4.0000f,  3.0000f,  8.0000f),
};

Vector3 c_r_wing[8] = {
	Vector3(-4.f, -3.0000f,  8.0000f),
	Vector3(-3.f, -3.0000f,  8.0000f),
	Vector3(-4.f, -3.0000f, 12.0000f),
	Vector3(-3.f, -3.0000f, 12.0000f),
	Vector3(-4.f,  3.0000f, 12.0000f),
	Vector3(-3.f,  3.0000f, 12.0000f),
	Vector3(-4.f,  3.0000f,  8.0000f),
	Vector3(-3.f,  3.0000f,  8.0000f),
};

Vector2 c_wingUVs[24] = {
	Vector2(0.4686f, 0.2815f),
	Vector2(0.4844f, 0.2815f),
	Vector2(0.4844f, 0.4063f),
	Vector2(0.4686f, 0.4063f),
	Vector2(0.4686f, 0.4063f),
	Vector2(0.4844f, 0.4063f),
	Vector2(0.4844f, 0.5938f),
	Vector2(0.4686f, 0.5938f),
	Vector2(0.5938f, 0.4063f),
	Vector2(0.5781f, 0.4063f),
	Vector2(0.5781f, 0.2815f),
	Vector2(0.5938f, 0.2815f),
	Vector2(0.4844f, 0.4063f),
	Vector2(0.5000f, 0.4063f),
	Vector2(0.5000f, 0.5938f),
	Vector2(0.4844f, 0.5938f),
	Vector2(0.4844f, 0.2815f),
	Vector2(0.5781f, 0.2815f),
	Vector2(0.5781f, 0.4063f),
	Vector2(0.4844f, 0.4063f),
	Vector2(0.3758f, 0.2815f),
	Vector2(0.4684f, 0.2815f),
	Vector2(0.4684f, 0.4063f),
	Vector2(0.3758f, 0.4063f),
};
//////////////////////////////////////////////////////////////////////////
Vector3 c_l_leg[8] = {
	Vector3(1.0000f, 1.0000f, 0.0000f),
	Vector3(2.0000f, 1.0000f, 0.0000f),
	Vector3(1.0000f, 1.0000f, 6.0000f),
	Vector3(2.0000f, 1.0000f, 6.0000f),
	Vector3(1.0000f, 2.0000f, 6.0000f),
	Vector3(2.0000f, 2.0000f, 6.0000f),
	Vector3(1.0000f, 2.0000f, 0.0000f),
	Vector3(2.0000f, 2.0000f, 0.0000f),
};

Vector3 c_r_leg[8] = {
	Vector3(-2.0000f, 1.0000f, 0.0000f),
	Vector3(-1.0000f, 1.0000f, 0.0000f),
	Vector3(-2.0000f, 1.0000f, 6.0000f),
	Vector3(-1.0000f, 1.0000f, 6.0000f),
	Vector3(-2.0000f, 2.0000f, 6.0000f),
	Vector3(-1.0000f, 2.0000f, 6.0000f),
	Vector3(-2.0000f, 2.0000f, 0.0000f),
	Vector3(-1.0000f, 2.0000f, 0.0000f),
};

Vector2 c_legUVs[24] = {
	Vector2(0.4532f, 0.7500f),
	Vector2(0.4999f, 0.7500f),
	Vector2(0.4999f, 0.9063f),
	Vector2(0.4532f, 0.9063f),
	Vector2(0.4532f, 0.9063f),
	Vector2(0.4999f, 0.9063f),
	Vector2(0.4999f, 1.0000f),
	Vector2(0.4532f, 1.0000f),
	Vector2(0.5468f, 0.9063f),
	Vector2(0.5939f, 0.9063f),
	Vector2(0.5939f, 0.7500f),
	Vector2(0.5468f, 0.7500f),
	Vector2(0.4999f, 1.0000f),
	Vector2(0.5468f, 1.0000f),
	Vector2(0.5468f, 0.9063f),
	Vector2(0.4999f, 0.9063f),
	Vector2(0.4999f, 0.7500f),
	Vector2(0.5468f, 0.7500f),
	Vector2(0.5468f, 0.9063f),
	Vector2(0.4999f, 0.9063f),
	Vector2(0.4532f, 0.7500f),
	Vector2(0.4061f, 0.7500f),
	Vector2(0.4061f, 0.9063f),
	Vector2(0.4532f, 0.9063f),
};
//////////////////////////////////////////////////////////////////////////
Vector3 c_r_foot[8] = {
	Vector3(-3.0000f, -2.0000f, -0.0000f),
	Vector3(0.0000f, -2.0000f, -0.0000f),
	Vector3(-3.0000f, -2.0000f,  1.0000f),
	Vector3(0.0000f, -2.0000f,  1.0000f),
	Vector3(-3.0000f,  1.0000f,  1.0000f),
	Vector3(0.0000f,  1.0000f,  1.0000f),
	Vector3(-3.0000f,  1.0000f,  0.0000f),
	Vector3(0.0000f,  1.0000f,  0.0000f),
};

Vector3 c_l_foot[8] = {
	Vector3(0.0000f, -2.0000f, 0.0000f),
	Vector3(3.0000f, -2.0000f, 0.0000f),
	Vector3(0.0000f, -2.0000f, 1.0000f),
	Vector3(3.0000f, -2.0000f, 1.0000f),
	Vector3(0.0000f,  1.0000f, 1.0000f),
	Vector3(3.0000f,  1.0000f, 1.0000f),
	Vector3(0.0000f,  1.0000f, 0.0000f),
	Vector3(3.0000f,  1.0000f, 0.0000f),
};
//////////////////////////////////////////////////////////////////////////
Vertex3D chickenModel[] = { //72 for whole head. 
	Vertex3D(c_head[0], c_headUVs[0]),	Vertex3D(c_head[1],	c_headUVs[1]),	Vertex3D(c_head[3],	c_headUVs[2]),	Vertex3D(c_head[2],	c_headUVs[3]),
	Vertex3D(c_head[2], c_headUVs[4]),	Vertex3D(c_head[3],	c_headUVs[5]),	Vertex3D(c_head[5],	c_headUVs[6]),	Vertex3D(c_head[4],	c_headUVs[7]),
	Vertex3D(c_head[4], c_headUVs[8]),	Vertex3D(c_head[5],	c_headUVs[9]),	Vertex3D(c_head[7],	c_headUVs[10]),	Vertex3D(c_head[6],	c_headUVs[11]),
	Vertex3D(c_head[6], c_headUVs[12]), Vertex3D(c_head[7], c_headUVs[13]), Vertex3D(c_head[1], c_headUVs[14]), Vertex3D(c_head[0], c_headUVs[15]),
	Vertex3D(c_head[1], c_headUVs[16]), Vertex3D(c_head[7], c_headUVs[17]), Vertex3D(c_head[5], c_headUVs[18]), Vertex3D(c_head[3], c_headUVs[19]),
	Vertex3D(c_head[6], c_headUVs[20]), Vertex3D(c_head[0], c_headUVs[21]), Vertex3D(c_head[2], c_headUVs[22]), Vertex3D(c_head[4], c_headUVs[23]),

	Vertex3D(c_beak[0], c_beakUVs[0]),	Vertex3D(c_beak[1],	c_beakUVs[1]),	Vertex3D(c_beak[3],	c_beakUVs[2]),	Vertex3D(c_beak[2],	c_beakUVs[3]),
	Vertex3D(c_beak[2], c_beakUVs[3]),	Vertex3D(c_beak[3],	c_beakUVs[2]),	Vertex3D(c_beak[5],	c_beakUVs[4]),	Vertex3D(c_beak[4],	c_beakUVs[5]),
	Vertex3D(c_beak[4], c_beakUVs[6]),	Vertex3D(c_beak[5],	c_beakUVs[7]),	Vertex3D(c_beak[7],	c_beakUVs[8]),	Vertex3D(c_beak[6],	c_beakUVs[9]),
	Vertex3D(c_beak[6], c_beakUVs[10]), Vertex3D(c_beak[7], c_beakUVs[11]), Vertex3D(c_beak[1], c_beakUVs[12]), Vertex3D(c_beak[0], c_beakUVs[13]),
	Vertex3D(c_beak[1], c_beakUVs[1]), Vertex3D(c_beak[7], c_beakUVs[14]), Vertex3D(c_beak[5], c_beakUVs[15]), Vertex3D(c_beak[3], c_beakUVs[2]),
	Vertex3D(c_beak[6], c_beakUVs[16]), Vertex3D(c_beak[0], c_beakUVs[17]), Vertex3D(c_beak[2], c_beakUVs[18]), Vertex3D(c_beak[4], c_beakUVs[19]),

	Vertex3D(c_waddle[0], c_waddleUVs[0]),	Vertex3D(c_waddle[1],	c_waddleUVs[1]),	Vertex3D(c_waddle[3],	c_waddleUVs[2]),	Vertex3D(c_waddle[2],	c_headUVs[3]),
	Vertex3D(c_waddle[2], c_waddleUVs[4]),	Vertex3D(c_waddle[3],	c_waddleUVs[5]),	Vertex3D(c_waddle[5],	c_waddleUVs[6]),	Vertex3D(c_waddle[4],	c_headUVs[7]),
	Vertex3D(c_waddle[4], c_waddleUVs[8]),	Vertex3D(c_waddle[5],	c_waddleUVs[9]),	Vertex3D(c_waddle[7],	c_waddleUVs[10]),	Vertex3D(c_waddle[6],	c_headUVs[11]),
	Vertex3D(c_waddle[6], c_waddleUVs[12]), Vertex3D(c_waddle[7],	c_waddleUVs[13]),	Vertex3D(c_waddle[1],	c_waddleUVs[14]),	Vertex3D(c_waddle[0],	c_headUVs[15]),
	Vertex3D(c_waddle[1], c_waddleUVs[1]), Vertex3D(c_waddle[7],	c_waddleUVs[16]),	Vertex3D(c_waddle[5],	c_waddleUVs[17]),	Vertex3D(c_waddle[3],	c_headUVs[2]),
	Vertex3D(c_waddle[6], c_waddleUVs[18]), Vertex3D(c_waddle[0],	c_waddleUVs[19]),	Vertex3D(c_waddle[2],	c_waddleUVs[20]),	Vertex3D(c_waddle[4],	c_headUVs[21]),

	Vertex3D(c_body[0], c_bodyUVs[0]),	Vertex3D(c_body[1],	c_bodyUVs[1]),	Vertex3D(c_body[3],	c_bodyUVs[2]),	Vertex3D(c_body[2],	c_bodyUVs[3]),
	Vertex3D(c_body[2], c_bodyUVs[4]),	Vertex3D(c_body[3],	c_bodyUVs[5]),	Vertex3D(c_body[5],	c_bodyUVs[6]),	Vertex3D(c_body[4],	c_bodyUVs[7]),
	Vertex3D(c_body[4], c_bodyUVs[8]),	Vertex3D(c_body[5],	c_bodyUVs[9]),	Vertex3D(c_body[7],	c_bodyUVs[10]),	Vertex3D(c_body[6],	c_bodyUVs[11]),
	Vertex3D(c_body[6], c_bodyUVs[12]), Vertex3D(c_body[7], c_bodyUVs[13]), Vertex3D(c_body[1], c_bodyUVs[14]), Vertex3D(c_body[0], c_bodyUVs[15]),
	Vertex3D(c_body[1], c_bodyUVs[16]), Vertex3D(c_body[7], c_bodyUVs[17]), Vertex3D(c_body[5], c_bodyUVs[18]), Vertex3D(c_body[3], c_bodyUVs[19]),
	Vertex3D(c_body[6], c_bodyUVs[20]), Vertex3D(c_body[0], c_bodyUVs[21]), Vertex3D(c_body[2], c_bodyUVs[22]), Vertex3D(c_body[4], c_bodyUVs[23]),

	Vertex3D(c_r_leg[0], c_legUVs[0]),	Vertex3D(c_r_leg[1], c_legUVs[1]),	Vertex3D(c_r_leg[3], c_legUVs[2]),	Vertex3D(c_r_leg[2], c_legUVs[3]),
	Vertex3D(c_r_leg[2], c_legUVs[4]),	Vertex3D(c_r_leg[3], c_legUVs[5]),	Vertex3D(c_r_leg[5], c_legUVs[6]),	Vertex3D(c_r_leg[4], c_legUVs[7]),
	Vertex3D(c_r_leg[4], c_legUVs[8]),	Vertex3D(c_r_leg[5], c_legUVs[9]),	Vertex3D(c_r_leg[7], c_legUVs[10]),	Vertex3D(c_r_leg[6], c_legUVs[11]),
	Vertex3D(c_r_leg[6], c_legUVs[12]), Vertex3D(c_r_leg[7], c_legUVs[13]),	Vertex3D(c_r_leg[1], c_legUVs[14]),	Vertex3D(c_r_leg[0], c_legUVs[15]),
	Vertex3D(c_r_leg[1], c_legUVs[16]), Vertex3D(c_r_leg[7], c_legUVs[17]),	Vertex3D(c_r_leg[5], c_legUVs[18]),	Vertex3D(c_r_leg[3], c_legUVs[19]),
	Vertex3D(c_r_leg[6], c_legUVs[20]), Vertex3D(c_r_leg[0], c_legUVs[21]),	Vertex3D(c_r_leg[2], c_legUVs[22]),	Vertex3D(c_r_leg[4], c_legUVs[23]),

	Vertex3D(c_l_leg[0], c_legUVs[0]),	Vertex3D(c_l_leg[1], c_legUVs[1]),	Vertex3D(c_l_leg[3], c_legUVs[2]),	Vertex3D(c_l_leg[2], c_legUVs[3]),
	Vertex3D(c_l_leg[2], c_legUVs[4]),	Vertex3D(c_l_leg[3], c_legUVs[5]),	Vertex3D(c_l_leg[5], c_legUVs[6]),	Vertex3D(c_l_leg[4], c_legUVs[7]),
	Vertex3D(c_l_leg[4], c_legUVs[8]),	Vertex3D(c_l_leg[5], c_legUVs[9]),	Vertex3D(c_l_leg[7], c_legUVs[10]),	Vertex3D(c_l_leg[6], c_legUVs[11]),
	Vertex3D(c_l_leg[6], c_legUVs[12]), Vertex3D(c_l_leg[7], c_legUVs[13]),	Vertex3D(c_l_leg[1], c_legUVs[14]),	Vertex3D(c_l_leg[0], c_legUVs[15]),
	Vertex3D(c_l_leg[1], c_legUVs[16]), Vertex3D(c_l_leg[7], c_legUVs[17]),	Vertex3D(c_l_leg[5], c_legUVs[18]),	Vertex3D(c_l_leg[3], c_legUVs[19]),
	Vertex3D(c_l_leg[6], c_legUVs[20]), Vertex3D(c_l_leg[0], c_legUVs[21]),	Vertex3D(c_l_leg[2], c_legUVs[22]),	Vertex3D(c_l_leg[4], c_legUVs[23]),


	Vertex3D(c_r_wing[0], c_wingUVs[0]),	Vertex3D(c_r_wing[1], c_wingUVs[1]),	Vertex3D(c_r_wing[3], c_wingUVs[2]),	Vertex3D(c_r_wing[2], c_wingUVs[3]),
	Vertex3D(c_r_wing[2], c_wingUVs[4]),	Vertex3D(c_r_wing[3], c_wingUVs[5]),	Vertex3D(c_r_wing[5], c_wingUVs[6]),	Vertex3D(c_r_wing[4], c_wingUVs[7]),
	Vertex3D(c_r_wing[4], c_wingUVs[8]),	Vertex3D(c_r_wing[5], c_wingUVs[9]),	Vertex3D(c_r_wing[7], c_wingUVs[10]),	Vertex3D(c_r_wing[6], c_wingUVs[11]),
	Vertex3D(c_r_wing[6], c_wingUVs[12]),	Vertex3D(c_r_wing[7], c_wingUVs[13]),	Vertex3D(c_r_wing[1], c_wingUVs[14]),	Vertex3D(c_r_wing[0], c_wingUVs[15]),
	Vertex3D(c_r_wing[1], c_wingUVs[16]),	Vertex3D(c_r_wing[7], c_wingUVs[17]),	Vertex3D(c_r_wing[5], c_wingUVs[18]),	Vertex3D(c_r_wing[3], c_wingUVs[19]),
	Vertex3D(c_r_wing[6], c_wingUVs[20]),	Vertex3D(c_r_wing[0], c_wingUVs[21]),	Vertex3D(c_r_wing[2], c_wingUVs[22]),	Vertex3D(c_r_wing[4], c_wingUVs[23]),

	Vertex3D(c_l_wing[0], c_wingUVs[0]),	Vertex3D(c_l_wing[1], c_wingUVs[1]),	Vertex3D(c_l_wing[3], c_wingUVs[2]),	Vertex3D(c_l_wing[2], c_wingUVs[3]),
	Vertex3D(c_l_wing[2], c_wingUVs[4]),	Vertex3D(c_l_wing[3], c_wingUVs[5]),	Vertex3D(c_l_wing[5], c_wingUVs[6]),	Vertex3D(c_l_wing[4], c_wingUVs[7]),
	Vertex3D(c_l_wing[4], c_wingUVs[8]),	Vertex3D(c_l_wing[5], c_wingUVs[9]),	Vertex3D(c_l_wing[7], c_wingUVs[10]),	Vertex3D(c_l_wing[6], c_wingUVs[11]),
	Vertex3D(c_l_wing[6], c_wingUVs[12]),	Vertex3D(c_l_wing[7], c_wingUVs[13]),	Vertex3D(c_l_wing[1], c_wingUVs[14]),	Vertex3D(c_l_wing[0], c_wingUVs[15]),
	Vertex3D(c_l_wing[1], c_wingUVs[16]),	Vertex3D(c_l_wing[7], c_wingUVs[17]),	Vertex3D(c_l_wing[5], c_wingUVs[18]),	Vertex3D(c_l_wing[3], c_wingUVs[19]),
	Vertex3D(c_l_wing[6], c_wingUVs[20]),	Vertex3D(c_l_wing[0], c_wingUVs[21]),	Vertex3D(c_l_wing[2], c_wingUVs[22]),	Vertex3D(c_l_wing[4], c_wingUVs[23]),
};
/*
float g_uvCoordsForPlayerSkin[] =
{
//  minU     minV     maxU     maxV
0.6250f, 0.0000f, 0.7500f, 0.1250f,  // Helm Top)
0.7500f, 0.0000f, 0.8750f, 0.1250f, // Helm Bottom
0.5000f, 0.1250f, 0.6250f, 0.2500f, // Helm Right
0.6250f, 0.1250f, 0.7500f, 0.2500f, // Helm Front
0.7500f, 0.1250f, 0.8750f, 0.2500f, // Helm Left
0.8750f, 0.1250f, 1.0000f, 0.2500f, // Helm Back





0.0625f, 0.7500f, 0.1250f, 0.5625f, // Right Leg Layer 2 Top
0.1250f, 0.7500f, 0.1875f, 0.5625f, // Right Leg Layer 2 Bottom
0.0000f, 0.5625f, 0.0625f, 0.7500f, // Right Leg Layer 2 Right
0.0625f, 0.5625f, 0.1250f, 0.7500f, // Right Leg Layer 2 Front
0.1250f, 0.5625f, 0.1875f, 0.7500f, // Right Leg Layer 2 Left
0.1875f, 0.5625f, 0.2500f, 0.7500f, // Right Leg Layer 2 Back
0.3125f, 0.7500f, 0.4375f, 0.5625f, // Torso Layer 2 Top
0.4375f, 0.7500f, 0.5625f, 0.5625f, // Torso Layer 2 Bottom
0.2500f, 0.5625f, 0.3125f, 0.7500f, // Torso Layer 2 Right
0.3125f, 0.5625f, 0.4375f, 0.7500f, // Torso Layer 2 Front
0.4375f, 0.5625f, 0.5000f, 0.7500f, // Torso Layer 2 Left
0.5000f, 0.5625f, 0.6250f, 0.7500f, // Torso Layer 2 Back
0.6875f, 0.7500f, 0.7500f, 0.5625f, // Right Arm Layer 2 Top
0.7500f, 0.7500f, 0.8125f, 0.5625f, // Right Arm Layer 2 Bottom
0.6250f, 0.5625f, 0.6875f, 0.7500f, // Right Arm Layer 2 Right
0.6875f, 0.5625f, 0.7500f, 0.7500f, // Right Arm Layer 2 Front
0.7500f, 0.5625f, 0.8125f, 0.7500f, // Right Arm Layer 2 Left
0.8125f, 0.5625f, 1.0000f, 0.7500f, // Right Arm Layer 2 Back
0.0625f, 0.7500f, 0.1250f, 0.8125f, // Left Leg Layer 2 Top
0.1250f, 0.7500f, 0.1875f, 0.8125f, // Left Leg Layer 2 Bottom
0.0000f, 0.8125f, 0.0625f, 1.0000f, // Left Leg Layer 2 Right
0.0625f, 0.8125f, 0.1250f, 1.0000f, // Left Leg Layer 2 Front
0.1250f, 0.8125f, 0.1875f, 1.0000f, // Left Leg Layer 2 Left
0.1875f, 0.8125f, 0.2500f, 1.0000f, // Left Leg Layer 2 Back
0.8125f, 0.7500f, 0.8750f, 0.8125f, // Left Arm Layer 2 Top
0.8750f, 0.7500f, 0.9375f, 0.8125f, // Left Arm Layer 2 Bottom
0.7500f, 0.8125f, 0.8125f, 1.0000f, // Left Arm Layer 2 Right
0.8125f, 0.8125f, 0.8750f, 1.0000f, // Left Arm Layer 2 Front
0.8750f, 0.8125f, 0.9375f, 1.0000f, // Left Arm Layer 2 Left
0.9375f, 0.8125f, 1.0000f, 1.0000f, // Left Arm Layer 2 Back
};
*/