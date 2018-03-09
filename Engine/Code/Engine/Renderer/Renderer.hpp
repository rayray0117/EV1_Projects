#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <string>
#include <vector>
#include <map>
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
enum Capability
{
	CAPABILITY_BLEND = 0x0BE2,
	CAPABILITY_DEPTH_TEST = 0x0B71,
	CAPABILITY_LINE_SMOOTH = 0x0B20,
	CAPABILITY_POLYGON_SMOOTH = 0x0B41,
	CAPABILITY_TEXTURE_2D = 0x0DE1,
	CAPABILITY_CULL_FACE = 0x0B44,
	NUM_CAPABILITIES
};
enum BlendFactor
{
	BLEND_ZERO							= 0,
	BLEND_ONE							= 1,
	BLEND_SOURCE_COLOR					= 0x0300,
	BLEND_ONE_MINUS_SOURCE_COLOR		= 0x0301,
	BLEND_DESTINATION_COLOR				= 0x0306,
	BLEND_ONE_MINUS_DESTINATION_COLOR	= 0x0307,
	BLEND_SOURCE_ALPHA					= 0x0302,
	BLEND_ONE_MINUS_SOURCE_ALPHA		= 0x0303,
	BLEND_DESTINATION_ALPHA				= 0x0304,
	BLEND_ONE_MINUS_DESTINATION_ALPHA	= 0x0305,
	NUM_BLEND_FACTORS
};

enum DepthFunction
{
	DEPTH_NEVER	   = 0x0200,                    
	DEPTH_LESS     = 0x0201,
	DEPTH_EQUAL    = 0x0202,
	DEPTH_LEQUAL   = 0x0203,
	DEPTH_GREATER  = 0x0204,
	DEPTH_NOTEQUAL = 0x0205,
	DEPTH_GEQUAL   = 0x0206,
	DEPTH_ALWAYS   = 0x0207,
	NUM_DEPTH_FUNCTIONS
};

enum PrimativeType
{
	PRIMATIVE_QUADS = 0x0007,
	PRIMATIVE_QUADS_STRIP = 0x0008,
	PRIMATIVE_LINE_LOOP = 0x0002,
	PRIMATIVE_LINE = 0x0001,
	PRIMATIVE_LINE_STRIP = 0x0003,
	PRIMATIVE_TRIANGLE_FAN = 0x0006,
	NUM_PRIMATE_TYPES
};

enum BufferType
{
	BUFFER_ARRAY = 0x8892,
	NUM_BUFFER_TYPES
};

enum UsageType
{
	USAGE_STATIC_DRAW = 0x88E4,
	USAGE_DYNAMIC_DRAW = 0x88E8,
	NUM_USAGE_TYPES
};

enum MatrixMode
{
	MATRIX_MODELVIEW = 0x1700,
	MATRIX_PROJECTION = 0x1701,
	NUM_MATIX_MODES
};
//////////////////////////////////////////////////////////////////////////
class Renderer 
{
public:
	Renderer();

	// Clears
	void ClearScreen(const Rgba& clearColor );
	void ClearDepth();

	// Capabilities
	void EnableCapability(Capability cap);
	void DisableCapability(Capability cap);
	void EnableBackFaceCulling(bool isNowBackFaceCulling = true);
	void EnableDepthTesting(bool isNowDepthTesting = true);

	// Bindings
	void BindTexture(const Texture*);
	void BindBuffer(unsigned int bufferId, BufferType bufferType = BUFFER_ARRAY);

	// Set views, colors, widths
	void SetOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight);
	void SetPerspectiveProjection(float fovVerticalDegrees, float aspectRatio, float zNear, float zFar);
	void SetViewport(int x, int y, int width, int height);
	void SetLineWidth(float lineThickness);
	void SetColor(const Rgba& color);
	void SetBlendFunc(BlendFactor sourceFactor, BlendFactor destinationFactor);
	void SetDepthFunc(DepthFunction func);

	void LoadMatrix(const float* matrix);
	void SetMatrixMode(MatrixMode mode);

	// Buffer/VBO stuff
	void SetBufferData(const void * data, int size, UsageType usageDrawHint, BufferType bufferType = BUFFER_ARRAY);
	void SetVBO3DData(unsigned int vboId, const Vertex3D* vertexArray, int numVertexes, UsageType usageDrawHint, BufferType bufferType = BUFFER_ARRAY);
	void GenerateBuffers(unsigned int* buffers, int numberOfBuffers);
	void DeleteBuffers(unsigned int* buffers, int numberOfBuffers);
	void DeleteVBO(unsigned int vboId);

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
	void scale2DUniform(float scale);
	void scale3DUniform(float scale);
	void scale3D(const Vector3& scales);
	void rotateAroundPosition2D(const Vector2& position, float degrees);

	// Base Draw Functions
	void drawVertexArray3D(const Vertex3D* vertexArray, int numVertexes, PrimativeType type);
	void drawVBO3D(unsigned int vboID, int numVertexes, PrimativeType type);
	void drawVBO3D(unsigned int vboID, int numVertexes, PrimativeType type, int first);

	void drawLine(const Vector2& startPos, const Vector2& endPos, const Rgba& startColor, const Rgba& endColor);
	void drawLine(const Vector3& startPos, const Vector3& endPos, const Rgba& startColor, const Rgba& endColor);
	void drawLine(const Vector2& startPos, const Vector2& endPos, const Rgba& color);
	void drawLineAndSetLineWidth(const Vector2& startPos, const Vector2& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness);
	void drawLineAndSetLineWidth(const Vector3& startPos, const Vector3& endPos, const Rgba& startColor, const Rgba& endColor, float lineThickness);
	void drawLineStrip(const Vertex2D* vertexes, int numVertexes);

	// CreateOrGet
	Texture* CreateOrGetTexture(const std::string& imageFilePath);
	BitmapFont* CreateOrGetFont(const std::string& bitmapFontName);

	// CreateAndGet
	unsigned int CreateAndGetVBOId();

	//void DrawTexturedAABB2(const AABB2& bounds, const Texture& texture);
	void drawTexturedAABB2(const AABB2& bounds, const Texture& texture, const AABB2& textureCoords = AABB2::ZERO_TO_ONE);
	void drawTexturedAABB2(const AABB2& bounds, const Texture& texture, const Rgba& tint);
	void drawTexturedAABB2(const AABB2& bounds, const AABB2& textureCoords = AABB2::ZERO_TO_ONE, const Rgba& tint = Rgba::WHITE);
	void drawTexturedSquare(const Vector2& offset, float scale, const AABB2& textureCoords = AABB2::ZERO_TO_ONE, const Rgba& tint = Rgba::WHITE);

	void drawAABB2(const AABB2& bounds, const Rgba& color);
	void drawAABB2(const Vector2& mins, const Vector2& maxs, const Rgba& color);
	void drawAABB2(const IntVector2& mins, const IntVector2& maxs, const Rgba& color);


	void drawDisc(const Vector2& center, float radius, const Rgba& color, float numSides = 64.f);
	
	void drawAxes(float axisLength, float alpha = 1.f);

	void drawQuads(const Vertex3D* vertexes, int numVertexes);
	void drawTexturedQuads(const Vertex3D* vertexes, int numVertexes, const Texture& texture);
	void drawTexturedQuads(const Vertex3D* vertexes, int numVertexes);
	void drawQuads3D(const Vertex3D* vertexArray, int numVertexes);

	void DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText,float cellHeight,
		const Rgba& tint = Rgba::WHITE,float cellAspectScale = 1.f,const BitmapFont* font = nullptr);

	
	 

	// Outdated. #ToDo: Remove and replace with better functionality.
	//void drawLine(float startX, float startY, float endX, float endY);
	//void drawRegularPolygon(float centerX, float centerY, float numSides, float exRadius, float offset = 1.5f);
	//void drawCircle(float centerX, float centerY, float radius, float numSides = 64.f);
	
protected:
	Texture* CreateTextureFromFile(const std::string& imageFilePath);
	Texture* CreateTextureFromData(const std::string& name, unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);
	void CreateBuiltInTextures();
	Texture* GetTexture(const std::string& imageFilePath);
	unsigned int CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);

	BitmapFont* CreateBitmapFromFile(const std::string& bitmapFontName);
	BitmapFont* GetBitmapFont(const std::string& bitmapFontName);
private:
	std::vector< Texture* >		m_alreadyLoadedTextures;
	std::map<std::string, BitmapFont* > m_loadedFonts;
	Texture* m_plainWhiteTexture;
	unsigned int m_genericVBO;
};