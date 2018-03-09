// This is a Forseth definition of a sprite which is slightly different than a Squirrel one.
// Also requires g_mainRenderer to be set the renderer being used. This would be fixed in Engine2 because all textures are stored in the RHI rather than individual renderers
#pragma once
#include "Engine/General/DataDrivenDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
//////////////////////////////////////////////////////////////////////////
class Texture2D;
//////////////////////////////////////////////////////////////////////////
class Sprite : public DataDrivenDefinition<Sprite>
{
friend DataDrivenDefinition;
	Sprite(const XMLNode& element);
	~Sprite() {}
public:
	Sprite(const std::string& name);

	const Texture2D& getTexture() const;
	//Don't know where I'd use this yet
	//const Vector2 getTexelDimensions() const;
public:
	AABB2 m_uv = AABB2::ZERO_TO_ONE;
protected:
	Texture2D* m_spriteTexture;
};