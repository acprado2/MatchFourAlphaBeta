#include "state.h"


State::State( long long board_p1, long long board_p2, std::string move )
{
	this->board_p1 = board_p1;
	this->board_p2 = board_p2;
	this->move = move;
}


State::~State()
{
}
