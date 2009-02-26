#ifndef __IPNODE_FILE_HEADER_H__
#define __IPNODE_FILE_HEADER_H__

#include <iostream>
#include <limits.h>

#include <set>
#include <vector>

#include <math.h>
#include "P2PTools.h"
#include "Point3D.h"


namespace P2P
{


	class IPNode
	{
	public:
		
		DEFINE_EXCEPTION(IPNodeCreation);
		//Container type
		//std::vector<IPNode> Container;

		typedef Point3D Point;
		typedef Point::Distance Distance;

	private:
		unsigned short m_us_protocol_port;		
		unsigned short m_us_raynet_port;
		unsigned int m_ui_addr;
		mutable unsigned int m_ui_staleness; // smaller the fresher

	
		Point3D m_o_position;

	public:

		enum{ MOST_UP_TO_DATE=0, MOST_OUT_TO_DATE = UINT_MAX};

			IPNode():
			m_us_protocol_port(0), 			
			m_us_raynet_port(0), 
			m_ui_addr(0), 
			m_ui_staleness( MOST_OUT_TO_DATE), //very stale.. data from another node will always be fresher
			m_o_position() // Position NOT set.. but fresher node will overwrite this..
			{}

		IPNode(	unsigned short _us_protocol_port, 			
				unsigned short _raynet_us_port,
				int _i_addr):
			m_us_protocol_port(_us_protocol_port), 			
			m_us_raynet_port(_raynet_us_port),
			m_ui_addr(_i_addr), 
			m_ui_staleness( MOST_OUT_TO_DATE ), //very stale.. data from another node will always be fresher
			m_o_position() // Position NOT set.. but fresher node will overwrite this..
			{}

		IPNode(unsigned short _us_protocol_port, 
				unsigned short _raynet_us_port,
				int _i_addr, 
				const Point3D& _pos ) 
			:
			m_us_protocol_port(_us_protocol_port), 		
			m_us_raynet_port(_raynet_us_port),
			m_ui_addr( _i_addr),
			m_ui_staleness(MOST_UP_TO_DATE),
			m_o_position( _pos )
			
		{}
			
		//static IPNode getThis();
		
		friend std::ostream& operator<< ( std::ostream& _out, const IPNode& _obj);

		friend std::istream& operator>> ( std::istream& _in, IPNode& _obj);

		void logWrite( std::ostream& _out ) const;

		

		friend bool operator ==(const IPNode&, const IPNode& );

		friend bool operator != (const IPNode& _lhs, const IPNode& _rhs)
		{
			return ! ( _lhs == _rhs );
		}

		bool samePeer( const IPNode& _other) const
		{
			//only check addr and protocol port.. they're unique
			return	_other.m_ui_addr == m_ui_addr &&
					_other.m_us_protocol_port ==  m_us_protocol_port;
		}
		
		friend bool operator < (const IPNode& _lhs, const IPNode& _rhs)
		{	
			//only check addr and protocol port.. they're unique
			if (_rhs.m_ui_addr == _lhs.m_ui_addr )
				return _rhs.m_us_protocol_port >  _lhs.m_us_protocol_port;
			else 
				return _rhs.m_ui_addr >  _lhs.m_ui_addr;
			
		}
		

		unsigned short getProtocolPort()const { return m_us_protocol_port;}		
		unsigned short getRaynetPort()const { return m_us_raynet_port;}

		int getAddr() const { return m_ui_addr;}

		bool fresher( const IPNode& _another) const
		{			
			return _another.m_ui_staleness > m_ui_staleness;
		}
		// This is "conceptually" a const method... the outside world is not impacted
		void staler() const
		{
			if ( m_ui_staleness != MOST_OUT_TO_DATE )
				m_ui_staleness++;
		}

		const Point& getPoint() const{  return m_o_position; }
		Point& getPoint() {  return m_o_position; }
		
		Distance getDistance( const IPNode& _peer ) const 
		{ 
			return m_o_position.getDistance( _peer.m_o_position  );
		};


		void setPoint(const Point& _point) 
		{ 
			m_o_position = _point; 
		}
		

		static IPNode createNode( const std::string& _data );
	};







}
#endif
