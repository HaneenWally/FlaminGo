#pragma once
#include "definitions.h"
#include "Point.h"
#include "State.h"

typedef std::pair<bool, CellState> Territory;
typedef std::vector<Territory> TerritoryRow;
typedef std::vector<TerritoryRow> TerritoryMat;

class GoEngine
{
private:
	// helpers methods
	void mergeNewVisited(TerritoryMat& territories, std::stack<Point> &newVisited, CellState territoryColor);
	void buildAllTerritories(const State& state, TerritoryMat& territories);
	CellState getOpponentColor(CellState color);
	bool isValidMove_stateUpdated(State state, const State& prevState, Action move);
	int removeCapturedHelper(State & state, Point point, CellState color);
	void checkTerritory(int x, int y, const State &state, TerritoryMat& territories);
	// bool isValidMove_ignoreKO(State* state, const State* prevState, Action move)
public:
	// constructor
	GoEngine();

	// basic functions
	Score computeScore(const State& state);
	bool isGoal(const State &currentState, Action currentMove, Action prevMove);
	bool isValidMove(State state, const State& prevState, Action move);
	bool isValidMove(const State* state, const State* prevState, Action move);
	std::vector<Action> getValidMoves(const State* state, const State* prevState, CellState nextColourToPlay);
	bool getRandomAction(Action& result, const State* state, const State* prevState, CellState playerColor);
	/*
	apply a valid passed action to the passed state. the state is changed in-place
	the difference between it and applyAction that it doesn't check if the action is valid or not (for better perf.)
	see also @applyAction
	*/
	void applyValidAction(State& state, Action action);
	/*
	apply a passed action to the passed state. the state is changed in-place
	the difference between it and applyValidAction that it checks if the action is valid or not
	returns true if the action is applied, false if the action is invalid to the current state
	see also @applyAction
	*/
	bool applyAction(State& state, const State * prevState, Action action);
	

	// may be used
	bool isSelfCapture(const State& state, Point point, CellState color);
	int removeCaptured(State & state, Point point, CellState color); // take the player stone's color
	bool isKo(const State& currentState, const State& prevState);


	// bool processMove(State& currentState, const State& prevState, Move currentMove) const; // return the new state in the currentState
	vector<Point> getEmptyCells(const State& state);

	// destructor
	~GoEngine();
};
#pragma once
