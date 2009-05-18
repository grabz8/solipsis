/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author HUIBAN Vincent

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

#ifndef _GEOM_BUILDER_H_
#define _GEOM_BUILDER_H_

#include <iostream>
#include <map>
#include <vector>

#include "ogrsf_frmts.h"

#include <Linear/Linear.h>

namespace pm_trax {

///**
// * Definition of a symbol geometric representation
// * @author		Vincent Huiban
// */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Geom {
		
public:
		
	Geom( int featureId, const Vec3d& pos = Vec3d( 0.0, 0.0, 0.0 ), double length = 0.0, double width = 0.0, double heading = 0.0, double pitch = 0.0 )
		: m_featureId( featureId ), m_position( pos ), m_length( length ), m_width( width ), m_heading( heading ), m_pitch( pitch ) {}

	Geom( const Geom& right ) { *this = right; };
	  
	const Geom& operator=( const Geom& right ) {
		m_featureId = right.m_featureId;
		m_position = right.m_position; 
		m_length = right.m_length; 
		m_width = right.m_width; 
		m_heading = right.m_heading; 
		m_pitch = right.m_pitch; 
		return( *this ); 
	};
	
	void setFeatureId( int featureId ) { m_featureId = featureId; };
	int featureId() { return m_featureId; };

	void setPosition( const Vec3d& pos ) { m_position = pos; };
	Vec3d position() { return m_position; };

	void updatePosition() { 
		m_position += Vec3d( m_length*cos( m_heading )*cos( m_pitch ),
						 	 m_length*sin( m_heading )*cos( m_pitch ),
						     m_length*sin( m_pitch ) ); 
	};

	Vec3d previousPosition() { return Vec3d( m_position - Vec3d( m_length*cos( m_heading )*cos( m_pitch ), m_length*sin( m_heading )*cos( m_pitch ), m_length*sin( m_pitch ) ) ); }

	Vec3d direction() { 
		Vec3d dir( m_length*cos( m_heading )*cos( m_pitch ), m_length*sin( m_heading )*cos( m_pitch ), m_length*sin( m_pitch ) );
		dir.normalize();
		return dir;
	}

	void setLength( double l ) { m_length = l; };
	double length() { return m_length; };
	
	void setWidth( double w ) { m_width = w; };
	double width() { return m_width; }; 
	
	void setHeading( double h ) { m_heading = h; };
	double heading() { return m_heading; };

	void setPitch( double p ) { m_pitch = p; };
	double pitch() { return m_pitch; };
	
protected:
		
	int m_featureId;
	Vec3d m_position;
	double m_length;
	double m_width;
	double m_heading;
	double m_pitch;
	
}; // Geom

/**
 * Definition of a Geom container
 * @author		Vincent Huiban
 */

class GeomBuilder 
{
	
public:
	
	typedef struct {
		std::vector< Geom * > m_geoms;
		double m_weight;
		std::string m_name;
		std::string m_nature;


		OGRLineString* toLineString() {
			unsigned int i;
			OGRLineString* ls = new OGRLineString;
			for( i = 0; i < m_geoms.size(); i++ ) {
				Vec3d pos = m_geoms[i]->position();
				ls->addPoint( pos[0], pos[1], pos[2] );
			}
			return ls;
		}

	} Feature;
			
	/** Constructor. */
	GeomBuilder(); 
	
	/** Destructor. */
	virtual ~GeomBuilder() {};

	static GeomBuilder* instance();
	
	std::vector< Feature* > features() { return m_features; };

	std::vector< OGRLineString* > lineStrings();

	int addFeature( double weight );

	Feature* feature( int featureId ) { return m_features[featureId]; };

	void setCurrentFeatureId( int id ) { m_currentFeatureId = id; };

	int currentFeatureId() { return m_currentFeatureId; };
	
	void setCurrentFeature( Feature* f );
	
	Feature* currentFeature() { return m_features[m_currentFeatureId]; };

	void addGeomToFeatureById( int featureId, Geom* g );

	void insertGeomInFeatureById( int featureId, int pointId, Geom* g );

	//void updateGeomInFeatureById( int featureId, int pointId, Geom* g );

	void updateLastGeomInFeatureById( int featureId, Geom* g );
	
	Geom* lastGeomInFeature( int featureId );

	Vec3d lastPositionInFeature( int featureId );

	Vec3d lastDirectionInFeature( int featureId );
	
	Geom* geomInFeature( int featureId, int pointId );

	Vec3d positionInFeature( int featureId, int pointId );

	int numGeomsInFeature( int featureId );
		
	void write( const std::string& name );

	std::string geometriesAsText();

protected:

	static GeomBuilder* m_theGeomBuilder;
	
	std::vector< Feature* > m_features;

	std::map< int, int > m_feature2peak;
			
	std::map< int, float > m_feature2value;

	std::map< int, OGRLinearRing* > m_feature2ring;

	std::map< int, int > m_feature2id;
	
	int m_currentFeatureId;

	OGRFeatureDefn* m_featureDefn;
	
}; // GeomBuilder

}; // pm_trax

#endif // _GEOM_BUILDER_H_
