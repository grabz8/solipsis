#include "CommandInterpreter.h"

#include "TestManager.h"
#include <functional>
#include <iterator>
#include <sstream>
namespace CppTest
{

	const char* const LIST_COMMAND = "-L";
	const char* const SILENT_COMMAND = "-S";
	const char* const TYPE_COMMAND = "-T";

	//offsets
	const int COMMAND = 1;
	const int TYPE = 2;

	//quantity params
	const int HAS_CHOSE_TYPE = 3;



#define NOT_A_KNOWN_COMMAND ( m_arr_params[COMMAND] != TYPE_COMMAND &&\
	m_arr_params[COMMAND] != LIST_COMMAND && \
	m_arr_params[COMMAND] != SILENT_COMMAND )


#define PARAM_QU m_arr_params.size()
#define COMMANDS_HAVE_WRONG_PARAMS (m_arr_params[COMMAND] == SILENT_COMMAND && (PARAM_QU != 3  || PARAM_QU != 2)   ) ||\
	(m_arr_params[COMMAND] == TYPE_COMMAND && (PARAM_QU != 3  || PARAM_QU != 2)    ) ||\
	(m_arr_params[COMMAND] == LIST_COMMAND && PARAM_QU != 2) \


#define CORRECT_PARAMETERS \
	(PARAM_QU == 2 && ( m_arr_params[COMMAND] ==  SILENT_COMMAND ||  m_arr_params[COMMAND] == TYPE_COMMAND || m_arr_params[COMMAND] == LIST_COMMAND) || \
	PARAM_QU == 3 && ( m_arr_params[COMMAND] ==  SILENT_COMMAND ||  m_arr_params[COMMAND] == TYPE_COMMAND ) || \
	false)

	//output devices
	std::stringstream SILENT;
#define OUT std::cout 



	bool CommandInterpreter::run( std::ostream& _OUT )
	{
		bool bOK = false;
		//assure that only TWO or THREE parameters exist



		if(  !CORRECT_PARAMETERS	)
		{
			_OUT << "	Usage:\n"
				"\tMYTESTAPP -t       : runs all tests\n"
				"\tMYTESTAPP -s       : runs all silently  ( success returns 1  ..failure :  0 )\n"
				"\tMYTESTAPP -t  TYPE : runs only TYPE tests\n"
				"\tMYTESTAPP -s  TYPE : runs only TYPE tests silently\n"
				"\tMYTESTAPP -l       : lists all test types\n";
		}
		else{
			//OK
			if  ( m_arr_params[COMMAND] == LIST_COMMAND) 
			{
				_printHeader( _OUT);
				TestManager::getInstance().printTypes( _OUT);				
				bOK = true;

			}
			else if( m_arr_params[COMMAND] == SILENT_COMMAND )  
			{	
				if ( m_arr_params.size() == HAS_CHOSE_TYPE )				
					bOK = TestManager::getInstance().runTestType( m_arr_params[TYPE], SILENT );							
				else
					bOK = TestManager::getInstance().runAllTests(  SILENT );			
			}
			else 
			{
				_printHeader( _OUT);
				if ( m_arr_params.size() == HAS_CHOSE_TYPE )
					bOK = TestManager::getInstance().runTestType( m_arr_params[TYPE], _OUT);			
				else
					bOK = TestManager::getInstance().runAllTests(  _OUT);			

				if ( bOK )
					_OUT << "\n ************** ALL OK ***************" << std::endl;
				else
					_OUT << "\n **************   KO   ***************" << std::endl;
			}
		}


		return bOK;
	}


	void CommandInterpreter::_printHeader( std::ostream& _OUT )
	{
		_OUT << "\n*************UNIT TESTER************** "<< std::endl;
	}

}