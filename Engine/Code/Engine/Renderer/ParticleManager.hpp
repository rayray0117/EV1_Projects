#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Singleton.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class ParticleSystem;
struct SQT;
class SimpleRenderer;
//////////////////////////////////////////////////////////////////////////
class ParticleManager : public Singleton<ParticleManager>
{
public:
	ParticleManager() {}
	~ParticleManager();
	
	void Update(float deltaSeconds);
	
	// An easy way to render all active particles. 
	// It is recommended that you call a ParticleSystem's render directly since the System might be attached to a specific object which may or may not be rendered at the moment.
	void RenderAll(SimpleRenderer* renderer) const;

	void PlayEffect_OneOff(const std::string& name, const SQT& transform, SimpleRenderer* renderer);
	ParticleSystem* PlayEffect(const std::string& name, const SQT& transform, SimpleRenderer* renderer);
public:
	std::vector<ParticleSystem*> m_systems;

private:
	ParticleSystem* addSystem(const std::string& name, SimpleRenderer* renderer);
	void cleanSystems();
};
//////////////////////////////////////////////////////////////////////////
#define PARTICLE_MANAGER ParticleManager::GetInstance()