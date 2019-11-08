#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <stack>
#include <memory>
#include <math.h>
#include <algorithm>
#include <initializer_list>
#include "assert.h"



#define BOARD_DIMENSION 13
#define KOMI_VALUE 6.5

// core declarations
struct WhiteBlack {
	int white, black;
	WhiteBlack(int white, int black) : white(white), black(black)
	{}
	WhiteBlack() : white(0), black(0)
	{}
};

enum CellState {
	EMPTY = 0,
	WHITE = 1,
	BLACK = -1,
};

// This function switch the color depending on the current color.
#define Switch(current_color) (current_color == WHITE? BLACK:WHITE)

enum Result
{
	WIN = 1,
	LOSE = 0
};

enum Player
{
	AI_AGENT = 1,
	OPPONENT = 0
};


struct Score : public WhiteBlack
{
	Score(int whiteScore, int blackScore) : WhiteBlack(whiteScore + KOMI_VALUE, blackScore) {}

};


// helper abbreviations
typedef std::pair<int, int> pii;

// new types in terms of other types
typedef std::vector< std::vector<CellState> > Board;
typedef std::vector< std::vector<bool> > Matrix;
typedef WhiteBlack CapturedStones;
