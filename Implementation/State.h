#pragma once
#include "definitions.h"
#include "Action.h"
#include <vector>
using namespace std;
class State : public Board
{
private:
	/* data */
	void init();
	CapturedStones capturedStones;
	//Player player;  // The player who made the move to this state. [USELESS DATA].
					// CellState nextPlayerToPlay; // the color of next player to play, initial state will be white and alternate every time
	CellState color;		// NOTE: color in state means the color which make that state. not the color which is ready to play.
							// NOTE: the color of the state is totally useless for MCTS. It's needed to communicate with the engine.
public:
	vector<Point>last_captured_positions;
	Action actionMakeState; // action that make this state
	CapturedStones getCapturedstones() const;
	State(const Board & m, int capturedByBlack, int capturedByWhite);
	State(const Board & m);
	State();
	// State(const State& ref);
	CellState& operator() (int row, int col);
	CellState operator() (int row, int col) const;
	State operator+(Action m);
	State& operator+=(const Action& m);
	State& operator-=(const Action& m);
	void setCapturedStones(int white, int black);
	// H&H
											//void get_actions(vector<Action>&);      // Add all possible actions to the passed vector.         // CHANGED TO GAME ENGINE.
	// void apply_action(Action actionToApply, GoEngine engine);              // apply certain action to this state using the engine.
	friend ostream& operator<<(ostream& os, const State& state);
	void set_color(CellState col) { this->color = col; }
	void clear();
	CellState get_color() { return color; }


	~State();
};
