#ifndef ALPHABETA
#define ALPHABETA

#include <map>
#include <unordered_map>
#include <time.h>
#include <vector>
#include "state.h"

// Hash function for our lookup table
/*struct BoardHash
{
	std::size_t operator()( const std::pair<unsigned long long, unsigned long long>& p ) const
	{
		size_t res = p.first * 0x243F6A8885A308D3LL; // multiply by pi
		res ^= p.second;
		return res;
	}
};*/

class AlphaBeta
{
public:
	AlphaBeta();
	~AlphaBeta();

	// Iterative deepening search
	State search( State state, size_t turnTime );

	// Check if a given state is a terminal/leaf node
	bool terminalTest( State& state );

private:

	// Searches to fixed depth
	State performSearch( State state, int depth );

	int maxValue( State state, int alpha, int beta, int target_depth, int cur_depth );
	int minValue( State state, int alpha, int beta, int target_depth, int cur_depth );

	int utility( State state, int depth );
	State successor( State state, int idx, bool isMin );

	bool shouldContinueSearch();
	bool timeElapsed();

	std::map<int, std::vector<State>> m_successors; // Map of successor states with potential actions to take (K = util, V = state)
	//std::unordered_map<std::pair<unsigned long long, unsigned long long>, int, BoardHash> m_transposition;

	// Lookup tables
	static const unsigned int m_horizontal[];
	static const std::unordered_map<unsigned int, unsigned int> m_vertical;

	// Time values
	size_t m_turnTime;
	size_t m_startTime;
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