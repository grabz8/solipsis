#include "IPNode.h"
//#include  <Winsock2.h>

#include "P2PTools.h"





namespace P2P{


	//-------------------------------
	std::ostream& operator<< ( std::ostream& _out, const IPNode& _obj)
	{
		using namespace IP;

		typedef IPNode::Point::Coordinate Coordinate;

		unsigned short Pport = hton(_obj.m_us_protocol_port);	
		unsigned short Rport = hton(_obj.m_us_raynet_port);
		unsigned int addr = hton(_obj.m_ui_addr);
		unsigned int staleness = hton(_obj.m_ui_staleness );


		_out.write( (const char*) &Pport, sizeof(Pport) );		
		_out.write( (const char*) &Rport, sizeof(Rport) );
		_out.write( (const char*)&addr  ,sizeof(addr ) );
		_out.write( (const char*)&staleness  ,sizeof(staleness ) );

		_out << _obj.getPoint();		


		//m_x_pos
		return _out;
	}
	//-------------------------------
	std::istream& operator>> ( std::istream& _in, IPNode& _obj)
	{
		using namespace IP;

		//unsigned short port = ::ntohs(_obj.m_us_port);
		//int addr = ntoh(_obj.m_i_addr);
		typedef IPNode::Point::Coordinate Coordinate;
/*
		unsigned short Kport = hton(_obj.m_us_kleinberg_port);
		unsigned short Vport = hton(_obj.m_us_voronoi_port);
		unsigned short Rport = hton(_obj.m_us_raynet_port);
*/

		_in.read( (char*)&_obj.m_us_protocol_port, sizeof(_obj.m_us_protocol_port) ) ;
		_obj.m_us_protocol_port = ntoh(_obj.m_us_protocol_port );

	
		_in.read( (char*)&_obj.m_us_raynet_port, sizeof(_obj.m_us_raynet_port) ) ;
		_obj.m_us_raynet_port = ntoh(_obj.m_us_raynet_port );

		_in.read((char*)&_obj.m_ui_addr,  sizeof(_obj.m_ui_addr) ) ;
		_obj.m_ui_addr = ntoh(_obj.m_ui_addr  );

		_in.read((char*)&_obj.m_ui_staleness,  sizeof(_obj.m_ui_staleness) ) ;
		_obj.m_ui_staleness = ntoh(_obj.m_ui_staleness  );

		//positions
		
		
		_in >> _obj.m_o_position;		


		return _in;
	}
	//-------------------------------
	IPNode::Point::Coordinate Positive( IPNode::Point::Coordinate val)
	{
		if(  val < 0 )
			return val * -1;
		return val;
	}

	//-------------------------------
	
	
	//-------------------------------
	bool operator ==(const IPNode& _lhs, const IPNode& _rhs )
	{
		return	_lhs.m_ui_addr == _rhs.m_ui_addr &&
				_lhs.m_us_protocol_port == _rhs.m_us_protocol_port &&
				_lhs.m_us_raynet_port == _rhs.m_us_raynet_port &&
				_lhs.m_o_position == _rhs.m_o_position;

		//staleness is not a semantic member attribute	
	}
	//-------------------------------
	void IPNode::logWrite(  std::ostream& _out ) const
	{

		_out << "\nPport = " << m_us_protocol_port			
			 <<", Rport = " << m_us_raynet_port
			 <<", address = " <<  ::inet_ntoa( (in_addr&)m_ui_addr);

	}
	//-------------------------------
	IPNode IPNode::createNode( const std::string& _data )
	{
		const char PORT_ADDR_SEP=',';

		//protocol port
		size_t CommaPos1 = _data.find(PORT_ADDR_SEP);
		if ( CommaPos1 == std::string::npos )
		{
			THROW_WITH_DATA(IPNodeCreation, "Node string incorrect, no comma after protocol port:" << _data)
		}
		const std::string strProtocolport = _data.substr( 0, CommaPos1);	




		//raynet port .. there may be NO address after raynet...ie local host 
		size_t CommaPos2 = _data.find(PORT_ADDR_SEP, CommaPos1 + 1);
		const std::string strRport = _data.substr( CommaPos1 + 1, CommaPos2);	



		// address part
		std::string strADDR; 		
		
		if ( CommaPos2 != std::string::npos ) // explicit address
		{
			strADDR = _data.substr( CommaPos2 + 1, _data.size() - (CommaPos2+1) );		
		}
		else //implicit addres
		{
			unsigned int ThisAddress = IP::getHostAddr();
			strADDR = ::inet_ntoa( (in_addr&)ThisAddress );
		}
	

		return  IPNode(
					::atoi(strProtocolport.c_str()) ,					
					::atoi(strRport.c_str()) ,
					::inet_addr(strADDR.c_str()) 
					) ; 
	}
	//-------------------------------
}