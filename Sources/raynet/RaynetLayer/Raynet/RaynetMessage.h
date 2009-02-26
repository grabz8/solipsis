#ifndef __TCP_PACKET_FILE_HEADER__
#define __TCP_PACKET_FILE_HEADER__


#include <functional>
#include <iostream>
#include <strstream>
#include <vector>
#include <assert.h>

#include <memory>

#include "P2PTools.h"
#include "refptr.h"
#include "RaynetVersion.h"
#include "Point3D.h"

namespace P2P
{

		
	

	// Make visible hton /ntoh only (not all tools..)
	//using namespace IP;		

	/*
		Protocol :		
		network (4bytes unsigned) version;		
		network (PointSize bytes) Point;
		network (4bytes unsigned) DATA_SIZE .. (excluding itself );
		size amount of bytes

	*/
 





	class RaynetMessage
	{
	

		//-----innerclass----
		class MemoryBlock : public Tools::NO_COPY_ASSIGN
		{
			char* m_p_Data;
			uint m_i_size;
		public:
			MemoryBlock():m_p_Data(0), m_i_size(0){}
			
			MemoryBlock(const char* _p_Data, uint _size ):	m_p_Data(0), m_i_size(_size)
			{
				m_p_Data = new char[_size];
				::memcpy(m_p_Data,_p_Data,  _size);
			}
			~MemoryBlock()
			{
				::delete[] m_p_Data;		
			}

			void setSize( uint _size )
			{
				m_i_size =_size;

				::delete[] m_p_Data;
				m_p_Data = new char[_size];
				::memset( m_p_Data, 0, _size);			
			}

			char* getBuffer()           {   return m_p_Data; } 
			const char* getBuffer()const{   return m_p_Data; } 
		};
		//---------------------





	public:
	
		// TCP header stuff
		struct HEADER{
			enum
			{			
				VERSION_POS = 0,
				POINT_POS = VERSION_POS +  sizeof(RAYNET_VERSION),
				DATA_SIZE_POS = POINT_POS + sizeof(Point3D),
				END = DATA_SIZE_POS +  sizeof( 4)
			};
		};	
		typedef Tools::DataBlock<HEADER::END> Header;

	private:
		
		Header m_o_headerBlock;
		
		//versions
		RaynetVersion _getVersion() const{	return IP::ntoh( m_o_headerBlock.getAt< RaynetVersion > (HEADER::VERSION_POS ) );   }
		uint4         _getDataSize()const{  return IP::ntoh( m_o_headerBlock.getAt< uint4 >         (HEADER::DATA_SIZE_POS ) );   }
		//Warning: Point3D is not OS independent yet.. ran out of time to do it
		Point3D       _getPoint()const   {  return m_o_headerBlock.getAt< Point3D > (                (HEADER::POINT_POS ) );    }

		



		void          _setVersion ( RaynetVersion _v){ m_o_headerBlock.setAt( HEADER::VERSION_POS,    IP::hton( _v ) )   ;}
		void          _setDataSize(uint4 _size      ){ m_o_headerBlock.setAt( HEADER::DATA_SIZE_POS  ,IP::hton( _size ) )   ; }
		//Warning: Point3D is not OS independent yet.. ran out of time to do it
		void          _setPoint( const Point3D& _p) { m_o_headerBlock.setAt( HEADER::DATA_SIZE_POS  ,_p )   ;   }

		//uint _getSize() {  return IP::ntoh( m_o_headerBlock.getAt< RaynetVersion > (HEADER::VERSION_POS ) );  }

		MemoryBlock  m_o_memBlock;
		

	public: 

		static const int HEADER_SIZE = HEADER::END;

		RaynetMessage( const Point3D& _point, const char* _pData, int _size)
			: 
		m_o_headerBlock(),
		m_o_memBlock( _pData, _size)
		{		
			//header
			_setVersion(RAYNET_VERSION);
			_setDataSize(_size);
			_setPoint( _point);					
			
		}

		
		
		RaynetMessage( const Header& _header)
			:
			m_o_headerBlock(_header)
		{			

			m_o_memBlock.setSize( _getDataSize());
		}
	
		Point3D getPoint()const{  return _getPoint( ); }

		//void*		getHeaderBuffer()		{ return m_o_headerBlock.getWritableBuffer();  }
		const char* getHeaderBuffer() const	{ return m_o_headerBlock.getBuffer();  }

		uint getHeaderSize() const
		{
			return HEADER::END;
		}
	

		char*		getDataBuffer()       {	return m_o_memBlock.getBuffer();}
		const char* getDataBuffer() const {	return m_o_memBlock.getBuffer();}

		uint getDataSize()const {	return _getDataSize(); }

		

	};

	typedef std::auto_ptr< RaynetMessage > RaynetMessagePtr;
	


}

#endif