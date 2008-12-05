#ifndef __UDP_PACKET_FILE_HEADER__
#define __UDP_PACKET_FILE_HEADER__


#include <functional>
#include <iostream>
#include <strstream>
#include <vector>
#include <assert.h>
#include <limits.h>

#include "P2PTools.h"

#include "RaynetVersion.h"

namespace P2P{

	

	
		


	class UDPPacket
	{

	public: 
		enum Type { e_KLEINBERG_MESSAGE, e_KLEINBERG_RESPONSE, e_VORONOI_MESSAGE, e_VORONOI_RESPONSE};
	// buffer stuff

		

	enum { MAX_PACKET_SIZE = USHRT_MAX};

	private:
	
		// header stuff (scope with struct)
		/*
		SIZE : 2 bytes
		FROM_PORT : 2 bytes
		VERSION : 2 bytes
		MESSAGE_TYPE ( MESS,RESP):1 byte		
		QU_OF_NODES : 1 bytes
			[ QU_OF_NODES * sizeof(NODE) data]

		BYTES IN NETWORK ORDER
		*/
	
	typedef unsigned short ushort;
	typedef unsigned int   uint;
	typedef unsigned char  uchar;

		struct HEADER{
			enum{						
					SIZE_POS   =  0	,				
					FROM_PORT_POS = SIZE_POS + sizeof(ushort),  
					VERSION_POS=  FROM_PORT_POS + sizeof(ushort), 
					MESS_POS   = VERSION_POS + sizeof(ushort), 					
					QU_NODES_POS= MESS_POS + sizeof(uchar),
					END = QU_NODES_POS + sizeof(uchar) 
			};
		};

	
	

		//unsigned char m_o_datablock[MAX_SIZE ];
		//int m_i_bufSize;
		Tools::DataBlock< MAX_PACKET_SIZE > m_o_datablock;

		int m_i_RemoteAddr;	
		unsigned short m_us_RemotePort;


		
		

		inline void _setTotalSize( ushort _size)  { m_o_datablock.setAt( HEADER::SIZE_POS  ,    IP::hton( _size ) );}
		inline void _setVersion( ushort _v)	      { m_o_datablock.setAt( HEADER::VERSION_POS,   IP::hton( _v ) )   ;}
		inline void _setType( uchar _t)			  { m_o_datablock.setAt( HEADER::MESS_POS,      IP::hton(_t))    ;}		
		inline void _setCallBackPort(ushort _port){ m_o_datablock.setAt( HEADER::FROM_PORT_POS, IP::hton( _port )) ;}      
		inline void _setQuOfNodes( uchar _qu)     { m_o_datablock.setAt( HEADER::QU_NODES_POS,  IP::hton(_qu) )       ;}


		inline ushort _getTotalSize()   const{ return IP::ntoh( m_o_datablock.getAt<ushort>(HEADER::SIZE_POS ) ); }
		inline ushort _getVersion( )    const{ return IP::ntoh( m_o_datablock.getAt<ushort>(HEADER::VERSION_POS) ) ; }
		inline uchar  _getType( )       const{ return IP::ntoh( m_o_datablock.getAt<uchar>(HEADER::MESS_POS )); }		
		inline ushort _getCallBackPort()const{ return IP::ntoh( m_o_datablock.getAt<ushort>( HEADER::FROM_PORT_POS)) ;}      
		inline uchar  _getQuOfNodes( )  const{ return IP::ntoh( m_o_datablock.getAt<uchar>( HEADER::QU_NODES_POS )) ; }
		

		


	public:

	
		UDPPacket(  unsigned short _localPort, //host byte order
					unsigned int _RemoteAddr,  //network byte order					
					const  char* _data, 
					int _size) :  			
			m_o_datablock(_data, _size),	  
			m_i_RemoteAddr(_RemoteAddr),
			m_us_RemotePort(  _getCallBackPort() )
		{	
			if ( _getVersion() !=  RAYNET_VERSION)
			{
				THROW(ProtocolVersionException,"Incorrect network protocol version");
			}			
			_setCallBackPort( _localPort );

			//ensure value is set
			assert( _getCallBackPort( ) == _localPort);

			
		}		

		UDPPacket(unsigned short _remotePort,
					unsigned int _RemoteAddr,
					unsigned short _callbackPort,
					Type _type) :  
			m_o_datablock(),
			m_i_RemoteAddr(_RemoteAddr),
			m_us_RemotePort( _remotePort)
		{				
			_setType( _type);
			_setVersion( RAYNET_VERSION) ;
			_setQuOfNodes( 0);
			_setTotalSize( HEADER::END); // just the header only
			_setCallBackPort(  _callbackPort  );
		}		

		unsigned short getCallBackPort()const{  return _getCallBackPort(); }

		const  char* getBuffer()const { return m_o_datablock.getBuffer() ;}
		
		int getRemoteAddr()const { return m_i_RemoteAddr;}
		
		unsigned short getRemotePort() const          { return m_us_RemotePort;}

		int size() const{ return m_o_datablock.size();}

		Type getType() const { return (Type) _getType( );  }


		template <class container >
		void setObjects( const container& _objects)
		{

			m_o_datablock.writeAllAt( HEADER::END, _objects);

			_setQuOfNodes( _objects.size() );
			_setTotalSize( m_o_datablock.size() ); // just the header only

		}

		template <class container >
		void getObjects(  container& _objects ) const
		{
			_objects.clear();
			m_o_datablock.readAllAt( HEADER::END, _objects, _getQuOfNodes() );
		}


		bool sameData(  const UDPPacket& _rhs ) const
		{
			//NOTE:  the FROM_PORT is not considered "DATA".. 
			//		 .. it is logically the same as the remote port and remote addr
			//       .. but stored as a binary field so that the remote machine CAN
			//       ..use it to send packages back....so we ignore the port in the comparison
			return (
				// compare the sizes of both packets
					m_o_datablock.getAt<short>(HEADER::SIZE_POS )
						==
					_rhs.m_o_datablock.getAt<short>(HEADER::SIZE_POS )
				   )
				   &&
				   // compare the binary data AFTER the ports..<version> onwards
				   m_o_datablock.equal( HEADER::VERSION_POS, _rhs.m_o_datablock);
			
		}
		friend bool operator == ( const UDPPacket& _lhs ,const UDPPacket& _rhs ) 
		{
			return _lhs.m_i_RemoteAddr == _rhs.m_i_RemoteAddr &&	
				  	_lhs.m_us_RemotePort == _rhs.m_us_RemotePort &&
					_lhs.sameData( _rhs );// check size position and onwards..
		}
		


		friend bool operator != ( const UDPPacket& _lhs ,const UDPPacket& _rhs ) 
		{
			return !(_lhs ==_rhs);
		}

		friend std::ostream& operator << (  std::ostream& _os, const UDPPacket& _p)
		{
			switch( _p.getType() )
			{
				case e_KLEINBERG_MESSAGE:
				_os << "KLEINBERG MESSAGE\n";
				break;
				case e_KLEINBERG_RESPONSE:
				_os << "KLEINBERG RESPONSE\n";
				break;				
				case e_VORONOI_MESSAGE:
				_os << "VORONOI MESSAGE\n";
				break;
				case e_VORONOI_RESPONSE:
				_os << "VORONOI RESPONSE\n";
				break;
			};

			_os << "packet size:" << _p.size() << "\n"
				<<" packet data:";
			std::ios_base::fmtflags oldFlags = _os.flags();

			_os.write( (const char*) _p.getBuffer(), _p.size() );	
			return _os;
		}
	};


}

#endif