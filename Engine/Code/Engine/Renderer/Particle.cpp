#include "Engine/Renderer/Particle.hpp"

//////////////////////////////////////////////////////////////////////////
void Particle::Update(float deltaSeconds)
{
	m_forces *= deltaSeconds;

	m_velocity += m_forces;

	m_position += m_velocity * deltaSeconds;

	m_age += deltaSeconds;
}