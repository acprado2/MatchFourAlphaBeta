#include <algorithm>
#include <limits>
#include <stdlib.h>
#include <iostream>
#include "alphabeta.h"


AlphaBeta::AlphaBeta()
{
}


AlphaBeta::~AlphaBeta()
{
}

State AlphaBeta::search( State state, size_t time )
{
	// TODO: write some function to deal with when to stop searching based on time left
	State res( state );
	for ( int i = 1; i < 5; ++i )
	{
		res = performSearch( state, i );
	}
	return res;
}

State AlphaBeta::performSearch( State state, int depth )
{
	m_successors.clear();
	int v = maxValue( state, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth, 0 );
		
	// Pick a random action from the highest utility moves
	std::vector<State> actions = m_successors.rbegin()->second;
	return actions.at( rand() % actions.size() );
}

int AlphaBeta::maxValue( State state, int alpha, int beta, int target_depth, int cur_depth )
{
	std::cout << "MAX PLY: " << cur_depth << "\n";
	if ( terminalTest( state ) || cur_depth == target_depth )
	{
		std::pair<unsigned long long, unsigned long long> p = {state.board_p1, state.board_p2 };
		auto it = m_lookup.find( p );
		if ( it == m_lookup.end() )
		{
			int res = utility( state );
			m_lookup[p] = res;
			return res;
		}
		return it->second;
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
		State s = successor( state, i, false );
		
		v = std::min( v, minValue( s, alpha, beta, target_depth, cur_depth + 1 ) );

		if ( v >= beta )
		{
			return v;
		}
		alpha = std::max( alpha, v );

		// Insert successor states into map so we can determine what move to make
		if ( cur_depth == 0 )
		{
			if ( m_successors.find( v ) != m_successors.end() )
			{
				m_successors.at( v ).push_back( s );
			}
			else
			{
				std::vector<State> vec {s};
				m_successors.insert( { v, vec } );
			}
		}
	}
	return v;
}

int AlphaBeta::minValue( State state, int alpha, int beta, int target_depth, int cur_depth )
{
	std::cout << "MIN PLY: " << cur_depth << "\n";
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
			if ( m_successors.find( v ) != m_successors.end() )
			{
				m_successors.at( v ).push_back( s );
			}
			else
			{
				std::vector<State> vec {s};
				m_successors.insert( { v, vec } );
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
			unsigned char horiMatch = 0xF << i; // Horizontal four-in-a-row to check

			if ( ( p1[j] & horiMatch ) == horiMatch || ( state.board_p1 & vertMatch ) == vertMatch )
			{
				state.terminal_p1 = true;
				return true;
			}
			else if ( ( p2[j] & horiMatch ) == horiMatch || ( state.board_p2 & vertMatch ) == vertMatch )
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
	// Check if this is a terminal state first (best/worst case)
	if ( state.terminal_p1 )
		return std::numeric_limits<int>::max();
	else if ( state.terminal_p2 )
		return std::numeric_limits<int>::min();

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