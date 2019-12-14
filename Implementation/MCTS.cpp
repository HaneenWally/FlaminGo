#include "definitions.h"
#include "MCTS.h"
#include "assert.h"

MCTS::MCTS()
{
	iterations = 0;
	UCB1_C = sqrt(2);
	max_iterations = 100;
	max_millis = 1 * 1000;  // MUST BE CHANGED.
	simulation_depth = 100;
}
GoEngine MCTS::engine = GoEngine();
int MCTS::get_iterations() const
{
	return iterations;
}

float MCTS::Policy(Node* node, Node* child)
{
	// (wk / nk) + C * sqrt(ln(n par) / nk)
	float ucb_exploitation = (float)child->get_wins() / child->get_num_visits();
	float ucb_exploration = sqrt(log(node->get_num_visits()) / child->get_num_visits());
	float ucb_score = ucb_exploitation + UCB1_C * ucb_exploration;
	return ucb_score;
}

// get best child for given node based on UCB score
Node* MCTS::get_best_child(Node* node, float ucb_c)
{
	// The node should be fully expanded (generated all its child) to choose among them
	if (!node->is_fully_expanded())
	{
		return NULL;
	}

	float best_ucb_score = -1;
	Node* best_node = NULL;

	int num_children = node->get_num_children();
	for (int i = 0; i < num_children; ++i)
	{
		Node* child = node->get_child(i);
		float ucb_score = Policy(node, child);

		if (ucb_score > best_ucb_score)
		{
			best_ucb_score = ucb_score;
			best_node = child;
		}
	}

	return best_node;
}

Node* MCTS::get_most_visited_child(Node* node)
{
	int most_visits = -1;
	Node* best_node = NULL;

	int num_childs = node->get_num_children();
	for (int i = 0; i<num_childs; ++i)
	{
		Node* child = node->get_child(i);
		if (child->get_num_visits() > most_visits)
		{
			most_visits = child->get_num_visits();
			best_node = child;
		}
	}

	return best_node;
}

// Descend, return node with best score
Node* MCTS::Select(Node* node)
{
	while (!node->is_terminal() && node->is_fully_expanded())
	{
		node = get_best_child(node, UCB1_C);
	}
	return node;
}

// Expand, Expand by adding single child (if not terminal or not fully expanded)
Node* MCTS::Expand(Node* node, CellState AI_COLOR)
{
	assert(node != NULL);
	if (!node->is_fully_expanded() && !node->is_terminal())
	{
		node = node->expand(AI_COLOR);
	}
	return node;
}

//Simulate, Apply random actions till the game ends(win or lose)
Result MCTS::Simulate(State state,State prev_state, Action action, Action prev_action, CellState AI_COLOR)
{

	if (!engine.isGoal(state, action, prev_action))
	{
		for (int d = 0; d < simulation_depth; ++d)
		{
			if (engine.isGoal(state, action, prev_action))
			{
				break;
			}
			prev_action = action;
			if (engine.getRandomAction(action, &state, &prev_state,Switch(state.get_color()))) // TODO: interface correct and send missing params [DONE]
			{
				this->engine.applyValidAction(state, action);
			}
			else
			{
				break;
			}
		}
	}
	/*
	//This Part is valid in case no actions = terminal state.
	int counter = simulation_depth;
	while(!state.is_terminal() && counter-- )
	{
	Action action;
	state.get_random_action(action)
	state.apply_action(action);
	}
	*/
	Score score = engine.computeScore(state);
	return ( is_winner(AI_COLOR, score) ? WIN:LOSE);
	// return state.evalute();     // WIN or LOSE.
}

//Back Propagation, Update the path of hte node
void MCTS::Propagate(Node* node, Result reward, CellState AI_COLOR)
{
	State tmp = node->get_state();
	if(node && tmp.get_color() == AI_COLOR){
		reward = (reward == WIN ? LOSE : WIN); // Toggle the state.
	}
	while (node)
	{
		reward = (reward == WIN ? LOSE : WIN); // Toggle the state.
		node->update(reward);
		node = node->get_parent();
	}
}

Action MCTS::run(State& current_state, int seed, int time_limit, CellState AI_COLOR)
{
	this->max_millis = time_limit;
	timer.init();

	Node root_node(current_state, NULL);

	Node* best_node = NULL;
	iterations = 0;

	while (true)
	{
		timer.loop_start();

		// 1. SELECT
		Node* node = Select(&root_node);
        //puts("Node selected successfully.");
		// 2. Expand
		node = Expand(node, AI_COLOR);
		//puts("Node expanded successfully.");

		State state(node->get_state());

		// 3. Simulate   // NOTE: the parent node will never = NULL, as the concept of expanding prevent that from happening.
		Result reward = Simulate(state,node->get_parent()==NULL? state:node->get_parent()->get_state(), node->get_action(), node->get_parent()->get_action(), AI_COLOR);
        //puts("Got the reward successfully.");
		//if(explored_states) explored_states->push_back(state);

		// 4. BACK PROPAGATION
		Propagate(node, reward, AI_COLOR);
		//puts("Propagation is done successfully.");


		best_node = get_most_visited_child(&root_node);

		timer.loop_end();
		if (max_millis > 0 && timer.check_duration(max_millis)) break;

		// exit loop if current iterations exceeds max_iterations
		if (max_iterations > 0 && iterations > max_iterations) break;
		iterations++;
		//cout << "simulation number " << iterations << " done.\n";
	}

	// Return the action to the best node
	if (best_node)
	{
		return best_node->get_action();
	}

	// You shouldn't be here.
	assert(!"No best action found");
	return Action();
}

MCTS::~MCTS()
{

}
