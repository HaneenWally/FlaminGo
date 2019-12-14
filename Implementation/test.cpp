#include "definitions.h"
#include "GoEngine.h"
#include <time.h>
#include "MCTS.h"
#include <windows.h>

using namespace std;
void test1();
void test2();
void test3();

int main()
{
	test3();
	return 0;
}
void Print(State& state, Point p)
{
    cout << "The current Board:\n";
	char arr[] = {'B','.','W'};
	for(int i=0;i<BOARD_DIMENSION;++i)
		for(int j = 0;j<BOARD_DIMENSION;++j){
            if(i == p.x && j == p.y){
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, 10);
                cout << arr[state(i,j)+1] << " \n"[j==BOARD_DIMENSION-1];
                SetConsoleTextAttribute(hConsole, 15);
            }
            else cout << arr[state(i,j)+1] << " \n"[j==BOARD_DIMENSION-1];
		}

}
void test3()
{
    freopen("log.txt","w",stdout);
    int tests = 10;
    while(tests--){
        State state,prv_state,old_state;
        prv_state.set_color(EMPTY);
        CellState arr[] = {WHITE,BLACK,EMPTY};
        string out[] = {"WHITE","BLACK"};
        srand(time(NULL));
        int wh = 0,bl = 0;
        for(int i=0;i<BOARD_DIMENSION;++i)
            for(int j = 0;j<BOARD_DIMENSION;++j)
                state(i,j) = arr[ rand()%3 ], wh+= state(i,j) == WHITE, bl += state(i,j) == BLACK;

        for(int i=0;i<BOARD_DIMENSION && abs(wh-bl) > 2;++i)
            for(int j = 0;j<BOARD_DIMENSION && abs(wh-bl) > 2;++j)
                if(bl > wh && state(i,j) == BLACK) state(i,j) = arr[2],bl--;
                else if(wh > bl && state(i,j) == WHITE) state(i,j) = arr[2],wh--;

        //cout << state << endl;
        int idx = rand()%2;
        CellState OPPO = arr[idx];
        CellState AI = arr[!idx];
        state.set_color(OPPO);
        //puts("Applying MCTS..");
        MCTS carloh;
        GoEngine engine;
        Action cur, prv(OPPO,Point(0, 0));
        int turn = 0;
        long long counter = 0;
        while (!engine.isGoal(state, cur, prv) && ++counter <= 50)
        {
            old_state = state;
            prv = cur;
            if (!turn) {
                cur = carloh.run(state, 1, 5 * 1000, AI);
                engine.applyAction(state, prv_state.get_color() == EMPTY? NULL: &prv_state, cur);
                //Print(state, cur.getMove());
            }
            else {
                engine.getRandomAction(cur, &state, &prv_state, OPPO);
                engine.applyAction(state, &prv_state, cur);
            }
            // cout << cur << endl;
            //cout << state << endl;
            prv_state = old_state;

            turn = !turn;
        }
        puts("----");

        cout << "AI COLOR: " << out[!idx] << endl;
        Score sc = engine.computeScore(state);
        if(is_winner(arr[!idx], sc)) puts("WIN");
        else puts("LOSE");

        //if (sc.white > sc.black) cout << "White wins\n";
        //else cout << "Black Wins\n";
        cout << "White score: " << sc.white << " Black score: " << sc.black << endl;
    }
}

void test1() {
	GoEngine engine;

	State state;
	// cout << "...testing compute score..." << endl;
	Score s(0, 0);// = engine.computeScore(state);
				  // cout << "black score = " << s.black << ", white score = " << s.white << endl;

	CellState cells[13][13]{
		EMPTY,EMPTY,EMPTY,EMPTY,BLACK,BLACK,BLACK,BLACK,WHITE,WHITE,EMPTY,EMPTY,EMPTY,
		EMPTY,EMPTY,EMPTY,EMPTY,WHITE,BLACK,BLACK,BLACK,WHITE,WHITE,EMPTY,EMPTY,EMPTY,
		EMPTY,EMPTY,EMPTY,EMPTY,WHITE,WHITE,BLACK,BLACK,BLACK,WHITE,WHITE,WHITE,WHITE,
		BLACK,WHITE,WHITE,EMPTY,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,WHITE,BLACK,
		BLACK,BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,
		BLACK,BLACK,BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,EMPTY,EMPTY,
		EMPTY,EMPTY,EMPTY,BLACK,BLACK,WHITE,WHITE,BLACK,WHITE,BLACK,EMPTY,EMPTY,EMPTY,
		EMPTY,EMPTY,EMPTY,EMPTY,BLACK,EMPTY,WHITE,BLACK,EMPTY,BLACK,EMPTY,EMPTY,EMPTY,
		EMPTY,EMPTY,EMPTY,EMPTY,BLACK,BLACK,BLACK,BLACK,BLACK,WHITE,BLACK,EMPTY,EMPTY,
		BLACK,BLACK,BLACK,BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,EMPTY,EMPTY,
		BLACK,BLACK,WHITE,WHITE,EMPTY,EMPTY,EMPTY,EMPTY,WHITE,WHITE,BLACK,EMPTY,EMPTY,
		BLACK,WHITE,WHITE,WHITE,EMPTY,EMPTY,EMPTY,EMPTY,WHITE,BLACK,BLACK,EMPTY,EMPTY,
		EMPTY,WHITE,WHITE,WHITE,EMPTY,EMPTY,EMPTY,EMPTY,WHITE,WHITE,BLACK,EMPTY,EMPTY
	}; // 82 black, 77 white
	for (int i = 0; i<13; i++) {
		for (int j = 0; j<13; j++) {
			state(i, j) = cells[i][j];
		}
	}
	//     for (int i = 0; i<13; i++){
	//     for (int j=0;j<13;j++){
	//         cout << state(i,j) << " ";
	//     }
	//     cout << endl;
	// }

	s = engine.computeScore(state);
	cout << "black score = " << s.black << ", white score = " << s.white << endl;
}


void test2() {
	GoEngine engine;

	State state;
	// cout << "...testing compute score..." << endl;
	Score s(0, 0);// = engine.computeScore(state);
				  // cout << "black score = " << s.black << ", white score = " << s.white << endl;

	CellState cells[13][13]{
		//    0     1     2     3    4      5     6     7     8    9     1 0   11    12
		EMPTY,EMPTY,EMPTY,EMPTY,BLACK,BLACK,BLACK,BLACK,WHITE,WHITE,EMPTY,EMPTY,EMPTY,// 0
		EMPTY,EMPTY,EMPTY,EMPTY,WHITE,BLACK,BLACK,BLACK,WHITE,WHITE,EMPTY,EMPTY,EMPTY,//1
		EMPTY,EMPTY,EMPTY,EMPTY,WHITE,WHITE,BLACK,BLACK,BLACK,WHITE,WHITE,WHITE,WHITE,//2
		BLACK,WHITE,WHITE,EMPTY,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,WHITE,BLACK,//3
		BLACK,BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,//4
		BLACK,BLACK,BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,EMPTY,EMPTY,//5
		EMPTY,EMPTY,EMPTY,BLACK,BLACK,WHITE,WHITE,BLACK,WHITE,BLACK,EMPTY,EMPTY,EMPTY,//6
		EMPTY,EMPTY,EMPTY,EMPTY,BLACK,EMPTY,WHITE,BLACK,EMPTY,BLACK,EMPTY,EMPTY,EMPTY,//7
		EMPTY,EMPTY,EMPTY,EMPTY,BLACK,BLACK,BLACK,BLACK,BLACK,WHITE,BLACK,EMPTY,EMPTY,//8
		BLACK,BLACK,BLACK,BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,EMPTY,EMPTY,//9
		BLACK,BLACK,WHITE, BLACK ,EMPTY,EMPTY,EMPTY,EMPTY,WHITE,WHITE,BLACK,EMPTY,EMPTY,//10
		BLACK,WHITE,WHITE, BLACK ,EMPTY,EMPTY,EMPTY,EMPTY,WHITE,BLACK,BLACK,EMPTY,EMPTY,//11
		EMPTY,WHITE,WHITE, BLACK ,EMPTY,EMPTY,EMPTY,EMPTY,WHITE,WHITE,BLACK,EMPTY,EMPTY //12
	}; // 82 black, 77 white
	for (int i = 0; i<13; i++) {
		for (int j = 0; j<13; j++) {
			state(i, j) = cells[i][j];
		}
	}
	//     for (int i = 0; i<13; i++){
	//     for (int j=0;j<13;j++){
	//         cout << state(i,j) << " ";
	//     }
	//     cout << endl;
	// }
	State prev = state;
	prev(12, 0) = BLACK;
	prev(12, 1) = EMPTY;
	prev(12, 2) = EMPTY;
	prev(11, 1) = EMPTY;
	// prev(11,2) = EMPTY;
	// prev(10,2) = EMPTY;
	cout << engine.isValidMove(state, prev, Action(BLACK, 12, 0)) << endl;
}
