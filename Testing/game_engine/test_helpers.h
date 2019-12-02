#if !defined(TEST_HELPERS)
#define TEST_HELPERS
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "State.h"

using namespace std;

namespace tst {

struct Play {
    CellState player;
    int row, col;
};

struct Game {
    int board_size;
    vector<Play> plays;
    vector<State> states;
};

struct ScoredBoard {
    int board_size;
    float white_score;
    int black_score;
    int white_captured = 0;
    int black_captured = 0;
    Board board;
};

void readBoard(ifstream &in, Board &board) {
    char cell = ' ';
    for (auto &&v : board) {
        for (auto &&c : v) {
            in >> cell;
            if (cell == '.')
                c = EMPTY;
            else if (cell == 'b')
                c = BLACK;
            else
                c = WHITE;
        }
    }
}

ScoredBoard parse_board(string board_file = "./boards/1377954818019999562.txt") {
    ifstream in;
    in.open(board_file, ios::in);
    ScoredBoard scoredBoard;
    in >> scoredBoard.board_size;
    scoredBoard.board = vector<vector<CellState>>(scoredBoard.board_size, vector<CellState>(scoredBoard.board_size, EMPTY));

    readBoard(in, scoredBoard.board);

    string whichone = "white";
    in >> whichone;
    if (whichone == "white") {
        in >> scoredBoard.white_score;
        in >> whichone;
        in >> scoredBoard.black_score;
    } else {
        in >> scoredBoard.black_score;
        in >> whichone;
        in >> scoredBoard.white_score;
    }

    in.close();

    return scoredBoard;
}

Game parse_game(string game_file = "./games/1377954818019999562.txt") {
    ifstream in;
    in.open(game_file, ios::in);
    Game game = Game();
    in >> game.board_size;

    Play play;
    char player;
    while (in >> player) {
        play.player = (player == 'w') ? WHITE : BLACK;
        in >> play.row >> play.col;

        Board board = vector<vector<CellState>>(game.board_size, vector<CellState>(game.board_size, EMPTY));
        readBoard(in, board);

        game.plays.push_back(play);
        game.states.push_back(State(board));
    }

    in.close();
    return game;
}

vector<string> get_files(string path) {
    vector<string> files;

    struct dirent *entry = nullptr;
    DIR *dp = nullptr;
    dp = opendir(path.c_str());
    if (dp != nullptr) {
        while ((entry = readdir(dp))) {
            if (strlen(entry->d_name) > 3)
                files.push_back(entry->d_name);
        }
    }
    closedir(dp);

    return files;
}

}  // namespace tst

bool operator==(const State &s1, const State &s2) {
    if (s1.size() != s2.size() || s1.size() == 0 || s1[0].size() != s2[0].size()) return false;
    for (int i = 0; i < s1.size(); ++i) {
        for (int j = 0; j < s2.size(); ++j) {
            if (s1(i, j) != s2(i, j))
                return false;
        }
    }
    return true;
}

#endif  // MACRO
