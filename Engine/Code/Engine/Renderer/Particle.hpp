#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/Vector3.hpp"
//////////////////////////////////////////////////////////////////////////
class Particle
{
public:
	Particle() {}
	~Particle() {}
	

	void Update(float deltaSeconds);

	FORCEINLINE double getAge() const
	{
		return m_age;
	}

	FORCEINLINE bool isAlive() const
	{
		return m_age < m_deathAge;
	}
	FORCEINLINE bool isDead() const
	{
		return !isAlive();
	}

	FORCEINLINE float getPercentDead() const
	{
		return (float)(m_age / m_deathAge); //Not safe for m_deathAge being zero. #ToDo: Make math utils functions inline so this can call inverse_d.
	}

public:
	Vector3 m_position = Vector3::ZERO;
	Vector3 m_velocity = Vector3::ZERO;
	Vector3 m_forces = Vector3::ZERO;
	Vector3 m_radial = Vector3::ZERO;
	Vector3 m_tangential = Vector3::ZERO;

	Vector3 m_size = Vector3::ONE;

	float m_age = 0.0;
	float m_deathAge = MAX_FLOAT;
};