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

#include "utils/Utils.h"

#include "Config.h"
#include "GeomBuilder.h"


pm_trax::GeomBuilder* pm_trax::GeomBuilder::m_theGeomBuilder = NULL;

pm_trax::GeomBuilder* pm_trax::GeomBuilder::instance()
{
  if( NULL == m_theGeomBuilder )
    m_theGeomBuilder = new pm_trax::GeomBuilder;
  return m_theGeomBuilder;
}

pm_trax::GeomBuilder::GeomBuilder()
{
    OGRFieldDefn idField( "ID", OFTInteger );
    OGRFieldDefn weightField( "WEIGHT", OFTReal );

    m_featureDefn = new OGRFeatureDefn( NULL );
    m_featureDefn->AddFieldDefn( &idField );
    m_featureDefn->AddFieldDefn( &weightField );
}

std::vector< OGRLineString* > pm_trax::GeomBuilder::lineStrings()
{
	std::vector< OGRLineString* > linestrings;
	for( unsigned int i = 0; i < m_features.size(); i++ )
		if( m_features[i]->m_geoms.size() > 1 ) // more than 2 points in a line string
			linestrings.push_back( m_features[i]->toLineString() );
	return linestrings;
}

int pm_trax::GeomBuilder::addFeature( double weight )
{
	Feature* feature = new Feature;
	feature->m_weight = weight;
	m_features.push_back( feature );		
	return m_features.size()-1;
}

void pm_trax::GeomBuilder::setCurrentFeature( pm_trax::GeomBuilder::Feature* f )
{ 
	bool found = false;
	int i = 0;
	while( !found && i < m_features.size() ) {
		if( f == m_features[i] )
			found = true;
		i++;
	}
	if( found )
		m_currentFeatureId = i-1;
	else std::cerr << "Could not find the feature in the set." << std::endl;
};

void pm_trax::GeomBuilder::addGeomToFeatureById( int id, pm_trax::Geom* g )
{
	if( id < m_features.size() ) {
		g->setFeatureId( id );	
		m_features[id]->m_geoms.push_back( g );		
	}
	else {
		pm_utils::notify( pm_utils::notifyLevel() ) << "GeomBuilder::addGeomToFeatureById -> could not add geom" << std::endl;
		exit( 1 );
	}
}

void pm_trax::GeomBuilder::updateLastGeomInFeatureById( int featureId, pm_trax::Geom* g )
{
	if( featureId < m_features.size() && !m_features[featureId]->m_geoms.empty() ) {		
		pm_trax::Geom *lg = m_features[featureId]->m_geoms[m_features[featureId]->m_geoms.size()-1];
		lg->setPosition( g->position() );
		lg->setLength( g->length() );
		lg->setHeading( g->heading() );
		lg->setPitch( g->pitch() );
	}			
	else {
		pm_utils::notify( pm_utils::notifyLevel() ) << "GeomBuilder::updateLastGeomInFeatureById -> could not update geom" << std::endl;
		exit( 1 );
	}
}

void pm_trax::GeomBuilder::insertGeomInFeatureById( int featureId, int pointId, pm_trax::Geom* g )
{
	if( featureId < m_features.size() && pointId < m_features[featureId]->m_geoms.size() ) {
		m_features[featureId]->m_geoms.insert( m_features[featureId]->m_geoms.begin()+pointId, g );
	}			
}

//void pm_trax::GeomBuilder::updateGeomInFeatureById( int featureId, int pointId, pm_trax::Geom* g )
//{
//	if( featureId < m_features.size() && pointId < m_features[featureId]->m_geoms.size() ) {
//		pm_trax::Geom *geom = m_features[featureId]->m_geoms[pointId];
//		geom->setPosition( g->position() );
//		geom->setLength( g->length() );
//		geom->setHeading( g->heading() );
//		geom->setPitch( g->pitch() );
//	}			
//	else {
//		pm_utils::notify( pm_utils::notifyLevel() ) << "GeomBuilder::updateGeomInFeatureById -> could not update geom" << std::endl;
//		exit( 1 );
//	}
//}

pm_trax::Geom* pm_trax::GeomBuilder::lastGeomInFeature( int featureId )
{
	if( featureId < m_features.size() && !m_features[featureId]->m_geoms.empty() )
		return( m_features[featureId]->m_geoms[m_features[featureId]->m_geoms.size()-1] );
	return NULL;
}

Vec3d pm_trax::GeomBuilder::lastDirectionInFeature( int featureId ) 
{
	assert( featureId >= 0 && featureId < m_features.size() && m_features[featureId]->m_geoms.size() > 1 );
	return lastPositionInFeature( featureId )-positionInFeature( featureId, m_features[featureId]->m_geoms.size()-2 ); 
}

Vec3d pm_trax::GeomBuilder::lastPositionInFeature( int featureId )
{
	return lastGeomInFeature( featureId )->position();
}

pm_trax::Geom* pm_trax::GeomBuilder::geomInFeature( int featureId, int pointId )
{
	if( featureId >= 0 )
		if( featureId < m_features.size() )
			if( pointId >= 0 )
				if( pointId < m_features[featureId]->m_geoms.size() )
					return( m_features[featureId]->m_geoms[pointId] );
	return NULL;
}

Vec3d pm_trax::GeomBuilder::positionInFeature( int featureId, int pointId )
{
	return geomInFeature( featureId, pointId )->position();
}

int pm_trax::GeomBuilder::numGeomsInFeature( int featureId ) 
{ 
	if( featureId < m_features.size() ) {
		return m_features[featureId]->m_geoms.size();
	}
	return 0; 
};

void pm_trax::GeomBuilder::write( const std::string& name ) 
{

    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver;
    
    OGRRegisterAll();

    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
                pszDriverName );
    if( poDriver == NULL )
    {
        printf( "%s driver not available.\n", pszDriverName );
        exit( 1 );
    }

    OGRDataSource *poDS = poDriver->CreateDataSource( name.c_str(), NULL );
    if( poDS == NULL )
    {
        printf( "Creation of output file failed.\n" );
        exit( 1 );
    }
;
    OGRLayer *poLayer;

    poLayer = poDS->CreateLayer( name.c_str(), NULL, wkbLineString25D, NULL );
    if( poLayer == NULL )
    {
        printf( "Layer creation failed.\n" );
        exit( 1 );
    }

    OGRFieldDefn idField( "ID", OFTInteger );
    if( poLayer->CreateField( &idField ) != OGRERR_NONE )
    {
        printf( "Creating 'ID' field failed.\n" );
        exit( 1 );
    }
    OGRFieldDefn weightField( "WEIGHT", OFTReal );
    if( poLayer->CreateField( &weightField ) != OGRERR_NONE )
    {
        printf( "Creating 'WEIGHT' field failed.\n" );
        exit( 1 );
    }

    int i;
    for( i = 0; i <  m_features.size(); i++ ) {

		if( m_features[i]->m_geoms.size() > 1 ) {
			OGRLineString* ls = m_features[i]->toLineString();
			OGRFeature* f = new OGRFeature( m_featureDefn );
			f->SetField( m_featureDefn->GetFieldIndex("WEIGHT"), m_features[i]->m_weight );	
			f->SetField( m_featureDefn->GetFieldIndex("ID"), i );
			f->SetGeometryDirectly( ls );
		 
			if( poLayer->CreateFeature( f ) != OGRERR_NONE )
	        {
	           printf( "Failed to create feature in shapefile.\n" );
	           exit( 1 );
	        }
		}    
    }

    OGRDataSource::DestroyDataSource( poDS );
}

std::string pm_trax::GeomBuilder::geometriesAsText()
{
    int i;
	std::string geomsAsText;
	for( i = 0; i <  m_features.size(); i++ ) {
		char* asText = new char[4096];
		OGRLineString* ls = m_features[i]->toLineString();
		ls->exportToWkt( &asText );
		geomsAsText += asText;
		geomsAsText += "\n";	
	}
	return geomsAsText;
}