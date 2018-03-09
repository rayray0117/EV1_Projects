#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/RHI.hpp"
#include "Engine/RHI/Texture2D.hpp"
//#include "Engine/Core/EngineConfig.hpp"

//////////////////////////////////////////////////////////////////////////
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderableString.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/SkeletonInstance.hpp"
//
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/RHI/RasterState.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/RHI/BlendState.hpp"
#include "Engine/Renderer/Font.hpp"
#include "Engine/RHI/DepthStencilState.hpp"
#include "StructuredBuffer.hpp"
#include "../Core/Profiler.hpp"
//////////////////////////////////////////////////////////////////////////
SimpleRenderer::SimpleRenderer()
	: device(nullptr)
	, context(nullptr)
	, output(nullptr)
	, mo_currentRenderTarget(nullptr)
	, matrix_cb(nullptr)
	, time_cb(nullptr)
	, m_genericIBO(nullptr)
	, m_genericVBO(nullptr)
	, m_currentBlendState(nullptr)
	, m_defaultRasterState(nullptr)
	, m_depthStencilDesc(nullptr)
	, m_depthStencilState(nullptr)
	, m_defaultDepthStencil(nullptr)
	, mo_currentDepthStencil(nullptr)
	, m_matrixMode(RHIEnum::MATRIX_PROJECTION)
	, m_genericMesh(nullptr)
	, m_genericMeshBuilder(nullptr)
	, m_sampler(nullptr)
{
	m_projectionMatrixStack.make(Matrix4(Vector3::XAXIS, Vector3::YAXIS, -Vector3::ZAXIS));
	m_vertexBufferStorage.push_back(nullptr);

	matrixData.projection = m_projectionMatrixStack.back();
	matrixData.model = m_modelMatrixStack.back();
	matrixData.view = m_viewMatrixStack.back();
}

SimpleRenderer::~SimpleRenderer()
{
	destroy();
}

////////////////////////////////////////////////////////////////////////// Renderer //////////////////////////////////////////////////////////////////////////
void SimpleRenderer::ClearScreen(const Rgba& clearColor)
{
	context->ClearRenderTarget(mo_currentRenderTarget, clearColor);
}

void SimpleRenderer::ClearScreen(const Rgba& clearColor,Texture2D* renderTarget)
{
	context->ClearRenderTarget(renderTarget, clearColor);
}

void SimpleRenderer::ClearDepth(float depth, unsigned int stencil)
{
	context->ClearDepthStencil(mo_currentDepthStencil, depth, stencil);
}

void SimpleRenderer::EnableCapability(RHIEnum::Capability cap)
{
	ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");

	UNUSED(cap);
}

void SimpleRenderer::DisableCapability(RHIEnum::Capability cap)
{
	ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");

	UNUSED(cap);
}

void SimpleRenderer::EnableBackFaceCulling(bool isNowBackFaceCulling /*= true*/)
{
	if (isNowBackFaceCulling && m_defaultRasterState->m_cull != RHIEnum::BACK_CULLING)
	{
		RasterState* temp = new RasterState(device, RHIEnum::BACK_CULLING, m_defaultRasterState->m_fill);
		SAFE_DELETE(m_defaultRasterState);
		m_defaultRasterState = temp;
		SetRasterState(m_defaultRasterState);
	}
	else if(!isNowBackFaceCulling && m_defaultRasterState->m_cull == RHIEnum::BACK_CULLING)
	{
		RasterState* temp = new RasterState(device, RHIEnum::NO_CULLING, m_defaultRasterState->m_fill);
		SAFE_DELETE(m_defaultRasterState);
		m_defaultRasterState = temp;
		SetRasterState(m_defaultRasterState);
	}
}

void SimpleRenderer::EnableWireframeCulling(bool isNowWireFrame /*= true*/)
{
	if (isNowWireFrame && m_defaultRasterState->m_fill != RHIEnum::FILLMODE_WIRE)
	{
		RasterState* temp = new RasterState(device, m_defaultRasterState->m_cull, RHIEnum::FILLMODE_WIRE);
		SAFE_DELETE(m_defaultRasterState);
		m_defaultRasterState = temp;
		SetRasterState(m_defaultRasterState);
	}
	else if (!isNowWireFrame && m_defaultRasterState->m_fill == RHIEnum::FILLMODE_WIRE)
	{
		RasterState* temp = new RasterState(device, m_defaultRasterState->m_cull, RHIEnum::FILLMODE_SOLID);
		SAFE_DELETE(m_defaultRasterState);
		m_defaultRasterState = temp;
		SetRasterState(m_defaultRasterState);
	}
}

void SimpleRenderer::EnableDepthTesting(bool isNowDepthTesting /*= true*/)
{
	if (m_depthStencilDesc->depthTestEnabled != isNowDepthTesting)
	{
		m_depthStencilDesc->depthTestEnabled = isNowDepthTesting;
		SAFE_DELETE(m_depthStencilState);
		m_depthStencilState = new DepthStencilState(device, *m_depthStencilDesc);
		SetDepthStencilState(m_depthStencilState);
	}
}

void SimpleRenderer::EnableDepthWriting(bool isNowDepthWriting /*= true*/)
{
	if (m_depthStencilDesc->depthWritingEnabled != isNowDepthWriting)
	{
		m_depthStencilDesc->depthWritingEnabled = isNowDepthWriting;
		SAFE_DELETE(m_depthStencilState);
		m_depthStencilState = new DepthStencilState(device, *m_depthStencilDesc);
		SetDepthStencilState(m_depthStencilState);
	}
}

void SimpleRenderer::EnableDepth(bool useDepth /*= true*/)
{
	EnableDepthTesting(useDepth);
	EnableDepthWriting(useDepth);
}

void SimpleRenderer::DisableBlendState()
{
	if (!m_blendState.enabled)
		return;

	SAFE_DELETE(m_currentBlendState);
	BlendState *bs = new BlendState(device, false);
	context->SetBlendState(bs);

	m_blendState.enabled = false;

	m_currentBlendState = bs;
}

void SimpleRenderer::EnableLight(uint lightNumber, light_struct light)
{
	lightData.lights[lightNumber] = light;
	light_cb->update(context, &lightData);
}

void SimpleRenderer::EnablePointLight(const Vector3& position, const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& specAttenuation, uint lightNum)
{
	light_struct light;


	light.light_position = Vector4(position, 1.f);
	light.light_color = Vector4(color, intensity);
	light.attenuation = attenuation;
	light.angle = M_PI;
	light.spec_attenuation = specAttenuation;

	EnableLight(lightNum, light);
}

void SimpleRenderer::EnableDirectionalLight(const Vector3& direction, const Rgba& color, uint lightNum /*= 0*/)
{
	light_struct light;

	light.light_position = -direction*5000000.f;
	light.light_color = Vector4(color, 1.0f);
	light.light_direction = Vector4(direction, 0.f);
	light.attenuation = Vector3(1.f, 0.f, 0.f);
	light.angle = M_PI_2;
	light.spec_attenuation = light.attenuation;

	EnableLight(lightNum, light);
}

void SimpleRenderer::EnableSpotLight(const Vector3& position, const Vector3& direction, float angle, const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& specAttenuation /*= Vector3(0.f, 0.f, 1.f)*/, uint lightNum /*= 0*/)
{
	light_struct light;

	light.light_position = position;
	light.light_color = Vector4(color, intensity);
	light.light_direction = Vector4(direction, 0.f);
	light.attenuation = attenuation;
	light.angle = convertDegreesToRadians(angle);
	light.spec_attenuation = specAttenuation;

	EnableLight(lightNum, light);
}

void SimpleRenderer::BindTexture(const Texture2D* texture, uint index)
{
	if (texture)
	{
		context->SetTexture(index, texture);
	}
	else
	{
		context->SetTexture(index, m_plainWhiteTexture);
	}
}

void SimpleRenderer::BindTexture(const std::string& textureName, uint index /*= 0*/)
{
	Texture2D* texture = CreateOrGetTexture(textureName);
	if (texture)
	{
		context->SetTexture(index, texture);
	}
}

void SimpleRenderer::BindShader(ShaderProgram* shader)
{
	if (shader)
	{
		context->SetShaderProgram(shader);
	}
	else
	{
		context->SetShaderProgram(CreateOrGetShader("default")); //#ToDo: See if I ever would actually want to set my shader to null.
	}
}

void SimpleRenderer::BindShader(const std::string& shaderName)
{
	ShaderProgram* shader = CreateOrGetShader(shaderName);
	if (shader)
	{
		context->SetShaderProgram(shader);
	}
}

void SimpleRenderer::BindBuffer(unsigned int bufferId, RHIEnum::BufferType bufferType /*= D3DEnum::BUFFER_ARRAY*/)
{
	ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");
	
	UNUSED(bufferId);
	UNUSED(bufferType);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::SetOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight)
{
	LoadMatrix(Matrix4());
	Matrix4 proj = Matrix4::CreateOrthoProjection(bottomLeft, topRight, -1.f, 1.f);
	m_projectionMatrixStack.transform(proj);
	SetProjectionMatrix(m_projectionMatrixStack.back());
}

void SimpleRenderer::SetPerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float zNear, float zFar)
{
	LoadMatrix(Matrix4());
	Matrix4 proj = Matrix4::CreatePerspectiveProjection(fovVerticalDegrees, aspectRatio, zNear, zFar);
	m_projectionMatrixStack.transform(proj);
	SetProjectionMatrix(m_projectionMatrixStack.back());
}

void SimpleRenderer::SetViewport(int x, int y, int width, int height)
{
	context->SetViewport(x, y, width, height);
}

void SimpleRenderer::SetLineWidth(float lineThickness)
{
	//ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");

	UNUSED(lineThickness);
}

void SimpleRenderer::SetColor(const Rgba& color)
{
	ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");

	UNUSED(color);
}

void SimpleRenderer::SetBlendFunc(RHIEnum::BlendFactor sourceFactor, RHIEnum::BlendFactor destinationFactor)
{
	//ToDo: cleanup later
	SAFE_DELETE(m_currentBlendState);
	BlendState* bs = new BlendState(device, sourceFactor, destinationFactor);
	context->SetBlendState(bs);

	m_blendState.enabled = true;
	m_blendState.src_factor = sourceFactor;
	m_blendState.dest_factor = destinationFactor;

	m_currentBlendState = bs;
}

void SimpleRenderer::SetDepthFunc(RHIEnum::DepthFunction func)
{
	if (m_depthStencilDesc->depthFunction != func)
	{
		m_depthStencilDesc->depthFunction = func;
		SAFE_DELETE(m_depthStencilState);
		m_depthStencilState = new DepthStencilState(device, *m_depthStencilDesc);
		SetDepthStencilState(m_depthStencilState);
	}
}

void SimpleRenderer::SetSampler(const Sampler* sampler, unsigned int index /*= 0*/)
{
	context->SetSampler(index, sampler);
}

void SimpleRenderer::SetSampler(RHIEnum::FilterMode min, RHIEnum::FilterMode max, uint index /*= 0*/)
{
	// Figure out if this will cause issues since it may be referenced for other indexes
	SAFE_DELETE(m_sampler);
	m_sampler = new Sampler(device, min, max);
	SetSampler(m_sampler, index);
}

void SimpleRenderer::SetConstantBuffer(const ConstantBuffer* constBuffer, unsigned int index)
{
	context->SetConstantBuffer(index, constBuffer);
}

void SimpleRenderer::SetStructuredBuffer(const StructuredBuffer* structBuffer, uint index /*= 5*/)
{
	context->SetStructuredBuffer(index, structBuffer);
}

void SimpleRenderer::SetRenderTarget(Texture2D* renderTarget, Texture2D* depthStencil)
{
	if (renderTarget != nullptr) 
	{
		mo_currentRenderTarget = renderTarget;
	}
	else 
	{
		mo_currentRenderTarget = output->getRenderTarget();
	}

	if (depthStencil == nullptr)
	{
		depthStencil = m_defaultDepthStencil;
	}

	mo_currentDepthStencil = depthStencil;
	context->SetRenderTarget(mo_currentRenderTarget, depthStencil);
}

void SimpleRenderer::SetDepthStencilState(DepthStencilState* depthStencilState)
{
	context->SetDepthStencilState(depthStencilState);
}

void SimpleRenderer::SetRasterState(RasterState* rasterState /*= nullptr*/)
{
	if (rasterState)
	{
		context->SetRasterState(rasterState);
	}
	else
	{
		context->SetRasterState(m_defaultRasterState);
	}
}

void SimpleRenderer::SetAmbientLight(float intensity, const Rgba& color /*= Rgba::WHITE*/)
{
	lightData.ambient = Vector4(color);
	lightData.ambient.w = intensity;
	light_cb->update(context, &lightData);
}

void SimpleRenderer::SetSpecularConstants(float specPower, float specFactor)
{
	lightData.spec_factor = specFactor;
	lightData.spec_power = specPower;
	light_cb->update(context, &lightData);
}

void SimpleRenderer::SetEyePosition(const Vector3& pos)
{
	lightData.eye_position = Vector4(pos.x, pos.y, pos.z, 1.0f);
	light_cb->update(context, &lightData);
}

void SimpleRenderer::LoadMatrix(const Matrix4& matrix)
{
	switch (m_matrixMode)
	{
	case RHIEnum::MATRIX_MODELVIEW:
		break;
	case RHIEnum::MATRIX_PROJECTION: m_projectionMatrixStack.make(matrix);
		break;
	case RHIEnum::MATRIX_VIEW: m_viewMatrixStack.make(matrix);
		break;
	case RHIEnum::MATRIX_MODEL: m_modelMatrixStack.make(matrix);
		break;
	case RHIEnum::NUM_MATIX_MODES:
		break;
	default:
		break;
	}
	UpdateMatrixBuffer();
}

void SimpleRenderer::SetProjectionMatrix(const Matrix4& projection)
{
	matrixData.projection = projection.getTranpose();
	matrix_cb->update(context, &matrixData);
}

void SimpleRenderer::SetViewMatrix(const Matrix4& view)
{
	matrixData.view = view.getTranpose();
	matrix_cb->update(context, &matrixData);
}

void SimpleRenderer::SetModelMatrix(const Matrix4& model)
{
	matrixData.model = model.getTranpose();
	matrix_cb->update(context, &matrixData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SimpleRenderer::SetBufferData(const void * data, int size, RHIEnum::UsageType usageDrawHint, RHIEnum::BufferType bufferType /*= D3DEnum::BUFFER_ARRAY*/)
{
	ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");

	UNUSED(data);
	UNUSED(size);
	UNUSED(usageDrawHint);
	UNUSED(bufferType);
}

void SimpleRenderer::SetVBO3DData(VertexBuffer* vboId, const Vertex3D* vertexArray, int numVertexes, RHIEnum::UsageType usageDrawHint, RHIEnum::BufferType bufferType /*= D3DEnum::BUFFER_ARRAY*/)
{
	vboId->setBufferData(context, vertexArray, numVertexes);
	UNUSED(bufferType);
	UNUSED(usageDrawHint);
}

void SimpleRenderer::SetVBO3DData(unsigned int vboId, const Vertex3D* vertexArray, int numVertexes, RHIEnum::UsageType usageDrawHint /*= RHIEnum::USAGE_DYNAMIC_DRAW*/, RHIEnum::BufferType bufferType /*= RHIEnum::BUFFER_ARRAY*/)
{
	if (m_vertexBufferStorage[vboId])
	{
		if (m_vertexBufferStorage[vboId]->usage_type == RHIEnum::USAGE_DYNAMIC_DRAW)
		{
			m_vertexBufferStorage[vboId]->setBufferData(context, vertexArray, numVertexes);
		}
		else
		{
			SAFE_DELETE(m_vertexBufferStorage[vboId]);
			m_vertexBufferStorage[vboId] = device->CreateVertexBuffer(vertexArray, numVertexes, usageDrawHint);
			UNUSED(bufferType);
		}
	}
	else
	{
		m_vertexBufferStorage[vboId] = device->CreateVertexBuffer(vertexArray, numVertexes, usageDrawHint);
	}
}

void SimpleRenderer::SetIBO3DData(IndexBuffer* iboId, const unsigned int* indexArray, int numIndexes, RHIEnum::UsageType usageDrawHint /*= RHIEnum::USAGE_DYNAMIC_DRAW*/, RHIEnum::BufferType bufferType /*= RHIEnum::BUFFER_ARRAY*/)
{
	iboId->setBufferData(context, indexArray, numIndexes);
	UNUSED(bufferType);
	UNUSED(usageDrawHint);
}

void SimpleRenderer::SetMeshData(unsigned int meshId, MeshBuilder* mb, RHIEnum::UsageType /*usageDrawHint*/ /*= RHIEnum::USAGE_STATIC_DRAW*/)
{
	if (mb == nullptr)
		return;

	SAFE_DELETE(m_meshStorage[meshId]);
	m_meshStorage[meshId] = CreateAndGetMeshStatic(mb);
}

void SimpleRenderer::GenerateBuffers(unsigned int* buffers, int numberOfBuffers)
{
	ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");

	UNUSED(buffers);
	UNUSED(numberOfBuffers);
}

void SimpleRenderer::DeleteBuffers(unsigned int* buffers, int numberOfBuffers)
{
	ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");

	UNUSED(buffers);
	UNUSED(numberOfBuffers);
}

void SimpleRenderer::DeleteVBO(VertexBuffer* vboId)
{
	SAFE_DELETE(vboId);
}

void SimpleRenderer::DeleteVBO(unsigned int vboId)
{
	SAFE_DELETE(m_vertexBufferStorage[vboId]);
}

void SimpleRenderer::DeleteIBO(IndexBuffer* iboId)
{
	SAFE_DELETE(iboId);
}

//////////////////////////////////////////////////////////////////////////
void SimpleRenderer::pushMatrix()
{
	switch (m_matrixMode)
	{
	case RHIEnum::MATRIX_MODELVIEW:
		break;
	case RHIEnum::MATRIX_PROJECTION: m_projectionMatrixStack.push();
		break;
	case RHIEnum::MATRIX_VIEW: m_viewMatrixStack.push();
		break;
	case RHIEnum::MATRIX_MODEL: m_modelMatrixStack.push();
		break;
	case RHIEnum::NUM_MATIX_MODES:
		break;
	default:
		break;
	}
}

void SimpleRenderer::popMatrix()
{
	switch (m_matrixMode)
	{
	case RHIEnum::MATRIX_MODELVIEW:
		break;
	case RHIEnum::MATRIX_PROJECTION: 
		m_projectionMatrixStack.pop();
		break;
	case RHIEnum::MATRIX_VIEW: 
		m_viewMatrixStack.pop();
		break;
	case RHIEnum::MATRIX_MODEL: 
		m_modelMatrixStack.pop();
		break;
	case RHIEnum::NUM_MATIX_MODES:
		break;
	default:
		break;
	}

	UpdateMatrixBuffer();
}

void SimpleRenderer::translate2D(const Vector2& pos)
{
	translate2D(pos.x, pos.y);
}

void SimpleRenderer::translate2D(float x, float y)
{
	translate3D(x, y, 0.f);
}

void SimpleRenderer::translate3D(const Vector3& pos)
{
	translate3D(pos.x, pos.y, pos.z);
}

void SimpleRenderer::translate3D(float x, float y, float z)
{
	Matrix4 translateMatrix = Matrix4::CreateTranslation(Vector3(x, y, z));
	switch (m_matrixMode)
	{
	case RHIEnum::MATRIX_MODELVIEW: 
		break;
	case RHIEnum::MATRIX_PROJECTION: m_projectionMatrixStack.transform(translateMatrix);
		break;
	case RHIEnum::MATRIX_VIEW: m_viewMatrixStack.transform(translateMatrix);
		break;
	case RHIEnum::MATRIX_MODEL: m_modelMatrixStack.transform(translateMatrix);
		break;
	case RHIEnum::NUM_MATIX_MODES:
		break;
	default:
		break;
	}

	UpdateMatrixBuffer();
}

void SimpleRenderer::rotate2D(float degrees)
{
	rotate3D(degrees, 0.f, 0.f, 1.f);
}

void SimpleRenderer::rotate3D(float degrees, const Vector3& rotationAxis)
{
	rotate3D(degrees, rotationAxis.x, rotationAxis.y, rotationAxis.z);
}

void SimpleRenderer::rotate3D(float degrees, float x, float y, float z)
{
	Matrix4 rotationMatrix;
	if (x != 0.f)
	{
		rotationMatrix = Matrix4::CreateRotationDegreesAboutX(degrees);
	}
	else if (y != 0.f)
	{
		rotationMatrix = Matrix4::CreateRotationDegreesAboutY(degrees);
	}
	else if (z != 0.f)
	{
		rotationMatrix = Matrix4::CreateRotationDegreesAboutZ(degrees);
	}
	else
	{
		return;
	}

	switch (m_matrixMode)
	{
	case RHIEnum::MATRIX_MODELVIEW:
		break;
	case RHIEnum::MATRIX_PROJECTION: m_projectionMatrixStack.transform(rotationMatrix);
		break;
	case RHIEnum::MATRIX_VIEW: m_viewMatrixStack.transform(rotationMatrix);
		break;
	case RHIEnum::MATRIX_MODEL: m_modelMatrixStack.transform(rotationMatrix);
		break;
	case RHIEnum::NUM_MATIX_MODES:
		break;
	default:
		break;
	}

	UpdateMatrixBuffer();
}

void SimpleRenderer::rotate3D(const Quaternion& rotation)
{
	Matrix4 rotationMatrix = rotation.getAsMatrix();

	switch (m_matrixMode)
	{
	case RHIEnum::MATRIX_MODELVIEW:
		break;
	case RHIEnum::MATRIX_PROJECTION: m_projectionMatrixStack.transform(rotationMatrix);
		break;
	case RHIEnum::MATRIX_VIEW: m_viewMatrixStack.transform(rotationMatrix);
		break;
	case RHIEnum::MATRIX_MODEL: m_modelMatrixStack.transform(rotationMatrix);
		break;
	case RHIEnum::NUM_MATIX_MODES:
		break;
	default:
		break;
	}

	UpdateMatrixBuffer();
}

void SimpleRenderer::scale2DUniform(float scale)
{
	scale3DUniform(scale);
}

void SimpleRenderer::scale3DUniform(float scale)
{
	Vector3 scales = Vector3::ONE * scale;
	scale3D(scales);
}

void SimpleRenderer::scale3D(const Vector3& scales)
{
	Matrix4 scaleMatrix = Matrix4::CreateScale(scales);
	switch (m_matrixMode)
	{
	case RHIEnum::MATRIX_MODELVIEW:
		break;
	case RHIEnum::MATRIX_PROJECTION: m_projectionMatrixStack.transform(scaleMatrix);
		break;
	case RHIEnum::MATRIX_VIEW: m_viewMatrixStack.transform(scaleMatrix);
		break;
	case RHIEnum::MATRIX_MODEL: m_modelMatrixStack.transform(scaleMatrix);
		break;
	case RHIEnum::NUM_MATIX_MODES:
		break;
	default:
		break;
	}

	UpdateMatrixBuffer();
}

void SimpleRenderer::transform(const Matrix4& transformMatrix)
{
	switch (m_matrixMode)
	{
	case RHIEnum::MATRIX_MODELVIEW:
		break;
	case RHIEnum::MATRIX_PROJECTION: m_projectionMatrixStack.transform(transformMatrix);
		break;
	case RHIEnum::MATRIX_VIEW: m_viewMatrixStack.transform(transformMatrix);
		break;
	case RHIEnum::MATRIX_MODEL: m_modelMatrixStack.transform(transformMatrix);
		break;
	case RHIEnum::NUM_MATIX_MODES:
		break;
	default:
		break;
	}
	UpdateMatrixBuffer();
}

void SimpleRenderer::rotateAroundPosition2D(const Vector2& position, float degrees)
{
	translate2D(position);
	rotate2D(degrees);
	translate2D(position*-1.f);
}
//////////////////////////////////////////////////////////////////////////
void SimpleRenderer::drawVertexArray3D(const Vertex3D* vertexArray, int numVertexes, RHIEnum::PrimativeType type)
{
	ERROR_RECOVERABLE("FUNCTION UNFINISHED: Not able to convert from openGL to DX11 yet!");
	UNUSED(vertexArray);
	UNUSED(numVertexes);
	UNUSED(type);
}

void SimpleRenderer::drawVBO3D(VertexBuffer* vboID, int numVertexes, RHIEnum::PrimativeType type)
{
	drawVBO3D(vboID, numVertexes, type, 0);
}

void SimpleRenderer::drawVBO3D(VertexBuffer* vboID, int numVertexes, RHIEnum::PrimativeType type, int first)
{
	std::vector<uint> indexes = getPrimativeIndexList(type, numVertexes);
	if (indexes.empty())
	{
		context->draw(type, vboID, numVertexes, first);
	}
	else
	{
		m_genericIBO->setBufferData(context, indexes.data(), indexes.size());
		drawIBO3D(vboID, m_genericIBO, indexes.size(), type, first);
	}
}

void SimpleRenderer::drawVBO3D(unsigned int vboID, int numVertexes, RHIEnum::PrimativeType type)
{
	drawVBO3D(vboID, numVertexes, type, 0);
}

void SimpleRenderer::drawVBO3D(unsigned int vboID, int numVertexes, RHIEnum::PrimativeType type, int first)
{
	drawVBO3D(m_vertexBufferStorage[vboID], numVertexes, type, first);
}

void SimpleRenderer::drawIBO3D(VertexBuffer* vboID, IndexBuffer* iboId, int numIndexes, RHIEnum::PrimativeType type)
{
	drawIBO3D(vboID, iboId, numIndexes, type, 0);
}

void SimpleRenderer::drawIBO3D(VertexBuffer* vboID, IndexBuffer* iboId, int numIndexes, RHIEnum::PrimativeType type, int startVertex, uint startIndex)
{
	context->draw_indexed(type, vboID, iboId, numIndexes, startIndex, startVertex);
}

void SimpleRenderer::drawMesh(Mesh* mesh)
{
	//PROFILE_SCOPE_FUNCTION();
	for each (DrawInstruction drawInstruct in mesh->m_drawInstructions)
	{
		if (drawInstruct.useIbo)
		{
			drawIBO3D(mesh->vbo, mesh->ibo, drawInstruct.vertex_count, drawInstruct.primative, drawInstruct.start_vertex, drawInstruct.start_index);
		}
		else
		{
			drawVBO3D(mesh->vbo, drawInstruct.vertex_count, drawInstruct.primative, drawInstruct.start_vertex);
		}
	}
}

void SimpleRenderer::drawMesh(unsigned int meshID)
{
	drawMesh(m_meshStorage[meshID]);
}

//////////////////////////////////////////////////////////////////////////
Texture2D* SimpleRenderer::CreateOrGetTexture(const std::string& imageFilePath)
{
	// Try to find that texture from those already loaded
	Texture2D* texture = GetTexture(imageFilePath);
	if (texture)
		return texture;

	texture = CreateTextureFromFile(imageFilePath);
	return texture;
}

BitmapFont* SimpleRenderer::CreateOrGetBitmapFont(const std::string& bitmapFontName)
{
	BitmapFont* bitmapFont = GetBitmapFont(bitmapFontName);
	if (bitmapFont)
		return bitmapFont;

	bitmapFont = CreateBitmapFromFile(bitmapFontName);
	return bitmapFont;
}

Font* SimpleRenderer::CreateOrGetFont(const std::string& fontName)
{
	Font* font = GetFont(fontName);
	if (font)
		return font;

	font = CreateFontFromFile(fontName);
	return font;
}

ShaderProgram* SimpleRenderer::CreateOrGetShader(const std::string& filePath)
{
	ShaderProgram* shader = GetShaderProgram(filePath);
	if (shader)
		return shader;

	shader = CreateShaderProgramFromFile(filePath);
	return shader;
}

VertexBuffer* SimpleRenderer::CreateAndGetVBODynamic()
{
	return device->CreateVertexBuffer(nullptr, 1000, RHIEnum::USAGE_DYNAMIC_DRAW);
}

VertexBuffer* SimpleRenderer::CreateAndGetVBOStatic(std::vector<Vertex3D> vertices)
{
	return device->CreateVertexBuffer(vertices.data(), vertices.size());
}

IndexBuffer* SimpleRenderer::CreateAndGetIBODynamic()
{
	return device->CreateIndexBuffer(nullptr, 1000, RHIEnum::USAGE_DYNAMIC_DRAW);
}

IndexBuffer* SimpleRenderer::CreateAndGetIBOStatic(std::vector<unsigned int> indices)
{
	return device->CreateIndexBuffer(indices.data(), indices.size());
}

Mesh* SimpleRenderer::CreateAndGetMeshDynamic()
{
	Mesh* result = new Mesh();
	result->vbo = CreateAndGetVBODynamic();
	result->ibo = CreateAndGetIBODynamic();
	return result;
}

Mesh* SimpleRenderer::CreateAndGetMeshStatic(MeshBuilder* mb)
{
	Mesh* result = new Mesh();
	result->vbo = CreateAndGetVBOStatic(mb->m_vertices);
	if(mb->usesIboEver())
		result->ibo = CreateAndGetIBOStatic(mb->m_indices);
	result->m_drawInstructions = mb->m_drawInstructions;
	return result;
}

Mesh* SimpleRenderer::CreateAndGetSkeletonMesh(Skeleton* skeleton)
{
	m_genericMeshBuilder->clear();
	uint jointCount = skeleton->getJointCount();
	for (uint i = 0; i < jointCount; i++)
	{
		Joint currentJoint = skeleton->m_joints[i];
		m_genericMeshBuilder->begin(RHIEnum::PRIMATIVE_LINE_STRIP, false);
		m_genericMeshBuilder->addSphereOutline(currentJoint.global_transform.getTranslation(), .01f, Rgba::CYAN);
		if (!currentJoint.parentName.empty())
		{
			m_genericMeshBuilder->setColor(Rgba::CYAN);
			m_genericMeshBuilder->addVertex(currentJoint.global_transform.getTranslation());
			m_genericMeshBuilder->setColor(Rgba(0, 64, 64, 255));
			std::string parent = currentJoint.parentName;
			m_genericMeshBuilder->addVertex(skeleton->getJointTransform(parent.c_str()).getTranslation());
		}
		m_genericMeshBuilder->end();
	}
	return CreateAndGetMeshStatic(m_genericMeshBuilder);
}

Sampler* SimpleRenderer::CreateAndGetSampler()
{
	Sampler* sampler = new Sampler(device);
	return sampler;
}

StructuredBuffer* SimpleRenderer::CreateAndGetStructedBuffer(std::vector<Matrix4> vertices)
{
	StructuredBuffer* ssbo = new StructuredBuffer(device, vertices.data(), sizeof(Matrix4), vertices.size());
	return ssbo;
}

UAVTexture* SimpleRenderer::CreateAndGetUAVTexture(uint width, uint height)
{
	UAVTexture* tex = (UAVTexture*)(new Texture2D(device, width, height, RHIEnum::IMAGEFORMAT_RGBA8, true));
	return tex;
}

unsigned int SimpleRenderer::CreateAndGetVBOId()
{
	unsigned int id = m_vertexBufferStorage.size();
	m_vertexBufferStorage.push_back(nullptr);
	return id;
}

unsigned int SimpleRenderer::CreateAndGetMeshId()
{
	unsigned int id = m_meshStorage.size();
	m_meshStorage.push_back(nullptr);
	return id;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRenderer::drawLine(const Vector2& startPos, const Vector2& endPos, const Rgba& startColor, const Rgba& endColor)
{
	drawLine(Vector3(startPos.x, startPos.y, Z2D), Vector3(endPos.x, endPos.y, Z2D), startColor, endColor);
}

void SimpleRenderer::drawLine(const Vector2& startPos, const Vector2& endPos, const Rgba& color)
{
	drawLine(startPos, endPos, color, color);
}

void SimpleRenderer::drawLine(const Vector3& startPos, const Vector3& endPos, const Rgba& startColor, const Rgba& endColor)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(startPos, startColor));
	vertexArray.push_back(Vertex3D(endPos, endColor));

	BindTexture(m_plainWhiteTexture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_LINE);
}

void SimpleRenderer::drawLineAndSetLineWidth(const Vector2& startPos, const Vector2& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness)
{
	SetLineWidth(lineThickness);
	drawLine(startPos, endPos, startColor, endColor);
}

void SimpleRenderer::drawLineAndSetLineWidth(const Vector3& startPos, const Vector3& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness)
{
	SetLineWidth(lineThickness);
	drawLine(startPos, endPos, startColor, endColor);
}

void SimpleRenderer::drawLineStrip(const Vertex2D* vertexes, int numVertexes)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.reserve(numVertexes);

	for (int i = 0; i < numVertexes; ++i)
	{
		vertexArray.push_back(vertexes[i]);
	}

	BindTexture(m_plainWhiteTexture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_LINE_STRIP);
}

void SimpleRenderer::drawThickLine(const Vector2& startPos, const Vector2& endPos, float thickness, const Rgba& startColor, const Rgba& endColor)
{
	float halfThickness = thickness *.5f;

	Vector2 padding = endPos - startPos;
	padding.normalize();
	padding.rotate90Degrees();

	m_genericMeshBuilder->clear();

	m_genericMeshBuilder->begin(RHIEnum::PRIMATIVE_QUADS, false);
	m_genericMeshBuilder->setColor(startColor);
	m_genericMeshBuilder->addVertex(startPos + padding*halfThickness);
	m_genericMeshBuilder->addVertex(startPos - padding*halfThickness);

	m_genericMeshBuilder->setColor(endColor);
	m_genericMeshBuilder->addVertex(endPos - padding*halfThickness);
	m_genericMeshBuilder->addVertex(endPos + padding*halfThickness);
	m_genericMeshBuilder->end();
	SAFE_DELETE(m_genericMesh);
	m_genericMesh = CreateAndGetMeshStatic(m_genericMeshBuilder); //TODO("Fix this to use a dynamic mesh not static.");
	BindTexture(nullptr);
	drawMesh(m_genericMesh);
}

//////////////////////////////////////////////////////////////////////////
void SimpleRenderer::drawTexturedAABB2(const AABB2& bounds, const Texture2D& texture, const AABB2& textureCoords)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.maxs.y)));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.mins.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.maxs.y)));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.mins.y)));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.maxs.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.mins.y)));
	BindTexture(&texture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_QUADS);
}

void SimpleRenderer::drawTexturedAABB2(const AABB2& bounds, const Texture2D& texture, const Rgba& tint)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(0.f, 1.f), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.mins.y, Z2D), Vector2(1.f, 1.f), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(1.f, 0.f), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.maxs.y, Z2D), Vector2(0.f, 0.f), tint));
	BindTexture(&texture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_QUADS);
}

void SimpleRenderer::drawTexturedAABB2(const AABB2& bounds, const AABB2& textureCoords, const Rgba& tint)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.maxs.y), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.mins.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.maxs.y), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.mins.y), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.maxs.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.mins.y), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(textureCoords.mins.x, textureCoords.maxs.y), tint));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(textureCoords.maxs.x, textureCoords.mins.y), tint));
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_TRIANGLE);
}

void SimpleRenderer::drawTexturedSquare(const Vector2& offset, float scale, const AABB2& textureCoords, const Rgba& tint)
{
	AABB2 square = AABB2::ZERO_TO_ONE;
	square.maxs *= scale;
	square.translate(offset);
	drawTexturedAABB2(square, textureCoords, tint);
}

void SimpleRenderer::drawAABB2(const AABB2& bounds, const Rgba& color)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.mins.y, Z2D), Vector2(0.f, 1.f), color));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.mins.y, Z2D), Vector2(1.f, 1.f), color));
	vertexArray.push_back(Vertex3D(Vector3(bounds.maxs.x, bounds.maxs.y, Z2D), Vector2(1.f, 0.f), color));
	vertexArray.push_back(Vertex3D(Vector3(bounds.mins.x, bounds.maxs.y, Z2D), Vector2(0.f, 0.f), color));
	BindTexture(m_plainWhiteTexture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_QUADS);
}

void SimpleRenderer::drawAABB2(const Vector2& mins, const Vector2& maxs, const Rgba& color)
{
	drawAABB2(AABB2(mins, maxs), color);
}

void SimpleRenderer::drawAABB2(const IntVector2& mins, const IntVector2& maxs, const Rgba& color)
{
	drawAABB2(Vector2((float)mins.x, (float)mins.y), Vector2((float)maxs.x, (float)maxs.y), color);
}

void SimpleRenderer::drawThickAABB2(const Vector2& mins, const Vector2& maxs, float thickness, const Rgba& edgeColor, const Rgba& fillColor)
{
	float halfThickness = thickness *.5f;
	AABB2 padding(mins, maxs);
	padding.AddPaddingToSides(-halfThickness, -halfThickness);

	m_genericMeshBuilder->clear();

	m_genericMeshBuilder->begin(RHIEnum::PRIMATIVE_QUADS, false);
	m_genericMeshBuilder->setColor(fillColor);
	m_genericMeshBuilder->addVertex(Vector2(padding.mins.x, padding.maxs.y));
	m_genericMeshBuilder->addVertex(Vector2(padding.mins.x, padding.mins.y));
	m_genericMeshBuilder->addVertex(Vector2(padding.maxs.x, padding.mins.y));
	m_genericMeshBuilder->addVertex(Vector2(padding.maxs.x, padding.maxs.y));
	m_genericMeshBuilder->end();

	m_genericMeshBuilder->begin();
	m_genericMeshBuilder->setColor(edgeColor);
	
	m_genericMeshBuilder->addVertex(Vector2(mins.x, maxs.y)); //0
	m_genericMeshBuilder->addVertex(Vector2(mins.x, mins.y)); //1
	m_genericMeshBuilder->addVertex(Vector2(maxs.x, mins.y)); //2
	m_genericMeshBuilder->addVertex(Vector2(maxs.x, maxs.y)); //3

	m_genericMeshBuilder->addVertex(Vector2(padding.mins.x, padding.maxs.y)); //4
	m_genericMeshBuilder->addVertex(Vector2(padding.mins.x, padding.mins.y)); //5
	m_genericMeshBuilder->addVertex(Vector2(padding.maxs.x, padding.mins.y)); //6
	m_genericMeshBuilder->addVertex(Vector2(padding.maxs.x, padding.maxs.y)); //7

	//Left
	m_genericMeshBuilder->addIndex(0);
	m_genericMeshBuilder->addIndex(1);
	m_genericMeshBuilder->addIndex(4);
	m_genericMeshBuilder->addIndex(4);
	m_genericMeshBuilder->addIndex(1);
	m_genericMeshBuilder->addIndex(5);

	//Bottom
	m_genericMeshBuilder->addIndex(5);
	m_genericMeshBuilder->addIndex(1);
	m_genericMeshBuilder->addIndex(2);
	m_genericMeshBuilder->addIndex(5);
	m_genericMeshBuilder->addIndex(2);
	m_genericMeshBuilder->addIndex(6);

	//Right
	m_genericMeshBuilder->addIndex(7);
	m_genericMeshBuilder->addIndex(6);
	m_genericMeshBuilder->addIndex(3);
	m_genericMeshBuilder->addIndex(3);
	m_genericMeshBuilder->addIndex(6);
	m_genericMeshBuilder->addIndex(2);

	//Top
	m_genericMeshBuilder->addIndex(0);
	m_genericMeshBuilder->addIndex(4);
	m_genericMeshBuilder->addIndex(7);
	m_genericMeshBuilder->addIndex(0);
	m_genericMeshBuilder->addIndex(7);
	m_genericMeshBuilder->addIndex(3);

	m_genericMeshBuilder->end();
	SAFE_DELETE(m_genericMesh);
	m_genericMesh = CreateAndGetMeshStatic(m_genericMeshBuilder); //TODO("Fix this to use a dynamic mesh not static.");
	BindTexture(nullptr);
	drawMesh(m_genericMesh);
}
// 
// void SimpleRenderer::drawThickCircle(const Vector2& center, float radius, float thickness, const Rgba& edgeColor, const Rgba& fillColor, float numSides /*= 64.f*/)
// {
// 
// }

void SimpleRenderer::drawDisc(const Vector2& center, float radius, const Rgba& color, float numSides /*= 64.f*/)
{
	std::vector<Vertex3D> vertexArray;
	vertexArray.reserve((unsigned int)numSides);
	float angleInRadians = (2.f * M_PI) / numSides;

	vertexArray.push_back(Vertex3D(Vector3(center.x, center.y, Z2D), color));

	for (int i = 0; i <= numSides; i++)
	{
		float angleTheta = angleInRadians*i;

		float x = center.x + radius * cosf(angleTheta);
		float y = center.y + radius * sinf(angleTheta);
		vertexArray.push_back(Vertex3D(Vector3(x, y, Z2D), color));
	}

	BindTexture(m_plainWhiteTexture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_TRIANGLE_FAN);
}
//////////////////////////////////////////////////////////////////////////
void SimpleRenderer::drawAxes(float axisLength, float alpha /*= 1.f*/)
{
	//Draw X-axis
	Rgba red;
	red.SetAsFloats(1.0f, 0.0f, 0.0f, alpha);
	drawLine(Vector3::ZERO, Vector3::XAXIS*axisLength, red, red);

	//Draw Y-axis
	Rgba green;
	green.SetAsFloats(0.0f, 1.0f, 0.0f, alpha);
	drawLine(Vector3::ZERO, Vector3::YAXIS*axisLength, green, green);

	//Draw Z-axis
	Rgba blue;
	blue.SetAsFloats(0.0f, 0.0f, 1.0f, alpha);
	drawLine(Vector3::ZERO, Vector3::ZAXIS*axisLength, blue, blue);
}

void SimpleRenderer::drawGridXZ(float axisLength, float alpha /*= 1.f*/)
{
	//Draw X-axis
	Rgba red;
	red.SetAsFloats(1.0f, 0.0f, 0.0f, alpha);

	for (float axisIndex = 1.0f; axisIndex <= axisLength; axisIndex += 1.f)
	{
		drawLine(Vector3::ZAXIS*axisIndex, Vector3::XAXIS*axisLength + Vector3::ZAXIS*axisIndex, red, red);
	}

	//Draw Z-axis
	Rgba blue;
	blue.SetAsFloats(0.0f, 0.0f, 1.0f, alpha);
	for (float axisIndex = 1.0f; axisIndex <= axisLength; axisIndex+=1.f)
	{
		drawLine(Vector3::XAXIS*axisIndex, Vector3::ZAXIS*axisLength + Vector3::XAXIS*axisIndex, blue, blue);
	}
}

//////////////////////////////////////////////////////////////////////////
void SimpleRenderer::drawQuads(const Vertex3D* vertexes, int numVertexes)
{
	SetVBO3DData(m_genericVBO, vertexes, numVertexes, RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, numVertexes, RHIEnum::PRIMATIVE_QUADS);
}

void SimpleRenderer::drawTexturedQuads(const Vertex3D* vertexes, int numVertexes, const Texture2D& texture)
{
	BindTexture(&texture);
	drawQuads(vertexes, numVertexes);
}

void SimpleRenderer::drawTexturedQuads(const Vertex3D* vertexes, int numVertexes)
{
	drawQuads(vertexes, numVertexes);
}

void SimpleRenderer::DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint, float cellAspectScale /*= 1.f*/, const BitmapFont* font /*= nullptr*/)
{
	//PROFILE_SCOPE_FUNCTION();
	if (!font)
	{
		//DebuggerPrintf("Didn't set a font!");

		if (m_loadedBitmapFonts.empty())
			return;
		else
			font = m_loadedBitmapFonts[0]; //Set it to the first font found since fonts can't be removed from outside the renderer.

	}

	std::vector<Vertex3D> vertexArray;
	vertexArray.reserve(asciiText.length() * 4);
	Vector3 bottomLeftStart(startBottomLeft.x, startBottomLeft.y, Z2D);
	float cellWidth = cellHeight * cellAspectScale;

	// Corner offsets
	Vector3 bottomLeftPos(0.f, 0.f, Z2D);
	Vector3 bottomRightPos(cellWidth, 0.f, Z2D);
	Vector3 topRightPos(cellWidth, cellHeight, Z2D);
	Vector3 topLeftPos(0.f, cellHeight, Z2D);

	for (unsigned int letterIndex = 0; letterIndex < asciiText.length(); letterIndex++)
	{
		AABB2 letterTexCoords = font->GetTexCoordsForGlyph(asciiText[letterIndex]);
		Vector2 bottomLeftTex(letterTexCoords.mins.x, letterTexCoords.maxs.y);
		Vector2 bottomRightTex(letterTexCoords.maxs.x, letterTexCoords.maxs.y);
		Vector2 topRightTex(letterTexCoords.maxs.x, letterTexCoords.mins.y);
		Vector2 topLeftTex(letterTexCoords.mins.x, letterTexCoords.mins.y);


		vertexArray.push_back(Vertex3D((bottomLeftStart + bottomLeftPos), bottomLeftTex, tint));
		vertexArray.push_back(Vertex3D((bottomLeftStart + bottomRightPos), bottomRightTex, tint));
		vertexArray.push_back(Vertex3D((bottomLeftStart + topRightPos), topRightTex, tint));
		vertexArray.push_back(Vertex3D((bottomLeftStart + topLeftPos), topLeftTex, tint));

		bottomLeftStart += bottomRightPos;
	}

	BindTexture(&font->getTexture2D());
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_QUADS);
}

void SimpleRenderer::drawText2DWithBitmapFont_Centered(const Vector2& bottomCenter, const std::string& asciiText, float cellHeight, const Rgba& tint, float cellAspectScale /*= 1.f*/, const BitmapFont* font /*= nullptr*/)
{
	float textWidth = BitmapFont::getTextWidth(asciiText, cellHeight, cellAspectScale);
	float halfTextWidth = textWidth *.5f;

	Vector2 startBottomLeft(bottomCenter.x - halfTextWidth, bottomCenter.y);
	DrawText2D(startBottomLeft, asciiText, cellHeight, tint, cellAspectScale, font);
}

void SimpleRenderer::drawStringWithBitmapFont(const Vector2& startBottomLeft, const RenderableString& str, const BitmapFont* font /*= nullptr*/, float cellAspectScale /*= 1.f*/)
{
	//PROFILE_SCOPE_FUNCTION();
	if (!font)
	{
		//DebuggerPrintf("Didn't set a font!");
		return;
	}

	std::vector<Vertex3D> vertexArray;
	vertexArray.reserve(str.m_string.length() * 4);
	Vector3 bottomLeftStart(startBottomLeft.x, startBottomLeft.y, Z2D);
	float cellHeight = str.m_scale;
	float cellWidth = cellHeight * cellAspectScale;

	// Corner offsets
	Vector3 bottomLeftPos(0.f, 0.f, Z2D);
	Vector3 bottomRightPos(cellWidth, 0.f, Z2D);
	Vector3 topRightPos(cellWidth, cellHeight, Z2D);
	Vector3 topLeftPos(0.f, cellHeight, Z2D);

	for (unsigned int letterIndex = 0; letterIndex < str.m_string.length(); letterIndex++)
	{
		AABB2 letterTexCoords = font->GetTexCoordsForGlyph(str.m_string[letterIndex]);
		Vector2 bottomLeftTex(letterTexCoords.mins.x, letterTexCoords.maxs.y);
		Vector2 bottomRightTex(letterTexCoords.maxs.x, letterTexCoords.maxs.y);
		Vector2 topRightTex(letterTexCoords.maxs.x, letterTexCoords.mins.y);
		Vector2 topLeftTex(letterTexCoords.mins.x, letterTexCoords.mins.y);


		vertexArray.push_back(Vertex3D((bottomLeftStart + bottomLeftPos), bottomLeftTex, str.m_color));
		vertexArray.push_back(Vertex3D((bottomLeftStart + bottomRightPos), bottomRightTex, str.m_color));
		vertexArray.push_back(Vertex3D((bottomLeftStart + topRightPos), topRightTex, str.m_color));
		vertexArray.push_back(Vertex3D((bottomLeftStart + topLeftPos), topLeftTex, str.m_color));

		bottomLeftStart += bottomRightPos;
	}

	BindTexture(&font->getTexture2D());
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_QUADS);
}

void SimpleRenderer::drawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float scale, const Rgba& tint, const std::string& fontName)
{
	//PROFILE_SCOPE_FUNCTION();

	Font* font = CreateOrGetFont(fontName);

	if (!font)
	{
		DebuggerPrintf("Couldn't load font: %s", fontName.c_str());
		return;
	}

	std::vector<Vertex3D> vertexArray;
	vertexArray.reserve(asciiText.length() * 4);
	Vector3 topLeftStart(startBottomLeft.x, startBottomLeft.y + scale, Z2D);
	

	int prevGlyphId = -1;

	for (unsigned int letterIndex = 0; letterIndex < asciiText.length(); letterIndex++)
	{
		FontCharacter glyph = font->getGlyph(asciiText[letterIndex]);
		float cellHeight = (float)glyph.height * scale;
		float cellWidth = (float)glyph.width * scale;
		// Corner offsets
		Vector3 bottomLeftPos(0.f, -cellHeight, 0.f);
		Vector3 bottomRightPos(cellWidth, -cellHeight, 0.f);
		Vector3 topRightPos(cellWidth, 0.f, 0.f);
		Vector3 topLeftPos(0.f, 0.f, 0.f);

		AABB2 letterTexCoords = font->GetTexCoordsForGlyph(asciiText[letterIndex]);
		Vector2 bottomLeftTex	(letterTexCoords.mins.x, letterTexCoords.maxs.y);
		Vector2 bottomRightTex	(letterTexCoords.maxs.x, letterTexCoords.maxs.y);
		Vector2 topRightTex		(letterTexCoords.maxs.x, letterTexCoords.mins.y);
		Vector2 topLeftTex		(letterTexCoords.mins.x, letterTexCoords.mins.y);

		Vector3 offset(0.f, (-glyph.yoffset*scale), 0.f);
		offset.x = ( glyph.xoffset + font->getKerning(prevGlyphId, glyph.id) ) * scale;
		prevGlyphId = glyph.id;

		vertexArray.push_back(Vertex3D((topLeftStart + offset + bottomLeftPos), bottomLeftTex, tint));
		vertexArray.push_back(Vertex3D((topLeftStart + offset + bottomRightPos), bottomRightTex, tint));
		vertexArray.push_back(Vertex3D((topLeftStart + offset + topRightPos), topRightTex, tint));
		vertexArray.push_back(Vertex3D((topLeftStart + offset + topLeftPos), topLeftTex, tint));

		topLeftStart += topRightPos;
		topLeftStart += Vector3(glyph.xadvance, 0.f, 0.f);
	}

	Texture2D* texture = CreateOrGetTexture(font->m_texturePath);
	BindTexture(texture);
	SetVBO3DData(m_genericVBO, vertexArray.data(), vertexArray.size(), RHIEnum::USAGE_DYNAMIC_DRAW);
	drawVBO3D(m_genericVBO, vertexArray.size(), RHIEnum::PRIMATIVE_QUADS);
}

void SimpleRenderer::drawSkeletonInstance(const SkeletonInstance& skeleton, const Mesh& /*skelMesh*/)
{
	pushMatrix();
	for (uint i = 0; i < skeleton.m_pose.m_transforms.size(); i++)
	{
		//const SQT& currentSQT = skeleton.m_pose.m_transforms[i];
		const SQT& currentSQT = skeleton.get_joint_global_transform(i);
		pushMatrix();
		transform(currentSQT.getAsMatrix());

		//const DrawInstruction& drawInstruct = skelMesh.m_drawInstructions[i];
		//drawVBO3D(skelMesh.vbo, drawInstruct.vertex_count, drawInstruct.primative, drawInstruct.start_vertex);
		drawAxes(1.f);
		popMatrix();
	}
	popMatrix();
}

void SimpleRenderer::drawSkinMeshSkel(const SkeletonInstance& skeleton, Mesh* skelMesh)
{
	std::vector<Matrix4> skinTrans;
	for (uint i = 0; i < skeleton.m_pose.m_transforms.size(); i++)
	{
		Matrix4 bindInverse = skeleton.mo_skeleton->getJointTransform(i);
		//Matrix4 bindInverse = skeleton.get_joint_global_transform(i).getAsMatrix();
		bindInverse=bindInverse.getInverse();

		SQT currentSQT = skeleton.get_joint_global_transform(i);

		bindInverse = bindInverse.getTransformed(currentSQT.getAsMatrix());
		Matrix4 newTrans = bindInverse;
		//Matrix4 newTrans = currentSQT.getAsMatrix();
		newTrans.tranpose();
		skinTrans.push_back(newTrans);
		//skinTrans.push_back(Matrix4());
	}
	skeleton.m_skinTransfroms->update(context, skinTrans.data());

	drawMesh(skelMesh);
}

//-----------------------------------------------------------------------------------------------
void SimpleRenderer::UpdateGameTime(float deltaSeconds)
{
	timeData.game_frame_time = deltaSeconds;
	timeData.game_time += deltaSeconds;
}

void SimpleRenderer::UpdateSystemTime(float deltaSeconds)
{
	timeData.system_frame_time = deltaSeconds;
	timeData.system_time += deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
Texture2D* SimpleRenderer::CreateTextureFromFile(const std::string& imageFilePath)
{

	Texture2D* texture = device->CreateTextureFromFile(imageFilePath.c_str());
	m_alreadyLoadedTextures.push_back(texture);
	return texture;
}

Texture2D* SimpleRenderer::CreateTextureFromData(const std::string& name, unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
{
	Texture2D* texture = device->CreateTextureFromData(name, imageTexelBytes, width, height, bytesPerTexel);
	m_alreadyLoadedTextures.push_back(texture);
	return texture;
}

void SimpleRenderer::CreateBuiltInTextures()
{
	unsigned char plainWhiteTexel[4] = { 255, 255, 255, 255 };
	m_plainWhiteTexture = CreateTextureFromData("White", plainWhiteTexel, 1, 1, 4);

	unsigned char plainGrayTexel[4] = { 128, 128, 128, 255 };
	m_plainGrayTexture = CreateTextureFromData("Gray", plainGrayTexel, 1, 1, 4);

	unsigned char plainColorTexel[36] = 
	{
		/*Red*/		255,   0, 0, 255, /*Green*/		  0, 255,   0, 255,   /*Blue*/	0,     0, 255, 255,
		/*Yellow*/	255, 255, 0, 255, /*Magenta*/	255,   0, 255, 255,   /*Cyan*/	0,   255, 255, 255,
		/*Black*/	  0,   0, 0, 255, /*Gray*/		128, 128, 128, 255,   /*White*/ 255, 255, 255, 255
	};

	m_plainColorTexture = CreateTextureFromData("ColorSpectrum", plainColorTexel, 3, 3, 4);

	unsigned char flatNormalTexture[] =
	{
		128, 128, 255, 255
	};
	m_flatNormalTexture = CreateTextureFromData("FlatNormal", flatNormalTexture, 1, 1, 4);
}
//-----------------------------------------------------------------------------------------------
Texture2D* SimpleRenderer::GetTexture(const std::string& imageFilePath)
{
	for (int textureIndex = 0; textureIndex < (int)m_alreadyLoadedTextures.size(); ++textureIndex)
	{
		Texture2D* texture = m_alreadyLoadedTextures[textureIndex];
		if (imageFilePath == texture->m_imageFilePath)
			return texture;
	}
	return nullptr;
}
//------------------------------------------------------------------------------------------------
ShaderProgram* SimpleRenderer::CreateShaderProgramFromFile(const std::string& filePath)
{
	ShaderProgram* shader = device->CreateShaderFromFile_hlsl(filePath.c_str());
	m_loadedShaders.push_back(shader);
	return shader;
}

ShaderProgram* SimpleRenderer::CreateShaderProgramFromString(const std::string& name, const std::string& buffer)
{
	ShaderProgram* shader = device->CreateShaderFromString_hlsl(name.c_str(), buffer);
	m_loadedShaders.push_back(shader);
	return shader;
}

void SimpleRenderer::CreateBuiltInShaderPrograms()
{
	std::string defaultUnlitShader =R"(

Texture2D <float4> tImage : register(t0);

SamplerState sSampler : register(s0);

cbuffer matrix_cb : register(b0) {
	float4x4 MODEL;
	float4x4 VIEW;
	float4x4 PROJECTION;
}

cbuffer time_cb : register(b1) {
	float GAME_TIME;
	float SYSTEM_TIME;
	float GAME_FRAME_TIME;
	float SYSTEM_FRAME_TIME;
}

struct vs_input_t {
	float3 position : POSITION;
	float4 tint : TINT;
	float2 uv : UV;

	//float3 normal : NORMAL;
};

struct v2f_t {

	float4 position : SV_Position;
	float4 tint : TINT;
	float2 uv : UV;
	//float3 normal: NORMAL;
};

v2f_t VertexFunction(vs_input_t input) {
	v2f_t v2f = (v2f_t)0;

	float4 local = float4(input.position, 1.0f);
	float4 world = mul(local, MODEL);
	float4 view = mul(world, VIEW);
	float4 clip = mul(view, PROJECTION);
    
	v2f.position = clip;
	v2f.tint = input.tint;
	v2f.uv = input.uv;

	return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{

	float4 albedo = tImage.Sample(sSampler, input.uv);

	return albedo * input.tint;
}
	)";

	std::string oldefaultLitShader = 
	R"(
//-------------------------------------------------------------------------
// VERTEX INPUT
struct vertex_in_t
{
   float3 position : POSITION;
   float3 normal : NORMAL;
   float4 tint : TINT;
   float2 uv : UV;
};

//-------------------------------------------------------------------------
// VERTEX TO FRAGMENT DATA
struct vertex_to_fragment_t
{
   float4 position : SV_Position; 
   float3 normal : NORMAL;
   float4 tint : TINT;
   float2 uv : UV;

   float3 world_position : WORLD;
};

//-------------------------------------------------------------------------
// CONSTANT BUFFERS
cbuffer matrix_cb : register(b0)
{
   float4x4 MODEL;
   float4x4 VIEW;
   float4x4 PROJECTION;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,
cbuffer time_cb : register(b1) 
{
   float GAME_TIME;
   float SYSTEM_TIME;
   float GAME_FRAME_TIME;
   float SYSTEM_FRAME_TIME;
};

cbuffer light_cb : register(b2)
{
   // GLOBAL LIGHT DATA
   float4 AMBIENT;         // <r, g, b, intensity>

   // PER LIGHT DATA
   // POINT LIGHT, DOT3
   float4 LIGHT_COLOR;       // <r, g, b, intensity>
   float4 LIGHT_POSITION;
   float4 ATTENUATION;       // <constant, linear, quadratic, padding>
   float4 SPEC_ATTENUATION;  // <constant, linear, quadratic, padding>

   // SURFACE DATA
   float SPEC_FACTOR;      // [0, 1] specular scaling factor
   float SPEC_POWER;       // [POSITIVE INTEGER]  more speculative power.
   float2 LIGHT_PADDING;

// Needed for lighting - but I consider part of the matrix buffer
   float4 EYE_POSITION; 
}


Texture2D <float4> tDiffuse : register(t0);
Texture2D <float4> tNormal : register(t1);

SamplerState sSampler : register(s0);

//-------------------------------------------------------------------------
// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction( vertex_in_t vertex ) 
{
   vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

   float4 model_position = float4( vertex.position, 1.0f );
   float4 world_position = mul( model_position, MODEL );
   float4 view_position = mul( world_position, VIEW );
   float4 clip_position = mul( view_position, PROJECTION );

   out_data.position = clip_position;
   out_data.normal = mul( float4(vertex.normal, 0.0f), MODEL ).xyz; // W get sa 0, so as not to take on the translation of the model matrix
   out_data.tint = vertex.tint;
   out_data.uv = vertex.uv;
   out_data.world_position = world_position.xyz;
   return out_data;
}

float4 NormalAsColor( float3 n )
{
   return float4( (n + 1.0f) * .5f, 1.0f );
}

//-------------------------------------------------------------------------
// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction( vertex_to_fragment_t data ) : SV_Target0
{
   // ?  Sample Color at Texture Coordinate data.uv
   float4 color = tDiffuse.Sample( sSampler, data.uv );
   float4 diffuse = color * data.tint;
   
   float3 normal_color = tNormal.Sample( sSampler, data.uv ).xyz;
   float3 surface_normal = normal_color * float3( 2, 2, -1 ) - float3( 1, 1, 0 );
   
   // renormalize - the interpolation step may have skewed it
   // float3 normal = data.normal;
   float3 normal = surface_normal;

   normal = normalize(normal);

   // calculate the eye vector
   float3 vector_to_eye = EYE_POSITION.xyz - data.world_position;
   float3 eye_vector = -normalize(vector_to_eye); // get direction from eye to fragment

   // light factor

   // first, calculate ambient (just added in - once per fragment)
   float4 ambient_factor = float4( AMBIENT.xyz * AMBIENT.w, 1.0f );

   // PER LIGHT START
   // point light factor - light factors are calculated PER LIGHT and added 
   // to the ambient factor to get your final diffuse factor
   float3 vector_to_light = LIGHT_POSITION.xyz - data.world_position;
   float distance_to_light = length(vector_to_light);
   float3 dir_to_light = vector_to_light / distance_to_light;

   float dot3 = saturate( dot( dir_to_light, normal ) );
   float attenuation = LIGHT_COLOR.w / (ATTENUATION.x 
      + distance_to_light * ATTENUATION.y
      + distance_to_light * distance_to_light * ATTENUATION.z);

   // don't allow attenuation to go above one (so don't allow it to blow out the image)
   attenuation = saturate(attenuation);

   float4 light_color = float4( LIGHT_COLOR.xyz, 1.0f ); 
   float4 light_diffuse_factor = light_color * dot3 * attenuation;

   // Calculate Spec Component
   float spec_attenuation = LIGHT_COLOR.w / (SPEC_ATTENUATION.x 
      + distance_to_light * SPEC_ATTENUATION.y
      + distance_to_light * distance_to_light * SPEC_ATTENUATION.z );
   
   // figure how much the reflected vector coincides with the eye vector
   float3 ref_light_dir = reflect( dir_to_light, normal ); 
   float spec_dot3 = saturate( dot( ref_light_dir, eye_vector ) );

   // take it to the spec power, and scale it by the spec factor and our attenuation
   float spec_factor = spec_attenuation * SPEC_FACTOR * pow( spec_dot3, SPEC_POWER );
   float4 spec_color = spec_factor * light_color;

   // END PER LIGHT PART

   // calculate final diffuse factor by adding all light factors to the ambiant factor
   float4 diffuse_factor = saturate(ambient_factor + light_diffuse_factor);

   // final color is our diffuse color multiplied by diffuse factor.  Spec color is added on top. 
   float4 final_color = diffuse_factor * diffuse + spec_color;

   // calculate final color
   return final_color;
}
	)";

	std::string defaultUntexturedShader = R"(

//-------------------------------------------------------------------------
// VERTEX INPUT
struct vertex_in_t
{
   float3 position : POSITION;
   float4 tint : TINT;
   float2 uv : UV;
};

//-------------------------------------------------------------------------
// VERTEX TO FRAGMENT DATA
struct vertex_to_fragment_t
{
   float4 position : SV_Position; 
   float4 tint : TINT;
   float2 uv : UV;
};

//-------------------------------------------------------------------------
// CONSTANT BUFFERS
cbuffer matrix_cb : register(b0)
{
   float4x4 MODEL;
   float4x4 VIEW;
   float4x4 PROJECTION;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,
cbuffer time_cb : register(b1) 
{
   float GAME_TIME;
   float SYSTEM_TIME;
   float GAME_FRAME_TIME;
   float SYSTEM_FRAME_TIME;
};


Texture2D <float4> tTexture : register(t0);
SamplerState sSampler : register(s0);

//-------------------------------------------------------------------------
// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction( vertex_in_t vertex ) 
{
   vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

   float4 model_position = float4( vertex.position, 1.0f );
   float4 world_position = mul( model_position, MODEL );
   float4 view_position = mul( world_position, VIEW );
   float4 clip_position = mul( view_position, PROJECTION );

   out_data.position = clip_position;
   out_data.tint = vertex.tint;
   out_data.uv = vertex.uv;
   return out_data;
}

//-------------------------------------------------------------------------
// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction( vertex_to_fragment_t data ) : SV_Target0
{
   // ?  Sample Color at Texture Coordinate data.uv
   float4 final_color = data.tint;
   return final_color;
}

)";

	std::string defaultLitShader = R"(

//-------------------------------------------------------------------------
// VERTEX INPUT
struct vertex_in_t
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tint : TINT;
	float2 uv : UV;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

//-------------------------------------------------------------------------
// VERTEX TO FRAGMENT DATA
struct vertex_to_fragment_t
{
	float4 position : SV_Position;
	float3 normal : NORMAL;
	float4 tint : TINT;
	float2 uv : UV;

	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;

	float4 world_position : WORLD;
};

//-------------------------------------------------------------------------
// CONSTANT BUFFERS
cbuffer matrix_cb : register(b0)
{
	float4x4 MODEL;
	float4x4 VIEW;
	float4x4 PROJECTION;
};

// MAX SIZE: 4096 Elements (float4), 16 KB
// ALIGNMENT:  must be aligned to 16B,
cbuffer time_cb : register(b1)
{
	float GAME_TIME;
	float SYSTEM_TIME;
	float GAME_FRAME_TIME;
	float SYSTEM_FRAME_TIME;
};

struct light_struct
{
	float4 LIGHT_COLOR;       // <r, g, b, intensity>
	float4 LIGHT_DIRECTION;
	float4 LIGHT_POSITION;
	float3 ATTENUATION;       // <constant, linear, quadratic>
	float  ANGLE;
	float4 SPEC_ATTENUATION; // <constant, linear, quadratic, padding>
};
#define MAX_LIGHTS 8
cbuffer light_cb : register(b2)
{
	// GLOBAL LIGHT DATA
	float4 AMBIENT;         // <r, g, b, intensity>

							// PER LIGHT DATA
							// POINT LIGHT, DOT3
	light_struct LIGHTS[MAX_LIGHTS];
	

							  // SURFACE DATA
	float SPEC_FACTOR;      // [0, 1] specular scaling factor
	float SPEC_POWER;       // [POSITIVE INTEGER]  more speculative power.
	float2 LIGHT_PADDING;

	// Needed for lighting - but I consider part of the matrix buffer
	float4 EYE_POSITION;
}


Texture2D <float4> tDiffuse : register(t0);
Texture2D <float4> tNormal : register(t1);
Texture2D <float4> tSpecular : register(t2);
Texture2D <float4> tAmbientOcclusion : register(t3);

SamplerState sSampler : register(s0);

///////////////////////////////////////////////////////////////////////////////
float4 DoDiffuse(light_struct light, float3 light_vector, float3 surface_normal)
{
	float NdotL = saturate(dot(light_vector, surface_normal));
	float4 light_color = float4(light.LIGHT_COLOR.xyz, 1.0f);
	return light_color * NdotL;
}
float4 DoSpecular(light_struct light, float3 view_vector, float3 light_vector, float3 surface_normal)
{
	//Phong
	float3 R = reflect(light_vector, surface_normal);
	float RdotV = saturate(dot(R, view_vector));

	//Blinn-Phong
	//float3 half_angle_vector = normalize(light_vector + view_vector);
	//float NdotH = max(0, dot(surface_normal, half_angle_vector));

	return float4((light.LIGHT_COLOR.xyz * pow(RdotV, SPEC_POWER) * SPEC_FACTOR),1.0f);
}
float DoAttenuation(light_struct light, float distance, float4 attenuationType)
{
	float attenuation = light.LIGHT_COLOR.w / (attenuationType.x + attenuationType.y * distance + attenuationType.z * distance * distance);
	return saturate(attenuation);
}
struct LightingResult
{
	float4 diffuse;
	float4 specular;
};
LightingResult DoPointLight(light_struct light, float3 view_vector, float4 point_being_shaded, float3 surface_normal)
{
	LightingResult result;

	float3 light_vector = light.LIGHT_POSITION.xyz - point_being_shaded.xyz;
	float distance = length(light_vector);
	light_vector = light_vector / distance;

	float attenuation = DoAttenuation(light, distance, float4(light.ATTENUATION, 1.0f));
	float specAttenuation = DoAttenuation(light, distance, light.SPEC_ATTENUATION);

	result.diffuse = DoDiffuse(light, light_vector, surface_normal) * attenuation;
	result.specular = DoSpecular(light, view_vector, light_vector, surface_normal) * specAttenuation;

	return result;
}

LightingResult DoDirectionalLight(light_struct light, float3 view_vector, float4 point_being_shaded, float3 surface_normal)
{
	LightingResult result;

	float3 light_vector = -light.LIGHT_DIRECTION.xyz;

	result.diffuse = DoDiffuse(light, light_vector, surface_normal);
	result.specular = DoSpecular(light, view_vector, light_vector, surface_normal);

	return result;
}
float DoSpotCone(light_struct light, float3 light_vector)
{
	float minCos = cos(light.ANGLE);
	float maxCos = (minCos + 1.0f) / 2.0f;
	float cosAngle = dot(light.LIGHT_DIRECTION.xyz, -light_vector);
	cosAngle = clamp(cosAngle, minCos, maxCos);
	return smoothstep(minCos, maxCos, cosAngle);
}
LightingResult DoSpotLight(light_struct light, float3 view_vector, float4 point_being_shaded, float3 surface_normal)
{
	LightingResult result;

	float3 light_vector = light.LIGHT_POSITION.xyz - point_being_shaded.xyz;
	float distance = length(light_vector);
	light_vector = light_vector / distance;

	float attenuation = DoAttenuation(light, distance, float4(light.ATTENUATION, 1.0f));
	float specAttenuation = DoAttenuation(light, distance, light.SPEC_ATTENUATION);
	float spotIntensity = DoSpotCone(light, light_vector);

	result.diffuse = DoDiffuse(light, light_vector, surface_normal) * attenuation * spotIntensity;
	result.specular = DoSpecular(light, view_vector, light_vector, surface_normal) * specAttenuation * spotIntensity;

	return result;
}
LightingResult ComputeLighting(float4 point_being_shaded, float3 surface_normal)
{
	float3 view_vector = -normalize(EYE_POSITION.xyz - point_being_shaded.xyz);
	
	LightingResult totalResult = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	[unroll]
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

		result = DoSpotLight(LIGHTS[i], view_vector, point_being_shaded, surface_normal);

		totalResult.diffuse += result.diffuse;
		totalResult.specular += result.specular;
	}

	totalResult.diffuse = saturate(totalResult.diffuse);
	totalResult.specular = saturate(totalResult.specular);

	return totalResult;
}
//-------------------------------------------------------------------------
// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction(vertex_in_t vertex)
{
	vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;

	float4 model_position = float4(vertex.position, 1.0f);
	float4 world_position = mul(model_position, MODEL);
	float4 view_position = mul(world_position, VIEW);
	float4 clip_position = mul(view_position, PROJECTION);

	out_data.position = clip_position;
	out_data.normal = mul(float4(vertex.normal, 0.0f), MODEL).xyz; // W get sa 0, so as not to take on the translation of the model matrix
	out_data.tint = vertex.tint;
	out_data.uv = vertex.uv;
	out_data.world_position = world_position;
	out_data.tangent = mul(float4(vertex.tangent,0.f), MODEL).xyz;
	out_data.bitangent = mul(float4(vertex.bitangent,0.f), MODEL).xyz;
	return out_data;
}

float4 NormalAsColor(float3 n)
{
	return float4((n + 1.0f) * .5f, 1.0f);
}

//-------------------------------------------------------------------------
// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction(vertex_to_fragment_t data) : SV_Target0
{

	float3 specularMap = tSpecular.Sample(sSampler, data.uv).xyz;
	float3 occlusionMap = tAmbientOcclusion.Sample(sSampler, data.uv).xyz;
	float4 color = tDiffuse.Sample(sSampler, data.uv);
	float4 diffuse = color * data.tint;

	float3 normal_color = tNormal.Sample(sSampler, data.uv).xyz;
	float3 faceNormal = normalize(data.normal);

	float3 tangent = data.tangent;
	float3 bitangent = normalize(cross(tangent, faceNormal));
	tangent = normalize(cross(faceNormal, bitangent));

	float3x3 tbn = { tangent, bitangent, faceNormal };

	float3 surface_normal = normal_color * float3(2, 2, 1) - float3(1, 1, 0);
	surface_normal = mul(surface_normal, tbn);

	float3 normal = surface_normal;
	normal = normalize(normal);

	LightingResult lit = ComputeLighting(data.world_position, normal);

	float4 ambient = float4(AMBIENT.xyz * AMBIENT.w, 1.0f) * float4(occlusionMap, 1.0f);
	

	float4 material_specular = tSpecular.Sample(sSampler, data.uv);
	float4 specular = material_specular * lit.specular;

	
	float4 diffuse_factor = saturate(ambient + lit.diffuse);
	float4 final_color = diffuse_factor * diffuse + specular;

	return final_color;
}


)";

std::string particleUnlitShader = R"(

Texture2D <float4> tImage : register(t0);

SamplerState sSampler : register(s0);

cbuffer matrix_cb : register(b0) {
	float4x4 MODEL;
	float4x4 VIEW;
	float4x4 PROJECTION;
}

cbuffer time_cb : register(b1) {
	float GAME_TIME;
	float SYSTEM_TIME;
	float GAME_FRAME_TIME;
	float SYSTEM_FRAME_TIME;
}


cbuffer particle_cb : register(b6) {
	float4 START_COLOR;
	float4 END_COLOR;
	float PARTICLE_FRACTION;
	float3 PARTICLE_PADDING;
}

struct vs_input_t {
	float3 position : POSITION;
	float4 tint : TINT;
	float2 uv : UV;

	//float3 normal : NORMAL;
};

struct v2f_t {

	float4 position : SV_Position;
	float4 tint : TINT;
	float2 uv : UV;
	//float3 normal: NORMAL;
};

float4 myLerp(float4 start, float4 end, float fractionToEnd)
{
	float fractionOfStart = 1.f - fractionToEnd;
	float4 result;
	result.x = (fractionOfStart * start.x) + (fractionToEnd * end.x);
	result.y = (fractionOfStart * start.y) + (fractionToEnd * end.y);
	result.z = (fractionOfStart * start.z) + (fractionToEnd * end.z);
	result.w = (fractionOfStart * start.w) + (fractionToEnd * end.w);
	return result;
}

v2f_t VertexFunction(vs_input_t input) {
	v2f_t v2f = (v2f_t)0;

	float4 local = float4(input.position, 1.0f);
	float4 world = mul(local, MODEL);
	float4 view = mul(world, VIEW);
	float4 clip = mul(view, PROJECTION);
    
	v2f.position = clip;
	v2f.tint = lerp(START_COLOR, END_COLOR, PARTICLE_FRACTION);
	v2f.uv = input.uv;

	return v2f;
}

float4 FragmentFunction(v2f_t input) : SV_Target0
{

	float4 albedo = tImage.Sample(sSampler, input.uv);

	return albedo * input.tint;
}
	)";




	CreateShaderProgramFromString(DEFAULT_UNLIT_SHADER, defaultUnlitShader);
	CreateShaderProgramFromString(DEFAULT_LIT_SHADER, defaultLitShader);
	CreateShaderProgramFromString(DEFAULT_UNTEXTURED_SHADER, defaultUntexturedShader);

	CreateShaderProgramFromString(PARTICLE_UNLIT_SHADER, particleUnlitShader);
}

ShaderProgram* SimpleRenderer::GetShaderProgram(const std::string& filePath)
{
	for (int shaderIndex = 0; shaderIndex < (int)m_loadedShaders.size(); ++shaderIndex)
	{
		ShaderProgram* shader = m_loadedShaders[shaderIndex];
		if (shader && filePath == shader->m_filePath)
			return shader;
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------------------


BitmapFont* SimpleRenderer::CreateBitmapFromFile(const std::string& bitmapFontName)
{
	Texture2D* texture = CreateOrGetTexture("Data/Fonts/" + bitmapFontName + ".png");

	BitmapFont* font = new BitmapFont(bitmapFontName, texture);
	m_loadedBitmapFonts.push_back(font);
	return font;
}

BitmapFont* SimpleRenderer::GetBitmapFont(const std::string& bitmapFontName)
{
	for (int fontIndex = 0; fontIndex < (int)m_loadedBitmapFonts.size(); ++fontIndex)
	{
		BitmapFont* font = m_loadedBitmapFonts[fontIndex];
		if (bitmapFontName == font->m_name)
			return font;
	}
	return nullptr;
}

Font* SimpleRenderer::CreateFontFromFile(const std::string& fontName)
{
	Font* font = new Font(fontName);
	CreateOrGetTexture(font->m_texturePath); // Preload in texture
	m_loadedFonts.push_back(font);
	return font;
}

Font* SimpleRenderer::GetFont(const std::string& fontName)
{
	for (int fontIndex = 0; fontIndex < (int)m_loadedFonts.size(); ++fontIndex)
	{
		Font* font = m_loadedFonts[fontIndex];
		if (fontName == font->m_filePath)
			return font;
	}
	return nullptr;
}

std::vector<uint> SimpleRenderer::getPrimativeIndexList(RHIEnum::PrimativeType type, int numVertexes) const
{
	std::vector<uint> indexes;
	switch (type)
	{
	case RHIEnum::PRIMATIVE_QUADS: indexes = getQuadsIndexList(numVertexes);
		break;
	case RHIEnum::PRIMATIVE_QUADS_STRIP:
		break;
	case RHIEnum::PRIMATIVE_LINE_LOOP:
		break;
	case RHIEnum::PRIMATIVE_LINE:
		break;
	case RHIEnum::PRIMATIVE_LINE_STRIP:
		break;
	case RHIEnum::PRIMATIVE_TRIANGLE_FAN: indexes = getTriangleFanIndexList(numVertexes);
		break;
	case RHIEnum::PRIMATIVE_TRIANGLE:
	case RHIEnum::NUM_PRIMATE_TYPES:
	default:  //Shouldn't need an index buffer.
		break;
	}
	return indexes;
}

std::vector<uint> SimpleRenderer::getQuadsIndexList(int numVertexes) const
{
	int numQuads = numVertexes / 4;
	std::vector<uint> indexes;
	indexes.reserve(6 * numQuads);

	for (int i = 0; i < numQuads; i++)
	{
		int offset = 4 * i;
		indexes.push_back(0 + offset); //bottomLeft
		indexes.push_back(1 + offset); //bottomRight
		indexes.push_back(2 + offset); //topRight
		indexes.push_back(0 + offset); //bottomLeft
		indexes.push_back(2 + offset); //topRight
		indexes.push_back(3 + offset); //topLeft
	}
	return indexes;
}

std::vector<uint> SimpleRenderer::getTriangleFanIndexList(int numVertexes) const
{
	int numTriangles = numVertexes - 1;
	std::vector<uint> indexes;

	for (int i = 0; i < numTriangles - 1; i++)
	{
		indexes.push_back(0);
		indexes.push_back(i + 1);
		indexes.push_back(i + 2);
	}

	return indexes;
}



void SimpleRenderer::UpdateMatrixBuffer()
{
	SetProjectionMatrix(m_projectionMatrixStack.back());
	SetViewMatrix(m_viewMatrixStack.back());
	SetModelMatrix(m_modelMatrixStack.back());
}

//////////////////////////////////////////////////////////////////////////
void SimpleRenderer::cleanUpLoadedFonts()
{
	for (int index = 0; index < (int)m_loadedFonts.size(); ++index)
	{
		SAFE_DELETE(m_loadedFonts[index]);
	}
}

void SimpleRenderer::cleanUpLoadedBitmapFonts()
{
	for (int index = 0; index < (int)m_loadedBitmapFonts.size(); ++index)
	{
		SAFE_DELETE(m_loadedBitmapFonts[index]);
	}
}

void SimpleRenderer::cleanUpLoadedShaders()
{
	for (int index = 0; index < (int)m_loadedShaders.size(); ++index)
	{
		SAFE_DELETE(m_loadedShaders[index]);
	}
}

void SimpleRenderer::cleanUpLoadedTextures()
{
	for (int index = 0; index < (int)m_alreadyLoadedTextures.size(); ++index)
	{
		SAFE_DELETE(m_alreadyLoadedTextures[index]);
	}
}

void SimpleRenderer::cleanUpLoadedVertexBuffers()
{
	for (int index = 0; index < (int)m_vertexBufferStorage.size(); ++index)
	{
		SAFE_DELETE(m_vertexBufferStorage[index]);
	}
}

void SimpleRenderer::cleanUpLoadedMeshs()
{
	for (int index = 0; index < (int)m_meshStorage.size(); ++index)
	{
		SAFE_DELETE(m_meshStorage[index]);
	}
}

////////////////////////////////////////////////////////////////////////// Forseth //////////////////////////////////////////////////////////////////////////
void SimpleRenderer::setup(uint width, uint height, std::string title)
{
	RHIInstance *ri = RHIInstance::GetInstance();
	ri->create_output(device, context, output, width, height);

	output->m_window->set_title(title.c_str());

	m_defaultRasterState = new RasterState(device);
	SetRasterState(m_defaultRasterState);

	m_genericVBO = CreateAndGetVBODynamic();
	m_genericIBO = CreateAndGetIBODynamic();
	m_genericMeshBuilder = new MeshBuilder();
	CreateBuiltInTextures();
	CreateBuiltInShaderPrograms();

	matrix_cb = new ConstantBuffer(device, &matrixData, sizeof(matrixData));
	time_cb = new ConstantBuffer(device, &timeData, sizeof(timeData));
	light_cb = new ConstantBuffer(device, &lightData, sizeof(lightData));

	//Bind constant buffers
	SetConstantBuffer(matrix_cb, MARTIX_BUFFER_INDEX);
	SetConstantBuffer(time_cb, TIME_BUFFER_INDEX);
	SetConstantBuffer(light_cb, LIGHT_BUFFER_INDEX);

	m_defaultDepthStencil = new Texture2D(device, width, height, RHIEnum::IMAGEFORMAT_D24S8);
	mo_currentDepthStencil = nullptr;

	m_depthStencilDesc = new DepthStencilDesc();
	m_depthStencilDesc->depthTestEnabled = true;
	m_depthStencilDesc->depthWritingEnabled = true;
	m_depthStencilDesc->depthFunction = RHIEnum::DEPTH_LESS;
	m_depthStencilState = new DepthStencilState(device, *m_depthStencilDesc);
	SetDepthStencilState(m_depthStencilState);

	SetSampler(RHIEnum::FILTER_POINT, RHIEnum::FILTER_POINT);

	SetRenderTarget(nullptr, nullptr);
}

void SimpleRenderer::destroy()
{
	SAFE_DELETE(m_defaultRasterState);
	SAFE_DELETE(m_sampler);

	SAFE_DELETE(matrix_cb);
	SAFE_DELETE(time_cb);
	SAFE_DELETE(light_cb);

	SAFE_DELETE(m_currentBlendState);
	cleanUpLoadedTextures();
	cleanUpLoadedShaders();
	cleanUpLoadedFonts();
	cleanUpLoadedBitmapFonts();
	cleanUpLoadedMeshs();
	cleanUpLoadedVertexBuffers();
	SAFE_DELETE(m_defaultDepthStencil);
	SAFE_DELETE(m_depthStencilState);
	SAFE_DELETE(m_depthStencilDesc);
	SAFE_DELETE(m_genericVBO);
	SAFE_DELETE(m_genericIBO);
	SAFE_DELETE(m_genericMesh);
	SAFE_DELETE(m_genericMeshBuilder);
	SAFE_DELETE(context);
	SAFE_DELETE(output);
	SAFE_DELETE(device);
}

void SimpleRenderer::processMessages()
{
	output->m_window->processMsgs();
}

bool SimpleRenderer::isClosed() const
{
	return !output->m_window->isOpen();
}

void SimpleRenderer::present()
{
	output->present();
}

void SimpleRenderer::createWindowOutput(RHIOutput* out_output, IntVector2 size, IntVector2 position, std::string title /*= "Secondary Window"*/)
{
	// First, create the window
	Window* window = new Window();
	window->set_client_position(position.x, position.y, size.x, size.y);
	window->open();
	window->show();
	window->set_title(title.c_str());

	out_output = device->CreateOutput(window);
}
