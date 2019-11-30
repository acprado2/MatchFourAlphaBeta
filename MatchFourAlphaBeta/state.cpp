#include "state.h"


State::State( unsigned long long board_p1, unsigned long long board_p2 )
{
	this->board_p1 = board_p1;
	this->board_p2 = board_p2;
	this->move = move;
	this->utility = 0;
	this->terminal_p1 = false;
	this->terminal_p2 = false;
	this->stalemate = false;
}

State::State( const State &s )
{
	this->board_p1 = s.board_p1;
	this->board_p2 = s.board_p2;
	this->move = s.move;
	this->utility = s.utility;
	this->terminal_p1 = s.terminal_p1;
	this->terminal_p2 = s.terminal_p2;
	this->stalemate = s.stalemate;
}

State::~State()
{
}
