#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "GoEngine.h"
#include "definitions.h"
#include "test_helpers.h"
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(gameseq_test_suite)

BOOST_AUTO_TEST_CASE(testing_fullgame_seq, *utf::tolerance(0.00001)) {
    GoEngine engine;
    State state;

    string games_path = "../games/";
    vector<string> games_files = tst::get_files(games_path);
    for (auto game_file : games_files) {
        tst::Game game = tst::parse_game(games_path + game_file);

        BOOST_REQUIRE(game.board_size == BOARD_DIMENSION);

        for (int i = 0; i < game.board_size; i++) {
            for (int j = 0; j < game.board_size; j++) {
                state(i, j) = EMPTY;
            }
        }

        for (int i = 0; i < game.plays.size(); i++) {
            engine.applyValidAction(state, Action(game.plays[i].player, game.plays[i].row, game.plays[i].col));

            bool match = state == game.states[i];

            BOOST_TEST(match, "Error:\n");
            if (!match) {
                cout << "board file" << game_file << " board_num:" << i + 1;

                cout
                    << "Required State:\n"
                    << game.states[i] << endl;
                cout << "Givem State:\n"
                     << state << endl;

                return;
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
