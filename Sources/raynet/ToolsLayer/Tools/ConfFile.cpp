#include "ConfFile.h"

#include <fstream>
#include <strstream>

/*
FILE FORMAT
-----------







*/
/*
namespace P2P
{

	//DEFINE_EXCEPTION(ConfFilePathError)
	//DEFINE_EXCEPTION(ConfFileDataError)
	const std::string PORT("PORT");
	const std::string GOSSIP("GOSSIP");
	const std::string LOG_FILE("LOG_FILE");
	const std::string LOG_LEVEL("LOG_LEVEL");
	const std::string UPDATE_SIZE("UPDATE_SIZE");
	const std::string VIEW_SIZE("VIEW_SIZE");
	const std::string PEER("PEER");
	const std::string POSITION("POSITION");

	const char COMMENT='#';
	const char SPACE=' ';
	const char PORT_ADDR_SEP=',';

	const std::string COMPULSORY_VALUES[]={ PORT, GOSSIP, UPDATE_SIZE, VIEW_SIZE,POSITION};


	//-------------------------	
	template <class Node>
	void ConfFile<Node>::_load( const std::string& _strPath )
	{

		// add to vector required values
		m_arr_AbsentValues.assign( COMPULSORY_VALUES, &COMPULSORY_VALUES[ QU_ELEMS(COMPULSORY_VALUES)]  );

		std::ifstream o_confFile( _strPath.c_str() );
		if ( ! o_confFile.good() )
		{
			THROW(ConfFilePathError,"path error");
		}

		std::string strLine;

		while ( o_confFile.good() )
		{	
			strLine.clear();
			std::getline( o_confFile, strLine);
			strLine.erase( 
				std::remove( strLine.begin(), strLine.end(), SPACE ),
				strLine.end() );
			if (!strLine.empty() && strLine[0] != COMMENT )
				_processLine(strLine);	

		}


		if ( !m_arr_AbsentValues.empty() )
		{
			THROW(ConfFileDataError,"Missing a compulsory value in confFile");			
		}
		if ( m_o_Nodes.size() > m_i_viewSize )
		{
			THROW(ConfFileDataError,"Too many nodes in conf file");			
		}


	}

	//-------------------------
	ConfFile::ConfFile(  )
		:m_i_viewSize(0),
		m_us_port(0),
		m_i_GossipInterval(0),
		m_i_UpdateSize(0)

	{
		std::string strPath = Tools::getModuleDir();
		strPath += Tools::SEP_SLASH;
		strPath += CONF_FILE_NAME;
		_load( strPath);
	}

	//-------------------------
	ConfFile::ConfFile( const std::string& _strPath)
		:m_i_viewSize(0),
		m_us_port(0),
		m_i_GossipInterval(0),
		m_i_UpdateSize(0)
	{	
		_load( _strPath);
	}
	//-------------------------
	void ConfFile::_readPeer( const std::string& _data, std::vector<IPNode>& _nodes)
	{
		size_t CommaPos = _data.find(PORT_ADDR_SEP);
		const std::string strPORT = _data.substr( 0, CommaPos);	
		std::string strADDR = _data.substr( CommaPos + 1, _data.size() - (CommaPos+1) );
		// If not address specified.. assume Local address...
		if ( CommaPos == std::string::npos )
		{
			unsigned int ThisAddress = Tools::getHostAddr();
			strADDR = ::inet_ntoa( (in_addr&)ThisAddress );
		}
		
		_nodes.push_back( IPNode(::atoi(strPORT.c_str()),::inet_addr(strADDR.c_str()) )  );  		


	}

	//-------------------------
	void ConfFile::_processLine( const std::string& _strLine)
	{	
		size_t npos = _strLine.find('='); 	
		std::string strName = _strLine.substr( 0, npos);
		Tools::UpperCaser( strName );
		std::string strValue = _strLine.substr( npos + 1, _strLine.size() - (npos+1) );

		// Name and Values.. remove name from list of unfound required values	
		m_arr_AbsentValues.erase(  
			std::remove( m_arr_AbsentValues.begin(), m_arr_AbsentValues.end(), strName),
			m_arr_AbsentValues.end()
			);


		if    ( strName == PORT)
			m_us_port = ::atoi(strValue.c_str() );
		else if  ( strName == GOSSIP)
			m_i_GossipInterval = ::atoi(strValue.c_str() );
		else if (  strName == UPDATE_SIZE)
			m_i_UpdateSize = ::atoi(strValue.c_str() );
		else if (  strName == VIEW_SIZE)
			m_i_viewSize = ::atoi(strValue.c_str() );
		else if (  strName == LOG_FILE)
			m_str_logFile = strValue.c_str() ;
		else if (  strName == LOG_LEVEL)	
			m_str_logLevel = strValue.c_str() ;	
		else if ( strName == PEER )	
			_readPeer(  strValue, m_o_Nodes);	
		else if ( strName == POSITION)
			_readPosition( strValue );
		else{
			THROW(ConfFileDataError,"Unrecognised Conf File value");
		}




	}
	//-------------------------
	void ConfFile::_readPosition( const std::string& _strLine)
	{
		std::stringstream strm( _strLine  );

		strm >> m_o_position.m_x_pos;
		strm.ignore(1);// ignore separators
		strm >> m_o_position.m_y_pos;
		strm.ignore(1);
		strm >> m_o_position.m_z_pos;

	}


}*/