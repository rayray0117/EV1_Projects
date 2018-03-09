#pragma once
//------------------------------------------------------------------------
// RHITypes.h
// If you're not doing the fullscreen optionals - you can leave this file 
// blank for now.
//------------------------------------------------------------------------




namespace RHIEnum {
	enum Capability
	{
		CAPABILITY_BLEND,
		CAPABILITY_DEPTH_TEST,
		CAPABILITY_LINE_SMOOTH,
		CAPABILITY_POLYGON_SMOOTH,
		CAPABILITY_TEXTURE_2D,
		CAPABILITY_CULL_FACE,
		NUM_CAPABILITIES
	};
	enum BlendFactor
	{
		BLEND_ZERO,
		BLEND_ONE,
		BLEND_SOURCE_COLOR,
		BLEND_ONE_MINUS_SOURCE_COLOR,
		BLEND_DESTINATION_COLOR,
		BLEND_ONE_MINUS_DESTINATION_COLOR,
		BLEND_SOURCE_ALPHA,
		BLEND_ONE_MINUS_SOURCE_ALPHA,
		BLEND_DESTINATION_ALPHA,
		BLEND_ONE_MINUS_DESTINATION_ALPHA,
		NUM_BLEND_FACTORS
	};

	enum DepthFunction
	{
		DEPTH_NEVER,
		DEPTH_LESS,
		DEPTH_EQUAL,
		DEPTH_LEQUAL,
		DEPTH_GREATER,
		DEPTH_NOTEQUAL,
		DEPTH_GEQUAL,
		DEPTH_ALWAYS,
		NUM_DEPTH_FUNCTIONS
	};

	enum ImageFormat
	{
		IMAGEFORMAT_RGBA8, // only supporting this one for class - up to you if you want to support more
		IMAGEFORMAT_D24S8,
	};

	enum PrimativeType
	{
		PRIMATIVE_QUADS,
		PRIMATIVE_QUADS_STRIP,
		PRIMATIVE_LINE_LOOP,
		PRIMATIVE_LINE,
		PRIMATIVE_LINE_STRIP,
		PRIMATIVE_TRIANGLE_FAN,
		PRIMATIVE_TRIANGLE,
		NUM_PRIMATE_TYPES
	};


	enum BufferType
	{
		BUFFER_ARRAY,
		NUM_BUFFER_TYPES
	};

	enum UsageType
	{
		USAGE_STATIC_DRAW,
		USAGE_DYNAMIC_DRAW,
		NUM_USAGE_TYPES
	};


	enum MatrixMode
	{
		MATRIX_MODELVIEW,
		MATRIX_PROJECTION,
		MATRIX_VIEW,
		MATRIX_MODEL,
		NUM_MATIX_MODES
	};

	enum FilterMode
	{
		FILTER_POINT,
		FILTER_LINEAR,
		NUM_FILTER_MODES
	};

	// [OPTIONAL] You're only required to be able to create a Renderable Window
	// But, if you would prefer to have Fullscreen Window, or a Dedicated Window
	// this is a good time to add it.
	enum OutputMode
	{
		RENDEROUTPUT_WINDOWED,
		RENDEROUTPUT_BORDERLESS,
		RENDEROUTPUT_FULLSCREEN_WINDOW,     // Requires Window Optional and ability to resize SwapChain
		RENDEROUTPUT_FULLSCREEN_DEDICATED,  // Dedicated Fullscreen Mode.  
	};

	enum CullMode
	{
		BACK_CULLING,
		FRONT_CULLING,
		NO_CULLING,
		NUM_CULLINGS
	};

	enum FillMode
	{
		FILLMODE_SOLID,
		FILLMODE_WIRE,
		NUM_FILLMODES
	};
}