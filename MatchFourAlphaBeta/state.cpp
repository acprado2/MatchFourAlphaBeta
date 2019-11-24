#include "state.h"


State::State( unsigned long long board_p1, unsigned long long board_p2, std::string move )
{
	this->board_p1 = board_p1;
	this->board_p2 = board_p2;
	this->move = move;
	this->utility = 0;
	this->terminal_p1 = false;
	this->terminal_p2 = false;
}


State::~State()
{
}
