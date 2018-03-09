#include "Engine/Renderer/ParticleManager.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "Engine/Math/Transform.hpp"
//////////////////////////////////////////////////////////////////////////
ParticleManager::~ParticleManager()
{

}
//////////////////////////////////////////////////////////////////////////
void ParticleManager::Update(float deltaSeconds)
{
	for (uint i = 0; i < m_systems.size(); ++i)
	{
		if (nullptr == m_systems[i])
		{
			continue;
		}

		m_systems[i]->Update(deltaSeconds);
	}

	cleanSystems();
}

void ParticleManager::RenderAll(SimpleRenderer* renderer) const
{
	for (uint i = 0; i < m_systems.size(); ++i)
	{
		if (nullptr == m_systems[i])
		{
			continue;
		}

		m_systems[i]->Render(renderer);
	}
}

void ParticleManager::PlayEffect_OneOff(const std::string& name, const SQT& transform, SimpleRenderer* renderer)
{
	ParticleSystem* system = addSystem(name, renderer);
	system->bIsOneOff = true;
	system->m_transform = transform;
	system->Reset();
}

ParticleSystem* ParticleManager::PlayEffect(const std::string& name, const SQT& transform, SimpleRenderer* renderer)
{
	ParticleSystem* system = addSystem(name, renderer);
	system->bIsOneOff = false;
	system->m_transform = transform;
	system->Reset();
	return system;
}
//////////////////////////////////////////////////////////////////////////
ParticleSystem* ParticleManager::addSystem(const std::string& name, SimpleRenderer* renderer)
{
	ParticleSystem* system = new ParticleSystem();
	system->Initialize(name, renderer);
	m_systems.push_back(system);
	return system;
}

void ParticleManager::cleanSystems()
{
	for (uint i = 0; i < m_systems.size(); ++i)
	{
		if (nullptr == m_systems[i])
		{
			continue;
		}

		if (m_systems[i]->isDead())
		{
			m_systems[i]->Destroy_WhenFinished();
		}

		if (m_systems[i]->bPendingDestroy && m_systems[i]->isFinished())
		{
			SAFE_DELETE(m_systems[i]);
		}
	}
}
