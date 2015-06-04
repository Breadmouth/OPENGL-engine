#include "CheckerAI.h"

CheckerAI::CheckerAI()
{

}

CheckerAI::~CheckerAI()
{

}

void CheckerAI::Update()
{

}

Piece* CheckerAI::GetPiece(int i)
{
	return &m_pieces[i];
}

Piece* CheckerAI::GetPieceAtPos(int x, int y)
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