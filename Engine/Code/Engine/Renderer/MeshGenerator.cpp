#include "Engine/Renderer/MeshGenerator.hpp"
#include "../Core/EngineBase.hpp"

void MeshGenerator::TestVoxel(const Vector3& position, std::vector<Vertex3D>& vertexBuffer)
{
	//Bottom (-Z)
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 0.f), Vector2(0.f, 0.f), Rgba(127, 127, 0)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 0.f), Vector2(1.f, 0.f), Rgba(127, 127, 0)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 0.f), Vector2(1.f, 1.f), Rgba(127, 127, 0)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 0.f), Vector2(0.f, 1.f), Rgba(127, 127, 0)));

		//Top (+Z)
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 1.f), Vector2(0.f, 1.f), Rgba(127, 127, 255)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), Rgba(127, 127, 255)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 1.f), Vector2(1.f, 0.f), Rgba(127, 127, 255)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 1.f), Vector2(0.f, 0.f), Rgba(127, 127, 255)));

		// East (+X)
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 0.f), Vector2(0.f, 1.f), Rgba(255, 127, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 0.f), Vector2(1.f, 1.f), Rgba(255, 127, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 1.f), Vector2(1.f, 0.f), Rgba(255, 127, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 1.f), Vector2(0.f, 0.f), Rgba(255, 127, 127)));

		// West (-X)
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 1.f), Vector2(1.f, 0.f), Rgba(0, 127, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 1.f), Vector2(0.f, 0.f), Rgba(0, 127, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 0.f), Vector2(0.f, 1.f), Rgba(0, 127, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 0.f), Vector2(1.f, 1.f), Rgba(0, 127, 127)));

		// South (-Y)
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 0.f), Vector2(0.f, 1.f), Rgba(127, 0, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 0.f), Vector2(1.f, 1.f), Rgba(127, 0, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 1.f), Vector2(1.f, 0.f), Rgba(127, 0, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 1.f), Vector2(0.f, 0.f), Rgba(127, 0, 127)));

		// North (+Y)
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 1.f), Vector2(1.f, 0.f), Rgba(127, 255, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 1.f), Vector2(0.f, 0.f), Rgba(127, 255, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 0.f), Vector2(0.f, 1.f), Rgba(127, 255, 127)));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 0.f), Vector2(1.f, 1.f), Rgba(127, 255, 127)));
}

void MeshGenerator::Cube(const Vector3& position, std::vector<Vertex3D>& vertexBuffer, unsigned int subdivision /*= 0*/)
{
	// Front (+Z)
	Vector3 normalZPos(0.f, 0.f, -1.f);
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 0.f), Vector2(0.f, 0.f), normalZPos, Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 0.f), Vector2(1.f, 0.f), normalZPos, Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 0.f), Vector2(1.f, 1.f), normalZPos, Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 0.f), Vector2(0.f, 1.f), normalZPos, Vector3::XAXIS));

	// Back (-Z)
	Vector3 normalZNeg(0.f, 0.f, 1.f);
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 1.f), Vector2(0.f, 1.f), normalZNeg, -Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), normalZNeg, -Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 1.f), Vector2(1.f, 0.f), normalZNeg, -Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 1.f), Vector2(0.f, 0.f), normalZNeg, -Vector3::XAXIS));

	// Left (-X)
	Vector3 normalXNeg(1.f, 0.f, 0.f);
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 0.f), Vector2(0.f, 0.f), normalXNeg, -Vector3::ZAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 1.f), Vector2(1.f, 0.f), normalXNeg, -Vector3::ZAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 1.f), Vector2(1.f, 1.f), normalXNeg, -Vector3::ZAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 0.f), Vector2(0.f, 1.f), normalXNeg, -Vector3::ZAXIS));

	// Right (+X)
	Vector3 normalXPos(-1.f, 0.f, 0.f);
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 0.f), Vector2(0.f, 1.f), normalXPos, Vector3::ZAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 1.f), Vector2(1.f, 1.f), normalXPos, Vector3::ZAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 1.f), Vector2(1.f, 0.f), normalXPos, Vector3::ZAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 0.f), Vector2(0.f, 0.f), normalXPos, Vector3::ZAXIS));

	// Top (+Y)
	Vector3 normalYPos(0.f, 1.f, 0.f);
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 0.f), Vector2(0.f, 1.f), normalYPos, Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 1.f, 1.f), Vector2(1.f, 1.f), normalYPos, Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 1.f), Vector2(1.f, 0.f), normalYPos, Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 1.f, 0.f), Vector2(0.f, 0.f), normalYPos, Vector3::XAXIS));

	// Bottom (-Y)
	Vector3 normalYNeg(0.f, -1.f, 0.f);
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 0.f), Vector2(0.f, 0.f), normalYNeg, -Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(1.f, 0.f, 1.f), Vector2(1.f, 0.f), normalYNeg, -Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 1.f), Vector2(1.f, 1.f), normalYNeg, -Vector3::XAXIS));
	vertexBuffer.push_back(Vertex3D(position + Vector3(0.f, 0.f, 0.f), Vector2(0.f, 1.f), normalYNeg, -Vector3::XAXIS));

	UNUSED(subdivision);
}
