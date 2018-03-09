#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Core/Profiler.hpp"
//#include "Engine/Core/EngineConfig.hpp"
////////////////////////////////////////////////////////////////////////// Particle Emitter //////////////////////////////////////////////////////////////////////////
ParticleEmitter::~ParticleEmitter()
{
	Destroy_Immediate();
}
//////////////////////////////////////////////////////////////////////////
void ParticleEmitter::Initialize(SimpleRenderer* renderer)
{
	GUARANTEE_RECOVERABLE(m_maxNumParticles != 0, Stringf("WARNING: \"%s\" has 0 for it's maximum \" // /*number of particles making it essentially unusable!", m_name.c_str()).c_str());

	m_particles.resize(m_maxNumParticles);

	if (mo_mesh == nullptr)
	{
		MeshBuilder* mb = new MeshBuilder();
		mb->addSimple2dQuad(AABB2::NEG_ONE_TO_ONE, 0.f, AABB2::ZERO_TO_ONE, Rgba::WHITE);
		mo_mesh = renderer->CreateAndGetMeshStatic(mb);

		SAFE_DELETE(mb);
	}


	particleData.start_color = m_startTint;
	particleData.end_color = m_endTint;
	particle_cb = new ConstantBuffer(renderer->device, &particleData, sizeof(particleData));
}

void ParticleEmitter::Initialize(const std::string& defName, SimpleRenderer* renderer)
{
	ParticleEmitterDefinition* define = ParticleEmitterDefinition::getDefinition(defName);

	ASSERT_RECOVERABLE(nullptr != define, Stringf("WARNING: Particle Emitter Definition \"%s\" not found!", defName).c_str());
	if (nullptr == define)
	{
		m_name = defName;
		Initialize(renderer);
		return;
	}

	m_name = define->m_name;
	bPersistent = define->bPersistent;
	m_deathAge = define->m_deathAge;
	m_warmUpTime = define->m_warmUpTime;
	fps = define->fps;
	bPrewarm = define->bPrewarm;

	m_maxNumParticles = define->m_maxNumParticles;
	m_emissionRate = define->m_emissionRate;

	m_gravity = define->m_gravity;
	m_radialAcceleration = define->m_radialAcceleration;
	m_tangentialAcceleration = define->m_tangentialAcceleration;

	m_minLifetime = define->m_minLifetime;
	m_maxLifetime = define->m_maxLifetime;

	m_minSpeed = define->m_minSpeed;
	m_maxSpeed = define->m_maxSpeed;

	m_direction = define->m_direction;

	m_transform = define->m_transform;

	m_minSize = define->m_minSize;
	m_maxSize = define->m_maxSize;

	mo_mesh = define->m_mesh;
	m_texturePath = define->m_texturePath;

	m_startTint = define->m_startTint;
	m_endTint = define->m_endTint;

	m_sourceBlend = define->m_sourceBlend;
	m_destinationBlend = define->m_destinationBlend;

	Initialize(renderer);

	//If the definition never had a mesh, then hand it the default one we created so other can use it and we don't have to clean that up.
	if (nullptr == define->m_mesh)
	{
		define->m_mesh = mo_mesh;
	}
}

//////////////////////////////////////////////////////////////////////////
void ParticleEmitter::Update(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	EmitParticles(deltaSeconds);
	
	UpdateParticles(deltaSeconds);

	cleanParticles();

	m_age += deltaSeconds;
}

void ParticleEmitter::Render(SimpleRenderer* renderer) const
{
	PROFILE_SCOPE_FUNCTION();
	//Set Material Stuff
	renderer->BindTexture(m_texturePath);
	renderer->SetBlendFunc(m_sourceBlend, m_destinationBlend);
	renderer->BindShader(PARTICLE_UNLIT_SHADER);
	renderer->SetConstantBuffer(particle_cb, 6);


	renderer->pushMatrix();
	//renderer->drawMesh(m_mesh); //Maybe I could use this as a Debug thing later?

	renderer->transform(m_transform.getAsMatrix());



	//particle_buffer* pb = const_cast<particle_buffer*>(&particleData); //Spooky cheating const stuff to modify my variable in a const function.
	for (uint i = 0; i < m_particleCount; ++i)
	{
		PROFILE_SCOPE("Particle");
		const Particle& current = m_particles[i];
		//pb->particle_fraction = current.getPercentDead();
		particleData.particle_fraction = current.getPercentDead();
		particle_cb->update(renderer->context, &particleData);

		renderer->pushMatrix();
		renderer->scale3D(current.m_size);
		renderer->translate3D(current.m_position);
		renderer->drawMesh(mo_mesh);
		renderer->popMatrix();
	}

	renderer->popMatrix();
}

void ParticleEmitter::Prewarm(float secondsToWarmFor)
{
	const float deltaSeconds = inverse_f(fps);
	float timeSimulated = 0.f;

	while (timeSimulated < secondsToWarmFor)
	{
		Update(deltaSeconds);
		timeSimulated += deltaSeconds;
	}
	m_age = 0.f;
}

void ParticleEmitter::Play()
{
	bStopEmitting = false;
}

void ParticleEmitter::Stop()
{
	bStopEmitting = true;
}

void ParticleEmitter::Reset()
{
	m_particleCount = 0;
	m_emitterTime = 0.f;
	m_age = 0.f;
	bPendingDestroy = false;

	if (bPrewarm)
	{
		Prewarm();
	}
	Play();
}

void ParticleEmitter::Destroy_WhenFinished()
{
	bStopEmitting = true;
	bPendingDestroy = true;
}

void ParticleEmitter::Destroy_Immediate()
{
	m_particleCount = 0;
	m_emitterTime = 0.f;
	bPendingDestroy = true;
	bStopEmitting = true;
	SAFE_DELETE(particle_cb);
	//SAFE_DELETE(mo_mesh);

}
//////////////////////////////////////////////////////////////////////////
void ParticleEmitter::UpdateParticles(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	for (uint i = 0; i < m_particleCount; ++i)
	{
		Particle& current = m_particles[i];

		current.m_forces = Vector3::ZERO;
		current.m_radial = Vector3::ZERO;

		// dont apply radial forces until moved away from the emitter
		if (current.m_position != Vector3::ZERO)
		{
			current.m_radial = current.m_position;
			current.m_radial.normalize();
		}
		current.m_tangential = current.m_radial;
		current.m_radial *= m_radialAcceleration;

		TODO("Switch to work as 3D instead of 2D");
		float newY = current.m_tangential.x;
		current.m_tangential.x = -current.m_tangential.y;
		current.m_tangential.y = newY;

		current.m_tangential *= m_tangentialAcceleration;

		current.m_forces = current.m_radial + current.m_tangential + m_gravity;

		current.Update(deltaSeconds);
	}
}

void ParticleEmitter::EmitParticles(float deltaSeconds)
{
	PROFILE_SCOPE_FUNCTION();
	if (bStopEmitting)
		return;

	float rate = inverse_f(m_emissionRate);
	m_emitterTime += deltaSeconds;

	while (m_particleCount < m_maxNumParticles && m_emitterTime > rate)
	{
		//Setup particle! Note: Should probably be broken up into a seperate particle.
		//Particle* particle = new Particle();
		Particle particle;
		particle.m_deathAge = getRandomFloatInRange(m_minLifetime, m_maxLifetime);
		
		particle.m_velocity = m_direction;
		float speed = getRandomFloatInRange(m_minSpeed, m_maxSpeed);
		particle.m_velocity.setLength(speed);

		particle.m_size = getRandomVector3InRange(m_minSize, m_maxSize);

		addParticle(particle);

		m_emitterTime -= rate;
	}
}

void ParticleEmitter::addParticle(const Particle& particle)
{
	m_particles[m_particleCount] = particle;
	++m_particleCount;
}

void ParticleEmitter::removeParticle(uint index)
{
	if (m_particleCount == 0 || index >= m_particleCount)
		return;

	--m_particleCount;
	m_particles[index] = m_particles[m_particleCount];
}

void ParticleEmitter::cleanParticles()
{
	PROFILE_SCOPE_FUNCTION();
	for (uint particleIndex = 0; particleIndex < m_particles.size(); ++particleIndex)
	{
		if (m_particles[particleIndex].isDead())
		{
			removeParticle(particleIndex);
		}
	}
}
////////////////////////////////////////////////////////////////////////// Definition //////////////////////////////////////////////////////////////////////////
#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"

ParticleEmitterDefinition::~ParticleEmitterDefinition()
{
	SAFE_DELETE(m_mesh);
}
//////////////////////////////////////////////////////////////////////////
ParticleEmitterDefinition::ParticleEmitterDefinition(const std::string& name)
	: DataDrivenDefinition(name)
{
	ASSERT_OR_DIE(m_name != ERROR_NO_NAME, "ERROR: A Particle Emitter name was not assigned");

	m_transform.setFromMatrix(Matrix4());
}


ParticleEmitterDefinition::ParticleEmitterDefinition(const XMLNode& element)
	: DataDrivenDefinition(element)
{
	ASSERT_OR_DIE(m_name != ERROR_NO_NAME, "ERROR: A Particle Emitter name was not assigned");

	m_transform.setFromMatrix(Matrix4());


	// Emitter
	XMLNode emitterNode = element.getChildNode("Base");
	if (!emitterNode.isEmpty())
	{
		// Lifetime
		XMLNode lifetimeNode = emitterNode.getChildNode("Lifetime");
		if (!lifetimeNode.isEmpty())
		{
			bPersistent = ParseXMLAttribute(lifetimeNode, "persistent", bPersistent);
			m_deathAge = ParseXMLAttribute(lifetimeNode, "death", m_deathAge);
		}

		// Warming
		XMLNode warmNode = emitterNode.getChildNode("Warming");
		if (!warmNode.isEmpty())
		{
			bPrewarm = ParseXMLAttribute(warmNode, "enabled", true); //If I added Warming to my xml assume I want it, but let me also explicitly say so.
			m_warmUpTime = ParseXMLAttribute(warmNode, "time",m_warmUpTime);
			fps = ParseXMLAttribute(warmNode, "fps", fps);
		}

		// Emission
		XMLNode emissionNode = emitterNode.getChildNode("Emission");
		if (!emissionNode.isEmpty())
		{
			m_maxNumParticles = (uint)ParseXMLAttribute(emissionNode, "max", (int)m_maxNumParticles);
			m_emissionRate = ParseXMLAttribute(emissionNode, "rate", m_emissionRate);
		}

		// Transform
		XMLNode transformNode = emitterNode.getChildNode("Transform");
		if (!transformNode.isEmpty())
		{	
			Vector3 scale = Vector3::ONE;
			Vector3 translation = Vector3::ZERO;

			scale = ParseXMLAttribute(transformNode, "scale", scale);
			translation = ParseXMLAttribute(transformNode, "position", translation);

			m_transform.scale = scale;
			m_transform.position = translation;
		}

		// Force
		XMLNode forceNode = emitterNode.getChildNode("Force");
		if (!forceNode.isEmpty())
		{
			m_gravity = ParseXMLAttribute(forceNode, "gravity", m_gravity);
			m_radialAcceleration = ParseXMLAttribute(forceNode, "radial", m_radialAcceleration);
			m_tangentialAcceleration = ParseXMLAttribute(forceNode, "tangent", m_tangentialAcceleration);
		}

	}
	


	// Particle
	XMLNode particleNode = element.getChildNode("Particle");
	if (!particleNode.isEmpty())
	{
		// Lifetime
		XMLNode lifetimeNode = particleNode.getChildNode("Lifetime");
		if (!lifetimeNode.isEmpty())
		{
			m_minLifetime = ParseXMLAttribute(lifetimeNode, lifetimeNode.getAttributeName(), m_minLifetime);
			m_maxLifetime = ParseXMLAttribute(lifetimeNode, "max", m_minLifetime);
		}

		// Speed
		XMLNode speedNode = particleNode.getChildNode("Speed");
		if (!speedNode.isEmpty())
		{
			m_minSpeed = ParseXMLAttribute(speedNode, speedNode.getAttributeName(), m_minSpeed);
			m_maxSpeed = ParseXMLAttribute(speedNode, "max", m_minSpeed);
		}

		XMLNode directionNode = particleNode.getChildNode("Direction");
		if (!directionNode.isEmpty())
		{
			m_direction = ParseXMLAttribute(directionNode, directionNode.getAttributeName(), m_direction);
		}

		// Size
		XMLNode sizeNode = particleNode.getChildNode("Size");
		if (!sizeNode.isEmpty())
		{
			m_minSize = ParseXMLAttribute(sizeNode, sizeNode.getAttributeName(), m_minSize);
			m_maxSize = ParseXMLAttribute(sizeNode, "max", m_minSize);
		}

		// Texture
		XMLNode textureNode = particleNode.getChildNode("Texture");
		if (!textureNode.isEmpty())
		{
			m_texturePath = ParseXMLAttribute(textureNode, textureNode.getAttributeName(), m_texturePath);
		}

		// Color
		XMLNode colorNode = particleNode.getChildNode("Color");
		if (!colorNode.isEmpty())
		{
			m_startTint = ParseXMLAttribute(colorNode, colorNode.getAttributeName(), m_startTint);
			m_endTint = ParseXMLAttribute(colorNode, "end", m_startTint);
		}

		/*/
		// Blend
		XMLNode blendNode = element.getChildNode("Blend");
		if (!blendNode.isEmpty())
		{
			m_sourceBlend = ParseXMLAttribute(blendNode, "source", m_sourceBlend);
			m_destinationBlend = ParseXMLAttribute(blendNode, "dest", m_destinationBlend);
		}
		//*/
	}
}