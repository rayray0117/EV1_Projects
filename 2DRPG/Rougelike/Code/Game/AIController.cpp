#include "Game/AIController.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Pawn.hpp"
#include "Game/Behavior.hpp"
//////////////////////////////////////////////////////////////////////////
AIController::AIController()
    :Controller()
{
}
//////////////////////////////////////////////////////////////////////////
void AIController::Act()
{
	if (mo_controlledPawn->mo_target == nullptr)
		mo_controlledPawn->FindTarget();

	Behavior* behaviorToRun = getBehaviorToRun();
	if(behaviorToRun)
		behaviorToRun->Act(*mo_controlledPawn);
    getCurrentMap()->NextPawn(); //End my turn since I don't want the game to be frozen if the AI freaks out.
}

void AIController::MoveRandomDirection()
{
    IntVector2 tileCoords = mo_controlledPawn->getTileCoords();

    int direction = getRandomIntLessThan(4);
    switch (direction)
    {
    case 0:
        tileCoords.x++; //This is currently not safe. Improve later with something like TileInfo or MoveRight.
        break;
    case 1:
        tileCoords.x--; //This is currently not safe. Improve later with something like TileInfo or MoveLeft.
        break;
    case 2:
        tileCoords.y++; //This is currently not safe. Improve later with something like TileInfo or MoveUp.
        break;
    case 3:
        tileCoords.y--; //This is currently not safe. Improve later with something like TileInfo or MoveDown.
        break;
    default: 
        /* DO NOTHING */
        break;
    }

    // Try to move to these coords, if not then just skip my turn.
    mo_controlledPawn->MoveToTile(tileCoords);
}

Behavior* AIController::getBehaviorToRun()
{
	Behavior* runBehavior = nullptr;
	float runBehaviorValue = 0;
	for (Behavior* currentBehavior : mo_controlledPawn->m_behaviors)
	{
		float currentBehaviorValue = currentBehavior->CalcUtility(*mo_controlledPawn);
		if (currentBehaviorValue > runBehaviorValue)
		{
			runBehavior = currentBehavior;
			runBehaviorValue = currentBehaviorValue;
		}
	}

	return runBehavior;
}
