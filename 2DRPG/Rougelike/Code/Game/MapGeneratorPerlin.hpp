#pragma once
#include "Game/MapGenerator.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
struct Condition
{
	Condition(const XMLNode& element);

	const bool isInRange(float value) const;
	void ChangeIfValidTile(Map* mapToEdit, const IntVector2& tileCoords);

	Vector2 ifNoise;
	std::string changeToType = "";
	std::string ifTile = "";
	float chanceToRun = 1.f;
};
//////////////////////////////////////////////////////////////////////////
class MapGeneratorPerlin : public MapGenerator
{
public:
	MapGeneratorPerlin(const XMLNode& element);
	~MapGeneratorPerlin() {};
	
	void Generate(Map* mapToEdit) override;
public:
	float m_scale = 1.f;
	uint m_numOctaves = 1;
	float m_octavePersistence = 0.5f;
	float m_octaveScale = 2.f;
	bool m_renormalize = true;
	uint m_seed = 0;
private:
	IntVector2 m_dimensions;
	IntVector2 m_offset;
	std::vector<Condition> m_rules;
};