#pragma once
#include "Engine\Math\Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
class Mesh;
class Rgba;
//////////////////////////////////////////////////////////////////////////
class DebugEndEffector
{
public:
	DebugEndEffector();
	DebugEndEffector(const Rgba& color);
	~DebugEndEffector();

	void Update(float deltaTime);
	void Render() const;
	
public:
	Vector3 m_position = Vector3::ZERO;
	float m_movementSpeed = 50.f;
	Mesh* m_mesh = nullptr;
};