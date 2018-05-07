#pragma once
#include "Game/Controller.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>
//////////////////////////////////////////////////////////////////////////
class PlayerController : public Controller
{
public:
    PlayerController();
	~PlayerController() {}

	void Render() const;
    void Act();
	void Possess(Pawn* pawnToControl) override;
	void Reset() override;
public:
	bool m_isMyTurn = false;

private:
	void MoveState();
	bool AttackState();
	void SelectionInput();

	void getSelectableTilesForMovement();
	void getSelectableTilesForAttack();
	bool isInSelectableList(const IntVector2& selectedTileCoords) const;

	void SetCamera();

	void drawMoveState() const;
	void drawAttackState() const;
private:
	bool m_inAttackState = false;

	IntVector2 m_selectionOffset = IntVector2(0, 0);
	std::vector<IntVector2> m_selectableTileCoords;
};