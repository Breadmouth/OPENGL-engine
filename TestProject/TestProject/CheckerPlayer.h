#pragma once
#include "CheckerPiece.h"
#include "glm\vec2.hpp"

class CheckerPlayer
{
public:
	CheckerPlayer();
	~CheckerPlayer();

	void Update();

	Piece* GetPieceAtPos(int x, int y);
	Piece* GetPiece(int i);

protected:
	Piece m_pieces[12];

	glm::vec2 m_cursor;

	float m_selectCooldown;
};