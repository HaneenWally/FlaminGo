#include "State.h"
#include "GoEngine.h"
#include "MCTS.h"
#include "definitions.h"

#define ACK 25
// Declarations:
State state,prev_state;
Action current, prev_action(CellState::EMPTY, Point(0,0));
CellState color[] = {WHITE, BLACK};
int positive, best_x, best_y, current_color;
GoEngine engine;
//------------------------------------------
extern "C"
void PyInit_implementation(){
    printf("Hello Taha");
}

extern "C"
// This function will take 2 arrays to fill the board with initial value.
// NOTE: the positive values in the array mean that it's AI_AGENT move.
void fill_initial(int* X,int* Y, int white,int black){
    state.clear();
    prev_state.set_color(CellState::EMPTY);
    prev_state.clear();
    state.setCapturedStones(white, black);
    int idx = 0;
    while(X[idx] != ACK)
        state(abs(X[idx])-1,abs(Y[idx])-1) = X[idx] > 0? color[positive] : color[!positive], idx ++;
}

extern "C"
void set_color(int black){ // if the AI_AGENT is black then the variable = 1.
    positive = black;
}

extern "C"
// the following function apply moves which came from the log.
// after finishing this function, we have now the full initial state.
void reach_initial(int* X,int* Y,int cur){
    // cur = 1 if the first man to play is black.
    int idx = 0, x, y;
    current_color = cur;
    State old_state;
    while(X[idx] != ACK){
        Point p(X[idx],Y[idx]);
        Action action(color[current_color], p);
        current = action;
        old_state = state;
        engine.applyAction(state, prev_state.get_color() == CellState::EMPTY ? NULL:&prev_state, action);
        prev_state = old_state;
        current_color = !current_color;
        idx ++;
        prev_action = current;
    }
    idx = 0;
    for(int i = 0;i<BOARD_DIMENSION;++i)
        for(int j = 0;j<BOARD_DIMENSION;++j)
            if(state(i,j)==color[positive]) X[idx] = i+1,Y[idx++] = j+1;
            else if(state(i,j)==color[!positive]) X[idx] = -(i+1),Y[idx++] = -(j+1);
    X[idx] = Y[idx] = ACK;
}

void change_borad(int x,int y,int turn){
    State old_state;
    Point p(x,y);
    prev_action = current;
    Action action(color[turn], p);
    current = action;
    old_state = state;
    engine.applyAction(state, prev_state.get_color() == CellState::EMPTY ? NULL:&prev_state, action);
    prev_state = old_state;
    cout << state << endl;
    
}

extern "C"
void make_move(int* X,int* Y, int remaining_time){ 
    best_x = best_y = ACK;
    MCTS MC;
    // puts("running MCTS..");
	Action act = MC.run(state,1, remaining_time*1000, color[positive]); // WARNING: the time should be changed.
    best_x = act.p.x; best_y = act.p.y;
    X[0] = best_x; Y[0] = best_y;
    change_borad(best_x,best_y,positive);
    int idx = 1; // should start from 1.
    for(Point p:state.last_captured_positions)
        X[idx] = p.x, Y[idx] = p.y,idx++;
    X[idx] = Y[idx] = ACK;
}


bool valid(int x,int y){
    Point p(x,y);
    Action action(color[!positive], p);
    return engine.isValidMove(&state, prev_state.get_color() == CellState::EMPTY ? NULL:&prev_state, action);
}

extern "C"
// NOTE: the remaining time will be in seconds.
// NOTE: the first element of the passed array is the move which the opponent did.
int opponent_move(int* X,int* Y,int remaining_time){
    // the following line is useless IN CASE we r dealing with AI vs AI. but it's important in AI vs HUMAN.
    if(!valid(X[0],Y[0])) return 0;

    change_borad(X[0],Y[0],!positive);
    int idx = 0;
    for(Point p:state.last_captured_positions)
        X[idx] = p.x, Y[idx] = p.y,idx++;
    X[idx] = Y[idx] = ACK;
    return 1;
}

extern "C"
int AI_score(int black){
    Score sc = engine.computeScore(state);
    if(black) return sc.black;
    return sc.white;
}

extern "C"
int is_done(){
    return engine.isGoal(state,current,prev_action );
}

