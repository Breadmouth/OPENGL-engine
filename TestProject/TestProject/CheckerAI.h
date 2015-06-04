#pragma once
#include "CheckerPiece.h"

class CheckerAI
{
public:
	CheckerAI();
	~CheckerAI();

	void Update();

	Piece* GetPieceAtPos(int x, int y);
	Piece* GetPiece(int i);

protected:
	Piece m_pieces[12];

	void MakeDecision();
};