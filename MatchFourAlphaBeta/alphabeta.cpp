#include <algorithm>
#include <limits>
#include <stdlib.h>
#include <iostream>
#include "alphabeta.h"

// Lookup tables count the number of set bits so we can determine danger states in constant time
const unsigned int AlphaBeta::m_horizontal[16] = { 
	0, 1, 1, 2, 
	1, 2, 2, 3, 
	1, 2, 2, 3, 
	2, 3, 3, 4 };

const std::unordered_map<unsigned int, unsigned int> AlphaBeta::m_vertical = { 
	{ 0x0000000, 0 }, { 0x0000001, 1 }, { 0x0000100, 1 }, { 0x0000101, 2 }, 
	{ 0x0010000, 1 }, { 0x0010001, 2 }, { 0x0010100, 2 }, { 0x0010101, 3 },
	{ 0x1000000, 1 }, { 0x1000001, 2 }, { 0x1000100, 2 }, { 0x1000101, 3 },
	{ 0x1010000, 2 }, { 0x1010001, 3 }, { 0x1010100, 3 }, { 0x1010101, 4 } };

AlphaBeta::AlphaBeta()
{
	m_turnTime = 20;
	m_startTime = 0;
}


AlphaBeta::~AlphaBeta()
{
}

State AlphaBeta::search( State state, size_t turnTime )
{
	m_turnTime = turnTime;
	m_startTime = static_cast<size_t>( time( NULL ) );
	m_successors.clear();
	State res( state );
	for ( int i = 4; ( ( i < 6 ) && shouldContinueSearch() ); ++i )
	{
		State tmp = performSearch( state, i );
		if ( tmp != state )
		{
			// Don't overwrite our previous search if the current one terminates early
			res = tmp;
		}
	}
	return res;
}

State AlphaBeta::performSearch( State state, int depth )
{
	int v = maxValue( state, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), depth, 0 );
		
	// Pick a random action from the highest utility moves
	if ( m_successors.size() > 0 )
	{
		std::vector<State> actions = m_successors.rbegin()->second;
		return actions.at( rand() % actions.size() );
	}

	// We've been passed a terminal state
	return state;
}

int AlphaBeta::maxValue( State state, int alpha, int beta, int target_depth, int cur_depth )
{
	if ( terminalTest( state ) || cur_depth == target_depth )
	{
		//std::pair<unsigned long long, unsigned long long> p = { state.board_p1, state.board_p2 };
		//auto it = m_transposition.find( p );
		//if ( it == m_transposition.end() )
		{
			int res = utility( state, cur_depth );
			//m_transposition[p] = res;
			return res;
		}
		//return it->second;
	}
	int v = std::numeric_limits<int>::min();

	// Generate states
	if ( cur_depth == 0 && m_successors.size() > 0 )
	{
		std::map<int, std::vector<State>> tmpSuccessors = m_successors;
		m_successors.clear();
		for( auto it = tmpSuccessors.rbegin(); it != tmpSuccessors.rend(); ++it )
		{
			std::vector<State> actions = it->second;
			for ( int i = 0; i < actions.size(); ++i )
			{
				State s( actions[i] );
				int res = minValue( s, alpha, beta, target_depth, cur_depth + 1 );
				v = std::max( v, res );

				alpha = std::max( alpha, v );

				// Insert successor states into map so we can determine what move to make
				if ( cur_depth == 0 )
				{
					// Only check the time at depth 0 to reduce overhead
					if ( timeElapsed() )
					{
						// We're out of time. Terminate the search
						m_successors.clear();
						return v;
					}

					s.utility = res;
					if ( m_successors.find( res ) != m_successors.end() )
					{
						m_successors.at( res ).push_back( s );
					}
					else
					{
						std::vector<State> vec {s};
						m_successors.insert( { res, vec } );
					}
				}
			}
		}
	}
	else
	{
		for ( int i = 0; i < 64; ++i )
		{
			// Check if this tile is already filled
			if ( state.board_p1 & ( 1LL << i ) || state.board_p2 & ( 1LL << i ) )
			{
				continue;
			}
			State s = successor( state, i, false );
		
			int res = minValue( s, alpha, beta, target_depth, cur_depth + 1 );
			v = std::max( v, res );

			if ( v > beta )
			{
				return v;
			}
			alpha = std::max( alpha, v );

			// Insert successor states into map so we can determine what move to make
			if ( cur_depth == 0 )
			{
				// Only check the time at depth 0 to reduce overhead
				if ( timeElapsed() )
				{
					// We're out of time. Terminate the search
					m_successors.clear();
					return v;
				}

				s.utility = res;
				if ( m_successors.find( res ) != m_successors.end() )
				{
					m_successors.at( res ).push_back( s );
				}
				else
				{
					std::vector<State> vec {s};
					m_successors.insert( { res, vec } );
				}
			}
		}
	}
	return v;
}

int AlphaBeta::minValue( State state, int alpha, int beta, int target_depth, int cur_depth )
{
	if ( terminalTest( state ) || cur_depth == target_depth )
	{
		//std::pair<unsigned long long, unsigned long long> p = { state.board_p1, state.board_p2 };
		//auto it = m_transposition.find( p );
		//if ( it == m_transposition.end() )
		{
			int res = utility( state, cur_depth );
			//m_transposition[p] = res;
			return res;
		}
		//return it->second;
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

		v = std::min( v, maxValue( s, alpha, beta, target_depth, cur_depth + 1 ) );

		if ( v < alpha )
		{
			return v;
		}
		beta = std::min( beta, v );
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

	// Check if the entire board has filled (stalemate)
	if ( ( state.board_p1 ^ state.board_p2 ) == std::numeric_limits<unsigned long long>::max() )
	{
		state.stalemate = true;
		return true;
	}

	return false;
}

// Heuristic
int AlphaBeta::utility( State state, int depth )
{
	// Check if this is a terminal state first (best/worst case)
	// Give better priority to higher up terminal nodes so we stop loitering on killer move states
	if ( state.terminal_p1 )
		return std::numeric_limits<int>::max() - depth;
	else if ( state.terminal_p2 )
		return std::numeric_limits<int>::min() + depth;
	else if ( state.stalemate )
		return std::numeric_limits<int>::min() + 100; // Stalemates aren't as bad as losing, but are close

	// Check for danger states
	// Danger states occur when in a four-tile span there are three tiles 
	// of a single type with a blank in the last space. This becomes a killer
	// move if there are more than of these in a single state
	unsigned char *p1 = reinterpret_cast<unsigned char *>( &state.board_p1 );
	unsigned char *p2 = reinterpret_cast<unsigned char *>( &state.board_p2 );

	int rowShift = 0; // amount of bits to shift to reach current row
	int p1DangerCnt = 0, p2DangerCnt = 0; // number of danger tiles in the state
	int p1PairCnt = 0, p2PairCnt = 0, p1SingleCnt = 0, p2SingleCnt = 0; // Pairs and single tiles in the sate
	for ( int i = 0; i < 5; ++i )
	{
		for ( int j = 0; j < 8; ++j )
		{
			// Check for danger tiles by counting horizontal/vertical bits
			unsigned int shiftAmount = rowShift + j;
			unsigned int p1CntHor = m_horizontal[( p1[j] >> i) & 0xF];
			unsigned int p2CntHor = m_horizontal[( p2[j] >> i) & 0xF];
			unsigned int p1CntVert = m_vertical.at( ( state.board_p1 >> ( shiftAmount ) ) & 0x1010101 );
			unsigned int p2CntVert = m_vertical.at( ( state.board_p2 >> ( shiftAmount ) ) & 0x1010101 );

			// Horizontal
			if ( p1CntHor == 0 )
			{
				switch ( p2CntHor )
				{
				case 1:
					p2SingleCnt += 3;
					break;
				case 2:
					p2PairCnt += 8;
					break;
				case 3:
					++p2DangerCnt;
				}
			}
			else if ( p1CntHor >= 1 )
			{
				switch ( p2CntHor )
				{
				case 1:
					p2SingleCnt += 3 - p1CntHor;
					break;
				case 2:
					p2PairCnt += ( p1CntHor == 1 ) ? 5 : 0;
					break;
				}
			}

			if ( p2CntHor == 0 )
			{
				switch ( p1CntHor )
				{
				case 1:
					p1SingleCnt += 3;
					break;
				case 2:
					p1PairCnt += 8;
					break;
				case 3:
					++p1DangerCnt;
					break;
				}
			}
			else if ( p2CntHor >= 1 )
			{
				switch ( p1CntHor )
				{
				case 1:
					p1SingleCnt += 3 - p2CntHor;
					break;
				case 2:
					p1PairCnt += ( p2CntHor == 1 ) ? 5 : 0;
					break;
				}
			}

			// Vertical
			if ( p1CntVert == 0 )
			{
				switch ( p2CntVert )
				{
				case 1:
					p2SingleCnt += 3;
					break;
				case 2:
					p2PairCnt += 8;
					break;
				case 3:
					++p2DangerCnt;
					break;
				}
			}
			else if ( p1CntVert >= 1 )
			{
				switch ( p2CntVert )
				{
				case 1:
					p2SingleCnt += 3 - p1CntVert;
					break;
				case 2:
					p2PairCnt += ( p1CntVert == 1 ) ? 5 : 0;
					break;
				}
			}

			if ( p2CntVert == 0 )
			{
				switch ( p1CntVert )
				{
				case 1:
					p1SingleCnt += 3;
					break;
				case 2:
					p1PairCnt += 8;
					break;
				case 3:
					++p1DangerCnt; 
					break;
				}
			}
			else if ( p2CntVert >= 1 )
			{
				switch ( p1CntVert )
				{
				case 1:
					p1SingleCnt += 3 - p2CntVert;
					break;
				case 2:
					p1PairCnt += ( p2CntVert == 1 ) ? 5 : 0;
					break;
				}
			}

			if ( p1DangerCnt >= 2 )
			{
				// This is a killer move in our favor
				return std::numeric_limits<int>::max() - 50; 
			}
			else if ( p2DangerCnt >= 2 ) 
			{
				// This is a killer move in our opponent's favor
				return std::numeric_limits<int>::min() + 50; 
			}
		}
		rowShift += 8;
	}
	return ( ( p1DangerCnt - p2DangerCnt ) * 10000 ) + ( p1PairCnt - p2PairCnt ) + ( p1SingleCnt - p2SingleCnt ); // Danger tiles should have priority in dealing with
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
	char row = 'a';
	int col = idx % 8 + 1;
	for ( int i = 0; i < 7; ++i )
	{
		idx -= 8;
		if ( idx >= 0 )
		{
			++row;
		}
	}
	s.move = row + std::to_string( col );
	return s;
}

// Check if we should continue searching deeper in the tree
bool AlphaBeta::shouldContinueSearch()
{
	size_t elapsed = static_cast<size_t>( time( NULL ) ) - m_startTime;
	if ( elapsed > ( m_turnTime ) / 2 )
	{
		// Don't start another search if more than half of our time has elapsed
		return false;
	}
	return true;
}

// Check if we should stop our current search
bool AlphaBeta::timeElapsed()
{
	size_t elapsed = static_cast<size_t>( time( NULL ) ) - m_startTime;
	if ( static_cast<int>( m_turnTime - elapsed ) <= 5 )
	{
		// Cancel our search when we have five or less seconds remaining
		return true;
	}
	return false;
}