#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/RHI/RHITypes.hpp"
#include <string>
#include <vector>
#include <map>
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MatrixStack.hpp"
//////////////////////////////////////////////////////////////////////////
class ShaderProgram;
class Sampler;
class VertexBuffer;
class IndexBuffer;
class Mesh;
class MeshBuilder;
class Texture2D;
class Skeleton;
class SkeletonInstance;

class RHIDeviceContext;
class RHIDevice;
class RHIOutput;

class Vector2;
class IntVector2;
class Vector3;
class Vector4;
class Quaternion;
class Rgba;
struct Vertex3D;
struct Vertex2D;
class AABB2;
class BitmapFont;
class Font;
class RasterState;
class BlendState;
class ConstantBuffer;
class DepthStencilState;
class StructuredBuffer;
struct DepthStencilDesc;
struct RenderableString;
typedef Texture2D UAVTexture;
//////////////////////////////////////////////////////////////////////////
constexpr int MARTIX_BUFFER_INDEX = 0;
constexpr int TIME_BUFFER_INDEX = 1;
constexpr int LIGHT_BUFFER_INDEX = 2;

constexpr uint DIFFUSE_TEXTURE_INDEX = 0;
constexpr uint NORMAL_TEXTURE_INDEX = 1;
constexpr uint SPECULAR_TEXTURE_INDEX = 2;

constexpr char* DEFAULT_UNLIT_SHADER = "default_unlit";
constexpr char* DEFAULT_LIT_SHADER = "default_lit";
constexpr char* DEFAULT_UNTEXTURED_SHADER = "default_untextured";
constexpr char* PARTICLE_UNLIT_SHADER = "unlit_particle";
//////////////////////////////////////////////////////////////////////////
struct matrix_buffer //#ToDo
{
	Matrix4 model;
	Matrix4 view;
	Matrix4 projection;
};

struct time_buffer
{
	float game_time;
	float system_time;
	float game_frame_time;
	float system_frame_time;
};

struct light_struct
{
	light_struct()
		: light_color(1, 1, 1, 0)
		, light_position(0, 0, 0, 1)
		, light_direction(0.f, 0.f, 0.f, 0.f)
		, attenuation(0, 0, 0)     // will effectively dampen the light - at intensity 1, this will make the light constant
		, angle(0.f)
		, spec_attenuation(0, 0, 0, 0)
	{}
	
	Vector4 light_color; // <r, g, b, intensity>
	Vector4 light_direction; // <x, y, z, padding>
	Vector4 light_position; // <x, y, z, padding>  // variables can not cross a 16-byte boundary, so we pad out 
	Vector3 attenuation;    // <constant, linear, quadratic, unused>
	float angle;
	Vector4 spec_attenuation; // <constant, linear, quadratic, unused>
};

struct light_buffer
{
	light_buffer()
		: ambient(1, 1, 1, 1)
		, spec_factor(0.0f)
		, spec_power(1.0f)
	{}

	Vector4 ambient;  // <r, g, b, intensity>

	light_struct lights[8];

	float spec_factor;
	float spec_power;
	Vector2 unused;

	Vector4 eye_position;
};

struct blend_state
{
	bool enabled;
	RHIEnum::BlendFactor src_factor;
	RHIEnum::BlendFactor dest_factor;
};


//////////////////////////////////////////////////////////////////////////
class SimpleRenderer
{
public:
	SimpleRenderer();
	~SimpleRenderer();
	////////////////////////////////////////////////////////////////////////// Renderer //////////////////////////////////////////////////////////////////////////
public:
	// Clears
	void ClearScreen(const Rgba& clearColor);
	void ClearScreen(const Rgba& clearColor, Texture2D* renderTarget);
	void ClearDepth(float depth = 1.0f, unsigned int stencil = 0);

	// Capabilities
	void EnableCapability(RHIEnum::Capability cap);
	void DisableCapability(RHIEnum::Capability cap);
	void EnableBackFaceCulling(bool isNowBackFaceCulling = true);
	void EnableWireframeCulling(bool isNowWireFrame = true);
	void EnableDepthTesting(bool isNowDepthTesting = true);
	void EnableDepthWriting(bool isNowDepthWritng = true);
	void EnableDepth(bool useDepth = true);
	void DisableBlendState();

	void EnableLight(uint lightNumber, light_struct light);
	void EnablePointLight(const Vector3& position, const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& specAttenuation = Vector3(0.f, 0.f, 1.f), uint lightNum = 2);
	void EnableDirectionalLight(const Vector3& direction, const Rgba& color, uint lightNum = 0);
	void EnableSpotLight(const Vector3& position, const Vector3& direction, float angle, const Rgba& color, float intensity, const Vector3& attenuation, const Vector3& specAttenuation = Vector3(0.f, 0.f, 1.f), uint lightNum = 1);

	// Bindings
	void BindTexture(const Texture2D* texture, uint index = 0);
	void BindTexture(const std::string& textureName, uint index = 0);
	void BindShader(ShaderProgram* shader);
	void BindShader(const std::string& shaderName);
	void BindBuffer(unsigned int bufferId, RHIEnum::BufferType bufferType = RHIEnum::BUFFER_ARRAY);

	// Set views, colors, widths
	void SetOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight);
	void SetPerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float zNear, float zFar);
	void SetViewport(int x, int y, int width, int height);
	void SetLineWidth(float lineThickness);
	void SetColor(const Rgba& color);
	void SetBlendFunc(RHIEnum::BlendFactor sourceFactor, RHIEnum::BlendFactor destinationFactor);
	void SetDepthFunc(RHIEnum::DepthFunction func);
	void SetSampler(const Sampler* sampler, unsigned int index = 0);
	void SetSampler(RHIEnum::FilterMode min, RHIEnum::FilterMode max, uint index = 0);
	void SetConstantBuffer(const ConstantBuffer* constBuffer, unsigned int index);
	void SetStructuredBuffer(const StructuredBuffer* structBuffer, uint index = 5);
	void SetRenderTarget(Texture2D* renderTarget, Texture2D* depthStencil = nullptr);
	void SetDepthStencilState(DepthStencilState* depthStencilState);
	void SetRasterState(RasterState* rasterState = nullptr);

	void SetAmbientLight(float intensity, const Rgba& color);
	void SetSpecularConstants(float specPower, float specFactor);
	void SetEyePosition(const Vector3& pos);

	void LoadMatrix(const Matrix4& matrix);
	void SetMatrixMode(RHIEnum::MatrixMode mode) { m_matrixMode = mode; }
	void SetProjectionMatrix(const Matrix4& projection);
	void SetViewMatrix(const Matrix4& view);
	void SetModelMatrix(const Matrix4& model);

	// Buffer/VBO stuff
	void SetBufferData(const void * data, int size, RHIEnum::UsageType usageDrawHint, RHIEnum::BufferType bufferType = RHIEnum::BUFFER_ARRAY);
	void SetVBO3DData(VertexBuffer* vboId, const Vertex3D* vertexArray, int numVertexes, RHIEnum::UsageType usageDrawHint = RHIEnum::USAGE_DYNAMIC_DRAW, RHIEnum::BufferType bufferType = RHIEnum::BUFFER_ARRAY);
	void SetIBO3DData(IndexBuffer* iboId, const unsigned int* indexArray, int numIndexes, RHIEnum::UsageType usageDrawHint = RHIEnum::USAGE_DYNAMIC_DRAW, RHIEnum::BufferType bufferType = RHIEnum::BUFFER_ARRAY);
	void GenerateBuffers(unsigned int* buffers, int numberOfBuffers);
	void DeleteBuffers(unsigned int* buffers, int numberOfBuffers);
	void DeleteVBO(VertexBuffer* vboId);
	void DeleteIBO(IndexBuffer* iboId);

	void SetVBO3DData(unsigned int vboId, const Vertex3D* vertexArray, int numVertexes, RHIEnum::UsageType usageDrawHint = RHIEnum::USAGE_DYNAMIC_DRAW, RHIEnum::BufferType bufferType = RHIEnum::BUFFER_ARRAY);
	void DeleteVBO(unsigned int vboId);

	void SetMeshData(unsigned int meshId, MeshBuilder* mb, RHIEnum::UsageType usageDrawHint = RHIEnum::USAGE_STATIC_DRAW);

	// Matrix manipulations
	void pushMatrix();
	void popMatrix();
	void translate2D(const Vector2& pos);
	void translate2D(float x, float y);
	void translate3D(const Vector3& pos);
	void translate3D(float x, float y, float z);
	void rotate2D(float degrees);
	void rotate3D(float degrees, const Vector3& rotationAxis);
	void rotate3D(float degrees, float x, float y, float z);
	void rotate3D(const Quaternion& rotation);
	void scale2DUniform(float scale);
	void scale3DUniform(float scale);
	void scale3D(const Vector3& scales);
	void transform(const Matrix4& transformMatrix);
	void rotateAroundPosition2D(const Vector2& position, float degrees);

	// Base Draw Functions
	void drawVertexArray3D(const Vertex3D* vertexArray, int numVertexes, RHIEnum::PrimativeType type);
	void drawVBO3D(VertexBuffer* vboID, int numVertexes, RHIEnum::PrimativeType type);
	void drawVBO3D(VertexBuffer* vboID, int numVertexes, RHIEnum::PrimativeType type, int first);
	void drawIBO3D(VertexBuffer* vboID, IndexBuffer* iboId, int numVertexes, RHIEnum::PrimativeType type);
	void drawIBO3D(VertexBuffer* vboID, IndexBuffer* iboId, int numVertexes, RHIEnum::PrimativeType type, int startVertex, uint startIndex = 0);
	void drawMesh(Mesh* mesh);

	void drawVBO3D(unsigned int vboID, int numVertexes, RHIEnum::PrimativeType type);
	void drawVBO3D(unsigned int vboID, int numVertexes, RHIEnum::PrimativeType type, int first);
	void drawMesh(unsigned int meshID);

	// CreateOrGet
	Texture2D* CreateOrGetTexture(const std::string& imageFilePath);
	BitmapFont* CreateOrGetBitmapFont(const std::string& bitmapFontName);
	Font* CreateOrGetFont(const std::string& fontName);
	ShaderProgram* CreateOrGetShader(const std::string& filePath);

	// CreateAndGet
	VertexBuffer* CreateAndGetVBODynamic();
	VertexBuffer* CreateAndGetVBOStatic(std::vector<Vertex3D> vertices);
	IndexBuffer* CreateAndGetIBODynamic();
	IndexBuffer* CreateAndGetIBOStatic(std::vector<unsigned int> indices);
	Mesh* CreateAndGetMeshDynamic();
	Mesh* CreateAndGetMeshStatic(MeshBuilder* mb);
	Mesh* CreateAndGetSkeletonMesh(Skeleton* skeleton);
	Sampler* CreateAndGetSampler();
	StructuredBuffer* CreateAndGetStructedBuffer(std::vector<Matrix4> vertices);
	UAVTexture* CreateAndGetUAVTexture(uint width, uint height);

	unsigned int CreateAndGetVBOId();
	unsigned int CreateAndGetMeshId();

	// Primitive Draw Functions
	void drawLine(const Vector2& startPos, const Vector2& endPos, const Rgba& startColor, const Rgba& endColor);
	void drawLine(const Vector3& startPos, const Vector3& endPos, const Rgba& startColor, const Rgba& endColor);
	void drawLine(const Vector2& startPos, const Vector2& endPos, const Rgba& color);
	void drawLine(const Vector3& startPos, const Vector3& endPos, const Rgba& color);
	void drawLineWithDirection(const Vector3& startPos, const Vector3& direction, const Rgba& color, float length = 1.f);
	void drawLineAndSetLineWidth(const Vector2& startPos, const Vector2& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness);
	void drawLineAndSetLineWidth(const Vector3& startPos, const Vector3& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness);
	void drawLineStrip(const Vertex2D* vertexes, int numVertexes);
	void drawThickLine(const Vector2& startPos, const Vector2& endPos, float thickness, const Rgba& startColor, const Rgba& endColor);

	void drawTexturedAABB2(const AABB2& bounds, const Texture2D& texture, const AABB2& textureCoords);
	void drawTexturedAABB2(const AABB2& bounds, const Texture2D& texture, const Rgba& tint);
	void drawTexturedAABB2(const AABB2& bounds, const AABB2& textureCoords, const Rgba& tint);
	void drawTexturedSquare(const Vector2& offset, float scale, const AABB2& textureCoords, const Rgba& tint);

	void drawAABB2(const AABB2& bounds, const Rgba& color);
	void drawAABB2(const Vector2& mins, const Vector2& maxs, const Rgba& color);
	void drawAABB2(const IntVector2& mins, const IntVector2& maxs, const Rgba& color);
	void drawThickAABB2(const Vector2& mins, const Vector2& maxs, float thickness, const Rgba& edgeColor, const Rgba& fillColor);


	void drawThickCircle(const Vector2& center, float radius, float thickness, const Rgba& edgeColor, const Rgba& fillColor, float numSides = 64.f);
	void drawDisc(const Vector2& center, float radius, const Rgba& color, float numSides = 64.f);

	void drawAxes(float axisLength, float alpha = 1.f);
	void drawGridXZ(float axisLength, float alpha = 1.f);

	void drawQuads(const Vertex3D* vertexes, int numVertexes);
	void drawTexturedQuads(const Vertex3D* vertexes, int numVertexes, const Texture2D& texture);
	void drawTexturedQuads(const Vertex3D* vertexes, int numVertexes);

	void drawDebugTriDirectionalPlane(const Vector3& startPos, const Vector3& direction1, const Vector3& direction2, const Rgba& color, float length = 1.f);
	void drawTri(const Vector3& startPos, const Vector3& midPos, const Vector3& endPos, const Rgba& color);

	//Bitmap Fonts
	void DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint, float cellAspectScale = 1.f, const BitmapFont* font = nullptr);
	void drawText2DWithBitmapFont_Centered(const Vector2& bottomCenter, const std::string& asciiText, float cellHeight, const Rgba& tint, float cellAspectScale = 1.f, const BitmapFont* font = nullptr);
	void drawStringWithBitmapFont(const Vector2& startBottomLeft, const RenderableString& str, const BitmapFont* font = nullptr, float cellAspectScale = 1.f);
	void drawStringsWithBitmapFont(const Vector2& startBottomLeft, const std::vector<RenderableString>& str, const BitmapFont* font = nullptr, float cellAspectScale = 1.f);
	// Fonts
	void drawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float scale, const Rgba& tint, const std::string& fontName);

	void drawSkeletonInstance(const SkeletonInstance& skeleton, const Mesh& skelMesh);
	void drawSkinMeshSkel(const SkeletonInstance& skeleton, Mesh* skelMesh);

	void UpdateGameTime(float deltaSeconds);
	void UpdateSystemTime(float deltaSeconds);
protected:
	Texture2D* CreateTextureFromFile(const std::string& imageFilePath);
	Texture2D* CreateTextureFromData(const std::string& name, unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);
	void CreateBuiltInTextures();
	Texture2D* GetTexture(const std::string& imageFilePath);

	ShaderProgram* CreateShaderProgramFromFile(const std::string& filePath);
	ShaderProgram* CreateShaderProgramFromString(const std::string& name, const std::string& buffer);
	void CreateBuiltInShaderPrograms();
	ShaderProgram* GetShaderProgram(const std::string& filePath);


	BitmapFont* CreateBitmapFromFile(const std::string& bitmapFontName);
	BitmapFont* GetBitmapFont(const std::string& bitmapFontName);

	Font* CreateFontFromFile(const std::string& fontName);
	Font* GetFont(const std::string& fontName);
private:
 	std::vector< Texture2D* >			m_alreadyLoadedTextures;
	std::vector< ShaderProgram* >		m_loadedShaders;
	std::vector< Font* >				m_loadedFonts;
 	std::vector<BitmapFont* >			m_loadedBitmapFonts;

	std::vector< VertexBuffer* >		m_vertexBufferStorage;
	std::vector< Mesh*>					m_meshStorage;


	Texture2D* m_plainWhiteTexture;
	Texture2D* m_plainColorTexture;
	Texture2D* m_flatNormalTexture;
	Texture2D* m_plainGrayTexture;
	VertexBuffer* m_genericVBO;
	IndexBuffer* m_genericIBO;
	Mesh* m_genericMesh;
	MeshBuilder* m_genericMeshBuilder;

	RHIEnum::MatrixMode m_matrixMode;
	MatrixStack m_projectionMatrixStack;
	MatrixStack m_viewMatrixStack;
	MatrixStack m_modelMatrixStack;
private:
	void cleanUpLoadedFonts();
	void cleanUpLoadedBitmapFonts();
	void cleanUpLoadedShaders();
	void cleanUpLoadedTextures();
	void cleanUpLoadedVertexBuffers();
	void cleanUpLoadedMeshs();

	std::vector<uint> getPrimativeIndexList(RHIEnum::PrimativeType type, int numVertexes) const;
	std::vector<uint> getQuadsIndexList(int numVertexes) const;
	std::vector<uint> getTriangleFanIndexList(int numVertexes) const;

	void UpdateMatrixBuffer();
	////////////////////////////////////////////////////////////////////////// Forseth //////////////////////////////////////////////////////////////////////////
public:
	void setup(uint width, uint height, std::string title = "Game Window");
	// void setup( Window *window );
	// void setup( Texture2D *default_render_target );

	void destroy();

	void processMessages();

	bool isClosed() const;

	void present();

	void createWindowOutput(RHIOutput* out_output, IntVector2 size, IntVector2 position, std::string title = "Secondary Window");
public: //#ToDo: determine if I want these to be public or close them off.
	// RHI needed stuff
	RHIDeviceContext* context;
	RHIDevice* device;
	RHIOutput* output;

	Texture2D* mo_currentRenderTarget;

	Texture2D* m_defaultDepthStencil;
	Texture2D* mo_currentDepthStencil;
	
	Sampler* m_sampler;

	RasterState* m_defaultRasterState;

	blend_state m_blendState;
	BlendState* m_currentBlendState;

	DepthStencilDesc* m_depthStencilDesc;
	DepthStencilState* m_depthStencilState;

	matrix_buffer matrixData;
	ConstantBuffer* matrix_cb;

	time_buffer timeData;
	ConstantBuffer* time_cb;

	light_buffer lightData;
	ConstantBuffer* light_cb;
};