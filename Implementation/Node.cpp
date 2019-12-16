#include "definitions.h"
#include "Node.h"
#include "assert.h"
#include <stdlib.h>
#include <time.h> 

Node::Node(State& state_, Node* parent_ = NULL) :
	state(state_),
	action(),
	parent(parent_),
	visits_count(0),
	wins(0),
	depth(parent_ ? parent_->depth + 1 : 0)
{}

//All nodes share the same game engine.
GoEngine Node::engine = GoEngine();

Node* Node::expand(CellState AI_COLOR)
{
	// First check if this Node is fully expanded or not.
	if (is_fully_expanded()) return NULL;
	srand(time(NULL));
	// Check if this is the first expansion.
	if (children.empty()) {

		// Get the list of actions that this Node can do.
		actions = engine.getValidMoves(&state, (parent == NULL ? NULL : &(parent->state)), Switch(state.get_color()));
		
		// NOTE: if "pass" is included in the list of actions. then it will be the last one.
		
		//if( !actions.empty() && actions.back().isPass() &&  !is_winner(AI_COLOR, engine.computeScore(state)) ) actions.pop_back();
		//if(actions.empty()) return NULL;
		
		// NOTE: assuming there are another actions in the list.

		//puts("List of Valid Actions");
		//for(auto x:actions) cout << x;
		//state.get_actions(actions);
		// Shuffle these actions. To randomly pick a child.
		std::random_shuffle(actions.begin(), actions.end());  // This part will change when ML comes.
	}

	// If there is no actions, then this Node is a terminal Node, and u shouldn't be here!
	assert("ERROR! the Node have no actions to perform and classified as normal Node not terminal node" && !actions.empty());
	//cout << "The size of the actions I have: " << actions.size() << endl;
    //puts("will add child now and the action is");
	Node* child = add_child(actions.back()); // NOTE: for every call of function "expand",this function will
	actions.pop_back();
	return child;
}

void Node::update(Result reward)
{
	wins += (reward == WIN); // MIND BELOW.
	visits_count++;
}

Node* Node::add_child(const Action& new_action)
{
	// Create a new Node with the same state.
	Node* child = new Node(state, this);
	// Fill its action.
	child->action = new_action;
	// Change its state to the new state( old state + new action )
	engine.applyValidAction(child->state, new_action);
	// Now add it to the children list.
	children.push_back(child);
	return child;
}
Node::~Node(){
	for(auto ptr:children)
		delete ptr;
}
