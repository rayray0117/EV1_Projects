#include "Engine/UI/UIElement.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Core/EngineConfig.hpp"
//////////////////////////////////////////////////////////////////////////
UIElement::UIElement()
{

}

UIElement::~UIElement()
{
	RemoveSelf();
	DestroyChildren();
}
//////////////////////////////////////////////////////////////////////////
void UIElement::DebugRender() const
{
	g_mainEngineRenderer->drawAABB2(getLocalBounds(), m_debugInnerBoundsColor);
	for (UIElement* current : m_children)
	{
		current->DebugRender();
	}
}

//////////////////////////////////////////////////////////////////////////
void UIElement::AddChildElement(UIElement* child)
{
	m_children.push_back(child);
}

void UIElement::RemoveChildElement(UIElement* child)
{
	for (uint i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] == child)
		{
			m_children[i] = m_children.back();
			m_children.pop_back();
		}
	}
}

void UIElement::RemoveSelf()
{
	if (m_parent == nullptr) return;

	m_parent->RemoveChildElement(this);
}

void UIElement::DestroyChildren()
{
	while (!m_children.empty())
	{
		delete m_children.front();
	}
}

UIElement* UIElement::getParent() const
{
	return m_parent;
}
//////////////////////////////////////////////////////////////////////////
Vector2 UIElement::getSize() const
{
	return getParentSize() * m_position.ratio + m_position.unit;
}

Vector2 UIElement::getParentSize() const
{
	if (nullptr == m_parent)
	{
		return Vector2::ZERO;
	}
	else
	{
		return m_parent->getSize();
	}
}

AABB2 UIElement::getLocalBounds() const
{
	Vector2 size = getSize();
	Vector2 pivotPoint = size * m_pivot;

	AABB2 bounds(Vector2::ZERO, size);
	bounds.translate(pivotPoint);
	return bounds;
}
