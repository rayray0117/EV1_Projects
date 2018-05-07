#pragma once
#include "Game/Actor.hpp"

#include "Game/FeatureDefinition.hpp"
//////////////////////////////////////////////////////////////////////////
class Feature : public Actor
{
public:
	Feature();
	Feature(std::string defName);
	~Feature();

	//virtual void Update(float deltaSeconds);
    virtual void Render() const;
	void DebugRender() const;
	void Redraw();

public:
	bool m_isSolid = true;
	FeatureDefinition* mo_def = nullptr;
	bool m_isExit = false;
	std::string m_exitToMapName;
};

