#ifndef __TOOLS_FOR_CPPTEST___
#define __TOOLS_FOR_CPPTEST___

#include <string>
#include <algorithm>
#include <functional>

// I do not need Visual C telling me about transform funcs
#pragma warning( disable : 4996 )



namespace CppTest
{

	inline char* UpperCaser( char* _pParam)
	{		
		std::transform( &_pParam[0], &_pParam[::strlen(_pParam) ], &_pParam[0] , std::ptr_fun( &toupper ) );		
		return _pParam;
		
	}


	inline std::string& UpperCaser( std::string& _pParam)
	{		
		std::transform( _pParam.begin(), _pParam.end(),  _pParam.begin() , std::ptr_fun( &toupper ));		
		return _pParam;
		
	}


	
	class AssureException {
		
		std::string m_str_exp;
		std::string m_str_file;
		int m_i_line;

	public:

		AssureException( const char* _boolExp, const char* _pFile, int _line)	
			:
		m_str_exp( _boolExp ),
		m_str_file( _pFile),
		m_i_line(_line)
		{
		}

		const std::string& getFile()
		{
			return m_str_file;
		}

		int getLine() 
		{
			return m_i_line;
		}

		const std::string& getBoolExp()
		{ 
			return m_str_exp;
		}		
	};



}


#endif