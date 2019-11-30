#ifndef STATE
#define STATE

#include <string>

class State
{
public:
	State( unsigned long long board_p1, unsigned long long board_p2 );
	State( const State &s );
	~State();

	// Use int_64 for board states so we can do bitwise math for evaluation (fast!)
	unsigned long long board_p1; // Pieces on the board for player one
	unsigned long long board_p2; // Pieces on the board for player two

	int utility; // Utility value for this state
	std::string move; // Move taken for this state
	bool terminal_p1;
	bool terminal_p2;
	bool stalemate;
}; // State

#endif // STATE
