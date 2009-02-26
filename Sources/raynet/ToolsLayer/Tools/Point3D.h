#ifndef __3D_POINTER___HEADER_H__
#define __3D_POINTER___HEADER_H__


#include "P2PTools.h"
#include <iostream>
#include <math.h>

namespace P2P
{
	class Point3D
	{

	public: // typedefs
		typedef float Coordinate;
		typedef int Distance;
		static const unsigned int MAX_DISTANCE = INT_MAX;


	private://members
		Coordinate m_x_pos, m_y_pos, m_z_pos;

		

	public: // methods
		
		Point3D() : m_x_pos(0), m_y_pos(0), m_z_pos(0){}
		Point3D( Coordinate _x_pos, Coordinate _y_pos, Coordinate _z_pos) 
			: m_x_pos(_x_pos), m_y_pos(_y_pos), m_z_pos(_z_pos){}
		
		Coordinate getXpos() const{ return m_x_pos;} 
		Coordinate getYpos() const{ return m_y_pos;} 
		Coordinate getZpos() const{ return m_z_pos;} 

		void setXpos( Coordinate _v) {  m_x_pos = _v;}
		void setYpos( Coordinate _v) {  m_y_pos = _v;}
		void setZpos( Coordinate _v) {  m_z_pos = _v;}

		

		friend std::ostream& operator<< (  std::ostream& _out, const Point3D& _position)
		{		
			const char SEPARATOR=',';
			_out.write( (const char*)&_position.m_x_pos  ,sizeof(Coordinate ) );
			_out.write( &SEPARATOR, sizeof( SEPARATOR ) );
			_out.write( (const char*)&_position.m_y_pos  ,sizeof(Coordinate ) );
			_out.write( &SEPARATOR, sizeof( SEPARATOR ) );
			_out.write( (const char*)&_position.m_z_pos  ,sizeof(Coordinate ) );
			return _out;
		}

		friend std::istream& operator>> (  std::istream& _in,  Point3D& _position)
		{
			int SEPARATOR= 1;
			_in.read((char*)&_position.m_x_pos,  sizeof(Coordinate) ) ;
			//_obj.m_o_position.m_x_pos = ntoh( _obj.m_o_position.m_x_pos  );

			_in.ignore(SEPARATOR);
			_in.read((char*)&_position.m_y_pos,  sizeof(Coordinate) ) ;
			//_obj.m_o_position.m_y_pos = ntoh( _obj.m_o_position.m_y_pos  );

			_in.ignore(SEPARATOR);
			_in.read((char*)&_position.m_z_pos,  sizeof(Coordinate) ) ;
			//_obj.m_o_position.m_z_pos = ntoh( _obj.m_o_position.m_z_pos  );
			return _in;
		}




		friend bool operator== (   const Point3D& _l, const Point3D& _r)
		{
			return	_l.m_x_pos == _r.m_x_pos &&
					_l.m_y_pos == _r.m_y_pos &&
					_l.m_z_pos == _r.m_z_pos ;
		}


		Distance getDistance(const Point3D& _other) const
		{
			return ( Distance )
				sqrt( 
				pow (  m_x_pos - _other.m_x_pos , 2)+
				pow (  m_y_pos - _other.m_y_pos , 2)+
				pow (  m_z_pos - _other.m_z_pos , 2)
				);
		}

	
		void set ( const std::string& _humanReadable)
		{		
			std::stringstream strm( _humanReadable);
			strm >> m_x_pos;
			strm.ignore(1);// ignore separators
			strm >> m_y_pos;
			strm.ignore(1);
			strm >> m_z_pos;
		}

	};


}
#endif