#include "Engine/RHI/DX11.hpp"

D3D11_PRIMITIVE_TOPOLOGY PrimativeTypeToD3D(RHIEnum::PrimativeType type)
{
	switch (type)
	{

	case RHIEnum::PRIMATIVE_LINE_LOOP:
	case RHIEnum::PRIMATIVE_LINE: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case RHIEnum::PRIMATIVE_LINE_STRIP: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;
	case RHIEnum::PRIMATIVE_TRIANGLE_FAN:
	case RHIEnum::PRIMATIVE_QUADS:
	case RHIEnum::PRIMATIVE_QUADS_STRIP:
	case RHIEnum::PRIMATIVE_TRIANGLE: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case RHIEnum::NUM_PRIMATE_TYPES:
	default: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	}
}



bool PrimativeTypeValid(RHIEnum::PrimativeType type)
{
	switch (type)
	{
	case RHIEnum::PRIMATIVE_QUADS: return false;
		break;
	case RHIEnum::PRIMATIVE_QUADS_STRIP: return false;
		break;
	case RHIEnum::PRIMATIVE_LINE_LOOP: return false;
		break;
	case RHIEnum::PRIMATIVE_LINE: return true;
		break;
	case RHIEnum::PRIMATIVE_LINE_STRIP: return true;
		break;
	case RHIEnum::PRIMATIVE_TRIANGLE_FAN: return false;
		break;
	case RHIEnum::PRIMATIVE_TRIANGLE: return true;
		break;
	default: return false;
		break;
	}
}

bool TypeValid(RHIEnum::PrimativeType type)
{
	return PrimativeTypeValid(type);
}

D3D11_USAGE UsageTypeToD3D(RHIEnum::UsageType type)
{
	switch (type)
	{
	case RHIEnum::USAGE_STATIC_DRAW: return D3D11_USAGE_IMMUTABLE;
		break;
	case RHIEnum::USAGE_DYNAMIC_DRAW: return D3D11_USAGE_DYNAMIC;
		break;
	case RHIEnum::NUM_USAGE_TYPES:
	default: return D3D11_USAGE_DEFAULT;
		break;
	}
}

D3D11_USAGE TypeToD3D(RHIEnum::UsageType type)
{
	return UsageTypeToD3D(type);
}

D3D11_PRIMITIVE_TOPOLOGY TypeToD3D(RHIEnum::PrimativeType type)
{
	return PrimativeTypeToD3D(type);
}

D3D11_BLEND TypeToD3D(RHIEnum::BlendFactor factor)
{
	switch (factor)
	{
	case RHIEnum::BLEND_ZERO: return D3D11_BLEND_ZERO;
		break;
	case RHIEnum::BLEND_ONE: return D3D11_BLEND_ONE;
		break;
	case RHIEnum::BLEND_SOURCE_COLOR: return D3D11_BLEND_SRC_COLOR;
		break;
	case RHIEnum::BLEND_ONE_MINUS_SOURCE_COLOR: return D3D11_BLEND_INV_SRC_COLOR;
		break;
	case RHIEnum::BLEND_DESTINATION_COLOR: return D3D11_BLEND_DEST_COLOR;
		break;
	case RHIEnum::BLEND_ONE_MINUS_DESTINATION_COLOR: return D3D11_BLEND_INV_DEST_COLOR;
		break;
	case RHIEnum::BLEND_SOURCE_ALPHA: return D3D11_BLEND_SRC_ALPHA;
		break;
	case RHIEnum::BLEND_ONE_MINUS_SOURCE_ALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
		break;
	case RHIEnum::BLEND_DESTINATION_ALPHA: return D3D11_BLEND_DEST_ALPHA;
		break;
	case RHIEnum::BLEND_ONE_MINUS_DESTINATION_ALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
		break;
	case RHIEnum::NUM_BLEND_FACTORS:
	default:  return D3D11_BLEND_ZERO;
		break;
	}
}



D3D11_FILTER FilterModeToD3D(RHIEnum::FilterMode min_filter, RHIEnum::FilterMode mag_filter)
{
	if (min_filter == RHIEnum::FILTER_POINT)
	{
		if (mag_filter == RHIEnum::FILTER_POINT)
		{
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
		else
		{
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}
	}
	else
	{
		if (mag_filter == RHIEnum::FILTER_POINT)
		{
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		}
		else
		{
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
	}
}

D3D11_FILTER TypeToD3D(RHIEnum::FilterMode min_filter, RHIEnum::FilterMode mag_filter)
{
	if (min_filter == RHIEnum::FILTER_POINT)
	{
		if (mag_filter == RHIEnum::FILTER_POINT)
		{
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
		else
		{
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}
	}
	else
	{
		if (mag_filter == RHIEnum::FILTER_POINT)
		{
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		}
		else
		{
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
	}
}



D3D11_CULL_MODE CullModeToD3D(RHIEnum::CullMode cull)
{
	switch (cull)
	{
	case RHIEnum::BACK_CULLING: return D3D11_CULL_BACK;
		break;
	case RHIEnum::FRONT_CULLING: return D3D11_CULL_FRONT;
		break;
	case RHIEnum::NO_CULLING: return D3D11_CULL_NONE;
		break;
	case RHIEnum::NUM_CULLINGS:
	default: return D3D11_CULL_NONE;
		break;
	}
}
D3D11_CULL_MODE TypeToD3D(RHIEnum::CullMode cull)
{
	return CullModeToD3D(cull);
}

D3D11_FILL_MODE FillModeToD3D(RHIEnum::FillMode fill)
{
	switch (fill)
	{
	case RHIEnum::FILLMODE_SOLID: return D3D11_FILL_SOLID;
		break;
	case RHIEnum::FILLMODE_WIRE: return D3D11_FILL_WIREFRAME;
		break;
	case RHIEnum::NUM_FILLMODES:
	default: return D3D11_FILL_SOLID;
		break;
	}
}
D3D11_FILL_MODE TypeToD3D(RHIEnum::FillMode fill)
{
	return FillModeToD3D(fill);
}

D3D11_COMPARISON_FUNC DepthFuncToD3D(RHIEnum::DepthFunction depthFunc)
{
	switch (depthFunc)
	{
	case RHIEnum::DEPTH_NEVER: return D3D11_COMPARISON_NEVER;
		break;
	case RHIEnum::DEPTH_LESS: return D3D11_COMPARISON_LESS;
		break;
	case RHIEnum::DEPTH_EQUAL: return D3D11_COMPARISON_EQUAL;
		break;
	case RHIEnum::DEPTH_LEQUAL: return D3D11_COMPARISON_LESS_EQUAL;
		break;
	case RHIEnum::DEPTH_GREATER: return D3D11_COMPARISON_GREATER;
		break;
	case RHIEnum::DEPTH_NOTEQUAL: return D3D11_COMPARISON_NOT_EQUAL;
		break;
	case RHIEnum::DEPTH_GEQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
		break;
	case RHIEnum::DEPTH_ALWAYS: return D3D11_COMPARISON_ALWAYS;
		break;
	case RHIEnum::NUM_DEPTH_FUNCTIONS:
	default: return D3D11_COMPARISON_LESS;
		break;
	}
}

D3D11_COMPARISON_FUNC TypeToD3D(RHIEnum::DepthFunction depthFunc)
{
	return DepthFuncToD3D(depthFunc);
}