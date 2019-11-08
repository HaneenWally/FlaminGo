/* Represents the player move*/
#include "definitions.h"
#include "Action.h"

Action::Action(CellState player, int x, int y) : p(x, y)
{
	this->player = player;
}

Action::Action(CellState color) : p(-1, -1)
{
	this->player = color;
}
Action::Action() : p(-1,-1)
{}

bool Action::isPass() const
{
	return p.x == -1 && p.y == -1;
}

bool Action::set_Move(int x, int y)
{
	if (x < BOARD_DIMENSION && y < BOARD_DIMENSION)
	{
		this->p.x = x;
		this->p.y = y;
		return true;
	}
	return false;
}
Point Action::getMove() const
{
	return this->p;
}
CellState Action::getColour() const
{
	return this->player;
}
ostream& operator<<(ostream &out,const Action& action)
{
	out << "The Action is: " << action.p.x << " " << action.p.y << endl;
	return out;
}
Action::~Action()
{
}
