#include "Game/PlayerController.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Pawn.hpp"
#include "Game/Tile.hpp"
#include "Game/World.hpp"
#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Camera2D.hpp"
//////////////////////////////////////////////////////////////////////////
const Rgba MOVEMENT_TILE_COLOR = Rgba(200, 255, 200, 128);//Rgba(175, 206, 255, 128);
const Rgba ATTACK_TILE_COLOR = Rgba(255, 200, 200, 128);//Rgba(255, 124, 122, 128);
//////////////////////////////////////////////////////////////////////////
PlayerController::PlayerController()
    :Controller()
{
}
//////////////////////////////////////////////////////////////////////////
void PlayerController::Render() const
{
	if (!m_isMyTurn)
		return;

	if (m_inAttackState)
	{
		drawAttackState();
	}
	else
	{
		drawMoveState();
	}
}

void PlayerController::Act()
{
	m_isMyTurn = true; //If this is called then someone I control is up meaning it is my turn.

	SetCamera();


    bool endTurn = false;

	if (wasInputJustPressed(INPUT_REST))
	{
		endTurn = true;
		m_selectionOffset = IntVector2::ZERO;
		m_selectableTileCoords.clear();
		m_inAttackState = false;
	}
	else
	if (wasInputJustPressed(INPUT_CAMERA))
	{
		g_theGame->m_theCamera->ToggleViewEntireMap();
	}
	else 
	if (m_inAttackState)
	{
		endTurn = AttackState();
	}
	else
	{
		if (m_selectableTileCoords.empty())
			getSelectableTilesForMovement();

		MoveState();
	}

    if (endTurn)
    {
		Map* currentMap = mo_controlledPawn->mo_currentMap;
		m_isMyTurn = false; //I'm ending my turn so make sure I know that.
        currentMap->NextPawn();
    }
}

void PlayerController::Possess(Pawn* pawnToControl)
{
	Controller::Possess(pawnToControl);
	m_inAttackState = false;
	m_selectionOffset = IntVector2::ZERO;
	m_selectableTileCoords.clear();
}

void PlayerController::Reset()
{
	Controller::Reset();
	m_inAttackState = false;
	m_selectionOffset = IntVector2::ZERO;
	m_selectableTileCoords.clear();
	m_isMyTurn = false;
}

void PlayerController::MoveState()
{
	SelectionInput();

	if (wasInputJustPressed(INPUT_ACCEPT))
	{
		IntVector2 tileCoords = mo_controlledPawn->getTileCoords();
		IntVector2 selectedTileCoords = tileCoords + m_selectionOffset;


		if (isInSelectableList(selectedTileCoords))
		{
			mo_controlledPawn->MoveToTile(selectedTileCoords);
			mo_controlledPawn->mo_currentMap->setVisibleTiles();
			g_theGame->Redraw();
			m_inAttackState = true;
			m_selectionOffset = IntVector2::ZERO;
			m_selectableTileCoords.clear();
			getSelectableTilesForAttack();
		}
	}
	else if (wasInputJustPressed(INPUT_LMB))
	{
		const IntVector2 selectedTileCoords = IntVector2(mo_controlledPawn->mo_currentMap->getCursorMapPos());

		if (isInSelectableList(selectedTileCoords))
		{
			mo_controlledPawn->MoveToTile(selectedTileCoords);
			mo_controlledPawn->mo_currentMap->setVisibleTiles();
			g_theGame->Redraw();
			m_inAttackState = true;
			m_selectionOffset = IntVector2::ZERO;
			m_selectableTileCoords.clear();
			getSelectableTilesForAttack();
		}
	}
}

bool PlayerController::AttackState()
{
	//*/
	if (m_selectableTileCoords.empty()) //If I couldn't attack anywhere then just skip this phase.
	{
		m_selectionOffset = IntVector2::ZERO;
		m_selectableTileCoords.clear();
		m_inAttackState = false;
		return true;
	}
	//*/

	SelectionInput();
	//*/
	if (wasInputJustPressed(INPUT_ACCEPT))
	{
		IntVector2 tileCoords = mo_controlledPawn->getTileCoords();
		IntVector2 selectedTileCoords = tileCoords + m_selectionOffset;

		if (isInSelectableList(selectedTileCoords))
		{
			mo_controlledPawn->AttackTile(selectedTileCoords);
			m_inAttackState = false;
			m_selectionOffset = IntVector2::ZERO;
			m_selectableTileCoords.clear();
			return true;
		}
	}
	else if(wasInputJustPressed(INPUT_LMB))
	{
		const IntVector2 selectedTileCoords = IntVector2(mo_controlledPawn->mo_currentMap->getCursorMapPos());

		if (isInSelectableList(selectedTileCoords))
		{
			mo_controlledPawn->AttackTile(selectedTileCoords);
			m_inAttackState = false;
			m_selectionOffset = IntVector2::ZERO;
			m_selectableTileCoords.clear();
			return true;
		}
	}
	
	return false;
	/*/
	m_inAttackState = false;
	return true;
	//*/
}

void PlayerController::SelectionInput()
{
	if (wasInputJustPressed(INPUT_RIGHT))
	{
		m_selectionOffset.x++;
	}
	else if (wasInputJustPressed(INPUT_LEFT))
	{
		m_selectionOffset.x--;
	}
	else if (wasInputJustPressed(INPUT_UP))
	{
		m_selectionOffset.y++;
	}
	else if (wasInputJustPressed(INPUT_DOWN))
	{
		m_selectionOffset.y--;
	}
}
////////////////////////////////////////////////////////////////////////// Helpers for getting usable tiles //////////////////////////////////////////////////////////////////////////
void PlayerController::getSelectableTilesForMovement()
{
	std::vector<Tile*> neighbors;
	//Assume everyone just moves in a radius. If time expand to different types of movement like cross and square.
	mo_controlledPawn->mo_currentMap->getNeighbors_Radial(mo_controlledPawn->getTileCoords(), neighbors, mo_controlledPawn->getCombinedStats().m_stats[STAT_MOVEMENT]);

	for (Tile* current : neighbors)
	{
		if(current == nullptr)
			continue;
		if(current->isOpen() && current->m_visibilty == TILE_VIS_IS_SEEN) //Only add the ones that exist, are open, can be seen. May need to add check if there is an enemy in my way too.S
			m_selectableTileCoords.push_back(current->m_tileCoords);
		else if(current->mo_pawn == mo_controlledPawn)
			m_selectableTileCoords.push_back(current->m_tileCoords); //If the tile wasn't open was it because I'm not it?
	}
}

void PlayerController::getSelectableTilesForAttack()
{
	std::vector<Tile*> neighbors;
	//Assume everyone just attacks in a radius. If time expand to different types like cross and square.
	mo_controlledPawn->mo_currentMap->getNeighbors_Radial(mo_controlledPawn->getTileCoords(), neighbors, 1);//Assume a basic attack which only hits targets right next to you.

	for (Tile* current : neighbors)
	{
		if (current == nullptr)
			continue;
		if (current->mo_pawn == mo_controlledPawn)
			continue;
		if (current->m_visibilty == TILE_VIS_IS_SEEN) //Only add the ones that exist, and can be seen. May need to add check if there is an enemy in my way too.S
			m_selectableTileCoords.push_back(current->m_tileCoords);
		/*/
		else if (current->mo_pawn == mo_controlledPawn)
			m_selectableTileCoords.push_back(current->m_tileCoords); //If the tile wasn't open was it because I'm not it?
		//*/
	}
}

bool PlayerController::isInSelectableList(const IntVector2& selectedTileCoords) const
{
	for (uint i = 0; i < m_selectableTileCoords.size(); i++)
	{
		if (selectedTileCoords == m_selectableTileCoords[i])
			return true;
	}

	return false;
}

void PlayerController::SetCamera()
{
	// Set the camera to the current controlled pawn's location even if I'm not using this view.
	g_theGame->m_theCamera->SetPosition(mo_controlledPawn->getTileCoords());
	g_theGame->m_theCamera->SetViewRange(mo_controlledPawn->getCombinedStats().m_stats[STAT_SIGHT] + mo_controlledPawn->mo_currentMap->m_sightBonus);
}

void PlayerController::drawMoveState() const
{
	for (uint i = 0; i < m_selectableTileCoords.size(); i++)
	{
		Tile* tile = mo_controlledPawn->mo_currentMap->getTile(m_selectableTileCoords[i]);
		if (!tile) //Should be a tile but just to be safe.
			continue;

		tile->drawIsSelectableOverlay(MOVEMENT_TILE_COLOR);
	}

	IntVector2 mapPos = mo_controlledPawn->getTileCoords() + m_selectionOffset;
	g_renderer->drawThickAABB2(mapPos, mapPos + IntVector2::ONE, .125f, Rgba::GREEN, Rgba(0, 0, 0, 0));
}

void PlayerController::drawAttackState() const
{
	for (uint i = 0; i < m_selectableTileCoords.size(); i++)
	{
		Tile* tile = mo_controlledPawn->mo_currentMap->getTile(m_selectableTileCoords[i]);
		if (!tile) //Should be a tile but just to be safe.
			continue;

		tile->drawIsSelectableOverlay(ATTACK_TILE_COLOR);
	}

	IntVector2 mapPos = mo_controlledPawn->getTileCoords() + m_selectionOffset;
	g_renderer->drawThickAABB2(mapPos, mapPos + IntVector2::ONE, .125f, Rgba::RED, Rgba(0, 0, 0, 0));
}
