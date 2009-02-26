#include "TestAdder.h"
#include "Tools.h"

/*
	Usage:
		MYTESTAPP -t       : runs all tests
		MYTESTAPP -s       : runs all silently  ( success returns 1  ..failure :  0 )
		MYTESTAPP -t  TYPE : runs only TYPE tests
		MYTESTAPP -s  TYPE : runs only TYPE tests silently
		MYTESTAPP -l       : lists all test types


*/


namespace CppTest
{

	int Run( int _argC, char ** _argv);

}
//creates a static to add to test list
#define ADD_TEST(FUNC,TYPE,DESC)\
	CppTest::TestAdder FUNC##tester (FUNC,#TYPE,DESC);



#define ASSURE(BOOL_EXP)\
	if ( !(BOOL_EXP) ) throw CppTest::AssureException( #BOOL_EXP, __FILE__, __LINE__);