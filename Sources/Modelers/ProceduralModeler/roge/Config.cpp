/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Archivideo / MERLET Jonathan

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

using namespace std;

#include "Config.h"
#include "Extruder/texInfo.h"

namespace roge {
	
	Config::Config( const std::string& filename ) 
	{
		load( filename );
	};

	static Config* theConf = NULL;

	Config* Config::instance( const std::string& filename )
	{
		if( NULL == theConf && filename != "" ) { 
			theConf = new Config( filename );
		}
		return theConf;
	}

	std::string Config::readTagValue( const std::string& tag, TiXmlElement* parentElement )
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

	std::string Config::readAttributeValue( const std::string& tag, const std::string& attrName )
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
	
	void Config::convert( const std::string& str_int, int& val )
	{
		val = atoi( str_int.c_str() );
	}
	
	void Config::convert( const std::string& str_float, float& val )
	{
		val = atof( str_float.c_str() );
	}

	void Config::convert( const std::string& str_double, double& val )
	{
		val = atof( str_double.c_str() );
	}

	void Config::convert( const std::string& str_vec3d, Vec3d& val )
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

	void Config::load( const std::string& filename )
	{
		std::cout << "Loading file: " << filename << std::endl;
		m_configDoc = new TiXmlDocument( filename.c_str() );
		m_configDoc->LoadFile();
		m_configRoot = m_configDoc->RootElement();
	}

	std::vector< TiXmlElement* > Config::elementsByTagName( const std::string& tag,  TiXmlElement* parentElement )
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

	TiXmlElement* Config::firstElementByTagName( const std::string& tag, TiXmlElement* parentElement )
	{
		std::vector< TiXmlElement* > elements = elementsByTagName( tag, parentElement );
		if( elements.size() == 0 )
			return NULL;
		return elements[0];
	}
	
	Config::~Config() 
	{}
			
	int Config::getSourceFileNum() 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "source" );
		return( list.size() );
	}
	
	Config::DataSource getDataFromString(const std::string& strSource) {
		if( strSource == std::string("ign_bdtopo_pays") )
			return Config::UNDEFINED;
		else if( strSource == std::string("ign_bdtopo") )
			return Config::UNDEFINED;
		else if( strSource == std::string("gdt_dynamap") )
			return Config::UNDEFINED;
		return Config::UNDEFINED;
	}
	
	bool Config::getSourceFileName( std::string& sourceFilename, Config::DataSource& data, int index ) 
	{
		// gets a specific filename under the source tag
		std::vector< TiXmlElement* > list = elementsByTagName( "source" );
		TiXmlElement* element = NULL;
	
		if( index < list.size() ) {
			//gets the file name
			element = list[index];
			sourceFilename = element->Attribute( "name" );
			data = getDataFromString( "" );
		} 
		else {
			return false;
		}
		return true;
	}
	
	bool Config::getTexture( std::string& texName, double& textureLength, const std::string& type ) 
	{
		//gets a specific filename under the texture tag
		std::vector< TiXmlElement* > list = elementsByTagName( "texture" );
		TiXmlElement* element = NULL;
		if( list.size() != NB_TEXTURES ) {
			return false;
		}
		element = list[0];
		int i = 0;	
		while( type != std::string ( element->Attribute( "type" ) ) ) {
			//find the texture we are searching by its type
			i++;
			element = list[i];
		}
		if( i == list.size() ) {
			return false;
		}
		//get the informations
		texName = element->Attribute( "name" );
		convert( element->Attribute( "length" ), textureLength );
		return true;
	}
	
	bool Config::getTranslation( Vec3d& translation ) 
	{
		//gets the translation specified in the file
		std::vector< TiXmlElement* > list = elementsByTagName( "translation" );
		if( list.size() != 1 ){
			translation = Vec3d( 0, 0, 0 );
			return false;
		}
		convert( list[0]->GetText(), translation );
		return true;
	}
	
	bool Config::getSmoothness(double & smoothness) 
	{
		//gets the smoothness specified in the file
		std::vector< TiXmlElement* > list = elementsByTagName( "smoothness" );
		if( list.size() != 1 ) {
			return false;
		}
		//get the value
		convert( list[0]->GetText(), smoothness );
		return true;
	}
	
	bool Config::getOutputContourFile( std::string& contoursFile ) 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "outfiles" );
		if( list.size() != 1 ) {
			return false;
		}
		//gets the node corresponding to the contours file 
		contoursFile = list[0]->Attribute( "outlines" );
		return true;
	}
	
	bool Config::getOutputBridgesFile( std::string& bridgesFile ) 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "outfiles" );
		if( list.size() != 1 ) {
			return false;
		}
		//gets the node corresponding to the bridges file
		bridgesFile = list[0]->Attribute( "bridges" );
		return true;
	}
	
	bool Config::getOutputRoadsFile( std::string& roadsFile ) 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "outfiles" );
		if( list.size() != 1 ) {
			return false;
		}
		roadsFile = list[0]->Attribute( "roads" );
		return true;
	}
	
	bool Config::getOutputDXFFile( std::string& dxfFile ) 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "outfiles" );
		if( list.size() != 1 ) {
			return false;
		}
		dxfFile = list[0]->Attribute( "dxf" );
		return true;
	}
	
	bool Config::getMaxRoadsInContour( int& maxroads ) 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "maxRoadsInContour" );
		if( list.size() != 1 ) {
			return false;
		}
		convert( list[0]->Attribute( "value" ), maxroads );
		return true;
	}
	
	bool Config::getFlags( bool& extrude, bool& intersections, bool& bridges,	bool& contours ) 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "flags" );
		if( list.size() != 1 ) {
			return false;
		}
		extrude = std::string( list[0]->Attribute( "extrude" ) ) == std::string( "true" );
		intersections = std::string( list[0]->Attribute( "intersections" ) ) == std::string( "true" );
		bridges = std::string( list[0]->Attribute( "bridges" ) ) == std::string( "true" );
		contours = std::string( list[0]->Attribute( "contours") ) == std::string( "true");
		return true;
	}
	
	double Config::getWidth( const std::string& type ) 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "roadwidth" );
		if( list.size() != NB_TEXTURES+1 ) { // +1 for the default value.....
			return -1;
		}
		TiXmlElement* element = NULL;
		int i=0;
		while( type != std::string( list[i]->Attribute( "type" ) ) && i < list.size() ) {
			i++;
			element = list[i];
		}
		if( i == list.size() ) {
			return false;
		}
		double width = 0.0;
		convert( element->Attribute( "width" ), width );
		return width;
	
	}
	
	double Config::getEmbankmentWidth() {
		std::vector< TiXmlElement* > list = elementsByTagName( "embankment" );
		if( list.size() != 1 ) {
			return -1;
		}	
		double embankment = 0.0;
		convert( list[0]->Attribute( "width" ), embankment );
		return embankment;
	}
	
	double Config::getRoadThickness() 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "roadThickness" );
		if( list.size() != 1 ) {
			return -1;
		}	
		double thickness = 0.0;
		convert( list[0]->Attribute( "value" ), thickness );
		return thickness;
	}
	
	int Config::getSegmentLimit() 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "limitations" );
		if( list.size() != 1 ) {
			return -1;
		}	
		int limit = 0;
		convert( list[0]->Attribute( "segmentIndex" ), limit );
		return limit;
	}
	
	std::string Config::getDtmFilename() 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "dtm" );
		if( list.size() != 1 )
			return "";
		return list[0]->GetText();
	}
	
	bool Config::getTextureInfo( std::map< std::pair< std::string, int >, texInfo* >& texInfos )
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "texture" );
		TiXmlElement* element = NULL;
		unsigned int i;
		for( i = 0 ; i < list.size() ; ++i ) {
			element = list[i];
			texInfo* tmp = new texInfo();
			convert( element->Attribute( "length" ), tmp->length );
			convert( element->Attribute( "width" ), tmp->width );
			convert( element->Attribute( "numOfWays" ), tmp->nbOfWays );
			tmp->filename.assign( element->Attribute( "name" ) );
			tmp->nature.assign( element->Attribute( "nature" ) );
			texInfos.insert( make_pair( make_pair( tmp->nature, tmp->nbOfWays ), tmp ) );
		}
		return true;
	}
	
	std::vector< std::string > Config::getTexturesNames() 
	{
		std::vector< std::string > names;
		std::vector< TiXmlElement* > list = elementsByTagName( "texture" );
		TiXmlElement* element = NULL;	
		unsigned int i;
		for( i = 0 ; i < list.size() ; ++i ) {
			element = list[i];
			names.push_back( element->Attribute( "name" ) );
		}
		return names;
	}

	bool Config::getIntersectionCenterTexture( std::string& texName, double& textureLength ) 
	{
		std::vector< TiXmlElement* > list = elementsByTagName( "textureIntersectionCenter" );
		bool textureFound = false;
		if( list.size() == 1 ) {
			TiXmlElement *element = list[0];
			texName = element->Attribute( "name" );
			convert( element->Attribute( "length" ), textureLength );
			textureFound = true;
		}
		return textureFound;
	}

} //namespace roge
