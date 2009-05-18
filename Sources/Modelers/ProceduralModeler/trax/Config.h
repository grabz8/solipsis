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

#ifndef CONFIG_H
#define CONFIG_H

#include <map>

#include <tinyxml.h>

#include "Linear/Linear.h"

namespace pm_trax {

class Config
{
public:

	Config( const std::string& filename = std::string( "ls.xml" ) );
  
	static Config* instance( const std::string& filename = std::string( "ls.xml" ) );
	
	void load( const std::string& filename );
	
	std::string notifyLevel();
	std::string output();

	// data access methods
	int depth();
	Vec3d axiomPosition();
	double axiomWeight();
	double axiomLength();
	double axiomHeading();
	double axiomPitch();
	
	std::map< double, std::vector< double > > branchProperties();

	std::map< double, std::string > contextPatternSection2Type();

	std::map< double, double > contextDensitySection2AngleToPeak();
	std::map< double, double > contextDensitySection2DistanceToPeak();
	std::map< double, double > contextDensitySection2MinValueToTrigger();
	std::map< double, std::string >	contextDensitySection2Type();
	std::map< double, double >	contextDensitySection2Impact();

	std::map< double, std::string >	contextObstacleSection2Type();
	std::map< double, double > contextObstacleSection2Dilatation();

	std::map< double, std::string >	contextReliefSection2Type();
	std::map< double, double >	contextReliefSection2Impact();

	std::map< double, std::string >	contextIntersectionSection2Type();

	std::map< std::string, std::string > globalContextMaps();
	std::map< std::string, std::string > localContextMaps();
	
protected:

	// general purpose methods
	std::string readTagValue( const std::string& tag, TiXmlElement* parentElement = NULL );

	std::string readAttributeValue( const std::string& tag, const std::string& attr );
	
	std::vector< TiXmlElement* > elementsByTagName( const std::string& tag,  TiXmlElement* parent = NULL );
	TiXmlElement* firstElementByTagName( const std::string& tag,  TiXmlElement* parent = NULL );

	void convert( const std::string& str_val, int& int_val );
	void convert( const std::string& str_val, float& flt_val );
	void convert( const std::string& str_val, double& dbl_val );
	void convert( const std::string& str_val, Vec3d& v3d_val );
	
	// members
	TiXmlDocument* m_configDoc;
	TiXmlElement*  m_configRoot;

	std::map< double, std::vector< double > > m_branches;
	double m_axiomLength;
	double m_axiomHeading;
	double m_axiomPitch;
	double m_axiomWeight;

	
}; // Config

}; // namespace pm_trax

#endif // CONFIG_H
