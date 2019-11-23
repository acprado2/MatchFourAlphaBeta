#ifndef STATE
#define STATE

#include <string>

class State
{
public:
	State( long long board_p1, long long board_p2, std::string move );
	~State();

	long long board_p1; // Pieces on the board for player one
	long long board_p2; // Pieces on the board for player two
	int utility; // Utility value for this state
	std::string move; // Move taken for this state
}; // State

#endif // STATE
