#pragma once

#include "definitions.h"
#include "Node.h"
#include "LoopTimer.h"
#include "State.h"
#include "Action.h"
#include <vector>

#include "GoEngine.h"

// Monte Carlo Tree Search class
class MCTS
{
private:
	LoopTimer timer;
	int iterations;
	float Policy(Node*, Node*);  //the policy of calculating the score of node.
	static GoEngine engine;

	// helpers
	bool get_random_action(Action&);        // return false if no action found.
public:

	float UCB1_C;           // Value of C in UCB1 function: sqrt(2), 2, 1.5
	int max_iterations;     // max # of iteration to run MCTS
	int max_millis;         // max time to run MCTS
	int simulation_depth;   // max depth to run simulation

	MCTS();
	const LoopTimer& get_timer() const;
	int get_iterations() const;
	Node* get_best_child(Node*, float);
	Node* get_most_visited_child(Node*);
	Node* Select(Node*);
	Node* Expand(Node*);
	Result Simulate(State, State, Action, Action, CellState);
	void Propagate(Node*, Result);
	Action run(State&, int,int, CellState);

	~MCTS();
};
