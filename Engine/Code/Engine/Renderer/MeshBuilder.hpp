#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Renderer/DrawInstruction.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class Vector2;
class Vector3;
class AABB2;
class BinaryStream;
//////////////////////////////////////////////////////////////////////////
class MeshBuilder
{
public:
	MeshBuilder();

	void clear();

	void begin();
	void begin(RHIEnum::PrimativeType type, bool useIbo);
	void end();

	void addVertex(const Vector3& position);
	void addVertex(const Vector3& position, const Vector2& uv);
	void addVertex(const Vector3& position, const Vector2& uv, const Vector3& tangent, const Vector3& normal);
	void addVertex(const Vector3& position, const Vector2& uv, const Vector3& tangent, const Vector3& bitangent, const Vector3& normal);
	void addVertex(const Vertex3D& vertex);
	inline void addIndex(uint index) { m_indices.push_back(index); }

	void setUV(float u, float v);
	void setUV(const Vector2& uv);
	void setUV(uint index, const Vector2& uv);
	//void setUVs(const Vector2& uv0, const Vector2& uv1);
	void setAllUVs(const Vector2& uv);

	void setNormal(const Vector3& normal);
	void setTangent(const Vector3& tangent);
	void setBitangent(const Vector3& bitangent);
	void setTangentAndNormal(const Vector3& tangent, const Vector3& normal);
	void setTangentAndBitangent(const Vector3& tangent, const Vector3& bitangent);
	void setTBN(const Vector3& tangent, const Vector3& bitangent, const Vector3& normal);

	void setColor(const Rgba& color);

	void setBones(const UIntVector4& boneIndexes);
	void setWeights(const Vector4& weights);


	//void transform(const Matrix4& transformMatrix, uint startIndex = 0, uint count = )

	inline bool empty() const { return m_drawInstructions.empty(); }
	inline uint getNextVertexIndex() const { return m_vertices.size(); }
	inline uint getDataFlags() const { return m_dataFlags; }


	void addSphereOutline(const Vector3& center, float radius, const Rgba& color, float numSides = 16.f);
	void addSimple2dQuad(const AABB2& bounds, float Z2D, const AABB2& texureBounds, const Rgba& color);


	const bool usesIboEver() const;



	void write(BinaryStream &stream);
	void read(BinaryStream &stream);
public:
	Vertex3D m_vertexStamp;
	uint m_dataFlags;

	std::vector<Vertex3D> m_vertices;
	std::vector<uint> m_indices;
	//#ToDo: Add in material class
	std::vector<DrawInstruction> m_drawInstructions;

	DrawInstruction m_currentDrawInstruction;
	uint m_recursionCount;
};