#pragma once
#include "Engine/RHI/RHITypes.hpp"
//////////////////////////////////////////////////////////////////////////
struct DrawInstruction
{
	unsigned int start_index;
	unsigned int start_vertex;
	unsigned int vertex_count;
	RHIEnum::PrimativeType primative;
	bool useIbo;
};