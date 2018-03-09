#pragma once
#include <string>
#include "Engine/Core/Rgba.hpp"

struct RenderableString
{
	RenderableString(const std::string& str = "", float scale = 1.f, const Rgba& color = Rgba(255, 255, 255, 255)) : m_string(str), m_scale(scale), m_color(color) {}

	std::string m_string;
	float m_scale;
	Rgba m_color;
};