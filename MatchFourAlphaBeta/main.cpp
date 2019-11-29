#include <iostream>
#include "alphabeta.h"

State printBoard( bool bAgentFirst, State state, std::vector<std::string> &firstPlayerMoves, std::vector<std::string> &secondPlayerMoves, size_t turnTime );
State opponentMove( State state );
void printPlayerMoves( int idx, State state, std::vector<std::string> firstPlayerMoves, std::vector<std::string> secondPlayerMoves );

int main( )
{
	// NOTE: For UI, if we are player 2, swap returned p1/p2 states and values
	AlphaBeta ab;
	State state( 0LL, 0LL );
	size_t turnTime;
	char turnOrder;
	std::vector<std::string> firstPlayerMoves, secondPlayerMoves;

	std::cout << "Please enter the time to take per turn (in seconds): ";
	std::cin >> turnTime;
	std::cout << "\nPlease enter turn order (A = Agent first | O = Agent second): ";
	std::cin >> turnOrder;
	if ( turnTime <= 0 )
	{
		std::cout << "\nInvalid Inputs. Terminating...\n";
		return 0;
	}
	std::cout << "\n";

	while ( state.terminal_p1 != true && state.terminal_p2 != true )
	{
		state = ( turnOrder == 'A' || turnOrder == 'a' ) ? printBoard( true, ab.search( state, turnTime ), firstPlayerMoves, secondPlayerMoves, turnTime ) : 
														   printBoard( false, opponentMove( state ), firstPlayerMoves, secondPlayerMoves, turnTime );
	}
}

// Print current board state
State printBoard( bool bAgentFirst, State state, std::vector<std::string> &firstPlayerMoves, std::vector<std::string> &secondPlayerMoves, size_t turnTime )
{
	AlphaBeta ab;
	firstPlayerMoves.push_back( state.move );
	if ( !bAgentFirst )
	{
		state = ab.search( state, turnTime ); // replace else state with alphabeta
		secondPlayerMoves.push_back( state.move );
		std::swap( state.board_p1, state.board_p2 ); // swap boards (alphabeta always assumes agent is first)
		std::swap( state.terminal_p1, state.terminal_p2 );
	}

	char row = 'A';
	std::cout << std::string( 4, ' ' ) << "1 2 3 4 5 6 7 8 " << std::string( 5, ' ' ) << ( bAgentFirst ? "Agent vs. Opponent\n" : "Opponent vs. Agent\n" );

	// Print the first 8 moves with the board
	for ( int i = 0; i < 8; ++i )
	{
		std::cout << std::string( 2, ' ' ) << row;
		for ( int j = 0; j < 8; ++j )
		{
			int pos = ( i * 8 ) + j;
			if ( state.board_p1 & ( 1LL  << pos ) )
			{
				std::cout << " " << "X";
			}
			else if ( state.board_p2 & ( 1LL  << pos ) )
			{
				std::cout << " " << "0";
			}
			else
			{
				std::cout << " " << "-";
			}
		}
		std::cout << std::string( 7, ' ' ) << i + 1 << ". ";
		printPlayerMoves( i, state, firstPlayerMoves, secondPlayerMoves );
		std::cout << "\n";
		++row;
	}

	// Print the remaining moves
	for ( unsigned int i = 8; i < firstPlayerMoves.size(); ++i )
	{
		std::cout << std::string( 26, ' ' );
		printPlayerMoves( i, state, firstPlayerMoves, secondPlayerMoves );
		std::cout << "\n";
	}

	// Game is over, return to main()
	if ( state.terminal_p1 || state.terminal_p2 )
	{
		return state;
	}

	std::cout << "\n" << "Agent's move is: " << ( bAgentFirst ? firstPlayerMoves.back() : secondPlayerMoves.back() ) << "\n\n";
	if ( bAgentFirst )
	{
		state = opponentMove( state );
		secondPlayerMoves.push_back( state.move );
	}
	return state;
}

// Receive next opponent move
State opponentMove( State state )
{
	std::cout << "Choose Opponent's next move: ";
	while ( true )
	{
		std::cin >> state.move;

		if ( ( state.move[0] < 'a' || state.move[0] > 'h' ) || ( state.move[1] < '1' || state.move[1] > '8' ) )
		{
			std::cout << "\n\nPlease enter a valid move: ";
		}
		else
		{
			break;
		}
	}
	unsigned int pos = static_cast<unsigned int>( ( 8 * ( state.move[0] - 'a') ) + ( state.move[1] - '1' ) ); // convert move string to a board position (8 * row + col)
	state.board_p2 |= ( 1LL << pos );
	std::cout << "\n\n";
	return state;
}

void printPlayerMoves( int idx, State state, std::vector<std::string> firstPlayerMoves, std::vector<std::string> secondPlayerMoves )
{
	if ( firstPlayerMoves.size() == idx + 1 )
	{
		std::cout << firstPlayerMoves[idx];
		if ( state.terminal_p1 )
		{
			std::cout << " wins\n";
			return;
		}
		std::cout << std::string( 2, ' ' ) << secondPlayerMoves[idx];
		if ( state.terminal_p2 )
		{
			std::cout << " wins\n";
			return;
		}
	}
	else if ( firstPlayerMoves.size() > idx )
	{
		std::cout << firstPlayerMoves[idx] << std::string( 2, ' ' ) << secondPlayerMoves[idx];
	}
}

