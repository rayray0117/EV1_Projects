#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/Math/Transform.hpp"

#include <vector>
//////////////////////////////////////////////////////////////////////////
class SimpleRenderer;
//////////////////////////////////////////////////////////////////////////
#include "Engine/General/DataDrivenDefinition.hpp"
class ParticleSystemDefinition : public DataDrivenDefinition<ParticleSystemDefinition>
{
	friend DataDrivenDefinition;
public:

public:
	std::vector<std::string> m_emitterDefNames;
	SQT m_transform;
	float m_oneOffTime = -1.f;
protected:
	ParticleSystemDefinition(const std::string& name);
	ParticleSystemDefinition(const XMLNode& element);
};
//////////////////////////////////////////////////////////////////////////
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void Initialize();
	void Initialize(const std::string& defName, SimpleRenderer* renderer);
	
	void Update(float deltaSeconds);
	void Render(SimpleRenderer* renderer) const;

	void Play();
	void Stop();
	void Reset();

	void Destroy(bool immediate = false)
	{
		if (immediate)
			Destroy_Immediate();
		else
			Destroy_WhenFinished();
	}

	void Destroy_WhenFinished();
	void Destroy_Immediate();


	bool isDead() const;
	bool isFinished() const;


	//Adds a ParticleEmitter by creating and initializing one from the definition with that name.
	//Returns the newly created particle emitter so it can be edited further by the caller.
	ParticleEmitter* AddEmitter(const std::string& name, SimpleRenderer* renderer);

	// Adds a ParticleEmitter that was premade by the caller. Does not call an Initialize! You must call an Initialize before adding it to the System!
	void AddEmitter(ParticleEmitter* emitter);
public:
	std::string m_name;
	
	SQT m_transform;

	float m_oneOffTime = -1.f; //Amount of time to play as a one off before destroying. If negative, checks for the longest lifetime of an emitter that is not persistent. If all are persistent then defaults to 1 second.
	bool bIsOneOff = false;
protected:
	void cleanEmitters();
	void setupOneOffTime();

	void Prewarm();

protected:
	std::vector<ParticleEmitter*> m_emitters;
	float m_age = 0.f;
protected:
	friend class ParticleManager;
	bool bPendingDestroy = false;
};