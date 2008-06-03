/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __VectorModifier_h__
#define __VectorModifier_h__

#include <Ogre.h>

using namespace Ogre;
using namespace std;

namespace Solipsis {

//-------------------------------------------------------------------------------------
class VectorModifier
{
public:
	VectorModifier() {};
	~VectorModifier() {};

	/// brief calculate the vector normal from a triangular face
	/// file VectorModifier.h
	static Vector3 inline getNormal( Vector3 p1, Vector3 p2, Vector3 p3 )
	{
		Vector3 a = p1-p2;
		Vector3 b = p2-p3;

		return Vector3( (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x) );
	}

	/// brief rotate a vector3 on X from (0,0,0) in degrees
	/// file VectorModifier.h
	static void inline rotateX( Vector3 &point, Real angle )
	{
		Real cosX = Math::Cos(angle);
		Real sinX = Math::Sin(angle);

		//x = point.x;								//		x' = x  
		Real y = point.y * cosX - point.z * sinX;	//		y' = y*cos(a) - z*sin(a)
		point.z = point.y * sinX + point.z * cosX;	//		z' = y*sin(a) + z*cos(a)
		point.y = y;
	}

	/// brief rotate a vector3 on Y from (0,0,0) in degrees
	/// file VectorModifier.h
	static void inline rotateY( Vector3 &point, Real angle )
	{
		Real cosY = Math::Cos(angle);
		Real sinY = Math::Sin(angle);

		Real x = point.x * cosY + point.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
		//y = point.y;								//		y' = y  
		point.z = -point.x * sinY + point.z * cosY;		//		z' = -x*sin(a) + z*cos(a)
		point.x = x;
	}

	/// brief rotate a vector3 on Z from (0,0,0) in degrees
	/// file VectorModifier.h
	static void inline rotateZ( Vector3 &point, Real angle )
	{
		Real cosZ = Math::Cos(angle);
		Real sinZ = Math::Sin(angle);

		Real x = point.x * cosZ + point.y * sinZ;	//		x' = x*cos(a) + y*sin(a)  
		point.y = -point.x * sinZ + point.y * cosZ;	//		y' = -x*sin(a) + y*cos(a)  
		//z = point.z;								//		z' = z
		point.x = x;
	}

	/// brief rotate a vector3 on X,Y & Z from (0,0,0) in degrees
	/// file VectorModifier.h
	static void inline rotateXYZ( Vector3 &point, Real angleX, Real angleY, Real angleZ )
	{
		Real cosX = Math::Cos(angleX);
		Real sinX = Math::Sin(angleX);
		Real cosY = Math::Cos(angleY);
		Real siny = Math::Sin(angleY);
		Real cosZ = Math::Cos(angleZ);
		Real sinZ = Math::Sin(angleZ);

		Real x = point.x * (cosZ * cosY) + point.y *
			(sinZ * cosX - cosZ * siny * sinX) + point.z *
			(sinZ * sinX + cosZ * siny * cosX);
		Real y = point.x * (-sinZ * cosY) + point.y *
			(cosZ * cosX + sinZ * siny * sinX) + point.z *
			(cosZ * sinX + sinZ * siny * cosX);
		point.z = point.x * (-siny) + point.y * 
			(-cosY * sinX) + point.z * (cosY * cosX);
		point.x = x;
		point.y = y;
	}

	/// brief remove the occurences from a vector list of int
	static bool addToList( list<int> &list, int value)
	{
		std::list<int>::iterator v = list.begin();

		while( (*v) != value && v != list.end() ) v++;
		if( (*v) != value ) 
		{
			list.push_back( value );
			return true;
		}
		return false;
	}

	/// brief remove the occurences from a vector list of Vector3
	static bool addToList( list<Vector3> &list, Vector3 vec)
	{
		std::list<Vector3>::iterator v = list.begin();

		while( (*v) != vec && v != list.end() ) v++;
		if( (*v) != vec ) 
		{
			list.push_back( vec );
			return true;
		}
		return false;
	}

	/// brief remove the occurences from a vector list of pair<Vector3, int>
	static bool addToList( list< pair<unsigned int, Vector3> > &list, pair<unsigned int, Vector3> pr)
	{
		std::list< pair<unsigned int, Vector3> >::iterator p = list.begin();
		
		while( (*p) != pr && p != list.end() ) p++;
		if( (*p) != pr ) 
		{
			list.push_back( pr );
			return true;
		}
		return false;
	}

	/// brief get the nearest point from another contained in a list
	static Vector3 getNearestPoint( list< Vector3 > lst, Vector3 vec )
	{
		Real distance, temp = 0.;
		Vector3 nearest = (*lst.begin());
		std::list< Vector3 >::iterator v = lst.begin();
		distance = (*v).squaredDistance( vec );

		for( v++; v != lst.end(); v++ )
		{
//			temp = (*v).distance( vec );		// This operation is expensive in terms of CPU operations
			temp = (*v).squaredDistance( vec );
			if( temp < distance )
			{
				distance = temp;
				nearest = (*v);
			}
		}

		return nearest;
	}
};

} //namespace

#endif //__VectorModifier_h__