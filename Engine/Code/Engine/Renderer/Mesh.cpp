#include "Engine/Renderer/Mesh.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
//////////////////////////////////////////////////////////////////////////
Mesh::Mesh()
	: vbo(nullptr)
	, ibo(nullptr)
{
}

Mesh::~Mesh()
{
	SAFE_DELETE(vbo);
	SAFE_DELETE(ibo);
}