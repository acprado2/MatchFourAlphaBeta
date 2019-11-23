#ifndef ALPHABETA
#define ALPHABETA

class AlphaBeta
{
public:
	AlphaBeta();
	~AlphaBeta();
}; // AlphaBeta

#endif // ALPHABETA

/*
function Alpha-Beta-Search(state) returns an action
	inputs: state, current state in game

	v<- MAX-VALUE(state, -inf, +inf)
	returns the action in SUCCESSORS(state) with value v
---------------------------------------------------------
function MAX-VALUE(state, A, B) returns a utility value
	inputs: state, current state in game
	A, the value of the best alternative for MAX along the path to state
	B, the value of the best alternative for MIN along the path to state

	if TERMINAL-TEST(state) then return UTILITY(state)
	v<- -inf
	for a, s in SUCCESSORS(state) do
		v<- MAX(v, MIN-VALUE(s, A, B))
		if v >= B then return v
		A <- MAX(A, v)
	return v

function MIN-VALUE(state, A, B) returns a utility value
	inputs: state, current state in game
		A, the value of the best alternative for MAX along the path to state
		B, the value of the best alternative for MIN along the path to state
	if TERMINAL-TEST(state) then return UTILITY(state)
	v<- +inf
	for a, s in SUCCESSORS(state) do
		v<- MIN(v, MAX-VALUE(s, A, B))
		if v <= A then return v
		B<- MIN(B, v)
	return v
*/