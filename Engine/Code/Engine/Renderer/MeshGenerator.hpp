#pragma once
#include <vector>
#include "Engine/Renderer/Vertex3D.hpp"

namespace MeshGenerator
{
	void TestVoxel(const Vector3& position, std::vector<Vertex3D>& vertexBuffer);
	void Cube(const Vector3& position, std::vector<Vertex3D>& vertexBuffer, unsigned int subdivision = 0);
}