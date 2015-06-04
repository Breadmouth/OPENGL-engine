#include "CheckerBoard.h"

CheckerBoard::CheckerBoard()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (j < 3)
				m_board[i][j] = BLACK;
			else if (j < 5)
				m_board[i][j] = EMPTY;
			else
				m_board[i][j] = WHITE;
		}
	}
}

CheckerBoard::~CheckerBoard()
{

}

void CheckerBoard::Create()
{

}

void CheckerBoard::Draw()
{
	//Gizmos::addCylinderFilled(glm::vec3(0, 0, 0), 10.f, 10.f, 4, glm::vec4(1, 1, 1, 1));
}


void CheckerBoard::Update(float dt)
{
	if (m_playerTurn)
	{
		//wait for player input
	}
	else
	{
		//ai make decision
	}
}

void CheckerBoard::Destroy()
{

}

void CheckerBoard::ClearBoard()
{

}

