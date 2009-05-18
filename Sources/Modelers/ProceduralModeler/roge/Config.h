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

#ifndef _Config_h_
#define _Config_h_

#include <vector>
#include <map>

#include <tinyxml.h>
	
#include "Linear/Linear.h"

class texInfo;

namespace roge {
	
	#define NB_TEXTURES 5
	
	/*! \brief Configuration file handler for genRoute
	 */
	
	class Config {
	
	public:

		typedef enum DataSource {
			UNDEFINED = 0x0
		};
	
		Config( const std::string& filename = std::string( "roge.xml" ) );
  
		static Config* instance( const std::string& filename = "" );
	
		void load( const std::string& filename );
	
		virtual ~Config();
	
		/*! gets source file number
		 */
		int getSourceFileNum();
	
		/*! gets source file name
		 \param sourceFilleName source file name
		 \param index index of the source file
		 \return true if nothing bad happened, else return false
		 */
		bool getSourceFileName( std::string& sourceFilename, roge::Config::DataSource &data, int index);
	
		/*! gets value of smoothness
		 \param smoothness the smoothness defined in the configuration file
		 \return true if nothing bad happened, else return false
		 */
		bool getSmoothness(double & smoothness);
	
		/*! gets output file for contours
		 \param contourFile name of the file in which genRoute will write the contours
		 \return true if nothing bad happened, else return false
		 */
		bool getOutputContourFile( std::string& contourFile);
	
		/*! gets output file for bridges
		 \param bridgesFile name of the file in which genRoute will write bridges
		 \return true if nothing bad happened, else return false
		 */
		bool getOutputBridgesFile( std::string& bridgesFile);
	
		/*! gets output file for roads
		 \param roadsFile name of the file in which genRoute will write roads
		 \return true if nothing bad happened, else return false
		 */
		bool getOutputRoadsFile( std::string& roadsFile);
	
		/*! gets output file for contour dxf
		 \param roadsFile name of the file in which genRoute will write contours dxf
		 \return true if nothing bad happened, else return false
		 */
		bool getOutputDXFFile( std::string& dxfFile);
	
		/*! gets textures names
		 \param texFileName name of a texture file
		 \param textureLength texture length
		 \param type type of the texture (equivalent to type of the road)
		 \return true if nothing bad happened, else return false
		 */
		bool getTexture( std::string& texFileName, double & textureLength, const std::string& type);
	
		/*! gets translation that will be applied to source data
		 \param translation translation vector
		 \return true if nothing bad happened, else return false
		 */
		bool getTranslation(Vec3<double>& translation);
	
		/*! gets max number of roads in a contour
		 \param max max number
		 \return true if nothing bad happened, else return false
		 */
		bool getMaxRoadsInContour(int& max);
	
		/*! gets flags that determine what genRoute will compute
		 \param extrude extrusion flag
		 \param intersections intersection computing flag
		 \param bridges bridges computing flag
		 \param contours contours computing flag
		 \return true if nothing bad happened, else return false
		 */
		bool getFlags(bool& extrude, bool& intersections, bool& bridges,
				bool& contours);
	
		/*! gets road width
		 \param type type of the road (path, road, 4 way road, etc...)
		 \return width of the type of road
		 */
		double getWidth(const std::string& type);
	
		/*! gets embankment width
		 \return embankment width of -1 if an error occured
		 */
		double getEmbankmentWidth();
	
		/*! gets road thickness
		 \return road thickness
		 */
		double getRoadThickness();
	
		/*! gets segment limit: index of the highest segment on which the program will search for intersection, when it computes intersections geometry. In some case, a smaller number could avoids problems.
		 */
		int getSegmentLimit();
	
		/*! gets the filename of the dtm to project the contours on
		 */
		std::string getDtmFilename();
	
		/*! gets textures names
		 \param texFileName name of a texture file
		 \param textureLength texture length
		 \param textureWidth texture width
		 \param type type of the texture (equivalent to type of the road)
		 \return true if nothing bad happened, else return false
		 */
		bool getTextureInfo(
				std::map<std::pair<std::string, int>, texInfo*>& texInfos);
		
		std::vector<std::string> getTexturesNames();
	
		Config& operator=(const Config& right);
		  
		bool getIntersectionCenterTexture(std::string& texName, double& textureLength);
	
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
	
	};

} //namespace arSolipsisRoadGenerator

#endif
