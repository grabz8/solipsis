#ifndef __TEST_MANAGER_MAIN_FILE__
#define __TEST_MANAGER_MAIN_FILE__


#include <vector>
#include <map>
#include <set>

#include <string>


#include "Tools.h"

//TestManager::getInstance().addTest(  _pFuncType , Test( _pf_FuncToRun,  _pDesc)  );
namespace CppTest
{
	
		
	class Test
	{	
		typedef void (*FuncType) ();		
		FuncType m_pf_func;
		std::string m_str_Desc;

	public:
		Test( FuncType _pfun,  const char* _Desc):
			m_pf_func( _pfun),
			m_str_Desc(_Desc)
		{}
	
		FuncType getFunc()const { return m_pf_func;}
		const std::string& getDesc() const{ return m_str_Desc;}

		bool run (std::ostream& OUT) const;
	};
	typedef std::vector<Test> Tests;




//-------------------------
	typedef std::vector<std::string> Strings;

/*	struct CaseLess : public std::binary_function<int, std::string, std::string>
	{
		int operator() ( const std::string& _left, const std::string& _right)
		{
			UpperCaser( _left.c_str() );
			UpperCaser( _right.c_str() );
		return 0;
		}
	};
*/

	class TestManager
	{
	private:
	
		typedef std::vector<Test> TestContainer;
		typedef std::map< std::string, TestContainer  > TestMap;
		TestMap m_o_map;

		typedef std::set< std::string  > StringSet;
		StringSet m_o_types;

		bool _testType(  const TestContainer& _tests, std::ostream& OUT);
		
	public:

		//~TestManager();

		static TestManager& getInstance();
		
		void printTypes (  std::ostream& OUT);

		void addTest( const std::string& _str_Type, const Test& _o_test );

		bool runAllTests( std::ostream&);

		bool runTestType( const std::string& _type, std::ostream&);

	};

}

#endif