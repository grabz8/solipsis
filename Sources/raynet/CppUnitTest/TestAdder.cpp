#include "TestAdder.h"

#include "TestManager.h"
namespace CppTest
{
	
	TestAdder::TestAdder( void (* _pf_FuncToRun) (), const char* _pFuncType, const char* _pDesc)
	{
		TestManager::getInstance().addTest(  _pFuncType,  Test( _pf_FuncToRun,_pDesc)  );
		
	}
	


}



