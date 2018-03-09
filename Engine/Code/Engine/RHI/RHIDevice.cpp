#include "Engine/RHI/DX11.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/Sampler.hpp"
//////////////////////////////////////////////////////////////////////////
RHIDevice::RHIDevice()
	: mo_immediate_context(nullptr)
	, dx_device(nullptr)
{
}

RHIDevice::RHIDevice(RHIInstance *owner, ID3D11Device *dxd)
{
	mo_instance = owner;
	dx_device = dxd;
	mo_immediate_context = nullptr;
}

RHIDevice::~RHIDevice()
{
	DX_SAFE_RELEASE(dx_device);
}

//------------------------------------------------------------------------
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

Texture2D* RHIDevice::CreateTextureFromFile(const char* filename)
{
	Texture2D* texture = new Texture2D(this, filename);
	return texture;
}

Texture2D* RHIDevice::CreateTextureFromData(const std::string& name, unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
{
	Texture2D* texture = new Texture2D(this);
	texture->setFromData(imageTexelBytes, width, height, bytesPerTexel);
	texture->m_imageFilePath = name;
	return texture;
}

ShaderProgram* RHIDevice::CreateShaderFromFile_hlsl(char const *filename)
{
	std::vector<unsigned char> buffer;
	
	/*void *src_code = FileReadToBuffer(filename, &src_size);*/
	if (!ReadBufferToFile(buffer, filename)) 
	{
		//ASSERT_RECOVERABLE( true, "File not found. %s", filename );
		return nullptr;
	}

	ID3DBlob *vs_byte_code = nullptr;
	ID3DBlob *fs_byte_code = nullptr;

	vs_byte_code = CompileHLSLToShaderBlob(filename, buffer.data(), buffer.size(), "VertexFunction", "vs_5_0");
	fs_byte_code = CompileHLSLToShaderBlob(filename, buffer.data(), buffer.size(), "FragmentFunction", "ps_5_0");

	if ((vs_byte_code != nullptr) && (fs_byte_code != nullptr)) 
	{

		ID3D11VertexShader *vs = nullptr;
		ID3D11PixelShader *fs = nullptr;

		dx_device->CreateVertexShader(vs_byte_code->GetBufferPointer(),
			vs_byte_code->GetBufferSize(),
			nullptr,
			&vs);

		dx_device->CreatePixelShader(fs_byte_code->GetBufferPointer(),
			fs_byte_code->GetBufferSize(),
			nullptr,
			&fs);

		ShaderProgram *program = new ShaderProgram(this, vs, fs, vs_byte_code, fs_byte_code);
		program->m_filePath = filename;
		return program;
	}

	return nullptr;
}

ShaderProgram* RHIDevice::CreateShaderFromString_hlsl(char const *name, const std::string& buffer)
{
	ID3DBlob *vs_byte_code = nullptr;
	ID3DBlob *fs_byte_code = nullptr;

	vs_byte_code = CompileHLSLToShaderBlob(name, buffer.data(), buffer.size(), "VertexFunction", "vs_5_0");
	fs_byte_code = CompileHLSLToShaderBlob(name, buffer.data(), buffer.size(), "FragmentFunction", "ps_5_0");

	if ((vs_byte_code != nullptr) && (fs_byte_code != nullptr))
	{

		ID3D11VertexShader *vs = nullptr;
		ID3D11PixelShader *fs = nullptr;

		dx_device->CreateVertexShader(vs_byte_code->GetBufferPointer(),
			vs_byte_code->GetBufferSize(),
			nullptr,
			&vs);

		dx_device->CreatePixelShader(fs_byte_code->GetBufferPointer(),
			fs_byte_code->GetBufferSize(),
			nullptr,
			&fs);

		ShaderProgram *program = new ShaderProgram(this, vs, fs, vs_byte_code, fs_byte_code);
		program->m_filePath = name;
		return program;
	}

	return nullptr;
}

VertexBuffer* RHIDevice::CreateVertexBuffer(const Vertex3D* vertices, uint vertex_count, RHIEnum::UsageType usage /*= RHIEnum::USAGE_STATIC_DRAW*/)
{
	VertexBuffer *buffer = new VertexBuffer(this, vertices, vertex_count, usage);
	return buffer;
}

IndexBuffer* RHIDevice::CreateIndexBuffer(const uint *indices, uint const index_count, RHIEnum::UsageType usage)
{
	IndexBuffer* buffer = new IndexBuffer(this, indices, index_count, usage);
	return buffer;
}

RHIOutput* RHIDevice::CreateOutput(Window* window, RHIEnum::OutputMode mode /*= RHIEnum::RENDEROUTPUT_WINDOWED*/)
{
	return new RHIOutput(this, window, mode);
}
