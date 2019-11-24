#include "alphabeta.h"


AlphaBeta::AlphaBeta()
{
}


AlphaBeta::~AlphaBeta()
{
}

State AlphaBeta::search( State state, int depth )
{
	successors.clear();
	int v = maxValue( state, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth, 0 );
		
	// Pick a random action from the highest utility moves
	std::vector<State> actions = successors.rbegin()->second;
	return actions.at( rand() % actions.size() );
}

int AlphaBeta::maxValue( State state, int alpha, int beta, int target_depth, int cur_depth )
{
	if ( terminalTest( state ) || cur_depth == target_depth )
	{
		// TODO: Add to transposition table here
		return utility( state );
	}
	int v = std::numeric_limits<int>::min();

	// Generate states
	for ( int i = 0; i < 64; ++i )
	{
		// Check if this tile is already filled
		if ( state.board_p1 & ( 1LL << i ) || state.board_p2 & ( 1LL << i ) )
		{
			continue;
		}
		State s = successor( state, i, true );
		// TODO: Check transposition table here

		if ( v >= beta )
		{
			return v;
		}
		alpha = std::max( alpha, v );

		// Insert successor states into map so we can determine what move to make
		if ( cur_depth == 0 )
		{
			if ( successors.find( v ) != successors.end() )
			{
				successors.at( v ).push_back( s );
			}
			else
			{
				std::vector<State> vec {s};
				successors.insert( { v, vec } );
			}
		}
	}
	return v;
}

int AlphaBeta::minValue( State state, int alpha, int beta, int target_depth, int cur_depth )
{
	if ( terminalTest( state ) || cur_depth == target_depth )
	{
		// TODO: Add to transposition table here (also check if in it)
		return utility( state );
	}
	int v = std::numeric_limits<int>::max();

	// Generate states
	for ( int i = 0; i < 64; ++i )
	{
		// Check if this tile is already filled
		if ( state.board_p1 & ( 1LL << i ) || state.board_p2 & ( 1LL << i ) )
		{
			continue;
		}
		State s = successor( state, i, true );

		v = std::max( v, maxValue( s, alpha, beta, target_depth, cur_depth + 1 ) );

		if ( v <= alpha )
		{
			return v;
		}
		beta = std::max( beta, v );

		// Insert successor states into map so we can determine what move to make
		if ( cur_depth == 0 )
		{
			if ( successors.find( v ) != successors.end() )
			{
				successors.at( v ).push_back( s );
			}
			else
			{
				std::vector<State> vec {s};
				successors.insert( { v, vec } );
			}
		}
	}
	return v;
}

// Check if this is a leaf node
// To make this check as fast as possible, we will check for
// four in a row vertically or horizontally using bit shifts
bool AlphaBeta::terminalTest( State& state )
{
	// Board is 8x8 so split our long into an array of 8-bit characters for 
	// easier manipulation
	unsigned char *p1 = reinterpret_cast<unsigned char *>( &state.board_p1 );
	unsigned char *p2 = reinterpret_cast<unsigned char *>( &state.board_p2 );

	int rowShift = 0; // amount of bits to shift to reach current row
	for ( int i = 0; i < 5; ++i )
	{
		for ( int j = 0; j < 8; ++j )
		{
			unsigned long long vertMatch = 0x1010101LL << ( rowShift + j ); // Vertical four-in-a-row to check

			if ( p1[j] & ( 0xF << i ) || ( state.board_p1 & vertMatch ) == vertMatch )
			{
				state.terminal_p1 = true;
				return true;
			}
			else if ( p2[j] & ( 0xF << i ) || ( state.board_p2 & vertMatch ) == vertMatch )
			{
				state.terminal_p2 = true;
				return true;
			}
		}
		rowShift += 8;
	}
	return false;
}

// Heuristic
int AlphaBeta::utility( State state )
{
	return 0;
}

// Generate a successor node for a given board index
State AlphaBeta::successor( State state, int idx, bool isMin )
{
	State s = state;
	if ( isMin )
	{
		s.board_p2 ^= ( 1LL << idx );
	}
	else
	{
		s.board_p1 ^= ( 1LL << idx );
	}

	// Determine move string
	char row = 'A';
	int col = idx % 8;
	for ( int i = 0; i < 7; ++i )
	{
		idx -= 8;
		if ( idx > 0 )
		{
			++row;
		}
	}
	s.move = row + std::to_string( col );
	return s;
}