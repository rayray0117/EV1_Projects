#pragma once
#include "Engine/General/DataDrivenDefinition.hpp"
#include "Game/Sprite.hpp"
//////////////////////////////////////////////////////////////////////////
class FeatureDefinition : public DataDrivenDefinition<FeatureDefinition>
{
	friend DataDrivenDefinition<FeatureDefinition>;
public:
	~FeatureDefinition() {};
	
public:
	bool m_isSolid;
	bool m_isExit = false;
	Sprite m_sprite;
private:
	FeatureDefinition(const XMLNode& element);
};
