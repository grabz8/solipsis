#include "CppTest.h"

#include "CommandInterpreter.h"
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include "Tools.h"

namespace CppTest
{



	int Run( int _argC, char ** _argv)		
	{
		typedef std::vector<std::string> Strings;
		Strings parameters;
		/*std::copy_n( &_pz_ArgV[0],
					  _argC, 
					  std::back_insert_iterator( parameters));*/

		std::transform( &_argv[0],
						&_argv[ _argC ],
						std::back_insert_iterator< Strings >( parameters),
						std::ptr_fun<char*,char*>( UpperCaser  ) 
		);

		//now all params are in vector, in upper case (easier to interpret)
		CommandInterpreter interpreter( parameters);
		return interpreter.run(  std::cout  ) ? 0 : 1;
		
	}

}