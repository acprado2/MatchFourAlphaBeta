#include <iostream>
#include <time.h>
#include "alphabeta.h"

State printBoard( bool bAgentFirst, State state, std::vector<std::string> &firstPlayerMoves, std::vector<std::string> &secondPlayerMoves, size_t turnTime );
State opponentMove( State state );
bool printPlayerMoves( unsigned int idx, State state, std::vector<std::string> firstPlayerMoves, std::vector<std::string> secondPlayerMoves );

int main( )
{
	// NOTE: For UI, if we are player 2, swap returned p1/p2 states and values
	srand( static_cast<unsigned int>( time(NULL) ) );
	State state( 0LL, 0LL );
	size_t turnTime;
	char turnOrder;
	std::vector<std::string> firstPlayerMoves, secondPlayerMoves;

	std::cout << "Please enter the time to take per turn (in seconds): ";
	std::cin >> turnTime;
	std::cout << "\nPlease enter turn order (A = Agent first | O = Opponent first): ";
	std::cin >> turnOrder;
	if ( turnTime <= 0 )
	{
		std::cout << "\nInvalid Inputs. Terminating...\n";
		return 0;
	}
	std::cout << "\n";

	AlphaBeta ab;
	while ( state.terminal_p1 != true && state.terminal_p2 != true && state.stalemate != true )
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
		std::swap( state.terminal_p1, state.terminal_p2 ); // swap terminal states (alphabeta always assumes agent is first)
	}

	// Quickly test state for terminality
	ab.terminalTest( state );

	char row = 'A';
	std::cout << std::string( 4, ' ' ) << "1 2 3 4 5 6 7 8 " << std::string( 5, ' ' ) << ( bAgentFirst ? "Agent vs. Opponent\n" : "Opponent vs. Agent\n" );
	bool bPrintPlayerMoves = true; // Check so that we don't print extra moves after game is over

	// Print the first 8 moves with the board
	for ( unsigned int i = 0; i < 8; ++i )
	{
		std::cout << std::string( 2, ' ' ) << row;
		for ( unsigned int j = 0; j < 8; ++j )
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
		if ( bPrintPlayerMoves )
		{
			bPrintPlayerMoves = printPlayerMoves( i, state, firstPlayerMoves, secondPlayerMoves );
		}
		std::cout << "\n";
		++row;
	}

	// Print the remaining moves
	for ( unsigned int i = 8; i < firstPlayerMoves.size(); ++i )
	{
		std::cout << std::string( 26, ' ' ) << i + 1 << ". ";
		if ( !printPlayerMoves( i, state, firstPlayerMoves, secondPlayerMoves ) )
		{
			break;
		}
		std::cout << "\n";
	}

	if ( state.terminal_p1 || state.terminal_p2 )
	{
		// Game is over, return to main()
		return state;
	}
	else if ( state.stalemate )
	{
		// Board has filled with no victor
		std::cout << "Stalemate!\n";
		return state;
	}

	std::cout << "\n" << "Agent's move is: " << ( bAgentFirst ? firstPlayerMoves.back() : secondPlayerMoves.back() ) << "\n\n";
	if ( bAgentFirst )
	{
		state = opponentMove( state );
		secondPlayerMoves.push_back( state.move );
	}
	else
	{
		// Re-swap now that we've finished printing to the screen
		std::swap( state.terminal_p1, state.terminal_p2 );
	}
	return state;
}

// Receive next opponent move
State opponentMove( State state )
{
	unsigned int pos;
	std::cout << "Choose Opponent's next move: ";
	while ( true )
	{
		std::cin >> state.move;
		pos = static_cast<unsigned int>( ( 8 * ( state.move[0] - 'a') ) + ( state.move[1] - '1' ) ); // convert move string to a board position (8 * row + col)
		// Check that this is a valid move
		if ( ( state.move[0] < 'a' || state.move[0] > 'h' ) || ( state.move[1] < '1' || state.move[1] > '8' ) )
		{
			std::cout << "\n\nPlease enter a valid move: ";
		}
		else if ( state.board_p1 & ( 1LL << pos ) || state.board_p2 & ( 1LL << pos ) )
		{
			std::cout << "\n\nThat move has already been played. Please select another move: ";
		}
		else
		{
			break;
		}
	}
	state.board_p2 |= ( 1LL << pos );
	std::cout << "\n\n";
	return state;
}

// Print a row of player moves. Returns true if we should stop printing player moves
bool printPlayerMoves( unsigned int idx, State state, std::vector<std::string> firstPlayerMoves, std::vector<std::string> secondPlayerMoves )
{
	if ( firstPlayerMoves.size() == idx + 1 )
	{
		std::cout << firstPlayerMoves[idx];
		if ( state.terminal_p1 )
		{
			std::cout << " wins";
			return false;
		}
	}
	else if ( firstPlayerMoves.size() > idx )
	{
		std::cout << firstPlayerMoves[idx];
	}

	if ( secondPlayerMoves.size() == idx + 1 )
	{
		std::cout << std::string( 2, ' ' ) << secondPlayerMoves[idx];
		if ( state.terminal_p2 )
		{
			std::cout << " wins";
			return false;
		}
	}
	else if ( secondPlayerMoves.size() > idx )
	{
		std::cout << std::string( 2, ' ' ) << secondPlayerMoves[idx];
	}

	return true;
}

