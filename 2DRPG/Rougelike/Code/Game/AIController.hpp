#pragma once
#include "Game/Controller.hpp"
//////////////////////////////////////////////////////////////////////////
class Behavior;
//////////////////////////////////////////////////////////////////////////
class AIController : public Controller
{
public:
	AIController();
	~AIController() {}
	
    void Act();

    void MoveRandomDirection();
public:
	

private:
	Behavior* getBehaviorToRun();
};