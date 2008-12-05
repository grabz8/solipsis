#ifndef __CPP_TEST_TEST_ADDER__
#define __CPP_TEST_TEST_ADDER__

namespace CppTest
{
	class TestAdder
	{
	public:
		TestAdder( void (* FuncToRun) (), const char* FuncType, const char* desc);
	};


}
#endif
