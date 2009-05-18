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

#include <iostream>
#include <string>

#include "Config.h"

const double WRONG_ANGLE = -9999.9999;

pm_trax::Config::Config( const std::string& filename ) 
{
	m_branches.clear();
	m_axiomLength = -1.0;
	m_axiomHeading = WRONG_ANGLE;
	m_axiomPitch = WRONG_ANGLE;
	m_axiomWeight = -1.0;

	load( filename );
};

static pm_trax::Config* theConfig = NULL;

pm_trax::Config* pm_trax::Config::instance( const std::string& filename )
{
	if( NULL == theConfig && filename != "" ) { 
		theConfig = new pm_trax::Config;
	    theConfig->load( filename );
	}
	return theConfig;
}

std::string pm_trax::Config::readTagValue( const std::string& tag, TiXmlElement* parentElement )
{
	if( parentElement == NULL && m_configRoot != NULL ) {		
		TiXmlElement* element = m_configRoot->FirstChildElement( tag.c_str() );
		if( element != NULL ) {
			return element->GetText();
		}
	}
	else {
		TiXmlElement* element = parentElement->FirstChildElement( tag.c_str() );
		if( element != NULL ) {
			return element->GetText();
		}

	}
	return "";
}

std::string pm_trax::Config::readAttributeValue( const std::string& tag, const std::string& attrName )
{
	if( m_configRoot != NULL ) {		
		TiXmlElement* element = m_configRoot->FirstChildElement( tag.c_str() );
		if( element != NULL ) {
			return element->Attribute( attrName.c_str() );
		}
		else {
			element = m_configRoot->FirstChildElement();
			while( element != NULL ) {
				TiXmlElement *childElement = element->FirstChildElement( tag.c_str() );
				if( childElement != NULL )
					return childElement->Attribute( attrName.c_str() );
				element = m_configRoot->NextSiblingElement();
			}
		}
	}
	return "";
}	
	
void pm_trax::Config::convert( const std::string& str_int, int& val )
{
	val = atoi( str_int.c_str() );
}

void pm_trax::Config::convert( const std::string& str_double, double& val )
{
	val = atof( str_double.c_str() );
}

void pm_trax::Config::convert( const std::string& str_vec3d, Vec3d& val )
{
	std::string stringlist[3];
	std::string::size_type pos = str_vec3d.find( " ", 0 );
	stringlist[0] = str_vec3d.substr( 0, pos );
	std::string::size_type pos1 = str_vec3d.find( " ", pos+1 );
	stringlist[1] = str_vec3d.substr( pos, pos1-pos );
	std::string::size_type pos2 = str_vec3d.rfind( " ", str_vec3d.length() );
	stringlist[2] = str_vec3d.substr( pos2, str_vec3d.length()-pos2 );
	val = Vec3d( atof( stringlist[0].c_str() ), atof( stringlist[1].c_str() ), atof( stringlist[2].c_str() ) );
}

void pm_trax::Config::load( const std::string& filename )
{
  m_configDoc = new TiXmlDocument( filename.c_str() );
  m_configDoc->LoadFile();
  m_configRoot = m_configDoc->RootElement();
}

std::vector< TiXmlElement* > pm_trax::Config::elementsByTagName( const std::string& tag,  TiXmlElement* parentElement )
{
	std::vector< TiXmlElement* > elements;
	if( parentElement == NULL )
		parentElement = m_configRoot;
	TiXmlElement* childElement = NULL;
	for( childElement = parentElement->FirstChildElement( tag.c_str() ); childElement; childElement = childElement->NextSiblingElement( tag.c_str() ) ) {
		elements.push_back( childElement );
	}
	return elements;
}

TiXmlElement* pm_trax::Config::firstElementByTagName( const std::string& tag, TiXmlElement* parentElement )
{
	std::vector< TiXmlElement* > elements = elementsByTagName( tag, parentElement );
	if( elements.size() == 0 )
		return NULL;
	return elements[0];
}

std::string pm_trax::Config::notifyLevel() 
{
	std::vector< TiXmlElement* > elements = elementsByTagName( "notifylevel" ); 
	if( elements.size() == 1 )
		return elements[0]->GetText();
	return "";
}

std::string pm_trax::Config::output() 
{
	std::vector< TiXmlElement* > elements = elementsByTagName( "output" ); 
	if( elements.size() == 1 )
		return elements[0]->GetText();
	return "";
}

int pm_trax::Config::depth() 
{	
	int depth = 0;
	TiXmlElement* parentElement = elementsByTagName( "lsystem" )[0];
	std::vector< TiXmlElement* > childrenElements = elementsByTagName( "depth", parentElement ); 
	if( childrenElements.size() == 1 )
		convert( childrenElements[0]->GetText(), depth );
	return depth;
}

Vec3d pm_trax::Config::axiomPosition() 
{
	Vec3d position = Vec3d( 0, 0, 0 );
	TiXmlElement* parentElement = elementsByTagName( "lsystem" )[0];
	std::vector< TiXmlElement* > childrenElements = elementsByTagName( "axiom", parentElement ); 
	if( childrenElements.size() == 1 )
		convert( childrenElements[0]->Attribute( "position" ), position );
	return position;
}

double pm_trax::Config::axiomWeight() 
{
	if( m_axiomWeight <	0.0 ) {
		TiXmlElement* parentElement = elementsByTagName( "lsystem" )[0];
		std::vector< TiXmlElement* > childrenElements = elementsByTagName( "axiom", parentElement ); 
		if( childrenElements.size() == 1 )
			convert( childrenElements[0]->Attribute( "section" ), m_axiomWeight );
	}
	return m_axiomWeight;
}

double pm_trax::Config::axiomLength() 
{
	if( m_axiomLength < 0.0 ) {
		TiXmlElement* parentElement = elementsByTagName( "lsystem" )[0];
		std::vector< TiXmlElement* > childrenElements = elementsByTagName( "axiom", parentElement ); 
		if( childrenElements.size() == 1 )
			convert( childrenElements[0]->Attribute( "length" ), m_axiomLength );
	}
	return m_axiomLength;
}

double pm_trax::Config::axiomHeading() 
{
	if( m_axiomHeading == WRONG_ANGLE ) {
		TiXmlElement* parentElement = elementsByTagName( "lsystem" )[0];
		std::vector< TiXmlElement* > childrenElements = elementsByTagName( "axiom", parentElement ); 
		if( childrenElements.size() == 1 )
			convert( childrenElements[0]->Attribute( "heading" ), m_axiomHeading );
	}
	return m_axiomHeading;
}

double pm_trax::Config::axiomPitch() 
{
	if( m_axiomPitch == WRONG_ANGLE ) {
		TiXmlElement* parentElement = elementsByTagName( "lsystem" )[0];
		std::vector< TiXmlElement* > childrenElements = elementsByTagName( "axiom", parentElement ); 
		if( childrenElements.size() == 1 )
			convert( childrenElements[0]->Attribute( "pitch" ), m_axiomPitch );
	}
	return m_axiomPitch;
}

std::map< double, std::vector< double > > pm_trax::Config::branchProperties() 
{
	if( m_branches.empty() ) {
		TiXmlElement* parentElement = NULL;
		if( ( parentElement = firstElementByTagName( "lsystem" ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "branch", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
				double section = 0.0;
				convert( (*it)->Attribute( "section" ), section );
				std::vector< double > params;
				double length = 0.0;
				convert( (*it)->Attribute( "length" ), length );
				params.push_back( length );
				double proba = 0.0;
				convert( (*it)->Attribute( "triggerproba" ), proba );
				params.push_back( proba );
				m_branches.insert( std::make_pair< double, std::vector< double > >( section, params ) );
			}
		}
	}
	return m_branches;
}

std::map< double, double > pm_trax::Config::contextDensitySection2AngleToPeak() 
{
	std::map< double, double > section2AngleToPeak;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "densities", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "density", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
				double section = 0.0;
				convert( (*it)->Attribute( "section" ), section );
				double angle = 0.0;
				convert( (*it)->Attribute( "maxanglewithpeak" ), angle );
				section2AngleToPeak.insert( std::make_pair< double, double >( section, angle*Linear::DegToRad ) );
			}
		}
		}
	}
	return section2AngleToPeak;
}

std::map< double, double > pm_trax::Config::contextDensitySection2DistanceToPeak()
{
	std::map< double, double > sectionDistanceToPeak;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "densities", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "density", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
				double section = 0.0;
				convert( (*it)->Attribute( "section" ), section );
				double dist = 0.0;
				convert( (*it)->Attribute( "maxdistancetopeak" ), dist );
				sectionDistanceToPeak.insert( std::make_pair< double, double >( section, dist ) );
			}
		}
		}
	}
	return sectionDistanceToPeak;
}

std::map< double, double > pm_trax::Config::contextDensitySection2MinValueToTrigger()
{
	std::map< double, double > sectionMinDensityToTrigger;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "densities", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "density", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
				double section = 0.0;
				convert( ( *it )->Attribute( "section" ), section );
				double density = 0.0;
				convert( ( *it )->Attribute( "mindensitytotrigger" ), density );
				sectionMinDensityToTrigger.insert( std::make_pair< double, double >( section, density ) );
			}
		}
		}
	}
	return sectionMinDensityToTrigger;
}

std::map< double, std::string > pm_trax::Config::contextDensitySection2Type()
{
	std::map< double, std::string > section2Type;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "densities", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "density", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
					double section = 0.0;
					convert( ( *it )->Attribute( "section" ), section );
					std::string type = ( *it )->Attribute( "type" );
					section2Type.insert( std::make_pair< double, std::string >( section, type ) );
			}
		}
		}
	}
	return section2Type;
}

std::map< double, double > pm_trax::Config::contextDensitySection2Impact()
{
	std::map< double, double > section2Impact;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "densities", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "density", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
					double section = 0.0;
					convert( ( *it )->Attribute( "section" ), section );
					double impact = 0.0;
					convert( ( *it )->Attribute( "impact" ), impact );
					section2Impact.insert( std::make_pair< double, double >( section, impact ) );
			}
		}
		}
	}
	return section2Impact;
}

std::map< double, std::string > pm_trax::Config::contextReliefSection2Type()
{
	std::map< double, std::string > section2Type;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "reliefs", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "relief", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
					double section = 0.0;
					convert( ( *it )->Attribute( "section" ), section );
					std::string type = ( *it )->Attribute( "type" );
					section2Type.insert( std::make_pair< double, std::string >( section, type ) );
			}
		}
		}
	}
	return section2Type;
}

std::map< double, double > pm_trax::Config::contextReliefSection2Impact()
{
	std::map< double, double > section2Impact;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "reliefs", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "relief", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
					double section = 0.0;
					convert( ( *it )->Attribute( "section" ), section );
					double impact = 0.0;
					convert( ( *it )->Attribute( "impact" ), impact );
					section2Impact.insert( std::make_pair< double, double >( section, impact ) );
			}
		}
		}
	}
	return section2Impact;
}

std::map< double, std::string > pm_trax::Config::contextObstacleSection2Type()
{
	std::map< double, std::string > section2Type;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "local", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "obstacles", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "obstacle", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
				double section = 0.0;
				convert( ( *it )->Attribute( "section" ), section );
				std::string type = ( *it )->Attribute( "type" );
				section2Type.insert( std::make_pair< double, std::string >( section, type ) );
			}
		}
		}
	}
	return section2Type;
}

std::map< double, double > pm_trax::Config::contextObstacleSection2Dilatation()
{
	std::map< double, double > section2Dilatation;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "local", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "obstacles", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "obstacle", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
				double section = 0.0;
				convert( ( *it )->Attribute( "section" ), section );
				double dilatation = 0.0;
				convert( ( *it )->Attribute( "dilatedistance" ), dilatation );
				section2Dilatation.insert( std::make_pair< double, double >( section, dilatation ) );
			}
		}
		}
	}
	return section2Dilatation;
}

std::map< double, std::string > pm_trax::Config::contextIntersectionSection2Type()
{
	std::map< double, std::string > section2Type;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "local", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "intersections", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "intersection", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
					double section = 0.0;
					convert( ( *it )->Attribute( "section" ), section );
					std::string type = ( *it )->Attribute( "type" );
					section2Type.insert( std::make_pair< double, std::string >( section, type ) );
			}
		}
		}
	}
	return section2Type;
}

std::map< std::string, std::string > pm_trax::Config::globalContextMaps()
{
	std::map< std::string, std::string > maps;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
			if( ( parentElement = firstElementByTagName( "maps", parentElement ) ) != NULL ) {
				std::vector< TiXmlElement* > childrenElements = elementsByTagName( "map", parentElement ); 	
				std::vector< TiXmlElement* >::iterator it;
				for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
					std::string type = ( *it )->Attribute( "type" );
					std::string mapfile = ( *it )->GetText();
					maps.insert( std::make_pair< std::string, std::string >( type, mapfile ) );
				}
			}
		}
	}
	return maps;
}

std::map< std::string, std::string > pm_trax::Config::localContextMaps()
{
	std::map< std::string, std::string > maps;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "local", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "maps", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "map", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
				std::string type = ( *it )->Attribute( "type" );
				std::string mapfile = ( *it )->GetText();
				maps.insert( std::make_pair< std::string, std::string >( type, mapfile ) );
			}
		}
		}
	}
	return maps;
}

std::map< double, std::string > pm_trax::Config::contextPatternSection2Type()
{
	std::map< double, std::string  > section2Type;
	TiXmlElement* parentElement = NULL;
	if( ( parentElement = firstElementByTagName( "context" ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "global", parentElement ) ) != NULL ) {
		if( ( parentElement = firstElementByTagName( "patterns", parentElement ) ) != NULL ) {
			std::vector< TiXmlElement* > childrenElements = elementsByTagName( "pattern", parentElement ); 	
			std::vector< TiXmlElement* >::iterator it;
			for( it = childrenElements.begin(); it != childrenElements.end(); it++ ) {
				std::string type = ( *it )->Attribute( "type" );
				double section = 0.0;
				convert( ( *it )->Attribute( "section" ), section );
				section2Type.insert( std::make_pair< double, std::string >( section, type ) );
			}
		}
		}
	}
	return section2Type;
}
