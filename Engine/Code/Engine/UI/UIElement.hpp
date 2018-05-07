#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/UI/UIMetric.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/Rgba.hpp"

#include <vector>
//////////////////////////////////////////////////////////////////////////
class UIElement
{
public:
	UIElement();
	~UIElement();
	
	void DebugRender() const;

	void AddChildElement(UIElement* child);
	void RemoveChildElement(UIElement* child);
	void RemoveSelf();
	void DestroyChildren();
	
	UIElement* getParent() const;

	Vector2 getSize() const;
	Vector2 getParentSize() const;

	AABB2 getLocalBounds() const;
public:
	UIMetric m_position; //I hate this name but can't think of a better one at the moment, so I'll use Forseth's.
	UIMetric m_size;
	Vector2  m_pivot; //As percents
	//AABB2    m_bounds;
	//SQT		 m_transfrom;

	UIElement* m_parent = nullptr;
	std::vector<UIElement*> m_children; // Look into changing this to a link list, based on Forseth's recommendation.

	Rgba m_debugInnerBoundsColor = Rgba::FORESTGREEN;
};