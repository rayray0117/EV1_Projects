#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Renderer/Particle.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Core/Rgba.hpp"

#include <vector>
#include <string>
//////////////////////////////////////////////////////////////////////////
class Mesh;
class SimpleRenderer;
class ConstantBuffer;
//////////////////////////////////////////////////////////////////////////
struct particle_buffer
{
	Vector4 start_color;
	Vector4 end_color;
	mutable float particle_fraction;
	Vector3 padding;
};
//////////////////////////////////////////////////////////////////////////
#include "Engine/General/DataDrivenDefinition.hpp"
class ParticleEmitterDefinition : public DataDrivenDefinition<ParticleEmitterDefinition>
{
	friend DataDrivenDefinition;
	friend class ParticleSystemDefinition;
public:
	~ParticleEmitterDefinition();


	//////////////////////////////////////////////////////////////////////////
	// Setters for defining emitter data. 
	// - Use these when available instead of setting the variables directly for safety.
	//////////////////////////////////////////////////////////////////////////
	FORCEINLINE void setLifetime(float lifetime)
	{
		setLifetime(lifetime, lifetime);
	}
	FORCEINLINE void setLifetime(float minLifetime, float maxLifetime)
	{
		m_minLifetime = minLifetime;
		m_maxLifetime = maxLifetime;
	}

	/*/ Old thought code: This would actually be more like a burst
	// How many to spawn at once when emitting particles.
	FORCEINLINE void setSpawnAmount(uint spawnAmount)
	{
	setSpawnAmount(spawnAmount, spawnAmount);
	}
	// How many to spawn at once when emitting particles.
	FORCEINLINE void setSpawnAmount(uint minSpawnAmount, uint maxSpawnAmount)
	{
	m_minSpawnAmount = minSpawnAmount;
	m_maxSpawnAmount = maxSpawnAmount;
	}
	//*/

	FORCEINLINE void setTint(const Rgba& startTint, float endAlpha = 0.f)
	{
		setTint(startTint, Rgba(startTint, endAlpha));
	}
	FORCEINLINE void setTint(const Rgba& startTint, const Rgba& endTint)
	{
		m_startTint = startTint;
		m_endTint = endTint;
	}

	FORCEINLINE void setSize(const Vector3& size)
	{
		setSize(size, size);
	}
	FORCEINLINE void setSize(const Vector3& minSize, const Vector3& maxSize)
	{
		m_minSize = minSize;
		m_maxSize = maxSize;
	}

	FORCEINLINE void setSpeed(float speed)
	{
		setSpeed(speed, speed);
	}
	FORCEINLINE void setSpeed(float minSpeed, float maxSpeed)
	{
		m_minSpeed = minSpeed;
		m_maxSpeed = maxSpeed;
	}
public:
	bool bPersistent = true; // If true, the emitter will only die when the SystemOwner dies
	float m_deathAge = 0.f; //How long to emit particles for
	float m_warmUpTime = -1.0f; //How long to run before we've finished warming up. Negative means to run for the max lifetime of our particles.
	float fps = 60.f; //Assumed frame rate game is running at. Helps with getting more accurate update results
	bool bPrewarm = false; // If true, will always prewarm particles when (re)started.


	//////////////////////////////////////////////////////////////////////////
	//Basic Particle properties
	//////////////////////////////////////////////////////////////////////////
	uint m_maxNumParticles = 0;
	float m_emissionRate = 1; // In particles per second.

	Vector3 m_gravity = Vector3::ZERO;
	float m_radialAcceleration = 0.f;
	float m_tangentialAcceleration = 0.f;

	float m_minLifetime = 0.0;
	float m_maxLifetime = 0.0;

	//  Old thought code: This would actually be more like a burst
	// How many to spawn at once when emitting particles.
	//uint m_minSpawnAmount = 0;
	//uint m_maxSpawnAmount = 0;

	float m_minSpeed = 0.f;
	float m_maxSpeed = 0.f;

	Vector3 m_direction = Vector3::XAXIS;


	//////////////////////////////////////////////////////////////////////////
	//Visual properties
	//////////////////////////////////////////////////////////////////////////
	SQT m_transform; //Used as an offset from the main particle system.

	Vector3 m_minSize = Vector3::ONE;
	Vector3 m_maxSize = Vector3::ONE;

	Mesh* m_mesh = nullptr; //Mesh to use for drawing.
	TODO("Switch to Material System once that is implemented!");
	std::string m_texturePath = "White"; //Texture to bind to when drawing.

	Rgba m_startTint = Rgba::WHITE;
	Rgba m_endTint = Rgba(m_startTint, 0.f);

	RHIEnum::BlendFactor m_sourceBlend = RHIEnum::BLEND_SOURCE_ALPHA;
	RHIEnum::BlendFactor m_destinationBlend = RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA;

protected:
	ParticleEmitterDefinition(const std::string& name);
	ParticleEmitterDefinition(const XMLNode& element);
};
//////////////////////////////////////////////////////////////////////////
class ParticleEmitter
{
public:
	ParticleEmitter() {}
	~ParticleEmitter();
	
	//Always call an Initialize after you have set all of your values!
	//Initialize will set up the defaults that 
	void Initialize(SimpleRenderer* renderer);
	void Initialize(const std::string& defName, SimpleRenderer* renderer);

	void Update(float deltaSeconds);

	void Render(SimpleRenderer* renderer) const;


	// Should spawn particles and simulate the system up to the max
	// lifetime of the longest living particle
	// [optionally pass it a set time]
	void Prewarm(float secondsToWarmFor);
	void Prewarm()
	{
		float warmTime = m_warmUpTime >= 0.f ? m_warmUpTime : m_maxLifetime;
		Prewarm(warmTime);
	}

	// resumes playing of an effect
	void Play();

	// stops emission on this effect but allows
	// existing particls to continue to play out.
	void Stop();

	// Destroys existing particles, puts effect
	// back in it's initial state [will run initial burst and prewarm]
	void Reset();

	
	/*FORCEINLINE*/ //Can't remember if inline also inlines non-inlined functions that are called, which I don't want.
	void Destroy(bool immediate = false)
	{
		if (immediate)
			Destroy_Immediate();
		else
			Destroy_WhenFinished();
	}

	// stops the effect, but allows the existing particles to play out.
	// The system will destroy itself once it is finished (see is_finished)
	void Destroy_WhenFinished();

	// stops a particle effect, immeidately destroying 
	// all of its existing particles.
	void Destroy_Immediate();

	// no systems will spawn and all particles have met a natural end.
	// (a stopped particle system is considered finished once its particles have played out)
	FORCEINLINE bool isFinished() const
	{
		return m_particleCount == 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// Setters for defining emitter data. NOTE: USE PARTICLE EMITTER DEFINITION FOR INTIAL SET UP!
	// - Use these when available instead of setting the variables directly for safety.
	// - These are here if you want to change somethings after setting it up with the definition
	//////////////////////////////////////////////////////////////////////////
	FORCEINLINE void setLifetime(float lifetime)
	{
		setLifetime(lifetime, lifetime);
	}
	FORCEINLINE void setLifetime(float minLifetime, float maxLifetime)
	{
		m_minLifetime = minLifetime;
		m_maxLifetime = maxLifetime;
	}
	
	/*/ Old thought code: This would actually be more like a burst
	// How many to spawn at once when emitting particles.
	FORCEINLINE void setSpawnAmount(uint spawnAmount)
	{
		setSpawnAmount(spawnAmount, spawnAmount);
	}
	// How many to spawn at once when emitting particles.
	FORCEINLINE void setSpawnAmount(uint minSpawnAmount, uint maxSpawnAmount)
	{
		m_minSpawnAmount = minSpawnAmount;
		m_maxSpawnAmount = maxSpawnAmount;
	}
	//*/

	FORCEINLINE void setTint(const Rgba& startTint, float endAlpha = 0.f) 
	{
		setTint(startTint, Rgba(startTint, endAlpha));
	}
	FORCEINLINE void setTint(const Rgba& startTint, const Rgba& endTint)
	{
		m_startTint = startTint;	
		m_endTint = endTint;
	}

	FORCEINLINE void setSize(const Vector3& size)
	{
		setSize(size, size);
	}
	FORCEINLINE void setSize(const Vector3& minSize, const Vector3& maxSize)
	{
		m_minSize = minSize;
		m_maxSize = maxSize;
	}

	FORCEINLINE void setSpeed(float speed)
	{
		setSpeed(speed, speed);
	}
	FORCEINLINE void setSpeed(float minSpeed, float maxSpeed)
	{
		m_minSpeed = minSpeed;
		m_maxSpeed = maxSpeed;
	}

	FORCEINLINE void setMaxNumberOfParticles(uint maxNumParticles)
	{
		m_maxNumParticles = maxNumParticles;
		m_particles.resize(m_maxNumParticles);
	}


	//////////////////////////////////////////////////////////////////////////
	// Age Getting Stuff
	//////////////////////////////////////////////////////////////////////////
	FORCEINLINE bool isAlive() const
	{
		return (m_age < m_deathAge || bPersistent);
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
	std::string m_name = "Unnamed Particle Emitter";
	bool bPersistent = true; // If true, the emitter will only die when the SystemOwner dies
	float m_deathAge = 0.f; //How long to emit particles for
	float m_warmUpTime = -1.0f; //How long to run before we've finished warming up. Negative means to run for the max lifetime of our particles.
	float fps = 60.f; //Assumed frame rate game is running at. Helps with getting more accurate update results
	bool bPrewarm = false; // If true, will always prewarm particles when (re)started.


	//////////////////////////////////////////////////////////////////////////
	//Basic Particle properties
	//////////////////////////////////////////////////////////////////////////
	float m_emissionRate = 1; // In particles per second.

	Vector3 m_gravity				= Vector3::ZERO;
	float m_radialAcceleration		= 0.f;
	float m_tangentialAcceleration	= 0.f;

	float m_minLifetime = 0.0;
	float m_maxLifetime = 0.0;

	//  Old thought code: This would actually be more like a burst
	// How many to spawn at once when emitting particles.
	//uint m_minSpawnAmount = 0;
	//uint m_maxSpawnAmount = 0;

	float m_minSpeed = 0.f;
	float m_maxSpeed = 0.f;

	Vector3 m_direction = Vector3::XAXIS;


	//////////////////////////////////////////////////////////////////////////
	//Visual properties
	//////////////////////////////////////////////////////////////////////////
	SQT m_transform; //Used as an offset from the main particle system.

	Vector3 m_minSize = Vector3::ONE;
	Vector3 m_maxSize = Vector3::ONE;

	Mesh* mo_mesh = nullptr; //Pointer to Mesh to used for drawing. Usually held by the definition so is not deleted.
	TODO("Switch to Material System once that is implemented!");
	std::string m_texturePath = "White"; //Texture to bind to when drawing.

	Rgba m_startTint = Rgba::WHITE;
	Rgba m_endTint   = Rgba(m_startTint, 0.f);

	RHIEnum::BlendFactor m_sourceBlend		= RHIEnum::BLEND_SOURCE_ALPHA;
	RHIEnum::BlendFactor m_destinationBlend = RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA;

protected:
	void UpdateParticles(float deltaSeconds);
	void EmitParticles(float deltaSeconds);

	void addParticle(const Particle& particle);
	void removeParticle(uint index);
	void cleanParticles();

protected:
	std::vector<Particle> m_particles;
	uint m_maxNumParticles = 0;
	uint m_particleCount = 0; //Number of particles currently alive and where I can start using a new particle.
	float m_emitterTime = 0.f; //A cool way to keep track of how many particles I should be emitting by having a collection of time
	float m_age = 0.f;

protected:
	friend class ParticleSystem;
	bool bPendingDestroy = false;
	bool bStopEmitting = false;
	ConstantBuffer* particle_cb;
	particle_buffer particleData;
};