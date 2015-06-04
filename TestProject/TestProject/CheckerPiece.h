#pragma once

static struct Piece
{
	int m_x;
	int m_y;
	bool m_alive = true;
	//false is regular piece
	//true for king
	bool m_type = false;
};