#ifndef __P2P__LOG_FILE__
#define __P2P__LOG_FILE__



#include <string>
#include <fstream>
#include <sstream>
#include "P2PTools.h"



namespace P2P
{


#define LOG(DATA)\
	if( P2P::LogFile::isLogging( P2P::LogFile::LOW )  )\
	{\
		P2P::LogFile::MacroLocker obj;\
		P2P::LogFile::getLogStream() << DATA;  \
	}

#define LOG_TABLE(DESC,TABLE)\
	if( P2P::LogFile::isLogging( P2P::LogFile::HIGH )  )\
	{\
		P2P::LogFile::MacroLocker obj;\
		P2P::LogFile::LogTable( DESC,TABLE);  \
	}





	class LogFile
	{
	public:
		DEFINE_EXCEPTION(LogFileFailed)
		DEFINE_EXCEPTION(LogInitException)

		enum LogLevel {NONE, LOW, HIGH};

	private:
		
		
		
		//init values
		static std::string s_str_file;
		volatile static LogLevel s_e_level;		
		
		static LogFile& getInstance();


		std::ofstream m_o_logfile;
		LogFile( const std::string& _file, LogLevel);
		void printTime();

	public:
			

		template <class Container>
		static void LogTable( const std::string& _desc,const Container& _cont);

		static std::ostream& getLogStream();
		static void init( const std::string& _file, const std::string& LogLevel );			

		static bool isLogging( LogLevel _level) { return s_e_level >=_level;}
	

		//used for providing safe macros
		struct MacroLocker
		{
			MacroLocker();
			~MacroLocker();
		};
		
	};

	//---------------------------
	template <class Container>
	void LogFile::LogTable( const std::string& _desc, const Container& _cont)
	{
		
			//all nodes have logWrite... call NodeObj::logWrite( node, ostream)  on each Node type object 

			LogFile& log = getInstance();		
			log.printTime();			

			log.m_o_logfile << "\n--------" << _desc << "--------";
			//std::for_each( _vec.begin(), _vec.end(),  std::bind2nd( &Node::logWrite, strm )		);
			for ( Container::const_iterator it = _cont.begin(); it != _cont.end(); it++)
			{
				it->logWrite( log.m_o_logfile );
			}
			log.m_o_logfile << "\n---------------------------";	
			log.m_o_logfile.flush();		
	}
}




#endif
