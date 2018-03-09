#include "Engine/RHI/ComputeShader.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/DX11.hpp"
#include <vector>
#include "Engine/Core/FileUtils.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
//////////////////////////////////////////////////////////////////////////
static ID3DBlob* CompileHLSLToShaderBlob(char const *opt_filename,  // optional: used for error messages
	void const *source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const *entrypoint,                                           // Name of the Function we treat as the entry point for this stage
	char const* target);                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)
//////////////////////////////////////////////////////////////////////////
ComputeJob::ComputeJob()
{
	/* DO NOTHING */
}

ComputeJob::~ComputeJob()
{
	SAFE_DELETE(shader_to_run);
	SAFE_DELETE(const_buffer_main);
}

void ComputeJob::set_shader(ComputeShader *cs)
{
	shader_to_run = cs;
}

void ComputeJob::set_main_const_buffer(ConstantBuffer* cbuffer)
{
	const_buffer_main = cbuffer;
}

void ComputeJob::set_grid_dimensions(uint x, uint y, uint z /*= 1*/)
{
	grid_size.x = x;
	grid_size.y = y;
	grid_size.z = z;
}

void ComputeJob::set_resource(uint /*idx*/, Texture2D* /*res*/)
{
	//ToDo
}

void ComputeJob::set_sampler(uint /*idx*/, Sampler* /*samp*/)
{
	//ToDo
}

void ComputeJob::set_rw_resource(uint /*idx*/, UAVTexture* /*res*/)
{
	//ToDo
}
//////////////////////////////////////////////////////////////////////////
ComputeShader::ComputeShader(RHIDevice* device)
	: mo_device(device)
{

}

ComputeShader::~ComputeShader()
{
	Destroy();
}

void ComputeShader::Destroy()
{
	DX_SAFE_RELEASE(dx_compute_shader);
}

bool ComputeShader::LoadFile(char const *filename, char const *entry_point /*= nullptr*/, char const *defines /*= nullptr*/)
{
	std::vector<unsigned char> buffer;

	/*void *src_code = FileReadToBuffer(filename, &src_size);*/
	if (!ReadBufferToFile(buffer, filename))
	{
		//ASSERT_RECOVERABLE( true, "File not found. %s", filename );
		return false;
	}

	ID3DBlob *cs_byte_code = nullptr;

	if (entry_point == nullptr)
		entry_point = "ComputeFunction";
	if (defines == nullptr)
		defines = "cs_5_0";

	cs_byte_code = CompileHLSLToShaderBlob(filename, buffer.data(), buffer.size(), entry_point, defines);

	if ((cs_byte_code != nullptr))
	{
		mo_device->dx_device->CreateComputeShader(cs_byte_code->GetBufferPointer(), cs_byte_code->GetBufferSize(), nullptr, &dx_compute_shader);
		return true;
	}

	return false;
}
////////////////////////////////////////////////////////////////////////// //////////////////////////////////////////////////////////////////////////

static ID3DBlob* CompileHLSLToShaderBlob(char const *opt_filename,  // optional: used for error messages
	void const *source_code,                                          // buffer containing source code.
	size_t const source_code_size,                                    // size of the above buffer.
	char const *entrypoint,                                           // Name of the Function we treat as the entry point for this stage
	char const* target)                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)
{
	/* DEFINE MACROS - CONTROLS SHADER
	// You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
	D3D_SHADER_MACRO defines[1];
	defines[0].Name = "TEST_MACRO";
	defines[0].Definition = nullptr;
	*/

	DWORD compile_flags = 0U;
#if defined(DEBUG_SHADERS)
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

	ID3DBlob *code = nullptr;
	ID3DBlob *errors = nullptr;

	HRESULT hr = ::D3DCompile(source_code,
		source_code_size,                   // plain text source code
		opt_filename,                       // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
		nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
		entrypoint,                         // Entry Point for this shader
		target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
		compile_flags,                      // Flags that control compilation
		0,                                  // Effect Flags (we will not be doing Effect Files)
		&code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
		&errors);                          // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr)) {
		if (errors != nullptr) {
			char *error_string = (char*)errors->GetBufferPointer();
			DebuggerPrintf("Failed to compile [%s].  Compiler gave the following output;\n%s",
				opt_filename,
				error_string);
			DX_SAFE_RELEASE(errors);
		}
	}

	// will be nullptr if it failed to compile
	return code;
}