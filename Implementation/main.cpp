#include "State.h"
#include "GoEngine.h"
#include "MCTS.h"
#include "definitions.h"

#define ACK 25
// Declarations:
State state,prev_state;
CellState color[] = {WHITE, BLACK};
int positive, best_x, best_y, current_color;
GoEngine engine;
//------------------------------------------
extern "C"
// This function will take 2 arrays to fill the board with initial value.
// NOTE: the positive values in the array mean that it's AI_AGENT move.
void fill_initial(int* X,int* Y){
    state.clear();
    prev_state.set_color(CellState::EMPTY);
    prev_state.clear();
    int idx = 0;
    while(X[idx] != ACK)
        state(abs(X[idx])-1,abs(Y[idx])-1) = X[idx] > 0? color[positive] : color[!positive];
}

extern "C"
void set_color(int black){ // if the AI_AGENT is black then the variable = 1.
    positive = black;
}

extern "C"
// the following function apply moves which came from the log.
// after finishing this function, we have now the full initial state.
void reach_initial(int* X,int* Y,int cur){

    int idx = 0, x, y;
    current_color = cur;
    State old_state;
    while(X[idx] != ACK){
        Action action(color[current_color], {X[idx], Y[idx]} );
        old_state = state;
        engine.applyAction(state, prev_state.get_color() == CellState::EMPTY ? NULL:&prev_state, action);
        prev_state = old_state;
        current_color = !current_color;
    }
    idx = 0;
    for(int i = 0;i<BOARD_DIMENSION;++i)
        for(int j = 0;j<BOARD_DIMENSION;++j)
            if(state(i,j)==color[positive]) X[idx] = i+1,Y[idx++] = j+1;
            else if(state(i,j)==color[!positive]) X[idx] = -(i+1),Y[idx++] = -(j+1);
}

extern "C"
void* Carloh(void* ptr){
    State state = *( (State *) ptr);
    MCTS MC;
    puts("running MCTS..");
	Action act = MC.run(state,1);
    puts("MCTS done. Best action found.");
    best_x = act.p.x;
    best_y = act.p.y;
}

extern "C"
void get_best_move(int& x,int& y){
    while (best_x == ACK || best_y == ACK);
    x = best_x;
    y = best_y;
}

extern "C"
void  getRemovedPositions(int * listState,int* move,int * removedStonesX,int * removedStonesY,int color){//move[0] row number move[1] column number 
    State currentState;
    for(int i=0;i<BOARD_DIMENSION;i++){
        for(int j=0;j<BOARD_DIMENSION;j++){
            if(listState[i+j*BOARD_DIMENSION]==1)
                currentState[i][j]=BLACK;
            else if(listState[BOARD_DIMENSION*BOARD_DIMENSION+ i+j*BOARD_DIMENSION]==1)
                currentState[i][j]=WHITE;    
        }
    }
    CellState currentMoveColour;

    if(color==0){
        currentMoveColour=BLACK;
    }
    else{
        currentMoveColour=WHITE;
    }
    Action action(currentMoveColour,move[0],move[1]);
    engine.applyValidAction(currentState,action);
    
    std::vector<Point> lastMovesCaptured=currentState.last_captured_positions;
    
    

    for(int i=0;i<lastMovesCaptured.size();i++){
        removedStonesX[lastMovesCaptured[i].x]=1;
        removedStonesY[lastMovesCaptured[i].y]=1;
    }

}


extern "C"
void make_move(int* X,int* Y,int remaining_time){ //NOTE: the remaining time will be in seconds.
    // NOTE: the first element of the passed array is the move which the opponent did.
    best_x = best_y = ACK;
    // variables.
    State old_state;
    Action action(color[!positive], {X[0], Y[0]} );
    //pthread_t thread;
    // -----------------

    old_state = state;
    engine.applyAction(state, prev_state.get_color() == CellState::EMPTY ? NULL:&prev_state, action);
    prev_state = old_state;

    State* ptr = &state;
    //int thread_ID = pthread_create(&thread, NULL, Carloh, ptr);

    // The part below, should be replaced with the captured positions.
    int random = 10;
    for(int i = 0;i<random;++i)
        X[i] = ((random*50+10)*3)%19,Y[i] = ((random*30+7)*5)%19,
    X[random] = ACK;
}


