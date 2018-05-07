#pragma once
#include "Engine/Core/EngineBase.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class Pawn;
//////////////////////////////////////////////////////////////////////////
class Controller
{
public:
	Controller();
    ~Controller(){}

	virtual void Render() const {}
	virtual void Act()=0;

	virtual void Possess(Pawn* pawnToControl) { mo_controlledPawn = pawnToControl; };
	virtual void Reset() { mo_controlledPawn = nullptr; }
public:
	Pawn* mo_controlledPawn = nullptr;
	uint m_controllerId;
    static std::vector<Controller*> s_controllers;
};