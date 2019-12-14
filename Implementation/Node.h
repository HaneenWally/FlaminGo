#pragma once
#include "definitions.h"
#include "State.h"
#include "GoEngine.h"

// This class impelements the Node of Carloh Tree.
class Node
{
private:
	State state;		// the state of this Node
	Action action;	// the action which led to the state of this Node
	Node* parent;		    // parent of this TreeNode
	int visits_count;		// number of times this Node has been visited
	int wins;               // number of wins.
	int depth;              // the depth of this Node
	static GoEngine engine;
	//Player player;		    	// player who made the decision. [USELESS DATA]
	//CellState color;	// The color which made the decision. [Instead of player].

	std::vector< Node* > children;	            // all current children
	std::vector< Action > actions;		        // possible actions from this state

	Node* add_child(const Action& new_action);  // add child with a specific action.

public:

	Node(State&, Node*);
	~Node();
	Node* expand(CellState);                                 //adding a single child to this Node
	void update(Result);
	//--------------

	// ------Getters

	// state of the Node
	const State& get_state() const { return state; }

	// the action that led to this state
	Action get_action() const { return action; }

	// all children have been expanded and simulated
	bool is_fully_expanded() const { return !children.empty() && actions.empty(); }

	// does this Node end the search (i.e. the game)
	bool is_terminal() {
	    Action tmp(Switch(state.get_color()),-2,-2);
		return engine.isGoal(state, action, parent == NULL? tmp:parent->get_action());
		//return state.is_terminal();
	}

	// number of times this Node has been visited
	int get_num_visits() const { return visits_count; }

	// how deep the Node is in the tree
	int get_depth() const { return depth; }

	int get_wins() const { return wins; }

	// number of children the TreeNode has
	int get_num_children() const { return children.size(); }

	// get the i'th child
	Node* get_child(int i) const { return children[i]; }

	// get parent
	Node* get_parent() const { return parent; }



};
/*
This part will be instead of state.evaluate.
Score score = engine.computeScore(currentState);
return score.white > score.black ? (WIN)
*/
