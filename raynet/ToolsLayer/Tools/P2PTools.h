#ifndef _P2P_TOOLS_FILE_HEADER_
#define _P2P_TOOLS_FILE_HEADER_




#include <string>
#include <sstream>
#include <time.h>





// Annoying warning: No I will not use 
// msoft proprietary iterators
#pragma warning( disable : 4996 )

//------------OS INDEPENDENT HEADERS--------------------------

#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <iterator>

#include "BasicTypedefs.h"

#include <OSDependentTools.h>









namespace Tools
{




	//--------------------EXCEPTIONS-------------------------//
#define THROW_WITH_DATA(_EXC,STRM)\
	{\
	std::stringstream strm;\
	strm << STRM;\
	throw _EXC(__FILE__,__LINE__, strm.str() );\
}

#define THROW(_EXC,_data) throw _EXC(__FILE__,__LINE__, _data);

	const std::string SYSTEM_ERROR("system_error");

	const std::string NETWORK_ERROR("network_error");





#define DEFINE_EXCEPTION(CLASS)\
	class CLASS : public Tools::Exception {\
	public:\
	CLASS( const char* _file, int _line,  const std::string& _reason)\
	: Exception(_file, _line, _reason,  #CLASS ){}\
	};



	class Exception : public std::exception{

		std::string m_str_reason;	

	public:

		Exception(	const char* _file, 
			int _line,
			const std::string& _reason ,
			const std::string& _type 
			)	

		{	
			m_str_reason = "Type:";
			m_str_reason += _type;
			m_str_reason +=( "\n");


			if ( _reason == SYSTEM_ERROR )
			{				
				m_str_reason += systemError();			
			}
			else if ( _reason == NETWORK_ERROR )
			{
				m_str_reason += networkError();
			}
			else
			{
				m_str_reason.reserve( 2 * MAX_PATH);
				m_str_reason += _file;
				m_str_reason +=":";

				char buffer[7]; // what ! a source file with > 999999 lines?? don't give me that
				::_itoa_s( _line, buffer, sizeof(buffer),10 );
				m_str_reason += buffer;
				m_str_reason +=":";
				m_str_reason += _reason;
			}
		}

		virtual ~Exception(){}
		virtual const char * what() const
		{
			return m_str_reason.c_str();		
		};
	};
	//----------------------------------------------------//

















	//-------------USEFUL FUNCTIONS---------------------//


	inline char* UpperCaser( char* _pParam)
	{		
		std::transform( &_pParam[0], &_pParam[::strlen(_pParam) ], &_pParam[0] , std::ptr_fun( &toupper ) );		
		return _pParam;

	}

	//------

	inline std::string& UpperCaser( std::string& _pParam)
	{		
		std::transform( _pParam.begin(), _pParam.end(),  _pParam.begin() , std::ptr_fun( &toupper ));		
		return _pParam;

	}

	//------	

	inline unsigned int rndNumber( unsigned int _ceiling)
	{
		struct Seeder{
			Seeder(){	time_t timer;	::srand( (int) time(&timer) ); 	}
		};
		static Seeder setRndSeed;

		//time_t timer;
		//::srand( (int) time(&timer) ); 	
		const int iVal = rand();
		return iVal == 0 ? iVal : iVal % _ceiling;
	}

	//------
	// DESIGN NOTE: This is used for logging the kleinberg view to file
	template <class NodeType>
	inline std::ostream& operator<< ( std::ostream& _ost, const std::vector<NodeType>& _vec)
	{
		_ost << "\n";
		std::copy( _vec.begin(), _vec.end(), std::ostream_iterator<NodeType>(_ost,"\n")  );		

		return _ost;
	}




	











	//------------ COMPILE-TIME TOOLS ETC-----------

	// STOP copying and assigning certain classes
	class NO_COPY_ASSIGN{
	protected: 
		NO_COPY_ASSIGN(){}
	private:
		NO_COPY_ASSIGN( const NO_COPY_ASSIGN&);
		NO_COPY_ASSIGN& operator= ( const NO_COPY_ASSIGN&);
	};


#   define QU_ELEMS(arr) ( sizeof(arr) / sizeof( arr[0])  )




















	/*  
	DATABLOCK Class

	DESIGN NOTE: Why not use std::vector instead of DataBlock?

	Datablock maps different binary C types to a 
	block of data.
	*/

	DEFINE_EXCEPTION(DataBlockSizeError);
	DEFINE_EXCEPTION(DataBlockSerialisationError);


	template <  int SIZE >
	class DataBlock
	{
		char m_buf[SIZE];
		unsigned int m_i_size;


	public:
		//---------
		DataBlock( const  char* _data, int _size) : m_i_size(_size)
		{
			if  ( SIZE < _size) THROW( DataBlockSizeError," too big");
			::memcpy( m_buf, _data, _size);
		}
		//---------
		DataBlock() : m_i_size(0)
		{
			::memset( m_buf, 0, SIZE );
		}
		/*
		inline void _setQuOfNodes( int _qu)             { m_arr_packet.setAt( QU_NODES_POS,(char)_qu)        ;}
		inline unsigned short _getTotalSize() const{ return ::ntohs( m_arr_packet.getAt<short>(SIZE_POS ) ); }
		*/
		//---------
		template <class T>
		void setAt( unsigned int _pos, const T& _t) 
		{ 
			//update the known length of packet
			m_i_size = ( m_i_size > _pos?  m_i_size : _pos);
			((T&)m_buf[_pos]) = _t; 
		}
		//---------
		template <class T>
		T getAt( unsigned int _pos) const     
		{ 
			return (T&)m_buf[_pos];   
		}
		//---------
		template < class container >
		void writeAllAt (  unsigned int _pos, const container& _objects)
		{									

			/*		std::strstream TEST( m_buf + _pos , SIZE - _pos );
			int MYINT = 0xFFFFEEEE;
			TEST.write( (const char*)&MYINT ,4);
			int ANOTHERINT;
			TEST.read( ( char*)&ANOTHERINT ,4);
			*/		


			std::ostrstream bufferstream ( m_buf + _pos , SIZE - _pos );
			bufferstream.flags( std::ios::binary | bufferstream.flags() );

			for ( container::const_iterator it= _objects.begin(); bufferstream.good() && it != _objects.end(); it++)
			{
				bufferstream << *it;				
			}

			bufferstream << std::ends;

			if ( !bufferstream.good() )
			{
				THROW(DataBlockSerialisationError," strstream object failed to write data");			
			}

			const unsigned int LastByteWritten = _pos + bufferstream.pcount();
			m_i_size = m_i_size  > LastByteWritten ? m_i_size  : LastByteWritten;			

		}



		//---------
		template < class container >
		void readAllAt (  unsigned int _pos, container& _objects, int _quOfObjects) const
		{
			std::istrstream bufferstream ( m_buf + _pos , SIZE - _pos);
			bufferstream.flags( std::ios::binary | bufferstream.flags() );

			while( bufferstream.good() && _quOfObjects > 0 )
			{
				container::value_type t;				
				//_read( bufferstream , t);
				bufferstream >> t;
				_objects.push_back( t);				
				_quOfObjects--;
			}
			if ( ! (bufferstream.good() && _quOfObjects == 0) )
			{
				THROW(DataBlockSerialisationError," strstream object failed to read data");			
			}
		}
		//---------
		const char* getBuffer() const{ return m_buf; }
		//---------
		int size() const{  return m_i_size; }
		//---------
		void forceSize(int _size) {   m_i_size = _size; }
		//---------
		uint maxBufSize() const{  return SIZE; }
		//---------
		char* getWritableBuffer(){ return m_buf; }

		//---------
		friend bool operator==( const DataBlock<SIZE>& _lhs, const DataBlock<SIZE>& _rhs  )
		{
			//	char m_buf[SIZE];
			//unsigned int m_i_size;

			return _lhs.m_i_size == _rhs.m_i_size
				&&
				::memcmp( _lhs.m_buf, _rhs.m_buf, _lhs.m_i_size) == 0;
		}
		//---------	
		bool equal( int _offset, const DataBlock<SIZE>& _rhs) const
		{
			return m_i_size == _rhs.m_i_size
				&&
				::memcmp( m_buf + _offset, _rhs.m_buf + _offset, m_i_size - _offset) == 0;
		}

		//---------
		friend bool operator!=( const DataBlock<SIZE>& _lhs, const DataBlock<SIZE>& _rhs  )
		{
			//	char m_buf[SIZE];
			//unsigned int m_i_size;

			return !(_lhs==_rhs);
		}
	};


	
}
















namespace IP	
{
		using P2P::uchar;
		using P2P::ushort;
		using P2P::uint;

		typedef ushort Port;
		typedef uint   Addr;


		//DESIGN NOTE:
		// Write these functions to avoid making mistakes
		// whether to use htonl OR htons.. let compiler
		// work that out
		static inline uint  ntoh( uint _val)
		{		
			return ::ntohl(_val );
		}		

		/*static inline uint  ntoh( float _val)
		{
		return *(uint*)&_val;
		//return ::ntohl( *(uint*)&_val );//stop rounding off mechanism

		}*/	
		static inline ushort ntoh( ushort _val)
		{
			return ::ntohs(_val );
		}
		static inline uchar ntoh( uchar _val)
		{
			return _val;
		}

		static inline uint hton( uint _val)
		{
			return ::htonl(_val );
		}

		/*	static inline float  hton( float _val)
		{
		return _val;
		//uint uVal = ::htonl( *(uint*)&_val );//stop rounding off mechanism
		//return  *(float* )&uVal;			
		}*/
		static inline ushort hton( ushort _val)
		{
			return ::htons(_val );
		}
		static inline uchar hton( uchar _val)
		{
			return _val ;
		}



		//------------Helper class--------------//

		DEFINE_EXCEPTION(UDPSocketCreationError)
		class SafeSocket
		{
			SOCKET m_handle;
			bool m_bReleased;


		public:
			SafeSocket( SOCKET _handle)
				:m_handle( _handle ), m_bReleased( false)
			{
				if( m_handle == INVALID_SOCKET )
				{	    	
					THROW(UDPSocketCreationError,Tools::NETWORK_ERROR);
				}

			}
			//----------
			~SafeSocket()
			{
				if ( ! m_bReleased)
					::closesocket( m_handle);
			}
			//----------
			SOCKET release()
			{
				m_bReleased = true;
				return m_handle;
			}
			//----------
			operator SOCKET(){ return m_handle;}

			//----------
		};



		DEFINE_EXCEPTION(GetHostAddrError);
		inline Addr getHostAddr()
		{		
			char name[500];
			unsigned int ret = 0;

			int res =::gethostname(  name, sizeof(name) );
			if ( res == 0 )
			{
				struct hostent  *pHost = ::gethostbyname( name );	

				if ( pHost  &&  pHost->h_addr_list &&  *pHost->h_addr_list)
				{
					ret = (**(struct in_addr **)pHost->h_addr_list).S_un.S_addr;
				}
			}

			if ( ret == 0)
			{
				THROW(GetHostAddrError,"Cannot read host addr");
			}
			return ret;
		}




		// IP defines
		static const int MAX_UDP_PACKET_SIZE = USHRT_MAX;




} // end of IP stuff










#endif