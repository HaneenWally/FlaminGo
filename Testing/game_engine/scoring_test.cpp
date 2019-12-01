#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "GoEngine.h"
#include "definitions.h"
#include "test_helpers.h"
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(scoring_test_suite)

BOOST_AUTO_TEST_CASE(testing_with_empty_baord, *utf::tolerance(0.00001)) {
    GoEngine engine;
    State state;
    int board_size = 19;
    int black_captured = 0;
    int white_captured = 0;
    float required_white_score = 6.5;
    int required_black_score = 0;
    BOOST_REQUIRE(board_size == BOARD_DIMENSION);

    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            state(i, j) = EMPTY;
        }
    }

    Score s(white_captured, black_captured);
    s = engine.computeScore(state);

    BOOST_TEST(s.white == required_white_score, "white score should equal: " << required_white_score << ", given: " << s.white);
    BOOST_TEST(s.black == required_black_score, "black score should equal: " << required_black_score << ", given: " << s.black);
}

BOOST_AUTO_TEST_CASE(testing_with_one_stone_on_baord, *utf::tolerance(0.00001)) {
    GoEngine engine;
    State state;
    int board_size = 19;
    int black_captured = 0;
    int white_captured = 0;
    float required_white_score = 19 * 19 + 6.5;
    int required_black_score = 0;
    BOOST_REQUIRE(board_size == BOARD_DIMENSION);

    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            state(i, j) = EMPTY;
        }
    }
    state(board_size / 2, board_size / 2) = WHITE;

    Score s(white_captured, black_captured);
    s = engine.computeScore(state);

    BOOST_TEST(s.white == required_white_score, "white score should equal: " << required_white_score << ", given: " << s.white);
    BOOST_TEST(s.black == required_black_score, "black score should equal: " << required_black_score << ", given: " << s.black);
}

BOOST_AUTO_TEST_CASE(testing_adding_captured_stones, *utf::tolerance(0.00001)) {
    GoEngine engine;
    State state;
    int board_size = 19;
    int black_captured = 3;
    int white_captured = 9;
    float required_white_score = 6.5 + black_captured;
    int required_black_score = 0 + white_captured;
    BOOST_REQUIRE(board_size == BOARD_DIMENSION);

    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            state(i, j) = EMPTY;
        }
    }
    state(board_size / 2, board_size / 2) = WHITE;
    state(board_size / 2 + 1, board_size / 2 + 1) = BLACK;
    required_white_score++;
    required_black_score++;

    Score s(white_captured, black_captured);
    s = engine.computeScore(state);

    BOOST_TEST(s.white == required_white_score, "white score should equal: " << required_white_score << ", given: " << s.white);
    BOOST_TEST(s.black == required_black_score, "black score should equal: " << required_black_score << ", given: " << s.black);
}

BOOST_AUTO_TEST_CASE(testing_with_baords_in_folder_boards, *utf::tolerance(0.00001)) {
    string boards_path = "../boards/";
    vector<string> boards_files = tst::get_files(boards_path);

    for (auto board_file : boards_files) {
        GoEngine engine;
        int board_size = 19;
        int black_captured = 0;
        int white_captured = 0;
        float required_white_score = 6.5;
        int required_black_score = 0;

        tst::ScoredBoard scoredBoard = tst::parse_board(boards_path + board_file);

        BOOST_REQUIRE(board_size == BOARD_DIMENSION);

        required_white_score = scoredBoard.white_score;
        required_black_score = scoredBoard.black_score;
        black_captured = scoredBoard.black_captured;
        white_captured = scoredBoard.white_captured;

        Score s(white_captured, black_captured);

        State state(scoredBoard.board);
        s = engine.computeScore(state);

        BOOST_TEST(s.white == required_white_score, "white score should equal: " << required_white_score << ", given: " << s.white);
        BOOST_TEST(s.black == required_black_score, "black score should equal: " << required_black_score << ", given: " << s.black);
    }
}

BOOST_AUTO_TEST_SUITE_END()
