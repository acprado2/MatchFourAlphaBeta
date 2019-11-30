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
}


AlphaBeta::~AlphaBeta()
{
}

State AlphaBeta::search( State state, size_t time )
{
	// TODO: write some function to deal with when to stop searching based on time left
	State res( state );
	for ( int i = 3; i < 4; ++i )
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
	if ( terminalTest( state ) || cur_depth == target_depth )
	{
		//std::pair<unsigned long long, unsigned long long> p = { state.board_p1, state.board_p2 };
		//auto it = m_transposition.find( p );
		//if ( it == m_transposition.end() )
		{
			int res = utility( state );
			//m_transposition[p] = res;
			return res;
		}
		//return it->second;
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
		
		int res = minValue( s, alpha, beta, target_depth, cur_depth + 1 );
		v = std::max( v, res );

		if ( v >= beta )
		{
			return v;
		}
		alpha = std::max( alpha, v );

		// Insert successor states into map so we can determine what move to make
		if ( cur_depth == 0 )
		{
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
			int res = utility( state );
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

		if ( v <= alpha )
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
int AlphaBeta::utility( State state )
{
	// Check if this is a terminal state first (best/worst case)
	if ( state.terminal_p1 )
		return std::numeric_limits<int>::max();
	else if ( state.terminal_p2 )
		return std::numeric_limits<int>::min();
	else if ( state.stalemate )
		return std::numeric_limits<int>::min() - 2; // Stalemates aren't as bad as losing, but are close

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
			unsigned int p1CntHor = m_horizontal[( p1[j] >> i) & 0xF];
			unsigned int p2CntHor = m_horizontal[( p2[j] >> i) & 0xF];
			unsigned int p1CntVert = m_vertical.at( ( state.board_p1 >> ( rowShift + j ) ) & 0x1010101 );
			unsigned int p2CntVert = m_vertical.at( ( state.board_p2 >> ( rowShift + j ) ) & 0x1010101 );

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
				return std::numeric_limits<int>::max() - 1; 
			}
			else if ( p2DangerCnt >= 2 ) 
			{
				// This is a killer move in our opponent's favor
				return std::numeric_limits<int>::min() - 1; 
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