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

        State curr;
        // for (int i = 0; i < game.states.size(); i++) {
        //     engine.processMove()
        // }

        // for (auto &&play : game.plays) {
        //     cout << play.player << " " << play.row << " " << play.col << endl;
        // }

        // for (auto &state : game.states) {
        //     for (auto &r : state) {
        //         for (auto &c : r) {
        //             cout << c << ' ';
        //         }
        //     }
        // }
        break;
    }
}

BOOST_AUTO_TEST_SUITE_END()
