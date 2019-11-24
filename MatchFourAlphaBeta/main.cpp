#include <iostream>
#include "alphabeta.h"

int main( )
{
	State state( 0xEA93A6EA, 0LL, "A2"  );
	for ( int i = 0; i < 64; i += 8 )
	{
		for ( int j = i; j < i + 8; ++j )
		{
			if ( state.board_p1 & ( 1LL << j ) )
				std::cout<<"1";
			else
				std::cout<<"0";
		}
		std::cout<<"\n";
	}
	return 0;
}