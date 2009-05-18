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

#include <ctime>
#include <iostream>
#include <map>

#if OGRE_PLATFORM != OGRE_PLATFORM_WIN32 
#include <GetPot>
#endif

#include <Ogre.h>

#include "BSpline/BSpline.h"
#include "Extruder/Extrusion.h"

#include "trax/Config.h"
#include "trax/GeomBuilder.h"
#include "trax/LSystem.h"

#include "utils/Utils.h"

#include "roge/RoadNetwork.h"
#include "roge/ComputeFactory.h"

using namespace Ogre;

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#define WIN32_LEAN_AND_MEAN 
#include "windows.h" 
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT ) {
#else 
int main( int argc, char** argv ) 
{	
	GetPot cl(argc, argv);

	if( argc < 2 || cl.search("-h" ) ) {
		pm_utils::notify( pm_utils::notifyLevel() ) << "Usage : " << argv[0] << " <XML config file>" << std::endl;
	    exit(0);
	}

	std::vector<std::string> files = commandLine.nominus_vector();
#endif 

	//////////////////////////////////////////////
	// GENERATION OF TRANSPORT AXES FROM A LSYSTEM
	//////////////////////////////////////////////

	pm_utils::redirectOutputToFile( "LOG.txt" );

    std::string traxfilename( "C:/src/workspace/Generation_automatique_de_contenu_3D/Modelisation_procedurale/Axes_de_transport/roge/trax/Release/ls.xml" );
	// -- build the instance for the TRAX XML Config manager --
	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> TRAX INITIALIZING FROM TRAX CONFIG FILE: " <<  traxfilename << std::endl;
    pm_trax::Config::instance( traxfilename );

		// - set the notification level from Config -
	pm_utils::setNotifyLevel( pm_trax::Config::instance()->notifyLevel() );

	// -- build the instance for the GeomBuilder --
	pm_trax::GeomBuilder::instance();

	// -- create the LSystem and initialize it -- 
	pm_trax::LSystem lsystem;	
	lsystem.init();	
	
	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> TRAX ITERATING..." << std::flush; 
	pm_utils::notify( pm_utils::PM_DEBUG ) << "main -> TRAX LSYSTEM AXIOM: " << lsystem.asString( lsystem.axiom() ) << std::endl; 
	// -- make the LSystem iterate at least 'depth' times --
	lsystem.iterate( pm_trax::Config::instance()->depth() );
	pm_utils::notify( pm_utils::notifyLevel() ) << " DONE." << std::endl; 
	pm_utils::notify( pm_utils::PM_DEBUG ) << "main -> TRAX LSYSTEM STATE #" << lsystem.states().size()-1 << ": " << lsystem.asString( lsystem.state( lsystem.states().size()-1 ) ) << std::endl;

	//// - write geometries as an OGR datasource -
	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> WRITING TRANSPORT AXES TO DATASOURCE: " <<  pm_trax::Config::instance()->output() << std::endl;
	pm_trax::GeomBuilder::instance()->write( pm_trax::Config::instance()->output() );
	
	///////////////////////////////////////////////
	std::vector< pm_trax::GeomBuilder::Feature* > features = pm_trax::GeomBuilder::instance()->features();

	std::string rogefilename( "C:/src/workspace/Generation_automatique_de_contenu_3D/Modelisation_procedurale/Axes_de_transport/roge/roge/Release/rgConfig.xml" );
	// -- build the instance for the ROGE XML Config manager --
	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> INITIALIZING FROM ROGE CONFIG FILE: " <<  rogefilename << std::endl;	
	roge::Config::instance( rogefilename );

	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> ROGE Creating the roads network..." << std::endl;
	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> ROGE Creating roadways..." << std::flush;
	roge::RoadNetwork network( features );

	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> ROGE Creating roads intersections..." << std::flush;
	network.createIntersections();

	if (!network.verifyNetwork()) {
		pm_utils::notify( pm_utils::PM_ERROR ) << "main -> ROGE Error in network" << std::endl;
		return 1;
	}

	// - get flags -
	bool extrudeFlag, interFlag, bridgeFlag, contourFlag;
	if (!roge::Config::instance()->getFlags( extrudeFlag, interFlag, bridgeFlag, contourFlag ) ) {
		pm_utils::notify( pm_utils::PM_ERROR ) << "main -> ROGE Error in ROGE config file" << std::endl;
		return 1;
	}

	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> ROGE Computing the geometry of the roads network..." << std::endl;	
	// - create the factory for geometry computing -
	roge::ComputeFactory factory;
	
	// - create the extruder -
	Extrusion extruder;
	extruder.setMaterialsPerSubmeshesFlag( false );
	
	//Prepare a mesh pointer
	MeshPtr roadNetworkMesh;
	if( extrudeFlag ) {
		pm_utils::notify( pm_utils::notifyLevel() ) << "main -> ROGE Computing the geometry of roadways..." << std::flush;
		roadNetworkMesh = factory.extrude( network, extruder );
		pm_utils::notify( pm_utils::notifyLevel() ) << std::endl;
	}

	// intersections & contours
	MeshPtr intersections;
	std::vector< OGRLineString* > contours;
	if (extrudeFlag && interFlag) {
		pm_utils::notify( pm_utils::notifyLevel() ) << "main -> ROGE Computing the geometry of intersections..." << std::flush;
		intersections = factory.createIntersections( roadNetworkMesh, network, contourFlag, contours );
		pm_utils::notify( pm_utils::notifyLevel() ) << std::endl;
		ostringstream ossInter, ossRoad;
		ossInter << intersections->getName() << ".mesh";
		MeshSerializer serializer;
		serializer.exportMesh(intersections.get(), ossInter.str().c_str());
		ossRoad << roadNetworkMesh->getName() << ".mesh";
		serializer.exportMesh(roadNetworkMesh.get(), ossRoad.str().c_str());
		
	}

	return 0;

}

#ifdef __cplusplus
}
#endif

