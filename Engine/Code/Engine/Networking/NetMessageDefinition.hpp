#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/Event.hpp"
#include <functional>
//////////////////////////////////////////////////////////////////////////
class NetMessage;
//////////////////////////////////////////////////////////////////////////
class NetMessageDefinition
{
public:
	NetMessageDefinition() {}
	~NetMessageDefinition() {}
	
public:
	uint8 type_index;

	//Event<NetMessage*> handler;
	 std::function<void(NetMessage*)> handler;
};