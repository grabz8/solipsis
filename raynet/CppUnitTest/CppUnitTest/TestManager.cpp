#include "TestManager.h"

#include <functional>
#include <algorithm>
#include <sstream>

namespace CppTest
{
	
	

	TestManager& TestManager::getInstance()
	{
		static TestManager man;
		return man;
	}

	void TestManager::addTest( const std::string& _str_Type, const Test& _o_test )
	{
		std::string UpperCase( _str_Type);
		UpperCaser( UpperCase );

		m_o_map[UpperCase].push_back( _o_test );
		m_o_types.insert( UpperCase);
	}

	void TestManager::printTypes(  std::ostream& OUT )
	{
/*
		typedef std::vector<Test> TestContainer;
		typedef std::map< TestContainer > TestMap;
*/
		
		OUT <<  std::string(" Types: \n\t\t").c_str();
		std::copy(
			m_o_types.begin(),
			m_o_types.end(),
			std::ostream_iterator<std::string>( OUT, "\n\t\t")
		);

		OUT << std::endl;
	}

	bool TestManager::runAllTests( std::ostream& OUT )
	{
		
		bool bOK = true;
		for ( TestMap::iterator it = m_o_map.begin(); it != m_o_map.end(); it++)
		{
			OUT <<"[" << it->first << "]\n";
			const bool bTest = _testType(  it->second , OUT);
			bOK = ( !bOK ? false : bTest );
		}	

		OUT << std::endl;
		return bOK;
	}

	bool TestManager::runTestType( const std::string& _type, std::ostream&  OUT )
	{			
		bool bOK = false;

		OUT <<"[" << _type  << "]\n";		
		TestMap::iterator itFound = m_o_map.find(_type );
		if ( itFound != m_o_map.end() )
		{
			bOK = _testType(  m_o_map[_type], OUT );		
		}
		else
		{
			OUT << "\tNONE";
		}	

		OUT << std::endl;
		return bOK;
	}


	bool TestManager::_testType( const TestContainer& _tests, std::ostream& OUT )
	{		
		//std::for_each( _tests.begin(), _tests.end(), 
		bool bOK =true;
		for ( TestContainer::const_iterator it = _tests.begin(); it != _tests.end(); it++ )
		{
				const bool bPassed = it->run( OUT);	
				bOK = ( !bPassed ? false : bOK);
		}
		return bOK;
	}

/*
	typedef std::vector<Test> TestContainer;
		typedef std::map< std::string, TestContainer > TestMap;
		TestMap m_o_map;

		typedef std::set< std::string > StringSet;
		StringSet m_o_types;

		bool _testType(  const std::vector<std::string>& _tests);

*/





	//----------------------------- TEST CLASS------------------------------------



		bool Test::run (std::ostream& OUT) const
		{
			//print test out first..
			OUT << m_str_Desc;

			//Get test results
			std::string result = "KO";
			std::string reason = "";

			bool bResult = false;
			try
			{
				(*m_pf_func)();
				result = "OK";
				bResult = true;
			}
			catch ( AssureException& e)
			{		

				std::string ss = e.getFile();
				std::stringstream strm;
				strm << "**ASSURE FAILED**" << "\n  [" << e.getFile().c_str() 
									    << ", " << e.getLine() << "]"
										<< "\n  Expression:" << e.getBoolExp().c_str();
				reason = strm.str();
				
			}
			catch ( std::exception& e)
			{					
				reason = e.what();
			}
			catch ( ...)
			{	
				reason = "UNKNOWN!";				
			}

	
			// justify dots
			const int SCREEN_LENGTH = 80;
			std::string printResult;
			printResult.reserve(SCREEN_LENGTH);
			printResult.insert( printResult.begin() ,
								SCREEN_LENGTH - ( m_str_Desc.size() + result.size()) ,
								'.'
								);

			printResult.append( result );

			//print test out first..
			OUT << printResult << '\n';

			if ( !reason.empty() )
			{
				OUT << reason << std::endl;

			}

			return bResult;
		}










}