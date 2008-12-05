#include "LogFile.h"
#include "Thread.h"




namespace P2P
{	


	// init class members..
		std::string LogFile::s_str_file;
		volatile	LogFile::LogLevel LogFile::s_e_level = LogFile::NONE;	

		
		// Safe macro locking
		Tools::Mutex m_class_mutex;
		
		LogFile::MacroLocker::MacroLocker()
		{
			m_class_mutex.lock();
		}
		LogFile::MacroLocker::~MacroLocker()
		{
			m_class_mutex.unlock();
		}
		



	//--------------------------
	LogFile::LogFile( const std::string& _file, LogLevel s_e_level)		
	
	{
		std::string path = Tools::getModuleDir();
		path += Tools::SEP_SLASH;
		path += _file;

		m_o_logfile.open( path.c_str()  );
		if (  ! m_o_logfile.good() )
		{
			THROW(LogFileFailed,"failed to open log file");
		}
	}

	//--------------------------
	void LogFile::printTime()
	{
		// time string ( without newline at end thank you!!)
		time_t time;
		::time(&time);
		char * pTimeNoNewLine = ::ctime( &time );
		char *pNewline = ::strchr(pTimeNoNewLine,'\n') ; 
		if ( pNewline ) 
			*pNewline =' ';
		const int ENOUGH_SPACE_FOR_TIME = 80; //roughly		

		m_o_logfile << "\n" << pTimeNoNewLine <<  ":";
	}	
	//--------------------------

	LogFile& LogFile::getInstance()
	{
		if( !s_str_file.empty() )
		{
			static LogFile s_logFile(s_str_file, s_e_level);				
			return s_logFile;			
		}
		else
		{
			THROW(LogFileFailed,"NoPathForLogging!!")
		}
	}
	//--------------------------
	std::ostream& 	LogFile::getLogStream()
	{
		//LOCK(g_class_mutex)
		LogFile& log = getInstance();		
		log.printTime();		
		
		return log.m_o_logfile;					
	}

	//--------------------------

	void LogFile::init( const std::string& _file,  const std::string& _strlevel)
	{
		//default to low logging for logfile
		std::string strlevel = (_strlevel.empty() && !_file.empty()? std::string("LOW"): _strlevel);
	
		if ( strlevel== "LOW")
		{
			if ( _file.empty() )
			{
				THROW(LogInitException,"No log filename for LOW logging");
			}
			s_str_file = _file;
			s_e_level = LOW;
		}
		else if ( strlevel== "HIGH")
		{
			if ( _file.empty() )
			{
				THROW(LogInitException,"No log filename for HIGH logging");
			}
			s_str_file = _file;
			s_e_level = HIGH;
		}
		else
		{
			s_e_level = NONE;
		}
	}


	

	

}

