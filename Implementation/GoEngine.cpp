#include "definitions.h"
#include "GoEngine.h"
#include <stdlib.h>
#include <time.h>

#include<random>

#define right(s, m) s(m.x + 1, m.y)
#define xy(m) m.x, m.y
#define pxy(m) m.p.x, m.p.y

// const Point ADJ_OFFSETS[4] {Point(0, 1), Point(1, 0), Point(-1,0), Point(0,-1)};
const Point ADJ_OFFSETS[4] = { Point(0, -1), Point(1, 0), Point(-1,0), Point(0,1) };

bool isOnBoard(Point p) {
	return p.x < BOARD_DIMENSION && p.y < BOARD_DIMENSION && p.x >= 0 && p.y >= 0;
}

CellState GoEngine::getOpponentColor(CellState color)
{
	if (color == WHITE)
		return BLACK;
	return WHITE;
}

bool GoEngine::isSelfCapture(const State& state, Point point, CellState color)
{
	std::stack<Point> s;
	Matrix visited(BOARD_DIMENSION, std::vector<bool>(BOARD_DIMENSION, false));
	s.push(point);
	while (!s.empty())
	{
		Point currentPoint = s.top();
		s.pop();
		bool isVisited = visited[currentPoint.x][currentPoint.y];
		if (isVisited) // visited from another function call
			continue;
		// first time we see this point
		visited[currentPoint.x][currentPoint.y] = true;

		// push all empty neighbors and update final if we found
		// update finalTerritoryColor
		for (int i = 0; i < 4; i++)
		{
			Point adj = currentPoint + ADJ_OFFSETS[i];
			if (!isOnBoard(adj)) continue;
			if (visited[adj.x][adj.y]) // if it is visited (already pushed before in this function call) or out of boarder, don't process it
				continue;
			if (state(xy(adj)) == CellState::EMPTY)
				return false;

			// ooh, we find two different colors in the same territory color
			else if (state(xy(adj)) == color)
			{
				s.push(adj);
			}
		}
	}
	return true;

}

GoEngine::GoEngine()
{
}
/**
* This Method takes an input position x,y and returns which player has this position and 0 if no one
*
* **/
void GoEngine::checkTerritory(int x, int y, const State &state, TerritoryMat& territories)
{
	if (territories[x][y].first)
		return;
	if (state(x, y) != EMPTY) // if this state is colored, we don't consider it a territory like the point with two different colors adj's.
	{
		territories[x][y].first = true;
		territories[x][y].second = EMPTY;
		return;
	}

	// this passed point is new point and we never see it before

	CellState finalTerritoryColor = EMPTY; // we always start from empty territory, we check handled if it colored above
	std::stack<Point> s;
	std::stack<Point> newVisited;
	// bool say visited or not, if visited cellState define its status (BLACK=> black territory,WHITE=>white territory, EMPTY=> not for any one territory)
	s.push(Point(x, y));
	//std::cout << "stack is good\n";
	// territories[x][y] = Territory(true, EMPTY);

	bool emptyTerritory = false;
	while (!s.empty())
	{
		// std::cout << "begin while loop \n";
		Point currentPoint = s.top();
		s.pop();
		bool isVisited = territories[currentPoint.x][currentPoint.y].first;
		if (isVisited) // visited from another function call
			continue;
		// first time we see this point
		newVisited.push(currentPoint);
		// std::cout << "newVisited is good\n";
		territories[currentPoint.x][currentPoint.y].first = true;
		// std::cout << "writing in territories done in first\n";

		// push all empty neighbors and update final if we found
		// update finalTerritoryColor
		for (int i = 0; i < 4; i++)
		{
			Point adj = currentPoint + ADJ_OFFSETS[i];
			if (!isOnBoard(adj)) continue;
			if (territories[adj.x][adj.y].first && state(adj.x, adj.y) == EMPTY) // if it is visited (already pushed before in this function call) or out of boarder, don't process it
				continue;
			// std::cout << "adj " << adj.x << adj.y << "\n";
			// if empty, just push it
			if (state(xy(adj)) == CellState::EMPTY)
				s.push(adj);


			// ooh, we find two different colors in the same territory color
			else if (finalTerritoryColor != EMPTY && state(xy(adj)) != finalTerritoryColor) {
				finalTerritoryColor = EMPTY;
				emptyTerritory = true; // for sure
			}

			// the same color or from empty->that color
			else if (!emptyTerritory)
				finalTerritoryColor = state(xy(adj));
		}
	}
	mergeNewVisited(territories, newVisited, finalTerritoryColor);
	// return finalTerritoryColor;
}


bool GoEngine::isValidMove_stateUpdated(State, State const&, Action) {
	printf("not yet implemented");
	return false;
}

bool GoEngine::isValidMove(State state, const State& prevState, Action move) {
	// occupied point
	if (move.isPass()) return true;
	if (!isOnBoard(move.p)) return false;
	if (state(pxy(move)) != int(CellState::EMPTY)) return false;
	
	state = state + move;
	//std::cout << "state after placing move " << state(12, 0) << '\n';
	int numCaptured = removeCaptured(state, move.p, move.getColour());
	if (numCaptured > 0) {
		std::cout << "numCaptured: " << numCaptured << "\n";
		return !isKo(state, prevState);
	}
	return !isSelfCapture(state, move.p, move.getColour()); // the state passed here is the state after move played
}

bool GoEngine::isValidMove(const State* state, const State* prevState, Action move) {
	// occupied point
	if (move.isPass()) return true;
	if (!isOnBoard(move.p)) return false;
	if ((*state)(pxy(move)) != int(CellState::EMPTY)) return false;
	State newState(*state);
	newState += move;
	//std::cout << "state after placing move " << (*state)(12, 0) << '\n';
	int numCaptured = removeCaptured(newState, move.p, move.getColour());
	if (numCaptured > 0) {
		//std::cout << "numCaptured: " << numCaptured << "\n";
		if (prevState == NULL) return true;
		return !isKo(newState, *prevState);
	}
	return !isSelfCapture(newState, move.p, move.getColour()); // the state passed here is the state after move played
}

bool GoEngine::isKo(const State& currentState, const State& prevState) {
	for (int i = 0; i<BOARD_DIMENSION; i++) {
		for (int j = 0; j<BOARD_DIMENSION; j++) {
			if (currentState[i][j] != prevState[i][j])
				return false;
		}
	}
	return true;
}

bool GoEngine::isGoal(const State &currentState, Action currentMove, Action prevMove)
{
	if (prevMove.isPass() && currentMove.isPass())
		return true;
	for (int i = 0; i < BOARD_DIMENSION; i++)
	{
		for (int j = 0; j < BOARD_DIMENSION; j++)
		{
			if (currentState[i][j] == EMPTY)
				return false;
		}
	}
	return true;
}

std::vector<Action> GoEngine::getValidMoves(const State *_state, const State* _prevState, CellState nextColourToPlay) {
	std::vector<Action> validMoves;
	for (int i = 0; i<BOARD_DIMENSION; i++) {
		for (int j = 0; j<BOARD_DIMENSION; j++) {
			Action currentMove(nextColourToPlay, i, j);
			if (isValidMove(_state, _prevState, currentMove))
				validMoves.push_back(currentMove);
		}
	}
	validMoves.push_back(Action()); // add the pass move
	return validMoves;
}

void GoEngine::buildAllTerritories(const State& state, TerritoryMat& territories) {
	for (int i = 0; i< BOARD_DIMENSION; i++) {
		for (int j = 0; j< BOARD_DIMENSION; j++) {
			checkTerritory(i, j, state, territories);
		}
	}
	// TODO: remove it
	// for(int i = 0; i< BOARD_DIMENSION; i++){
	//   for(int j = 0; j< BOARD_DIMENSION; j++){
	//     if(territories[i][j].second == EMPTY && state(i,j) == EMPTY)
	//       std::cout << "EMPTY " << i << ", " << j << '\n';
	//   }
	// }
}
/**
*
*/
int GoEngine::removeCapturedHelper(State &state, Point point, CellState color)
{
	CellState toBePushed = getOpponentColor(color);

	std::stack<Point> s;
	Matrix visited(BOARD_DIMENSION, std::vector<bool>(BOARD_DIMENSION, false));
	std::stack<Point> pointsToRemove; // points that will be removed if all is captured
									  // bool say visited or not, if visited cellState define its status (BLACK=> black territory,WHITE=>white territory, EMPTY=> not for any one territory)
	s.push(point);
	// territories[x][y] = Territory(true, EMPTY);
	while (!s.empty())
	{
		Point currentPoint = s.top();
		s.pop();
		bool isVisited = visited[currentPoint.x][currentPoint.y];

		if (isVisited) // visited from another function call
			continue;
		// first time we see this point
		visited[currentPoint.x][currentPoint.y] = true;
		pointsToRemove.push(currentPoint);
		// push all empty neighbors and update final if we found
		// update finalTerritoryColor
		for (int i = 0; i < 4; i++)
		{
			Point adj = currentPoint + ADJ_OFFSETS[i];
			//if (adj.x == 12 && adj.y == 0) std::cout << "this is loai " << currentPoint.x << ", " << currentPoint.y << "\n";
			if (!isOnBoard(adj)) continue;
			//std::cout << "adj: " << adj.x << ", " << adj.y << "\n";
			if (visited[adj.x][adj.y]) // if it is visited (already pushed before in this function call) or out of boarder, don't process it
				continue;

			// if empty, just push it
			if (state(xy(adj)) == CellState::EMPTY)
				return 0;
			// ooh, we find two different colors in the same territory color
			else if (state(xy(adj)) == toBePushed)
				s.push(adj);
		}
	}

	// if i made it here this means that the point i started with belongs to a captured group
	// I will update both players score and board
	int numCaptured = pointsToRemove.size();
	while (!pointsToRemove.empty())
	{
		Point currentPoint = pointsToRemove.top();
		pointsToRemove.pop();
		//std::cout << "this point is captured: " << currentPoint.x << ", " << currentPoint.y << "\n";
		state(currentPoint.x, currentPoint.y) = EMPTY;
		state.last_captured_positions.push_back(currentPoint);
	}
	return numCaptured;
}


int GoEngine::removeCaptured(State &state, Point point, CellState color)
{
	CellState opColor = getOpponentColor(color);

	int totalCaptured = 0;
	for (int i = 0; i<4; i++) {
		Point adj = point + ADJ_OFFSETS[i];
		if (!isOnBoard(adj)) continue;
		if (state(xy(adj)) == opColor) {
			totalCaptured += removeCapturedHelper(state, adj, color);
		}
	}
	return totalCaptured;
}

// take action
// bool GoEngine::processMove(State& currentState, const State& prevState, Move currentMove){
//   if(currentState(currentMove.p.x, currentMove.p.y) != EMPTY) return false;
//   State nextState = currentState + currentMove;
//   int numCaptured = removeCaptured(nextState, currentMove.p, currentMove.getColour());
//   if(!isValidMove_stateUpdated(nextState, prevState, currentMove)) return false;
//   // checkTerritory(move.p.x, move.p.y, currentState);
// }

Score GoEngine::computeScore(const State& state) {
	int blackStones = 0;
	int whiteStones = 0;
	int blackTerr = 0;
	int whiteTerr = 0;

	TerritoryMat territories(BOARD_DIMENSION, TerritoryRow(BOARD_DIMENSION, Territory(false, EMPTY))); //creates 19 * 19 vector and gives it an initial value
																									   // std::cout << "territories initialized\n";
	buildAllTerritories(state, territories);
	/* for(int i = 0; i< BOARD_DIMENSION; i++){
	for(int j = 0; j< BOARD_DIMENSION; j++){
	if (territories[i][j].second == EMPTY)
	checkTerritory(i, j, state, territories);
	}
	}*/
	// std::cout << "territories builded in compute score\n";
	for (int i = 0; i<BOARD_DIMENSION; i++) {
		for (int j = 0; j<BOARD_DIMENSION; j++) {
			if (state(i, j) == BLACK) {
				blackStones++;
			}
			else if (state(i, j) == WHITE) {
				whiteStones++;
			}
			else {
				if (territories[i][j].second == BLACK) { blackTerr++; }
				else if (territories[i][j].second == WHITE) { whiteTerr++; }
			}
		}
	}
	//std::cout << "territories[12][0]:" << territories[12][0].first << std::endl;
	auto capturedStones = state.getCapturedstones();
	return Score(whiteStones + whiteTerr + capturedStones.black, blackStones + blackTerr + capturedStones.white); // TODO: add captured
}


void GoEngine::mergeNewVisited(TerritoryMat& territories, std::stack<Point> &newVisited, CellState territoryColor) {
	while (!newVisited.empty()) {
		auto p = newVisited.top(); newVisited.pop();
		territories[p.x][p.y].second = territoryColor;
	}
}


bool GoEngine::getRandomAction(Action& result, const State* state, const State* prevState, CellState playerColor) {
	auto moves = getValidMoves(state, prevState, playerColor);
	if (moves.empty()) return false;
	srand(time(NULL));
	// rand
	/*
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distribution(0, moves.size()); // distribution in range [1, 6]
	*/
	int sample = rand();
	sample%=moves.size();
    //cout << moves[sample];
	result = moves[sample];
	return true;
}

void GoEngine::applyValidAction(State& state, Action action){
	state.last_captured_positions.clear();
	state += action; // place the stone on the board
	state.set_color(Switch(state.get_color())); // toggle the color of the player who will play
	removeCaptured(state, action.p, action.getColour());
}

bool GoEngine::applyAction(State& state, const State * prevState, Action action){
	if (!isValidMove(&state, prevState, action)) return false;
	applyValidAction(state, action);
	return true;
}

GoEngine::~GoEngine()
{
}


// we will need class tracker as a wrapper for that class
// we may add function isDead() for that plays which is legal but can be ignored played as it is not worthy
