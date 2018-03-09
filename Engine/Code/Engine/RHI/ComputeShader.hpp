#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Math/UIntVector4.hpp"

//////////////////////////////////////////////////////////////////////////
class RHIDevice;
class Sampler;
class Texture2D;
typedef Texture2D UAVTexture;
struct ID3D11ComputeShader;
class ComputeShader;
class ConstantBuffer;
//////////////////////////////////////////////////////////////////////////
class ComputeJob
{
public:
	ComputeJob();
	~ComputeJob();

	// Sets the shader program to use for this job;
	void set_shader(ComputeShader *cs);
	void set_main_const_buffer(ConstantBuffer* cbuffer);

	// dispach grid - how many groups do we dispatch?
	// These arguments are just forwarded to ID3D11DeviceContext::Dispatch
	// when we run ("dispatch") this job.
	void set_grid_dimensions(uint x, uint y, uint z = 1);

	void set_resource(uint idx, Texture2D* res);
	void set_sampler(uint idx, Sampler* samp);

	// Set a UA resource view (bind points use register(u#) in the sahder)
	// I use "rw" for read/write resource - since I think it reads better than UA (unordered access)
	void set_rw_resource(uint idx, UAVTexture* res);


	// Sets the properties for this program by name
	// This is identical to proper blocks for Materials [optional task that was done
	// in AES].  This is not required for the assignment - but does help make the 
	// system more useable.
	//
	// If you have materials working in your engine - I recommend doing this step.  But since
	// it requires using shader reflection to have it work by property name, I recommend saving it for last if
	// you have not yet done it for a material class.
	//void set_uint(char const* id, const uint& v);
	//void set_float(char const* id, float const &v);
	//void set_vec2(char const* id, const Vector2& v);
	//void set_vec4(char const* id, const Vector4& v);
	//void set_color(char const* id, const Rgba& v);
	//// etc...

public:
	ComputeShader *shader_to_run;

	// the grid/block size use for a dispatch
	// You can store this with the job - or pass it to your
	// RHIDeviceContext::Dispatch call - I just prefer to keep it with 
	// the job.
	UIntVector4 grid_size; //Holding it as UInt4 so I don't have to make a new UInt3 class

	// members: left as an excercise to the reader,
	// plus they change depending on how many features you want to add or how
	// you decide to implement them....
	ConstantBuffer* const_buffer_main;
};
//////////////////////////////////////////////////////////////////////////
class ComputeShader
{
public:
	// Similar to a pipeline shader - needs the device used for creation
	// and the .hlsl file to load.
	ComputeShader(RHIDevice* device);

	// Cleans up used resources
	~ComputeShader();
	void Destroy();

	// Load a compute shader from an *.hlsl file.  Recommend 
	// starting with clear.hlsl
	bool LoadFile(char const *filename,
		char const *entry_point = nullptr,
		char const *defines = nullptr);

public:
	// At bare minimum, a ComputeShader just has the dx resource in it;
	// If you have a ShaderStage pipeline already in place for
	// your rendering pipeline, you could instead have a single ShaderStage 
	// here instead;
	ID3D11ComputeShader *dx_compute_shader;
	RHIDevice* mo_device;
	// Additional information from shader reflection I also usually store on this class.
};