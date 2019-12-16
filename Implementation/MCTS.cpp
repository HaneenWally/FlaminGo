#include "definitions.h"
#include "MCTS.h"
#include "assert.h"

MCTS::MCTS()
{
	iterations = 0;
	UCB1_C = sqrt(2);
	max_iterations = 10000;
	max_millis = 1 * 1000;  // MUST BE CHANGED.
	simulation_depth = 200;
	k = 100;
}
GoEngine MCTS::engine = GoEngine();
int MCTS::get_iterations() const
{
	return iterations;
}

float MCTS::Policy(Node* node, Node* child)
{
	Action a = child->get_action();
	
	int action_wins = rave[a].first;
	int action_simulations = rave[a].second;
	
	try
	{
		/* code */
	float b = sqrt(k / (k+action_simulations));
	float Qmc = (float)child->get_wins() / child->get_num_visits();
	float Qrave = (float) action_wins / action_simulations;
	float Q = (1 - b) * Qmc + b*Qrave;

	float exploitaion = (float)child->get_wins() / child->get_num_visits();
	float ucb_exploration = sqrt(log(node->get_num_visits()) / child->get_num_visits());

	float Qscore = Q + UCB1_C * ucb_exploration;
	return Qscore;
	}
	catch(...)
	{
		// cout << "We find the error";
		assert("we find the error");
	}
	
	// float Qscore = exploitaion + UCB1_C * ucb_exploration;
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

vector<Node*> MCTS::get_most_visited_child(Node* node)
{
	int most_visits1 = -1;
	int most_visits2 = -1;
	Node* best_node1 = NULL;
	Node* best_node2 = NULL;

	int num_childs = node->get_num_children();
	for (int i = 0; i<num_childs; ++i)
	{
		Node* child = node->get_child(i);
		if (child->get_num_visits() > most_visits1)
		{
			most_visits1 = child->get_num_visits();
			best_node2 = best_node1;
			best_node1 = child;
		}
		else if (child->get_num_visits() > most_visits2)
		{
			most_visits2 = child->get_num_visits();
			best_node2 = child;
		}
	}

	vector<Node*> best_moves;
	if(best_node1)
	{
		best_moves.push_back(best_node1);
	}
	if(best_node2)
	{
		best_moves.push_back(best_node2);
	}

	return best_moves;
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
	// puts("Here");
	srand(time(NULL));
	State old_state;
	vector<Point> availWhite = engine.getEmptyCells(state);
	vector<Point> availBlack = availWhite;
	vector<Point>* avail;
	// auto currentColor = AI_COLOR;
	if (!engine.isGoal(state, action, prev_action))
	{
		//engine.getRandomAction(action, &state, &prev_state,Switch(state.get_color()))
		for (int d = 0; d < simulation_depth; ++d)
		{
			// puts("IsGoal done 0");
			if (engine.isGoal(state, action, prev_action))
			{
				break;
			}

			if(Switch(state.get_color()) == WHITE){
				avail = &availWhite;
			}
			else{
				avail = &availBlack;
			}
			// puts("IsGoal done 1");
			
			if (! avail->empty())
			{
				//// puts("IsGoal done 2");
				int idx = rand() % avail->size();
			// puts("IsGoal done 2");

				action = Action(Switch(state.get_color()), (*avail)[idx]);
				// action.player = Switch(state.get_color());
				bool isEmpty = avail->empty();
			// puts("IsGoal done 3");

				while(!isEmpty && !engine.isValidMove(state, prev_state, action)){
					swap((*avail)[idx],(*avail)[avail->size()-1]);
					avail->pop_back();
					isEmpty = avail->empty();
					if (isEmpty) break;

					idx = rand() % avail->size();
					action = Action(Switch(state.get_color()), (*avail)[idx]);
				}
			// puts("IsGoal done 4");

				if (isEmpty){
					// cout << "What the hell\n";
					break;
				}

				swap((*avail)[idx],(*avail)[avail->size()-1]);
				avail->pop_back();
			// puts("IsGoal done 5");
				
				prev_action = action;
				old_state = state;
				// action.player = Switch(state.get_color());
				this->engine.applyValidAction(state, action);
				for(auto pos:state.last_captured_positions){
					availWhite.push_back(pos);
					availBlack.push_back(pos);
				}
				prev_state = old_state;
			// puts("IsGoal done 6");

			}
			else
			{
				// cout << "ezay ba2a ?!!\n";
				break;
				// avail = engine.getEmptyCells(state);
			}
			// puts("IsGoal done 3");
		}
	}
	/*
	// This Part is valid in case no actions = terminal state.
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
	// OPTIMIZATION asdsbadjasb
}

//Back Propagation, Update the path of hte node
void MCTS::Propagate(Node* node, Result reward, CellState AI_COLOR)
{
	int action_win = 0;
	if (reward == WIN){
		action_win = 1;
	}

	State tmp = node->get_state();

	if(tmp.get_color() == AI_COLOR)
        reward = (reward == WIN ? LOSE : WIN);


	while (node)
	{
		reward = (reward == WIN ? LOSE : WIN); // Toggle the state.
		node->update(reward);
		
		if(node->get_parent()){

			Action a = node->get_action();
			rave[a].first += action_win;
			rave[a].second += 1;

		}
		

		node = node->get_parent();
	}
}

Action MCTS::run(State& current_state, int seed, int time_limit, CellState AI_COLOR)
{
	this->max_millis = time_limit;
	timer.init();

	State root_state = current_state;
	Node root_node(current_state, NULL);

	vector<Node*> best_nodes;
	iterations = 0;
	while (true)
	{
		timer.loop_start();

		// 1. SELECT
		// puts("1");
		Node* node = Select(&root_node);
        // puts("Node selected successfully.");
		// 2. Expand
		// puts("2");
		node = Expand(node, AI_COLOR);
		// puts("Node expanded successfully.");

		// puts("hawdawd");
		State state = node->get_state();


		// puts("para1");
		State par = node->get_parent() == NULL ? state : node->get_parent()->get_state();
		// puts("para2");
		Action a = node->get_action();
		// puts("para3");
		Action pre_a = node->get_parent()->get_action();
		// puts("para4");
		// puts("3");
		// 3. Simulate   // NOTE: the parent node will never = NULL, as the concept of expanding prevent that from happening.
		Result reward = Simulate(state, par, a, pre_a, AI_COLOR);
		// puts("Got the reward successfully.");
		//if(explored_states) explored_states->push_back(state);

		// 4. BACK PROPAGATION
		// puts("4");
		Propagate(node, reward, AI_COLOR);
		// puts("Propagation is done successfully.");

		// puts("5");
		best_nodes = get_most_visited_child(&root_node);

		timer.loop_end();
		if (max_millis > 0 && timer.check_duration(max_millis)) break;

		// exit loop if current iterations exceeds max_iterations
		if (max_iterations > 0 && iterations > max_iterations) break;
		iterations++;

		//cout << "simulation number " << iterations << " done.\n";
	}
	// cout << "From Carloh: " << iterations << endl;
	// Return the action to the best node
	if (best_nodes.size())
	{
		Score sc = engine.computeScore(root_state);
		if(best_nodes[0]->get_action().isPass() && !is_winner(AI_COLOR, sc))
		{
			return best_nodes.back()->get_action();
		}

		return best_nodes[0]->get_action();
	}

	// You shouldn't be here.
	assert(!"No best action found");
	return Action();
}

MCTS::~MCTS()
{

}
