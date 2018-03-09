#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Rgba.hpp"
//////////////////////////////////////////////////////////////////////////
const float Z2D = 0.0f;
//////////////////////////////////////////////////////////////////////////
struct Vertex2D
{
	Vector2 m_pos;
	Rgba	m_color;
	Vector2 m_textureCoords;

	Vertex2D(Vector2 pos, Rgba color = Rgba::WHITE) : m_pos(pos), m_color(Rgba::WHITE) {}
	Vertex2D(Vector2 pos, Vector2 texCoords, Rgba color = Rgba::WHITE) : m_pos(pos), m_color(color), m_textureCoords(texCoords) {}
};
//////////////////////////////////////////////////////////////////////////
struct Vertex3D
{
	Vector3 m_pos;
	Rgba	m_color;
	Vector2 m_textureCoords;
	Vector3 m_normal;
	Vector3 m_tangent;
	Vector3 m_biTangent;
	UIntVector4	m_boneIndexes = UIntVector4(0, 0, 0, 0);
	Vector4 m_weights = Vector4(0,0,0,0);

	//Vertex3D(Vector3 pos) : m_pos(pos), m_color(Rgba::WHITE) {}
	Vertex3D(const Vector3& pos, const Rgba& color = Rgba(255, 255, 255, 255));
	Vertex3D(const Vector3& pos, const Vector2& texCoords, const Rgba& color = Rgba(255, 255, 255, 255));
	Vertex3D(const Vector3& pos, const Vector2& texCoords, const Vector3& normal, const Rgba& color = Rgba(255, 255, 255, 255));
	Vertex3D(const Vector3& pos, const Vector2& texCoords, const Vector3& normal, const Vector3& tangent, const Rgba& color = Rgba(255, 255, 255, 255));
	Vertex3D(const Vertex2D& copy2D);
};
//////////////////////////////////////////////////////////////////////////
// Inlines
inline Vertex3D::Vertex3D(const Vector3& pos, const Rgba& color)
	: m_pos(pos)
	, m_color(color)
	, m_textureCoords(0.f, 0.f)
	, m_normal(0.f, 0.f, -1.f)
{}

inline Vertex3D::Vertex3D(const Vector3& pos, const Vector2& texCoords, const Rgba& color) 
	: m_pos(pos)
	, m_color(color)
	, m_textureCoords(texCoords) 
	, m_normal(0.f, 0.f, -1.f)
{}


inline Vertex3D::Vertex3D(const Vector3& pos, const Vector2& texCoords, const Vector3& normal, const Rgba& color /*= Rgba(255, 255, 255, 255)*/)
	: m_pos(pos)
	, m_color(color)
	, m_textureCoords(texCoords)
	, m_normal(normal)
{}


inline Vertex3D::Vertex3D(const Vertex2D& copy2D)
	: m_pos(copy2D.m_pos.x, copy2D.m_pos.y, Z2D)
	, m_color(copy2D.m_color)
	, m_textureCoords(copy2D.m_textureCoords)
{}

inline Vertex3D::Vertex3D(const Vector3& pos, const Vector2& texCoords, const Vector3& normal, const Vector3& tangent, const Rgba& color /*= Rgba(255, 255, 255, 255)*/)
	: m_pos(pos)
	, m_color(color)
	, m_textureCoords(texCoords)
	, m_normal(normal)
	, m_tangent(tangent)
{
	m_biTangent = crossProduct(m_normal, m_tangent);
}
