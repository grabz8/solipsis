#ifndef __P2P_CONF_FILE_H__
#define __P2P_CONF_FILE_H__

#include <string>
#include <vector>

#include "P2PTools.h"

#include <string>
#include <map>
#include <set>



namespace P2P{
/*
 FORMAT ....

	PORT=500
	KLEINBERG_GOSSIP_INTERVAL=2
	VORONOI_GOSSIP_INTERVAL=2
	UPDATE_SIZE=5
	VIEW_SIZE=10
	#This node's position x,y,z
	POSITION=1000.2, 1000.0, 1000.0
	LOG_FILE=MyLogFile.txt
	LOG_LEVEL=[HIGH|LOW]
	
	#peers below.. can be empty set
	Peer=100[,xxx.xxx.xxx.xxx]
	Peer=200
	#etc

*/
	DEFINE_EXCEPTION(ConfFilePathError)
	DEFINE_EXCEPTION(ConfFileDataError)

	const std::string CONF_FILE_NAME("ConfFile.txt");

	template <class Node>
	class ConfFile
	{	
	public:
		typedef std::vector<Node> Nodes;

	private:


		Nodes m_o_Nodes;
	
		unsigned short m_us_protocol_port;	
		unsigned short m_us_raynet_port;
		unsigned int m_i_KleinbergGossipInterval;
		unsigned int m_i_VoronoiGossipInterval;
		unsigned int m_i_UpdateSize;
		unsigned int m_i_viewSize;
		unsigned int m_ui_tcpTimeout;
		std::string m_str_logFile;
		std::string m_str_logLevel;


		typedef std::vector< std::string  >  StringVec;
		StringVec m_arr_AbsentValues;
		typedef typename typename Node::Point Point;
		Point m_o_position;
		
		
		void _processLine( const std::string& _strLine);
		void _load( const std::string& _strPath );
		void _readPeer( const std::string& _data, Nodes& _nodes);
		void _readPosition( const std::string&  );


	public:
		ConfFile( );
		ConfFile( const std::string& _strPath );
		int protocolPort() const				    {	return m_us_protocol_port; }		
		unsigned short raynetPort() const			{   return m_us_raynet_port;} 
		int kleinbergGossipInterval()const			{	return m_i_KleinbergGossipInterval; }
		int voronoiGossipInterval()const			{	return m_i_VoronoiGossipInterval; }
		int updateSize()const					    {	return m_i_UpdateSize;  }
		int viewSize()const							{	return m_i_viewSize; }		
		const Nodes& initView() const	            {	return m_o_Nodes; }
		const std::string& logFile()const			{	return m_str_logFile; }
		const std::string& logLevel() const			{   return m_str_logLevel; }
		const Point& position() const				{   return m_o_position; }		
		unsigned int tcpTimeOut() const				{   return m_ui_tcpTimeout;}

	};
}




// Template implementations
#include <fstream>
#include <strstream>

namespace P2P
{

	// REQUIRE FIELDS IN CONFIGURATION FILE
	const std::string PROTOCOL_PORT("PROTOCOL_PORT");	
	const std::string RAYNET_PORT("RAYNET_PORT");
	const std::string KLEINBERG_GOSSIP_INTERVAL("KLEINBERG_GOSSIP_INTERVAL");
	const std::string VORONOI_GOSSIP_INTERVAL("VORONOI_GOSSIP_INTERVAL");
	const std::string LOG_FILE("LOG_FILE");
	const std::string LOG_LEVEL("LOG_LEVEL");
	const std::string UPDATE_SIZE("UPDATE_SIZE");
	const std::string VIEW_SIZE("VIEW_SIZE");
	const std::string PEER("PEER");
	const std::string POSITION("POSITION");
	const std::string TCP_TIMEOUT("TCP_TIMEOUT");

	const char COMMENT='#';	
	const char PORT_ADDR_SEP=',';

	const std::string COMPULSORY_VALUES[]=
	{  PROTOCOL_PORT, RAYNET_PORT, KLEINBERG_GOSSIP_INTERVAL, 
	  VORONOI_GOSSIP_INTERVAL, UPDATE_SIZE, VIEW_SIZE,POSITION, TCP_TIMEOUT};


	//-------------------------	
	template <class Node>
	inline void ConfFile<Node>::_load( const std::string& _strPath )
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

			// remove all those girly white space characters in line.. leaving only manly stuff
			strLine.erase( 
				std::remove_if( strLine.begin(), strLine.end(), std::ptr_fun( &::iswspace )  )  ,
				strLine.end() );

			// upper case lines			
			Tools::UpperCaser( strLine );


			if (!strLine.empty() && strLine[0] != COMMENT )
			{
				_processLine(strLine);	
			}

		}


		if ( !m_arr_AbsentValues.empty() )
		{
			THROW_WITH_DATA(ConfFileDataError,"Missing atleast compulsory value in confFile:" << m_arr_AbsentValues[0] );			
		}
		if ( m_o_Nodes.size() > m_i_viewSize )
		{
			THROW(ConfFileDataError,"Too many nodes in conf file");			
		}


	}

	//-------------------------
	template <class Node>
	inline ConfFile<Node>::ConfFile(  )
		:m_i_viewSize(0),
		m_us_voronoi_port(0),
		m_us_kleinberg_port(0),
		m_us_raynet_port(0),
		m_i_KleinbergGossipInterval(0),
		m_i_VoronoiGossipInterval(0),
		m_i_UpdateSize(0),
		m_ui_tcpTimeout(0)

	{
		std::string strPath = Tools::getModuleDir();
		strPath += Tools::SEP_SLASH;
		strPath += CONF_FILE_NAME;
		_load( strPath);
	}

	//-------------------------
	template <class Node>
	inline ConfFile<Node>::ConfFile( const std::string& _strPath)
		:m_i_viewSize(0),
		m_us_protocol_port(0),	
		m_us_raynet_port(0),
		m_i_KleinbergGossipInterval(0),
		m_i_VoronoiGossipInterval(0),
		m_i_UpdateSize(0),
		m_ui_tcpTimeout(0)
	{			
		_load( _strPath);
	}
	//-------------------------
	template <class Node>
	inline void ConfFile<Node>::_readPeer( const std::string& _data, Nodes& _nodes)
	{
		_nodes.push_back(  Node::createNode(_data)   );  	

	}

	//-------------------------
	template <class Node>
	inline void ConfFile<Node>::_processLine( const std::string& _strLine)
	{	
		// Get NAME / VALUE from line
		size_t npos = _strLine.find('='); 	
		const std::string strName = _strLine.substr( 0, npos);	
		const std::string strValue = _strLine.substr( npos + 1, _strLine.size() - (npos+1) );

		// Name and Values.. remove name from list of unfound required values	
		m_arr_AbsentValues.erase(  
			std::remove( 
						m_arr_AbsentValues.begin(), 
						m_arr_AbsentValues.end(),
						strName),
			m_arr_AbsentValues.end()
			);


		// Set configuration member variables

		//DESIGN NOTE: This could have been done with a loop over an array of memberVar functors		
		std::stringstream strm( strValue );		
		
		if    ( strName == PROTOCOL_PORT)
				strm >> m_us_protocol_port;
		else if (  strName == RAYNET_PORT )
				strm >> m_us_raynet_port;// = ::atoi(strValue.c_str() );
		else if ( strName == KLEINBERG_GOSSIP_INTERVAL)
				strm >> m_i_KleinbergGossipInterval;// = ::atoi(strValue.c_str() );
		else if ( strName == VORONOI_GOSSIP_INTERVAL)
				strm >> m_i_VoronoiGossipInterval;// = ::atoi(strValue.c_str() );
		else if (  strName == UPDATE_SIZE)
				strm >> m_i_UpdateSize; // = ::atoi(strValue.c_str() );
		else if (  strName == VIEW_SIZE)
				strm >> m_i_viewSize ;//= ::atoi(strValue.c_str() );
		else if (  strName == LOG_FILE)
				strm >> m_str_logFile;// = strValue.c_str() ;
		else if (  strName == LOG_LEVEL)	
				strm >> m_str_logLevel;// = strValue.c_str() ;	
		else if ( strName == PEER )	
				_readPeer(  strValue, m_o_Nodes);	
		else if ( strName == POSITION)
				m_o_position.set( strValue );				
		else if ( strName == TCP_TIMEOUT )
				strm >> m_ui_tcpTimeout;// = ::atoi(strValue.c_str() );
		else{
			THROW_WITH_DATA(ConfFileDataError,"Unrecognised Conf File value:" << strName.c_str() );
		}

	}


	//-------------------------
	template <class Node>
	inline void ConfFile<Node>:: _readPosition( const std::string&  _s)
	{
		std::stringstream strm( _s  );
		/*
		strm >> m_x_pos;
		strm.ignore(1);// ignore separators
		strm >> m_y_pos;
		strm.ignore(1);
		strm >> m_z_pos;	*/
		s >>m_o_position;

	}

	

}




#endif