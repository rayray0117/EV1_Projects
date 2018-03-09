#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "../Math/AABB2.hpp"
//////////////////////////////////////////////////////////////////////////
MeshBuilder::MeshBuilder()
	: m_dataFlags(0)
	, m_recursionCount(0)
	, m_vertexStamp(Vector3::ZERO)
{
}
//////////////////////////////////////////////////////////////////////////
void MeshBuilder::clear()
{
	m_vertices.clear();
	m_indices.clear();
	m_drawInstructions.clear();
	m_vertexStamp = Vertex3D(Vector3::ZERO);
}

void MeshBuilder::begin()
{
	m_currentDrawInstruction.primative = RHIEnum::PRIMATIVE_TRIANGLE;
	m_currentDrawInstruction.useIbo = true;
	m_currentDrawInstruction.start_vertex = getNextVertexIndex();
	m_currentDrawInstruction.start_index = m_indices.size();
	m_currentDrawInstruction.vertex_count = 0;
}

void MeshBuilder::begin(RHIEnum::PrimativeType type, bool useIbo)
{
	m_currentDrawInstruction.primative = type;
	m_currentDrawInstruction.useIbo = useIbo;
	m_currentDrawInstruction.start_vertex = getNextVertexIndex();
	m_currentDrawInstruction.start_index = m_indices.size();
	m_currentDrawInstruction.vertex_count = 0;
}

void MeshBuilder::end()
{
	if (m_currentDrawInstruction.useIbo)
	{
		m_currentDrawInstruction.vertex_count = m_indices.size() - m_currentDrawInstruction.start_index;
	}
	else
	{
		m_currentDrawInstruction.vertex_count = getNextVertexIndex() - m_currentDrawInstruction.start_vertex;
	}
	m_drawInstructions.push_back(m_currentDrawInstruction);
}

void MeshBuilder::addVertex(const Vector3& position)
{
	m_vertexStamp.m_pos = position;
	addVertex(m_vertexStamp);
}

void MeshBuilder::addVertex(const Vector3& position, const Vector2& uv)
{
	setUV(uv);
	addVertex(position);
}

void MeshBuilder::addVertex(const Vector3& position, const Vector2& uv, const Vector3& tangent, const Vector3& normal)
{
	setTangentAndNormal(tangent, normal);
	addVertex(position, uv);
}

void MeshBuilder::addVertex(const Vector3& position, const Vector2& uv, const Vector3& tangent, const Vector3& bitangent, const Vector3& normal)
{
	setBitangent(bitangent);
	addVertex(position, uv, tangent, normal);
}

void MeshBuilder::addVertex(const Vertex3D& vertex)
{
	m_vertices.push_back(vertex);
}

void MeshBuilder::setUV(float u, float v)
{
	setUV(Vector2(u, v));
}

void MeshBuilder::setUV(const Vector2& uv)
{
	m_vertexStamp.m_textureCoords = uv;
}

void MeshBuilder::setUV(uint index, const Vector2& uv)
{
	if (index >= getNextVertexIndex())
		return;
	m_vertices[index].m_textureCoords = uv;
}

void MeshBuilder::setAllUVs(const Vector2& uv)
{
	for each (Vertex3D vertex in m_vertices)
	{
		vertex.m_textureCoords = uv;
	}
}

void MeshBuilder::setNormal(const Vector3& normal)
{
	m_vertexStamp.m_normal = normal;
}

void MeshBuilder::setTangent(const Vector3& tangent)
{
	m_vertexStamp.m_tangent = tangent;
}

void MeshBuilder::setBitangent(const Vector3& bitangent)
{
	m_vertexStamp.m_biTangent = bitangent;
}

void MeshBuilder::setTangentAndNormal(const Vector3& tangent, const Vector3& normal)
{
	setTangent(tangent);
	setNormal(normal);
}

void MeshBuilder::setTangentAndBitangent(const Vector3& tangent, const Vector3& bitangent)
{
	setTangent(tangent);
	setBitangent(bitangent);
}

void MeshBuilder::setTBN(const Vector3& tangent, const Vector3& bitangent, const Vector3& normal)
{
	setTangent(tangent);
	setBitangent(bitangent);
	setNormal(normal);
}

void MeshBuilder::setColor(const Rgba& color)
{
	m_vertexStamp.m_color = color;
}

void MeshBuilder::setBones(const UIntVector4& boneIndexes)
{
	m_vertexStamp.m_boneIndexes = boneIndexes;
}

void MeshBuilder::setWeights(const Vector4& weights)
{
	m_vertexStamp.m_weights = weights;
}
//////////////////////////////////////////////////////////////////////////
void MeshBuilder::addSphereOutline(const Vector3& center, float radius, const Rgba& color, float numSides /*= 16.f*/)
{
	float angleInRadians = (2.f * M_PI) / numSides;

	//begin(RHIEnum::PRIMATIVE_LINE_STRIP, false);
	for (int i = 0; i <= numSides; i++)
	{
		float angleTheta = angleInRadians*i;

		float x = center.x + radius * cosf(angleTheta);
		float y = center.y + radius * sinf(angleTheta);
		addVertex(Vertex3D(Vector3(x, y, center.z), color));
	}
	for (int i = 0; i <= numSides; i++)
	{
		float angleTheta = angleInRadians*i;

		float x = center.x + radius * cosf(angleTheta);
		float z = center.z + radius * sinf(angleTheta);
		addVertex(Vertex3D(Vector3(x, center.y, z), color));
	}
	for (int i = 0; i <= numSides; i++)
	{
		float angleTheta = angleInRadians*i;

		float z = center.z + radius * cosf(angleTheta);
		float y = center.y + radius * sinf(angleTheta);
		addVertex(Vertex3D(Vector3(center.x, y, z), color));
	}
	//end();
}

void MeshBuilder::addSimple2dQuad(const AABB2& bounds, float z2D, const AABB2& textureCoords, const Rgba& color)
{
	begin();
	setColor(color);

	addVertex(Vector3(bounds.mins.x, bounds.mins.y, z2D), Vector2(textureCoords.mins.x, textureCoords.maxs.y));
	addVertex(Vector3(bounds.maxs.x, bounds.mins.y, z2D), Vector2(textureCoords.maxs.x, textureCoords.maxs.y));
	addVertex(Vector3(bounds.maxs.x, bounds.maxs.y, z2D), Vector2(textureCoords.maxs.x, textureCoords.mins.y));
	addVertex(Vector3(bounds.mins.x, bounds.maxs.y, z2D), Vector2(textureCoords.mins.x, textureCoords.mins.y));

	addIndex(0);
	addIndex(1);
	addIndex(2);
	addIndex(0);
	addIndex(2);
	addIndex(3);

	end();
}

const bool MeshBuilder::usesIboEver() const
{
	for (uint i = 0; i < m_drawInstructions.size(); i++)
	{
		if (m_drawInstructions[i].useIbo)
			return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
#include "Engine/Core/BinaryStream.hpp"
void MeshBuilder::write(BinaryStream &stream)
{
	stream.write(m_vertices.size());
	for each (Vertex3D var in m_vertices)
	{
		stream.write(var.m_pos);
		stream.write(var.m_color);
		stream.write(var.m_textureCoords);
		stream.write(var.m_normal);
		stream.write(var.m_tangent);
		stream.write(var.m_biTangent);
		stream.write(var.m_boneIndexes);
		stream.write(var.m_weights);
	}
	stream.write(m_indices.size());
	for each (uint var in m_indices)
	{
		stream.write(var);
	}
	stream.write(m_drawInstructions.size());
	for each (DrawInstruction var in m_drawInstructions)
	{
		stream.write(var);
	}
}

void MeshBuilder::read(BinaryStream &stream)
{
	clear();
	uint vertSize;
	stream.read(vertSize);
	for(uint i = 0; i < vertSize; i++)
	{
		Vertex3D var(Vector3::ZERO);
		stream.read(var.m_pos);
		stream.read(var.m_color);
		stream.read(var.m_textureCoords);
		stream.read(var.m_normal);
		stream.read(var.m_tangent);
		stream.read(var.m_biTangent);
		stream.read(var.m_boneIndexes);
		stream.read(var.m_weights);
		m_vertices.push_back(var);
	}
	uint indSize;
	stream.read(indSize);
	for (uint i = 0; i < indSize; i++)
	{
		uint var;
		stream.write(var);
		m_indices.push_back(var);
	}
	uint drawInSize;
	stream.read(drawInSize);
	for (uint i = 0; i < drawInSize; i++)
	{
		DrawInstruction var;
		stream.read(var);
		m_drawInstructions.push_back(var);
	}
}
