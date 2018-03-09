#pragma once
#include <string>
//////////////////////////////////////////////////////////////////////////
class RHIDevice;

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D10Blob;
struct ID3D11InputLayout;
//////////////////////////////////////////////////////////////////////////
enum eShaderStage
{
	SHADER_VERTEX,    // maps to target "vs_5_0", or Vertex Shader Model 5 (currently latest)
	SHADER_FRAGMENT,  // maps to target "ps_5_0", or Pixel Shader Model 5 (currently latest)
};
//////////////////////////////////////////////////////////////////////////
// Technically a ShaderProgram is just the program running on the GPU (or multiple
// programs as part of a pipeline).
class ShaderProgram
{
	friend class RHIDevice;
	friend class RHIDeviceContext;
	friend class SimpleRenderer;
public:
	~ShaderProgram();
private:
	ShaderProgram(RHIDevice* device, ID3D11VertexShader* vs, ID3D11PixelShader* fs, ID3D10Blob* vs_bytecode, ID3D10Blob* fs_bytecode);

	// feel free to add in more methods to help with the creation process if you want.
	void CreateInputLayout(RHIDevice* device);

	inline bool isValid() const { return (dx_vertex_shader != nullptr) && (dx_fragment_shader != nullptr); }

private:
	// All the steps to this (could be split out to a ShaderStage)
	ID3D11VertexShader* dx_vertex_shader;
	ID3D11PixelShader* dx_fragment_shader;

	// Input Layout - for now, we're going 
	// to assume every shader makes their own
	// for simplicity, but honestly you should make these
	// based on need
	ID3D11InputLayout* dx_input_layout;

	// [OPTIONAL] ByteCode - only need to keep it around if using Reflection
	ID3D10Blob* vs_byte_code;
	ID3D10Blob* fs_byte_code;


	std::string m_filePath;
};