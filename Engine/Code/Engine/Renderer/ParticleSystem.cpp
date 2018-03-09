#include "Engine/Renderer/ParticleSystem.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineConfig.hpp"
//////////////////////////////////////////////////////////////////////////
ParticleSystem::ParticleSystem()
{
	m_transform.setFromMatrix(Matrix4());
}

ParticleSystem::~ParticleSystem()
{
	Destroy_Immediate();
}
//////////////////////////////////////////////////////////////////////////
void ParticleSystem::Initialize()
{
	setupOneOffTime();
}

void ParticleSystem::Initialize(const std::string& defName, SimpleRenderer* renderer)
{
	ParticleSystemDefinition* define = ParticleSystemDefinition::getDefinition(defName);

	ASSERT_RECOVERABLE(nullptr != define, Stringf("WARNING: Particle Emitter Definition \"%s\" not found!", defName.c_str()).c_str());
	if (nullptr == define)
	{
		m_name = defName;
		Initialize();
		return;
	}

	m_name = define->m_name;
	m_oneOffTime = define->m_oneOffTime;
	m_transform = define->m_transform;

	for (uint i = 0; i < define->m_emitterDefNames.size(); ++i)
	{
		AddEmitter(define->m_emitterDefNames[i], renderer);
	}

	Initialize();
}

void ParticleSystem::Update(float deltaSeconds)
{
	m_age += deltaSeconds;
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		if (nullptr == m_emitters[i])
			continue;
		m_emitters[i]->Update(deltaSeconds);
	}

	cleanEmitters();
}

void ParticleSystem::Render(SimpleRenderer* renderer) const
{
	renderer->pushMatrix();
	renderer->transform(m_transform.getAsMatrix());
	for (ParticleEmitter* current : m_emitters)
	{
		if (nullptr == current || current->isFinished())
			continue;

		current->Render(renderer);
	}
	renderer->popMatrix();
}

void ParticleSystem::Play()
{
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		m_emitters[i]->Play();
	}
}

void ParticleSystem::Stop()
{
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		m_emitters[i]->Stop();
	}
}

void ParticleSystem::Reset()
{
	m_age = 0.f;
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		m_emitters[i]->Reset();
	}
}

void ParticleSystem::Destroy_WhenFinished()
{
	bPendingDestroy = true;
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		if(nullptr == m_emitters[i])
			continue;
		m_emitters[i]->Destroy_WhenFinished();
	}
}

void ParticleSystem::Destroy_Immediate()
{
	bPendingDestroy = true;
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		SAFE_DELETE(m_emitters[i]);
	}
}

bool ParticleSystem::isDead() const
{
	return (bIsOneOff && m_age > m_oneOffTime);
}

bool ParticleSystem::isFinished() const
{
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		if (nullptr == m_emitters[i])
			continue;

		if (!m_emitters[i]->isFinished())
			return false;
	}

	return true;
}

ParticleEmitter* ParticleSystem::AddEmitter(const std::string& name, SimpleRenderer* renderer)
{
	ParticleEmitter* emitter = new ParticleEmitter();
	emitter->Initialize(name, renderer);
	
	AddEmitter(emitter);
	
	return emitter;
}

void ParticleSystem::AddEmitter(ParticleEmitter* emitter)
{
	static uint numEmittersMade = 0;
	m_emitters.push_back(emitter);
	
	// Add a number to the end of its name so it'll be easier to debug.
	emitter->m_name.append(Stringf("_%i", numEmittersMade));

	++numEmittersMade;
}

//////////////////////////////////////////////////////////////////////////
void ParticleSystem::cleanEmitters()
{
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		if (nullptr == m_emitters[i])
			continue;

		if ((m_emitters[i]->isDead() || this->isDead()) && !m_emitters[i]->bPendingDestroy)
		{
			m_emitters[i]->Destroy();
		}

		// Don't actually delete them unless we are being destroyed also. This allows Systems to be reset correctly if they have emitters that aren't persistent.
		if (bPendingDestroy && m_emitters[i]->bPendingDestroy && m_emitters[i]->isFinished())
		{
			SAFE_DELETE(m_emitters[i]);
		}
	}
}

void ParticleSystem::setupOneOffTime()
{
	if (m_oneOffTime > 0.f)
		return;

	for (uint i = 0; i < m_emitters.size(); i++)
	{
		if (m_emitters[i]->bPersistent)
			continue;

		if (m_emitters[i]->m_maxLifetime > m_oneOffTime)
			m_oneOffTime = m_emitters[i]->m_maxLifetime;
	}

	if (m_oneOffTime < 0.f)
		m_oneOffTime = 1.f;
}

void ParticleSystem::Prewarm()
{
	for (uint i = 0; i < m_emitters.size(); ++i)
	{
		if (m_emitters[i]->bPrewarm)
		{
			m_emitters[i]->Prewarm();
		}
	}
}

////////////////////////////////////////////////////////////////////////// Definition //////////////////////////////////////////////////////////////////////////
#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"
//////////////////////////////////////////////////////////////////////////
ParticleSystemDefinition::ParticleSystemDefinition(const std::string& name)
	: DataDrivenDefinition(name)
{
	ASSERT_OR_DIE(m_name != ERROR_NO_NAME, "ERROR: A Particle System name was not assigned");

	m_transform.setFromMatrix(Matrix4());
}

ParticleSystemDefinition::ParticleSystemDefinition(const XMLNode& element)
	: DataDrivenDefinition(element)
{
	ASSERT_OR_DIE(m_name != ERROR_NO_NAME, "ERROR: A Particle System name was not assigned");

	m_oneOffTime = ParseXMLAttribute(element, "oneOffTime", m_oneOffTime);
	// Transform
	m_transform.setFromMatrix(Matrix4());
	XMLNode transformNode = element.getChildNode("Transform");
	if (!transformNode.isEmpty())
	{
		Vector3 scale = Vector3::ONE;
		Vector3 translation = Vector3::ZERO;

		scale = ParseXMLAttribute(transformNode, "scale", scale);
		translation = ParseXMLAttribute(transformNode, "position", translation);

		m_transform.scale = scale;
		m_transform.position = translation;
	}

	//Emitters
	int numEmitters = element.nChildNode("Emitter");
	for (int i = 0; i < numEmitters; i++)
	{
		XMLNode emitterNode = element.getChildNode("Emitter", i);
		
		// If it has no child nodes, we must be referencing a previously made emitter definition
		if (emitterNode.nChildNode() == 0)
		{
			std::string defName = ParseXMLAttribute(emitterNode, "name", ERROR_NO_NAME);
			m_emitterDefNames.push_back(defName);
		}
		else
		{
			ParticleEmitterDefinition* newDef = new ParticleEmitterDefinition(emitterNode);
			newDef->m_name = Stringf("%s_%s", m_name.c_str(), newDef->m_name.c_str());
			ParticleEmitterDefinition::s_registry[newDef->m_name] = newDef;

			m_emitterDefNames.push_back(newDef->m_name);
		}
	}
}
