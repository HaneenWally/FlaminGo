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

	// may be used
	bool getRandomAction(Action& result, const State* state, const State* prevState, CellState playerColor);
	bool isSelfCapture(const State& state, Point point, CellState color);
	int removeCaptured(State & state, Point point, CellState color); // take the player stone's color
	bool isKo(const State& currentState, const State& prevState);


	// bool processMove(State& currentState, const State& prevState, Move currentMove) const; // return the new state in the currentState


	// destructor
	~GoEngine();
};
#pragma once
