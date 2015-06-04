#pragma once
#include "CheckerAI.h"
#include "Gizmos.h"
#include "Application.h"

static enum PieceType {EMPTY, WHITE, BLACK, WHITEKING, BLACKKING};

class CheckerBoard : public Application
{
public:
	CheckerBoard();
	~CheckerBoard();

	virtual void Create();
	virtual void Destroy();
	virtual void Update(float dt);
	virtual void Draw();

	void ClearBoard();

protected:
	PieceType m_board[4][8];

	CheckerAI* m_ai;

	bool m_playerTurn;
};