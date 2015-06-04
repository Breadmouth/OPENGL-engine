#include "CheckerPlayer.h"


CheckerPlayer::CheckerPlayer()
{
	m_cursor.x = 0;
	m_cursor.y = 0;

	int k = 0;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 5; j < 8; ++j)
		{
			m_pieces[k].m_x = i;
			m_pieces[k].m_y = j;
		}
	}

	m_selectCooldown = 0;
}

CheckerPlayer::~CheckerPlayer()
{

}

void CheckerPlayer::Update()
{

}

Piece* CheckerPlayer::GetPiece(int i)
{
	return &m_pieces[i];
}

Piece* CheckerPlayer::GetPieceAtPos(int x, int y)
{
	for (int k = 0; k < 12; ++k)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 8; j++)
			{
				if (m_pieces[k].m_x == i && m_pieces[k].m_y == j)
				{
					return &m_pieces[k];
				}
			}
		}
	}

	return nullptr;
}