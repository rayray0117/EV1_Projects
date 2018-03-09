#pragma once
#include "Engine\RHI\RHITypes.hpp"
#include "Engine\Renderer\DrawInstruction.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class IndexBuffer;
class VertexBuffer;
//////////////////////////////////////////////////////////////////////////
class Mesh
{
	friend class SimpleRenderer;
public:
	~Mesh();

public:
	VertexBuffer* vbo;
	IndexBuffer* ibo;
	std::vector<DrawInstruction> m_drawInstructions;

private:
	Mesh();
};