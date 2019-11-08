/* Represents the player move */
#pragma once
#include "definitions.h"
#include "Point.h"
using namespace std;
class Action
{
private:
public:
	Point p;
	CellState player;

	Action(); // default constructor
			// construct with point played
	Action(CellState player, int x, int y);
	Action(CellState player, Point p);
	Action(CellState player); // pass move
	bool set_Move(int x, int y);
	Point getMove() const;
	CellState getColour() const;
	bool isPass() const;
    friend ostream& operator<<(ostream& os, const Action& action);

	~Action();
};
