#pragma once
#include "Engine/Core/EngineBase.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class RHIDevice;
class Shader;
class Texture2D;
class ConstantBuffer;
//////////////////////////////////////////////////////////////////////////
class Material
{
public:
	Material(RHIDevice* owner);
	~Material();
	
	void SetShader(Shader* shader);

	void SetTexture(uint bindIndex, Texture2D* resource);
	void SetConstantBuffer(uint bindIndex, ConstantBuffer* buffer);


	inline void SetDiffuse(Texture2D* tex) { SetTexture(0, tex); }
	inline void SetNormal(Texture2D* tex) { SetTexture(1, tex); }
	inline void SetSpecular(Texture2D* tex) { SetTexture(2, tex); }
public:
	RHIDevice* mo_device;

	Shader* mo_shader;
	std::vector<Texture2D*> mo_textures;
	std::vector<ConstantBuffer*> mo_buffers;
};