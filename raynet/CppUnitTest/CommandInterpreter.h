#ifndef __TEST_INTERPRETER_MAIN_FILE__
#define __TEST_INTERPRETER_MAIN_FILE__


#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "TestManager.h"
namespace CppTest
{
	class CommandInterpreter
	{
		Strings m_arr_params;
		void _printHeader( std::ostream& OUT);

	public:
		CommandInterpreter( const Strings _params ):
		  m_arr_params( _params){}
		  
		  bool run( std::ostream& );
	};

}

#endif