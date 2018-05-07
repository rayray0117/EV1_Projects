#include "Game/Behavior.hpp"

#include "Engine/Core/XMLParseHelper.h"
#include "ThirdParty/XMLParser/XMLParser.hpp"

#include "Game/WanderBehavior.hpp"
#include "Game/FleeBehavior.hpp"
#include "Game/PursueBehavior.hpp"
//////////////////////////////////////////////////////////////////////////
Behavior* Behavior::CreateBehavior(const XMLNode& element)
{
	std::string behaviorName = element.getName();
	if (behaviorName == "Wander") return new WanderBehavior(element);
	if (behaviorName == "Flee") return new FleeBehavior(element);
	if (behaviorName == "Pursue") return new PursueBehavior(element);

	return nullptr; //Maybe an error occurred?
}
