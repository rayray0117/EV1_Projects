#include "Game/DebugEndEffector.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
//////////////////////////////////////////////////////////////////////////
DebugEndEffector::DebugEndEffector()
{
	MeshBuilder mb;
	mb.begin(RHIEnum::PRIMATIVE_LINE_STRIP, false);
	mb.addSphereOutline(Vector3::ZERO, 5.f, Rgba::FORESTGREEN, 8.f);
	mb.end();
	m_mesh = g_renderer->CreateAndGetMeshStatic(&mb);
}

DebugEndEffector::DebugEndEffector(const Rgba& color)
{
	MeshBuilder mb;
	mb.begin(RHIEnum::PRIMATIVE_LINE_STRIP, false);
	mb.addSphereOutline(Vector3::ZERO, 5.f, color, 8.f);
	mb.end();
	m_mesh = g_renderer->CreateAndGetMeshStatic(&mb);
}

DebugEndEffector::~DebugEndEffector()
{
	SAFE_DELETE(m_mesh);
}
//////////////////////////////////////////////////////////////////////////
void DebugEndEffector::Update(float deltaTime)
{
	float speed = m_movementSpeed * deltaTime;
	//X
	if (g_inputSystem->isKeyDown(KEYCODE_RIGHT))
	{
		m_position.x += speed;
	}
	if (g_inputSystem->isKeyDown(KEYCODE_LEFT))
	{
		m_position.x -= speed;
	}
	//Y
	if (g_inputSystem->isKeyDown(KEYCODE_PAGEUP))
	{
		m_position.y += speed;
	}
	if (g_inputSystem->isKeyDown(KEYCODE_PAGEDOWN))
	{
		m_position.y -= speed;
	}
	//Z
	if (g_inputSystem->isKeyDown(KEYCODE_UP))
	{
		m_position.z -= speed;
	}
	if (g_inputSystem->isKeyDown(KEYCODE_DOWN))
	{
		m_position.z += speed;
	}
}

void DebugEndEffector::Render() const
{
	g_renderer->BindShader(DEFAULT_UNLIT_SHADER);
	g_renderer->BindTexture("White");
	g_renderer->BindTexture("FlatNormal", NORMAL_TEXTURE_INDEX);
	g_renderer->BindTexture("White", SPECULAR_TEXTURE_INDEX);
	g_renderer->BindTexture("White", 3);

	g_renderer->pushMatrix();
	g_renderer->translate3D(m_position);
	g_renderer->drawMesh(m_mesh);
	g_renderer->popMatrix();
}